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

  class module_1_1_replicable {// : public module_1_1<state_idx, state_idx> {
  public:
    module_1_1_replicable();
    virtual ~module_1_1_replicable();
    virtual void fprop(state_idx *in, state_idx *out);
    virtual void bprop(state_idx *in, state_idx *out);
    virtual void bbprop(state_idx *in, state_idx *out);
    virtual void fprop2(state_idx *in, state_idx *out) { ylerror("fprop2"); };
    virtual void bprop2(state_idx *in, state_idx *out) { ylerror("fprop2"); };
    virtual void bbprop2(state_idx *in, state_idx *out) { ylerror("fprop2"); };
  };

  ////////////////////////////////////////////////////////////////
  //! The linear module provides a linear combination of the input in
  //! with its internal weight matrix w and puts the result in the output.
  //! It is different from linear_module_dim0 in that it is
  //! not spatially replicable. It can still operate on Idx of any order 
  //! but will seem them as 1D idx (requiring contiguity).
  class linear_module: public module_1_1<state_idx, state_idx> {
  public:
    state_idx *w;

    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    //! \param in the size of the input to the linear combination.
    //! \param out the size of the output to the linear combination.
    linear_module(parameter *p, intg in, intg out);
    //! 
    virtual ~linear_module();
    //! forward propagation from in to out
    virtual void fprop(state_idx *in, state_idx *out);
    //! backward propagation from out to in
    virtual void bprop(state_idx *in, state_idx *out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(state_idx *in, state_idx *out);
    //! order of operation
    virtual int order() { return 1; }
    //! forgetting weights by replacing with random values
    virtual void forget(forget_param_linear &fp);
    //! normalize
    virtual void normalize();
    //! resize the output based on input dimensions
    virtual void resize_output(state_idx *in, state_idx *out);
  };

  class linear_module_replicable 
    : public linear_module, public module_1_1_replicable {
  public:
    linear_module_replicable(parameter *p, intg in, intg out)
      : linear_module(p, in, out), module_1_1_replicable() {}
    virtual ~linear_module_replicable() { } 
    virtual void fprop(state_idx *in, state_idx *out) {
      module_1_1_replicable::fprop(in, out); }
    virtual void bprop(state_idx *in, state_idx *out) {
      module_1_1_replicable::bprop(in, out); }
    virtual void bbprop(state_idx *in, state_idx *out) {
      module_1_1_replicable::bbprop(in, out); }
    virtual void fprop2(state_idx *in, state_idx *out) {
      linear_module::fprop(in, out); }
    virtual void bprop2(state_idx *in, state_idx *out) {
      linear_module::bprop(in, out); }
    virtual void bbprop2(state_idx *in, state_idx *out) {
      linear_module::bbprop(in, out); }
 };

  ////////////////////////////////////////////////////////////////
  //! The linear module (dim0) provides a linear combination of the input in
  //! with its internal weight matrix w and puts the result in the output.
  //! It is different from linear_module in that it is
  //! spatially replicable: it applies the linear combination only
  //! on the first dimension of the idxs (dim 0) and replicates the operation
  //! to remaining (3) dimensions if present (yielding 1D, 2D, or 3D replication
  //! if dimensions 1, 2 and 3 are present respectively).
  //! It can operate on idx of any order up to 4 dimensions.
  class linear_module_dim0: public linear_module {
  public:
    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    //! \param in the size of the input to the linear combination.
    //! \param out the size of the output to the linear combination.
    linear_module_dim0(parameter *p, intg in, intg out);
    virtual ~linear_module_dim0() {};
    //! forward propagation from in to out
    virtual void fprop(state_idx *in, state_idx *out);
    //! backward propagation from out to in
    virtual void bprop(state_idx *in, state_idx *out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(state_idx *in, state_idx *out);
  };

  ////////////////////////////////////////////////////////////////
  //! The convolution module 2D applies 2-dimensional convolutions on dimensions
  //! 1 and 2 (0 contains different layers of information to be connected to the
  //! output layers based on the connections table) of the input and puts the
  //! results in the output. It is spatially replicable in the sense that if
  //! dimension 3 is present, it loops over it and repeats the convolutions.
  class convolution_module_2D: public module_1_1<state_idx, state_idx> {
  public:
    state_idx	*kernel;
    intg	 thickness;
    intg	 stridei;
    intg	 stridej;
    Idx<intg>	*table; //!< the table of connections between input and output
    
    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    convolution_module_2D(parameter *p, intg kerneli, intg kernelj, 
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
    virtual int order() { return 3; }
  };

  ////////////////////////////////////////////////////////////////
  //! subsampling module 2D
  //! This module is spatially replicable: it applies 2D subsampling on
  //! dimensions 1 and 2 (0 contains different layers of information).
  //! If dimension 3 is present, it loops over it and repeats the subsampling.
  //! It can operate on idx of any order up to 4 dimensions.
  class subsampling_module_2D: public module_1_1<state_idx, state_idx> {
  public:
    state_idx	*coeff;
    state_idx	*sub;
    intg	 thickness;
    intg	 stridei;
    intg	 stridej;
    
    //! Constructor.
    //! \param p is used to store all parametric variables in a single place.
    subsampling_module_2D(parameter *p, intg stridei_, intg stridej_,
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
    virtual int order() { return 3; }
  };

  ////////////////////////////////////////////////////////////////
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
    addc_module(parameter *p, intg size);
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
