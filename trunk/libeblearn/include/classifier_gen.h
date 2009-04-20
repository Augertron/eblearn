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

#ifndef classifier_gen_H_
#define classifier_gen_H_

#include "libeblearn.h"
using namespace std;

namespace ebl {

  template <class Tdata> class classifier_gen {
  private:
    layers_n<state_idx>          	 &thenet;
    int                                  input_width;
    int                                  input_height;
    double				 coef;
    double				 bias;
    idx<int>				 sizes;
    idx<const char*>			 labels;
    idx<Tdata>                           sample;
    idx<void*>				 inputs;	//! state_idx*
    idx<void*>				 outputs;	//! state_idx*
    idx<void*>				 results;	//! idx<double>*

  public:	
    //! Constructor.  
    classifier_gen(layers_n<state_idx> &net_,
		   idx<int> &sizes_, 
		   idx<const char*> &labels_,
		   idx<Tdata> &sample_,
		   double bias_, double coef_);
    ~classifier_gen();

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

#endif /* CLASSIFIER_GEN_H_ */
