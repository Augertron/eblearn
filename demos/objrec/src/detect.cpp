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
#ifndef __OPENCV__
  eblerror("opencv not found, install and recompile");
#else
  // camera
  CvCapture* capture = cvCaptureFromCAM(1);
  if( !capture ) {
    fprintf( stderr, "ERROR: capture is NULL \n" );
    getchar();
    return -1;
  }
  // get 1st frame
  IplImage* frame = cvQueryFrame(capture);
  if (!frame) { cerr << "failed to grab frame." << endl; return -1; }
  idx<t_net> image = ipl2idx<t_net>(frame);

  // load configuration
  configuration conf(argv[1]);
  bool	color	  = conf.get_bool("color");
  uint	norm_size = conf.get_uint("normalization_size");
  t_net threshold = (t_net) conf.get_double("threshold");
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

  // gui
  module_1_1_gui netgui;
  bool	display = conf.get_bool("display");
  uint	wid	= display ? new_window("iris") : 0;
  float zoom	= 1;
  
  // initialize some buffers
  //  idxdim d(conf.get_uint("net_ih"), conf.get_uint("net_iw"), 3);
  idxdim d(240, 320, 1);
  idxdim d2(240, 320, 3);
  //  idxdim d(192, 256, 1);
  //  idxdim d2(192, 256, 3);
//   idxdim d(480, 640, 1);
//   idxdim d2(480, 640, 3);
  idx<t_net> im(d2), tmp(d.dim(0), d.dim(1));
  idx<t_net> resized;
  idx<t_net> y(d.dim(0), d.dim(1), color ? 3 : 1);
  //  state_idx<t_net> stin(d.dim(2), d.dim(0), d.dim(1)), stout(1,1,1);

  // select preprocessing  
  bgr_to_yp_module<t_net> ppyp(norm_size);
  bgr_to_ypuv_module<t_net> ppypuv(norm_size);
  module_1_1<t_net> &pp = color ?
    (module_1_1<t_net>&) ppypuv : (module_1_1<t_net>&) ppyp;
  // detector
  //double scales[] = { 2.7, 1.75};
  //  double scales[] = { 2.6, 2.0, 1.4};
  //  double scales[] = { 4.6, 2.8, 1.4};
  //  double scales[] = { 8, 4, 2};
  double scales[] = { 4.5, 2.5, 1.4};
  //  double scales[] = { 3 };
  detector<t_net> detect(*net, 3, scales, classes, &pp, norm_size, 0,
			 conf.get_double("gain"));
  detect.set_bgclass("bg");
  detect.set_silent();
  detector_gui dgui;

  // timing variables
  QTime t2;
  int tpp;
  time_t t0, t1;
  double diff;
  time(&t0);
  uint fps = 0, cnt = 0, iframe = 0;

  // answering variables

  // loop
  while(1) {
    // get a new frame
    frame = cvQueryFrame(capture);
    if (!frame) {
      cerr << "failed to grab frame." << endl;
      continue ;
    }
    t2.start();
    // convert ipl to idx image
    ipl2idx(frame, image);
    resized = image_mean_resize(image, d.dim(0), d.dim(1), 0);
    
    // run detector
    if (!display) { // fprop without display
      detect.fprop(resized, threshold);
    } else { // fprop and display
      disable_window_updates();
      clear_window();
      dgui.display_inputs_outputs(detect, resized, threshold, 0, 0, zoom,
				  (t_net)-1.1, (t_net)1.1, wid); 
      gui << at(image.dim(0) * zoom, 0) << "fps: " << fps;
      enable_window_updates();
    }
    tpp = t2.elapsed(); // stop processing timer
// //     // video
// //     //    ostringstream oss;
// //     //    oss << "video/frame_" << setfill('0') << setw(5) << iframe << ".png";
// //     //    cout << "saving " << oss.str() << endl;
// //     //    save_window(oss.str().c_str());
// //     //    usleep(200000);

// //     // use average answer
// //     answers.push_back(answer);
// //     if (answers.size() > 3)
// //       answers.pop_front();
// //     fill(votes.begin(), votes.end(), 0);
// //     for (ianswers = answers.begin(); ianswers != answers.end(); ++ianswers)
// //       votes[*ianswers] += 1;
// //     avg_answer = max_element(votes.begin(), votes.end()) - votes.begin();
    cout << "processing: " << tpp << " ms." << endl;
    // counters
    cnt++;
    iframe++;
    time(&t1);
    diff = difftime(t1, t0);
    if (diff >= 1) {
      fps = cnt;
      cnt = 0;
      time(&t0);
      cout << "fps: " << fps << endl;
    }
  }

  // release camera
  cvReleaseCapture(&capture);
  // free variables
  if (net) delete net;
#endif /* __OPENCV__ */
  return 0;
}
