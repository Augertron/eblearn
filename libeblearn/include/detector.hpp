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

using namespace std;


namespace ebl {

//   template <class Tdata>
//   detector<Tdata>::detector(module_1_1<state_idx,state_idx> &thenet_, 
// 				    idx<int> &sz, 
// 				    idx<const char*> &lbls,
// 				    double b, double c, int h, int w,
// 				    int nn_h_, int nn_w_) 
//     : thenet(thenet_), nn_h(nn_h_), nn_w(nn_w_) {
//     height = h;
//     width = w;
//     grabbed = idx<Tdata>(height, width);
//     bias = b;
//     coeff = c;
//     contrast = 1;
//     brightness = 0;
//     sizes = sz;
//     labels = lbls;
//     // initialize input and output states and result matrices for each size
//     inputs = idx<void*>(sizes.nelements());
//     outputs = idx<void*>(sizes.nelements());
//     results = idx<void*>(sizes.nelements());
//     { idx_bloop4(size, sizes, int, in, inputs, void*, out, outputs, void*,
// 		 r, results, void*) {
// 	in.set((void*) new state_idx(2, 84 + (size.get() * 12), 
// 				     84 + (size.get() * 12)));
// 	out.set((void*) new state_idx(lbls.nelements(), size.get(), 
// 				      size.get()));
// 	r.set((void*) new idx<double>(size.get(), 
// 				      size.get(), 2)); // (class,score)
//       }}
//     double sker[3][3] = {{0.3, 0.5, 0.3}, {0.5, 1, 0.5}, {0.3, 0.5, 0.3}};
//     smoothing_kernel = idx<double>(3, 3);
//     memcpy(smoothing_kernel.idx_ptr(), sker, sizeof (sker));
//   }


  template <class Tdata>
  detector<Tdata>::detector(module_1_1<state_idx, state_idx> &thenet_,
				      idx<unsigned int> &resolutions_, 
				      idx<const char*> &labels_,
				      double bias_, double coef_) 
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

  template <class Tdata>
  detector<Tdata>::detector(module_1_1<state_idx, state_idx> &thenet_,
				      unsigned int nresolutions_, 
				      idx<const char*> &labels_,
				      double bias_, double coef_) 
    : thenet(thenet_), coef(coef_), bias(bias_),
      inputs(1), outputs(1), results(1),
      labels(labels_),
      nresolutions(nresolutions_), resolutions(1, 2),
      manual_resolutions(false) {
    idx_clear(inputs);
    idx_clear(outputs);
    idx_clear(results);
  }

  template <class Tdata>
  void detector<Tdata>::compute_minmax_resolutions(idxdim &input_dims) {
    // compute maximum closest size of input compatible with the network size
    idxdim indim(1, input_dims.dim(0), input_dims.dim(1));
    thenet.fprop_size(indim); // set a valid input dimensions set
    in_maxdim.setdims(indim); // copy valid dims to in_maxdim

    // compute minimum input size compatible with network size
    idxdim minodim(1, 1, 1); // min output dims
    in_mindim = thenet.bprop_size(minodim); // compute min input dims
  }

  template <class Tdata>
  void detector<Tdata>::compute_resolutions(idxdim &input_dims,
					    unsigned int nresolutions) { 
    // first compute minimum and maximum resolutions for this input dims.
    compute_minmax_resolutions(input_dims);

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

  template <class Tdata>
  void detector<Tdata>::print_resolutions() {
    cout << resolutions.dim(0) << " resolutions: ";
    cout << resolutions.get(0, 0) << "x" << resolutions.get(0, 1);
    for (int i = 1; i < resolutions.dim(0); ++i)
      cout << ", " << resolutions.get(i, 0) << "x" << resolutions.get(i, 1);
    cout << endl;
  }
  
  template <class Tdata>
  void detector<Tdata>::init(idx<Tdata> &sample) {
    idxdim d(sample);
    if (!manual_resolutions)
      compute_resolutions(d, nresolutions);
    cout << "multi-resolution detection initialized to ";
    print_resolutions();
    
    // size of the sample to process
    int thickness = 2; // sample.dim(0); // TODO FIXME
    height = sample.dim(0);    
    width = sample.dim(1);
    grabbed = idx<Tdata>(height, width);    
    
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
	in.set((void*) new state_idx(thickness,
				     scaled_dims.dim(1), scaled_dims.dim(2)));
	out.set((void*) new state_idx(labels.nelements() + 1, 
				      out_dims.dim(1), out_dims.dim(2)));
	r.set((void*) new idx<double>(out_dims.dim(1), out_dims.dim(2),
				      2)); // (class,score)
      }}
  }
  

//   template <class Tdata>
//   detector<Tdata>::detector(module_1_1<state_idx, state_idx> &thenet)
//     : thenet(thenet) {}

  template <class Tdata>
  detector<Tdata>::~detector() {
    { idx_bloop3(in, inputs, void*, out, outputs, void*, r, results, void*) {
	state_idx *s;
	s = (state_idx*) in.get();
	if (s) delete s;
	s = (state_idx*) out.get();
	if (s) delete s;
	s = (state_idx*) r.get();
	if (s) delete s;
      }}
  }

  template <class Tdata> 
  void detector<Tdata>::mark_maxima(double threshold) {
    
    { idx_bloop2(out_map, outputs, void*, 
		 res_map, results, void*) {
	intg winnning_class = 0;
	idx<double> raw_maps = ((state_idx*) out_map.get())->x;
	idx<double> max_map = *((idx<double>*) res_map.get());

	double y_max = (double)(raw_maps.dim(1));
	double x_max = (double)(raw_maps.dim(2));
	double x=0, y=0;
	intg nms = (y_max >= 2*6+1) ? ((x_max >= 2*6+1) ? 6 : x_max) : y_max;

	idx_fill(max_map, (double)-1);
	{ idx_bloop1(raw_map, raw_maps, double) {
	    y = 0;
	    { idx_bloop2(max_map_row, max_map, double,
			 raw_map_row, raw_map, double) {
		x = 0;
		{ idx_bloop2(max_map_result, max_map_row, double,
			     raw_map_pix, raw_map_row, double)  {
		    double pix_val = raw_map_pix.get();
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

  template <class Tdata> 
  vector<bbox> detector<Tdata>::map_to_list(double threshold) {
    // make a list that contains the results
    //    idx<double> rlist(1, 10);
    vector<bbox> vb;
    //    rlist.resize(0, rlist.dim(1));
    idx<double> in0x(((state_idx*) inputs.get(0))->x);
    double original_h = grabbed.dim(0);
    double original_w = grabbed.dim(1);
    intg offset_h = 0, offset_w = 0;
    int scale_index = 0;
    { idx_bloop4(input, inputs, void*, output, outputs, void*,
		 r, results, void*, resolution, resolutions, unsigned int) {
	double in_h = (double)(((state_idx*) input.get())->x.dim(1));
	double in_w = (double)(((state_idx*) input.get())->x.dim(2));
	double out_h = (double)(((state_idx*) output.get())->x.dim(1));
	double out_w = (double)(((state_idx*) output.get())->x.dim(2));
	double scaleh = original_h / in_h;
	double scalew = original_w / in_w;
	double offset_h_factor = (in_h - in_mindim.dim(1)) * scaleh
	  / MAX(1, (out_h - 1));
	double offset_w_factor = (in_w - in_mindim.dim(2)) * scalew
	  / MAX(1, (out_w - 1));
	offset_h = 0;
	{ idx_bloop1(re, *((idx<double>*) r.get()), double) {
	    offset_w = 0;
	    { idx_bloop1(ree, re, double) {
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

  
//   template <class Tdata>
//   void detector<Tdata>::mark_maxima(idx<double> &in, idx<double> &inc, 
// 				 idx<double> &res, double threshold) {
//     idx_clear(res);
//     int tr[] = { 1, 2, 0 };
//     idx<double> s(inc.transpose(tr));
//     idx<double> z(in.transpose(tr));
//     z = z.unfold(0, 3, 1);
//     z = z.unfold(1, 3, 1);
//     { idx_bloop3(se, s, double, ze, z, double, re, res, double) {
// 	{ idx_bloop3(see, se, double, zee, ze, double, ree, re, double)  {
// 	    // find winning class
// 	    intg w = idx_indexmax(see);
// 	    double c = see.get(w);
// 	    // look if above threshold and local maximum
// 	    ree.set(-1.0, 0),
// 	      ree.set(-100.0, 1);
// 	    if ((c > threshold) &&
// 		(c > zee.get(w, 0, 0)) && (c > zee.get(w, 0, 1)) 
// 		&& (c > zee.get(w, 0, 2)) &&
// 		(c > zee.get(w, 1, 0)) && (c > zee.get(w, 1, 2)) &&
// 		(c > zee.get(w, 2, 0)) && (c > zee.get(w, 2, 1)) 
// 		&& (c > zee.get(w, 2, 2))) {
// 	      ree.set(w, 0);
// 	      ree.set(c, 1);
// 	    }
// 	  }}
//       }}
//   }

//   // produce a score between 0 and 1 for each class at each location
//   template <class Tdata>
//   idx<double> detector<Tdata>::postprocess_output(double threshold, int objsize) {
//     // find candidates at each scale
//     { idx_bloop2(out, outputs, void*, resu, results, void*) {
// 	idx<double> outx = ((state_idx*) out.get())->x;
// 	int c = outx.dim(0);
// 	int h = outx.dim(1);
// 	int w = outx.dim(2);
// 	idx<double> in(c, 2 + w, 2 + h);
// 	idx<double> inc(in);
// 	inc = inc.narrow(1, w, 1);
// 	inc = inc.narrow(2, h, 1);
// 	idx<double> m(c, h, w);
// 	idx_fill(in, 0.0);
// 	idx_clip(outx, 0.0, inc);
// 	// smooth
// 	{ idx_bloop2(ie, in, double, me, m, double) {
// 	    idx_2dconvol(ie, smoothing_kernel, me);
// 	  }}
// 	idx_copy(m, inc);
// 	// find points that are local maxima spatial and class-wise
// 	// write result in m. rescale result to [0 1]
// 	mark_maxima(in, inc, *((idx<double>*) resu.get()), threshold);
//       }}
//     // now prune between scale and make a list
//     idx<double> rlist(1, 6);
//     rlist.resize(0, rlist.dim(1));
//     idx<double> in0x(((state_idx*) inputs.get(0))->x);
//     intg s0i = in0x.dim(1);
//     intg s0j = in0x.dim(2);
//     { idx_bloop3(input, inputs, void*, output, outputs, void*, 
// 		 r, results, void*) {    	 
// 	idx<double> inx(((state_idx*) input.get())->x);
// 	intg sj = inx.dim(2);
// 	int i = 0, j = 0;
// 	double scale = s0j / (double) sj;
// 	{ idx_bloop1(re, *((idx<double>*) r.get()), double) {
// 	    j = 0;
// 	    double scalewidth = s0i / (double) (scale * (nn_h + re.dim(0) - 1));
// 	    { idx_bloop1(ree, re, double) {
// 		if (ree.get(1) > threshold) {
// 		  intg ri = rlist.dim(0);
// 		  rlist.resize(ri + 1, rlist.dim(1));
// 		  rlist.set(ree.get(0), ri, 0);
// 		  rlist.set(ree.get(1), ri, 1);
// 		  rlist.set((int) (i * scalewidth) + nn_h/2, ri, 2);
// 		  rlist.set((int) (j * scalewidth) + nn_w/2, ri, 3);
// 		  rlist.set(nn_h * scale, ri, 4);
// 		  rlist.set(nn_w * scale, ri, 5);
// 		}
// 		j++;
// 	      }}
// 	    i++;
// 	  }}
//       }}
//     return rlist;
//   }

//   template <class Tdata>
//   idx<double> detector<Tdata>::prune(idx<double> &res) {
//     // prune a list of detections.
//     // only keep the largest scoring within an area
//     idx<double> rlist(1, res.dim(1));
//     rlist.resize(0, rlist.dim(1));
//     { idx_bloop1(re, res, double) {
// 	double score = re.get(1);
// 	int px = (int) re.get(2);
// 	int py = (int) re.get(3);
// 	int bx = (int) re.get(4);
// 	int by = (int) re.get(5);
// 	// if its center is inside the box of a higher-scoring obj, kill it
// 	bool ok = true;
// 	{ idx_bloop1(o, res, double) {
// 	    double scoreo = o.get(1);
// 	    int pxo = (int) o.get(2);
// 	    int pyo = (int) o.get(3);
// 	    int bxo = (int) o.get(4);
// 	    int byo = (int) o.get(5);
// 	    if ((score < scoreo) && 
// 		collide_rect((int) (px - 0.5 * bx), (int) (py - 0.5 * by), 
// 			     bx, by, (int) (pxo - (0.5 * bxo)),	
// 			     (int) (pyo - (0.5 * byo)), bxo, byo))
// 	      ok = false;
// 	  }}
// 	if (ok) {
// 	  intg ri = rlist.dim(0);
// 	  rlist.resize(ri + 1, rlist.dim(1));
// 	  idx<double> out(rlist.select(0, ri));
// 	  idx_copy(re, out);
// 	}
//       }}
//     return rlist;
//   }

  template<class Tdata>
  void detector<Tdata>::pretty_bboxes(vector<bbox> &vb) {
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
  
  template <class Tdata>
  vector<bbox> detector<Tdata>::fprop(idx<Tdata> &img, double threshold) {
    init(img);
    grabbed = img;
    // do a fprop for each scaled input sample
    multi_res_fprop(img);
    
    // find points that are local maxima spatial and class-wise
    // write result in m. rescale result to [0 1]
    mark_maxima(threshold);

    // prune results btwn scales
    vector<bbox> rlist = map_to_list(threshold);
    pretty_bboxes(rlist);
    return rlist;
  }

  template <class Tdata> 
  void detector<Tdata>::multi_res_fprop(idx<Tdata> &sample) {

    { idx_bloop2(in, inputs, void*,
		 out, outputs, void*) {
	// generate multi-resolution input
  	idx<double> inx = ((state_idx*) in.get())->x;
  	idx<Tdata> imres = image_resize(sample, inx.dim(2), inx.dim(1), 1);
	// TODO: temporary, use channels_dim
  	idx<double> inx0 = inx.select(0, 0);
  	idx<double> inx1 = inx.select(0, 1);
  	idx_copy(imres, inx0);
  	idx_copy(imres, inx1);
  	idx_addc(inx, bias, inx);
  	idx_dotc(inx, coef, inx);

	// Run fprop for each scale
	state_idx *ii = (state_idx*)(in.get());
	state_idx *oo = (state_idx*)(out.get());
	thenet.fprop(*ii, *oo);
      }}

  }


  ////////////////////////////////////////////////////////////////

//   template <class Tdata>
//   detector_binocular<Tdata>::
//   detector_binocular(module_1_1<state_idx, state_idx> &thenet,
// 			 idx<int> &sz, idx<const char*> &lbls,
// 			 double b, double c, int h, int w)
//     : detector<Tdata>(thenet, sz, lbls, b, c, h, w) {
//     grabbed2 = idx<Tdata>(height, width);
//   }

//   template <class Tdata>
//   detector_binocular<Tdata>::~detector_binocular() {
//   }

//   template <class Tdata>
//   void detector_binocular<Tdata>::multi_res_prep(Tdata *left, Tdata *right, 
// 					 int dx, int dy, float zoom) {
//     // display
//     idx<Tdata> display(height, width), display2(height, width);
//     idx_clear(display);
//     idx_clear(display2);
//     // copy input images locally
//     memcpy(grabbed.idx_ptr(), left, height * width * sizeof (Tdata));
//     memcpy(grabbed2.idx_ptr(), right, height * width * sizeof (Tdata));
//     // prepare multi resolutions input
//     idx<double> inx;
//     int ni = ((state_idx*) inputs.get(0))->x.dim(1);
//     int nj = ((state_idx*) inputs.get(0))->x.dim(2);
//     int zi = max(ni, (int) (zoom * grabbed.dim(0)));
//     int zj = max(nj, (int) (zoom * grabbed.dim(1)));
//     int oi = (zi - ni) / 2;
//     int oj = (zj - nj) / 2;
//     idx<Tdata> imleft = image_resize(grabbed, zj, zi, 1);
//     imleft = imleft.narrow(0, ni, oi);
//     imleft = imleft.narrow(1, nj, oj);
//     idx<Tdata> tmp = image_resize(grabbed2, zj, zi, 1);
//     idx<Tdata> imright(tmp.dim(0), tmp.dim(1));
//     idx_clear(imright);
//     idx<Tdata> tmpright(imright);
//     tmp = tmp.narrow(0, imright.dim(0) - dx, 0);
//     tmp = tmp.narrow(1, imright.dim(1) - dy, 0);
//     tmpright = tmpright.narrow(0, imright.dim(0) - dx, dx);
//     tmpright = tmpright.narrow(1, imright.dim(1) - dy, dy);
//     idx_copy(tmp, tmpright);
//     imright = imright.narrow(0, ni, oi);
//     imright = imright.narrow(1, nj, oj);
//     // display
//     idx<Tdata> disp1(display.narrow(0, imright.dim(0), 0));
//     disp1 = disp1.narrow(1, imright.dim(1), 0);
//     idx<Tdata> disp2(display2.narrow(0, imright.dim(0), 0));
//     disp2 = disp2.narrow(1, imright.dim(1), 0);
//     idx_copy(imright, disp2);
//     idx_copy(imleft, disp1);
//     { idx_bloop1(in, inputs, void*) {
//   	inx = ((state_idx*) in.get())->x;
//   	ni = inx.dim(1);
//   	nj = inx.dim(2);
//   	idx<Tdata> iml = image_resize(imleft, nj, ni, 1);
//   	idx<double> inx0 = inx.select(0, 0);
//   	idx<Tdata> imr = image_resize(imright, nj, ni, 1);
//   	idx<double> inx1 = inx.select(0, 1);
//   	idx_copy(iml, inx0);
//   	idx_copy(imr, inx1);
//   	idx_addc(inx, bias, inx);
//   	idx_dotc(inx, coeff, inx);
//       }}
//     // display
//     memcpy(left, display.idx_ptr(), height * width * sizeof (Tdata));
//     memcpy(right, display2.idx_ptr(), height * width * sizeof (Tdata));
//   }

//   template <class Tdata>
//   idx<double> detector_binocular<Tdata>::fprop(Tdata *left, Tdata *right, 
// 						   float zoom, int dx, int dy, 
// 						   double threshold, 
// 						   int objsize) {
//     this->multi_res_prep(left, right, dx, dy, 0.5);
//     return this->multi_res_fprop(threshold, objsize);
//   }

  ////////////////////////////////////////////////////////////////

} // end namespace ebl

#endif
