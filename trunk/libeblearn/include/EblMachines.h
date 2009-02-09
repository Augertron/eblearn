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

#ifndef EBLMACHINES_H_
#define EBLMACHINES_H_

#include "Defines.h"
#include "Idx.h"
#include "Blas.h"
#include "EblStates.h"
#include "EblBasic.h"
#include "EblArch.h"
#include "EblNonLinearity.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////

  //! a simple fully-connected neural net layer: linear + tanh non-linearity.
  //! Unlike the f-layer class, this one is not spatially replicable.
  class nn_layer_full: public module_1_1<state_idx, state_idx> {
  public:
    //! linear module for weight matrix
    linear_module_dim0 *linear;
    //! bias vector
    addc_module *adder;
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

} // namespace ebl {

#endif /* EBLMACHINES_H_ */
