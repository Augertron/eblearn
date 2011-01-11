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

  template <typename T, class Tstate>
  detector_gui<T,Tstate>::
  detector_gui(bool show_detqueue_, uint step_, uint qheight_,
	       uint qwidth_, bool show_detqueue2_, uint step2_,
	       uint qheight2_, uint qwidth2_)
    : show_detqueue(show_detqueue_), show_detqueue2(show_detqueue2_),
      step(step_), step2(step2_), qheight(qheight_), qwidth(qwidth_), 
      qheight2(qheight2_), qwidth2(qwidth2_), detcnt(0) {
  }

  template <typename T, class Tstate>
  detector_gui<T,Tstate>::~detector_gui() {
  }
  
  template <typename T, class Tstate> template <typename Tin>
  vector<bbox*>& detector_gui<T,Tstate>::
  display(detector<T,Tstate> &cl, idx<Tin> &img, double threshold,
	  const char *frame_name, unsigned int h0, unsigned int w0,
	  double dzoom,  T vmin, T vmax, int wid, const char *wname,
	  float transparency) {
    display_wid = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector"));
    select_window(display_wid);

    // run network
    vector<bbox*>& vb = cl.fprop(img, threshold, frame_name);
    display_minimal(img, vb, cl.labels, h0, w0, dzoom, vmin, vmax, display_wid,
		    false, transparency);
    // draw masks class
    if (!mask_class.empty()) {
      idx<T> mask = cl.get_mask(mask_class);
      draw_mask(mask, h0, w0, dzoom, dzoom,
		255, 0, 0, 127, mask_threshold);
    }
    return vb;
  }

  template <typename T, class Tstate> template <typename Tin>
  void detector_gui<T,Tstate>::
  display_minimal(idx<Tin> &img, vector<bbox*>& vb, idx<ubyte> &labels,
		  unsigned int h0, unsigned int w0,
		  double dzoom,  T vmin, T vmax, int wid, bool show_parts,
		  float transparency) {
    // draw image
    draw_matrix(img, h0, w0, dzoom, dzoom, (Tin)vmin, (Tin)vmax);   
    // draw bboxes (in reverse order to display best on top)
    bbox *bb = NULL;
    for (int i = vb.size() - 1; i >= 0; --i) {
      bb = vb[(uint) i];
      // draw parts
      if (show_parts && dynamic_cast<bbox_parts*>(bb))
	draw_bbox_parts(*((bbox_parts*) bb), labels, h0, w0, dzoom);
      // draw box
      draw_bbox(*bb, labels, h0, w0, dzoom, transparency);
    }
  }

  template <typename T, class Tstate> template <typename Tin> 
  vector<bbox*>& detector_gui<T,Tstate>::
  display_input(detector<T,Tstate> &cl, idx<Tin> &img, double threshold,
		const char *frame_name, 
		unsigned int h0, unsigned int w0, double dzoom, T vmin,
		T vmax, int wid, const char *wname, float transparency) {
    display_wid = (wid >= 0) ? wid :
      new_window((wname ? wname : "detector: output"));
    select_window(display_wid);
    //    disable_window_updates();


    vector<bbox*> &bb = display(cl, img, threshold, frame_name,
				h0, w0, dzoom, vmin, vmax, wid, wname,
				transparency);
    w0 += (uint) (img.dim(1) * dzoom + 5);
    // draw input
    draw_matrix(img, "input", h0, w0, dzoom, dzoom, (Tin) vmin, (Tin) vmax);
    return bb;
  }

  template <typename T, class Tstate> template <typename Tin>
  vector<bbox*>& detector_gui<T,Tstate>::
  display_inputs_outputs(detector<T,Tstate> &cl, idx<Tin> &img,
			 double threshold,
			 const char *frame_name, 
			 unsigned int h0, unsigned int w0, double dzoom,
			 T vmin, T vmax, int wid, const char *wname,
			 T in_vmin, T in_vmax, float transparency) {
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
    select_window(display_wid_fprop);

    // draw input and output
    vector<bbox*>& bb =
      display_input(cl, img, threshold, frame_name,
		    h0, w0, dzoom, in_vmin, in_vmax, display_wid_fprop, wname,
		    transparency);

    // disable_window_updates();
    // draw internal inputs and outputs
    int h = (int) (h0 + cl.oheight * dzoom + 5 + 15);
    int scale = 0;
    int ohmax = ((Tstate*) cl.outputs.get(0))->x.dim(1);
    int ihmax = ((Tstate*) cl.inputs.get(0))->x.dim(1);
    bool first_time = true;
    ostringstream s;
    // compute min and max of all outputs, to maximize intensity display range
    if (vmin == vmax) {
      { idx_bloop1(out, cl.outputs, void*) {
	  idx<T> outx = ((Tstate*) out.get())->x;
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
	idx<T> inx = ((Tstate*) in.get())->x;
	//inx = inx.select(0, 0);
	inx = inx.shift_dim(0, 2);
	idx<T> outx = ((Tstate*) out.get())->x;

	// draw inputs
	gui << black_on_white() << at(h - 15, w0) << "scale #" << scale
	    << " " << inx.dim(0) << "x" << inx.dim(1);
	draw_matrix(inx, h, w0, dzoom, dzoom, (T)vmin, (T)vmax);
	// draw bboxes on scaled input 
	for (vector<bbox*>::iterator i = bb.begin(); i != bb.end(); ++i) {
	  if (scale == (*i)->scale_index)
	    draw_box((uint) (h + dzoom * (*i)->ih0), 
		     (uint) (w0 + dzoom * (*i)->iw0),
		     (uint) (dzoom * (*i)->ih), 
		     (uint) (dzoom * (*i)->iw), 0, 0, 255, 255,
		     new string((const char*)
				cl.labels[(*i)->class_id].idx_ptr()));
	}
	// draw outputs
	int hcat = 0;
	double czoom = dzoom * 2.0;
	int lab = 0;
	{ idx_bloop1(category, outx, T) {
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
      vector<idx<T> > &new_detections = cl.get_originals();
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

  template <typename T, class Tstate> void detector_gui<T,Tstate>::
  update_and_display_queue(deque<idx<T> > &queue, uint step, uint qheight,
			   uint qwidth, vector<idx<T> > &new_detections,
			   uint detcnt, uint &h0, uint &w0, double dzoom) {
    // update queue
    uint queuesz = qheight * qwidth;
    // loop over all new detections and add new ones based on the step
    for (typename vector<idx<T> >::iterator i = new_detections.begin();
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
    for (typename deque<idx<T> >::iterator i = queue.begin();
	 i != queue.end(); ++i) {
      draw_matrix(*i, h, w0 + w, dzoom, dzoom, (T)0, (T)255);
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

  template <typename T, class Tstate> template <typename Tin>
  vector<bbox*>& detector_gui<T,Tstate>::
  display_all(detector<T,Tstate> &cl, idx<Tin> &img, double threshold,
	      const char *frame_name,
	      unsigned int h0, unsigned int w0, double dzoom, T vmin,
	      T vmax, int wid, const char *wname) {
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
    select_window(display_wid_fprop);

    // draw input and output
    vector<bbox*>& bb =
      display_inputs_outputs(cl, img, threshold, frame_name,
			     h0, w0, dzoom, vmin, vmax, display_wid_fprop);

    // disable_window_updates();
    // draw internal states of first scale
    w0 = (cl.width + 5) * 2 + 5;
    module_1_1_gui mg;
    cl.prepare(img);
    cl.preprocess_resolution(0);
    mg.display_fprop(*(module_1_1<T,Tstate>*) &cl.thenet,
    		     *cl.input, *cl.output, h0, w0, (double) 1.0,
		     (T) -1.0, (T) 1.0, true, display_wid_fprop);
    //    enable_window_updates();
    return bb;
  }

  template <typename T, class Tstate> template <typename Tin>
  void detector_gui<T,Tstate>::display_current(detector<T,Tstate> &cl, 
					       idx<Tin> &sample,
					       int wid, const char *wname,
					       double dzoom){
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "detector: inputs, outputs & internals"));
    select_window(display_wid_fprop);
    disable_window_updates();
    clear_window();
    // draw internal states of first scale
    module_1_1_gui mg;
    cl.prepare(sample);
    cl.preprocess_resolution(0);
    mg.display_fprop(*(module_1_1<T,Tstate>*) &cl.thenet,
    		     *cl.input, *cl.output, (uint) 0, (uint) 0, dzoom,
		     (T) -1.0, (T) 1.0, true, display_wid_fprop);
    enable_window_updates();
  }

  template <typename T, class Tstate>
  void detector_gui<T,Tstate>::set_mask_class(const char *name, T threshold) {
    if (name) {
      mask_class = name;
      mask_threshold = threshold;
    }
  }
  
} // end namespace ebl

#endif
