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

#include "Idx.h"
#include "Blas.h"
#include "EblArch.h"
#include "EblStates.h"

namespace ebl {

  extern bool drand_ini;

  void err_not_implemented();

  ////////////////////////////////////////////////////////////////
  // linear module
  // It's different from f_layer in that it is
  // not spatially replicable and does not operate
  // on 3D state_idx.

  class linear_module: public module_1_1<state_idx, state_idx> {
  public:
    state_idx *w;

    virtual ~linear_module();
    linear_module(parameter *p, intg in, intg out);
    void fprop(state_idx *in, state_idx *out);
    void bprop(state_idx *in, state_idx *out);
    void bbprop(state_idx *in, state_idx *out);
    void forget(forget_param_linear &fp);
    void normalize();
  };

  ////////////////////////////////////////////////////////////////

  //! constant add
  class addc_module: public module_1_1<state_idx, state_idx> {
  public:
    // coefficients
    state_idx* bias;
    addc_module(parameter *p, intg size);
    ~addc_module();
    //! fprop from in to out
    void fprop(state_idx *in, state_idx *out);
    //! bprop
    void bprop(state_idx *in, state_idx *out);
    //! bbprop
    void bbprop(state_idx *in, state_idx *out);
    void forget(forget_param_linear &fp);
    void normalize();
  };

} // namespace ebl {

#endif /* EBLBASIC_H_ */
