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

#ifndef DETECTOR_HPP
#define DETECTOR_HPP

#include "numerics.h"

#ifndef __NOSTL__
#include <algorithm>
#include <typeinfo>
#include <iomanip>
#endif

using namespace std;

namespace ebl {

  template <typename T, class Tstate>
  detector<T,Tstate>::detector(module_1_1<T,Tstate> &thenet_,
			       idx<ubyte> &labels_, idx<T> &tgt,
			       module_1_1<T,Tstate> *pp_, uint ppkersz_,
			       const char *background, T bias_, float coef_,
			       bool single_output, std::ostream &o,
			       std::ostream &e)
    : thenet(thenet_), resizepp(MEAN_RESIZE, pp_, ppkersz_, true),
      coef(coef_), bias(bias_), input(NULL), output(NULL), minput(NULL),
      inputs(1), outputs(1), results(1), pp(pp_),
      ppkersz(ppkersz_), nresolutions(3), resolutions(1, 4),
      original_bboxes(nresolutions, 4),
      bgclass(-1), mask_class(-1), scales(NULL), scales_step(0),
      max_scale(1.0), min_scale(1.0),
      silent(false), restype(ORIGINAL),
      save_mode(false), save_dir(""), save_counts(labels_.dim(0), 0),
      min_size(0), max_size(0), bodetections(false),
      bppdetections(false), pruning(pruning_overlap),
      bbhfactor(1.0), bbwfactor(1.0),
      mem_optimization(false), optimization_swap(false), keep_inputs(true),
      mout(o), merr(e), targets(tgt), 
      min_hcenter_dist(0.0), min_wcenter_dist(0.0), 
      min_hcenter_dist2(0.0), min_wcenter_dist2(0.0), 
      max_overlap(.5), max_overlap2(0.0),
      mean_bb(false), max_object_hratio(0.0), 
      min_input_height(-1), min_input_width(-1) {
    // // default resolutions
    // double sc[] = { 4, 2, 1 };
    // set_resolutions(3, sc);
    // labels
    //labels = strings_to_idx(labels_);
    labels = labels_;
    mout << "Classes names:";
    idx_bloop1(name, labels, ubyte) {
      mout << " " << (const char*) name.idx_ptr();
    }
    mout << endl;
    // clear buffers
    idx_clear(inputs);
    idx_clear(outputs);
    idx_clear(results);
// #ifdef __ANDROID__ // TODO: temporary
//     bgclass = 0;
// #else
    if (!single_output)
      set_bgclass(background);
    //#endif
    // initilizations
    save_max_per_frame = limits<uint>::max();
    // by default, when foot line is equal, no overlap.
    set_confidence_type(confidence_max);
    ped_only = false; // temporary TODO
    share_parts = false;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_scaling_original() {
    nresolutions = 1;
    restype = ORIGINAL;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_resolutions(uint nresolutions_,
					   const double *scales_) {
    nresolutions = nresolutions_;
    restype = SCALES;
    scales = scales_;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_resolutions(int nresolutions_) {
    nresolutions = (uint) nresolutions_;
    restype = NSCALES;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_resolutions(double scales_step_,
					   double max_scale_,
					   double min_scale_) {
    restype = SCALES_STEP;
    scales_step = scales_step_;
    max_scale = max_scale_;
    min_scale = min_scale_;
    mout << "Multi resolution scales: step factor " << scales_step
	 << ", min/max resolution factor " << min_scale << ", " << max_scale
	 << endl;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_zpads(float hzpad_, float wzpad_) {
    idxdim minodim(1, 1, 1); // min output dims
    netdim = thenet.bprop_size(minodim); // compute min input dims
    hzpad = (uint) (hzpad_ * netdim.dim(1));
    wzpad = (uint) (wzpad_ * netdim.dim(2));
    resizepp.set_zpads(hzpad, wzpad);
    mout << "Adding zero padding on input (on each side): hpad: "
	 << hzpad << " wpad: " << wzpad << endl;
    if (hzpad_ > 1 || wzpad_ > 1)
      eblerror("zero padding coeff should be in [0 1] range");
  }
					   
  template <typename T, class Tstate>
  detector<T,Tstate>::~detector() {
    { idx_bloop3(in, inputs, void*, out, outputs, void*, r, results, void*) {
	Tstate *s;
	s = (Tstate*) in.get();
	if (s) delete s;
	s = (Tstate*) out.get();
	if (s) delete s;
	idx<T> *s2 = (idx<T>*) r.get();
	if (s2) delete s2;
      }}
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::init(idxdim &dsample) {
    // manually set net min input
    if (min_input_height > 0 && min_input_width > 0) {
      idxdim minodim(1, min_input_height, min_input_width);
      netdim = minodim;
    } else { // compute minimum input size compatible with network size
      idxdim minodim(1, 1, 1); // min output dims
      netdim = thenet.bprop_size(minodim); // compute min input dims
    }
    mout << "Network's minimum input dimension is: " << netdim << endl;
    mout << "min input -> "; thenet.pretty(netdim);
    // limit the input size as a factor of the object's height
    if (max_object_hratio > 0.0) {
      double objh = (double) netdim.dim(1) * bbhfactor; // object's height
      // max image's height
      double h = (double) dsample.dim(0);
      double w = (double) dsample.dim(1);
      double max_imh = objh * max_object_hratio + (double) (2 * hzpad);
      double max_imw = w * max_imh / h;
      max_size = (uint) std::max(max_imh, max_imw);
      mout << "Limiting image's height to " << max_size
	   << " in accordance to max_object_hratio " 
	   << max_object_hratio << " and total extra height padding " 
	   << hzpad * 2 << endl;
    }
    // size of the sample to process
    int thick = (dsample.order() == 2) ? 1 : dsample.dim(2); // TODO FIXME
    oheight = dsample.dim(0);
    owidth = dsample.dim(1);
    height = (int) (dsample.dim(0) * max_scale);    
    width = (int) (dsample.dim(1) * max_scale);
    input_dim = idxdim(height, width, thick);
    idxdim sd(thick, height, width);

    // first compute minimum and maximum resolutions for this input dims.
    compute_minmax_resolutions(input_dim);
    mout << "resolutions: input: " << dsample << " max-scaled input (* " 
	 << max_scale << "): " << input_dim << " min: " << in_mindim
	 << " max: " << in_maxdim << endl;
    mout << "Scaling type: ";
    switch (restype) {
    case ORIGINAL:
      mout << "1 scale only, the image's original scale." << endl;
      compute_resolutions(input_dim);
      break ;
    case MANUAL:
      mout << "Manual specification of each scale size." << endl;
      break ;
    case SCALES:
      mout << "Manual specification of each scale factor." << endl;
      compute_resolutions(input_dim, nresolutions, scales);
      break ;
    case NSCALES: // n scale between min and max resolutions
      mout << nresolutions << " scales between min and max scales." << endl;
      compute_resolutions(input_dim, nresolutions);
      break ;
    case SCALES_STEP: // step fixed amount between scale from min to max
      mout << scales_step << " scale step between min and max scales." << endl;
      compute_resolutions(input_dim, scales_step, min_scale, max_scale);
      break ;
    default: eblerror("unknown scaling mode");
    }
    original_bboxes = idx<uint>(nresolutions, 4);
    
    mout << "multi-resolution detection initialized to (ideal scales) ";
    print_resolutions();
    
    // resize input to closest compatible size
    //    sample = image_resize(sample, indim.dim(0), indim.dim(1));
    
    // (re)initialize input and output states and result matrices for each scale
    if ((uint)inputs.dim(0) != nresolutions) { 
      // delete arrays content
      { idx_bloop3(in, inputs, void*, out, outputs, void*, r, results, void*) {
    	Tstate *sin = (Tstate*) in.get();
    	Tstate *sout = (Tstate*) out.get();
    	idx<T> *res = (idx<T>*) r.get();

    	if (sin) delete sin;
    	if (sout) delete sout;
    	if (res) delete res;
    	}}
      // reallocate arrays
      inputs  = idx<void*>(nresolutions);
      idx_clear(inputs);
      outputs = idx<void*>(nresolutions);
      idx_clear(outputs);
      results = idx<void*>(nresolutions);
      idx_clear(results);
    }

    //sizes.set(sample.dim(0) / (float) in_mindim.dim(0), 3);
    mout << "machine's intermediate sizes for each resolution: " << endl;
    { idx_bloop4(resolution, resolutions, unsigned int, in, inputs, void*,
		 out, outputs, void*, r, results, void*) {
	// cast pointers
  	Tstate *sin = (Tstate*) in.get();
	Tstate *sout = (Tstate*) out.get();
	idx<T> *res = (idx<T>*) r.get();
	// Compute the input sizes for each scale
	idxdim scaled(thick, resolution.get(0), resolution.get(1));
	// Adapt the size to the network structure:
	idxdim outd = thenet.fprop_size(scaled);
	// remember correct resolution
	resolution.set(resolution.get(0), 2); // originally requested
	resolution.set(resolution.get(1), 3); // originally requested
	resolution.set(scaled.dim(1), 0); // network compatible
	resolution.set(scaled.dim(2), 1); // network compatible
	// set or resize buffers
	if (sin == NULL)
	  in.set((void*) new Tstate(thick, scaled.dim(1),
					   scaled.dim(2)));
	else
	  sin->resize(thick, scaled.dim(1), scaled.dim(2));
	if (sout == NULL)
	  out.set((void*) new Tstate(labels.dim(0), outd.dim(1),
					   outd.dim(2)));
	else
	  sout->resize(labels.dim(0), outd.dim(1), outd.dim(2));
	if (res == NULL)
	  r.set((void*) new idx<T>(outd.dim(1), outd.dim(2), 2));
	else
	  res->resize(outd.dim(1), outd.dim(2), 2);
	resizepp.set_dimensions(scaled.dim(1), scaled.dim(2));
	// print info about network sizes
	thenet.pretty(scaled);
      }}
  }
    
  template <typename T, class Tstate>
  int detector<T,Tstate>::get_class_id(const string &name) {
    int i = 0;
    
    idx_bloop1(lab, labels, ubyte) {
      if (!strcmp((const char *)lab.idx_ptr(), name.c_str()))
	return i;
      i++;
    }
    return -1;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_bgclass(const char *bg) {
    string name;
    
    if (bg)
      name = bg;
    else
      name = "bg"; // default name
    bgclass = get_class_id(name);
    if (bgclass != -1) {
      mout << "Background class is \"" << name << "\" with id " << bgclass;
      mout << "." << endl;
    } else if (bg)
      merr << "warning: background class \"" << bg << "\" not found." << endl;
  }

  // TODO: handle more than 1 class
  template <typename T, class Tstate>
  bool detector<T,Tstate>::set_mask_class(const char *mask) {
    string name;
    
    if (!mask)
      return false;
    name = mask;
    mask_class = get_class_id(name);
    if (mask_class != -1) {
      mout << "Mask class is \"" << name << "\" with id " << mask_class;
      mout << "." << endl;
      return true;
    }
    merr << "warning: mask class \"" << mask << "\" not found." << endl;
    return false;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_silent() {
    silent = true;
  }

  template <typename T, class Tstate>
  string& detector<T,Tstate>::set_save(const string directory) {
    save_mode = true;
    save_dir = directory;
    // save_dir += "_";
    // save_dir += tstamp();
    mout << "Enabling saving of detected regions into: ";
    mout << save_dir << endl;
    return save_dir;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_max_resolution(uint max_size_) {
    uint mzpad = std::max(hzpad * 2, wzpad * 2);
    max_size = max_size_ + mzpad;
    mout << "Setting maximum input size to " << max_size_ << "x"
	 << max_size_ << " (add twice max(hzpad,wzpad): " << mzpad
	 << ")" << endl;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_min_resolution(uint min_size_) {
    mout << "Setting minimum input size to " << min_size_ << "x"
	 << min_size_ << "." << endl;
    min_size = min_size_;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_pruning(t_pruning type, bool ped_only_,
				       float min_hcenter_dist_, 
				       float min_wcenter_dist_, 
				       float max_overlap_,
				       bool share_parts_,
				       T threshold_parts_,
				       float min_hcenter_dist2_, 
				       float min_wcenter_dist2_, 
				       float max_overlap2_, bool mean_bb_,
				       float ss_mhd, float ss_mwd) {
    mean_bb = mean_bb_;
    ped_only = ped_only_;
    share_parts = share_parts_;
    threshold_parts = threshold_parts_;
    min_hcenter_dist = min_hcenter_dist_;
    min_wcenter_dist = min_wcenter_dist_;
    max_overlap = max_overlap_;
    min_hcenter_dist2 = min_hcenter_dist2_;
    min_wcenter_dist2 = min_wcenter_dist2_;
    max_overlap2 = max_overlap2_;
    same_scale_mhd = ss_mhd;
    same_scale_mwd = ss_mwd;

    pruning = type;
    mout << "Pruning of bounding boxings: ";
    switch (pruning) {
    case pruning_none: mout << "none"; break ;
    case pruning_overlap:
      mout << " overlap, ignore matching bboxes (intersection/union) beyond "
	   << "the max_overlap threshold (" << max_overlap << ") and centers "
	   << "closer than " << min_hcenter_dist << " * height and "
	   << min_wcenter_dist << " * width." << endl;
      break ;
    case pruning_pedestrian: 
      mout << "pedestrian"; 
      // add composed classes
      add_class("ht");
      add_class("tb");
      add_class("htb");
      add_class("hb");
      add_class("mean");
      add_class("h");
      add_class("t");
      break ;
    default:
      eblerror("unknown type of pruning " << pruning);
    }
    mout << endl;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_bbox_factors(float hfactor, float wfactor,
					    float hfactor2, float wfactor2) {
    bbhfactor = hfactor;
    bbwfactor = wfactor;
    bbhfactor2 = hfactor2;
    bbwfactor2 = wfactor2;
    mout << "Setting factors on output bounding boxes sizes, height: "
	 << hfactor << ", width: " << wfactor << " height2: "
	 << hfactor2 << ", width2: " << wfactor2 << endl;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_confidence_type(t_confidence type) {
    conf_type = type;
    T max_dist;
    switch (conf_type) {
    case confidence_sqrdist:
      max_dist = idx_max(targets) - idx_min(targets);
      conf_ratio = targets.dim(0) * max_dist * max_dist;
      // shift value to be subtracted before dividing by conf_ratio
      conf_shift = idx_min(targets);
      mout << "Using sqrdist confidence formula with normalization ratio "
	   << conf_ratio << " and shift value " << conf_shift << endl;
      break ;
    case confidence_single:
      conf_ratio = idx_max(targets) - idx_min(targets);
      // shift value to be subtracted before dividing by conf_ratio
      conf_shift = idx_min(targets);
      mout << "Using single output confidence with normalization ratio "
	   << conf_ratio << " and shift value " << conf_shift << endl;
      break ;
    case confidence_max:
      conf_ratio = 2 * (idx_max(targets) - idx_min(targets));
      // shift value to be subtracted before dividing by conf_ratio
      conf_shift = 2 * (idx_min(targets));
      mout << "Using max confidence formula with normalization ratio "
	   << conf_ratio << " and shift value " << conf_shift << endl;
      break ;
    default:
      eblerror("confidence type " << conf_type << " undefined");
    }
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::set_max_object_hratio(double hratio) {
    max_object_hratio = hratio;
    mout << "Max image's height / object's height ratio is " << hratio << endl;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_min_input(intg h, intg w) {
    min_input_height = h;
    min_input_width = w;
    mout << "Manually setting network's minimum input: " << h << "x" << w 
	 << endl;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_mem_optimization(Tstate &in, Tstate &out, 
						bool keep_inputs_) {
    mout << "Optimizing memory usage by using only 2 alternating buffers."
	 << endl;
    mem_optimization = true;
    keep_inputs = keep_inputs_;
    minput = &in;
    input = &in;
    output = &out;
    // remember if we need to swap buffers because of odd operations.
    optimization_swap = !thenet.optimize_fprop(*input, *output);
  }
  
  ////////////////////////////////////////////////////////////////
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::compute_minmax_resolutions(idxdim &input_dims) {
    in_mindim = netdim;
    in_mindim.setdim(1, netdim.dim(1) + hzpad * 2);
    in_mindim.setdim(2, netdim.dim(2) + wzpad * 2);
    thenet.fprop_size(in_mindim);
    // TODO: this seems to screw things up
    if (min_size > 0) { // cap on maximum input size
      idxdim indim2(input_dims.dim(2), min_size, min_size);
      thenet.fprop_size(indim2);
      in_mindim.setdims(indim2);
    }
    // compute maximum closest size of input compatible with the network size
    idxdim indim(input_dims.dim(2), input_dims.dim(0) + hzpad * 2,
		 input_dims.dim(1) + wzpad * 2);
    if (max_size > 0) { // cap on maximum input size
      if (indim.dim(1) > max_size || indim.dim(2) > max_size) {
	if (indim.dim(1) > indim.dim(2)) {
	  indim.setdim(2, max_size * indim.dim(2) / indim.dim(1));
	  indim.setdim(1, max_size);
	} else {
	  indim.setdim(1, max_size * indim.dim(1) / indim.dim(2));
	  indim.setdim(2, max_size);
	}
      }
    }
    thenet.fprop_size(indim); // set a valid input dimensions set
    in_maxdim.setdims(indim); // copy valid dims to in_maxdim
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::compute_resolutions(idxdim &input_dims,
					       uint &nresolutions) {
    // nresolutions must be >= 1
    if (nresolutions == 0)
      eblerror("expected more resolutions than 0");
    // nresolutions must be less than the minimum pixel distance between min
    // and max
    unsigned int max_res = MIN(in_maxdim.dim(1) - in_mindim.dim(1),
			       in_maxdim.dim(2) - in_mindim.dim(2));
    if (nresolutions > max_res) {
      merr << "warning: the number of resolutions requested (";
      merr << nresolutions << ") is more than";
      merr << " the minimum distance between minimum and maximum possible";
      merr << " resolutions. (min: " << in_mindim << " max: " << in_maxdim;
      if (in_mindim == in_maxdim)
	nresolutions = 1;
      else
	nresolutions = 2;
      merr << ") setting it to " << nresolutions << endl;
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
      //      nresolutions += 10; // add a special min resolution
      resolutions.resize1(0, nresolutions);
      int n = nresolutions;
      // compute the step factor: x = e^(log(max/min)/(nres-1))
      double fact = MIN(in_maxdim.dim(1) / (double) in_mindim.dim(1),
			in_maxdim.dim(2) / (double) in_mindim.dim(2));
      double step = exp(log(fact)/(nresolutions - 1));
      double f;
      int i;
      for (f = step, i = 1; i < n; ++i, f *= step) {
	resolutions.set((uint)(in_maxdim.dim(1) / f), i, 0);
	resolutions.set((uint)(in_maxdim.dim(2) / f), i, 1);
      }
      resolutions.set(in_maxdim.dim(1), 0, 0); // max
      resolutions.set(in_maxdim.dim(2), 0, 1); // max
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::compute_resolutions(idxdim &input_dims) {
    nresolutions = 1;
    resolutions.resize1(0, nresolutions);
    resolutions.set(std::min((intg) max_size, input_dims.dim(0)), 0, 0);
    resolutions.set(std::min((intg) max_size, input_dims.dim(1)), 0, 1);
  }

  // use scales
  template <typename T, class Tstate>
  void detector<T,Tstate>::compute_resolutions(idxdim &input_dims,
					       uint nresolutions,
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
      merr << "warning: maxscale (" << maxscale << ") produces a resolution "
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


  template <typename T, class Tstate>
  void detector<T,Tstate>::compute_resolutions(idxdim &input_dims,
					       double scales_step,
					       double min_scale,
					       double max_scale) {
    double hmin = (std::max)((double)min_size, in_mindim.dim(1) * min_scale);
    double wmin = (std::max)((double)min_size, in_mindim.dim(2) * min_scale);
    if (min_scale == 0)
      eblerror("min_scale cannot be zero");
    in_mindim.setdim(1, (intg) hmin);
    in_mindim.setdim(2, (intg) wmin);
    // figure out how many resolutions can be used between min and max
    // with a step of scales_step:
    // nres = (log (max/min) / log step) + 1
    double fact = MIN(in_maxdim.dim(1) / hmin,
		      in_maxdim.dim(2) / wmin);
    nresolutions = (uint) (log(fact) / log(scales_step)) + 1;
    compute_resolutions(input_dims, nresolutions);
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::print_resolutions() {
    mout << resolutions.dim(0) << " resolutions: ";
    mout << resolutions.get(0, 0) << "x" << resolutions.get(0, 1);
    for (int i = 1; i < resolutions.dim(0); ++i)
      mout << ", " << resolutions.get(i, 0) << "x" << resolutions.get(i, 1);
    mout << endl;
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::mark_maxima(T threshold) {
    // loop on scales
    idx_bloop2(out_map, outputs, void*, res_map, results, void*) {
      intg winning_class = 0;
      idx<T> raw_maps = ((Tstate*) out_map.get())->x;
      idx<T> max_map = *((idx<T>*) res_map.get());
      int y_max = (int) raw_maps.dim(1);
      int x_max = (int) raw_maps.dim(2);
      int x = 0, y = 0;
      // size of window to search for local maximum
      int nms = 1; //y_max >= 2*6+1 ? ((x_max >= 2*6+1) ? 6 : x_max) : y_max;

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
		  max_map_result.set((T) winning_class, 0);
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

  //////////////////////////////////////////////////////////////////////////////
  // pruning
  
  // prune a list of detections.
  // only keep the largest scoring within an area
  template <typename T, class Tstate>
  void detector<T,Tstate>::prune_overlap(vector<bbox*> &raw_bboxes,
					 vector<bbox*> &pruned_bboxes,
					 float max_match,
					 bool same_class_only,
					 float min_hcenter_dist,
					 float min_wcenter_dist,
					 float threshold,
					 uint image_height, uint image_width,
					 float same_scale_mhd,
					 float same_scale_mwd) {
    size_t ib, jb;
    bbox *i, *j;
    // apply pre-process bb factors
    for (ib = 0; ib < raw_bboxes.size(); ++ib) {
      i = raw_bboxes[ib];
      // apply bbox factors
      i->h0 += (int) (i->height - i->height * bbhfactor)/2;
      i->w0 += (int) (i->width - i->width * bbwfactor)/2;
      i->height = (int) (i->height * bbhfactor);
      i->width = (int) (i->width * bbwfactor);
    }
    // for each bbox, check that matching with other bboxes does not go beyond
    // the maximum authorized matching score (0 means no overlap, 1 is identity)
    // and only keep ones with highest score when overlapping.
    // prune
    for (ib = 0; ib < raw_bboxes.size(); ++ib) {
      i = raw_bboxes[ib];
      if (i && i->class_id != bgclass && i->class_id != mask_class) {
	if (i->confidence < threshold)
	  continue ; // ignore if lower than threshold
	bool add = true;
	// check each other bbox
	for (jb = 0; jb < raw_bboxes.size() && add; ++jb) {
	  j = raw_bboxes[jb];
	  if (j && i != j) {
	    float match = i->match(*j);
	    bool overlap = false;
	    if (match >= max_match)
	      overlap = true; // there is overlap
	    // forbid centers to be closer than min dist to each other in each axis
	    if ((i->center_hdistance(*j) < min_hcenter_dist
		 && i->center_wdistance(*j) < min_wcenter_dist)
		|| (j->center_hdistance(*i) < min_hcenter_dist
		    && j->center_wdistance(*i) < min_wcenter_dist))
	      overlap = true;
	    // forbid centers to be closer than min dist to each other in each axis
	    // for boxes originating from the same scale. similar to applying stride on output.
	    if (i->scale_index == j->scale_index &&
		((i->center_hdistance(*j) < same_scale_mhd
		  && i->center_wdistance(*j) < same_scale_mwd)
		 || (j->center_hdistance(*i) < same_scale_mhd
		     && j->center_wdistance(*i) < same_scale_mwd)))
	      overlap = true;
	    // if same_class_only, allow pruning only if 2 bb are same class
	    bool allow_pruning = !same_class_only ||
	      (same_class_only && i->class_id == j->class_id);
	    // keep only 1 bb if overlap and pruning is ok
	    if (overlap && allow_pruning) {
	      if (i->confidence < j->confidence) {
		// it's not the highest confidence, stop here.
		add = false;
		break ;
	      } else if (i->confidence == j->confidence) {
		// we have a tie, keep the biggest one.
		if (i->height * i->width > j->height * j->width) {
		  delete j;
		  raw_bboxes[jb] = NULL;
		} else {
		  delete i;
		  raw_bboxes[ib] = NULL;
		  add = false;
		  break ;
		}
	      }
	    }
	  }
	}
	// if bbox survived, add it
	if (add)
	  pruned_bboxes.push_back(i);
      }
    }
#ifdef __DEBUG__
    mout << "Pruned " << raw_bboxes.size() << " bboxes to "
	 << pruned_bboxes.size() << " bboxes." << endl;
#endif
    // apply post process bb factors
    for (ib = 0; ib < pruned_bboxes.size(); ++ib) {
      i = pruned_bboxes[ib];
      // apply bbox factors
      float h0 = i->h0 + (i->height - i->height * bbhfactor2)/2;
      float w0 = i->w0 + (i->width - i->width * bbwfactor2)/2;
      float height = i->height * bbhfactor2;
      float width = i->width * bbwfactor2;
      // cut off bbox at image boundaries
      i->h0 = (int)std::max((float)0, h0);
      i->w0 = (int)std::max((float)0, w0);
      i->height = (int) MIN(height + h0,
		      (int) image_height) - i->h0;
      i->width = (int) MIN(width + w0,
		     (int) image_width) - i->w0;
    }
  }
	
  // prune a list of detections, take average vote for overlapping areas
  template <typename T, class Tstate>
  void detector<T,Tstate>::prune_vote(vector<bbox*> &raw_bboxes,
				      vector<bbox*> &pruned_bboxes,
				      float max_match,
				      bool same_class_only,
				      float min_hcenter_dist,
				      float min_wcenter_dist,
				      int classid) {
    // for each bbox, check that matching with other bboxes does not go beyond
    // the maximum authorized matching score (0 means no overlap, 1 is identity)
    // and only keep ones with highest score when overlapping.
    size_t ib, jb;
    bbox *i, *j;
    for (ib = 0; ib < raw_bboxes.size(); ++ib) {
      i = raw_bboxes[ib];
      if (i && i->class_id != bgclass && i->class_id != mask_class) {
	// center of the box
	rect<uint> this_bbox(i->h0, i->w0, i->height, i->width);
	bool add = true;
	vector<bbox*> overlaps;
	overlaps.push_back(i);
	// check each other bbox
	for (jb = 0; jb < raw_bboxes.size() && add; ++jb) {
	  j = raw_bboxes[jb];
	  if (j && i != j) {
	    rect<uint> other_bbox(j->h0, j->w0, j->height, j->width);
	    float match = this_bbox.match(other_bbox);
	    bool overlap = false;
	    if (match >= max_match)
	      overlap = true; // there is overlap
// 	    // forbid centers to be closer than min dist to each other in each axis
// 	    if ((this_bbox.center_hdistance(other_bbox) < min_hcenter_dist
// 		 && this_bbox.center_wdistance(other_bbox) < min_wcenter_dist)
// 		|| (other_bbox.center_hdistance(this_bbox) < min_hcenter_dist
// 		    && other_bbox.center_wdistance(this_bbox) < min_wcenter_dist))
// 	      overlap = true;
	    // if same_class_only, allow pruning only if 2 bb are same class
	    bool allow_pruning = !same_class_only ||
	      (same_class_only && i->class_id == j->class_id);
	    // keep only 1 bb if overlap and pruning is ok
	    if (overlap && allow_pruning) {
	      overlaps.push_back(j);
	    }
	  }
	}
	// ********** TODO: delete non kept bboxes

	// take mean of overlaps
	bbox mean = mean_bbox(overlaps, .01, same_class_only ? 
			      i->class_id : classid);
	bbox_parts *p = new bbox_parts(mean);
	for (uint k = 0; k < overlaps.size(); ++k)
	  p->add_part(*(overlaps[k]));
	pruned_bboxes.push_back(p);
      }
    }
#ifdef __DEBUG__
    mout << "Pruned " << raw_bboxes.size() << " bboxes to "
	 << pruned_bboxes.size() << " mean bboxes." << endl;
#endif
  }
	
  template <typename T, class Tstate>
  void detector<T,Tstate>::prune_pedestrian(vector<bbox*> &raw_bboxes,
					    vector<bbox*> &pruned_bboxes, bool ped_only,
					    T threshold) {
    size_t ib, jb;
    bbox *i, *head, *trunk, *body;
    uint head_area, trunk_area, body_area;

    // our intermediate bbox vector where we're gonna add synthethized bb
    vector<bbox*> raw2, raw22;

    mout << "raw bboxes: " << raw_bboxes.size() << endl;
    // apply regular overlap pruning to regular bboxes,
    // but forbid different classes to prune each other.

    //    prune_vote(raw_bboxes, raw22, max_overlap2, true, min_hcenter_dist2, min_wcenter_dist2);
    //    prune_overlap(raw22, raw2, max_overlap2, true, 0.0, 0.0);

    prune_overlap(raw_bboxes, raw2, max_overlap, true, 0.0, 0.0, oheight, owidth);

    mout << "raw2: " << raw2.size() << endl;
    
    // build lists of bb for each class
    vector<bbox*> bb_head, bb_trunk, bb_body;
    int id_head = get_class_id("head");
    int id_trunk = get_class_id("trunk");
    int id_body = get_class_id("ped");
    int id_ht = get_class_id("ht");
    int id_tb = get_class_id("tb");
    int id_htb = get_class_id("htb");
    int id_hb = get_class_id("hb");
    int id_mean = get_class_id("mean");
    int id_h = get_class_id("h");
    int id_t = get_class_id("t");
    for (ib = 0; ib < raw2.size(); ++ib) {
      i = raw2[ib];
      if (i->class_id == id_head)
	bb_head.push_back(i);
      else if (i->class_id == id_trunk)
	bb_trunk.push_back(i);
      else if (i->class_id == id_body)
	bb_body.push_back(i);
    }
     mout << "heads: " << bb_head.size() << " trunk: " << bb_trunk.size()
	  << " body: " << bb_body.size() << " raw2: " << raw2.size() << endl;

//      // add bodies for single heads
//     for (ib = 0; ib < bb_head.size(); ++ib) {
//       head = bb_head[ib];
//       // add an estimated body box
//       bbox_parts *b = new bbox_parts(*head);
//       b->class_id = id_h; // create new class
//       b->add_part(*head);
//       // estimate ped bb
//       float addw = (b->width / (float) .3) - b->width;
//       b->height = (uint) (b->height / (float) .3);
//       b->width = (uint) (b->width / (float) .3);
//       //b->h0 += (uint) (b->height * (float) .3);
//       // re-center bbox and cut out of image borders if necessary
//       int outbound = b->w0 - (int) (addw / 2.0);
//       if (outbound < 0) {
// 	b->w0 = 0;
// 	b->width += outbound;
//       } else
// 	b->w0 = outbound;
//       // check upper bounds
//       if (b->h0 + b->height > image.dim(0))
// 	b->height -= b->h0 + b->height - image.dim(0);
//       if (b->w0 + b->width > image.dim(1))
// 	b->width -= b->w0 + b->width - image.dim(1);
//       raw2.push_back(b);
//     }

     // add bodies for single trunks
    for (ib = 0; ib < bb_trunk.size(); ++ib) {
      trunk = bb_trunk[ib];
      // add an estimated body box
      bbox_parts *b = new bbox_parts(*trunk);
      b->class_id = id_t; // create new class
      b->add_part(*trunk);
      // estimate ped bb
      float addw = (b->width / (float) .6) - b->width;
      b->height = (uint) (b->height / (float) .6);
      b->width = (uint) (b->width / (float) .6);
      //b->h0 += (uint) (b->height * (float) .3);
      // re-center bbox and cut out of image borders if necessary
      int outbound = b->w0 - (int) (addw / 2.0);
      if (outbound < 0) {
	b->w0 = 0;
	b->width += outbound;
      } else
	b->w0 = outbound;
      // check upper bounds
      if (b->h0 + b->height > image.dim(0))
	b->height -= b->h0 + b->height - image.dim(0);
      if (b->w0 + b->width > image.dim(1))
	b->width -= b->w0 + b->width - image.dim(1);
      raw2.push_back(b);
    }


    float min_ht_overlap = .65; // minimum head/trunk overlap
    float min_tb_overlap = .65; // minimum trunk/body overlap
    float min_ht_ratio = .2, max_ht_ratio = .5; // min/max head/trunk area ratio
    float min_tb_ratio = .2, max_tb_ratio = .5; // min/max trunk/body area ratio
    float min_hb_ratio = .1, max_hb_ratio = .2; // min/max head/body area ratio
    // internal component bottom bar shouldnt be lower than external component
    // height * this ratio + h0
    float max_internal_height = .8; 
    float max_internal_height_hb = .5; 
    // max bonus for multiple confidences, per contributing class (> 1)
    // avg conf * max bonus * number of contributing classes (> 1)
    float conf_max_bonus = 0; //.1; 

    // build list of head/trunk overlapping bb pairs
    vector<bbox*> ht;
    for (ib = 0; ib < bb_trunk.size(); ++ib) {
      trunk = bb_trunk[ib];
      for (jb = 0; jb < bb_head.size(); ++jb) {
	head = bb_head[jb];
	head_area = head->area();
	trunk_area = trunk->area();
	// sanity check
	if (head_area == 0 || trunk_area == 0)
	  eblerror("zero-area bboxes should exists");
	// ignore if head is bigger than trunk
	if (head_area > trunk_area)
	  continue ;
	// ignore if head doesn't overlap by at least min_ht_overlap with trunk
	if (head->min_overlap(*trunk) < min_ht_overlap)
	  continue ;
	// head/trunk area ratio has to be in a certain range
	float r = head_area / (float) trunk_area;
	if (r < min_ht_ratio || r > max_ht_ratio)
	  continue ;
	// head has to be in the top end of the trunk
	if (head->h1() > trunk->h0 + trunk->height * max_internal_height)
	  continue ;
	// we met all conditions, add a new trunk box
	bbox_parts *b = new bbox_parts(*trunk);
	b->confidence += head->confidence; // accumulate confidences
	// remember which parts we used to compose this bb
	b->add_part(*head);
	ht.push_back(b);
	// also add an estimated body box
	b = new bbox_parts(*trunk);
	// remember which parts we used to compose this bb
	b->add_part(*head);
	b->confidence += head->confidence; // accumulate confidences
	// normalize confidence
	b->confidence /= 2; // 2 classes contributed, take mean conf
	b->confidence += b->confidence * conf_max_bonus * 2; // add bonus
	b->class_id = id_ht; // create new class
	// estimate ped bb
	float addw = (b->width / (float) .6) - b->width;
	b->height = (uint) (b->height / (float) .6);
	b->width = (uint) (b->width / (float) .6);
	//	b->h0 += (uint) (b->height * (float) .3);
	// re-center bbox and cut out of image borders if necessary
	int outbound = b->w0 - (int) (addw / 2.0);
	if (outbound < 0) {
	  b->w0 = 0;
	  b->width += outbound;
	} else
	  b->w0 = outbound;
	// check upper bounds
	if (b->h0 + b->height > image.dim(0))
	  b->height -= b->h0 + b->height - image.dim(0);
	if (b->w0 + b->width > image.dim(1))
	  b->width -= b->w0 + b->width - image.dim(1);
	raw2.push_back(b);
      }
    }

    mout << "head/trunk: " << ht.size() << " raw2: " << raw2.size() << endl;

    // add head/trunk/body combinations to raw2
    for (ib = 0; ib < bb_body.size(); ++ib) {
      body = bb_body[ib];
      // loop over already found combinations of head/trunk
      for (jb = 0; jb < ht.size(); ++jb) {
	trunk = ht[jb];
	body_area = body->area();
	trunk_area = trunk->area();
	// sanity check
	if (body_area == 0 || trunk_area == 0)
	  eblerror("zero-area bboxes should exists");
	// ignore if trunk is bigger than body
	if (trunk_area > body_area)
	  continue ;
	// ignore if trunk doesn't overlap by at least min_tb_overlap with body
	if (trunk->min_overlap(*body) < min_tb_overlap)
	  continue ;
	// trunk has to be in the top end of the body
	if (trunk->h1() > body->h0 + body->height * max_internal_height)
	  continue ;
	// trunk/body area ratio has to be in a certain range
	float r = trunk_area / (float) body_area;
	if (r < min_tb_ratio || r > max_tb_ratio)
	  continue ;
	//	cout << "****** adding htb bbox" << endl;
	// we met all conditions, add a new box
	bbox_parts *b = new bbox_parts(*body);
	// remember which parts we used to compose this bb
	b->add_part(*trunk);
	b->confidence += trunk->confidence; // accumulate confidences
	// normalize confidence
	b->confidence /= 3; // 3 classes contributed, take mean conf
	b->confidence += b->confidence * conf_max_bonus * 3; // add bonus
	b->class_id = id_htb; // create new class
	//	b->class_id = id_body;
	raw2.push_back(b);
      }
    }

    mout << "head/trunk/body raw2: " << raw2.size() << endl;

    // add head/body combinations to raw2
    for (ib = 0; ib < bb_body.size(); ++ib) {
      body = bb_body[ib];
      // loop over heads
      for (jb = 0; jb < bb_head.size(); ++jb) {
	head = bb_head[jb];
	body_area = body->area();
	head_area = head->area();
	// sanity check
	if (head_area == 0 || body_area == 0)
	  eblerror("zero-area bboxes should exists");
	// ignore if head is bigger than body
	if (head_area > body_area)
	  continue ;
	// ignore if head doesn't overlap by at least min_tb_overlap with body
	if (head->min_overlap(*body) < min_tb_overlap)
	  continue ;
	// head has to be in the top end of the body
	if (head->h1() > body->h0 + body->height * max_internal_height_hb)
	  continue ;
	// head/body area ratio has to be in a certain range
	float r = head_area / (float) body_area;
	if (r < min_hb_ratio || r > max_hb_ratio)
	  continue ;
	// we met all conditions, add a new box
	bbox_parts *b = new bbox_parts(*body);
	// remember which parts we used to compose this bb
	b->add_part(*head);
	b->confidence += head->confidence; // accumulate confidences
	// normalize confidence
	b->confidence /= 2; // 2 classes contributed, take mean conf
	b->confidence += b->confidence * conf_max_bonus * 2; // add bonus
	b->class_id = id_hb; // create new class
	//	b->class_id = id_body;
	raw2.push_back(b);
      }
    }

    mout << "head/body raw2: " << raw2.size() << endl;

    // add trunk/body combinations to raw2
    for (ib = 0; ib < bb_body.size(); ++ib) {
      body = bb_body[ib];
      // loop over heads
      for (jb = 0; jb < bb_trunk.size(); ++jb) {
	trunk = bb_trunk[jb];
	body_area = body->area();
	trunk_area = trunk->area();
	// sanity check
	if (body_area == 0 || trunk_area == 0)
	  eblerror("zero-area bboxes should exists");
	// ignore if trunk is bigger than body
	if (trunk_area > body_area)
	  continue ;
	// ignore if trunk doesn't overlap by at least min_tb_overlap with body
	if (trunk->min_overlap(*body) < min_tb_overlap)
	  continue ;
	// trunk has to be in the top end of the body
	if (trunk->h1() > body->h0 + body->height * max_internal_height)
	  continue ;
	// trunk/body area ratio has to be in a certain range
	float r = trunk_area / (float) body_area;
	if (r < min_tb_ratio || r > max_tb_ratio)
	  continue ;
	// we met all conditions, add a new box
	bbox_parts *b = new bbox_parts(*body);
	// remember which parts we used to compose this bb
	b->add_part(*trunk);
	b->confidence += trunk->confidence; // accumulate confidences
	// normalize confidence
	b->confidence /= 2; // 2 classes contributed, take mean conf
	b->confidence += b->confidence * conf_max_bonus * 2; // add bonus
	//b->class_id = id_body;
	b->class_id = id_tb; // create new class
	raw2.push_back(b);
      }
    }

    uint k, j, n = 0;
    bbox_parts *b1, *b2;
    vector<bbox*> raw3;
    if (share_parts) { // allow parts sharing
      for (k = 0; k < raw2.size(); ++k)
	raw3.push_back(raw2[k]);
    } else {
      // if bounding boxes are composed from same parts, keep the best one
      bool keep;
      for (k = 0; k < raw2.size(); ++k) {
	keep = true;
	b1 = (bbox_parts*) raw2[k];
	for (j = 0; j < raw2.size(); ++j) {
	  if (k == j)
	    continue ;
	  b2 = (bbox_parts*) raw2[j];
	  if (b1->share_parts(*b2)) {
	    if (b1->confidence < b2->confidence) {
	      keep = false;
	      break ;
	    }
	  }
	}
	if (keep)
	  raw3.push_back(b1);
	else 
	  n++;
      }
      mout << "Trashed " << n << "/" << raw2.size()
	   << " bboxes because they shared part with higher "
	   << "confidence bbox." << endl;
    }
    raw2.clear();

    // delete temporary bboxes
    for (jb = 0; jb < ht.size(); ++jb)
      delete(ht[jb]);
    ht.clear();
    
    if (ped_only) {
      vector<bbox*> raw4;
      for (ib = 0; ib < raw3.size(); ++ib) {
	i = raw3[ib];
	if (i->class_id == id_body || i->class_id == id_ht
	    || i->class_id == id_htb || i->class_id == id_hb
	    || i->class_id == id_tb || i->class_id == id_h 
	    || i->class_id == id_t)
	  raw4.push_back(i);
	else
	  delete i;
      }
      raw3.clear();
      
      // apply regular overlap pruning to regular and synthetized bboxes,
      // but forbid different classes to prune each other.      
      vector<bbox*> raw5, raw6;
      if (mean_bb) {
	prune_vote(raw4, raw5, max_overlap2, false, min_hcenter_dist2, min_wcenter_dist2, id_mean);
	prune_overlap(raw5, raw6, max_overlap, false, min_hcenter_dist, min_wcenter_dist,
		      oheight, owidth);
      } else
	prune_overlap(raw4, raw6, max_overlap, false, min_hcenter_dist, min_wcenter_dist,
		      oheight, owidth);

      for(ib=0; ib < raw6.size();++ib) 
	if (raw6[ib]->confidence >= threshold)
	  pruned_bboxes.push_back(raw6[ib]);
    } else {
      // apply regular overlap pruning to regular and synthetized bboxes,
      // but forbid different classes to prune each other.
      prune_overlap(raw3, pruned_bboxes, max_overlap, 
		    true, min_hcenter_dist, min_wcenter_dist, oheight, owidth);
    }
    mout << "pruned_bboxes: " << pruned_bboxes.size() << endl;
  }
	
  //////////////////////////////////////////////////////////////////////////////
  // outputs smoothing

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_smoothing(uint type) {
    smoothing_type = type;
    idx<T> ker;
    switch (smoothing_type) {
    case 0: mout << "Outputs smoothing disabled." << endl; break ;
    case 1:
      ker = idx<T>(3, 3);
      ker.set(.3, 0, 0);
      ker.set(.5, 0, 1);
      ker.set(.3, 0, 2);
      ker.set(.5, 1, 0);
      ker.set(1 , 1, 1);
      ker.set(.5, 1, 2);
      ker.set(.3, 2, 0);
      ker.set(.5, 2, 1);
      ker.set(.3, 2, 2);
      idx_dotc(ker, (T) (1 / (double) idx_sum(ker)), ker);
      smoothing_kernel = ker;
      mout << "Smoothing outputs with kernel: " << endl;
      smoothing_kernel.printElems();
      break ;
    default:
      eblerror("Unknown smoothing type " << type);
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::smooth_outputs() {
    if (smoothing_type != 0) {
      uint hpad = (uint) (smoothing_kernel.dim(0) / 2);
      uint wpad = (uint) (smoothing_kernel.dim(1) / 2);
      idx_bloop1(output, outputs, void*) {
	idx<T> &outx = ((Tstate*) output.get())->x;
	intg h = outx.dim(1), w = outx.dim(2);
	idx<T> in(h + 2 * hpad, w + 2 * wpad);
	idx<T> inc = in.narrow(0, h, hpad);
	inc = inc.narrow(1, w, wpad);
	idx_clear(in);
	idx_bloop1(out, outx, T) {
	  idx_copy(out, inc);
	  idx_2dconvol(in, smoothing_kernel, out);
	}
      }
    }
  }
    
  //////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void detector<T,Tstate>::map_to_list(T threshold, vector<bbox*> &raw_bboxes) {
    bbox::init_instance_id(); // reset unique ids to start from zero.
    // make a list that contains the results
    //    idx<T> in0x(((Tstate*) inputs.get(0))->x);
    double original_h = image.dim(0);
    double original_w = image.dim(1);
    intg offset_h = 0, offset_w = 0;
    int scale_index = 0;
    { idx_bloop5(input, inputs, void*, output, outputs, void*,
		 r, results, void*, resolution, resolutions, uint,
		 obbox, original_bboxes, uint) {
	// image region in the input at this scale.
	rect<uint> robbox(obbox.get(0), obbox.get(1),
			  obbox.get(2), obbox.get(3));
	double in_h = (double)(((Tstate*) input.get())->x.dim(1));
	double in_w = (double)(((Tstate*) input.get())->x.dim(2));
	double out_h = (double)(((Tstate*) output.get())->x.dim(1));
	double out_w = (double)(((Tstate*) output.get())->x.dim(2));
	double neth = netdim.dim(1); // network's input height
	double netw = netdim.dim(2); // network's input width
	double scalehi = original_h / robbox.height; // input to original
	double scalewi = original_w / robbox.width; // input to original
	int image_h0 = (int) (robbox.h0 * scalehi);
	int image_w0 = (int) (robbox.w0 * scalewi);
	// offset factor in input map
	double offset_h_factor = (in_h - neth)
	  / std::max((double) 1, (out_h - 1));
	double offset_w_factor = (in_w - netw)
	  / std::max((double) 1, (out_w - 1));
	offset_w = 0;
	// loop on width
	idx_eloop1(ro, ((Tstate*) output.get())->x, T) {
	  offset_h = 0;
	  // loop on height
	  idx_eloop1(roo, ro, T) {
	    int classid = 0;
	    // loop on classes (and their targets)
	    idx_bloop1(target, targets, T) {
	      if ((classid == bgclass) || (classid == mask_class)) {
		classid++;
		continue ;
	      }
	      T conf, max2;
	      intg p;
	      bool ini = false;
	      switch (conf_type) {
	      case confidence_sqrdist:
		conf = 1.0 - ((idx_sqrdist(target, roo) - conf_shift)
			      / conf_ratio);
		break ;
	      case confidence_single: // simply return class' out (normalized)
		conf = (roo.get(classid) - conf_shift) / conf_ratio;
	      case confidence_max:
		conf = roo.get(classid);
		for (p = 0; p < roo.dim(0); ++p) {
		  if (p != classid) {
		    if (!ini) {
		      max2 = roo.get(p);
		      ini = true;
		    } else {
		      if (roo.get(p) > max2)
			max2 = roo.get(p);
		    }
		  }
		}
		conf = ((conf - max2) - conf_shift) / conf_ratio;
		break ;
	      default:
		eblerror("confidence type " << conf_type << " undefined");
	      }
	      if (conf >= threshold) {
		bbox_parts bb;
		bb.class_id = (int) classid; // Class
		bb.confidence = conf; // Confidence
		bb.scale_index = scale_index; // scale index
		// original image
		// bbox's rectangle in original image
		bb.h0 = (int) (offset_h * offset_h_factor * scalehi) - image_h0;
		bb.w0 = (int) (offset_w * offset_w_factor * scalewi) - image_w0;
		bb.height = (int) (neth * scalehi);
		bb.width = (int) (netw * scalewi);
		// input map
		bb.iheight = (int) in_h; // input h
		bb.iwidth = (int) in_w; // input w
		bb.ih0 = (int) (offset_h * offset_h_factor);
		bb.iw0 = (int) (offset_w * offset_w_factor);
		bb.ih = (int) neth;
		bb.iw = (int) netw;
		// output map
		bb.oheight = (int) out_h; // output height
		bb.owidth = (int) out_w; // output width
		bb.oh0 = offset_h; // answer height in output
		bb.ow0 = offset_w; // answer height in output
		raw_bboxes.push_back(new bbox_parts(bb));
	      }
	      classid++;
	    }
	    offset_h++;
	  }
	  offset_w++;
	}
	scale_index++;
      }}
  }
  
  template<typename T, class Tstate>
  void detector<T,Tstate>::pretty_bboxes(vector<bbox*> &bboxes,
					 std::ostream &out) {
    out << endl << "detector: ";
    if (bboxes.size() == 0)
      out << "no objects found." << endl;
    else {
      out << "found " << bboxes.size() << " objects." << endl;
      size_t ib;
      bbox *i;
      for (ib = 0 ; ib < bboxes.size(); ++ib) {
	i = bboxes[ib];
	out << "- " << (i->class_id < labels.dim(0) ? 
			(const char*) (labels[i->class_id].idx_ptr()):"");
	out << " with confidence " << i->confidence;
	out << " in scale #" << i->scale_index;
	out << " (" << image.dim(0) << "x" << image.dim(1);
	out << " / " << i->scaleh << "x" << i->scalew;
	out << " = " << i->iheight << "x" << i->iwidth << ")";
	out << endl;
	out << "  bounding box: top left " << i->h0 << "x" << i->w0;
	out << " and size " << i->height << "x" << i->width;
	out << " out position: " << i->oh0 << "x" << i->ow0;
	out << " in " << i->oheight << "x" << i->owidth;
	out << endl;
      }
    }
  }

  template<typename T, class Tstate>
  void detector<T,Tstate>::pretty_bboxes_short(vector<bbox*> &bboxes,
					       std::ostream &out) {
    if (bboxes.size() == 0)
      out << "no objects found." << endl;
    else {
      out << "found " << bboxes.size() << " objects." << endl;
      size_t ib; const bbox *i;
      for (ib = 0 ; ib < bboxes.size(); ++ib) {
	i = bboxes[ib];
	out << (i->class_id < labels.dim(0) ? 
		(const char*) labels[i->class_id].idx_ptr() : "")
	    << " " << (const bbox&) *i << endl;
      }
    }
  }
  
  template <typename T, class Tstate> template <class Tin>
  vector<bbox*>& detector<T,Tstate>::fprop(idx<Tin> &img, T threshold,
					   const char *frame_name) {
    // prepare image and resolutions
    prepare(img);
    // do a fprop for each scaled input, based on the 'image' slot prepared
    // by prepare().
    multi_res_fprop();
    // smooth outputs
    smooth_outputs();

    // find points that are local maxima spatial and class-wise
    // write result in m. rescale result to [0 1]
    // TODO: use connected components instead of fixed-size window local maxima?
    //mark_maxima(threshold);

    // clear previous bounding boxes
    size_t ib; bbox *i;
    for (ib = 0 ; ib < raw_bboxes.size(); ++ib) {
      i = raw_bboxes[ib];
      if (i)
	delete i;
    }
    raw_bboxes.clear();
    // get new bboxes
    map_to_list(pruning == pruning_pedestrian ? threshold_parts : threshold, 
		raw_bboxes);
    vector<bbox*> &bb = raw_bboxes;
    // non-maximum suppression
    nms(raw_bboxes, pruned_bboxes, threshold, oheight, owidth);
    bb = pruned_bboxes; // assign pruned bb to bb
    // save positive response input windows in save mode
    if (save_mode)
      save_bboxes(bb, save_dir, frame_name);
    // backward connections
    back_module<T, Tstate>* back = (back_module<T, Tstate>*)((layers<T,Tstate>&)thenet).find("back");
    if (back) {
      back->bb(bb);
    }
    // return bounding boxes
    return bb;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::nms(vector<bbox*> &raw, vector<bbox*> &pruned,
			       float threshold, 
			       uint image_height, uint image_width) {
    // prune bounding boxes btwn scales
    if (pruning != pruning_none) {
      pruned.clear(); // reset pruned bb vector
      switch (pruning) {
      case pruning_overlap: 
	prune_overlap(raw, pruned, max_overlap, false, min_hcenter_dist, 
		      min_wcenter_dist, threshold, image_height, 
		      image_width, same_scale_mhd, same_scale_mwd); 
	break ;
      case pruning_pedestrian:
	prune_pedestrian(raw, pruned, ped_only, threshold); break ;
      default: break;
      }
    } else
      pruned = raw;
    // sort bboxes by confidence (most confident first)
    sort_bboxes(pruned);
    // print results
    if (!silent)
      pretty_bboxes(pruned);
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::save_bboxes(const vector<bbox*> &bboxes,
				       const string &dir,
				       const char *frame_name) {
#ifdef __NOSTL__
    eblerror("save_bboxes not implemented");
#else
    string classname;
    ostringstream fname, cmd;
    Tstate *input = NULL;
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
      input = (Tstate*) inputs.get((*bbox)->scale_index);
      inpp = input->x.narrow(1, (*bbox)->ih, (*bbox)->ih0);
      inpp = inpp.narrow(2, (*bbox)->iw, (*bbox)->iw0);
      //inpp = inpp.shift_dim(0, 2); // put channels back to 3rd position
      // get bbox of original input
      if ((*bbox)->height + (*bbox)->h0 > image.dim(0) || 
	  (*bbox)->width + (*bbox)->w0 > image.dim(1) || 
	  (*bbox)->h0 < 0 || (*bbox)->w0 < 0)
	cerr << "warning: trying to crop bbox outside of image bounds: bbox "
	     << **bbox << " in image " << image << endl;
      // make sure we don't try to crop outside of image bounds
      int h = std::max(0, (*bbox)->h0), w = std::max(0, (*bbox)->w0);
      int height = std::min((int) image.dim(0) - h, h + (*bbox)->height);
      int width = std::min((int) image.dim(1) - w, h + (*bbox)->width);
      inorig = image.narrow(0, height, h);
      inorig = inorig.narrow(1, width, w);
      // save preprocessed image as lush mat
      fname.str("");
      fname << dir_pp[(*bbox)->class_id]
	    << frame_name << "_" << classname << setw(3) << setfill('0')
	    << save_counts[(*bbox)->class_id] << MATRIX_EXTENSION;
      if (save_matrix(inpp, fname.str()))
	mout << "saved " << fname.str() << " (confidence "
	     << (*bbox)->confidence << ")" << endl;
      // save original image as png
      fname.str("");
      fname << dir_orig[(*bbox)->class_id] << frame_name << "_"  << classname
	    << setw(3) << setfill('0') << save_counts[(*bbox)->class_id]
	    << ".png";
      if (save_image(fname.str(), inorig, "png"))
	mout << "saved " << fname.str() << " (confidence "
	     << (*bbox)->confidence << ")" << endl;
      // increment file counter
      save_counts[(*bbox)->class_id]++;
      // stop if reached max save per frame
      if (i >= save_max_per_frame)
	break ;
    }
#endif
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::sort_bboxes(vector<bbox*> &bboxes) {
    bbox *tmp = NULL;
    float confidence;
    for (int i = 1; i < (int) bboxes.size(); ++i) {
      int k = i;
      confidence = bboxes[k]->confidence;
      for (int j = i - 1; j >= 0; j--) {
	if (confidence > bboxes[j]->confidence) {
	  // swap them
	  tmp = bboxes[j];
	  bboxes[j] = bboxes[k];
	  bboxes[k] = tmp;
	  k = j;
	}
      }
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::add_class(const char *name) {
    if (!name)
      eblerror("cannot add empty class name");
    mout << "Adding class " << name << endl;
    intg len = (std::max)(labels.dim(1), (intg) strlen(name) + 1);
    idx<ubyte> l2(labels.dim(0) + 1, len);
    idx_clear(l2);
    idx<ubyte> l1 = l2.narrow(0, labels.dim(0), 0);
    l1 = l1.narrow(1, labels.dim(1), 0);
    idx_copy(labels, l1);
    for (uint i = 0; i < strlen(name); ++i)
      l2.set(name[i], labels.dim(0), i);
    labels = l2;
    mout << "New class list is:";
    uint i = 0;
    idx_bloop1(lab, labels, ubyte)
      mout << " " << (const char *) lab.idx_ptr() << "(" << i++ << ")";
    mout << endl;
  }

  /////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  uint detector<T,Tstate>::get_total_saved() {
    uint total = 0;
    for (size_t i = 0; i < save_counts.size(); ++i)
      total += save_counts[i];
    return total;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_save_max_per_frame(uint max) {
    save_max_per_frame = max;
  }

  template <typename T, class Tstate>
  vector<idx<T> >& detector<T,Tstate>::get_originals() {
    if (bodetections) // recompute only if not up-to-date
      return odetections;
    idx<T> input;
    size_t i; bbox *bb;
    // clear vector
    odetections.clear();
    // loop on bounding boxes
    for (i = 0; i < pruned_bboxes.size(); ++i) {
      bb = pruned_bboxes[i];
      // exclude background class
      if ((bb->class_id == bgclass) || (bb->class_id == mask_class))
	continue ;
      // get bbox of input
      input = image.narrow(0, bb->height, bb->h0);
      input = input.narrow(1, bb->width, bb->w0);
      odetections.push_back(input);
    }
    bodetections = true;
    return odetections;
  }
  
  template <typename T, class Tstate>
  vector<idx<T> >& detector<T,Tstate>::get_preprocessed() {
    if (bppdetections) // recompute only if not up-to-date
      return ppdetections;
    idx<T> input;
    Tstate *sinput = NULL;
    size_t i; bbox *bb;

    // clear vector
    ppdetections.clear();
    // loop on bounding boxes
    for (i = 0; i < pruned_bboxes.size(); ++i) {
      bb = pruned_bboxes[i];
      // exclude background class
      if ((bb->class_id == bgclass) || (bb->class_id == mask_class))
	continue ;
      // get bbox of input
      sinput = (Tstate*) inputs.get(bb->scale_index);
      input = sinput->x.narrow(1, bb->ih, bb->ih0);
      input = input.narrow(2, bb->iw, bb->iw0);
      ppdetections.push_back(input);
    }
    bppdetections = true;
    return ppdetections;
  }
  
  template <typename T, class Tstate>
  idx<T> detector<T,Tstate>::get_mask(string &classname) {
    int id = get_class_id(classname), i = 0;
    idxdim d(image.dim(0), image.dim(1));
    if (mask.get_idxdim() != d)
      mask = idx<T>(d);
    if (id == -1) { // class not found
      merr << "warning: unknown class " << classname << endl;
      idx_clear(mask);
      return mask;
    }
    // merge all outputs of class 'id' into mask
    idx_bloop3(input, inputs, void*, output, outputs, void*,
	       obbox, original_bboxes, uint) {
      idx<T> in = ((Tstate*) input.get())->x.select(0, 0);
      idx<T> out = ((Tstate*) output.get())->x.select(0, id);
      rect<uint> o(obbox.get(0), obbox.get(1),
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
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::multi_res_fprop() {
    // timing
    timer t;
    t.start();
    for (intg i = 0; i < nresolutions; ++i) {

#ifdef __DUMP_STATES__
      RESET_DUMP(); // set dump counter to zero
      DUMP_PREFIX("dump" << i);
#endif

      preprocess_resolution(i);
      
#ifdef __DUMP_STATES__
      DUMP(input->x, "detector_input_");
#endif
      
      thenet.fprop(*input, *output);
      
#ifdef __DUMP_STATES__
      DUMP(output->x, "detector_output_");
#endif
      
      if (optimization_swap) { // swap output and input
      	tmp = input;
      	input = output;
      	output = tmp;
      }
    }
    mout << "net_processing=" << t.elapsed_ms() << endl;
  }

  template <typename T, class Tstate> template <class Tin>
  void detector<T,Tstate>::prepare(idx<Tin> &img) {
    // tell detections vectors they are not up-to-date anymore
    bodetections = false;
    bppdetections = false;
    // cast input if necessary
    idx<T> img2, tmp;
#ifndef __NOSTL__
    if (typeid(T) == typeid(Tin)) // input same type as net, shallow copy
      img2 = (idx<T>&) img;
    else
#endif
      { // deep copy to cast input into net's type
	img2 = idx<T>(img.get_idxdim());
	idx_copy(img, img2);
      }
    if (img2.order() == 2) {
      image = idx<T>(img2.dim(0), img2.dim(1), 1);
      tmp = image.select(2, 0);
      idx_copy(img2, tmp);  // TODO: avoid copy just to augment order
    } else
      image = img2;
#ifdef __DEBUG__
    mout << "Image in detector is " << image << " with range "
	 << idx_min(image) << ", " << idx_max(image) << endl;
#endif
    // if input size had changed, reinit resolutions
    if (!(input_dim == img.get_idxdim())) {
      init(img.get_idxdim());
    }
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::preprocess_resolution(uint res) {
    if (res >= nresolutions)
      eblthrow("cannot request resolution " << res << ", there are only "
	       << nresolutions << " resolutions");
    // create a fstate_idx pointing to our image
    idx<T> imres = image.shift_dim(2, 0);
    Tstate iminput(imres);
    idx<uint> bbox = original_bboxes.select(0, res);
    // select input/outputs buffers
    output = (Tstate*)(outputs.get(res));
    if (!mem_optimization || keep_inputs) // we use different bufs for each res
      input = (Tstate*)(inputs.get(res));
    else
      input = minput;
    // resize and preprocess input
    resizepp.set_dimensions(resolutions.get(res, 0), resolutions.get(res, 1));
    //    rect<int> outr(0, 0, resolutions.get(res, 2), resolutions.get(res, 3));
    rect<int> outr(0, 0, resolutions.get(res, 0), resolutions.get(res, 1));
    resizepp.set_output_region(outr);
    resizepp.fprop(iminput, *input);
    // memorize original input's bbox in resized input
    rect<int> bb = resizepp.get_original_bbox();
    bbox.set(bb.h0, 0);
    bbox.set(bb.w0, 1);
    bbox.set(bb.height, 2);
    bbox.set(bb.width, 3);
    // add bias and multiply by coeff if necessary
    if (bias != 0)
      idx_addc(input->x, bias, input->x);
    if (coef != 1)
      idx_dotc(input->x, coef, input->x);

    DEBUG("preprocessed input range: " << idx_min(input->x) << ", "
	  << idx_max(input->x) << endl
	  << "resized input (" << imres << ") to resolution " << res
	  << " (" << resolutions.get(res, 0) << "x" << resolutions.get(res, 1)
	  << "): " << input->x);
  }

} // end namespace ebl

#endif
