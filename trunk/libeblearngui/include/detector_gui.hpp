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

  template <class Tdata>
  void detector_gui::display(detector<Tdata> &cl, 
				 idx<Tdata> &img, float zoom, 
				 double threshold, int objsize,
				 unsigned int h0, unsigned int w0,
				 double dzoom, int wid, const char *wname){
    display_wid = (wid >= 0) ? wid : 
      gui.new_window((wname ? wname : "detector: output"));
    gui.select_window(display_wid);
    gui.disable_updates();

    // draw input
    gui.draw_matrix(img, "input", h0, w0, dzoom, dzoom);
    vector<bbox> vb = cl.fprop(img, threshold);
    w0 += img.dim(1) + 5;

    // draw output
    //    int nj = ((state_idx*) cl.inputs.get(0))->x.dim(2);

    vector<bbox>::iterator i = vb.begin();
    for ( ; i != vb.end(); ++i) {
      unsigned int h = zoom * i->h0;
      unsigned int w = zoom * i->w0;
      draw_box(h0 + h, w0 + w, i->height, i->width, 255, 0, 0,
	       new string(cl.labels.get(i->class_id)));
    }
    gui.draw_matrix(cl.grabbed, h0, w0);
       
//     { idx_bloop1(re, res, double) {
// // 	unsigned int h = zoom * (re.get(2) - (0.5 * re.get(4)));
// // 	unsigned int w = zoom * (nj - re.get(3) - (0.5 * re.get(5)));
// 	unsigned int h = zoom * re.get(3);
// 	unsigned int w = zoom * re.get(4);
// 	gui << at(h0 + h + 1, w0 + w + 1) << cl.labels.get((re.get(0)));
//       }}

    gui.enable_updates();
  }

  template <class Tdata>
  void detector_gui::display_inputs_outputs(detector<Tdata> &cl, 
						idx<Tdata> &img, 
						float zoom, 
						double threshold, int objsize,
						unsigned int h0,
						unsigned int w0,
						double dzoom, int wid, 
						const char *wname){
    display_wid_fprop = (wid >= 0) ? wid : 
      gui.new_window((wname ? wname : 
		      "detector: inputs, outputs & internals"));
    gui.select_window(display_wid_fprop);
    gui.disable_updates();

    // draw input and output
    display(cl, img, zoom, threshold, objsize, h0, w0, dzoom,
	    display_wid_fprop);

    // draw internal inputs and outputs
    int h = h0 + cl.height + 5;
    int scale = 0;
    int ohmax = ((state_idx*) cl.outputs.get(0))->x.dim(1);
    int ihmax = ((state_idx*) cl.inputs.get(0))->x.dim(1);
    bool first_time = true;
    ostringstream s;
    double vmin = 0;
    double vmax = 0;
    // compute min and max of all outputs, to maximize intensity display range
    { idx_bloop1(out, cl.outputs, void*) {
	idx<double> outx = ((state_idx*) out.get())->x;
	if (first_time) {
	  vmin = idx_min(outx);
	  vmax = idx_max(outx);
	first_time = false;
	} else {
	  vmin = MIN(vmin, idx_min(outx));
	  vmax = MAX(vmax, idx_max(outx));	  
	}
      }}
    cout << "outputs min: " << vmin << " max: " << vmax << endl;
    vmin = 0;
    // display all outputs
    first_time = true;
    { idx_bloop2(in, cl.inputs, void*, out, cl.outputs, void*) {
	idx<double> inx = ((state_idx*) in.get())->x;
	inx = inx.select(0, 0);
	idx<double> outx = ((state_idx*) out.get())->x;

	// draw inputs
	s.str("");
	s << "scale #" << scale << " " << inx.dim(0) << "x" << inx.dim(1);
	gui.draw_matrix(inx, s.str().c_str(), h, w0);

	// draw outputs
	int hcat = 0;
	double czoom = 7.0;
	int lab = 0;
	{ idx_bloop1(category, outx, double) {
	    s.str("");
	    if (first_time)
	      s << cl.labels.get(lab) << " ";
	    s << category.dim(0) << "x" << category.dim(1);
	    gui << at(h + ihmax + 5 + hcat, 
		      w0 + category.dim(1) * czoom + 2);
	    gui << s.str();
	    gui.draw_matrix(category, h + ihmax + 5 + hcat, w0, 
			    czoom, czoom, vmin, vmax);
	    hcat += ohmax * czoom + 2;
	    lab++;
	  }}

	scale++;
	w0 += inx.dim(1) + 5;
	first_time = false;
      }}
    gui.enable_updates();
  }

  template <class Tdata>
  void detector_gui::display_all(detector<Tdata> &cl, 
				     idx<Tdata> &img, 
				     float zoom, 
				     double threshold, int objsize,
				     unsigned int h0, unsigned int w0,
				     double dzoom, int wid, const char *wname){
    display_wid_fprop = (wid >= 0) ? wid : 
      gui.new_window((wname ?
		      wname : "detector: inputs, outputs & internals"));
    gui.select_window(display_wid_fprop);
    gui.disable_updates();

    // draw input and output
    display_inputs_outputs(cl, img, zoom, threshold, objsize, h0, w0, dzoom, 
			   display_wid_fprop);

    // draw internal states of first scale
    w0 = (cl.width + 5) * 2 + 5;
    state_idx *ii = ((state_idx*) cl.inputs.get(0));
    state_idx *oo = ((state_idx*) cl.outputs.get(0));
    module_1_1_gui mg;
    //    cl.thenet.fprop(*ii, *oo); 
    mg.display_fprop(cl.thenet, *ii, *oo, h0, w0, 1.0, true, display_wid_fprop);
  }

  template <class Tdata>
  void detector_gui::display_current(detector<Tdata> &cl, 
					 idx<Tdata> &sample,
					 int wid, const char *wname){
    display_wid_fprop = (wid >= 0) ? wid : 
      gui.new_window((wname ?
		      wname : "detector: inputs, outputs & internals"));
    gui.select_window(display_wid_fprop);
    gui.disable_updates();

    // draw internal states of first scale
    module_1_1_gui mg;
    state_idx *ii = ((state_idx*) cl.inputs.get(0));
    state_idx *oo = ((state_idx*) cl.outputs.get(0));
    cl.thenet.fprop(*ii, *oo); 
    mg.display_fprop(cl.thenet, *ii, *oo, 0, 0, 1.0, true, display_wid_fprop);
    gui.enable_updates();
  }

} // end namespace ebl

#endif

