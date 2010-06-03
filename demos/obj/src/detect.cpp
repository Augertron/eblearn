/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include "libeblearn.h"
#include "libeblearntools.h"

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#ifdef __GUI__
#include "libeblearngui.h"
#endif

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
    try {
      // check input parameters
      if ((argc != 2) && (argc != 3) ) {
	cerr << "wrong number of parameters." << endl;
	cerr << "usage: obj_detect <config file> [directory or file]" << endl;
	return -1;
      }
#ifdef __LINUX__
      feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
      ipp_init(1); // limit IPP (if available) to 1 core
      // load configuration
      configuration	conf(argv[1]);
      bool		color	      = conf.exists_bool("color");
      uint		norm_size     = conf.get_uint("normalization_size");
      t_net		threshold     = (t_net) conf.get_double("threshold");
      bool		display       = false;
      bool		mindisplay    = false;
      uint		display_sleep = 0;
      bool		save_video    = false;
      string		cam_type      = conf.get_string("camera");
      int		height        = conf.get_int("input_height");
      int		width         = conf.get_int("input_width");
      uint              wid           = 0; // window id
      string		outdir        = "out_";
      outdir += tstamp();
      outdir += "/";
      cout << "Saving outputs to " << outdir << endl;

      // load network and weights
      parameter<t_net> theparam;
      idx<ubyte> classes(1,1);
      try {
	load_matrix<ubyte>(classes, conf.get_cstring("classes"));
      } catch(string &err) { cerr << "warning: " << err << endl; }
      cout << "loading weights from " << conf.get_cstring("weights") << endl;
      module_1_1<t_net> *net = create_network(theparam, conf, classes.dim(0));
      theparam.load_x(conf.get_cstring("weights"));

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
      if (conf.exists("input_min")) // limit inputs size
	detect.set_min_resolution(conf.get_uint("input_min")); 
      if (conf.exists("input_max")) // limit inputs size
	detect.set_max_resolution(conf.get_uint("input_max"));
      detect.set_silent();
      if (conf.exists_bool("save_detections")) {
	string detdir = outdir;
	detdir += "detections";
	detdir = detect.set_save(detdir);
	if (conf.exists("save_max_per_frame"))
	  detect.set_save_max_per_frame(conf.get_uint("save_max_per_frame"));
      }
      if (conf.exists("pruning"))
	detect.set_pruning(conf.get_bool("pruning"));
      if (conf.exists("bbhfactor") && conf.exists("bbwfactor"))
	detect.set_bbox_factors(conf.get_float("bbhfactor"),
				conf.get_float("bbwfactor"));

      // initialize camera (opencv, directory, shmem or video)
      idx<t_net> frame;
      camera<t_net> *cam = NULL, *cam2 = NULL;
      if (conf.exists_bool("retrain") && conf.exists("retrain_dir")) {
	// extract false positives
	cam = new camera_directory<t_net>(conf.get_cstring("retrain_dir"));
      } else { // regular execution
	if (!strcmp(cam_type.c_str(), "directory")) {
	  if (argc == 3) 
	    cam = new camera_directory<t_net>(argv[2], height, width);
	  else if (conf.exists("input_dir"))
	    cam = new camera_directory<t_net>(conf.get_cstring("input_dir"), 
					      height, width);
	  else eblerror("expected 2nd argument");
	} else if (!strcmp(cam_type.c_str(), "opencv"))
	  cam = new camera_opencv<t_net>(-1, height, width);
#ifdef __LINUX__
	else if (!strcmp(cam_type.c_str(), "v4l2"))
	  cam = new camera_v4l2<t_net>(conf.get_cstring("device"),
				       height, width);
#endif
	else if (!strcmp(cam_type.c_str(), "shmem"))
	  cam = new camera_shmem<t_net>("shared-mem", height, width);
	else if (!strcmp(cam_type.c_str(), "video")) {
	  if (argc == 3)
	    cam = new camera_video<t_net>
	      (argv[2], height, width, conf.get_uint("input_video_sstep"),
	       conf.get_uint("input_video_max_duration"));
	  else eblerror("expected 2nd argument");
	} else eblerror("unknown camera type");
	// a camera directory may be used first, then switching to regular cam
	if (conf.exists_bool("precamera"))
	  cam2 = new camera_directory<t_net>(conf.get_cstring("precamdir"),
					     height, width);
      }

      // answer variables & initializations
      vector<bbox*> bboxes;
      vector<bbox*>::iterator ibboxes;
      ostringstream answer_fname;
      mkdir_full(outdir);
      answer_fname << outdir << "bbox.txt";
      // open file      
      ofstream fp(answer_fname.str().c_str());
      if (!fp) {
	cerr << "failed to open " << answer_fname.str() << endl;
	eblerror("open failed");
      }
    
      // gui
#ifdef __GUI__
      display 	= conf.exists_bool("display");
      mindisplay 	= conf.exists_bool("minimal_display");
      display_sleep	= conf.get_uint("display_sleep");
      save_video    = conf.exists_bool("save_video");
      uint qstep1 = 0, qheight1 = 0, qwidth1 = 0,
	qheight2 = 0, qwidth2 = 0, qstep2 = 0;
      if (conf.exists_bool("queue1")) {
	qstep1 = conf.get_uint("qstep1");
	qheight1 = conf.get_uint("qheight1");
	qwidth1 = conf.get_uint("qwidth1"); }
      if (conf.exists_bool("queue2")) {
	qstep2 = conf.get_uint("qstep2");
	qheight2 = conf.get_uint("qheight2");
	qwidth2 = conf.get_uint("qwidth2"); }
      module_1_1_gui netgui;
      wid  = display ? new_window("eblearn object recognition") : 0;
      float	zoom = 1;
      detector_gui<t_net> dgui(conf.exists_bool("queue1"), qstep1, qheight1,
			       qwidth1, conf.exists_bool("queue2"), qstep2,
			       qheight2, qwidth2);
      if (bmask_class)
	dgui.set_mask_class(conf.get_cstring("mask_class"),
			    (t_net) conf.get_double("mask_threshold"));
      night_mode();
      if (save_video) {
	string viddir = outdir;
	viddir += "video";
	cam->start_recording(wid, viddir.c_str());
      }
#endif  
      // timing variables
      timer tpass, toverall;
      long ms;
  
      // loop
      toverall.start();
      while(!cam->empty()) {
	// cout << "nopend: " << Srg<double>::nopened;
	// cout << " nopend: " << Srg<short>::nopened;
	// cout << " nopend: " << Srg<char const *>::nopened << endl;
	// get a new frame
	tpass.restart();
	// if the pre-camera is defined use it until empty
	if (cam2 && !cam2->empty())
	  frame = cam2->grab();
	else // empty pre-camera, use regular camera
	  frame = cam->grab();
	// run detector
	if (!display) { // fprop without display
	  bboxes = detect.fprop(frame, threshold);
	} 
#ifdef __GUI__
	else { // fprop and display
	  disable_window_updates();
	  clear_window();
	  if (mindisplay)
	    bboxes = dgui.display(detect, frame, threshold, 0, 0, zoom,
				  (t_net)0, (t_net)255, wid);
	  else
	    bboxes =
	      dgui.display_inputs_outputs(detect, frame, threshold, 0, 0, zoom,
					  (t_net)-1.1, (t_net)1.1, wid); 
	  enable_window_updates();
	  if (save_video)
	    cam->record_frame();
	}	    
#endif
	ms = tpass.elapsed_milliseconds();
	cout << "processing: " << ms << " ms." << endl;
	cout << "fps: " << cam->fps() << endl;
	// save bounding boxes
	for (ibboxes = bboxes.begin(); ibboxes != bboxes.end(); ++ibboxes) {
	  fp << cam->frame_name() << " " << (*ibboxes)->class_id << " "
	     << (*ibboxes)->confidence << " ";
	  fp << (*ibboxes)->w0 << " " << (*ibboxes)->h0 << " ";
	  fp << (*ibboxes)->w0 + (*ibboxes)->width << " ";
	  fp << (*ibboxes)->h0 + (*ibboxes)->height << endl;
	}
	// sleep display
	if (display_sleep > 0) {
	  cout << "sleeping for " << display_sleep << "ms." << endl;
	  millisleep(display_sleep);
	}
	if (conf.exists("save_max") && 
	    detect.get_total_saved() > conf.get_uint("save_max")) {
	  cout << "Reached max number of detections, exiting." << endl;
	  break ; // limit number of detection saves
	}
      }
      cout << "Execution time: " << toverall.elapsed_minutes() <<" mins" <<endl;
      if (save_video)
	cam->stop_recording(conf.exists_bool("use_original_fps") ?
			    cam->fps() : conf.get_uint("save_video_fps"));
      // free variables
      if (net) delete net;
      if (cam) delete cam;
      if (pp) delete pp;
      // close files
      fp.close();
#ifdef __GUI__
      quit_gui(); // close all windows
#endif
    } catch(string &err) { eblerror(err.c_str()); }
  return 0;
}
