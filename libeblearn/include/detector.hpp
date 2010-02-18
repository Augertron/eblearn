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
      inputs(1), outputs(1), results(1), resize_modules(1), pp(pp_),
      ppkersz(ppkersz_), nresolutions(3), resolutions(1, 2),
      original_bboxes(nresolutions, 4),
      bgclass(-1), scales(NULL), scales_step(0),
      silent(false), restype(SCALES),
      save_mode(false), save_dir(""), save_counts(labels_.dim(0), 0) {
    // default resolutions
    double sc[] = { 4, 2, 1 };
    set_resolutions(3, sc);
    // labels
    //labels = strings_to_idx(labels_);
    labels = labels_;
    // clear buffers
    idx_clear(inputs);
    idx_clear(outputs);
    idx_clear(results);
    idx_clear(resize_modules);
    set_bgclass(background);
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
    { idx_bloop4(in, inputs, void*, out, outputs, void*, r, results, void*,
		 rsz, resize_modules, void*) {
	state_idx<T> *s;
	s = (state_idx<T>*) in.get();
	if (s) delete s;
	s = (state_idx<T>*) out.get();
	if (s) delete s;
	idx<T> *s2 = (idx<T>*) r.get();
	if (s2) delete s2;
	module_1_1<T> *s3 = (module_1_1<T>*) rsz.get();
	if (s3) delete s3;
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
    cout << "resolutions: input: " << input_dim << " min: " << in_mindim << " max: " << in_maxdim << endl;

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
    
    // initialize input and output states and result matrices for each size
    // TODO: delete objects before clear (although unlikely to contain objects
    // because nresolutions should be set only once)
    if ((uint)inputs.dim(0) != nresolutions) {
      inputs  = idx<void*>(nresolutions);
      idx_clear(inputs);
    }
    if ((uint)outputs.dim(0) != nresolutions) {
      outputs = idx<void*>(nresolutions);
      idx_clear(outputs);
    }
    if ((uint)results.dim(0) != nresolutions) {
      results = idx<void*>(nresolutions);
      idx_clear(results);
    }
    if ((uint)resize_modules.dim(0) != nresolutions) {
      resize_modules = idx<void*>(nresolutions);
      idx_clear(resize_modules);
    }

    //sizes.set(sample.dim(0) / (float) in_mindim.dim(0), 3);
    cout << "machine's intermediate sizes for each resolution: " << endl;
    { idx_bloop5(resolution, resolutions, unsigned int, in, inputs, void*,
		 out, outputs, void*, r, results, void*,
		 rsz, resize_modules, void*) {
	// Compute the input sizes for each scale
	idxdim scaled(thick, resolution.get(0), resolution.get(1));
	// Adapt the size to the network structure:
	idxdim outd = thenet.fprop_size(scaled);
	// set or resize buffers
	state_idx<T> *sin = (state_idx<T>*) in.get();
	state_idx<T> *sout = (state_idx<T>*) out.get();
	idx<T> *res = (idx<T>*) r.get();
	resizepp_module<T> *rszpp = (resizepp_module<T>*) rsz.get();
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
	  rsz.set((void*) new resizepp_module<T>(scaled.dim(1), scaled.dim(2),
						 MEAN_RESIZE, pp, ppkersz));
	else
	  rszpp->set_dimensions(scaled.dim(1), scaled.dim(2));
	thenet.pretty(scaled);
      }}
  }
    
  template <class T>
  void detector<T>::set_bgclass(const char *bg) {
    int i = 0;
    string name;
    bool found = false;
    
    if (bg)
      name = bg;
    else
      name = "bg"; // default name
    idx_bloop1(lab, labels, ubyte) {
      if (!strcmp((const char *)lab.idx_ptr(), name.c_str())) {
	bgclass = i;
	found = true;
      }
      i++;
    }
    if (found) {
      cout << "Background class is \"" << name << "\" with id " << bgclass;
      cout << "." << endl;
    } else if (bg)
      cerr << "warning: background class \"" << bg << "\" not found." << endl;
  }

  template <class T>
  void detector<T>::set_silent() {
    silent = true;
  }

  template <class T>
  void detector<T>::set_save(const string directory) {
    save_mode = true;
    save_dir = directory;
  }
    
  template <class T>
  void detector<T>::compute_minmax_resolutions(idxdim &input_dims) {
    // compute maximum closest size of input compatible with the network size
    idxdim indim(input_dims.dim(2), input_dims.dim(0), input_dims.dim(1));
    thenet.fprop_size(indim); // set a valid input dimensions set
    in_maxdim.setdims(indim); // copy valid dims to in_maxdim

    // compute minimum input size compatible with network size
    idxdim minodim(1, 1, 1); // min output dims
    in_mindim = thenet.bprop_size(minodim); // compute min input dims
  }

  template <class T>
  void detector<T>::compute_resolutions(idxdim &input_dims, uint &nresolutions) {
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
    intg max_res = (intg) (MAX(in_mindim.dim(1), in_mindim.dim(2)) * maxscale);
    if (max_res > MAX(input_dims.dim(0), input_dims.dim(1)))
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
    { idx_bloop2(out_map, outputs, void*, res_map, results, void*) {
	intg winnning_class = 0;
	idx<T> raw_maps = ((state_idx<T>*) out_map.get())->x;
	idx<T> max_map = *((idx<T>*) res_map.get());

	T y_max = (T)(raw_maps.dim(1));
	T x_max = (T)(raw_maps.dim(2));
	T x=0, y=0;
	intg nms = (intg) ((y_max >= 2*6+1) ? 
			   ((x_max >= 2*6+1) ? 6 : x_max) : y_max);

	idx_fill(max_map, (T)-1);
	{ idx_bloop1(raw_map, raw_maps, T) {
	    y = 0;
	    { idx_bloop2(max_map_row, max_map, T,
			 raw_map_row, raw_map, T) {
		x = 0;
		{ idx_bloop2(max_map_result, max_map_row, T,
			     raw_map_pix, raw_map_row, T)  {
		    T pix_val = raw_map_pix.get();
		    if (pix_val > max_map_result.get(1)
			&& pix_val > threshold) {
		      intg local_max, i,j,
			// FIXME: temporarly fixed out of bounds with MAX and MIN
			i_min = (intg) MAX(0, (y+nms<=y_max
					)?((y-nms>0)?y-nms:0):y_max-2*nms+1),
			j_min = (intg) MAX(0, (x+nms<=x_max
					)?((x-nms>0)?x-nms:0):x_max-2*nms+1),
			i_max = (intg) MIN(raw_map.dim(0), (y-nms>0)?((y+nms<=y_max)?y+nms:y_max):2*nms+1),
			j_max = (intg) MIN(raw_map.dim(1), (x-nms>0)?((x+nms<=x_max)?x+nms:x_max):2*nms+1);
		      local_max = 1;
		      for (i = i_min; i < i_max; i++) {
			for (j = j_min; j < j_max; j++) {
			  if (pix_val <= raw_map.get(i,j)
			      && (i!=y || j!=x)) {
			    local_max = 0;
			  }
			}
		      }
		      if (local_max == 1) {
			max_map_result.set(winnning_class, 0);
			max_map_result.set(pix_val, 1);
			//cout << x << "," << y <<  "," << pix_val << endl;
		      }
		    }
		    x++;
		  }}
		y++;
	      }}
	    winnning_class++;
	  }}
      }}
  }

  template <class T>
  vector<bbox> detector<T>::map_to_list(T threshold) {
    // make a list that contains the results
    //    idx<double> rlist(1, 10);
    vector<bbox> vb;
    //    rlist.resize(0, rlist.dim(1));
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
	double offset_h_factor = (in_h - neth) / MAX(1, (out_h - 1));
	double offset_w_factor = (in_w - netw) / MAX(1, (out_w - 1));
	offset_h = 0;
	{ idx_bloop1(re, *((idx<T>*) r.get()), T) {
	    offset_w = 0;
	    { idx_bloop1(ree, re, T) {
		if ((ree.get(0) != bgclass) && 
		    (ree.get(1) > threshold)) {
		  bbox bb;
		  bb.class_id = (int) ree.get(0); // Class
		  bb.confidence = ree.get(1); // Confidence
		  bb.scale_index = scale_index; // scale index
		  // original image
		  uint oh0 = (uint) (offset_h * offset_h_factor * scalehi);
		  uint ow0 = (uint) (offset_w * offset_w_factor * scalewi);
		  bb.h0 = (uint) MAX(0, oh0 - robbox.h0 * scalehi);
		  bb.w0 = (uint) MAX(0, ow0 - robbox.w0 * scalewi);
		  bb.height = (uint) (MIN(neth * scalehi + oh0,
					  original_h + robbox.h0 * scalehi)
				      - MAX(robbox.h0 * scalehi, oh0));
		  bb.width = (uint) (MIN(netw * scalewi + ow0,
					 original_w + robbox.w0 * scalewi)
				     - MAX(robbox.w0 * scalewi, ow0));
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
		  vb.push_back(bb);
		}
		offset_w++;
	      }}
	    offset_h++;
	  }}
	scale_index++;
      }}
    return vb;
  }
  
  template<class T>
  void detector<T>::pretty_bboxes(vector<bbox> &vb) {
    cout << endl << "detector: ";
    if (vb.size() == 0)
      cout << "no object found." << endl;
    else {
      cout << "found " << vb.size() << " objects." << endl;
      vector<bbox>::iterator i = vb.begin();
      for ( ; i != vb.end(); ++i) {
	cout << "- " << labels[i->class_id].idx_ptr();
	cout << " with confidence " << i->confidence;
	cout << " in scale #" << i->scale_index;
	cout << " (" << grabbed.dim(0) << "x" << grabbed.dim(1);
	cout << " / " << i->scaleh << "x" << i->scalew;
	cout << " = " << i->iheight << "x" << i->iwidth << ")";
	cout << endl;
	cout << "  bounding box: top left " << i->h0 << "x" << i->w0;
	cout << " and size " << i->height << "x" << i->width;
	cout << " out position: " << i->oh0 << "x" << i->ow0;
	cout << " in " << i->oheight << "x" << i->owidth;
	cout << endl;
      }
    }
  }
  
  template <class T> template <class Tin>
  vector<bbox> detector<T>::fprop(idx<Tin> &img, T threshold) {
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
    
    // find points that are local maxima spatial and class-wise
    // write result in m. rescale result to [0 1]
    mark_maxima(threshold);

    // prune results btwn scales
    vector<bbox> rlist = map_to_list(threshold);
    if (!silent)
      pretty_bboxes(rlist);
    // save positive response input windows in save mode
    if (save_mode)
      save_bboxes(rlist, save_dir);
    // return bounding boxes
    return rlist;
  }

  template <class T>
  void detector<T>::save_bboxes(vector<bbox> &bboxes, const string &dir) {
    string classname;
    ostringstream fname, cmd;
    state_idx<T> *input = NULL;
    idx<T> inpp, inorig;
    vector<bbox>::iterator bbox;
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
    for (bbox = bboxes.begin(); bbox != bboxes.end(); ++bbox) {
      // exclude background class
      if (bbox->class_id == bgclass)
	continue ;
      // get class name
      classname = (const char *) labels[bbox->class_id].idx_ptr();
      // check if directory exists for this class, otherwise create it
      if (!dir_exists[bbox->class_id]) {
	cmd.str(""); cmd << "mkdir -p " << dir_pp[bbox->class_id];
	system(cmd.str().c_str());
	cmd.str(""); cmd << "mkdir -p " << dir_orig[bbox->class_id];
	system(cmd.str().c_str());
	dir_exists[bbox->class_id] = true;
      }
      // get bbox of preprocessed input at bbox's scale
      input = (state_idx<T>*) inputs.get(bbox->scale_index);
      inpp = input->x.narrow(1, bbox->ih, bbox->ih0);
      inpp = inpp.narrow(2, bbox->iw, bbox->iw0);
      //inpp = inpp.shift_dim(0, 2); // put channels back to 3rd position
      // get bbox of original input
      inorig = grabbed.narrow(0, bbox->height, bbox->h0);
      inorig = inorig.narrow(1, bbox->width, bbox->w0);
      // save preprocessed image as lush mat
      fname.str(""); fname << dir_pp[bbox->class_id] << classname;
      fname << setw(6) << setfill('0') << save_counts[bbox->class_id];
      fname << MATRIX_EXTENSION;
      save_matrix(inpp, fname.str());
      cout << "saved " << fname.str() << endl;
      // save original image as png
      fname.str(""); fname << dir_orig[bbox->class_id] << classname;
      fname << setw(6) << setfill('0') << save_counts[bbox->class_id] << ".png";
      save_image(fname.str(), inorig, "png");
      cout << "saved " << fname.str() << endl;
      // increment file counter
      save_counts[bbox->class_id]++;
    }
  }
  
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
  
  template <class T>
  void detector<T>::multi_res_fprop(idx<T> &sample) {
    // copy original input into a state_idx
    idx<T> imres = sample.shift_dim(2, 0);
    state_idx<T> input(imres.get_idxdim());
    idx_copy(imres, input.x);


    struct timeval start, end;

    long seconds, useconds;    
    uint ms = 0;


    // resize original input and fprop for each resolution
    { idx_bloop4(in, inputs, void*, out, outputs, void*,
		 bbox, original_bboxes, uint, rsz, resize_modules, void*) {
	state_idx<T> &ii = *(state_idx<T>*)(in.get()); // input
	state_idx<T> &oo = *(state_idx<T>*)(out.get()); // output
	resizepp_module<T> &rszpp = *(resizepp_module<T>*)(rsz.get());
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

    gettimeofday(&start, NULL);
    
	thenet.fprop(ii, oo);

    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    ms += (uint)(((seconds) * 1000 + useconds/1000.0) + 0.5);
      }}
    cout << "net: " << ms << " ms. ";
  }

} // end namespace ebl

#endif
