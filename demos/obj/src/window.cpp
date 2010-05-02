#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <fenv.h>
#include "libeblearn.h"
#include "libeblearntools.h"
#include "objrec.h"
#include "thread.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

////////////////////////////////////////////////////////////////
// vision thread

typedef float t_net; // network precision

class vision_thread : public thread {
public:
  vision_thread(configuration &conf, const char *arg2);
  ~vision_thread();

  virtual void execute();
  //! Return true if new data was copied, false otherwise.
  virtual bool get_data(idx<t_net> &frame, vector<bbox*> &bboxes);

private:
  //! Copy passed bounding boxes into bboxes class member
  //! (allocating new 'bbox' objects).
  void copy_bboxes(vector<bbox*> &bb);
  //! Turn 'updated' flag on, so that other threads know we just added new data.
  void set_updated();

  ////////////////////////////////////////////////////////////////
  // private members
private:
  configuration &conf;
  const char *arg2;
  pthread_mutex_t mutex1;
  idx<t_net> frame;
  vector<bbox*> bboxes;
  vector<bbox*>::iterator ibox;
  bool updated;
};

vision_thread::vision_thread(configuration &conf_, const char *arg2_)
  : conf(conf_), arg2(arg2_), mutex1(), updated(false) {
}

vision_thread::~vision_thread() {
}

void vision_thread::copy_bboxes(vector<bbox*> &bb) {
  // lock data
  pthread_mutex_lock(&mutex1);
  // clear bboxes
  bboxes.clear();
  // copy bboxes
  for (ibox = bb.begin(); ibox != bb.end(); ++ibox) {
    bboxes.push_back(new bbox(**ibox));
  }
  // unlock data
  pthread_mutex_unlock(&mutex1);
}

void vision_thread::set_updated() {
  // lock data
  pthread_mutex_lock(&mutex1);
  // set flag
  updated = true;
  // unlock data
  pthread_mutex_unlock(&mutex1);
}

bool vision_thread::get_data(idx<t_net> &frame2, vector<bbox*> &bboxes2) {
  // lock data
  pthread_mutex_lock(&mutex1);
  // only read data if it has been updated
  if (!updated) {
    // unlock data
    pthread_mutex_unlock(&mutex1);
    return false;
  }
  // check frame is correctly allocated, if not, allocate.
  if (frame2.order() != frame.order()) 
    frame2 = idx<t_net>(frame.get_idxdim());
  else if (frame2.get_idxdim() != frame.get_idxdim())
    frame2.resize(frame.get_idxdim());
  // copy frame
  idx_copy(frame, frame2);
  // clear bboxes
  for (ibox = bboxes2.begin(); ibox != bboxes2.end(); ++ibox) {
    if (*ibox)
      delete *ibox;
  }
  bboxes2.clear();
  // copy bboxes pointers (now responsible for deleting them).
  for (ibox = bboxes.begin(); ibox != bboxes.end(); ++ibox) {
    bboxes2.push_back(*ibox);
  }
  // reset updated flag
  updated = false; 
  // unlock data
  pthread_mutex_unlock(&mutex1);
  // confirm that we copied data.
  return true;
}

void vision_thread::execute() {
  bool		color		= conf.exists_bool("color");
  uint		norm_size	= conf.get_uint("normalization_size");
  t_net		threshold	= (t_net) conf.get_double("threshold");
  bool		display 	= false;
  bool		mindisplay 	= false;
  uint          display_sleep   = 0;
  bool		save_video 	= false;
  string        cam_type        = conf.get_string("camera");
  int           height          = conf.get_int("input_height");
  int           width           = conf.get_int("input_width");

  // load network and weights
  parameter<t_net> theparam;
  idx<ubyte> classes(1,1);
  try {
    load_matrix<ubyte>(classes, conf.get_cstring("classes"));
  } catch(string &err) { cerr << "warning: " << err << endl; }
  cout << "loading weights from " << conf.get_cstring("weights") << endl;
  module_1_1<t_net> *net = init_network(theparam, conf, classes.dim(0));
  try {
    theparam.load_x<double>(conf.get_cstring("weights"));
  } catch(string &err) { eblerror(err.c_str()); }

  // select preprocessing  
  module_1_1<t_net>* pp = NULL;
  if (!strcmp(cam_type.c_str(), "v4l2")) // Y -> Yp
    pp = new weighted_std_module<t_net>(norm_size, norm_size, 1, true,
					false, true);
  else if (color) // RGB -> YpUV
    pp = (module_1_1<t_net>*) new rgb_to_ypuv_module<t_net>(norm_size);
  else // RGB -> Yp
    pp = (module_1_1<t_net>*) new rgb_to_yp_module<t_net>(norm_size);
    
  // detector
  detector<t_net> detect(*net, classes, pp, norm_size, NULL, 0,
			 conf.get_double("gain"));
  detect.set_resolutions(conf.get_double("scaling"));
  bool bmask_class = false;
  if (conf.exists("mask_class"))
    bmask_class = detect.set_mask_class(conf.get_cstring("mask_class"));
  if (conf.exists("input_min"))
    detect.set_min_resolution(conf.get_uint("input_min")); // limit inputs size
  if (conf.exists("input_max"))
    detect.set_max_resolution(conf.get_uint("input_max")); // limit inputs size
  detect.set_silent();
  if (conf.exists_bool("save_detections")) {
    detect.set_save("detections");
    if (conf.exists("save_max_per_frame"))
      detect.set_save_max_per_frame(conf.get_uint("save_max_per_frame"));
  }

  // initialize camera (opencv, directory, shmem or video)
  camera<t_net> *cam = NULL, *cam2 = NULL;
  if (conf.exists_bool("retrain") && conf.exists("retrain_dir")) {
    // extract false positives
    cam = new camera_directory<t_net>(conf.get_cstring("retrain_dir"));
  } else { // regular execution
    if (!strcmp(cam_type.c_str(), "directory")) {
      if (arg2) cam = new camera_directory<t_net>(arg2, height, width);
      else eblerror("expected 2nd argument");
    } else if (!strcmp(cam_type.c_str(), "opencv"))
      cam = new camera_opencv<t_net>(-1, height, width);
    else if (!strcmp(cam_type.c_str(), "v4l2"))
      cam = new camera_v4l2<t_net>(conf.get_cstring("device"), height, width);
    else if (!strcmp(cam_type.c_str(), "shmem"))
      cam = new camera_shmem<t_net>("shared-mem", height, width);
    else if (!strcmp(cam_type.c_str(), "video")) {
      if (arg2)
	cam = new camera_video<t_net>
	  (arg2, height, width, conf.get_uint("input_video_sstep"),
	   conf.get_uint("input_video_max_duration"));
      else eblerror("expected 2nd argument");
    } else eblerror("unknown camera type");
    // a camera directory may be used first, then switching to regular camera
    if (conf.exists_bool("precamera"))
      cam2 = new camera_directory<t_net>(conf.get_cstring("precamdir"),
					 height, width);
  }
    
  // gui
#ifdef __GUI__
  display 	= conf.exists_bool("display");
  mindisplay 	= conf.exists_bool("minimal_display");
  display_sleep	= conf.get_uint("display_sleep");
  save_video    = conf.exists_bool("save_video");
  uint qstep1 = 0, qheight1 = 0, qwidth1 = 0,
    qheight2 = 0, qwidth2 = 0, qstep2 = 0;
  if (conf.exists_bool("queue1")) { qstep1 = conf.get_uint("qstep1");
    qheight1 = conf.get_uint("qheight1"); qwidth1 = conf.get_uint("qwidth1"); }
  if (conf.exists_bool("queue2")) { qstep2 = conf.get_uint("qstep2");
    qheight2 = conf.get_uint("qheight2"); qwidth2 = conf.get_uint("qwidth2"); }
  module_1_1_gui netgui;
  uint	wid	= display ? new_window("eblearn object recognition") : 0;
  float zoom	= 1;
  detector_gui<t_net> dgui(conf.exists_bool("queue1"), qstep1, qheight1,
			   qwidth1, conf.exists_bool("queue2"), qstep2,
			   qheight2, qwidth2);
  if (bmask_class)
    dgui.set_mask_class(conf.get_cstring("mask_class"),
			(t_net) conf.get_double("mask_threshold"));
  night_mode();
  if (save_video)
    cam->start_recording();
  // timing variables
  QTime t0;
  int tpp;
#endif  
  // loop
  while(!cam->empty()) {
    try{
#ifdef __GUI__
    t0.start();
    // disable_window_updates();
    // clear_window();
#endif
    // if the pre-camera is defined use it until empty
    if (cam2 && !cam2->empty())
      frame = cam2->grab();
    else // empty pre-camera, use regular camera
      frame = cam->grab();
    // run detector
    if (!display) { // fprop without display
      vector<bbox*> &bb = detect.fprop(frame, threshold);
      copy_bboxes(bb); // make a copy of bounding boxes
    } 
#ifdef __GUI__
    else { // fprop and display
      if (mindisplay) {
	vector<bbox*> &bb = dgui.display(detect, frame, threshold, 0, 0, zoom,
					 (t_net)0, (t_net)255, wid);
	copy_bboxes(bb); // make a copy of bounding boxes
      }
      else
	dgui.display_inputs_outputs(detect, frame, threshold, 0, 0, zoom,
				    (t_net)-1.1, (t_net)1.1, wid); 
      if (save_video)
	cam->record_frame();
    }
    // switch 'updated' flag on to warn we just added new data
    set_updated();
    tpp = t0.elapsed(); // stop processing timer
    cout << "processing: " << tpp << " ms.";
    cout << " fps: " << cam->fps() << endl;
#endif
    if (display_sleep > 0) {
      cout << "sleeping for " << display_sleep << "ms." << endl;
      usleep(display_sleep);
    }
    if (conf.exists("save_max") && 
	detect.get_total_saved() > conf.get_uint("save_max"))
      break ; // limit number of detection saves
    } catch (string &err) { cerr << err << endl; }
  }
  if (save_video)
    cam->stop_recording(conf.exists_bool("use_original_fps") ?
			cam->fps() : conf.get_uint("save_video_fps"));
  // free variables
  if (net) delete net;
  if (cam) delete cam;
  if (pp) delete pp;
}


////////////////////////////////////////////////////////////////
// main thread

void draw(bbox *b, rect &pos, idx<ubyte> &bgwin, idx<t_net> &frame,
	  configuration &conf) {
  uint control_offset = conf.get_uint("control_offset");
  uint text_offset = conf.get_uint("text_offset");
  uint text_height = conf.get_uint("text_height");
  disable_window_updates();
  clear_window();
  set_font_size(conf.get_int("font_size"));
  gui.draw_matrix(bgwin);
  uint hface = bgwin.dim(0) - frame.dim(0) - control_offset;
  uint wface = bgwin.dim(1) / 2 - frame.dim(1) / 2;
  gui << at(hface - text_height * 3, wface - text_offset)
      << "Imagine this is a window to outside world.";
  gui << at(hface - text_height * 2, wface - text_offset) <<
    "Move your head down to see the sky, up to see the ground,";
  gui << at(hface - text_height, wface - text_offset) <<
    "left to see right and right to see left.";
  if (b) {
    draw_box(hface + pos.h0, wface + pos.w0, pos.width, pos.height, 0, 0, 255);
  }
  draw_matrix(frame, hface, wface);
  enable_window_updates();
}

#ifdef __GUI__

void estimate_position(rect &srcpos, rect &pos, rect &tgtpos, idx<t_net> &frame,
		       float &h, float &w, configuration &conf,
		       float tgt_time_distance) {
  tgt_time_distance = tgt_time_distance * conf.get_float("smooth_factor");
  // update current position
  pos.h0 = (uint) MAX(0, srcpos.h0 + (tgtpos.h0 - (float) srcpos.h0)
		      * MIN(1.0, tgt_time_distance));
  pos.w0 = (uint) MAX(0, srcpos.w0 + (tgtpos.w0 - (float) srcpos.w0)
		      * MIN(1.0, tgt_time_distance));
  pos.height = (uint) MAX(0, srcpos.height +
			  (tgtpos.height - (float) srcpos.height)
			  * MIN(1.0, tgt_time_distance));
  pos.width = (uint) MAX(0, srcpos.width +
			 (tgtpos.width - (float) srcpos.width)
			 * MIN(1.0, tgt_time_distance));
  // transform position into screen position
  // cout << "cur pos: " << pos << " src: " << srcpos
  //      << " target: " << tgtpos << endl;
  h = (((pos.h0 + pos.height / 2.0) / frame.dim(0))
       - conf.get_float("hoffset")) * conf.get_float("hfactor");
  w = (((pos.w0 + pos.width / 2.0) / frame.dim(1))
       - conf.get_float("woffset")) * conf.get_float("wfactor");
  //    cout << " h: " << h << " w: " << w << endl;
}

void change_background(vector<string>::iterator &bgi, vector<string> &bgs,
		       idx<ubyte> &bg, configuration &conf) {
  bgi++;
  if (bgi == bgs.end()) {
    random_shuffle(bgs.begin(), bgs.end());
    bgi = bgs.begin();
  }
  bg = load_image<ubyte>(*bgi);
  bg = image_resize(bg, conf.get_uint("winszhmax"),
		    conf.get_uint("winszhmax") * 3, 0);
  cout << "Changed background to " << *bgi << " (" << bg << ")." << endl;
}

#endif

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
  // check input parameters
  if ((argc != 2) && (argc != 3) ) {
    cerr << "wrong number of parameters." << endl;
    cerr << "usage: obj_detect <config file> [directory or file]" << endl;
    return -1;
  }
#ifndef __MAC__
  feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
  ipp_init(1); // limit IPP (if available) to 1 core
  // load configuration
#ifdef __GUI__
  configuration conf(argv[1]);
  intg winszh = conf.get_int("winszh");
  intg winszw = conf.get_int("winszw");
  // TODO: read PAM format for alpha channel
  // idx<ubyte> window = load_image<ubyte>("/home/sermanet/eblearn/pvc_window.png");
  // cout << "window: " << window << endl;
  list<string> *lbgs = find_fullfiles(conf.get_string("bgdir"));
  if (!lbgs) eblerror("background files not found");
  vector<string> bgs;
  list_to_vector(*lbgs, bgs);
  srand(time(NULL));
  random_shuffle(bgs.begin(), bgs.end());
  vector<string>::iterator bgi = bgs.begin();
  for ( ; bgi != bgs.end(); ++bgi)
    cout << "found " << *bgi << endl;
  bgi = bgs.begin();
  idx<ubyte> bg = load_image<ubyte>(*bgi);
  bg = image_resize(bg, conf.get_uint("winszhmax"),
		    conf.get_uint("winszhmax") * 5, 0);
  cout << "loaded " << *bgi << ": " << bg << endl;
  idx<ubyte> bgwin;
  bgwin = bg.narrow(0, MIN(bg.dim(0), winszh),
		    MAX(0, bg.dim(0) / 2 - winszh/2));
  bgwin = bgwin.narrow(1, MIN(bg.dim(1), winszw),
		       MAX(0, bg.dim(1) / 2 - winszw/2));

  vision_thread vt(conf, argc == 3 ? argv[2] : NULL);
  vt.start();
  idx<t_net> frame(1,1,1);
  vector<bbox*> bboxes;
  rect pos(0, 0, 10, 10), srcpos(0, 0, 10, 10), tgtpos(0, 0, 10, 10);
  bbox *b = NULL;
  float h = 0, w = 0;
  // timing variables
  QTime main_timer, bg_timer, vt_timer, gui_timer, detection_timer;
  int main_time, vt_time, gui_time, detection_time; // time in milliseconds
  main_timer.start();
  bg_timer.start();
  vt_timer.start();
  gui_timer.start();
  detection_timer.start();
  int bgtime = conf.get_uint("bgtime") * 1000;

  // interpolation
  bool updated = false;
  bool first_time = true;
  
  while (1) {
    try {
      // check if new data is avaiable
      updated = vt.get_data(frame, bboxes);
      // update target position if vision thread ready
      if (updated) {
	// find bbox with max confidence
	if (bboxes.size() > 0) {
	  double maxconf = -5.0;
	  uint maxi = 0;
	  for (uint i = 0; i < bboxes.size(); ++i)
	    if (bboxes[i]->confidence > maxconf) {
	      maxconf = bboxes[i]->confidence;
	      maxi = i;
	    }
	  b = bboxes[maxi];
	  if (b) {
	    tgtpos.h0 = b->h0;
	    tgtpos.w0 = b->w0;
	    tgtpos.height = b->height;
	    tgtpos.width = b->width;
	    srcpos = pos;
	    detection_timer.restart();
	  }
	  if (first_time) {
	    pos = tgtpos;
	    srcpos = pos;
	    first_time = false;
	  }
	}
	// update vt time
	vt_time = vt_timer.elapsed();
	vt_timer.restart();
	// print timing info
	cout << "main: " << main_time << " ms "
	     << "gui: " << gui_time << " ms "
	     << "vision: " << vt_time << " ms " << endl;
      }
      // update position and draw if gui thread ready
      if (!gui.busy_drawing()) {
	// recompute position
	estimate_position(srcpos, pos, tgtpos, frame, h, w, conf,
			  detection_timer.elapsed() / (float) vt_time);
	// narrow original image into window
	bgwin = bg.narrow(0, MIN(bg.dim(0), winszh),
			  MIN(MAX(0, bg.dim(0) - 1 - winszh),
			      MAX(0, (1 - h) * (bg.dim(0) - winszh))));
	bgwin = bgwin.narrow(1, MIN(bg.dim(1), winszw),
			     MIN(MAX(0, bg.dim(1) - 1 - winszw),
				 MAX(0, w * (bg.dim(1) - winszw))));
	// draw
	draw(b, pos, bgwin, frame, conf);
	// update gui time
	gui_time = gui_timer.elapsed();
	gui_timer.restart();
      }
      // sleep for a little bit
      usleep(conf.get_uint("mainsleep") * 1000);
      // main timing
      main_time = main_timer.elapsed();
      main_timer.restart(); 
      // change background every bgtime
      if (bg_timer.elapsed() > bgtime) {
	bg_timer.restart();
	change_background(bgi, bgs, bg, conf);
      }
    } catch (string &err) {
      cerr << err << endl;
    }
   }
#endif
}
