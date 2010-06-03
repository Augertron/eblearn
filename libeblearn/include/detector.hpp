/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include "detector.h"
#include <algorithm>
#include <typeinfo>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


namespace ebl {

  template <class T>
  detector<T>::detector(module_1_1<T> &thenet_,	idx<ubyte> &labels_,
			module_1_1<T> *pp_, uint ppkersz_,
			const char *background, T bias_, float coef_)
    : thenet(thenet_), coef(coef_), bias(bias_),
      inputs(1), outputs(1), results(1), resize_modules(1), nets(1), pp(pp_),
      ppkersz(ppkersz_), nresolutions(3), resolutions(1, 2),
      original_bboxes(nresolutions, 4),
      bgclass(-1), mask_class(-1), scales(NULL), scales_step(0),
      silent(false), restype(SCALES),
      save_mode(false), save_dir(""), save_counts(labels_.dim(0), 0),
      min_size(0), max_size(0), bodetections(false),
      bppdetections(false), pruning(true), bbhfactor(1.0), bbwfactor(1.0) {
    // default resolutions
    double sc[] = { 4, 2, 1 };
    set_resolutions(3, sc);
    // labels
    //labels = strings_to_idx(labels_);
    labels = labels_;
    cout << "Classes names:";
    idx_bloop1(name, labels, ubyte) {
      cout << " " << name.idx_ptr();
    }
    cout << endl;
    // clear buffers
    idx_clear(inputs);
    idx_clear(outputs);
    idx_clear(results);
    idx_clear(resize_modules);
    idx_clear(nets);
    set_bgclass(background);
    // initilizations
    save_max_per_frame = numeric_limits<uint>::max();
  }
  
  template <class T>
  void detector<T>::set_resolutions(uint nresolutions_, const double *scales_) {
    nresolutions = nresolutions_;
    restype = SCALES;
    scales = scales_;
  }
  
  template <class T>
  void detector<T>::set_resolutions(int nresolutions_) {
    nresolutions = (uint) nresolutions_;
    restype = NSCALES;
  }
  
  template <class T>
  void detector<T>::set_resolutions(double scales_step_) {
    restype = SCALES_STEP;
    scales_step = scales_step_;
  }
  
  template <class T>
  detector<T>::~detector() {
    { idx_bloop5(in, inputs, void*, out, outputs, void*, r, results, void*,
		 rsz, resize_modules, void*, net, nets, void*) {
	state_idx<T> *s;
	s = (state_idx<T>*) in.get();
	if (s) delete s;
	s = (state_idx<T>*) out.get();
	if (s) delete s;
	idx<T> *s2 = (idx<T>*) r.get();
	if (s2) delete s2;
	module_1_1<T> *s3 = (module_1_1<T>*) rsz.get();
	if (s3) delete s3;
	module_1_1<T> *s4 = (module_1_1<T>*) net.get();
	if (s4) delete s4;
      }}
  }

  template <class T>
  void detector<T>::init(idxdim &dsample) {
    // size of the sample to process
    int thick = (dsample.order() == 2) ? 1 : dsample.dim(2); // TODO FIXME
    height = dsample.dim(0);    
    width = dsample.dim(1);
    input_dim = idxdim(height, width, thick);
    idxdim sd(thick, height, width);

    // first compute minimum and maximum resolutions for this input dims.
    compute_minmax_resolutions(input_dim);
    cout << "resolutions: input: " << input_dim << " min: " << in_mindim;
    cout << " max: " << in_maxdim << endl;

    switch (restype) {
    case MANUAL:
      break ;
    case SCALES:
	compute_resolutions(input_dim, nresolutions, scales);
	break ;
    case NSCALES: // n scale between min and max resolutions
	compute_resolutions(input_dim, nresolutions);
	break ;
    case SCALES_STEP: // step fixed amount between scale from min to max
	compute_resolutions(input_dim, scales_step);
      break ;
    default: eblerror("unknown scaling mode");
    }
    original_bboxes = idx<uint>(nresolutions, 4);
    
    cout << "multi-resolution detection initialized to ";
    print_resolutions();
    
    // resize input to closest compatible size
    //    sample = image_resize(sample, indim.dim(0), indim.dim(1));
    
    // (re)initialize input and output states and result matrices for each scale
    if ((uint)inputs.dim(0) != nresolutions) { 
      // delete arrays content
      { idx_bloop5(in, inputs, void*,
    		   out, outputs, void*, r, results, void*,
    		   rsz, resize_modules, void*, net, nets, void*) {
    	state_idx<T> *sin = (state_idx<T>*) in.get();
    	state_idx<T> *sout = (state_idx<T>*) out.get();
    	idx<T> *res = (idx<T>*) r.get();
    	resizepp_module<T> *rszpp = (resizepp_module<T>*) rsz.get();
    	module_1_1<T> *network = (module_1_1<T>*) net.get();

    	if (sin) delete sin;
    	if (sout) delete sout;
    	if (res) delete res;
    	if (rszpp) delete rszpp;
    	if (network) delete network;
    	}}
      // reallocate arrays
      inputs  = idx<void*>(nresolutions);
      idx_clear(inputs);
      outputs = idx<void*>(nresolutions);
      idx_clear(outputs);
      results = idx<void*>(nresolutions);
      idx_clear(results);
      resize_modules = idx<void*>(nresolutions);
      idx_clear(resize_modules);
      nets = idx<void*>(nresolutions);
      idx_clear(nets);
    }

    //sizes.set(sample.dim(0) / (float) in_mindim.dim(0), 3);
    cout << "machine's intermediate sizes for each resolution: " << endl;
    { idx_bloop6(resolution, resolutions, unsigned int, in, inputs, void*,
		 out, outputs, void*, r, results, void*,
		 rsz, resize_modules, void*, net, nets, void*) {
	// cast pointers
  	state_idx<T> *sin = (state_idx<T>*) in.get();
	state_idx<T> *sout = (state_idx<T>*) out.get();
	idx<T> *res = (idx<T>*) r.get();
	resizepp_module<T> *rszpp = (resizepp_module<T>*) rsz.get();
	// module_1_1<T> *network = (module_1_1<T>*) net.get();
	// Compute the input sizes for each scale
	idxdim scaled(thick, resolution.get(0), resolution.get(1));
	// // network resizes automatically, only allocate if not allocated
	// if (network == NULL) {
	//   net.set((void*) thenet.copy());
	//   network = (module_1_1<T>*) net.get();
	// }
	// Adapt the size to the network structure:
	idxdim outd = thenet.fprop_size(scaled);
	// idxdim outd = network->fprop_size(scaled);
	// set or resize buffers
	if (sin == NULL)
	  in.set((void*) new state_idx<T>(thick, scaled.dim(1), scaled.dim(2)));
	else
	  sin->resize(thick, scaled.dim(1), scaled.dim(2));
	if (sout == NULL)
	  out.set((void*) new state_idx<T>(labels.dim(0), outd.dim(1),
					   outd.dim(2)));
	else
	  sout->resize(labels.dim(0), outd.dim(1), outd.dim(2));
	if (res == NULL)
	  r.set((void*) new idx<T>(outd.dim(1), outd.dim(2), 2));
	else
	  res->resize(outd.dim(1), outd.dim(2), 2);
 	if (rszpp == NULL)
	  rsz.set((void*) new resizepp_module<T>
		  (scaled.dim(1), scaled.dim(2), MEAN_RESIZE,
		   pp?(module_1_1<T>*)pp->copy():NULL, ppkersz, true));
	else
	  rszpp->set_dimensions(scaled.dim(1), scaled.dim(2));
	// print info about network sizes
	//	network->pretty(scaled);
	thenet.pretty(scaled);
      }}
  }
    
  template <class T>
  int detector<T>::get_class_id(const string &name) {
    int i = 0;
    
    idx_bloop1(lab, labels, ubyte) {
      if (!strcmp((const char *)lab.idx_ptr(), name.c_str()))
	return i;
      i++;
    }
    return -1;
  }
  
  template <class T>
  void detector<T>::set_bgclass(const char *bg) {
    string name;
    
    if (bg)
      name = bg;
    else
      name = "bg"; // default name
    bgclass = get_class_id(name);
    if (bgclass != -1) {
      cout << "Background class is \"" << name << "\" with id " << bgclass;
      cout << "." << endl;
    } else if (bg)
      cerr << "warning: background class \"" << bg << "\" not found." << endl;
  }

  // TODO: handle more than 1 class
  template <class T>
  bool detector<T>::set_mask_class(const char *mask) {
    string name;
    
    if (!mask)
      return false;
    name = mask;
    mask_class = get_class_id(name);
    if (mask_class != -1) {
      cout << "Mask class is \"" << name << "\" with id " << mask_class;
      cout << "." << endl;
      return true;
    }
    cerr << "warning: mask class \"" << mask << "\" not found." << endl;
    return false;
  }

  template <class T>
  void detector<T>::set_silent() {
    silent = true;
  }

  template <class T>
  string& detector<T>::set_save(const string directory) {
    save_mode = true;
    save_dir = directory;
    save_dir += "_";
    save_dir += tstamp();
    cout << "Enabling saving of detected regions into: ";
    cout << save_dir << endl;
    return save_dir;
  }

  template <class T>
  void detector<T>::set_max_resolution(uint max_size_) {
    cout << "Setting maximum input size to " << max_size_ << "x"
	 << max_size_ << "." << endl;
    max_size = max_size_;
  }
  
  template <class T>
  void detector<T>::set_min_resolution(uint min_size_) {
    cout << "Setting minimum input size to " << min_size_ << "x"
	 << min_size_ << "." << endl;
    min_size = min_size_;
  }
  
  template <class T>
  void detector<T>::set_pruning(bool pruning_) {
    pruning = pruning_;
    cout << "Pruning of neighbor answers is "
	 << (pruning ? "enabled" : "disabled") << endl;
  }
  
  template <class T>
  void detector<T>::set_bbox_factors(float hfactor, float wfactor) {
    bbhfactor = hfactor;
    bbwfactor = wfactor;
    cout << "Setting factors on output bounding boxes sizes, height: "
	 << hfactor << ", width: " << wfactor << endl;
  }
  
  template <class T>
  void detector<T>::compute_minmax_resolutions(idxdim &input_dims) {
    // compute maximum closest size of input compatible with the network size
    idxdim indim(input_dims.dim(2), input_dims.dim(0), input_dims.dim(1));
    if (max_size > 0) { // cap on maximum input size
      for (int i = 0; i < indim.order(); ++i) {
	indim.setdim(i, MIN((intg) max_size, indim.dim(i)));
      }
    }
    thenet.fprop_size(indim); // set a valid input dimensions set
    in_maxdim.setdims(indim); // copy valid dims to in_maxdim

    // compute minimum input size compatible with network size
    idxdim minodim(1, 1, 1); // min output dims
    in_mindim = thenet.bprop_size(minodim); // compute min input dims
    // TODO: this seems to screw things up
    if (min_size > 0) { // cap on maximum input size
      idxdim indim2(input_dims.dim(2), min_size, min_size);
      thenet.fprop_size(indim2);
      in_mindim.setdims(indim2);
    }
  }

  template <class T>
  void detector<T>::compute_resolutions(idxdim &input_dims,
					uint &nresolutions) {
    // nresolutions must be >= 1
    if (nresolutions == 0)
      eblerror("expected more resolutions than 0");
    // nresolutions must be less than the minimum pixel distance between min
    // and max
    unsigned int max_res = MIN(in_maxdim.dim(1) - in_mindim.dim(1),
			       in_maxdim.dim(2) - in_mindim.dim(2));
    if (nresolutions > max_res) {
      cerr << "warning: the number of resolutions requested (";
      cerr << nresolutions << ") is more than";
      cerr << " the minimum distance between minimum and maximum possible";
      cerr << " resolutions. (min: " << in_mindim << " max: " << in_maxdim;
      if (in_mindim == in_maxdim)
	nresolutions = 1;
      else
	nresolutions = 2;
      cerr << ") setting it to " << nresolutions << endl;
    }
    
    // only 1 scale if min == max or if only 1 scale requested.
    if ((in_mindim == in_maxdim) || (nresolutions == 1)) {
      resolutions.resize1(0, 1);
      resolutions.set(in_mindim.dim(1), 0, 0);
      resolutions.set(in_mindim.dim(2), 0, 1);
    } else if (nresolutions == 2) { // 2 resolutions: min and max
      resolutions.resize1(0, 2);
      resolutions.set(in_maxdim.dim(1), 0, 0); // max
      resolutions.set(in_maxdim.dim(2), 0, 1); // max
      resolutions.set(in_mindim.dim(1), 1, 0); // min
      resolutions.set(in_mindim.dim(2), 1, 1); // min
    } else { // multiple resolutions: interpolate between min and max
      resolutions.resize1(0, nresolutions);
      int n = nresolutions - 2;
      // compute the step factor: x = e^(log(max/min)/(nres-1))
      double fact = MIN(in_maxdim.dim(1) / (double) in_mindim.dim(1),
			in_maxdim.dim(2) / (double) in_mindim.dim(2));
      double step = exp(log(fact)/(nresolutions - 1));
      double f;
      int i;
      for (f = step, i = 1; i <= n; ++i, f *= step) {
	resolutions.set((uint)(in_maxdim.dim(1) / f), i, 0);
	resolutions.set((uint)(in_maxdim.dim(2) / f), i, 1);
      }
      resolutions.set(in_maxdim.dim(1), 0, 0); // max
      resolutions.set(in_maxdim.dim(2), 0, 1); // max
      resolutions.set(in_mindim.dim(1), nresolutions - 1, 0); // min
      resolutions.set(in_mindim.dim(2), nresolutions - 1, 1); // min
    }
  }

  // use scales
  template <class T>
  void detector<T>::compute_resolutions(idxdim &input_dims, uint nresolutions,
					const double *scales) {
    uint i;
    // nresolutions must be >= 1
    if (nresolutions == 0)
      eblerror("expected more resolutions than 0");
    // check that scales do not oversample
    double maxscale = scales[0];
    for (i = 0; i < nresolutions; ++i)
      if (scales[i] > maxscale)
	maxscale = scales[i];
    intg max_res = (intg) (std::max(in_mindim.dim(1),
				    in_mindim.dim(2)) * maxscale);
    if (max_res > std::max(input_dims.dim(0), input_dims.dim(1)))
      cerr << "warning: maxscale (" << maxscale << ") produces a resolution "
	   << "bigger than original input (" << input_dims << ")." << endl;
    // compute minimum resolution scale
    double mscale = MIN(in_mindim.dim(1) / (double) input_dims.dim(0),
			in_mindim.dim(2) / (double) input_dims.dim(1));
    // compute scales
    resolutions.resize1(0, nresolutions);
    for (i = 0; i < nresolutions; ++i) {
      resolutions.set((uint) (mscale * scales[i] * input_dims.dim(0)), i, 0);
      resolutions.set((uint) (mscale * scales[i] * input_dims.dim(1)), i, 1);
    }
  }


  template <class T>
  void detector<T>::compute_resolutions(idxdim &input_dims, double scales_step){
    // figure out how many resolutions can be used between min and max
    // with a step of scales_step:
    // nres = (log (max/min) / log step) + 1
    double fact = MIN(in_maxdim.dim(1) / (double) in_mindim.dim(1),
		      in_maxdim.dim(2) / (double) in_mindim.dim(2));
    nresolutions = (uint) (log(fact) / log(scales_step)) + 1;
    compute_resolutions(input_dims, nresolutions);
  }

  template <class T>
  void detector<T>::print_resolutions() {
    cout << resolutions.dim(0) << " resolutions: ";
    cout << resolutions.get(0, 0) << "x" << resolutions.get(0, 1);
    for (int i = 1; i < resolutions.dim(0); ++i)
      cout << ", " << resolutions.get(i, 0) << "x" << resolutions.get(i, 1);
    cout << endl;
  }
  
  template <class T>
  void detector<T>::mark_maxima(T threshold) {
    // loop on scales
    idx_bloop2(out_map, outputs, void*, res_map, results, void*) {
      intg winning_class = 0;
      idx<T> raw_maps = ((state_idx<T>*) out_map.get())->x;
      idx<T> max_map = *((idx<T>*) res_map.get());
      int y_max = (int) raw_maps.dim(1);
      int x_max = (int) raw_maps.dim(2);
      int x = 0, y = 0;
      // size of window to search for local maximum
      int nms = y_max >= 2*6+1 ? ((x_max >= 2*6+1) ? 6 : x_max) : y_max;

      idx_fill(max_map, (T)-1);
      // loop on each class
      idx_bloop1(raw_map, raw_maps, T) {
	y = 0;
	// only process if we want to classify this class
	if ((winning_class != bgclass) && (winning_class != mask_class)) {
	  // loop on rows
	  idx_bloop2(max_map_row, max_map, T,
		     raw_map_row, raw_map, T) {
	    x = 0;
	    // loop on cols
	    idx_bloop2(max_map_result, max_map_row, T,
		       raw_map_pix, raw_map_row, T)  {
	      T pix_val = raw_map_pix.get();
	      if (pix_val > max_map_result.get(1)
		  && pix_val > threshold) {
		int local_max, i,j, i_min, j_min, i_max, j_max;
		i_min = std::max(0, (y+nms<=y_max)?
			    ((y-nms>0)?y-nms:0):y_max-2*nms+1);
		j_min = std::max(0, (x+nms<=x_max)?
			    ((x-nms>0)?x-nms:0):x_max-2*nms+1);
		i_max = MIN(raw_map.dim(0),
			    (y-nms>0)?((y+nms<=y_max)?y+nms:y_max):2*nms+1);
		j_max = MIN(raw_map.dim(1),
			    (x-nms>0)?((x+nms<=x_max)?x+nms:x_max):2*nms+1);
		local_max = 1;
		for (i = i_min; i < i_max; i++) {
		  for (j = j_min; j < j_max; j++) {
		    if (pix_val <= raw_map.get(i,j) && (i!=y || j!=x)) {
		      local_max = 0;
		    }
		  }
		}
		// current pixel is the maximum within the window
		if (local_max == 1) {
		  max_map_result.set(winning_class, 0);
		  max_map_result.set(pix_val, 1);
		}
	      }
	      x++;
	    }
	    y++;
	  }
	}
	winning_class++;
      }
    }
  }

  // prune a list of detections.
  // only keep the largest scoring within an area
  template <class T>
  void detector<T>::prune(vector<bbox*> &raw_bboxes,
			  vector<bbox*> &pruned_bboxes) {
    // for each bbox, check that center of current box is not within
    // another box, and only keep ones with highest score when overlapping
    vector<bbox*>::iterator i, j;
    for (i = raw_bboxes.begin(); i != raw_bboxes.end(); ++i) {
      // center of the box
      rect this_bbox((*i)->h0 + (*i)->height / 2,
		     (*i)->w0 + (*i)->width / 2, 1, 1);
      bool add = true;
      // check each other bbox
      for (j = raw_bboxes.begin(); (j != raw_bboxes.end()) && add; ++j) {
	if (i != j) {
	  rect other_bbox((*j)->h0, (*j)->w0, (*j)->height, (*j)->width);
	  if ((this_bbox.overlap(other_bbox)) &&
	      ((*i)->confidence < (*j)->confidence))
	    add = false;
	}
      }
      // if bbox survived, add it
      if (add)
	pruned_bboxes.push_back(*i);
    }
  }
	
  template <class T>
  void detector<T>::smooth_outputs() {
    cout << "smoothing not implemented" << endl;
  }
    
  template <class T>
  void detector<T>::map_to_list(T threshold, vector<bbox*> &raw_bboxes) {
    // make a list that contains the results
    idx<T> in0x(((state_idx<T>*) inputs.get(0))->x);
    double original_h = grabbed.dim(0);
    double original_w = grabbed.dim(1);
    intg offset_h = 0, offset_w = 0;
    int scale_index = 0;
    { idx_bloop5(input, inputs, void*, output, outputs, void*,
		 r, results, void*, resolution, resolutions, uint,
		 obbox, original_bboxes, uint) {
	rect robbox(obbox.get(0), obbox.get(1), obbox.get(2), obbox.get(3));
	double in_h = (double)(((state_idx<T>*) input.get())->x.dim(1));
	double in_w = (double)(((state_idx<T>*) input.get())->x.dim(2));
	double out_h = (double)(((state_idx<T>*) output.get())->x.dim(1));
	double out_w = (double)(((state_idx<T>*) output.get())->x.dim(2));
	double neth = in_mindim.dim(1); // network's input height
	double netw = in_mindim.dim(2); // netowkr's input width
	double scalehi = original_h / robbox.height; // in to original
	double scalewi = original_w / robbox.width; // in to original
	// offset factor in input map
	double offset_h_factor = (in_h - neth)
	  / std::max((double) 1, (out_h - 1));
	double offset_w_factor = (in_w - netw)
	  / std::max((double) 1, (out_w - 1));
	offset_h = 0;
	{ idx_bloop1(re, *((idx<T>*) r.get()), T) {
	    offset_w = 0;
	    { idx_bloop1(ree, re, T) {
		if ((ree.get(0) != bgclass) && (ree.get(0) != mask_class) && 
		    (ree.get(1) > threshold)) {
		  bbox bb;
		  bb.class_id = (int) ree.get(0); // Class
		  bb.confidence = ree.get(1); // Confidence
		  bb.scale_index = scale_index; // scale index
		  // original image
		  uint oh0 = (uint) (offset_h * offset_h_factor * scalehi);
		  uint ow0 = (uint) (offset_w * offset_w_factor * scalewi);
		  bb.h0 = (uint) std::max(0, (int) (oh0 - robbox.h0 * scalehi));
		  bb.w0 = (uint) std::max(0, (int) (ow0 - robbox.w0 * scalewi));
		  bb.height =
		    (uint) (MIN(neth * scalehi + oh0,
				original_h + robbox.h0 * scalehi)
			    - std::max((uint) (robbox.h0 * scalehi), oh0));
		  bb.width =
		    (uint) (MIN(netw * scalewi + ow0,
				original_w + robbox.w0 * scalewi)
			    - std::max((uint) (robbox.w0 * scalewi), ow0));
		  // apply bbox factors
		  bb.h0 = bb.h0 + (uint) (bb.height * bbhfactor / 2);
		  bb.w0 = bb.w0 + (uint) (bb.width * bbwfactor / 2);
		  bb.height = (uint) (bb.height * bbhfactor);
		  bb.width = (uint) (bb.width * bbwfactor);
		  // input map
		  bb.iheight = (uint) in_h; // input h
		  bb.iwidth = (uint) in_w; // input w
		  bb.ih0 = (uint) (offset_h * offset_h_factor);
		  bb.iw0 = (uint) (offset_w * offset_w_factor);
		  bb.ih = (uint) neth;
		  bb.iw = (uint) netw;
		  // output map
		  bb.oheight = (uint) out_h; // output height
		  bb.owidth = (uint) out_w; // output width
		  bb.oh0 = offset_h; // answer height in output
		  bb.ow0 = offset_w; // answer height in output
		  raw_bboxes.push_back(new bbox(bb));
		}
		offset_w++;
	      }}
	    offset_h++;
	  }}
	scale_index++;
      }}
  }
  
  template<class T>
  void detector<T>::pretty_bboxes(const vector<bbox*> &bboxes) {
    cout << endl << "detector: ";
    if (bboxes.size() == 0)
      cout << "no object found." << endl;
    else {
      cout << "found " << bboxes.size() << " objects." << endl;
      vector<bbox*>::const_iterator i = bboxes.begin();
      for ( ; i != bboxes.end(); ++i) {
	cout << "- " << labels[(*i)->class_id].idx_ptr();
	cout << " with confidence " << (*i)->confidence;
	cout << " in scale #" << (*i)->scale_index;
	cout << " (" << grabbed.dim(0) << "x" << grabbed.dim(1);
	cout << " / " << (*i)->scaleh << "x" << (*i)->scalew;
	cout << " = " << (*i)->iheight << "x" << (*i)->iwidth << ")";
	cout << endl;
	cout << "  bounding box: top left " << (*i)->h0 << "x" << (*i)->w0;
	cout << " and size " << (*i)->height << "x" << (*i)->width;
	cout << " out position: " << (*i)->oh0 << "x" << (*i)->ow0;
	cout << " in " << (*i)->oheight << "x" << (*i)->owidth;
	cout << endl;
      }
    }
  }
  
  template <class T> template <class Tin>
  vector<bbox*>& detector<T>::fprop(idx<Tin> &img, T threshold) {
    // tell detections vectors they are not up-to-date anymore
    bodetections = false;
    bppdetections = false;
    // cast input if necessary
    idx<T> img2, img3, tmp;
    if (typeid(T) == typeid(Tin)) // input same type as net, shallow copy
      img2 = (idx<T>&) img;
    else { // deep copy to cast input into net's type
      img2 = idx<T>(img.get_idxdim());
      idx_copy(img, img2);
    }
    if (img2.order() == 2) {
      img3 = idx<T>(img2.dim(0), img2.dim(1), 1);
      tmp = img3.select(2, 0);
      idx_copy(img2, tmp);  // TODO: avoid copy just to augment order
    } else
      img3 = img2;
    grabbed = img3;
    // if input size had changed, reinit resolutions
    if (!(input_dim == img.get_idxdim())) {
      init(img.get_idxdim());
    }
    // do a fprop for each scaled input sample
    multi_res_fprop(img3);
    // smooth outputs
    smooth_outputs();
    // find points that are local maxima spatial and class-wise
    // write result in m. rescale result to [0 1]
    // TODO: use connected components instead of fixed-size window local maxima?
    mark_maxima(threshold);
    // get bounding boxes
    for (vector<bbox*>::iterator k = raw_bboxes.begin(); k != raw_bboxes.end();
	 ++k)
      if (*k)
	delete *k;
    raw_bboxes.clear();
    map_to_list(threshold, raw_bboxes);
    vector<bbox*> &bb = raw_bboxes;
    // prune bounding boxes btwn scales
    if (pruning) {
      pruned_bboxes.clear();
      prune(raw_bboxes, pruned_bboxes);
      bb = pruned_bboxes;
    }
    // print results
    if (!silent)
      pretty_bboxes(bb);
    // save positive response input windows in save mode
    if (save_mode)
      save_bboxes(bb, save_dir);
    // return bounding boxes
    return bb;
  }

  template <class T>
  void detector<T>::save_bboxes(const vector<bbox*> &bboxes, const string &dir){
    string classname;
    ostringstream fname, cmd;
    state_idx<T> *input = NULL;
    idx<T> inpp, inorig;
    vector<bbox*>::const_iterator bbox;
    vector<bool> dir_exists(labels.dim(0), false);
    string root = dir;
    root += "/";
    vector<string> dir_pp(labels.dim(0), root.c_str());
    vector<string> dir_orig(labels.dim(0), root.c_str());

    // initialize directory names
    for (int i = 0; i < labels.dim(0); ++i) {
      classname = (const char *) labels[i].idx_ptr();
      dir_pp[i] += "preprocessed/";
      dir_pp[i] += classname;
      dir_pp[i] += "/";
      dir_orig[i] += "original/";
      dir_orig[i] += classname;
      dir_orig[i] += "/";
    }
    // loop on bounding boxes
    uint i = 0;
    for (bbox = bboxes.begin(); bbox != bboxes.end(); ++bbox, ++i) {
      // exclude background class
      if (((*bbox)->class_id == bgclass) || ((*bbox)->class_id == mask_class))
	continue ;
      // get class name
      classname = (const char *) labels[(*bbox)->class_id].idx_ptr();
      // check if directory exists for this class, otherwise create it
      if (!dir_exists[(*bbox)->class_id]) {
	mkdir_full(dir_pp[(*bbox)->class_id]);
	mkdir_full(dir_orig[(*bbox)->class_id]);
	dir_exists[(*bbox)->class_id] = true;
      }
      // get bbox of preprocessed input at bbox's scale
      input = (state_idx<T>*) inputs.get((*bbox)->scale_index);
      inpp = input->x.narrow(1, (*bbox)->ih, (*bbox)->ih0);
      inpp = inpp.narrow(2, (*bbox)->iw, (*bbox)->iw0);
      //inpp = inpp.shift_dim(0, 2); // put channels back to 3rd position
      // get bbox of original input
      inorig = grabbed.narrow(0, (*bbox)->height, (*bbox)->h0);
      inorig = inorig.narrow(1, (*bbox)->width, (*bbox)->w0);
      // save preprocessed image as lush mat
      fname.str(""); fname << dir_pp[(*bbox)->class_id] << classname;
      fname << setw(6) << setfill('0') << save_counts[(*bbox)->class_id];
      fname << MATRIX_EXTENSION;
      save_matrix(inpp, fname.str());
      cout << "saved " << fname.str() << endl;
      // save original image as png
      fname.str(""); fname << dir_orig[(*bbox)->class_id] << classname;
      fname << setw(6) << setfill('0');
      fname << save_counts[(*bbox)->class_id] << ".png";
      save_image(fname.str(), inorig, "png");
      cout << "saved " << fname.str() << endl;
      // increment file counter
      save_counts[(*bbox)->class_id]++;
      // stop if reached max save per frame
      if (i >= save_max_per_frame)
	break ;
    }
  }
  
  template <class T>
  uint detector<T>::get_total_saved() {
    uint total = 0;
    for (vector<uint>::iterator i = save_counts.begin();
	 i != save_counts.end(); ++i)
      total += *i;
    return total;
  }

  template <class T>
  void detector<T>::set_save_max_per_frame(uint max) {
    save_max_per_frame = max;
  }

  template <class T>
  vector<idx<T> >& detector<T>::get_originals() {
    if (bodetections) // recompute only if not up-to-date
      return odetections;
    idx<T> input;
    vector<bbox*>::const_iterator bbox;

    // clear vector
    odetections.clear();
    // loop on bounding boxes
    for (bbox = pruned_bboxes.begin(); bbox != pruned_bboxes.end(); ++bbox) {
      // exclude background class
      if (((*bbox)->class_id == bgclass) || ((*bbox)->class_id == mask_class))
	continue ;
      // get bbox of input
      input = grabbed.narrow(0, (*bbox)->height, (*bbox)->h0);
      input = input.narrow(1, (*bbox)->width, (*bbox)->w0);
      odetections.push_back(input);
    }
    bodetections = true;
    return odetections;
  }
  
  template <class T>
  vector<idx<T> >& detector<T>::get_preprocessed() {
    if (bppdetections) // recompute only if not up-to-date
      return ppdetections;
    idx<T> input;
    state_idx<T> *sinput = NULL;
    vector<bbox*>::const_iterator bbox;

    // clear vector
    ppdetections.clear();
    // loop on bounding boxes
    for (bbox = pruned_bboxes.begin(); bbox != pruned_bboxes.end(); ++bbox) {
      // exclude background class
      if (((*bbox)->class_id == bgclass) || ((*bbox)->class_id == mask_class))
	continue ;
      // get bbox of input
      sinput = (state_idx<T>*) inputs.get((*bbox)->scale_index);
      input = sinput->x.narrow(1, (*bbox)->ih, (*bbox)->ih0);
      input = input.narrow(2, (*bbox)->iw, (*bbox)->iw0);
      ppdetections.push_back(input);
    }
    bppdetections = true;
    return ppdetections;
  }
  
  template <class T>
  idx<T> detector<T>::get_mask(string &classname) {
    int id = get_class_id(classname), i = 0;
    idxdim d(grabbed.dim(0), grabbed.dim(1));
    if (mask.get_idxdim() != d)
      mask = idx<T>(d);
    if (id == -1) { // class not found
      cerr << "warning: unknown class " << classname << endl;
      idx_clear(mask);
      return mask;
    }
    // merge all outputs of class 'id' into mask
    idx_bloop3(input, inputs, void*, output, outputs, void*,
	       obbox, original_bboxes, uint) {
      idx<T> in = ((state_idx<T>*) input.get())->x.select(0, 0);
      idx<T> out = ((state_idx<T>*) output.get())->x.select(0, id);
      rect o(obbox.get(0), obbox.get(1),
	     obbox.get(2), obbox.get(3));
      // resizing to inputs, then to original input, to avoid precision loss
      out = image_resize(out, in.dim(0), in.dim(1), 1);
      out = out.narrow(0, obbox.get(2), obbox.get(0));
      out = out.narrow(1, obbox.get(3), obbox.get(1));
      out = image_resize(out, mask.dim(0), mask.dim(1), 1);
      if (i++ == 0)
	idx_copy(out, mask);
      else
	idx_max(mask, out, mask);
    }
    return mask;
  }
  
  template <class T>
  void detector<T>::multi_res_fprop(idx<T> &sample) {
    
    // copy original input into a state_idx
    idx<T> imres = sample.shift_dim(2, 0);
    state_idx<T> input(imres.get_idxdim());
    idx_copy(imres, input.x);
    timer t;
    t.start();

    // module_1_1<T> *network = NULL;
    // resize original input and fprop for each resolution
    { idx_bloop4(in, inputs, void*, out, outputs, void*,
		 bbox, original_bboxes, uint, rsz, resize_modules, void*) {
		 // net, nets, void*) {
	state_idx<T> &ii = *(state_idx<T>*)(in.get()); // input
	state_idx<T> &oo = *(state_idx<T>*)(out.get()); // output
	resizepp_module<T> &rszpp = *(resizepp_module<T>*)(rsz.get());

	// network = (module_1_1<T>*)(net.get());
	// resize and preprocess input
	rszpp.fprop(input, ii);
	// memorize original input's bbox in resized input
	rect bb = rszpp.get_original_bbox();
	bbox.set(bb.h0, 0);
	bbox.set(bb.w0, 1);
	bbox.set(bb.height, 2);
	bbox.set(bb.width, 3);
	// add bias and multiply by coeff if necessary
	if (bias != 0)
	  idx_addc(ii.x, bias, ii.x);
	if (coef != 1)
	  idx_dotc(ii.x, coef, ii.x);
	// run fprop for this scale
    
	thenet.fprop(ii, oo);
	// network->fprop(ii, oo);
      }}
    cout << "net: " << t.elapsed_milliseconds() << " ms. ";
  }

} // end namespace ebl

#endif
