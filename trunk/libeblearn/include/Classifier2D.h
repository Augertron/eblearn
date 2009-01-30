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

#include "Net.h"
#include "libidx.h"

namespace ebl {

  class Classifier2D {
  public:
    parameter 	        *theparam;
    lenet7		*thenet;
    int			height;
    int			width;
    Idx<ubyte>	        grabbed;
    Idx<ubyte>	        grabbed2;
    double		contrast;
    double		brightness;
    double		coeff;
    double		bias;
    Idx<int>		sizes;
    Idx<void*>	        inputs;		//! state_idx*
    Idx<void*>	        outputs;	//! state_idx*
    Idx<void*>          results;	//! Idx<double>*
    Idx<double>         smoothing_kernel;
    Idx<const char*>    labels;
	
    Classifier2D(const char *paramfile, Idx<int> &sz, Idx<const char*> &lbls,
		 double b, double c, int h, int w);
    virtual ~Classifier2D();

    Idx<double> fprop(ubyte *img, float zoom, double threshold = 1.8, 
		      int objsize = 60);
  
    // Sub functions
    Idx<ubyte> multi_res_prep(ubyte *img, float zoom);
    Idx<double> multi_res_fprop(double threshold, int objsize);
    Idx<double> postprocess_output(double threshold, int objsize);
    //! mark local maxima (in space and feature) of in r.
    //! Put winning class in (r i j 0) and score (normalized
    //! to 0 1) in (r i j 1).
    void mark_maxima(Idx<double> &in, Idx<double> &inc, 
		     Idx<double> &r, double threshold);
    Idx<double> prune(Idx<double> &res);
  };

  ////////////////////////////////////////////////////////////////

  class Classifier2DBinoc : public Classifier2D {
  public:
	
    Classifier2DBinoc(const char *paramfile, Idx<int> &sz, 
		      Idx<const char*> &lbls,
		      double b, double c, int h, int w);
    virtual ~Classifier2DBinoc();
  
    //! Compute multi-resolution inputs and fprop through each.
    Idx<double> fprop(ubyte *left, ubyte *right, 
		      float zoom, int dx, int dy, double threshold = 1.8, 
		      int objsize = 60);
  
    // Sub functions
    void multi_res_prep(ubyte *left, ubyte *right, 
			int dx, int dy, float zoom);
  };

  ////////////////////////////////////////////////////////////////

} // end namespace ebl

#endif /* CLASSIFIER2D_H_ */
