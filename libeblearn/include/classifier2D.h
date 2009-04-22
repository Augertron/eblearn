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

#ifndef CLASSIFIER2D_H_
#define CLASSIFIER2D_H_

#include "libeblearn.h"

namespace ebl {

  template <class Tdata> class classifier2D {
  public:
    module_1_1<state_idx,state_idx>	&thenet;
    int					 height;
    int					 width;
    idx<Tdata>				 grabbed;
    idx<Tdata>				 grabbed2;
    double				 contrast;
    double				 brightness;
    double				 coeff;
    double				 bias;
    idx<int>				 sizes;
    idx<void*>				 inputs;	//! state_idx*
    idx<void*>				 outputs;	//! state_idx*
    idx<void*>				 results;	//! idx<double>*
    idx<double>				 smoothing_kernel;
    idx<const char*>			 labels;
    int					 nn_h;
    int					 nn_w;
	
    //! Constructor.
    classifier2D(module_1_1<state_idx, state_idx> &thenet, 
		 idx<int> &sz, 
		 idx<const char*> &lbls, double b, double c, int h, int w, 
		 int nn_h = 96, int nn_w = 96);
    classifier2D(module_1_1<state_idx, state_idx> &thenet);
    virtual ~classifier2D();

    idx<double> fprop(idx<Tdata> &img, float zoom, double threshold = 1.8, 
		      int objsize = 60);
  
    // Sub functions
    idx<Tdata> multi_res_prep(idx<Tdata> &img, float zoom);
    idx<double> multi_res_fprop(double threshold, int objsize);
    idx<double> postprocess_output(double threshold, int objsize);
    //! mark local maxima (in space and feature) of in r.
    //! Put winning class in (r i j 0) and score (normalized
    //! to 0 1) in (r i j 1).
    void mark_maxima(idx<double> &in, idx<double> &inc, 
		     idx<double> &r, double threshold);
    idx<double> prune(idx<double> &res);
  };

  ////////////////////////////////////////////////////////////////

  template <class Tdata>
  class classifier2D_binocular : public classifier2D<Tdata> {
  public:
    using classifier2D<Tdata>::grabbed;
    using classifier2D<Tdata>::grabbed2;
    using classifier2D<Tdata>::height;
    using classifier2D<Tdata>::width;
    using classifier2D<Tdata>::coeff;
    using classifier2D<Tdata>::bias;
    using classifier2D<Tdata>::inputs;
    using classifier2D<Tdata>::outputs;

    classifier2D_binocular(module_1_1<state_idx, state_idx> &thenet,
		      idx<int> &sz, idx<const char*> &lbls,
		      double b, double c, int h, int w);
    virtual ~classifier2D_binocular();
  
    //! Compute multi-resolution inputs and fprop through each.
    idx<double> fprop(Tdata *left, Tdata *right, 
		      float zoom, int dx, int dy, double threshold = 1.8, 
		      int objsize = 60);
  
    // Sub functions
    void multi_res_prep(Tdata *left, Tdata *right, 
			int dx, int dy, float zoom);
  };

  ////////////////////////////////////////////////////////////////

  template <class Tdata>
  class classifierNMS : public classifier2D<Tdata> {
  public:
    double		       coef;
    double		       bias;
    idx<double>		       sizes;
    idx<const char*>	       labels;
    idx<Tdata>                 sample;
    using classifier2D<Tdata>::width;
    using classifier2D<Tdata>::height;
    using classifier2D<Tdata>::thenet;
    using classifier2D<Tdata>::inputs;
    using classifier2D<Tdata>::outputs;
    using classifier2D<Tdata>::results;
    using classifier2D<Tdata>::grabbed;
	
    //! Constructor.
    classifierNMS(module_1_1<state_idx, state_idx> &thenet_,
		  idx<double> &sizes_, 
		  idx<const char*> &labels_,
		  idx<Tdata> &sample_,
		  double bias_, double coef_);
    ~classifierNMS();

    //! do a fprop on thenet with multiple rescaled inputs
    void multi_res_fprop();

    //! call multi_res_fprop(), and analyze the output map
    idx<double> classify(double threshold);
    
    //! find maximas in output layer
    void mark_maxima(double threshold);

    //! prune btwn scales
    idx<double> map_to_list(double threshold);

  };

} // end namespace ebl

#include "classifier_gen.hpp"
#include "classifier2D.hpp"

#endif /* CLASSIFIER2D_H_ */
