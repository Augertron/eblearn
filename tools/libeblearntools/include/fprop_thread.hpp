/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
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

#ifndef FPROP_THREAD_HPP_
#define FPROP_THREAD_HPP_

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>

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
  fprop_thread<Tnet>::fprop_thread(configuration &conf_,
				   mutex *om,
				   const char *name_,
				   const char *arg2_, bool sync,
				   t_chans tc)
    : thread(om, name_, sync), conf(conf_), arg2(arg2_), frame(120, 160, 1),
      mutex_in(), mutex_out(),
      in_updated(false), out_updated(false), bavailable(false),
      frame_name(""), frame_id(0), outdir(""), color_space(tc),
      bload_image(false), pdetect(NULL) {
  }

  template <typename Tnet>
  fprop_thread<Tnet>::~fprop_thread() {
  }

  template <typename Tnet>
  void fprop_thread<Tnet>::set_out_updated() {
    // lock data
    mutex_out.lock();
    // set flag
    out_updated = true;
    // unlock data
    mutex_out.unlock();
  }
 
  template <typename Tnet>
  bool fprop_thread<Tnet>::get_data(idx<ubyte> &frame2,
				    string &frame_name_,
				    uint &id) {
    // lock data
    mutex_out.lock();
    // only read data if it has been updated
    if (!out_updated) {
      // unlock data
      mutex_out.unlock();
      return false;
    }
    // check frame is correctly allocated, if not, allocate.
    if (frame2.order() != uframe.order()) 
      frame2 = idx<ubyte>(uframe.get_idxdim());
    else if (frame2.get_idxdim() != uframe.get_idxdim())
      frame2.resize(uframe.get_idxdim());
    // copy frame
    idx_copy(uframe, frame2);    
    // set frame name
    frame_name_ = frame_name;
    // set frame id
    id = frame_id;
    // reset updated flag
    out_updated = false;
    // declare thread as available
    bavailable = true;
    // unlock data
    mutex_out.unlock();
    // confirm that we copied data.
    return true;
  }

  template <typename Tnet>
  bool fprop_thread<Tnet>::set_data(idx<ubyte> &frame2, string &name,
				    uint id) {
    // lock data (non blocking)
    if (!mutex_in.trylock())
      return false;
    // check frame is correctly allocated, if not, allocate.
    if (frame2.order() != uframe.order())
      uframe = idx<ubyte>(frame2.get_idxdim());
    else if (frame2.get_idxdim() != uframe.get_idxdim())
      uframe.resize(frame2.get_idxdim());
    // do not load image, use passed frame instead
    bload_image = false;
    // copy frame
    idx_copy(frame2, uframe);
    // copy name
    frame_name = name;
    // copy frame_id
    frame_id = id;
    // reset updated flag
    in_updated = true;
    // unlock data
    mutex_in.unlock();
    // declare thread as not available
    bavailable = false;
    // confirm that we copied data.
    return true;
  }

  template <typename Tnet>
  bool fprop_thread<Tnet>::set_dump(string &frame_fname_,
				    string &dump_fname) {
    // lock data (non blocking)
    if (!mutex_in.trylock())
      return false;
    // set the dump name in detector
    if (!pdetect)
      eblerror("pdetect shoud be allocated");
    pdetect->set_outputs_dumping(dump_fname.c_str());
    // image file to load and process
    frame_name = frame_fname_;
    bload_image = true;
    // reset updated flag
    in_updated = true;
    // unlock data
    mutex_in.unlock();
    // declare thread as not available
    bavailable = false;
    // confirm that we copied data.
    return true;
  }

  template <typename Tnet>
  bool fprop_thread<Tnet>::dumped() {
    // lock data
    mutex_out.lock();
    // only read data if it has been updated
    if (!out_updated) {
      // unlock data
      mutex_out.unlock();
      return false;
    }
    // reset updated flag
    out_updated = false;
    // declare thread as available
    bavailable = true;
    // unlock data
    mutex_out.unlock();
    // confirm that we copied data.
    return true;
  }

  template <typename Tnet>
  bool fprop_thread<Tnet>::available() {
    return bavailable;
  }
  
  template <typename Tnet>
  void fprop_thread<Tnet>::set_output_directory(string &out) {
    outdir = out;
  }

  template <typename Tnet>
  void fprop_thread<Tnet>::execute() { 
   try {
     // configuration
     bool       silent         = conf.exists_true("silent");
     bool	display	       = false;
     bool	mindisplay     = conf.exists_true("minimal_display");
     bool	display_states = conf.exists_true("display_states");
     uint	display_sleep  = conf.try_get_uint("display_sleep", 0);
     uint       wid	       = 0;	// window id
     uint       wid_states     = 0;	// window id
     display = conf.exists_true("display_threads");
     // load network and weights in a forward-only parameter
     parameter<FPROP_SFUNC(Tnet)> theparam;
     idx<ubyte> classes;
     vector<string> sclasses;
     answer_module<SFUNC2(Tnet)> *ans = NULL;
     uint noutputs = 1;
     intg thick = -1;
     if (conf.exists("classes")) {
       classes = load_matrix<ubyte>(conf.get_cstring("classes"));
       sclasses = ubyteidx_to_stringvector(classes);
       ans = create_answer<SFUNC2(Tnet)>(conf, classes.dim(0));
       noutputs = ans->get_nfeatures();
     }     
     module_1_1<FPROP_SFUNC(Tnet)> *net =
       create_network<FPROP_SFUNC(Tnet)>(theparam, conf, thick, noutputs);
     // loading weights
     if (!conf.exists("weights")) { // manual weights
       merr << "warning: \"weights\" variable not defined, loading manually "
	    << "if manual_load defined" << endl;
       if (conf.exists_true("manual_load"))
	 manually_load_network(*((layers<FPROP_SFUNC(Tnet)>*)net), conf);
     } else { // multiple-file weights
       // concatenate weights if multiple ones
       vector<string> w =
	 string_to_stringvector(conf.get_string("weights"));
       mout << "Loading weights from: " << w << endl;
       theparam.load_x(w);
     }
     // detector
     detector<SFUNC(Tnet)> detect(*net, sclasses, ans, NULL, NULL,
					mout, merr);
     detection_thread<Tnet>::init_detector(detect, conf, outdir);
     pdetect = &detect;
     // gui
#ifdef __GUI__
     Tnet display_min = (Tnet) conf.try_get_double("display_min", -1.7);
     Tnet display_max = (Tnet) conf.try_get_double("display_max", 1.7);
     Tnet display_in_max = (Tnet) conf.try_get_double("display_in_max", 255);
     Tnet display_in_min = (Tnet) conf.try_get_double("display_in_min", 0);
     float zoom = conf.try_get_float("display_zoom", 1);
     module_1_1_gui	netgui;
     wid_states  = display_states ? new_window("network states"):0;
     night_mode();
     string title = "eblearn fprop: ";
     title += _name;
     if (display) {
       wid = new_window(title.c_str());
       mout << "displaying in window " << wid << endl;
       night_mode();
     }
     detector_gui<FPROP_SFUNC(Tnet)> dgui;
#endif  
     // timing variables
     timer tpass, toverall;
     long ms;
     // loop
     toverall.start();
     // we're ready
     bavailable = true;
     while(!this->_stop) {
       tpass.restart();
       // wait until a new image is made available
       while (!in_updated && !_stop) {
	 millisleep(1);
       }
       if (_stop) break ;
       // we got a new frame, reset new frame flag
       in_updated = false; // no need to lock mutex
       if (!silent) mout << "processing " << frame_name;
       // prepare image
       if (bload_image) { // load image directly
	 frame = load_image<Tnet>(frame_name);
       } else { // used passed image
	 // check frame is correctly allocated, if not, allocate.
	 if (frame.order() != uframe.order())
	   frame = idx<Tnet>(uframe.get_idxdim());
	 else if (frame.get_idxdim() != uframe.get_idxdim())
	   frame.resize(uframe.get_idxdim());
	 // copy frame
	 idx_copy(uframe, frame);
       }
       if (!silent) mout << " (min: " << idx_min(frame)
			 << ", max: " << idx_max(frame) << ")" << endl;

       // run detector
       if (!display) { // fprop without display
	 detect.fprop(frame, frame_name.c_str());
       }
#ifdef __GUI__
       else { // fprop and display
	 disable_window_updates();
	 select_window(wid);
	 clear_window();
	 string title = _name;
	 title << ": " << frame_name;
	 set_window_title(title.c_str());
	 //	 clear_resize_window();
	 if (mindisplay) {
	   dgui.display(detect, frame, frame_name.c_str(),
			0, 0, zoom, display_min, display_max,
			wid, _name.c_str());
	 } else {
	   dgui.display_inputs_outputs(detect, frame,
				       frame_name.c_str(), 0, 0, zoom,
				       display_min, display_max, wid,
				       _name.c_str(),
				       display_in_min, display_in_max);
	 }
	 enable_window_updates();
       }
       if (display_states) {
	 dgui.display_current(detect, frame, wid_states, NULL, zoom);
	 select_window(wid);
       }
       // if (save_video && display) {
       // 	 string fname = viddir;
       // 	 fname += frame_name;
       // 	 save_window(fname.c_str());
       // 	 if (!silent) mout << "saved " << fname << endl;
       // }
#endif
       ms = tpass.elapsed_milliseconds();
       if (!silent) {
	 mout << "processing=" << ms << " ms ("
	      << tpass.elapsed() << ")" << endl;
       }
#ifdef __DEBUGMEM__
       pretty_memory();
#endif
       // switch 'updated' flag on to warn we just added new data
       set_out_updated();
       // display sleep
       if (display_sleep > 0) {
	 mout << "sleeping for " << display_sleep << "ms." << endl;
	 millisleep(display_sleep);
       }
     }
     mout << "fprop finished. Execution time: " << toverall.elapsed()
	  << endl;
     // free variables
     if (net) delete net;
   } catch(string &err) { eblerror(err.c_str()); }
  }

} // end namespace ebl

#endif /* FPROP_THREAD_HPP_ */
