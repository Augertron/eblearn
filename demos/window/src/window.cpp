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

#ifdef __GUI__
#include "libeblearngui.h"
#endif

#ifdef __OPENCV__
#include <opencv/cv.h>
//#include "FastMatchTemplate.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

////////////////////////////////////////////////////////////////
// main thread

void draw(bbox *b, rect &pos, idx<ubyte> &bgwin, idx<ubyte> &frame,
	  configuration &conf) {
  uint control_offset = conf.get_uint("control_offset");
  uint text_hoffset = bgwin.dim(0) - conf.get_uint("text_hoffset");
  uint text_woffset = bgwin.dim(1) / 2 - conf.get_uint("text_woffset");
  uint text_height = conf.get_uint("text_height");
  disable_window_updates();
  clear_window();
  set_font_size(conf.get_int("font_size"));
  gui.draw_matrix_unsafe(bgwin);
  uint hface = 0;//bgwin.dim(0) - frame.dim(0) - control_offset;
  uint wface = 0;//bgwin.dim(1) / 2 - frame.dim(1) / 2;
  gui << at(text_hoffset - text_height * 4, text_woffset)
      << "Imagine this is a window to outside world.";
  gui << at(text_hoffset - text_height * 3, text_woffset) <<
    "Move your head down to see the sky,";
  gui << at(text_hoffset - text_height * 2, text_woffset) <<
    "up to see the ground,";
  gui << at(text_hoffset - text_height, text_woffset) <<
    "left to see right and right to see left.";
  if (b) {
    draw_box(hface + pos.h0, wface + pos.w0, pos.width, pos.height, 0, 0, 255);
  }
  draw_matrix(frame, hface, wface);
  enable_window_updates();
}

#ifdef __GUI__

void estimate_position(rect &srcpos, rect &pos, rect &tgtpos, idx<ubyte> &frame,
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
  try {
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

  detection_thread<float> dt(conf, argc == 3 ? argv[2] : NULL);
  dt.start();
  idx<ubyte> frame(1,1,1);
  idx<ubyte> tpl(10, 10, 1), uframe; // undefined template at beginning
  vector<bbox*> bboxes;
  rect pos(0, 0, 10, 10), srcpos(0, 0, 10, 10), tgtpos(0, 0, 10, 10);
  bbox *b = NULL;
  float h = 0, w = 0;
  // timing variables
  QTime main_timer, bg_timer, dt_timer, gui_timer, detection_timer;
  int main_time, dt_time, gui_time, detection_time; // time in milliseconds
  float tgt_time_distance = 0;
  main_timer.start();
  bg_timer.start();
  dt_timer.start();
  gui_timer.start();
  detection_timer.start();
  int bgtime = conf.get_uint("bgtime") * 1000;

  // interpolation
  bool updated = false;
  bool first_time = true;

  string        cam_type        = conf.get_string("camera");
  int           height          = conf.get_int("input_height");
  int           width           = conf.get_int("input_width");
  bool          save_video    = conf.exists_bool("save_video");
  // initialize camera (opencv, directory, shmem or video)
  camera<ubyte> *cam = NULL, *cam2 = NULL;
  if (conf.exists_bool("retrain") && conf.exists("retrain_dir")) {
    // extract false positives
    cam = new camera_directory<ubyte>(conf.get_cstring("retrain_dir"));
  } else { // regular execution
    if (!strcmp(cam_type.c_str(), "directory")) {
      if (argc == 3) cam = new camera_directory<ubyte>(argv[2], height, width);
      else eblerror("expected 2nd argument");
    } else if (!strcmp(cam_type.c_str(), "opencv"))
      cam = new camera_opencv<ubyte>(-1, height, width);
    else if (!strcmp(cam_type.c_str(), "v4l2"))
      cam = new camera_v4l2<ubyte>(conf.get_cstring("device"), height, width);
    else if (!strcmp(cam_type.c_str(), "shmem"))
      cam = new camera_shmem<ubyte>("shared-mem", height, width);
    else if (!strcmp(cam_type.c_str(), "video")) {
      if (argc == 3)
	cam = new camera_video<ubyte>
	  (argv[2], height, width, conf.get_uint("input_video_sstep"),
	   conf.get_uint("input_video_max_duration"));
      else eblerror("expected 2nd argument");
    } else eblerror("unknown camera type");
    // a camera directory may be used first, then switching to regular camera
    if (conf.exists_bool("precamera"))
      cam2 = new camera_directory<ubyte>(conf.get_cstring("precamdir"),
					 height, width);
  }
  night_mode();
  if (save_video)
    cam->start_recording();
  
#ifdef __OPENCV__
  IplImage *iplframe = NULL;
  IplImage *ipltemplate = NULL;
#endif
  
  while(!cam->empty()) {
    try {
      frame = cam->grab();
      // send new frame to vision_thread and check if new data was output
      dt.set_data(frame);
      updated = dt.get_data(bboxes);
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
	    // update template
	    idx<ubyte> tmptpl = frame.narrow(0, b->height, b->h0);
	    tmptpl = tmptpl.narrow(1, b->width, b->w0);
	    tpl = idx<ubyte>(tmptpl.get_idxdim());
	    idx_copy(tmptpl, tpl);
	  }
	  if (first_time) {
	    pos = tgtpos;
	    srcpos = pos;
	    first_time = false;
	  }
	} else
	  b = NULL;
	// update dt time
	dt_time = dt_timer.elapsed();
	dt_timer.restart();
	// print timing info
	cout << "main: " << main_time << " ms "
	     << "gui: " << gui_time << " ms "
	     << "vision: " << dt_time << " ms " << endl;
      }
      // tracking
#ifdef __OPENCV__
      uframe = idx<ubyte>(frame.get_idxdim());
      idx_copy(frame, uframe);
      // IplImage* iplframe = idx_to_ubyte_ipl(uframe);
      // IplImage* ipltpl = idx_to_ubyte_ipl(tpl);
      IplImage* iplframe = ipl_pointer_to_idx(uframe);
      IplImage* ipltpl = ipl_pointer_to_idx(tpl);
      CvPoint minloc, maxloc;
       double minval, maxval;
      // vector<CvPoint> found;
      // vector<double> confidences;
      // FastMatchTemplate(*iplframe, *ipltpl, &found, &confidences, 1, false,
      // 			5, 1, 15);
      //      CvRect searchRoi;
      //      cvSetImageROI( searchImage, searchRoi );
      // vector<CvPoint>::iterator p = found.begin();
      // vector<double>::iterator c = confidences.begin();
      // for ( ; p != found.end() && c != confidences.end(); ++p, ++c) {
      // 	gui << at(p->x, p->y) << *c;
      // }
      IplImage *tm = cvCreateImage(cvSize(uframe.dim(1)  - tpl.dim(1)  + 1,
					  uframe.dim(0) - tpl.dim(0) + 1 ),
				   IPL_DEPTH_32F, 1 );
      cvMatchTemplate(iplframe, ipltpl, tm, CV_TM_SQDIFF_NORMED);
      cvMinMaxLoc(tm, &minval, &maxval, &minloc, &maxloc, 0);
      gui << at(minloc.y, minloc.x) << "M";
      // cvReleaseImage(&iplframe);
      // cvReleaseImage(&ipltpl);
#endif
      // update position and draw if gui thread ready
      if (!gui.busy_drawing()) {
	// recompute position
	if (dt_time > 0)
	  tgt_time_distance = detection_timer.elapsed() / (float) dt_time;
	estimate_position(srcpos, pos, tgtpos, frame, h, w, conf,
			  tgt_time_distance);
	// narrow original image into window
	bgwin = bg.narrow(0, MIN(bg.dim(0), winszh),
			  MIN(MAX(0, bg.dim(0) - 1 - winszh),
			      MAX(0, (1 - h) * (bg.dim(0) - winszh))));
	bgwin = bgwin.narrow(1, MIN(bg.dim(1), winszw),
			     MIN(MAX(0, bg.dim(1) - 1 - winszw),
				 MAX(0, w * (bg.dim(1) - winszw))));
	// draw
	draw(b, pos, bgwin, frame, conf);
      draw_matrix(tpl, (uint)0, (uint)180);
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
      if (save_video)
	cam->record_frame();      
    } catch (string &err) {
      cerr << err << endl;
    }
  }
  if (save_video)
    cam->stop_recording(conf.exists_bool("use_original_fps") ?
			cam->fps() : conf.get_uint("save_video_fps"));
  if (cam) delete cam;
#endif
  } catch(string &err) {
    cerr << err << endl;
  }
}
