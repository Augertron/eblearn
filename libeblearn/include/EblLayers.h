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

#ifndef EBLLAYERS_H_
#define EBLLAYERS_H_

#include "Defines.h"
#include "Idx.h"
#include "Blas.h"
#include "EblStates.h"
#include "EblBasic.h"
#include "EblArch.h"
#include "EblNonLinearity.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif 

namespace ebl {

  ////////////////////////////////////////////////////////////////
  //! a simple fully-connected neural net layer: linear + tanh non-linearity.
  class nn_layer_full: public module_1_1<state_idx, state_idx> {
  public:
    linear_module_replicable	 linear;  //!< linear module for weight matrix
    addc_module			 adder;	  //!< bias vector
    tanh_module			 sigmoid; //!< the non-linear function
    state_idx			*sum;	  //!< weighted sum

    //! constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    nn_layer_full(parameter &p, intg indim0, intg noutputs);
    virtual ~nn_layer_full();
    //! fprop from in to out
    void fprop(state_idx &in, state_idx &out);
    //! bprop
    void bprop(state_idx &in, state_idx &out);
    //! bbprop
    void bbprop(state_idx &in, state_idx &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! display fprop at (h0, w0)
    void display_fprop(state_idx &in, state_idx &out,
		       unsigned int &h0, unsigned int &w0, double zoom,
		       bool show_out = false);
  };

  ////////////////////////////////////////////////////////////////
  //! a convolution neural net layer: convolution + tanh non-linearity.
  class nn_layer_convolution: public module_1_1<state_idx, state_idx> {
  public:
    //!< convolution module
    convolution_module_2D_replicable convol;
    addc_module           adder;   //!< bias vector
    tanh_module           sigmoid; //!< the non-linear function
    state_idx             *sum;     //!< convolution result

    //! constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    nn_layer_convolution(parameter &p, intg kerneli, intg kernelj, 
			 intg ri, intg rj, Idx<intg> &tbl, intg thick);
    virtual ~nn_layer_convolution();
    //! fprop from in to out
    void fprop(state_idx &in, state_idx &out);
    //! bprop
    void bprop(state_idx &in, state_idx &out);
    //! bbprop
    void bbprop(state_idx &in, state_idx &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! display fprop at (h0, w0)
    void display_fprop(state_idx &in, state_idx &out,
		       unsigned int &h0, unsigned int &w0, double zoom,
		       bool show_out = false);
  };

  ////////////////////////////////////////////////////////////////
  //! a subsampling neural net layer: subsampling + tanh non-linearity.
  class nn_layer_subsampling: public module_1_1<state_idx, state_idx> {
  public:
    subsampling_module_2D_replicable subsampler; //!< subsampling module
    addc_module           adder;      //!< bias vector
    tanh_module           sigmoid;    //!< the non-linear function
    state_idx             *sum;        //!< subsampling result

    //! constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    nn_layer_subsampling(parameter &p, intg stridei, intg stridej,
					     intg subi, intg subj, 
					     intg thick);
    virtual ~nn_layer_subsampling();
    //! fprop from in to out
    void fprop(state_idx &in, state_idx &out);
    //! bprop
    void bprop(state_idx &in, state_idx &out);
    //! bbprop
    void bbprop(state_idx &in, state_idx &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! display fprop at (h0, w0)
    void display_fprop(state_idx &in, state_idx &out,
		       unsigned int &h0, unsigned int &w0, double zoom,
		       bool show_out = false);
  };

} // namespace ebl {

#endif /* EBLLAYERS_H_ */
