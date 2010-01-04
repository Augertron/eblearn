/***************************************************************************
 *   Copyright (C) 2009 by Yann LeCun and Pierre Sermanet *
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

#ifndef EBL_TRANSFER_H_
#define EBL_TRANSFER_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_states.h"
#include "ebl_basic.h"
#include "ebl_arch.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // weighted_std_module
  //! Weighted Standard Deviation Module.
  //! Local contrast normalization operation using a weighted expectation
  //! over a local neighborhood. An input set of feature maps is locally
  //! normalized to be zero mean and unit standard deviation.
  class weighted_std_module : public module_1_1<state_idx, state_idx> {
  private:
    layers_n<state_idx> convmean, convvar;
    power_module	sqrtmod;	//!< square root module
    power_module	invmod; //!< inverse module
    power_module	sqmod;	//!< square module
    diff_module		difmod; //!< difference module
    thres_module	thres;	//!< threshold module
    mul_module		mcw;
    state_idx		inmean, inzmean, inzmeansq, invar, instd, thstd, invstd;
    parameter		param;
    idx<double>         w; //!< weights

  public:
    //! <weighting> is <idx2<double>> that defines the weighting around
    //! the center component.
    //! <nfeatures> is the number of feature maps input to this module.
    weighted_std_module(uint kernelh, uint kernelw, int nf);
    //! destructor
    virtual ~weighted_std_module();    
    //! forward propagation from in to out
    virtual void fprop(state_idx &in, state_idx &out);
    //! backward propagation from out to in
    virtual void bprop(state_idx &in, state_idx &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(state_idx &in, state_idx &out);

    // friends
    friend class weighted_std_module_gui;
  };

  ////////////////////////////////////////////////////////////////
  // abs_module
  //! This module takes the absolute value of its input.
  //! This module is spatially replicable 
  //! (the input can have an order greater than 1 and the operation will apply
  //! to all elements).
  class abs_module: public module_1_1<state_idx, state_idx> {
  private:
    double threshold;
    
  public:
    //! Constructor. threshold makes the derivative of abs flat around zero
    //! with radius threshold.
    abs_module(double thresh = 0.0);
    virtual ~abs_module();
    //! forward propagation from in to out
    virtual void fprop(state_idx &in, state_idx &out);
    //! backward propagation from out to in
    virtual void bprop(state_idx &in, state_idx &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(state_idx &in, state_idx &out);
  };

} // namespace ebl {

#endif /* EBL_TRANSFER_H_ */
