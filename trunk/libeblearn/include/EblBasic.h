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

#ifndef EBLBASIC_H_
#define EBLBASIC_H_

#include "Defines.h"
#include "Idx.h"
#include "Blas.h"
#include "EblArch.h"
#include "EblStates.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // linear_module
  //! This module applies a linears combination of the input <in> 
  //! with its internal weight matrix w and puts the result in the output.
  //! This module has a replicable order of 1, if the input has a bigger order,
  //! use the replicable version of this module: linear_module_replicable.
  class linear_module: public module_1_1<state_idx, state_idx> {
  public:
    state_idx *w;

    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    //! \param in the size of the input to the linear combination.
    //! \param out the size of the output to the linear combination.
    linear_module(parameter &p, intg in, intg out);
    //! 
    virtual ~linear_module();
    //! forward propagation from in to out
    virtual void fprop(state_idx *in, state_idx *out);
    //! backward propagation from out to in
    virtual void bprop(state_idx *in, state_idx *out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(state_idx *in, state_idx *out);
    //! order of operation
    virtual int replicable_order() { return 1; }
    //! forgetting weights by replacing with random values
    virtual void forget(forget_param_linear &fp);
    //! normalize
    virtual void normalize();
    //! resize the output based on input dimensions
    virtual void resize_output(state_idx *in, state_idx *out);
  };

  //! The replicable version of linear_module.
  //! If the input has a bigger order than the replicable_order() of 
  //! linear_module, then this module loops on extra dimensions until
  //! it reaches the replicable order, and then calls the base module 
  //! linear_module.
  //! For example, if the base module works on an order of 1, an input with
  //! dimensions <42x9x9> will produce a 9x9 grid where each box contains
  //! the output of the processing of each <42> slice.
  DECLARE_REPLICABLE_MODULE_1_1(linear_module_replicable, 
				linear_module,
				(parameter &p, intg in, intg out),
				(p, in, out));

  ////////////////////////////////////////////////////////////////
  // convolution_module_2D
  //! This module applies 2D convolutions on dimensions 1 and 2 
  //! (0 contains different layers of information).
  //! This module has a replicable order of 3, if the input has a bigger order,
  //! use the replicable version of this module:
  //! convolution_module_2D_replicable.
  class convolution_module_2D: public module_1_1<state_idx, state_idx> {
  public:
    state_idx	*kernel;
    intg	 thickness;
    intg	 stridei;
    intg	 stridej;
    Idx<intg>	*table; //!< the table of connections between input and output
    
    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    convolution_module_2D(parameter &p, intg kerneli, intg kernelj, 
			  intg  stridei, intg stridej, 
			  Idx<intg> *table, intg thick);
    virtual ~convolution_module_2D();
    //! forward propagation from in to out
    virtual void fprop(state_idx *in, state_idx *out);
    //! backward propagation from out to in
    virtual void bprop(state_idx *in, state_idx *out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(state_idx *in, state_idx *out);
    //! forgetting weights by replacing with random values
    virtual void forget(forget_param_linear &fp);
    //! order of operation
    virtual int replicable_order() { return 3; }
    //! resize the output based on input dimensions
    virtual void resize_output(state_idx *in, state_idx *out);
  };

  //! The replicable version of convolution_module_2D.
  //! If the input has a bigger order than the replicable_order() of 
  //! convolution_module_2D, then this module loops on extra dimensions until
  //! it reaches the replicable order, and then calls the base module 
  //! convolution_module_2D.
  //! For example, if the base module works on an order of 3, an input with
  //! dimensions <2x16x16x9x9> will produce a 9x9 grid where each box contains
  //! the output of the processing of each <2x16x16> slice.
  DECLARE_REPLICABLE_MODULE_1_1(convolution_module_2D_replicable, 
				convolution_module_2D,
				(parameter &p, intg ki, intg kj, intg si, 
				 intg sj, Idx<intg> *table, intg thick),
				(p, ki, kj, si, sj, table, thick));

  ////////////////////////////////////////////////////////////////
  // subsampling_module_2D
  //! This module applies 2D subsampling on dimensions 1 and 2 
  //! (0 contains different layers of information).
  //! This module has a replicable order of 3, if the input has a bigger order,
  //! use the replicable version of this module:
  //! subsampling_module_2D_replicable.
  class subsampling_module_2D: public module_1_1<state_idx, state_idx> {
  public:
    state_idx	*coeff;
    state_idx	*sub;
    intg	 thickness;
    intg	 stridei;
    intg	 stridej;
    
    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    subsampling_module_2D(parameter &p, intg stridei_, intg stridej_,
			  intg subi, intg subj, intg thick);
    virtual ~subsampling_module_2D();
    //! forward propagation from in to out
    virtual void fprop(state_idx *in, state_idx *out);
    //! backward propagation from out to in
    virtual void bprop(state_idx *in, state_idx *out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(state_idx *in, state_idx *out);
    //! forgetting weights by replacing with random values
    virtual void forget(forget_param_linear &fp);
    //! order of operation
    virtual int replicable_order() { return 3; }
    //! resize the output based on input dimensions
    virtual void resize_output(state_idx *in, state_idx *out);
  };

  //! The replicable version of subsampling_module_2D.
  //! If the input has a bigger order than the replicable_order() of 
  //! subsampling_module_2D, then this module loops on extra dimensions until
  //! it reaches the replicable order, and then calls the base module 
  //! subsampling_module_2D.
  //! For example, if the base module works on an order of 3, an input with
  //! dimensions <2x16x16x9x9> will produce a 9x9 grid where each box contains
  //! the output of the processing of each <2x16x16> slice.
  DECLARE_REPLICABLE_MODULE_1_1(subsampling_module_2D_replicable, 
				subsampling_module_2D,
				(parameter &p, intg sti, intg stj, intg subi, 
				 intg subj, intg thick),
				(p, sti, stj, subi, subj, thick));

  ////////////////////////////////////////////////////////////////
  // addc_module
  //! The constant add module adds biases to the first dimension of the input
  //! and puts the results in the output. This module is spatially replicable 
  //! (the input can have an order greater than 1 and the operation will apply
  //! to all elements).
  class addc_module: public module_1_1<state_idx, state_idx> {
  public:
    state_idx* bias; //!< the biases

    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    //! \param size is the number of biases, or the size of dimensions 0 of 
    //! inputs and outputs.
    addc_module(parameter &p, intg size);
    virtual ~addc_module();
    //! forward propagation from in to out
    virtual void fprop(state_idx *in, state_idx *out);
    //! backward propagation from out to in
    virtual void bprop(state_idx *in, state_idx *out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(state_idx *in, state_idx *out);
    //! forgetting weights by replacing with random values
    virtual void forget(forget_param_linear &fp);
    //! normalizing
    virtual void normalize();
  };

} // namespace ebl {

#endif /* EBLBASIC_H_ */
