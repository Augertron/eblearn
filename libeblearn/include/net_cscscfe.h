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

#ifndef NET_CSCSCFE_H_
#define NET_CSCSCFE_H_

#include "Ebl.h"
#include "Net.h"

namespace ebl {

  //! full connection with Euclidean distance between replicable 
  //! 3D layers. This is like a layer of RBF WITHOUT NON-LINEAR FUNCTIONS.
  //! the output is the square Euclidean distance between the input
  //! and the weight
  //! the full connection is only across the first dimension 
  //! of the input and output layer.
  //! the other two dimensions are spatial dimensions accross which the 
  //! weight matrix is shared.
  class e_layer :
  public module_1_1<state_idx, state_idx>
    {
    public:
      //! weight matrix
      state_idx weight;

      virtual ~e_layer(void);
      //! new e-layer (Eucliden distance RBF). <tin> is the
      //! thickness of the input layer, <tout> is the thickness 
      //! of the output layer, <prm> is the parameter.
      e_layer(parameter &prm, intg tin, intg tout);	
      //! fprop from in to out
      void fprop(state_idx &in, state_idx &out);
      //! bprop
      void bprop(state_idx &in, state_idx &out);
      //! bbprop
      void bbprop(state_idx &in, state_idx &out);
      //! initialize the weights to random values
      void forget(forget_param_linear &fp);
    };

  //! Standard LeNet5-type architecture with the "RBF" layer on top
  class net_cscscfe : public module_1_1<state_idx, state_idx>
    {
    public:
      nn_machine_cscscf	&net;
      state_idx		netout;
      e_layer		&rbf;

      //! makes a new net_cscscfe module.
      //! <n> is a net-cscscf, <e> is an e-dist layer,
      //! <thick> is the number of outputs.
      //! <si>,<sj> are the initial replication factors.
      net_cscscfe(nn_machine_cscscf &n, e_layer &e, int thick, int si, int sj);
      virtual ~net_cscscfe();

      //! initialize the weights to random values
      void forget(forget_param_linear &fp);
      //! fprop from in to out
      virtual void fprop(state_idx &in, state_idx &out);
      //! bprop
      virtual void bprop(state_idx &in, state_idx &out);
      //! bbprop
      virtual void bbprop(state_idx &in, state_idx &out);
    };

} // end namespace ebl

#endif /* NET_CSCSCFE_H_ */
