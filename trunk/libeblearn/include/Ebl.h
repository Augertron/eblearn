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

#ifndef EBL_H_
#define EBL_H_

#include "Idx.h"
#include "Blas.h"
#include "EblStates.h"
#include "EblBasic.h"
#include "EblArch.h"

namespace ebl {

  /*! \mainpage libeblearn Library Main Page
   *
   * \section intro_sec Introduction
   *
   * This is the introduction.
   *
   * \section install_sec Installation
   *
   * \subsection step1 Step 1: TODO
   *  
   * TODO
   */

  //! see numerics.h for description
  extern bool drand_ini;

  void err_not_implemented();

  ////////////////////////////////////////////////////////////////

  //! a slab of standard Lush sigmoids
  class stdsigmoid_module: public module_1_1<state_idx, state_idx> {
  public:
    //! empty constructor
    stdsigmoid_module();
    virtual ~stdsigmoid_module();
    //! fprop from in to out
    virtual void fprop(state_idx *in, state_idx *out);
    //! bprop
    virtual void bprop(state_idx *in, state_idx *out);
    //! bbprop
    virtual void bbprop(state_idx *in, state_idx *out);
  };

  ////////////////////////////////////////////////////////////////

  //! a slab of tanh
  class tanh_module: public module_1_1<state_idx, state_idx> {
  public:
    //! fprop from in to out
    void fprop(state_idx *in, state_idx *out);
    //! bprop
    void bprop(state_idx *in, state_idx *out);
    //! bbprop
    void bbprop(state_idx *in, state_idx *out);
    void forget(forget_param_linear &fp);
    void normalize();
  };

  ////////////////////////////////////////////////////////////////

  //! a simple fully-connected neural net layer: linear + tanh non-linearity.
  //! Unlike the f-layer class, this one is not spatially replicable.
  class nn_layer_full: public module_1_1<state_idx, state_idx> {
  public:
    //! linear module for weight matrix
    linear_module_dim0 *linear;
    //! bias vector
    addc_module_dim0 *adder;
    //! weighted sum
    state_idx *sum;
    //! the non-linear function
    tanh_module *sigmoid;

    //! constructor. Arguments are a pointer to a parameter
    //! in which the trainable weights will be appended,
    //! the number of inputs, and the number of outputs.
    nn_layer_full(parameter *p, state_idx *instate, intg noutputs);
    virtual ~nn_layer_full();
    //! fprop from in to out
    void fprop(state_idx *in, state_idx *out);
    //! bprop
    void bprop(state_idx *in, state_idx *out);
    //! bbprop
    void bbprop(state_idx *in, state_idx *out);
    //! initialize the weights to random values
    void forget(forget_param_linear &fp);
  };

  ////////////////////////////////////////////////////////////////

  //! softmax module
  //! if in is idx0 -> out is idx0 and equal to 1
  //! if in is idx1 -> it is just one pool
  //! if in is idx2 -> it is just one pool
  //! if in is idx3 -> the last two dimensions are pools
  //! if in is idx4 -> the last two dimensions are pools
  //! if in is idx5 -> the last four dimensions are pools
  //! if in is idx6 -> the last four dimensions are pools

  class softmax: public module_1_1<state_idx, state_idx> {

  public:
    double beta;

    // <b> is the parameter beta in the softmax
    // large <b> turns the softmax into a max
    // <b> equal to 0 turns the softmax into 1/N

  private:
    void resize_nsame(state_idx *in, state_idx *out, int n);

  public:
    softmax(double b);
    ~softmax() {
    }
    ;
    void fprop(state_idx *in, state_idx *out);
    void bprop(state_idx *in, state_idx *out);
    void bbprop(state_idx *in, state_idx *out);

  };

  ////////////////////////////////////////////////////////////////
  //! Generic Jacobian tester

  class Jacobian_tester {

  public:
    Jacobian_tester() {
    }
    ;
    ~Jacobian_tester() {
    }
    ;

    // this function take any module_1_1 with a fprop et bprop implemented,
    // and tests if the jacobian is correct (by pertubation) 
    // (on a state_idx with 3 dimensions)
    void test(module_1_1<state_idx, state_idx> *module);

  };

  ////////////////////////////////////////////////////////////////
  //! Generic BBprop tester tester

  class Bbprop_tester {

  public:
    Bbprop_tester() {
    }
    ;
    ~Bbprop_tester() {
    }
    ;

    // this function take any module_1_1 with a fprop et bbprop implemented, 
    // and tests if the Bbprop is correct (by pertubation) 
    // (on a state_idx with 3 dimensions)
    void test(module_1_1<state_idx, state_idx> *module);

  };

  ////////////////////////////////////////////////////////////////
  //! Generic Bprop tester tester

  class Bprop_tester {

  public:
    Bprop_tester() {
    }
    ;
    ~Bprop_tester() {
    }
    ;

    // this function take any module_1_1 with a fprop et bprop implemented, 
    // and tests if the bprop is correct (by pertubation) 
    // (on a state_idx with 3 dimensions)
    void test(module_1_1<state_idx, state_idx> *module);

  };

} // namespace ebl {

#endif /* EBL_H_ */
