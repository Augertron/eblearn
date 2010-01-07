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

using namespace std;


namespace ebl {

  template <class T>
  detector<T>::detector(module_1_1<T> &thenet_, idx<unsigned int> &resolutions_,
			idx<const char*> &labels_, T bias_, float coef_) 
    : thenet(thenet_), coef(coef_), bias(bias_),
      inputs(1), outputs(1), results(1),
      labels(labels_),
      nresolutions(resolutions_.dim(0)), resolutions(resolutions_),
      manual_resolutions(true) {
    idx_clear(inputs);
    idx_clear(outputs);
    idx_clear(results);
    if (nresolutions < 1)
      eblerror("the number of resolutions is expected to be more than 0");
  }

  template <class T>
  detector<T>::detector(module_1_1<T> &thenet_, unsigned int nresolutions_, 
			idx<const char*> &labels_, T bias_, float coef_) 
    : thenet(thenet_), coef(coef_), bias(bias_),
      inputs(1), outputs(1), results(1),
      labels(labels_),
      nresolutions(nresolutions_), resolutions(1, 2),
      manual_resolutions(false) {
    idx_clear(inputs);
    idx_clear(outputs);
    idx_clear(results);
  }

  template <class T>
  void detector<T>::compute_minmax_resolutions(idxdim &input_dims) {
    // compute maximum closest size of input compatible with the network size
    idxdim indim(1, input_dims.dim(0), input_dims.dim(1));
    thenet.fprop_size(indim); // set a valid input dimensions set
    in_maxdim.setdims(indim); // copy valid dims to in_maxdim

    // compute minimum input size compatible with network size
    idxdim minodim(1, 1, 1); // min output dims
    in_mindim = thenet.bprop_size(minodim); // compute min input dims
  }

  template <class T>
  void detector<T>::compute_resolutions(idxdim &input_dims,
					unsigned int nresolutions) { 
    // first compute minimum and maximum resolutions for this input dims.
    compute_minmax_resolutions(input_dims);
    cout << "resolutions: min: " << in_mindim << " max: " << in_maxdim << endl;

    // nresolutions must be >= 1
    if (nresolutions == 0) {
      cerr << "warning: the number of resolutions is expected to be more ";
      cerr << "than 0, setting it to 1.";
      nresolutions = 1;
    }
    // nresolutions must be less than the minimum pixel distance between min
    // and max
    unsigned int max_res = MIN(in_maxdim.dim(1) - in_mindim.dim(1),
			       in_maxdim.dim(2) - in_mindim.dim(2));
    if (nresolutions > max_res) {
      cerr << "warning: the number of resolutions requested (";
      cerr << nresolutions << ") is more than";
      cerr << " the minimum distance between minimum and maximum possible";
      cerr << " resolutions. (min: " << in_mindim << " max: " << in_maxdim;
      cerr << ") setting it to " << max_res << endl;
      nresolutions = max_res;
    }
    
    // only 1 scale if min == max or if only 1 scale requested.
    if ((in_mindim == in_maxdim) || (nresolutions == 1)) {
      resolutions.resize1(0, 1);
      resolutions.set(in_maxdim.dim(1), 0, 0);
      resolutions.set(in_maxdim.dim(2), 0, 1);
    } else if (nresolutions == 2) { // 2 resolutions: min and max
      resolutions.resize1(0, 2);
      resolutions.set(in_maxdim.dim(1), 0, 0); // max
      resolutions.set(in_maxdim.dim(2), 0, 1); // max
      resolutions.set(in_mindim.dim(1), 1, 0); // min
      resolutions.set(in_mindim.dim(2), 1, 1); // min
    } else { // multiple resolutions: interpolate between min and max
      resolutions.resize1(0, nresolutions);
      int n = nresolutions - 2;
      int h = (int) ((float)(in_maxdim.dim(1) - in_mindim.dim(1)) / (n + 1));
      int w = (int) ((float)(in_maxdim.dim(2) - in_mindim.dim(2)) / (n + 1));
      for (int i = 1; i <= n; ++i) {
	resolutions.set(in_maxdim.dim(1) - h * i, i, 0);
	resolutions.set(in_maxdim.dim(2) - w * i, i, 1);
      }
      resolutions.set(in_maxdim.dim(1), 0, 0); // max
      resolutions.set(in_maxdim.dim(2), 0, 1); // max
      resolutions.set(in_mindim.dim(1), nresolutions - 1, 0); // min
      resolutions.set(in_mindim.dim(2), nresolutions - 1, 1); // min
    }
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
  void detector<T>::init(idx<T> &sample) {
    idxdim d(sample);
    // size of the sample to process
    int thickness = 2; // sample.dim(0); // TODO FIXME
    height = sample.dim(0);    
    width = sample.dim(1);
    grabbed = idx<T>(height, width);    
    
    idxdim sd(thickness, height, width);
    cout << "machine's intermediate sizes with a " << sd << " input: " << endl;
    thenet.pretty(sd);

    if (!manual_resolutions)
      compute_resolutions(d, nresolutions);
    cout << "multi-resolution detection initialized to ";
    print_resolutions();
    
    // resize input to closest compatible size
    //    sample = image_resize(sample, indim.dim(1), indim.dim(0));
    
    // initialize input and output states and result matrices for each size
    // TODO: if allocated, deallocate first
    inputs  = idx<void*>(nresolutions);
    outputs = idx<void*>(nresolutions);
    results = idx<void*>(nresolutions);

    //sizes.set(sample.dim(0) / (float) in_mindim.dim(0), 3);

    { idx_bloop4(resolution, resolutions, unsigned int, 
		 in, inputs, void*, 
		 out, outputs, void*,
		 r, results, void*) {
	// Compute the input sizes for each scale
	idxdim scaled_dims(thickness, resolution.get(0), resolution.get(1));
	// Adapt the size to the network structure:
	idxdim out_dims = thenet.fprop_size(scaled_dims);
	// set buffers
	in.set((void*) new state_idx<T>(thickness,
					scaled_dims.dim(1),
					scaled_dims.dim(2)));
	out.set((void*) new state_idx<T>(labels.nelements() + 1, 
					 out_dims.dim(1), out_dims.dim(2)));
	r.set((void*) new idx<T>(out_dims.dim(1), out_dims.dim(2),
				 2)); // (class,score)
      }}
  }
  
  template <class T>
  detector<T>::~detector() {
    { idx_bloop3(in, inputs, void*, out, outputs, void*, r, results, void*) {
	state_idx<T> *s;
	s = (state_idx<T>*) in.get();
	if (s) delete s;
	s = (state_idx<T>*) out.get();
	if (s) delete s;
	s = (state_idx<T>*) r.get();
	if (s) delete s;
      }}
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
	intg nms = (y_max >= 2*6+1) ? ((x_max >= 2*6+1) ? 6 : x_max) : y_max;

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
			i_min = MAX(0, (y+nms<=y_max
					)?((y-nms>0)?y-nms:0):y_max-2*nms+1),
			j_min = MAX(0, (x+nms<=x_max
					)?((x-nms>0)?x-nms:0):x_max-2*nms+1),
			i_max = MIN(raw_map.dim(0), (y-nms>0)?((y+nms<=y_max)?y+nms:y_max):2*nms+1),
			j_max = MIN(raw_map.dim(1), (x-nms>0)?((x+nms<=x_max)?x+nms:x_max):2*nms+1);
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
			cout << x << "," << y <<  "," << pix_val << endl;
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
    { idx_bloop4(input, inputs, void*, output, outputs, void*,
		 r, results, void*, resolution, resolutions, unsigned int) {
	double in_h = (double)(((state_idx<T>*) input.get())->x.dim(1));
	double in_w = (double)(((state_idx<T>*) input.get())->x.dim(2));
	double out_h = (double)(((state_idx<T>*) output.get())->x.dim(1));
	double out_w = (double)(((state_idx<T>*) output.get())->x.dim(2));
	double scaleh = original_h / in_h;
	double scalew = original_w / in_w;
	double offset_h_factor = (in_h - in_mindim.dim(1)) * scaleh
	  / MAX(1, (out_h - 1));
	double offset_w_factor = (in_w - in_mindim.dim(2)) * scalew
	  / MAX(1, (out_w - 1));
	offset_h = 0;
	{ idx_bloop1(re, *((idx<T>*) r.get()), T) {
	    offset_w = 0;
	    { idx_bloop1(ree, re, T) {
		if (ree.get(1) > threshold) {
		  bbox bb;
		  bb.class_id = ree.get(0); // Class
		  bb.confidence = ree.get(1); // Confidence
		  bb.scaleh = scaleh; // Scale
		  bb.scalew = scalew; // Scale
		  bb.h0 = offset_h * offset_h_factor; // Offset X in input
		  bb.w0 = offset_w * offset_w_factor; // Offset Y in input 
		  bb.scale_index = scale_index; // scale index
		  bb.iheight = in_h; // input h
		  bb.iwidth = in_w; // input w
		  bb.height = ceil(in_mindim.dim(1) * scaleh); // bbox h
		  bb.width = ceil(in_mindim.dim(2) * scalew); // bbox w
		  bb.oheight = out_h; // output height
		  bb.owidth = out_w; // output width
		  bb.oh = offset_h; // answer height in output
		  bb.ow = offset_w; // answer height in output
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
	cout << "- " << labels.get(i->class_id);
	cout << " with confidence " << i->confidence;
	cout << " in scale #" << i->scale_index;
	cout << " (" << grabbed.dim(0) << "x" << grabbed.dim(1);
	cout << " / " << i->scaleh << "x" << i->scalew;
	cout << " = " << i->iheight << "x" << i->iwidth << ")";
	cout << endl;
	cout << "  bounding box: top left " << i->h0 << "x" << i->w0;
	cout << " and size " << i->height << "x" << i->width;
	cout << " out position: " << i->oh << "x" << i->ow;
	cout << " in " << i->oheight << "x" << i->owidth;
	cout << endl;
      }
    }
  }
  
  template <class T> template <class Tin>
  vector<bbox> detector<T>::fprop(idx<Tin> &img, T threshold) {
    idx<T> img2;
    if (typeid(T) == typeid(Tin)) // input same type as net, shallow copy
      img2 = (idx<T>&) img;
    else { // deep copy to cast input into net's type
      img2 = idx<T>(img.get_idxdim());
      idx_copy(img, img2);
    }
    init(img2);
    grabbed = img2;
    // do a fprop for each scaled input sample
    multi_res_fprop(img2);
    
    // find points that are local maxima spatial and class-wise
    // write result in m. rescale result to [0 1]
    mark_maxima(threshold);

    // prune results btwn scales
    vector<bbox> rlist = map_to_list(threshold);
    pretty_bboxes(rlist);
    return rlist;
  }

  template <class T>
  void detector<T>::multi_res_fprop(idx<T> &sample) {
    { idx_bloop2(in, inputs, void*, out, outputs, void*) {
	// generate multi-resolution input
  	idx<T> inx = ((state_idx<T>*) in.get())->x;
  	idx<T> imres = image_resize(sample, inx.dim(2), inx.dim(1), 1);
	// TODO: temporary, use channels_dim
  	idx<T> inx0 = inx.select(0, 0);
  	idx<T> inx1 = inx.select(0, 1);
  	idx_copy(imres, inx0);
  	idx_copy(imres, inx1);
  	idx_addc(inx, bias, inx);
  	idx_dotc(inx, coef, inx);

	// Run fprop for each scale
	state_idx<T> *ii = (state_idx<T>*)(in.get());
	state_idx<T> *oo = (state_idx<T>*)(out.get());
	thenet.fprop(*ii, *oo);
      }}
  }

} // end namespace ebl

#endif
