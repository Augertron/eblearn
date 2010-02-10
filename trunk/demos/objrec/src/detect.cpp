#include <fenv.h>
#include "libeblearn.h"
#include "libeblearntools.h"

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

typedef float t_net; // network precision

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

  // load configuration, weights and classes names
  configuration conf("best.conf");
  bool color = conf.get_bool("color");
  uint norm_size = conf.get_uint("normalization_size");
  parameter<t_net> theparam;
  idx<ubyte> classes(1,1);
  load_matrix<ubyte>(classes, conf.get_cstring("classes"));
  cout << "loading weights from " << conf.get_cstring("weights") << endl;
  lenet_cscsc<t_net> net(theparam, 
			 conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
			 conf.get_uint("net_c1h"), conf.get_uint("net_c1w"),
			 conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
			 conf.get_uint("net_c2h"), conf.get_uint("net_c2w"),
			 conf.get_uint("net_s2h"), conf.get_uint("net_s2w"),
			 classes.dim(0), conf.get_bool("normabs"),
			 color, conf.get_bool("mirror"));
  theparam.load_x(conf.get_cstring("weights"));

  // find background class id
  intg bgid = -1;
  for (intg i = 0; i < classes.dim(0); ++i)
    if (!strcmp((const char *) classes[i].idx_ptr(), "bg"))
      bgid = i;
  if (bgid == -1) eblerror("no background class");

  // gui
  module_1_1_gui netgui;
  bool display = conf.get_bool("display");
  uint wid = display ? new_window("iris") : 0;
  float zoom = 1;
  
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

  // detector
  
  weighted_std_module<t_net> norm(norm_size, norm_size, 1, true, false, true);
  //double scales[] = { 2.7, 1.75};
  //  double scales[] = { 2.6, 2.0, 1.4};
  //  double scales[] = { 4.6, 2.8, 1.4};
  //  double scales[] = { 8, 4, 2};
  double scales[] = { 4.5, 2.5, 1.4};
  //double scales[] = { 1 };
  detector<t_net> detect((module_1_1<t_net>&)net, 3, scales, classes, &norm,
			 conf.get_uint("normalization_size"), 0, 1);
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
  t_net threshold = (t_net) conf.get_double("threshold");

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
    resized = image_resize(image, d.dim(0), d.dim(1), 0);
    bgr_to_y(resized, y);
    
    // run detector
    if (!display) { // fprop without display
      detect.fprop(y, threshold);
    } else { // fprop and display
      disable_window_updates();
      clear_window();
      dgui.display_inputs_outputs(detect, y, threshold, 0, 0, zoom,
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
#endif /* __OPENCV__ */
  return 0;
}
