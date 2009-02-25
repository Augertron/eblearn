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

#ifndef EBLCOST_H_
#define EBLCOST_H_

#include "Blas.h"
#include "EblArch.h"

namespace ebl {

  //! cost module base class
  template<class Tin1, class Tin2, class Tout>
    class cost_module : public module_2_1<Tin1, Tin2, Tout> {
  public:
    Idx<double> &targets;
    state_idx in2;
    state_idx out;

    cost_module(Idx<double> &targets_);
    virtual ~cost_module();

    virtual void fprop(Tin1 &in1, Tin2 &label, Tout &out) = 0;
    virtual void fprop_energies(Tin1 &in1, Idx<double> &energies);
    virtual void bprop(Tin1 &in1, Tin2 &label, Tout &out) = 0;
    virtual int infer2(Idx<double> &energies);
  };

  //! A module with 2 inputs that computes
  //! 0.5 times the sum of square difference between
  //! the components of the inputs. The two inputs
  //! must be states of the same size.
  class euclidean_module : public cost_module<state_idx,int,state_idx> {
  public:
    euclidean_module(Idx<double> &targets_);
    virtual ~euclidean_module();

    //! Computes 0.5 times the sum of square difference between
    //! the components of state <input1> and the components of
    //! state <input2>. Write the result into 0-dimensional state <output>.
    virtual void fprop(state_idx &in1, int &label, state_idx &out);

    //! Back-propagates gradients through <euclidean-module>.
    //! This multiplies the gradient of some function with respect
    //! to <output> (stored in the <dx> slot of <output>) by the
    //! jacobian of the <euclidean-module> with respect to its inputs.
    //! The result is written into the <dx> slots of <input1> and
    //! <input2>.
    virtual void bprop(state_idx &in1, int &label, state_idx &out);
  };

} // namespace ebl {

#include "EblCost.hpp"

#endif /* EBLCOST_H_ */
