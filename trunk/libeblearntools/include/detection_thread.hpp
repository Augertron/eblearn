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

#ifndef DETECTION_THREAD_HPP_
#define DETECTION_THREAD_HPP_

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

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // detection thread

  template <typename Tnet>
  detection_thread<Tnet>::detection_thread(configuration &conf_,
					   const char *arg2_)
    : conf(conf_), arg2(arg2_), frame(120, 160, 1), mutex_in(), mutex_out(),
      in_updated(false), out_updated(false) {
  }

  template <typename Tnet>
  detection_thread<Tnet>::~detection_thread() {
  }

  template <typename Tnet>
  void detection_thread<Tnet>::copy_bboxes(vector<bbox*> &bb) {
    // lock data
    pthread_mutex_lock(&mutex_out);
    // clear bboxes
    bboxes.clear();
    // copy bboxes
    for (ibox = bb.begin(); ibox != bb.end(); ++ibox) {
      bboxes.push_back(new bbox(**ibox));
    }
    // unlock data
    pthread_mutex_unlock(&mutex_out);
  }

  template <typename Tnet>
  void detection_thread<Tnet>::set_out_updated() {
    // lock data
    pthread_mutex_lock(&mutex_out);
    // set flag
    out_updated = true;
    // unlock data
    pthread_mutex_unlock(&mutex_out);
  }

  template <typename Tnet>
  bool detection_thread<Tnet>::get_data(vector<bbox*> &bboxes2,
					idx<ubyte> &frame2) {
    // lock data
    pthread_mutex_lock(&mutex_out);
    // only read data if it has been updated
    if (!out_updated) {
      // unlock data
      pthread_mutex_unlock(&mutex_out);
      return false;
    }
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
    // check frame is correctly allocated, if not, allocate.
    if (frame2.order() != uframe.order()) 
      frame2 = idx<ubyte>(uframe.get_idxdim());
    else if (frame2.get_idxdim() != uframe.get_idxdim())
      frame2.resize(uframe.get_idxdim());
    // copy frame
    idx_copy(uframe, frame2);    
    // reset updated flag
    out_updated = false; 
    // unlock data
    pthread_mutex_unlock(&mutex_out);
    // confirm that we copied data.
    return true;
  }

  template <typename Tnet>
  bool detection_thread<Tnet>::set_data(idx<ubyte> &frame2) {
    // lock data (non blocking)
    if (!pthread_mutex_trylock(&mutex_in))
      return false;
    // check frame is correctly allocated, if not, allocate.
    if (frame2.order() != uframe.order())
      uframe = idx<ubyte>(frame2.get_idxdim());
    else if (frame2.get_idxdim() != uframe.get_idxdim())
      uframe.resize(frame2.get_idxdim());
    // copy frame
    idx_copy(frame2, uframe);
    // reset updated flag
    in_updated = true;
    // unlock data
    pthread_mutex_unlock(&mutex_in);
    // confirm that we copied data.
    return true;
  }

  template <typename Tnet>
  void detection_thread<Tnet>::execute() { 
   try {
     // configuration
     bool	color	      = conf.exists_bool("color");
     uint	norm_size     = conf.get_uint("normalization_size");
     Tnet	threshold     = (Tnet) conf.get_double("threshold");
     bool	display       = conf.exists_bool("net_display");
     bool	mindisplay    = conf.exists_bool("minimal_display");
     uint	display_sleep = conf.get_uint("display_sleep");

     // load network and weights
     parameter<Tnet> theparam;
     idx<ubyte> classes(1,1);
     try { // try loading classes names but do not stop upon failure
       load_matrix<ubyte>(classes, conf.get_cstring("classes"));
     } catch(string &err) { cerr << "warning: " << err << endl; }
     cout << "loading weights from " << conf.get_cstring("weights") << endl;
     module_1_1<Tnet> *net = create_network(theparam, conf, classes.dim(0));
     theparam.load_x(conf.get_cstring("weights"));

     // select preprocessing  
     string        cam_type        = conf.get_string("camera");
     module_1_1<Tnet>* pp = NULL;
     if (!strcmp(cam_type.c_str(), "v4l2")) // Y -> Yp
       pp = new weighted_std_module<Tnet>(norm_size, norm_size, 1, true,
					  false, true);
     else if (color) // RGB -> YpUV
       pp = (module_1_1<Tnet>*) new rgb_to_ypuv_module<Tnet>(norm_size);
     else // RGB -> Yp
       pp = (module_1_1<Tnet>*) new rgb_to_yp_module<Tnet>(norm_size);
    
     // detector
     detector<Tnet> detect(*net, classes, pp, norm_size, NULL, 0,
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
       detect.set_save("detections");
       if (conf.exists("save_max_per_frame"))
	 detect.set_save_max_per_frame(conf.get_uint("save_max_per_frame"));
     }

     // gui
#ifdef __GUI__
     uint qstep1 = 0, qheight1 = 0, qwidth1 = 0,
       qheight2 = 0, qwidth2 = 0, qstep2 = 0;
     if (conf.exists_bool("queue1")) {
       qstep1 = conf.get_uint("qstep1");
       qheight1 = conf.get_uint("qheight1");
       qwidth1 = conf.get_uint("qwidth1");
     }
     if (conf.exists_bool("queue2")) {
       qstep2 = conf.get_uint("qstep2");
       qheight2 = conf.get_uint("qheight2");
       qwidth2 = conf.get_uint("qwidth2");
     }
     module_1_1_gui	netgui;
     uint		wid  = display ?
       new_window("eblearn object recognition") : 0;
     float		zoom = 1;
     detector_gui<Tnet> dgui(conf.exists_bool("queue1"), qstep1, qheight1,
			     qwidth1, conf.exists_bool("queue2"), qstep2,
			     qheight2, qwidth2);
     if (bmask_class)
       dgui.set_mask_class(conf.get_cstring("mask_class"),
			   (Tnet) conf.get_double("mask_threshold"));
     // timing variables
     QTime t0;
     int tpp;
#endif  
     // loop
     while(1) {
#ifdef __GUI__
       t0.start();
#endif
       // wait until a new image is made available
       while (!in_updated) {
	 usleep(1000);
       }
       // we got a new frame, reset new frame flag
       in_updated = false; // no need to lock mutex
       // check frame is correctly allocated, if not, allocate.
       if (frame.order() != uframe.order()) 
	 frame = idx<Tnet>(uframe.get_idxdim());
       else if (frame.get_idxdim() != uframe.get_idxdim())
	 frame.resize(uframe.get_idxdim());
       // copy frame
       idx_copy(uframe, frame);
       // run detector
       if (!display) { // fprop without display
	 vector<bbox*> &bb = detect.fprop(frame, threshold);
	 copy_bboxes(bb); // make a copy of bounding boxes
       }
#ifdef __GUI__
       else { // fprop and display
	 disable_window_updates();
	 clear_window();
	 if (mindisplay) {
	   vector<bbox*> &bb =
	     dgui.display(detect, frame, threshold, 0, 0, zoom,
			  (Tnet)0, (Tnet)255, wid);
	   copy_bboxes(bb); // make a copy of bounding boxes
	 }
	 else
	   dgui.display_inputs_outputs(detect, frame, threshold, 0, 0, zoom,
				       (Tnet)-1.1, (Tnet)1.1, wid); 
	 enable_window_updates();
       }
       tpp = t0.elapsed(); // stop processing timer
       cout << "processing: " << tpp << " ms." << endl;
#endif
       // switch 'updated' flag on to warn we just added new data
       set_out_updated();
       if (display_sleep > 0) {
	 cout << "sleeping for " << display_sleep << "ms." << endl;
	 usleep(display_sleep);
       }
       if (conf.exists("save_max") && 
	   detect.get_total_saved() > conf.get_uint("save_max"))
	 break ; // limit number of detection saves
     }
     // free variables
     if (net) delete net;
     if (pp) delete pp;
   } catch(string &err) { eblerror(err.c_str()); }
  }

} // end namespace ebl

#endif /* DETECTION_THREAD_HPP_ */
