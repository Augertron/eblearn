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
  template <class T> class full_layer: public module_1_1<T> {
  public: 
    //! Constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    //! \param indim0 The number of inputs
    //! \param noutputs The number of outputs.
    //! \param tanh If true, use tanh squasher, stdsigmoid otherwise.
    full_layer(parameter<T> *p, intg indim0, intg noutputs,
	       bool tanh = true, const char *name = "");
    //! Destructor.
    virtual ~full_layer();
    //! fprop from in to out
    void fprop(state_idx<T> &in, state_idx<T> &out);
    //! bprop
    void bprop(state_idx<T> &in, state_idx<T> &out);
    //! bbprop
    void bbprop(state_idx<T> &in, state_idx<T> &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! Return dimensions that are compatible with this module.
    //! See module_1_1_gen's documentation for more details.
    virtual idxdim fprop_size(idxdim &i_size);
    //! Return dimensions compatible with this module given output dimensions.
    //! See module_1_1_gen's documentation for more details.
    virtual idxdim bprop_size(const idxdim &o_size);
    //! Returns a deep copy of this module.
    virtual full_layer<T>* copy();

    // members ////////////////////////////////////////////////////////
  private:
    bool                         btanh;   //!< use tanh or stdsigmoid
  public:
    linear_module_replicable<T>  linear;  //!< linear module for weight matrix
    addc_module<T>		 adder;	  //!< bias vector
    module_1_1<T>               *sigmoid; //!< the non-linear function
    state_idx<T>		*sum;	  //!< weighted sum
  };

  ////////////////////////////////////////////////////////////////
  //! a convolution neural net layer: convolution + tanh non-linearity.
  template <class T> class convolution_layer: public module_1_1<T> {
  public:
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
    //! \param tanh If true, use tanh squasher, stdsigmoid otherwise.
    convolution_layer(parameter<T> *p, intg kerneli, intg kernelj, 
			 intg stridei, intg stridej, idx<intg> &tbl,
			 bool tanh = true, const char *name = "");
    virtual ~convolution_layer();
    //! fprop from in to out
    void fprop(state_idx<T> &in, state_idx<T> &out);
    //! bprop
    void bprop(state_idx<T> &in, state_idx<T> &out);
    //! bbprop
    void bbprop(state_idx<T> &in, state_idx<T> &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! Return dimensions that are compatible with this module.
    //! See module_1_1_gen's documentation for more details.
    virtual idxdim fprop_size(idxdim &i_size);
    //! Return dimensions compatible with this module given output dimensions.
    //! See module_1_1_gen's documentation for more details.
    virtual idxdim bprop_size(const idxdim &o_size);
    //! Returns a deep copy of this module.
    virtual convolution_layer<T>* copy();

    // members ////////////////////////////////////////////////////////
  private:
    bool                                 btanh;	//!< use tanh or stdsigmoid
  public:
    convolution_module_replicable<T>	 convol;//!< convolution module
    addc_module<T>			 adder;	//!< bias vector
    module_1_1<T>			*sigmoid;//!< the non-linear function
    state_idx<T>			*sum;	//!< convolution result
  };

  ////////////////////////////////////////////////////////////////
  //! a convolution layer with absolute rectification and constrast
  //! normalization
  template <class T> class convabsnorm_layer: public module_1_1<T> {
  public:
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
    //! \param tanh If true, use tanh squasher, stdsigmoid otherwise.
    convabsnorm_layer(parameter<T> *p, intg kerneli, intg kernelj, 
		      intg stridei, intg stridej, idx<intg> &tbl,
		      bool mirror = false, bool tanh = true, const char *name = "");
    //! Destructor.
    virtual ~convabsnorm_layer();
    //! fprop from in to out
    void fprop(state_idx<T> &in, state_idx<T> &out);
    //! bprop
    void bprop(state_idx<T> &in, state_idx<T> &out);
    //! bbprop
    void bbprop(state_idx<T> &in, state_idx<T> &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! Return dimensions that are compatible with this module.
    //! See module_1_1_gen's documentation for more details.
    virtual idxdim fprop_size(idxdim &i_size);
    //! Return dimensions compatible with this module given output dimensions.
    //! See module_1_1_gen's documentation for more details.
    virtual idxdim bprop_size(const idxdim &o_size);
    //! Returns a deep copy of this module.
    virtual convabsnorm_layer<T>* copy();

    // members ////////////////////////////////////////////////////////
  private:
    bool				 btanh;	//!< use tanh or stdsigmoid
  public:
    convolution_layer<T>	 lconv;	//!< convolution layer
    abs_module<T>		 abs;	//!< absolute rectification
    weighted_std_module<T>	 norm;	//!< constrast normalization
    state_idx<T>		*tmp;	//!< temporary results
    state_idx<T>		*tmp2;	//!< temporary results
  };

  ////////////////////////////////////////////////////////////////
  //! a subsampling neural net layer: subsampling + tanh non-linearity.
  template <class T> class subsampling_layer: public module_1_1<T> {
  public:
    //! constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    //! \param tanh If true, use tanh squasher, stdsigmoid otherwise.
    subsampling_layer(parameter<T> *p, intg stridei, intg stridej,
			 intg subi, intg subj, 
			 intg thick, bool tanh = true, const char *name = "");
    //! Destructor.
    virtual ~subsampling_layer();
    //! fprop from in to out
    void fprop(state_idx<T> &in, state_idx<T> &out);
    //! bprop
    void bprop(state_idx<T> &in, state_idx<T> &out);
    //! bbprop
    void bbprop(state_idx<T> &in, state_idx<T> &out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
    //! Return dimensions that are compatible with this module.
    //! See module_1_1_gen's documentation for more details.
    virtual idxdim fprop_size(idxdim &i_size);
    //! Return dimensions compatible with this module given output dimensions.
    //! See module_1_1_gen's documentation for more details.
    virtual idxdim bprop_size(const idxdim &o_size);
    //! Returns a deep copy of this module.
    virtual subsampling_layer<T>* copy();

    // members ////////////////////////////////////////////////////////
  private:
    bool				 btanh;	//!< use tanh or stdsigmoid
  public:
    subsampling_module_replicable<T>	 subsampler;	//!< subsampling module
    addc_module<T>			 adder;	//!< bias vector
    module_1_1<T>			*sigmoid;//!< the non-linear function
    state_idx<T>			*sum;	//!< subsampling result
  };

} // namespace ebl {

#include "ebl_layers.hpp"

#endif /* EBL_LAYERS_H_ */
