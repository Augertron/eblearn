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

#ifdef __DEBUGMEM__
  INIT_DEBUGMEM()
#endif

typedef float t_net; // network precision

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
  int main(int argc, char **argv) { // regular main without gui
#endif
    try {
      // check input parameters
      if ((argc != 2) && (argc != 3) ) {
	cerr << "warning: wrong number of parameters." << endl;
	cerr << "usage: mtdetect <config file> [directory or file]" << endl;
	//	return -1;
      }
#ifdef __LINUX__
      feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
      // load configuration
      configuration	conf(argv[1]);
      uint              ipp_cores     = 1;
      if (conf.exists("ipp_cores")) ipp_cores = conf.get_uint("ipp_cores");
      ipp_init(ipp_cores); // limit IPP (if available) to 1 core
      bool		save_video    = conf.exists_true("save_video");
      string		cam_type      = conf.get_string("camera");
      int		height        = conf.get_int("input_height");
      int		width         = conf.get_int("input_width");
      string		outdir        = "out_";
      bool              input_random  = conf.exists_true("input_random");
      uint              npasses       = 1;
      if (conf.exists("input_npasses"))
	npasses = conf.get_uint("input_npasses");
      outdir += tstamp();
      outdir += "/";
      cout << "Saving outputs to " << outdir << endl;

      // allocate threads
      uint nthreads = 1;
      bool updated = false;
      idx<ubyte> detframe; // frame returned by detection thread
      if (conf.exists("nthreads"))
	nthreads = (std::max)((uint) 1, conf.get_uint("nthreads"));
      list<detection_thread<t_net>*>  threads;
      list<detection_thread<t_net>*>::iterator ithreads;
      idx<uint> total_saved(nthreads);
      idx_clear(total_saved);
      cout << "Initializing " << nthreads << " detection threads." << endl;
      for (uint i = 0; i < nthreads; ++i) {
	ostringstream tname;
	tname << "Thread " << i;
	detection_thread<t_net> *dt =
	  new detection_thread<t_net>(conf, tname.str().c_str());
	threads.push_back(dt);
	dt->start();
	dt->set_output_directory(outdir);
      }

      // initialize camera (opencv, directory, shmem or video)
      idx<ubyte> frame;
      camera<ubyte> *cam = NULL, *cam2 = NULL;
      if (!strcmp(cam_type.c_str(), "directory")) {
	if (argc >= 3) // read input dir from command line
	  cam = new camera_directory<ubyte>(argv[2], height, width,
					    input_random, npasses);
	else if (conf.exists("input_dir")) // read input dir from conf
	  cam = new camera_directory<ubyte>(conf.get_cstring("input_dir"), 
					    height, width, input_random,
					    npasses);
	else eblerror("expected 2nd argument");
      } else if (!strcmp(cam_type.c_str(), "opencv"))
	cam = new camera_opencv<ubyte>(-1, height, width);
#ifdef __LINUX__
      else if (!strcmp(cam_type.c_str(), "v4l2"))
	cam = new camera_v4l2<ubyte>(conf.get_cstring("device"),
				     height, width);
#endif
      else if (!strcmp(cam_type.c_str(), "shmem"))
	cam = new camera_shmem<ubyte>("shared-mem", height, width);
      else if (!strcmp(cam_type.c_str(), "video")) {
	if (argc >= 3)
	  cam = new camera_video<ubyte>
	    (argv[2], height, width, conf.get_uint("input_video_sstep"),
	     conf.get_uint("input_video_max_duration"));
	else eblerror("expected 2nd argument");
      } else eblerror("unknown camera type");
      // a camera directory may be used first, then switching to regular cam
      if (conf.exists_bool("precamera"))
	cam2 = new camera_directory<ubyte>(conf.get_cstring("precamdir"),
					   height, width, input_random,
					   npasses);
	
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
      // display	     = conf.exists_bool("display");
      // mindisplay     = conf.exists_bool("minimal_display");
      // display_sleep  = conf.get_uint("display_sleep");
      // display_states = conf.exists_bool("display_states");
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
      // wid_states  = display_states ? new_window("network states"):0;
      // night_mode();
      // wid  = display ? new_window("eblearn object recognition") : 0;
      // night_mode();
#endif  
      // timing variables
      timer tpass, toverall;
      uint cnt = 0;
      cout << "i=" << cnt << endl;
  
      // loop
      toverall.start();
      while(!cam->empty()) {
	// get a new frame
	tpass.restart();
	// check for results and send new image for each thread
	uint i = 0;
	for (ithreads = threads.begin(); 
	     ithreads != threads.end(); ++ithreads, ++i) {
	  string processed_fname;
	  // retrieve new data if present
	  updated = (*ithreads)->get_data(bboxes, detframe, 
					  *(total_saved.idx_ptr() + i),
					  processed_fname);
	  // save bounding boxes
	  if (updated) {
	    for (ibboxes = bboxes.begin(); ibboxes != bboxes.end(); ++ibboxes) {
	      fp << processed_fname << " " << (*ibboxes)->class_id << " "
		 << (*ibboxes)->confidence << " ";
	      fp << (*ibboxes)->w0 << " " << (*ibboxes)->h0 << " ";
	      fp << (*ibboxes)->w0 + (*ibboxes)->width << " ";
	      fp << (*ibboxes)->h0 + (*ibboxes)->height << endl;
	    }
	    updated = false;
	    cnt++;
	    cout << "total_saved=" << idx_sum(total_saved);
	    if (conf.exists("save_max"))
	      cout << " / " << conf.get_uint("save_max");
	    cout << endl;
	    cout << "i=" << cnt << " remaining=" << (cam->size() - cnt);
	    cout << " elapsed="; toverall.pretty_elapsed();
	    if (cam->size() > 0) {
	      cout << " ETA=";
	      timer::pretty_secs((long)((cam->size() - cnt) * 
					(toverall.elapsed_seconds() 
					 / (float) cnt)));
	    }
	    if (conf.exists("save_max")) {
	      cout << " save_max_ETA=";
	      uint total = idx_sum(total_saved);
	      timer::pretty_secs((long)((conf.get_uint("save_max") - total)
					* (toverall.elapsed_seconds() 
					   / (float)total)));
	    }
	    cout << endl;
	  }
	  // check if ready
	  if ((*ithreads)->available()) {
	    // grab a new frame if available
	    if (cam->empty())
	      break ;
	    // if the pre-camera is defined use it until empty
	    if (cam2 && !cam2->empty())
	      frame = cam2->grab();
	    else // empty pre-camera, use regular camera
	      frame = cam->grab();
	    // send new frame to this thread
	    string frame_name = cam->frame_name();
	    while (!(*ithreads)->set_data(frame, frame_name))
	      millisleep(5);
	  }
	}
	// ms = tpass.elapsed_milliseconds();
	// cout << "processing: " << ms << " ms." << endl;
	// cout << "fps: " << cam->fps() << endl;
	// sleep display
// 	if (display_sleep > 0) {
// 	  cout << "sleeping for " << display_sleep << "ms." << endl;
// 	  millisleep(display_sleep);
// 	}
	if (conf.exists("save_max") && 
	    idx_sum(total_saved) > conf.get_uint("save_max")) {
	  cout << "Reached max number of detections, exiting." << endl;
	  break ; // limit number of detection saves
	}
	// sleep a bit between each iteration
	millisleep(10);
      }
      // let's wait for all threads to finish and tell them to stop.
      for (ithreads = threads.begin(); ithreads != threads.end(); ++ithreads) {
	(*ithreads)->stop();
	// wait that it actually stops
	while (!(*ithreads)->finished())
	  millisleep(10);
      }
      cout << "Execution time: "; toverall.pretty_elapsed(); cout << endl;
      if (save_video)
	cam->stop_recording(conf.exists_bool("use_original_fps") ?
			    cam->fps() : conf.get_uint("save_video_fps"),
			    outdir.c_str());
      // free variables
      if (cam) delete cam;
      for (ithreads = threads.begin(); ithreads != threads.end(); ++ithreads)
	delete *ithreads;
      // close files
      fp.close();
      cout << "Detection finished." << endl;
#ifdef __GUI__
      quit_gui(); // close all windows
#endif
    } catch(string &err) { eblerror(err.c_str()); }
  return 0;
}
