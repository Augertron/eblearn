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

using namespace std;

#include <deque>

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // detector_gui

  template <typename Tnet>
  detector_gui<Tnet>::
  detector_gui(bool show_detqueue_, uint step_, uint qheight_,
	       uint qwidth_, bool show_detqueue2_, uint step2_,
	       uint qheight2_, uint qwidth2_)
    : show_detqueue(show_detqueue_), show_detqueue2(show_detqueue2_),
      step(step_), step2(step2_), qheight(qheight_), qwidth(qwidth_), 
      qheight2(qheight2_), qwidth2(qwidth2_), detcnt(0) {
  }

  template <typename Tnet>
  detector_gui<Tnet>::~detector_gui() {
  }
  
  template <typename Tnet> template <typename Tin>
  vector<bbox*>& detector_gui<Tnet>::display(detector<Tnet> &cl, idx<Tin> &img,
					  double threshold, unsigned int h0,
					  unsigned int w0,
					  double dzoom,  Tnet vmin, Tnet vmax,
					  int wid, const char *wname) {
    display_wid = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector"));
    select_window(display_wid);
    // disable_window_updates();

    // run network
    vector<bbox*>& vb = cl.fprop(img, threshold);
    // draw output
    ostringstream label;
    vector<bbox*>::iterator i = vb.begin();
    for ( ; i != vb.end(); ++i) {
      uint h = (uint) (dzoom * (*i)->h0);
      uint w = (uint) (dzoom * (*i)->w0);
      label.str("");
      label.precision(2);
      label << cl.labels[(*i)->class_id].idx_ptr() << " "
	    << (*i)->confidence;
      draw_box(h0 + h, w0 + w, (uint) (dzoom * (*i)->height), 
	       (uint) (dzoom * (*i)->width), 0, 0, 255,
	       new string((const char *)label.str().c_str()));
    }
    draw_matrix(img, h0, w0, dzoom, dzoom, (Tin)vmin, (Tin)vmax);   
    // draw masks class
    if (!mask_class.empty()) {
      idx<Tnet> mask = cl.get_mask(mask_class);
      draw_mask(mask, h0, w0, dzoom, dzoom,
		255, 0, 0, 127, mask_threshold);
    }
    // enable_window_updates();
    return vb;
  }

  template <typename Tnet> template <typename Tin> 
  vector<bbox*>& detector_gui<Tnet>::
  display_input(detector<Tnet> &cl, idx<Tin> &img, double threshold,
		unsigned int h0, unsigned int w0, double dzoom, Tnet vmin,
		Tnet vmax, int wid, const char *wname) {
    display_wid = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector: output"));
    select_window(display_wid);
    disable_window_updates();

    // draw input
    draw_matrix(img, "input", h0, w0, dzoom, dzoom, (Tin) vmin, (Tin) vmax);
    w0 += (uint) (img.dim(1) * dzoom + 5);

    return display(cl, img, threshold, h0, w0, dzoom, vmin, vmax, wid, wname);
  }

  template <typename Tnet> template <typename Tin>
  vector<bbox*>& detector_gui<Tnet>::
  display_inputs_outputs(detector<Tnet> &cl, idx<Tin> &img, double threshold,
			 unsigned int h0, unsigned int w0, double dzoom,
			 Tnet vmin, Tnet vmax, int wid, const char *wname){
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
    select_window(display_wid_fprop);

    // draw input and output
    vector<bbox*>& bb =
      display_input(cl, img, threshold, h0, w0, dzoom, (Tnet)0, (Tnet)255,
		    display_wid_fprop);

    // disable_window_updates();
    // draw internal inputs and outputs
    int h = (int) (h0 + cl.height * dzoom + 5 + 15);
    int scale = 0;
    int ohmax = ((state_idx<Tnet>*) cl.outputs.get(0))->x.dim(1);
    int ihmax = ((state_idx<Tnet>*) cl.inputs.get(0))->x.dim(1);
    bool first_time = true;
    ostringstream s;
    // compute min and max of all outputs, to maximize intensity display range
    if (vmin == vmax) {
      { idx_bloop1(out, cl.outputs, void*) {
	  idx<Tnet> outx = ((state_idx<Tnet>*) out.get())->x;
	  if (first_time) {
	    vmin = idx_min(outx);
	    vmax = idx_max(outx);
	    first_time = false;
	  } else {
	    vmin = MIN(vmin, idx_min(outx));
	    vmax = std::max(vmax, idx_max(outx));	  
	  }
	}}
    }
    // display all outputs
    first_time = true;
    { idx_bloop2(in, cl.inputs, void*, out, cl.outputs, void*) {
	idx<Tnet> inx = ((state_idx<Tnet>*) in.get())->x;
	//inx = inx.select(0, 0);
	inx = inx.shift_dim(0, 2);
	idx<Tnet> outx = ((state_idx<Tnet>*) out.get())->x;

	// draw inputs
	gui << black_on_white() << at(h - 15, w0) << "scale #" << scale
	    << " " << inx.dim(0) << "x" << inx.dim(1);
	draw_matrix(inx, h, w0, dzoom, dzoom, (Tnet)vmin, (Tnet)vmax);
	// draw bboxes on scaled input
	for (vector<bbox*>::iterator i = bb.begin(); i != bb.end(); ++i) {
	  if (scale == (*i)->scale_index)
	    draw_box((uint) (h + dzoom * (*i)->ih0), 
		     (uint) (w0 + dzoom * (*i)->iw0),
		     (uint) (dzoom * (*i)->ih), 
		     (uint) (dzoom * (*i)->iw), 0, 0, 255,
		     new string((const char*)
				cl.labels[(*i)->class_id].idx_ptr()));
	}
	// draw outputs
	int hcat = 0;
	double czoom = dzoom * 2.0;
	int lab = 0;
	{ idx_bloop1(category, outx, Tnet) {
	    s.str("");
	    if (first_time)
	      s << cl.labels[lab].idx_ptr() << " ";
	    s << category.dim(0) << "x" << category.dim(1);
	    gui << at((uint) (h + ihmax * dzoom + 5 + hcat), 
		      (uint) (w0 + category.dim(1) * czoom + 2));
	    gui << black_on_white() << s.str();
	    draw_matrix(category, (uint) (h + ihmax * dzoom + 5 + hcat), w0, 
			czoom, czoom, vmin, vmax);
	    hcat += (int) (ohmax * czoom + 2);
	    lab++;
	  }}

	scale++;
	w0 += (uint) (inx.dim(1) * dzoom + 5);
	first_time = false;
      }}

    // display queues of detections
    if (show_detqueue || show_detqueue2) {
      uint hh0 = h0;
      vector<idx<Tnet> > &new_detections = cl.get_originals();
      if (show_detqueue)
	update_and_display_queue(detqueue, step, qheight, qwidth,
				 new_detections, detcnt, hh0, w0, dzoom);
      if (show_detqueue2)
	update_and_display_queue(detqueue2, step2, qheight2, qwidth2,
				 new_detections, detcnt, hh0, w0, dzoom);
      detcnt += new_detections.size();
    }
    // reactive window drawing
    // enable_window_updates();
    return bb;
  }

  template <typename Tnet> void detector_gui<Tnet>::
  update_and_display_queue(deque<idx<Tnet> > &queue, uint step, uint qheight,
			   uint qwidth, vector<idx<Tnet> > &new_detections,
			   uint detcnt, uint &h0, uint &w0, double dzoom) {
    // update queue
    uint queuesz = qheight * qwidth;
    // loop over all new detections and add new ones based on the step
    for (typename vector<idx<Tnet> >::iterator i = new_detections.begin();
	 i != new_detections.end(); ++i, detcnt++) {
      if (!(detcnt % std::max((uint) 1, step))) { // add when multiple of step
	if ((queue.size() >= queuesz) && (queue.size() > 0))
	  queue.pop_front();
	queue.push_back(*i);
      }
    }
    // display queue
    uint w = 0, wn = 0, h = 0;
    intg hmax = 0, wmax = 0;
    h = h0;
    for (typename deque<idx<Tnet> >::iterator i = queue.begin();
	 i != queue.end(); ++i) {
      draw_matrix(*i, h, w0 + w, dzoom, dzoom, (Tnet)0, (Tnet)255);
      w += i->dim(1) + 2;
      wn++;
      hmax = std::max(hmax, i->dim(0));
      wmax = std::max(wmax, (intg) w);
      if (wn >= qwidth) {
	wn = 0;
	w = 0;
	h += hmax + 2;
	hmax = 0;
      }
    }
    // update h0 and w0
    h0 += hmax;
    w0 += wmax;
  }

  template <typename Tnet> template <typename Tin>
  vector<bbox*>& detector_gui<Tnet>::
  display_all(detector<Tnet> &cl, idx<Tin> &img, double threshold,
	      unsigned int h0, unsigned int w0, double dzoom, Tnet vmin,
	      Tnet vmax, int wid, const char *wname) {
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
    select_window(display_wid_fprop);

    // draw input and output
    vector<bbox*>& bb =
      display_inputs_outputs(cl, img, threshold, h0, w0, dzoom, vmin, vmax,
			     display_wid_fprop);

    // disable_window_updates();
    // draw internal states of first scale
    w0 = (cl.width + 5) * 2 + 5;
    state_idx<Tnet> *ii =
      ((state_idx<Tnet>*) cl.inputs.get(cl.inputs.dim(0) - 1));
    state_idx<Tnet> *oo =
      ((state_idx<Tnet>*) cl.outputs.get(cl.inputs.dim(0) - 1));
    module_1_1_gui mg;
    //    cl.thenet.fprop(*ii, *oo); 
    mg.display_fprop(*((module_1_1<Tnet>*) cl.nets.get(cl.nets.dim(0) - 1)),
		     *ii, *oo, h0, w0,
		     1.0, vmin, vmax, true, display_wid_fprop);
    enable_window_updates();
    return bb;
  }

  template <typename Tnet> template <typename Tin>
  void detector_gui<Tnet>::display_current(detector<Tnet> &cl, 
					   idx<Tin> &sample,
					   int wid, const char *wname){
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
    select_window(display_wid_fprop);
    disable_window_updates();

    // draw internal states of first scale
    module_1_1_gui mg;
    state_idx<Tnet> *ii = ((state_idx<Tnet>*) cl.inputs.get(0));
    state_idx<Tnet> *oo = ((state_idx<Tnet>*) cl.outputs.get(0));
    //cl.thenet.fprop(*ii, *oo); 
    mg.display_fprop(*(module_1_1<Tnet>*) cl.nets.get(cl.nets.dim(0) - 1),
		     *ii, *oo, 0, 0, 1.0, -1.0, 1.0, true,
		     display_wid_fprop);
    // enable_window_updates();
  }

  template <typename Tnet>
  void detector_gui<Tnet>::set_mask_class(const char *name, Tnet threshold) {
    if (name) {
      mask_class = name;
      mask_threshold = threshold;
    }
  }
  
} // end namespace ebl

#endif
