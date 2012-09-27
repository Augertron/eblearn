/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
 *   All rights reserved.
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

// cost_module /////////////////////////////////////////////////////////////////

//! cost module base class
template <typename T> class cost_module : public ebm_2<T> {
 public:
  //! Constructor. Keep a reference to targets and allocate other buffers
  //! based on the targets order and dimensions.
  cost_module(idx<T1> &targets_);
  virtual ~cost_module();

 public:
  idx<T>   &targets;  //!< all the input targets
  state<T>  in2;      //!< Tmp holding target of current label.
  idx<T>    energies; //! the energy for each target
};

// eucliden_module /////////////////////////////////////////////////////////////

//! A module with 2 inputs that computes
//! 0.5 times the sum of square difference between
//! the components of the inputs. The two inputs
//! must be states of the same size.
template <typename T> class euclidean_module : public cost_module<T> {
 public:
  //! constructor. targets are output targets to train toward.
  euclidean_module(idx<T> &targets_);
  //! destructor.
  virtual ~euclidean_module();

  //! Computes 0.5 times the sum of square difference between
  //! the components of state <in1> and the components of
  //! state <in2> (where a copy of the target corresponding to <label>
  //! is copied. Write the result into 0-dimensional state <energy>.
  virtual void fprop1(state<T> &in1, Tstate2 &in2, state<T> &energy);

  //! Back-propagates gradients through <euclidean-module>.
  //! This multiplies the gradient of some function with respect
  //! to <energy> (stored in the <dx> slot of <energy>) by the
  //! jacobian of the <euclidean-module> with respect to its inputs.
  //! The result is written into the <dx> slots of <in1> and
  //! <in2>.
  VIRTUAL void bprop(state<T> &in1, Tstate2 &in2, state<T> &energy);

  //! mse has this funny property that the bbprop method mixes up the
  //! the first derivative after with the second derivative before, and
  //! vice versa. Only the first combination is used here.
  VIRTUAL void bbprop(state<T> &in1, Tstate2 &in2, state<T> &energy);

  //! TODO: implement?
  VIRTUAL void forget(forget_param_linear &fp) {}

  //! compute value of in2 that minimizes the energy, given in1
  VIRTUAL double infer2(state<T> &i1, Tstate2 &i2, infer_param &ip,
                        Tstate2 *label = NULL, state<T> *energy = NULL);

 protected:
  using cost_module<T1, T2, state<T>, Tstate2>::targets;
  using cost_module<T1, T2, state<T>, Tstate2>::in2;
  using cost_module<T1, T2, state<T>, Tstate2>::energies;
};

////////////////////////////////////////////////////////////////
//! performs a log-add over spatial dimensions of an idx3-state
//! output is an idx1-state
template<class T>
class logadd_layer { //: public module_1_1<fstate_idx, fstate_idx> { // TODO
 public:
  idx<T> expdist;
  idx<T> sumexp;

  logadd_layer(intg thick, intg si, intg sj);
  virtual ~logadd_layer() {
  }
  void fprop(fstate_idx<T> *in, fstate_idx<T> *out);
  void bprop(fstate_idx<T> *in, fstate_idx<T> *out);

  //! this is not algebraically correct, but it's
  //! numerically more stable (at least, I think so).
  void bbprop(fstate_idx<T> *in, fstate_idx<T> *out);
};

////////////////////////////////////////////////////////////////
//! distance_l2
template<class T> class distance_l2 : public ebm_2<T> {
 private:
  idx<T> tmp;

 public:
  distance_l2();
  virtual ~distance_l2();

  virtual void fprop(fstate_idx<T> &in1, fstate_idx<T> &in2,
                     fstate_idx<T> &energy);
  VIRTUAL void bprop(fstate_idx<T> &in1, fstate_idx<T> &in2,
                     fstate_idx<T> &energy);
  VIRTUAL void bbprop(fstate_idx<T> &in1, fstate_idx<T> &in2,
                      fstate_idx<T> &energy);
  VIRTUAL void forget(forget_param_linear &fp);
  VIRTUAL void infer2_copy(fstate_idx<T> &i1, fstate_idx<T> &i2,
                           fstate_idx<T> &energy);
};

////////////////////////////////////////////////////////////////
//! penalty_l1
template<class T> class penalty_l1 : public ebm_1<T> {
 private:
  double threshold;

 public:
  penalty_l1(T threshold_);
  virtual ~penalty_l1();

  virtual void fprop(fstate_idx<T> &in, fstate_idx<T> &energy);
  VIRTUAL void bprop(fstate_idx<T> &in, fstate_idx<T> &energy);
  VIRTUAL void bbprop(fstate_idx<T> &in, fstate_idx<T> &energy);
  VIRTUAL void forget(forget_param_linear &fp);
};

} // namespace ebl {

#include "ebl_cost.hpp"

#endif /* EBLCOST_H_ */
