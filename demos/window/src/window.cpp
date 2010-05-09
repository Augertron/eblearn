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
using namespace ebl; // all eblearn objects are under the ebl namespace

////////////////////////////////////////////////////////////////
// drawing and position interpolation

#ifdef __GUI__

void draw(bbox *b, rect &pos, idx<ubyte> &bgwin, idx<ubyte> &frame,
	  idx<ubyte> &tpl, configuration &conf) {
  uint control_offset = conf.get_uint("control_offset");
  uint text_hoffset = bgwin.dim(0) - conf.get_uint("text_hoffset");
  uint text_woffset = bgwin.dim(1) / 2 - conf.get_uint("text_woffset");
  uint text_height = conf.get_uint("text_height");
  disable_window_updates();
  clear_window();
  set_font_size(conf.get_int("font_size"));
  gui.draw_matrix_unsafe(bgwin);
  uint hface = bgwin.dim(0) - frame.dim(0) - control_offset;
  uint wface = bgwin.dim(1) / 2 - frame.dim(1) / 2;
  gui << at(text_hoffset - text_height * 4, text_woffset)
      << "Imagine this is a window to outside world.";
  gui << at(text_hoffset - text_height * 3, text_woffset) <<
    "Move your head down to see the sky,";
  gui << at(text_hoffset - text_height * 2, text_woffset) <<
    "up to see the ground,";
  gui << at(text_hoffset - text_height, text_woffset) <<
    "left to see right and right to see left.";
  if (b) {
    char red = 0, green = 0, blue = 0;
    if (b->class_id == -42)
      green = 255;
    else
      blue = 255;
    draw_box(hface + pos.h0, wface + pos.w0, pos.width, pos.height,
	     red, green, blue);
  }
  draw_matrix(frame, hface, wface);
  draw_matrix(tpl, "template");
  enable_window_updates();
}

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

////////////////////////////////////////////////////////////////
// main loop

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
      configuration conf(argv[1]);
      intg winszh = conf.get_int("winszh");
      intg winszw = conf.get_int("winszw");
      bool display = conf.exists_bool("window_display");
      
      // TODO: read PAM format for alpha channel
      // idx<ubyte> window = load_image<ubyte>("/home/sermanet/eblearn/pvc_window.png");
      // cout << "window: " << window << endl;

      // load background images
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
      // vision threads
      tracking_thread<float> tt(conf, argc == 3 ? argv[2] : NULL);
      tt.start();
      // input and output variables
      idx<ubyte> frame(1,1,1), tpl(1,1,1);
      vector<bbox*> bboxes;
      rect pos(0, 0, 10, 10), srcpos(0, 0, 10, 10), tgtpos(0, 0, 10, 10);
      bbox *b = NULL;
      float h = 0, w = 0;
      bool updated = false;
      bool first_time = true;
#ifdef __GUI__
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
#endif  
      // loop
      while(1) {
	try {
	  // get cam image and detected region
	  updated = tt.get_data(bboxes, frame, tpl);
	  // update target position if tracking thread ready
	  if (updated) {
	    // find bbox with max confidence
	    if (bboxes.size() > 0) {
	      double maxconf = -5.0;
	      uint maxi = 0;
	      // select bbox with maximum confidence
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
		//		detection_timer.restart();
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
	    if (display)
	      draw(b, pos, bgwin, frame, tpl, conf);
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
	} catch (string &err) {
	  cerr << err << endl;
	}
      }
    } catch(string &err) {
      cerr << err << endl;
    }
    return 0;
  }
