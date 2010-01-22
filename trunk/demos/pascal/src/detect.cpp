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

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

// argv[1] is expected to contain the directory of the pascal dataset
#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
#ifndef __BOOST__
  eblerror(BOOST_LIB_ERROR);
#endif
  // misc initializations
  cout << "* PASCAL object recognition demo" << endl;
  if (argc != 2) {
    cout << "Usage: ./pascal_detect <config file>" << endl;
    eblerror("config file not specified");
  }
  typedef float t_net;
  feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
  init_drand(time(NULL)); // initialize random seed
  configuration conf(argv[1]); // configuration file

  //! load classes names
  idx<ubyte> classes(1,1);
  load_matrix<ubyte>(classes, conf.get_cstring("classes"));
  
  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  idx<t_net> targets =
    create_target_matrix<t_net>(classes.dim(0), 1.0);

  //! create the network weights, network and trainer
  parameter<t_net> theparam; // trained weights
  lenet<t_net> net(theparam,
		   conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
		   conf.get_uint("net_c1h"), conf.get_uint("net_c1w"),
		   conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
		   conf.get_uint("net_c2h"), conf.get_uint("net_c2w"),
		   conf.get_uint("net_s2h"), conf.get_uint("net_s2w"),
		   conf.get_uint("net_full"), targets.dim(0),
		   conf.get_bool("absnorm"), conf.get_bool("color"));
  theparam.load_x(conf.get_cstring("weights"));
  rgb_to_ypuv_module<t_net> pp(conf.get_uint("normalization_size"));
  state_idx<t_net> buf(1,1,1);
  layers_2<t_net> ppnet((module_1_1<t_net>&)pp, buf, (module_1_1<t_net>&)net);

  // gui
  bool display = conf.get_bool("display"); // enable/disable display
  uint wid;
  float zoom = .5;
  if (display)
    wid = new_window("pascal detector");

  // detector
  detector<t_net> detect((module_1_1<t_net>&)ppnet, 5, classes);
  detect.set_bgclass("bg");
  detector_gui dgui;

  // answering variables and initializations
  t_net threshold = .98;
  vector<bbox> bboxes;
  vector<bbox>::iterator ibboxes;
  map<string, ofstream*> fp_cls, fp_det;
  map<string, ofstream*>::iterator ifp_cls, ifp_det;
  string name, img_name;
  // open all answer files
  ostringstream fname;
  idx_bloop1(cname, classes, ubyte) {
    name = (const char*) cname.idx_ptr();
    // classification task
    fname.str("");
    fname << conf.get_string("answer_root") << "/comp1_cls_test_";
    fname << name << ".txt";
    fp_cls[name] = new ofstream(fname.str().c_str());
    if (!*fp_cls[name]) {
      cerr << "failed to open " << fname.str() << endl;
      eblerror("open failed");
    }
    // detection task
    fname.str("");
    fname << conf.get_string("answer_root") << "/comp1_det_test_";
    fname << name << ".txt";
    fp_det[name] = new ofstream(fname.str().c_str());
    if (!*fp_det[name]) {
      cerr << "failed to open " << fname.str() << endl;
      eblerror("open failed");
    }
  }

  // process all images found in test directory
  cmatch what;
  regex r(IMAGE_PATTERN);
  path p(conf.get_string("test_root"));
  idx<t_net> im(1, 1, 1);
  if (!exists(p)) eblerror("cannot open test_root directory");
  directory_iterator end_itr; // default construction yields past-the-end
  for (directory_iterator itr(p); itr != end_itr; ++itr) {
    if (!is_directory(itr->status()) &&
	(regex_match(itr->leaf().c_str(), what, r))) {
      // load image and prepare it
      img_name = itr->leaf().substr(0, itr->leaf().size() - 4);
      cout << img_name << ":";
      im = load_image<t_net>(itr->path().string());
      
//       state_idx<t_net> in(im.dim(2), im.dim(0), im.dim(1));
//       im = im.shift_dim(2, 0);
//       idx_copy(im, in.x);
//   pp.fprop(in, buf);
//   idx<t_net> im2 = buf.x.shift_dim(0, 2);

      // run network
#ifndef __GUI__
      bboxes = detect.fprop(im, threshold);
#else
      if (display) {
	disable_window_updates();
	clear_window();
	bboxes = dgui.display(detect, im, threshold, 0, 0, zoom,
					     (t_net)0, (t_net)255, wid);
	enable_window_updates();
	//	sleep(1);
      } else
	bboxes = detect.fprop(im, threshold);
#endif
      cout << endl;
      // output answers and confidences to files
      for (ibboxes = bboxes.begin(); ibboxes != bboxes.end(); ++ibboxes) {
	name = (const char *) classes[ibboxes->class_id].idx_ptr();
	*fp_cls[name] << img_name << " " << ibboxes->confidence << endl;
	*fp_det[name] << img_name << " " << ibboxes->confidence << " ";
	*fp_det[name] << ibboxes->w0 << " " << ibboxes->h0 << " ";
	*fp_det[name] << ibboxes->w0 + ibboxes->width << " ";
	*fp_det[name] << ibboxes->h0 + ibboxes->height << endl;
      }
    }
  }
  // close all file pointers
  for (ifp_cls = fp_cls.begin(); ifp_cls != fp_cls.end(); ++ifp_cls) {
    ifp_cls->second->close();
    delete ifp_cls->second;
  }
  for (ifp_det = fp_det.begin(); ifp_det != fp_det.end(); ++ifp_det) {
    ifp_det->second->close();
    delete ifp_det->second;
  }
  return 0;
}
