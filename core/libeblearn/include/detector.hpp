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
  detector<T,Tstate>::
  detector(module_1_1<T,Tstate> &thenet_, vector<string> &labels_,
	   answer_module<T,T,T,Tstate> *answer_,
	   resizepp_module<T,Tstate> *resize, const char *background,
	   std::ostream &o, std::ostream &e,
	   bool adapt_scales_)
    : thenet(thenet_), resizepp(resize), resizepp_delete(false),
      input(NULL), minput(NULL), netdim_fixed(false),
      bgclass(-1), mask_class(-1), pnms(NULL), scales_step(0), min_scale(1.0),
      max_scale(1.0), restype(ORIGINAL), silent(false), save_mode(false),
      save_dir(""), save_counts(labels_.size(), 0), min_size(0), max_size(0),
      bodetections(false), bppdetections(false), mem_optimization(false),
      optimization_swap(false), keep_inputs(true), hzpad(0), wzpad(0),
      mout(o), merr(e), smoothing_type(0), initialized(false),
      bboxes_off(false), adapt_scales(adapt_scales_), answer(answer_),
      ignore_outsiders(false), corners_inference(0), corners_infered(false),
      pre_threshold(0), bbox_decision(0) {
    // // make sure the top module is an answer module
    // module_1_1<T,Tstate> *last = thenet.last_module();
    // if (!dynamic_cast<answer_module<T,Tstate>*>(last))
    //   eblerror("expected last module to be of type answer_module but found: "
    // 	       << last->name());
    scaler_mode = false;
    if (answer && (dynamic_cast<scaler_answer<T,T,T,Tstate>*>(answer) ||
		   dynamic_cast<scalerclass_answer<T,T,T,Tstate>*>(answer)))
      scaler_mode = true;
    if (answer) mout << "Using answer module: " << answer->describe() << endl;
    // look for resize module in network
    if (!resizepp) {
      resizepp = arch_find(&thenet, resizepp);
      if (resizepp) mout << "Found a resizepp module in network: "
			 << resizepp->describe() << endl;
      else mout << "No resizepp module found in network." << endl;
    }
    // set default resizing module
    if (!resizepp) {
      resizepp = new resizepp_module<T,Tstate>;
      resizepp->set_name("detector resizpp");
      cout << "Using default resizing module: " << resizepp->describe() << endl;
      resizepp_delete = true;
    }
    labels = labels_;
    mout << "Classes labels: " << labels << endl;
// #ifdef __ANDROID__ // TODO: temporary
//     bgclass = 0;
// #else
    //#endif
    // initilizations
    save_max_per_frame = limits<uint>::max();
    diverse_ordering = false;
    // set outpout streams of network
    thenet.set_output_streams(o, e);
    update_merge_alignment();
  }

  template <typename T, class Tstate>
  detector<T,Tstate>::~detector() {
    if (resizepp_delete && resizepp) delete resizepp;
    if (pnms) delete pnms;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_scaling_original() {
    nscales = 1;
    restype = ORIGINAL;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_scaling_type(t_scaling type) {
    restype = type;
    mout << "Setting scaling to type " << type << " (";
    switch (restype) {
    case MANUAL: mout << "MANUAL"; break ;
    case SCALES: mout << "SCALES"; break ;
    case NSCALES: mout << "NSCALES"; break ;
    case SCALES_STEP: mout << "SCALES_STEP"; break ;
    case ORIGINAL: mout << "ORIGINAL"; break ;
    case NETWORK: mout << "NETWORK"; break ;
    case SCALES_STEP_UP: mout << "SCALES_STEP_UP"; break ;
    default:
      eblerror("unknown type");
    }
    mout << ")" << endl;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_resolutions(const midxdim &scales_) {
    restype = MANUAL;
    manual_scales = scales_;
    if (manual_scales.size() == 0)
      eblerror("expected at least 1 scale but found 0");
    // add the feature dimension for each scale
    for (uint i = 0; i < manual_scales.size(); ++i) {
      idxdim &d = manual_scales[i];
      d.insert_dim(0, 1);
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_resolutions(const vector<double> &factors) {
    restype = SCALES;
    scale_factors = factors;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_resolution(double factor) {
    restype = SCALES;
    scale_factors.clear();
    scale_factors.push_back(factor);
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_resolutions(int nscales_) {
    nscales = (uint) nscales_;
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
    if (hzpad_ != 0 || wzpad_ != 0) {
      if (initialized) get_netdim(indim.order());
      else get_netdim(3);
      hzpad = (uint) (hzpad_ * netdim.dim(1));
      wzpad = (uint) (wzpad_ * netdim.dim(2));
      resizepp->set_zpads(hzpad, wzpad);
      mout << "Adding zero padding on input (on each side): hpad: "
	   << hzpad << " wpad: " << wzpad << " (" << hzpad_ << ","
	   << wzpad_ << " * " << netdim << ")" << endl;
      if (hzpad_ > 1 || wzpad_ > 1)
	eblerror("zero padding coeff should be in [0 1] range");
    }
  }

  template <typename T, class Tstate>
  int detector<T,Tstate>::get_class_id(const string &name) {
    for (uint i = 0; i < labels.size(); ++i)
      if (!strcmp(labels[i].c_str(), name.c_str()))
	return i;
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
  void detector<T,Tstate>::set_raw_thresholds(vector<float> &t) {
    mout << "Using multiple thresholds for raw bbox extractions: " << t << endl;
    raw_thresholds = t;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::
  set_nms(t_nms type, float pre_threshold_, float post_threshold,
	  float pre_hfact, float pre_wfact, float post_hfact, float post_wfact,
	  float woverh, float max_overlap, float max_hcenter_dist,
	  float max_wcenter_dist, float vote_max_overlap,
	  float vote_max_hcenter_dist, float vote_max_wcenter_dist) {
    pre_threshold = pre_threshold_;
    if (pnms) delete pnms;
    switch (type) {
    case nms_none: break ; // none
    case nms_overlap: // traditional overlap only
      pnms = new nms
	(post_threshold, max_overlap, max_hcenter_dist, max_wcenter_dist,
	 pre_hfact, pre_wfact, post_hfact, post_wfact, woverh, mout, merr);
      break ;
    case nms_voting: // voting only
      pnms = new voting_nms
	(post_threshold, vote_max_overlap, vote_max_hcenter_dist,
	 vote_max_wcenter_dist,
	 pre_hfact, pre_wfact, post_hfact, post_wfact, woverh, mout, merr);
      break ;
    case nms_voting_overlap: // voting + traditional overlap
      pnms = new voting_nms
	(post_threshold, max_overlap, max_hcenter_dist, max_wcenter_dist,
	 pre_hfact, pre_wfact, post_hfact, post_wfact, woverh,
	 vote_max_overlap, vote_max_hcenter_dist, vote_max_wcenter_dist,
	 mout, merr);
      break ;
    default: // unknown
      eblerror("unknown type of nms " << type);
    }
    mout << "Non-maximum suppression (nms): "
	 << (pnms ? pnms->describe() : "none") << endl;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_scaler_mode(bool set) {
    scaler_mode = set;
    mout << "Scaler mode is "
	 << (scaler_mode ? "enabled" : "disabled") << "." << endl;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_netdim(idxdim &d) {
    netdim = d;
    netdim.insert_dim(0, 1);
    netdim_fixed = true;
    mout << "Manually setting network's minimum input to " << d << endl;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_mem_optimization(Tstate &in, Tstate &out,
						bool keep_inputs_) {
    eblwarn("mem optimization temporarly broken because out is now mstate");
    // mout << "Optimizing memory usage by using only 2 alternating buffers";
    // mem_optimization = true;
    // keep_inputs = keep_inputs_;
    // mout << " (and " << (keep_inputs ? "":"not ")
    // 	 << "keeping multi-scale inputs)";
    // minput = &in;
    // input = &in;
    // output = &out;
    // // remember if we need to swap buffers because of odd operations.
    // optimization_swap = !thenet.optimize_fprop(*input, *output);
    // mout << endl;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_outputs_dumping(const char *name) {
    outputs_dump = name;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_bboxes_off() {
    bboxes_off = true;
  }

  template <typename T, class Tstate>
  vector<string>& detector<T,Tstate>::get_labels() {
    return labels;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_ignore_outsiders() {
    ignore_outsiders = true;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_corners_inference(uint type) {
    mout << "Setting corners inference type to " << type << endl;
    corners_inference = type;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_bbox_decision(uint type) {
    bbox_decision = type;
    mout << "Setting bbox decision type to " << type << endl;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::set_bbox_scalings(mfidxdim &scalings) {
    bbox_scalings = scalings;
    mout << "Setting bbox scalings to " << bbox_scalings << endl;
  }

  //////////////////////////////////////////////////////////////////////////////
  // initialization

  template <typename T, class Tstate>
  void detector<T,Tstate>::init(idxdim &dsample, const char *frame_name) {
    initialized = true;
    indim = dsample;
    // the network's minimum input dimensions
    get_netdim(dsample.order());
    // mout << "Network's minimum input dimensions are: " << netdim
    // 	 << thenet.pretty(netdim) << endl;
    // minimum input dimensions: factor of network's minimum input
    idxdim mindim = netdim * min_scale;
    // if (mindim.dim(1) + hzpad * 2 < netdim.dim(1))
    //   mindim.setdim(1, netdim.dim(1) - hzpad * 2);
    // if (mindim.dim(2) + wzpad * 2 < netdim.dim(2))
    //   mindim.setdim(2, netdim.dim(2) - wzpad * 2);
    mindim.setdim(0, dsample.dim(0)); // feature dimension is not scaled
    // maximum input dimensions: factor of original input
    idxdim maxdim = dsample * max_scale;
    for (uint i = 1; i < maxdim.order(); ++i)
      if (maxdim.dim(i) < netdim.dim(i))
	maxdim.setdim(i, netdim.dim(i));
    maxdim.setdim(0, dsample.dim(0)); // feature dimension is not scaled
    // determine scales
    compute_scales(scales, netdim, mindim, maxdim, dsample, restype, nscales,
		   scales_step, frame_name);
    // reallocate buffers if number of scales has changed
    if (scales.size() != ppinputs.size()) {
      EDEBUG("reallocating input and output buffers");
      DEBUGMEM_PRETTY("detector init scales");
      ppinputs.clear();
      outputs.clear();
      answers.clear();
      actual_scales.clear();
      // allocate buffers
      idxdim order(mindim);
      order.setdims(1); // minimum dims
      for (uint i = 0; i < scales.size(); ++i) {
	mstate<Tstate> *ppin = new mstate<Tstate>();
	ppin->push_back(new Tstate(order));
	ppinputs.push_back(ppin);
	mstate<Tstate> *ppout = new mstate<Tstate>();
	ppout->push_back(new Tstate(order));
	outputs.push_back(ppout);
	answers.push_back(new mstate<Tstate>());
      }
      DEBUGMEM_PRETTY("detector end of init scales");
      // copy ideal scales to actual scales vector (to be modified later)
      actual_scales.copy(scales);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // scaling methods

  template <typename T, class Tstate>
  void detector<T,Tstate>::
  compute_scales(midxdim &scales, idxdim &netdim, idxdim &mindim,
		 idxdim &maxdim, idxdim &indim, t_scaling type, uint nscales,
		 double scales_step, const char *frame_name) {
    // fill scales based on scaling type
    scales.clear();
    if (!silent)
      mout << "Scales: input: " << indim << " min: " << netdim
	   << " max: " << maxdim << endl
	   << "Scaling type " << type << ": ";
    switch (type) {
    case ORIGINAL:
      if (!silent) mout << "1 scale only, the image's original scale." << endl;
      scales.push_back(indim);
      break ;
    case MANUAL:
      scales = manual_scales;
      if (!silent)
	mout << "Manual specification of each scale size to: " << scales <<endl;
      break ;
    case SCALES:
      if (!silent)
	mout << "Manual specification of each scale factor applied to "
	     << "original dimensions." << endl;
      compute_resolutions(scales, indim, scale_factors);
      break ;
    case NSCALES: // n scale between min and max resolutions
      if (!silent)
	mout << nscales << " scales between min (" << netdim
	     << ") and max (" << maxdim << ") scales." << endl;
      compute_resolutions(scales, netdim, maxdim, nscales);
      break ;
    case SCALES_STEP: // step fixed amount from scale from max down to min
      if (!silent)
	mout << "Scale step of " << scales_step << " from max (" << maxdim
	     << ") down to min (" << mindim << ") scale." << endl;
      compute_resolutions(scales, mindim, maxdim, scales_step);
      break ;
    case SCALES_STEP_UP: // step fixed amount from scale min up to max
      if (!silent)
	mout << "Scale step of " << scales_step << " from min (" << mindim
	     << ") up to max (" << maxdim << ") scale." << endl;
      compute_resolutions_up(scales, indim, mindim, maxdim, scales_step);
      break ;
    case NETWORK:
      if (!silent)
	mout << "Resize all inputs to network's minimal size" << endl;
      scales.push_back(netdim);
      break ;
    default: eblerror("unknown scaling mode");
    }
    // limit scales with max_size
    for (midxdim::iterator i = scales.begin(); i != scales.end(); ) {
      idxdim d = *i;
      if (max_size > 0 && (d.dim(1) > max_size || d.dim(2) > max_size)) {
	scales.erase(i);
	mout << "removing scale " << d << " because of max size " << max_size
	     << endl;
      } else i++;
    }
    // // transform scales to be network sizes compatible
    // network_compatible_sizes(scales);
    // initialize original bboxes to entire image
    rect<int> bb(0, 0, indim.dim(1), indim.dim(2));
    for (uint i = 0; i < scales.size(); ++i)
      original_bboxes.push_back(bb);
    // print scales
    mout << "Detection initialized to ";
    if (adapt_scales) mout << "(network-adapted scales) ";
    if (scales.size() == 0) mout << "0 resolutions." << endl;
    else mout << scales.size() << " input resolutions: " << scales;
    mout << endl;
    if (scales.size() == 0)
      eblthrow("0 resolutions to compute in " << frame_name);
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::
  compute_resolutions(midxdim &scales,
		      idxdim &mindim, idxdim &maxdim, uint nscales) {
    scales.clear();
    if (nscales == 0)
      eblerror("expected at least 1 scale but found " << nscales);
    // nscales must be less than the minimum pixel distance between min and max
    uint max_res = std::min(maxdim.dim(1) - mindim.dim(1),
			    maxdim.dim(2) - mindim.dim(2));
    if (nscales > max_res) {
      merr << "warning: the number of resolutions requested (";
      merr << nscales << ") is more than";
      merr << " the minimum distance between minimum and maximum possible";
      merr << " resolutions. (min: " << mindim << " max: " << maxdim;
      if (mindim == maxdim)
	nscales = 1;
      else
	nscales = 2;
      merr << ") setting it to " << nscales << endl;
    }
    // only 1 scale if min == max or if only 1 scale requested.
    if ((mindim == maxdim) || (nscales == 1))
      scales.push_back(maxdim);
    else if (nscales == 2) { // 2 resolutions: min and max
      scales.push_back(mindim);
      scales.push_back(maxdim);
    } else { // multiple scales: interpolate between min and max
      // compute the step factor: x = e^(log(max/min)/(nres-1))
      double fact = MIN(maxdim.dim(1) / (double) mindim.dim(1),
			maxdim.dim(2) / (double) mindim.dim(2));
      double step = exp(log(fact)/(nscales - 1));
      double f;
      uint i;
      for (f = step, i = 1; i < nscales; ++i, f *= step) {
	idxdim d = maxdim * (1 / f);
	d.setdim(0, maxdim.dim(0)); // do not scale feature dimension
	scales.push_back(d);
      }
      scales.push_back(maxdim);
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::
  compute_resolutions(midxdim &scales,
		      idxdim &indims, vector<double> &scale_factors) {
    scales.clear();
    if (scale_factors.size() == 0)
      eblerror("expected at least 1 scale factor but found "
	       << scale_factors.size());
    // compute scales
    for (uint i = 0; i < scale_factors.size(); ++i) {
      idxdim d = indims * scale_factors[i];
      d.setdim(0, indims.dim(0)); // do not scale feature dimension
      scales.push_back(d);
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::
  compute_resolutions(midxdim &scales, idxdim &mindim, idxdim &maxdim,
		      double scales_step) {
    scales.clear();
    double factor = 1 / scales_step;
    // take steps from max scale until reaching min scale
    idxdim d = maxdim;
    scales.push_back(d);
    d = d * factor;
    d.setdim(0, maxdim.dim(0)); // do not scale feature dimension
    while (d >= mindim) {
      scales.push_back(d);
      d = d * factor;
      d.setdim(0, maxdim.dim(0)); // do not scale feature dimension
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::
  compute_resolutions_up(midxdim &scales, idxdim &indim, idxdim &mindim,
			 idxdim &maxdim, double scales_step) {
    scales.clear();
    double factor = std::max(mindim.dim(1) / (double) indim.dim(1),
			     mindim.dim(2) / (double) indim.dim(2));
    idxdim d = indim * factor;
    d.setdim(0, maxdim.dim(0)); // do not scale feature dimension
    while (d <= maxdim) {
      d.set_max(mindim); // make sure each dimension is bigger than mindim
      scales.push_front_new(d);
      factor *= scales_step;
      d = indim * factor;
      d.setdim(0, maxdim.dim(0)); // do not scale feature dimension
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::get_netdim(intg order0) {
    if (!netdim_fixed) {
      netdim = network_mindims(thenet, order0);
      mfidxdim m = resizepp->get_msize();
      netdim = m[0];
    }
  }
  
  template <typename T, class Tstate>
  void detector<T,Tstate>::network_compatible_sizes(midxdim &sizes) {
    for (uint i = 0; i < sizes.size(); ++i) {
      fidxdim sz = sizes[i];
      mfidxdim msz;
      msz.push_back_new(sz);
      cout << "original sz: " << msz;
      mfidxdim r = thenet.fprop_size(msz);
      cout << " sz2: " << msz << " returned: " << r << endl;
    }
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
      mout << endl;
      break ;
    default:
      eblerror("Unknown smoothing type " << type);
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::threshold_outputs(T t) {
    for (uint j = 0; j < outputs.size(); ++j) {
      mstate<Tstate> &o = outputs[j];
      for (uint i = 0; i < o.size(); ++i) {
	idx_threshold(o[i].x, t, -1);
      }
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::smooth_outputs() {
    if (smoothing_type != 0) {
      uint hpad = (uint) (smoothing_kernel.dim(0) / 2);
      uint wpad = (uint) (smoothing_kernel.dim(1) / 2);
      for (uint j = 0; j < outputs.size(); ++j) {
	mstate<Tstate> &o = outputs[j];
	for (uint i = 0; i < o.size(); ++i) {
	  idx<T> &outx = o[i].x;
	  intg h = outx.dim(1), w = outx.dim(2);
	  idx<T> in(h + 2 * hpad, w + 2 * wpad);
	  idx<T> inc = in.narrow(0, h, hpad);
	  inc = inc.narrow(1, w, wpad);
	  idx_clear(in);
	  uint k = 0;
	  idx_bloop1(out, outx, T) {
	    if (k != (uint) bgclass) { // do not smooth background class
	      idx_copy(out, inc);
	      idx_2dconvol(in, smoothing_kernel, out);
	    }
	    k++;
	  }
	}
      }
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::update_merge_alignment() {
    // check presence of merging module
    flat_merge_module<T,Tstate> *merger = NULL;
    vector<flat_merge_module<T,Tstate>*> mergers =
      arch_find_all(&thenet, merger);
    if (mergers.size() > 0) {
      mout << "Found merging module(s) in network: " << mergers << endl;
      for (uint i = 0; i < mergers.size(); ++i)
	mout << mergers[i]->describe()<< endl;
    } else {
      mout << "No merging module found in network." << endl;
      return ;
    }
    // align for each merger module
    for (uint i = 0; i < mergers.size(); ++i) {
      merger = mergers[i];
      // get the network narrowed up to the merger module (included)
      module_1_1<T,Tstate> *merger_net_included = arch_narrow(&thenet, merger);
      module_1_1<T,Tstate> *merger_net = arch_narrow(&thenet, merger, false);
      if (!merger_net || !merger_net_included)
	eblerror("failed to narrow network up to " << merger);
      EDEBUG("network narrowed up to merger module: " << merger->name());
      mout << "Aligning merging centers on top left image origin." << endl;
      //    for (uint i = 0; i < merger->get_ninputs(); ++i) {
      fidxdim c(1, 1, 1), f(1, 1, 1), f2(1, 1, 1), c0, c1;
      mfidxdim m(c), m0, m0m, m1, paddings; //(merger->get_ninputs());
      // determine input size and location of output pixel (0,0)
      mfidxdim mf(f);
      mf = resizepp->fprop_size(mf);
      merger_net_included->fprop_size(mf);
      m0m = merger->bprop_size(m);
      EDEBUG(merger_net->name() << " m0m: " << m0m);
      mfidxdim scales = merger->get_scales();
      // EDEBUG("strides: " << strides);
      vector<vector<int> > alloff;
      mfidxdim allstrides;
      float hs0 = 1, ws0 = 1;
      for (uint k = 0; k < m0m.size(); ++k) {
	//uint i = k - (k % 2);
	uint i = k;
	mfidxdim mm(m0m.size());
	mm.set_new(m0m[i], i);
	EDEBUG("mm: " << mm);
	// determine input size and location of output pixel (0,0)
	//merger_net_included->fprop_size(mf);
	m0 = merger_net->bprop_size(mm);
	// m0 = resizepp->bprop_size(m0);
	m0.remove_empty();
	// determine input size and location of output pixel (1,1)
	mm[i].setoffset(1, 1);
	mm[i].setoffset(2, 1);
	//merger_net_included->fprop_size(mf);
	m1 = merger_net->bprop_size(mm);
	// m1 = resizepp->bprop_size(m1);
	m1.remove_empty();
	EDEBUG("m0: " << m0);
	EDEBUG("m1: " << m1);

	//	uint fact = (uint) ceil(strides.size() / (float) m0.size());
	// c0 = m0[i / fact];
	// c1 = m1[i / fact];
	c0 = m0[0];
	c1 = m1[0];
	//fidxdim &stride = strides[i];
	// determine center of output pixel (0,0) in input space
	rect<float> p0(c0.offset(1), c0.offset(2), c0.dim(1), c0.dim(2));
	float hc = p0.hcenter(), wc = p0.wcenter();

	// // determine input pixel (0,0) in output space
	// fidxdim i0(1, 1)


	// if (hc < 0) {
	//   eblwarn("expected center's height to be >= 0 but got " << hc);
	//   hc = 1;
	// }
	// if (wc < 0) {
	//   eblwarn("expected center's width to be >= 0 but got " << wc);
	//   wc = 1;
	// }
	// determine stride of output space in input space
	float hs = (c1.offset(1) - c0.offset(1));// / scales[i].dim(0);
	float ws = (c1.offset(2) - c0.offset(2));// / scales[i].dim(0);
	// if (k == 0) {
	//   hs = hs0 / (scales[i].dim(0));
	//   ws = ws0 / (scales[i].dim(0));
	//   hs0 = hs;
	//   ws0 = ws;
	// } else {
	//   hs = hs0 / (scales[i].dim(0) * hs);
	//   ws = ws0 / (scales[i].dim(0) * ws);
	// }

	if (k == 0) {
	  hs0 = hs;
	  ws0 = ws;
	}
	float hos = hs0 / (scales[i].dim(0) * hs);
	float wos = ws0 / (scales[i].dim(0) * ws);
	fidxdim fi(hos, wos);
	allstrides.push_back_new(fi);
	
	// set paddings of merger
	//fidxdim pads(hc * hs, wc * ws, hc * hs, wc * ws);

	vector<int> offs;

	offs.push_back((int)(hc/hs));
	offs.push_back((int)(wc/ws));
	offs.push_back((int)(hc/hs));
	offs.push_back((int)(wc/ws));
	
	// offs.push_back((int)(hc*hos*scales[i].dim(0)));
	// offs.push_back((int)(wc*wos*scales[i].dim(0)));
	// offs.push_back((int)(hc*hos*scales[i].dim(0)));
	// offs.push_back((int)(wc*wos*scales[i].dim(0)));

	// offs.push_back((int)(hc*hos/hs0));
	// offs.push_back((int)(wc*wos/ws0));
	// offs.push_back((int)(hc*hos/hs0));
	// offs.push_back((int)(wc*wos/ws0));
	alloff.push_back(offs);
	// fidxdim pads(stride.dim(0) * hc / hs, stride.dim(1) * wc / ws,
	// 	     stride.dim(0) * hc / hs, stride.dim(1) * wc / ws);
	// fidxdim pads(stride.dim(0) * hc / hs, stride.dim(1) * wc / ws, 0, 0);
	//	paddings.push_back_new(pads);
	mout << merger->name() << "'s input " << i << " must be padded/narrowed with "
	     << offs << " to recenter " << p0 << " (center " << hc << "x" << wc
	     << "), (output stride is " << hs << "x" << ws << ")" << std::endl;
      }
      merger->set_offsets(alloff);
      merger->set_strides(allstrides);
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::get_corners(mstate<Tstate> &outputs, uint scale,
				       bool force) {
    if (!corners_infered || force) {
      if (corners_inference == 0 || corners_inference == 1) { // infer from net
	uint n = 0;
	// allocate corner vectors
	while (scale >= itl.size()) {
	  itl.push_back(new mfidxdim());
	  ibl.push_back(new mfidxdim);
	  itr.push_back(new mfidxdim);
	  ibr.push_back(new mfidxdim);
	  pptl.push_back(new mfidxdim);
	  pptr.push_back(new mfidxdim);
	  ppbl.push_back(new mfidxdim);
	  ppbr.push_back(new mfidxdim);
	  scale_indices.push_back(vector<uint>());
	}
	itl[scale].clear(); itr[scale].clear();
	ibl[scale].clear(); ibr[scale].clear();
	pptl[scale].clear(); pptr[scale].clear();
	ppbl[scale].clear(); ppbr[scale].clear();
	for (typename mstate<Tstate>::iterator o = outputs.begin();
	     o != outputs.end(); ++o) {
	  fidxdim d(o->x.get_idxdim());
	  fidxdim c(1, 1, 1), mc0;
	  mfidxdim mc(outputs.size());
	  mc.set_new(c, n);
	  mfidxdim m;
	  // top left
	  m = thenet.bprop_size(mc);
	  m.remove_empty();
	  mc0 = m[0];
	  itl[scale].push_back_new(mc0);
	  m = resizepp->get_msize();
	  // infer scale index for this output
	  scale_indices[scale].clear();
	  for (uint i = 0; i < m.size(); ++i)
	    if (m.exists(i)) {
	      scale_indices[scale].push_back(i);
	      break ;
	    }
	  m.remove_empty();
	  mc0 = m[0];
	  pptl[scale].push_back_new(mc0);
	  // top right
	  mc[n].setoffset(2, d.dim(2));
	  m = thenet.bprop_size(mc);
	  m.remove_empty();
	  mc0 = m[0];
	  itr[scale].push_back_new(mc0);
	  m = resizepp->get_msize();
	  m.remove_empty();
	  mc0 = m[0];
	  pptr[scale].push_back_new(mc0);
	  // bottom left
	  mc[n].setoffset(1, d.dim(1));
	  mc[n].setoffset(2, 0);
	  m = thenet.bprop_size(mc);
	  m.remove_empty();
	  mc0 = m[0];
	  ibl[scale].push_back_new(mc0);
	  m = resizepp->get_msize();
	  m.remove_empty();
	  mc0 = m[0];
	  ppbl[scale].push_back_new(mc0);
	  // bottom right
	  mc[n].setoffset(1, d.dim(1));
	  mc[n].setoffset(2, d.dim(2));
	  m = thenet.bprop_size(mc);
	  m.remove_empty();
	  mc0 = m[0];
	  ibr[scale].push_back_new(mc0);
	  m = resizepp->get_msize();
	  m.remove_empty();
	  mc0 = m[0];
	  ppbr[scale].push_back_new(mc0);
	  ++n;
	}
	EDEBUG("top left output " << itl[scale]);
	EDEBUG("top right output " << itr[scale]);
	EDEBUG("bottom left output " << ibl[scale]);
	EDEBUG("bottom right output " << ibr[scale]);

	if (corners_inference == 1) { // from net + save corners
	  // save corners to matrix
	  idx<float> scorners(itl.size(), 4, 4);
	  for (uint i = 0; i < itl.size(); ++i) {
	    scorners.set(itl[scale][i].offset(1), i, 0, 0);
	    scorners.set(itl[scale][i].offset(2), i, 0, 1);
	    scorners.set(itl[scale][i].dim(1), i, 0, 2);
	    scorners.set(itl[scale][i].dim(2), i, 0, 3);
	    scorners.set(itr[scale][i].offset(1), i, 1, 0);
	    scorners.set(itr[scale][i].offset(2), i, 1, 1);
	    scorners.set(itr[scale][i].dim(1), i, 1, 2);
	    scorners.set(itr[scale][i].dim(2), i, 1, 3);
	    scorners.set(ibl[scale][i].offset(1), i, 2, 0);
	    scorners.set(ibl[scale][i].offset(2), i, 2, 1);
	    scorners.set(ibl[scale][i].dim(1), i, 2, 2);
	    scorners.set(ibl[scale][i].dim(2), i, 2, 3);
	    scorners.set(ibr[scale][i].offset(1), i, 3, 0);
	    scorners.set(ibr[scale][i].offset(2), i, 3, 1);
	    scorners.set(ibr[scale][i].dim(1), i, 3, 2);
	    scorners.set(ibr[scale][i].dim(2), i, 3, 3);
	  }
	  save_matrix(scorners, "corners.mat");
	}
	corners_infered = true;
      } else if (corners_inference == 2) { // load corners
	// load corners from matrix
	idx<float> corners = load_matrix<float>("corners.mat");
	itl[scale].clear(); itr[scale].clear();
	ibl[scale].clear(); ibr[scale].clear();
	for (uint i = 0; i < corners.dim(0); ++i) {
	  // allocate
	  fidxdim d(outputs[0].x.get_idxdim());
	  d.setdims(1);
	  itl[scale].push_back_new(d);
	  itr[scale].push_back_new(d);
	  ibl[scale].push_back_new(d);
	  ibr[scale].push_back_new(d);
	  // set
	  itl[scale][i].setoffset(1, corners.get(i, 0, 0));
	  itl[scale][i].setoffset(2, corners.get(i, 0, 1));
	  itl[scale][i].setdim(1, corners.get(i, 0, 2));
	  itl[scale][i].setdim(2, corners.get(i, 0, 3));
	  itr[scale][i].setoffset(1, corners.get(i, 1, 0));
	  itr[scale][i].setoffset(2, corners.get(i, 1, 1));
	  itr[scale][i].setdim(1, corners.get(i, 1, 2));
	  itr[scale][i].setdim(2, corners.get(i, 1, 3));
	  ibl[scale][i].setoffset(1, corners.get(i, 2, 0));
	  ibl[scale][i].setoffset(2, corners.get(i, 2, 1));
	  ibl[scale][i].setdim(1, corners.get(i, 2, 2));
	  ibl[scale][i].setdim(2, corners.get(i, 2, 3));
	  ibr[scale][i].setoffset(1, corners.get(i, 3, 0));
	  ibr[scale][i].setoffset(2, corners.get(i, 3, 1));
	  ibr[scale][i].setdim(1, corners.get(i, 3, 2));
	  ibr[scale][i].setdim(2, corners.get(i, 3, 3));
	}
	corners_infered = true;
      }
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::extract_bboxes(T threshold, bboxes &bbs) {
    bbox::init_instance_id(); // reset unique ids to start from zero.
    // make a list that contains the results
    double original_h = indim.dim(1);
    double original_w = indim.dim(2);
    intg offset_h = 0, offset_w = 0;
    int scale_index = 0;
    for (uint scale = 0; scale < outputs.size(); ++scale) {
      answers[scale].clear();
      // get 4 corners coordinates for each scale
      mstate<Tstate> &oo = outputs[scale];

      // loop on output
      for (uint o = 0; o < oo.size(); ++o) {
	if (o < raw_thresholds.size()) threshold = raw_thresholds[o];
	float thresh = threshold;
	// Tstate &input = ppinputs[0][0];
	Tstate &output = oo[o];
	idx<T> outx = output.x;
	fidxdim &tl = itl[scale][o], &tr = itr[scale][o], &bl = ibl[scale][o];
	fidxdim &ptl = pptl[scale][o], &ptr = pptr[scale][o],
	  &pbl = ppbl[scale][o];
	// fidxdim &br = ibr[o];

	// steps in input space
	double hf = (bl.offset(1) - tl.offset(1)) / outx.dim(1);
	double wf = (tr.offset(2) - tl.offset(2)) / outx.dim(2);
	// steps in preprocessed space
	double phf = (pbl.offset(1) - ptl.offset(1)) / outx.dim(1);
	double pwf = (ptr.offset(2) - ptl.offset(2)) / outx.dim(2);
      
	// box scalings
	double hscaling = 1.0, wscaling = 1.0;
	if (o < bbox_scalings.size()) {
	  fidxdim &scaling = bbox_scalings[o];
	  hscaling = scaling.dim(0);
	  wscaling = scaling.dim(1);
	}

	bboxes bbtmp;
	// select elements
	// rect<int> &robbox = original_bboxes[0];
	// sizes
	// double in_h = (double) input.x.dim(1);
	// double in_w = (double) input.x.dim(2);
	// double out_h = (double) output.x.dim(1);
	// double out_w = (double) output.x.dim(2);
	// double neth = netdim.dim(1); // network's input height
	// double netw = netdim.dim(2); // network's input width
	// double scalehi = original_h / robbox.height; // input to original
	// double scalewi = original_w / robbox.width; // input to original
	// int image_h0 = (int) (robbox.h0 * scalehi);
	// int image_w0 = (int) (robbox.w0 * scalewi);
	// offset factor in input map
	// double offset_h_factor = (in_h - neth) / std::max((double)1, (out_h - 1));
	// double offset_w_factor = (in_w - netw) / std::max((double)1, (out_w - 1));
	offset_w = 0;
	Tstate out(outx.get_idxdim());
	answer->fprop(output, out);
	answers[scale].push_back_new(out);

	idx<T> tmp = outx.select(0, 1);
	// cout << "out " << o << " threshold " << thresh << " min " << idx_min(tmp)
	//      << " max " << idx_max(tmp) << endl;
      
	// loop on width
	idx_eloop1(ro, out.x, T) {
	  offset_h = 0;
	  // loop on height
	  idx_eloop1(roo, ro, T) {
	    int classid = (int) roo.get(0);
	    float conf = (float) roo.get(1);
	    bool accept = false;
	    // select decision criterion
	    switch (bbox_decision) {
	    case 0: accept = (conf >= thresh && classid != bgclass); break ;
	    case 1: accept = ((offset_h == outx.dim(1) - 1 && offset_w == 0) ||
			      (offset_h == 0 && offset_w == 0) ||
			      (offset_h == outx.dim(1) - 1
			       && offset_w == outx.dim(2) - 1) ||
			      (offset_h == 0 && offset_w == outx.dim(2) - 0));
	      break;
	    case 2: accept = ((offset_h == outx.dim(1) - 1
			       && offset_w == outx.dim(2) - 1));
	      break;
	    default: eblerror("unknown bbox decision type");
	    }
	    if (accept) {
	      bbox bb;
	      bb.class_id = classid; // Class
	      bb.confidence = conf; // Confidence
	      bb.iscale_index = scale_index + scale_indices[scale][o];
	      bb.oscale_index = scale_index; // scale index

	      bb.h0 = tl.offset(1) + offset_h * hf;
	      bb.w0 = tl.offset(2) + offset_w * wf;
	      bb.height = tl.dim(1);
	      bb.width = tl.dim(2);
	      bb.scale_centered(hscaling, wscaling);

	      bb.i.h0 = ptl.offset(1) + offset_h * phf;
	      bb.i.w0 = ptl.offset(2) + offset_w * pwf;
	      bb.i.height = ptl.dim(1);
	      bb.i.width = ptl.dim(2);

	      // // predicted offsets / scale
	      // float hoff = 0, woff = 0, scale = 1.0;
	      // if (scaler_mode) {
	      //   scale = (float) roo.gget(2);
	      //   if (roo.dim(0) == 5) { // class,conf,scale,h,w
	      // 	hoff = roo.gget(3) * neth;
	      // 	woff = roo.gget(4) * neth;
	      //   }
	      //   // cap scale
	      //   scale = std::max(min_scale_pred, std::min(max_scale_pred, scale));
	      //   scale = 1 / scale;
	      // }
	      // EDEBUG(roo.str());
	      // // original box in input map
	      // bb.iheight = (int) in_h; // input h
	      // bb.iwidth = (int) in_w; // input w
	      // bb.i0.h0 = (float) (offset_h * offset_h_factor);
	      // bb.i0.w0 = (float) (offset_w * offset_w_factor);
	      // bb.i0.height = (float) neth;
	      // bb.i0.width = (float) netw;
	      // output map
	      // bb.oheight = (int) out_h; // output height
	      // bb.owidth = (int) out_w; // output width
	      bb.o.h0 = offset_h; // answer height in output
	      bb.o.w0 = offset_w; // answer height in output
	      bb.o.height = 1;
	      bb.o.width = 1;
	      // // bb.o.h0 = 0;
	      // // bb.o.w0 = 0;
	      // // bb.o.h0 = out_h - 1;
	      // // bb.o.w0 = out_w - 1;

	      // // transformed box in input map
	      // bb.i.h0 = bb.i0.h0 + hoff;
	      // bb.i.w0 = bb.i0.w0 + woff;
	      // bb.i.height = bb.i0.height;
	      // bb.i.width = bb.i0.width;
	      // if (scale != 1.0)
	      //   bb.i.scale_centered(scale, scale);

	      // // infer original location through network
	      // idxdim d(1, bb.o.height, bb.o.width);
	      // d.setoffset(1, bb.o.h0);
	      // d.setoffset(2, bb.o.w0);
	      // mfidxdim md(d);
	      // mfidxdim d2 = thenet.bprop_size(md);
	      // fidxdim loc = d2[0];
	      // bb.i.h0 = loc.offset(1);
	      // bb.i.w0 = loc.offset(2);
	      // bb.i.height = loc.dim(1);
	      // bb.i.width = loc.dim(2);

	      // // add all input boxes
	      // for (uint q = 0; q < d2.size(); ++q)
	      //   bb.mi.push_back(rect<float>(d2[q].offset(1), d2[q].offset(2),
	      // 				  d2[q].dim(1), d2[q].dim(2)));

	      // // bb.h0 = loc.offset(1) * scalehi;
	      // // bb.w0 = loc.offset(2) * scalewi;
	      // // bb.height = loc.dim(1) * scalehi;
	      // // bb.width = loc.dim(2) * scalewi;


	      // // original image
	      // // bbox's rectangle in original image
	      // // bb.h0 = bb.i.h0 * scalehi;
	      // // bb.w0 = bb.i.w0 * scalewi;
	      // bb.h0 = bb.i.h0 * scalehi - image_h0;
	      // bb.w0 = bb.i.w0 * scalewi - image_w0;
	      // bb.height = bb.i.height * scalehi;
	      // bb.width = bb.i.width * scalewi;

	      bool ignore = false;
	      if (ignore_outsiders) { // ignore boxes that overlap outside
		if (bb.h0 < 0 || bb.w0 < 0
		    || bb.h0 + bb.height > original_h
		    || bb.w0 + bb.width > original_w)
		  ignore = true;
	      }

	      // push bbox to list
	      if (!ignore)
		bbtmp.push_back(new bbox(bb));
	    }
	    offset_h++;
	  }
	  offset_w++;
	}
	// add scale boxes into all boxes
	for (uint k = 0; k < bbtmp.size(); ++k)
	  bbs.push_back(bbtmp[k]);
	scale_index++;
      }
    }
  }

  template <typename T, class Tstate> template <class Tin>
  bboxes& detector<T,Tstate>::fprop(idx<Tin> &img, const char *frame_name) {
    TIMING1("t1 before prepare");
    TIMING2("t2 before prepare");
    TIMING_RESIZING_RESET();
    // prepare image and resolutions
    prepare(img, frame_name);
    // do a fprop for each scaled input, based on the 'image' slot prepared
    // by prepare().
    TIMING2("preparation");
    multi_res_fprop();
    TIMING2("net fprop");
    TIMING1("end of network");
    TIMING_RESIZING("total resizing time");
    // threshold before smoothing
    threshold_outputs(pre_threshold);
    // smooth outputs
    smooth_outputs();

    if (bboxes_off) // do not extract bboxes if off flag is true
      return raw_bboxes;
    // clear previous bounding boxes
    raw_bboxes.clear();
    // get new bboxes
    if (answer) extract_bboxes(0, raw_bboxes);//pre_threshold, raw_bboxes);
    // sort bboxes by confidence (most confident first)
    raw_bboxes.sort_by_confidence();
    TIMING1("extract bboxes");
    // non-maximum suppression
    fprop_nms(raw_bboxes, pruned_bboxes);
    // print results
    if (!silent) mout << "found " << pruned_bboxes.pretty(&labels);
    // save positive response input windows in save mode
    if (save_mode)
      save_bboxes(pruned_bboxes, save_dir, frame_name);
    // backward connections
    back_module<T, Tstate>* back = (back_module<T, Tstate>*)((layers<T,Tstate>&)thenet).find("back");
    if (back) {
      back->bb(pruned_bboxes);
    }
    TIMING1("end bboxes");
    // return bounding boxes
    TIMING2("post proc");
    return pruned_bboxes;
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::fprop_nms(bboxes &in, bboxes &out) {
    if (pnms) pnms->fprop(in, out);
    else out = in;
  }

  // bboxes operations /////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void detector<T,Tstate>::
  save_bboxes(bboxes &boxes, const string &dir, const char *frame_name) {
    bboxes bbs = boxes;
#ifdef __NOSTL__
    eblerror("save_bboxes not implemented");
#else
    ostringstream fname, cmd;
    midx<T> inpp;
    idx<T> inorig;
    vector<bool> dir_exists(labels.size(), false);
    string root = dir;
    root += "/";
    vector<string> dir_pp(labels.size(), root.c_str());
    vector<string> dir_orig(labels.size(), root.c_str());

    // initialize directory names
    for (uint i = 0; i < labels.size(); ++i) {
      dir_pp[i] += "preprocessed/";
      dir_pp[i] += labels[i];
      dir_pp[i] += "/";
      dir_orig[i] += "original/";
      dir_orig[i] += labels[i];
      dir_orig[i] += "/";
    }
    svector<midx<T> > &pp = get_preprocessed(bbs, save_max_per_frame,
					     diverse_ordering);
    // loop on bounding boxes
    for (uint i = 0; i < pp.size(); ++i) {
      midx<T> &sample = pp[i];
      const bbox &bb = bbs[i];
      // check if directory exists for this class, otherwise create it
      if (!dir_exists[bb.class_id]) {
	mkdir_full(dir_pp[bb.class_id]);
	mkdir_full(dir_orig[bb.class_id]);
	dir_exists[bb.class_id] = true;
      }
      ///////////////////////////////////////////////////////////////////////
      // preprocessed
      // make sure directory exists
      fname.str("");
      fname << dir_pp[bb.class_id]
	    << frame_name << "_" << labels[bb.class_id] << setw(3)
	    << setfill('0') << save_counts[bb.class_id] << MATRIX_EXTENSION;
      string d1 = dirname(fname.str().c_str());
      mkdir_full(d1);
      try {
	// save preprocessed image as lush mat
	if (save_matrices(sample, fname.str()))
	  mout << "saved " << fname.str() << ": " << sample << " (confidence "
	       << bb.confidence << ")" << endl;
      } catch(eblexception &e) {};
      // ///////////////////////////////////////////////////////////////////////
      // // original
      // // get bbox of original input
      // if (bb.height + bb.h0 > image.dim(1) ||
      // 	  bb.width + bb.w0 > image.dim(2) ||
      // 	  bb.h0 < 0 || bb.w0 < 0)
      // 	merr << "warning: trying to crop bbox outside of image bounds: bbox "
      // 	     << bb << " in image " << image << endl;
      // // make sure we don't try to crop outside of image bounds
      // float h = std::max((float)0, bb.h0), w = std::max((float)0, bb.w0);
      // float height = std::min((float) image.dim(0) - h, h + bb.height);
      // float width = std::min((float) image.dim(1) - w, h + bb.width);
      // if (height <= 0 || width <= 0 ||
      // 	  height + h <= 0 || height + h > image.dim(1) ||
      // 	  width + w <= 0 || width + w > image.dim(2)) {
      // 	merr << "warning: ignoring bbox original save out of bounds ("
      // 	     << h << "," << w << ")" << height << "x" << width << endl;
      // } else {
      // 	inorig = image.narrow(1, (int) height, (int) h);
      // 	inorig = inorig.narrow(2, (int) width, (int) w);
      // 	inorig = inorig.shift_dim(0, 2); // put channels back to dimension 2
      // 	// save original image as png
      // 	fname.str("");
      // 	fname << dir_orig[bb.class_id] << frame_name << "_"
      // 	      << labels[bb.class_id] << setw(3) << setfill('0')
      // 	      << save_counts[bb.class_id] << ".png";
      // 	// make sure directory exists
      // 	string d2 = dirname(fname.str().c_str());
      // 	mkdir_full(d2);
      // 	if (save_image(fname.str(), inorig, "png"))
      // 	  mout << "saved " << fname.str() << ": " << inorig << " (confidence "
      // 	       << bb.confidence << ")" << endl;
      // }
      // increment file counter
      save_counts[bb.class_id]++;
    }
#endif
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::add_class(const char *name) {
    if (!name)
      eblerror("cannot add empty class name");
    mout << "Adding class " << name << endl;
    labels.push_back(name);
    mout << "New class list is: " << labels << endl;
  }

  /////////////////////////////////////////////////////////////////////////////
  // saving methods

  template <typename T, class Tstate>
  uint detector<T,Tstate>::get_total_saved() {
    uint total = 0;
    for (size_t i = 0; i < save_counts.size(); ++i)
      total += save_counts[i];
    return total;
  }

  template <typename T, class Tstate>
  string& detector<T,Tstate>::set_save(const string &directory, uint nmax,
				       bool diverse) {
    save_mode = true;
    save_dir = directory;
    // save_dir += "_";
    // save_dir += tstamp();
    diverse_ordering = diverse;
    save_max_per_frame = nmax;
    mout << "Enabling saving of detected regions into: ";
    mout << save_dir << endl;
    mout << "Saving at most " << save_max_per_frame << " positive windows"
	 << (diverse_ordering ? " and ordering them by diversity." : ".")
	 << endl;
    return save_dir;
  }

  template <typename T, class Tstate>
  vector<idx<T> >& detector<T,Tstate>::get_originals() {
    if (bodetections) // recompute only if not up-to-date
      return odetections;
    idx<T> input;
    size_t i;
    // clear vector
    odetections.clear();
    // loop on bounding boxes
    for (i = 0; i < pruned_bboxes.size(); ++i) {
      bbox &bb = pruned_bboxes[i];
      // exclude background class
      if ((bb.class_id == bgclass) || (bb.class_id == mask_class))
	continue ;
      // check the box is not out of bounds
      if (bb.h0 < 0 || bb.w0 < 0
	  || bb.h0 + bb.height > image.dim(1)
	  || bb.w0 + bb.width > image.dim(2)) {
	merr << "warning: box " << bb << "is out of bounds in original image "
	     << image << endl;
	continue ;
      }
      // get bbox of input
      input = image.narrow(1, (int) bb.height, (int) bb.h0);
      input = input.narrow(2, (int) bb.width, (int) bb.w0);
      //input = input.shift_dim(0, 2); // put channels back to dimension 2
      odetections.push_back(input);
    }
    bodetections = true;
    return odetections;
  }

  template <typename T, class Tstate>
  midx<T> detector<T,Tstate>::get_preprocessed(const bbox &bb) {
    mstate<Tstate> &ins = ppinputs[bb.output_index];
    mstate<Tstate> &outs = outputs[bb.output_index];
    // get bbox of input given output bbox and its offsets
    idxdim d(1, 1, 1); //bb.oheight, bb.owidth);
    d.setoffset(1, bb.o.h0);
    d.setoffset(2, bb.o.w0);
    mfidxdim md;
    for (uint i = 0; i < outs.size(); ++i) {
      if (i == (uint) bb.oscale_index) md.push_back(d);
      else md.push_back_empty();
    }
    mfidxdim d2 = thenet.bprop_size(md);
    EDEBUG("get_preprocessed: bprop_size of " << md << " -> " << d2
	  << " from outputs " << outs << " to input " << ins);
    // get bboxes after the resizepp
    mfidxdim dims = resizepp->get_msize();
    if (dims.size() != ins.size())
      eblerror("expected same size dimensions and ins but got " << dims.size()
	       << " and " << ins.size() << " in " << dims << " and " << ins);
    midx<T> all(1);
    ins.get_padded_midx(dims, all);
    return all;
  }

  template <typename T, class Tstate>
  svector<midx<T> >& detector<T,Tstate>::
  get_preprocessed(bboxes &out, uint nmax, bool diverse, uint pre_diverse_max) {
    return get_preprocessed(pruned_bboxes, out, nmax, diverse, pre_diverse_max);
  }

  template <typename T, class Tstate>
  svector<midx<T> >& detector<T,Tstate>::
  get_preprocessed(bboxes &in, bboxes &out, uint nmax, bool diverse,
		   uint pre_diverse_max) {
    // if (bppdetections) // recompute only if not up-to-date
    //   return ppdetections;
    idx<T> input;
    size_t i;
    size_t n = in.size();
    // limit number of samples fed to diversity if enabled
    if (diverse && pre_diverse_max > 0) {
      if (nmax > 0) pre_diverse_max = std::max(pre_diverse_max, nmax);
      n = std::min((size_t) pre_diverse_max, n);
    }

    // clear vector
    ppdetections.clear();
    out.clear();
    // loop on bounding boxes
    for (i = 0; i < n; ++i) {
      bbox &bb = in[i];
      midx<T> all = get_preprocessed(bb);
      ppdetections.push_back_new(all);
      // outs.push_back(out);
      out.push_back(bb);
    }
    // diverse ordering
    if (diverse) out.sort_by_difference(ppdetections);
    // cap to n
    if (nmax > 0 && nmax < ppdetections.size()) {
      ppdetections.erase(ppdetections.begin() + nmax, ppdetections.end());
      out.erase(out.begin() + nmax, out.end());
    }
    // return
    bppdetections = true;
    return ppdetections;
  }

  template <typename T, class Tstate>
  idx<T> detector<T,Tstate>::get_mask(string &classname) {
    int id = get_class_id(classname);
    idxdim d(image.dim(1), image.dim(2));
    if (mask.get_idxdim() != d)
      mask = idx<T>(d);
    if (id == -1) { // class not found
      merr << "warning: unknown class " << classname << endl;
      idx_clear(mask);
      return mask;
    }
    eblerror("get_mask temporarly broken, outputs is now multi-state");
    // // merge all outputs of class 'id' into mask
    // for (uint i = 0; i < ppinputs.size(); ++i) {
    //   Tstate &ppin = (*ppinputs[i])[0];
    //   idx<T> in = ppin.x.select(0, 0);
    //   idx<T> out = outputs[i]->x.select(0, id);
    //   rect<int> ob = original_bboxes[i];
    //   // resizing to inputs, then to original input, to avoid precision loss
    //   out = image_resize(out, in.dim(0), in.dim(1), 1);
    //   out = out.narrow(0, ob.height, ob.h0);
    //   out = out.narrow(1, ob.width, ob.w0);
    //   out = image_resize(out, mask.dim(0), mask.dim(1), 1);
    //   if (i++ == 0)
    // 	idx_copy(out, mask);
    //   else
    // 	idx_max(mask, out, mask);
    // }
    return mask;
  }

  //////////////////////////////////////////////////////////////////////////////
  // processing

  template <typename T, class Tstate> template <class Tin>
  void detector<T,Tstate>::prepare(idx<Tin> &img, const char *frame_name) {
    // tell detections vectors they are not up-to-date anymore
    bodetections = false;
    bppdetections = false;
    // deep copy to cast input into net's type and move channels to 1st dim
    if (img.order() == 2) { // 1 channel only
      image = idx<T>(1, img.dim(0), img.dim(1));
      idx<T> tmp = image.select(0, 0);
      idx_copy(img, tmp);
    } else if (img.order() >= 3) { // multiple channels
      idx<Tin> tmp = img.shift_dim(2, 0);
      image = idx<T>(tmp.get_idxdim());
      idx_copy(tmp, image);
    } else
      eblerror("expected at least 2 dimensions in input but got " << img);
    // if input size had changed, reinit resolutions
    if (!initialized ||
	(!(indim == image.get_idxdim()) && restype != NETWORK)) {
      init(image.get_idxdim(), frame_name);
    }
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::prepare_scale(uint i) {
    if (i >= scales.size())
      eblthrow("cannot request scale " << i << ", there are only "
	       << nscales << " scales");
    // select input/outputs buffers
    //    output = outputs[0];
    if (!mem_optimization || keep_inputs) // we use different bufs for each i
      input = &finput;
    else
      input = minput;
    // set resizing of current scale
    idxdim d = scales[i];
    resizepp->set_dimensions(d.dim(1), d.dim(2));
    // // save actual resolutions
    // fidxdim tmp = d;
    // idxdim actual = thenet.fprop_size(tmp);
    // actual = thenet.bprop_size(actual);
    // actual_scales[i] = actual;
    // EDEBUG("requested resolution " << d << " at scale " << i
    // 	  << ": actual res " << actual);
  }

  template <typename T, class Tstate>
  void detector<T,Tstate>::multi_res_fprop() {
    // timing
    timer t;
    t.start();
    for (uint i = 0; i < scales.size(); ++i) {
      prepare_scale(i);
      input->x = image; // put image in input state
      // keep a copy of preprocess' output if displaying
      if (!mem_optimization || keep_inputs)
	resizepp->set_output_copy(ppinputs[i]);
      // fprop
      mstate<Tstate> &out = outputs[i];
      thenet.fprop(*input, out);
      get_corners(out, i, true);
      EDEBUG("detector outputs: " << out);
      // outputs dumping
      if (!outputs_dump.empty()) {
      	string fname = outputs_dump;
	if (out.size() == 1) {
	  idx<T> &o = out[0].x;
	  fname << "_" << o << ".mat";
	  save_matrix(o, fname);
	  mout << "Saved " << fname << " (" << o << ", min: " << idx_min(o)
	       << ", max: " << idx_max(o) << ")" << endl;
	} else {
	  // TODO: write code to save multi-state x components
	}
      }
      // memorize original input's bbox in resized input
      rect<int> &bbox = original_bboxes[i];
      rect<int> bb = resizepp->get_original_bbox();
      bbox.h0 = bb.h0;
      bbox.w0 = bb.w0;
      bbox.height = bb.height;
      bbox.width = bb.width;

// #ifdef __DUMP_STATES__
//       DUMP(output->x, "detector_output_");
// #endif

      if (optimization_swap) { // swap output and input
	eblerror("mem optimization temporarly broken because out is now mstate");
      	// tmp = input;
      	// input = output;
      	// output = tmp;
      }
    }
    if (!silent) mout << "net_processing=" << t.elapsed_ms() << endl;
  }

} // end namespace ebl

#endif
