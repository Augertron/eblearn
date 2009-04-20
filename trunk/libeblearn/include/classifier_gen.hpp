/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun, Pierre Sermanet, Clement Farabet    *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com, clement.farabet@gmail.com *
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

#ifndef classifier_gen_HPP
#define classifier_gen_HPP

#include "classifier_gen.h"
#include <ostream>

using namespace std;

namespace ebl {

  template <class Tdata>
  classifier_gen<Tdata>::classifier_gen(module_1_1<state_idx,state_idx> &net_,
				idx<int> &sizes_, 
				idx<const char*> &labels_,
				idx<Tdata> &sample_,
				double bias_, double coef_) 
    : coef(coef_), bias(bias_), thenet(net_), 
      sizes(sizes_), labels(labels_), sample(sample_) {
    
    // size of the sample to process 
    input_height = sample.dim(0);    
    input_width = sample.dim(1);

    // initialize input and output states and result matrices for each size
    inputs = idx<void*>(sizes.nelements());
    outputs = idx<void*>(sizes.nelements());
    results = idx<void*>(sizes.nelements());

    { idx_bloop4(size, sizes, int, 
		 in, inputs, void*, 
		 out, outputs, void*,
		 r, results, void*) {
	// Compute the input sizes for each scale
	intg scaled_input_height = input_height * *(sizes.ptr(0)) / *size.ptr();
	intg scaled_input_width = input_height * *(sizes.ptr(0)) / *size.ptr();
	in.set((void*) new state_idx(1, 
				     scaled_input_height,
				     scaled_input_width));
	out.set((void*) new state_idx(labels.nelements()+1, 
				      1, 
				      1));
	r.set((void*) new idx<double>(1,
				      1,
				      2)); // (class,score)
      }}
    cout << endl << "Classifier initialized" << endl;
  }
   
  template <class Tdata> 
  classifier_gen<Tdata>::~classifier_gen() {
    { idx_bloop3(in, inputs, void*, 
		 out, outputs, void*, 
		 r, results, void*) {
	delete((state_idx*) in.get());
	delete((state_idx*) out.get());
	delete((idx<double>*) r.get());
      }}
  }

  template <class Tdata> 
  idx<double> classifier_gen<Tdata>::classify(double threshold) {
    
    // do a fprop for each scaled input sample
    cout << "Running multiscale fprop on module" << endl;
    multi_res_fprop();
    
    // parse result
    // parse output feature map to extract positions of detections
    cout << "Parsing output" << endl;
    
    // find points that are local maxima spatial and class-wise
    // write result in m. rescale result to [0 1]
    mark_maxima(threshold);

    // prune results btwn scales
    idx<double> rlist = map_to_list(threshold);

    // Display results
    cout << endl << " Results: ";
    if (rlist.dim(0) == 0) 
      cout << "no object found." << endl;
    else {
      { idx_bloop1(re, rlist, double) {
	  re.printElems();
	  cout << " " << labels.get((int)re.get(0)) << endl;
	}}
    }

    return rlist;
  }

  template <class Tdata> 
  void classifier_gen<Tdata>::multi_res_fprop() {

    // generate multi-resolution input
    idx<double> inx;
    int ni, nj;
    { idx_bloop2(in, inputs, void*,
		 out, outputs, void*) {

	// Get size for each scale
  	inx = ((state_idx*) in.get())->x;
  	ni = inx.dim(1);
  	nj = inx.dim(2);
  	idx<Tdata> imres = image_resize(sample, nj, ni, 1);
  	idx<double> inx0 = inx.select(0, 0);
  	idx_copy(imres, inx0);
  	idx_addc(inx, bias, inx);
  	idx_dotc(inx, coef, inx);
	
	// Run fprop for each scale
	state_idx *ii = (state_idx*)(in.get());
	state_idx *oo = (state_idx*)(out.get());
	thenet.fprop(*ii, *oo);

      }}

  }

  template <class Tdata> 
  void classifier_gen<Tdata>::mark_maxima(double threshold) {
    
    { idx_bloop2(out_map, outputs, void*, 
		 res_map, results, void*) {
	intg winnning_class = 0;
	idx<double> raw_maps = ((state_idx*) out_map.get())->x;
	idx<double> max_map = *((idx<double>*) res_map.get());
	idx_clear(max_map);
	{ idx_bloop1(raw_map, raw_maps, double) {
	    { idx_bloop2(max_map_row, max_map, double,
			 raw_map_row, raw_map, double) {
		{ idx_bloop2(max_map_result, max_map_row, double,
			     raw_map_pix, raw_map_row, double)  {
		    double pix_val = raw_map_pix.get();
		    if (pix_val > max_map_result.get(1)
			&& pix_val > threshold) {
		      max_map_result.set(winnning_class, 0);
		      max_map_result.set(pix_val, 1);
		    }
		  }}
	      }}
	    winnning_class++;
	  }}
      }}
  }
  
  template <class Tdata> 
  idx<double> classifier_gen<Tdata>::map_to_list(double threshold) {
    
    // make a list that contains the results
    idx<double> rlist(1, 4);
    rlist.resize(0, rlist.dim(1));
    idx<double> in0x(((state_idx*) inputs.get(0))->x);
    intg s0i = in0x.dim(1);
    intg s0j = in0x.dim(2);
    { idx_bloop3(input, inputs, void*, 
		 output, outputs, void*, 
		 r, results, void*) {    	 
	idx<double> inx(((state_idx*) input.get())->x);
	double scale = s0j / (double)(inx.dim(2));
	{ idx_bloop1(re, *((idx<double>*) r.get()), double) {
	    { idx_bloop1(ree, re, double) {
		if (ree.get(1) > threshold) {
		  intg ri = rlist.dim(0);
		  rlist.resize(ri + 1, rlist.dim(1));
		  rlist.set(ree.get(0), ri, 0);
		  rlist.set(ree.get(1), ri, 1);
		  rlist.set(scale, ri, 2);
		  rlist.set(scale, ri, 3);
		}
	      }}
	  }}
      }}
  return rlist;
}

} // end namespace ebl

#endif
