#include <fenv.h>
#include "libeblearn.h"
#include "libeblearntools.h"
#include "objrec.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

////////////////////////////////////////////////////////////////
// network

//template <class T> intg Srg<T>::nopened = 0;
// intg Srg<float>::nopened = 0;
// intg Srg<int>::nopened = 0;
// intg Srg<ubyte>::nopened = 0;
// intg Srg<uint>::nopened = 0;

typedef float t_net; // network precision

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
  // check input parameters
  if ((argc != 2) && (argc != 3)) {
    cerr << "wrong number of parameters." << endl;
    cerr << "usage: obj_detect <config file> [directory or file]" << endl;
    return -1;
  }
  feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
  // load configuration
  configuration conf(argv[1]);
  bool		color		= conf.get_bool("color");
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
  load_matrix<ubyte>(classes, conf.get_cstring("classes"));
  cout << "loading weights from " << conf.get_cstring("weights") << endl;
  module_1_1<t_net> *net = init_network(theparam, conf, classes.dim(0));
  if (!theparam.load_x<double>(conf.get_cstring("weights")))
    eblerror("failed to load weights");

  // select preprocessing  
  module_1_1<t_net>* pp = color ?
    (module_1_1<t_net>*) new rgb_to_ypuv_module<t_net>(norm_size) :
    (module_1_1<t_net>*) new rgb_to_yp_module<t_net>(norm_size);
  // detector
  detector<t_net> detect(*net, classes, pp, norm_size, NULL, 0,
			 conf.get_double("gain"), 50);
  detect.set_resolutions(1.4);
  detect.set_max_resolution(600); // limit size of inputs
  //  detect.set_resolutions(9);
  //  double scales[] = { 4.5, 2.5, 1.4};
  //  detect.set_resolutions(3, scales);
  detect.set_silent();
  if (conf.get_bool("save_detections"))
    detect.set_save("detections");

  // initialize camera (opencv, directory, shmem or video)
  idx<t_net> frame;
  camera<t_net> *cam = NULL;
  if (!strcmp(cam_type.c_str(), "directory")) {
    if (argc == 3) cam = new camera_directory<t_net>(argv[2], height, width);
    else eblerror("expected 2nd argument");
  } else if (!strcmp(cam_type.c_str(), "opencv"))
    cam = new camera_opencv<t_net>(-1, height, width);
  else if (!strcmp(cam_type.c_str(), "shmem"))
    cam = new camera_shmem<t_net>("shared-mem", height, width);
  else if (!strcmp(cam_type.c_str(), "video")) {
    if (argc == 3)
      cam = new camera_video<t_net>
	(argv[2], height, width, conf.get_uint("input_video_sstep"),
	 conf.get_uint("input_video_max_duration"));
    else eblerror("expected 2nd argument");
  } else eblerror("unknown camera type");
  
  // gui
#ifdef __GUI__
  display 	= conf.get_bool("display");
  mindisplay 	= conf.get_bool("minimal_display");
  display_sleep	= conf.get_uint("display_sleep");
  save_video    = conf.get_bool("save_video");
  module_1_1_gui netgui;
  uint	wid	= display ? new_window("eblearn object recognition") : 0;
  float zoom	= 1;
  detector_gui dgui;
  night_mode();
  if (save_video)
    cam->start_recording();
  // timing variables
  QTime t0;
  int tpp;
#endif  
  
  // loop
  while(!cam->empty()) {
    // cout << "nopend: " << Srg<double>::nopened;
    // cout << " nopend: " << Srg<short>::nopened;
    // cout << " nopend: " << Srg<char const *>::nopened << endl;
    // get a new frame
#ifdef __GUI__
    t0.start();
#endif
    frame = cam->grab();
    // run detector
    if (!display) { // fprop without display
      detect.fprop(frame, threshold);
    } 
#ifdef __GUI__
    else { // fprop and display
      disable_window_updates();
      clear_window();
      if (mindisplay)
	  dgui.display(detect, frame, threshold, 0, 0, zoom,
		       (t_net)0, (t_net)255, wid);
      else
	dgui.display_inputs_outputs(detect, frame, threshold, 0, 0, zoom,
				    (t_net)-1.1, (t_net)1.1, wid); 
      enable_window_updates();
      if (save_video)
	cam->record_frame();
    }
    tpp = t0.elapsed(); // stop processing timer
    cout << "processing: " << tpp << " ms." << endl;
    cout << "fps: " << cam->fps() << endl;
#endif
    if (display_sleep > 0) {
      cout << "sleeping for " << display_sleep << "ms." << endl;
      usleep(display_sleep);
    }
  }
  if (save_video)
    cam->stop_recording(cam->fps());
  // free variables
  if (net) delete net;
  if (cam) delete cam;
  if (pp) delete pp;
  return 0;
}
