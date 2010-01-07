/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef DETECTOR_GUI_HPP_
#define DETECTOR_GUI_HPP_

//#include "detector_gui.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // detector_gui

  template <class Tdata, class T>
  void detector_gui::display(detector<Tdata> &cl, idx<T> &img,
			     double threshold, unsigned int h0, unsigned int w0,
			     double dzoom,  Tdata vmin, Tdata vmax,
			     int wid, const char *wname){
    display_wid = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector: output"));
    select_window(display_wid);
    disable_window_updates();

    // draw input
    draw_matrix(img, "input", h0, w0, dzoom, dzoom, (T) vmin, (T) vmax);
    vector<bbox> vb = cl.fprop(img, threshold);
    w0 += img.dim(1) + 5;

    // draw output
    vector<bbox>::iterator i = vb.begin();
    for ( ; i != vb.end(); ++i) {
      unsigned int h = dzoom * i->h0;
      unsigned int w = dzoom * i->w0;
      draw_box(h0 + h, w0 + w, dzoom * i->height, dzoom * i->width, 255, 0, 0,
	       new string(cl.labels.get(i->class_id)));
    }
    draw_matrix(cl.grabbed, h0, w0, dzoom, dzoom, (Tdata)vmin, (Tdata)vmax);   
    enable_window_updates();
  }

  template <class Tdata, class T>
  void detector_gui::display_inputs_outputs(detector<Tdata> &cl, 
					    idx<T> &img, double threshold,
					    unsigned int h0, unsigned int w0,
					    double dzoom, Tdata vmin,
					    Tdata vmax, int wid, 
					    const char *wname){
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
    select_window(display_wid_fprop);
    disable_window_updates();

    // draw input and output
    display(cl, img, threshold, h0, w0, dzoom, vmin, vmax, display_wid_fprop);

    // draw internal inputs and outputs
    int h = h0 + cl.height + 5;
    int scale = 0;
    int ohmax = ((state_idx<Tdata>*) cl.outputs.get(0))->x.dim(1);
    int ihmax = ((state_idx<Tdata>*) cl.inputs.get(0))->x.dim(1);
    bool first_time = true;
    ostringstream s;
    // compute min and max of all outputs, to maximize intensity display range
    if (vmin == vmax) {
      { idx_bloop1(out, cl.outputs, void*) {
	  idx<Tdata> outx = ((state_idx<Tdata>*) out.get())->x;
	  if (first_time) {
	    vmin = idx_min(outx);
	    vmax = idx_max(outx);
	    first_time = false;
	  } else {
	    vmin = MIN(vmin, idx_min(outx));
	    vmax = MAX(vmax, idx_max(outx));	  
	  }
	}}
    }
    // display all outputs
    first_time = true;
    { idx_bloop2(in, cl.inputs, void*, out, cl.outputs, void*) {
	idx<Tdata> inx = ((state_idx<Tdata>*) in.get())->x;
	inx = inx.select(0, 0);
	idx<Tdata> outx = ((state_idx<Tdata>*) out.get())->x;

	// draw inputs
	s.str("");
	s << "scale #" << scale << " " << inx.dim(0) << "x" << inx.dim(1);
	draw_matrix(inx, s.str().c_str(), h, w0, dzoom, dzoom, vmin, vmax);

	// draw outputs
	int hcat = 0;
	double czoom = dzoom * 7.0;
	int lab = 0;
	{ idx_bloop1(category, outx, Tdata) {
	    s.str("");
	    if (first_time)
	      s << cl.labels.get(lab) << " ";
	    s << category.dim(0) << "x" << category.dim(1);
	    gui << at(h + ihmax + 5 + hcat, 
		      w0 + category.dim(1) * czoom + 2);
	    gui << s.str();
	    draw_matrix(category, h + ihmax + 5 + hcat, w0, 
			czoom, czoom, vmin, vmax);
	    hcat += ohmax * czoom + 2;
	    lab++;
	  }}

	scale++;
	w0 += inx.dim(1) + 5;
	first_time = false;
      }}
    enable_window_updates();
  }

  template <class Tdata, class T>
  void detector_gui::display_all(detector<Tdata> &cl, idx<T> &img, 
				 double threshold,
				 unsigned int h0, unsigned int w0,
				 double dzoom, Tdata vmin, Tdata vmax,
				 int wid, const char *wname){
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
    select_window(display_wid_fprop);
    disable_window_updates();

    // draw input and output
    display_inputs_outputs(cl, img, threshold, h0, w0, dzoom, vmin, vmax,
			   display_wid_fprop);

    // draw internal states of first scale
    w0 = (cl.width + 5) * 2 + 5;
    state_idx<Tdata> *ii = ((state_idx<Tdata>*) cl.inputs.get(0));
    state_idx<Tdata> *oo = ((state_idx<Tdata>*) cl.outputs.get(0));
    module_1_1_gui mg;
    //    cl.thenet.fprop(*ii, *oo); 
    mg.display_fprop(cl.thenet, *ii, *oo, h0, w0, 1.0, vmin, vmax,
		     true, display_wid_fprop);
  }

  template <class Tdata, class T>
  void detector_gui::display_current(detector<Tdata> &cl, 
				     idx<T> &sample,
				     int wid, const char *wname){
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
    select_window(display_wid_fprop);
    disable_window_updates();

    // draw internal states of first scale
    module_1_1_gui mg;
    state_idx<Tdata> *ii = ((state_idx<Tdata>*) cl.inputs.get(0));
    state_idx<Tdata> *oo = ((state_idx<Tdata>*) cl.outputs.get(0));
    cl.thenet.fprop(*ii, *oo); 
    mg.display_fprop(cl.thenet, *ii, *oo, 0, 0, 1.0, -1.0, 1.0,
		     true, display_wid_fprop);
    enable_window_updates();
  }

} // end namespace ebl

#endif

