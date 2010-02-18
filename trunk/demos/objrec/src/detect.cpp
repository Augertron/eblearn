#include <fenv.h>
#include "libeblearn.h"
#include "libeblearntools.h"
#include "objrec.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <algorithm>

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

////////////////////////////////////////////////////////////////
// network

typedef double t_net; // network precision

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
  // check input parameters
  if ((argc != 2) && (argc != 3)) {
    cerr << "wrong number of parameters." << endl;
    cerr << "usage: obj_detect <config file> [input path]" << endl;
    cerr << "  if input path is present, recursively classify images found ";
    cerr << "  in path, otherwise use camera as input." << endl;
    return -1;
  }
  // load configuration
  configuration conf(argv[1]);
  bool		color		= conf.get_bool("color");
  uint		norm_size	= conf.get_uint("normalization_size");
  t_net		threshold	= (t_net) conf.get_double("threshold");
  bool		save_detections = conf.get_bool("save_detections");
  bool		display 	= false;

  // load network and weights
  parameter<t_net> theparam;
  idx<ubyte> classes(1,1);
  load_matrix<ubyte>(classes, conf.get_cstring("classes"));
  cout << "loading weights from " << conf.get_cstring("weights") << endl;
  module_1_1<t_net> *net = init_network(theparam, conf, classes.dim(0));
  theparam.load_x(conf.get_cstring("weights"));

  // find background class id
  intg bgid = -1;
  for (intg i = 0; i < classes.dim(0); ++i)
    if (!strcmp((const char *) classes[i].idx_ptr(), "bg"))
      bgid = i;
  if (bgid == -1) eblerror("no background class");

  // initialize camera if used, otherwise list images in directory
  bool use_cam = (argc == 3) ? false : true;
  string ipath = (argc == 3) ? argv[2] : "";
  idx<t_net> image;
  camera<t_net> *cam = NULL;
  files_list *fl = NULL;
  string fdir, fname;
  ostringstream oss;
  uint frame_id = 0;
  if (use_cam)
    cam = new camera<t_net>(-1, 240, 320);
  else { // otherwise initialize the list of images to process
    cout << "Processing all images in directory " << ipath << endl;
    fl = find_images(ipath);
    if (!fl) { cerr << "error wrong directory: " << ipath << endl; return -1; }
    cout << "Found " << fl->size() << " images." << endl;
  }
  
  // gui
#ifdef __GUI__
  display 	= conf.get_bool("display");
  module_1_1_gui netgui;
  uint	wid	= display ? new_window("eblearn object recognition") : 0;
  float zoom	= 1;
  detector_gui dgui;
  // timing variables
  QTime t0;
  int tpp;
#endif  
  // select preprocessing  
  bgr_to_yp_module<t_net> ppyp(norm_size);
  bgr_to_ypuv_module<t_net> ppypuv(norm_size);
  module_1_1<t_net> &pp = color ?
    (module_1_1<t_net>&) ppypuv : (module_1_1<t_net>&) ppyp;
  // detector
  //  double scales[] = { 8, 4, 2};
  double scales[] = { 4.5, 2.5, 1.4};
  //  double scales[] = { 16, 12, 8, 6, 4, 2, 1 };
  //  double scales[] = { 3 };
  detector<t_net> detect(*net, classes, &pp, norm_size, 0,
			 conf.get_double("gain"));
  detect.set_resolutions(1.4);
  //  detect.set_resolutions(9);
  //  detect.set_resolutions(3, scales);
  detect.set_bgclass("bg");
  detect.set_silent();
  if (save_detections)
    detect.set_save("detections");

  while(1) {
    // get a new frame
#ifdef __GUI__
    t0.start();
#endif
    if (use_cam) { // get frame from camera
      image = cam->grab();
      cout << "fps: " << cam->fps() << endl;
    } else { // get image from input directory
      if (fl->size() == 0)
	break ; // stop when list is empty
      fdir = fl->front().first; // directory
      fname = fl->front().second; // file name
      fl->pop_front(); // remove first element
      cout << frame_id << "/" << fl->size() << ": processing " << fdir << "/" << fname << endl;
      oss.str(""); oss << fdir << "/" << fname;
      try {
	image = load_image<t_net>(oss.str());
      } catch (const char *err) {
	cerr << "failed to load image " << oss.str() << ". Continuing..." << endl;
	continue ;
      }
    }
    // run detector
    if (!display) { // fprop without display
      detect.fprop(image, threshold);
    } 
#ifdef __GUI__
    else { // fprop and display
      disable_window_updates();
      clear_window();
      dgui.display_inputs_outputs(detect, image, threshold, 0, 0, zoom,
				  (t_net)-1.1, (t_net)1.1, wid); 
      enable_window_updates();
    }
    tpp = t0.elapsed(); // stop processing timer
    cout << "processing: " << tpp << " ms." << endl;
#endif
    frame_id++;
    //    sleep(1);
  }
  // free variables
  if (net) delete net;
  if (cam) delete cam;
  if (fl) delete fl;
  return 0;
}
