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

#ifndef EBL_COST_H_
#define EBL_COST_H_

#include "libidx.h"
#include "ebl_arch.h"

namespace ebl {

  //! cost module base class
  template<class Tin1, class Tin2>
    class cost_module : public ebm_2<Tin1, Tin2> {
  public:
    //! all the input targets
    idx<double> &targets;
    //! a temporary buffer where targets are copied based on input label
    state_idx	 in2;
    //! the energy for each target
    idx<double>	 energies;

    //! Constructor. Keep a reference to targets and allocate other buffers
    //! based on the targets order and dimensions.
    cost_module(idx<double> &targets_);
    virtual ~cost_module();
  };

  //! A module with 2 inputs that computes
  //! 0.5 times the sum of square difference between
  //! the components of the inputs. The two inputs
  //! must be states of the same size.
  class euclidean_module : public cost_module<state_idx,int> {
  public:
    euclidean_module(idx<double> &targets_);
    virtual ~euclidean_module();

    //! Computes 0.5 times the sum of square difference between
    //! the components of state <in1> and the components of
    //! state <in2> (where a copy of the target corresponding to <label>
    //! is copied. Write the result into 0-dimensional state <energy>.
    virtual void fprop(state_idx &in1, int &label, state_idx &energy);

    //! Back-propagates gradients through <euclidean-module>.
    //! This multiplies the gradient of some function with respect
    //! to <energy> (stored in the <dx> slot of <energy>) by the
    //! jacobian of the <euclidean-module> with respect to its inputs.
    //! The result is written into the <dx> slots of <in1> and
    //! <in2>.
    virtual void bprop(state_idx &in1, int &label, state_idx &energy);

    //! mse has this funny property that the bbprop method mixes up the
    //! the first derivative after with the second derivative before, and
    //! vice versa. Only the first combination is used here.
    virtual void bbprop(state_idx &in1, int &label, state_idx &energy);

    //! TODO: implement?
    virtual void forget(forget_param_linear &fp) {}

    //! compute value of in2 that minimizes the energy, given in1
    virtual double infer2(state_idx &i1, int &i2, infer_param &ip, 
			  int *label = NULL, state_idx *energy = NULL);
  };

  ////////////////////////////////////////////////////////////////

  //! performs a log-add over spatial dimensions of an idx3-state
  //! output is an idx1-state
  class logadd_layer { //: public module_1_1<state_idx, state_idx> { // TODO
  public:
    idx<double> expdist;
    idx<double> sumexp;

    logadd_layer(intg thick, intg si, intg sj);
    virtual ~logadd_layer() {
    }
    void fprop(state_idx *in, state_idx *out);
    void bprop(state_idx *in, state_idx *out);

    //! this is not algebraically correct, but it's
    //! numerically more stable (at least, I think so).
    void bbprop(state_idx *in, state_idx *out);
  };

} // namespace ebl {

#include "ebl_cost.hpp"

#endif /* EBLCOST_H_ */
