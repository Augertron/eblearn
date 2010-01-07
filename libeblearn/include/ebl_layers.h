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

#ifndef EBL_LAYERS_H_
#define EBL_LAYERS_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_states.h"
#include "ebl_basic.h"
#include "ebl_arch.h"
#include "ebl_nonlinearity.h"
#include "ebl_transfer.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  //! a simple fully-connected neural net layer: linear + tanh non-linearity.
  template <class T> class nn_layer_full: public module_1_1<T> {
  public:
    linear_module_replicable<T>	 linear;  //!< linear module for weight matrix
    addc_module<T>		 adder;	  //!< bias vector
    tanh_module<T>		 sigmoid; //!< the non-linear function
    state_idx<T>		*sum;	  //!< weighted sum

    //! constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    nn_layer_full(parameter<T> &p, intg indim0, intg noutputs);
    virtual ~nn_layer_full();
    //! fprop from in to out
    void fprop(state_idx<T> &in, state_idx<T> &out);
    //! bprop
    void bprop(state_idx<T> &in, state_idx<T> &out);
    //! bbprop
    void bbprop(state_idx<T> &in, state_idx<T> &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! these two functions help scaling input data
    virtual idxdim fprop_size(idxdim &i_size);
    virtual idxdim bprop_size(const idxdim &o_size);
  };

  ////////////////////////////////////////////////////////////////
  //! a convolution neural net layer: convolution + tanh non-linearity.
  template <class T> class nn_layer_convolution: public module_1_1<T> {
  public:
    convolution_module_2D_replicable<T>	 convol;   //!< convolution module
    addc_module<T>			 adder;	   //!< bias vector
    tanh_module<T>			 sigmoid;  //!< the non-linear function
    state_idx<T>			*sum;	   //!< convolution result
 
    //! constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    //! \param p is used to store all parametric variables in a single place.
    //! \param kerneli is the height of the convolution kernel
    //! \param kernelj is the width of the convolution kernel
    //! \param stridei is the stride at which convolutions are done on 
    //!        the height axis.
    //! \param stridej is the stride at which convolutions are done on 
    //!        the width axis.
    //! \param table is the convolution connection table.
    nn_layer_convolution(parameter<T> &p, intg kerneli, intg kernelj, 
			 intg stridei, intg stridej, idx<intg> &tbl);
    virtual ~nn_layer_convolution();
    //! fprop from in to out
    void fprop(state_idx<T> &in, state_idx<T> &out);
    //! bprop
    void bprop(state_idx<T> &in, state_idx<T> &out);
    //! bbprop
    void bbprop(state_idx<T> &in, state_idx<T> &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! these two functions help scaling input data
    virtual idxdim fprop_size(idxdim &i_size);
    virtual idxdim bprop_size(const idxdim &o_size);
  };

  ////////////////////////////////////////////////////////////////
  //! a convolution layer with absolute rectification and constrast
  //! normalization
  template <class T> class layer_convabsnorm: public module_1_1<T> {
  public:
    nn_layer_convolution<T>	 lconv;	//!< convolution layer
    abs_module<T>		 abs;	//!< absolute rectification
    weighted_std_module<T>	 norm;	//!< constrast normalization
    state_idx<T>		*tmp;	//!< temporary results
    state_idx<T>		*tmp2;	//!< temporary results

    //! constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    //! \param p is used to store all parametric variables in a single place.
    //! \param kerneli is the height of the convolution kernel
    //! \param kernelj is the width of the convolution kernel
    //! \param stridei is the stride at which convolutions are done on 
    //!        the height axis.
    //! \param stridej is the stride at which convolutions are done on 
    //!        the width axis.
    //! \param table is the convolution connection table.
    layer_convabsnorm(parameter<T> &p, intg kerneli, intg kernelj, 
			 intg stridei, intg stridej, idx<intg> &tbl);
    virtual ~layer_convabsnorm();
    //! fprop from in to out
    void fprop(state_idx<T> &in, state_idx<T> &out);
    //! bprop
    void bprop(state_idx<T> &in, state_idx<T> &out);
    //! bbprop
    void bbprop(state_idx<T> &in, state_idx<T> &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! these two functions help scaling input data
    virtual idxdim fprop_size(idxdim &i_size);
    virtual idxdim bprop_size(const idxdim &o_size);
  };

  ////////////////////////////////////////////////////////////////
  //! a subsampling neural net layer: subsampling + tanh non-linearity.
  template <class T> class nn_layer_subsampling: public module_1_1<T> {
  public:
    subsampling_module_2D_replicable<T>	 subsampler;	//!< subsampling module
    addc_module<T>			 adder;	   //!< bias vector
    tanh_module<T>			 sigmoid;  //!< the non-linear function
    state_idx<T>			*sum;	   //!< subsampling result

    //! constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    nn_layer_subsampling(parameter<T> &p, intg stridei, intg stridej,
			 intg subi, intg subj, 
			 intg thick);
    virtual ~nn_layer_subsampling();
    //! fprop from in to out
    void fprop(state_idx<T> &in, state_idx<T> &out);
    //! bprop
    void bprop(state_idx<T> &in, state_idx<T> &out);
    //! bbprop
    void bbprop(state_idx<T> &in, state_idx<T> &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! these two functions help scaling input data
    virtual idxdim fprop_size(idxdim &i_size);
    virtual idxdim bprop_size(const idxdim &o_size);
  };

} // namespace ebl {

#include "ebl_layers.hpp"

#endif /* EBL_LAYERS_H_ */
