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

#ifndef EBL_CODEC_H_
#define EBL_CODEC_H_

#include "defines.h"
#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_states.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  //! generic coder/decoder module.
  class codec: public ebm_2<state_idx, state_idx> {
  public:
    // encoder
    module_1_1<state_idx, state_idx>	*encoder;
    state_idx				 enc_out;
    ebm_2<state_idx, state_idx>		*enc_cost;
    double				 weight_energy_enc;
    state_idx				 enc_energy;
    // z
    state_idx				 z;
    ebm_2<state_idx, state_idx>		*z_cost;
    double				 weight_energy_z;
    state_idx				 z_energy;
    // decoder
    module_1_1<state_idx, state_idx>	*decoder;
    state_idx				 dec_out;
    ebm_2<state_idx, state_idx>		*dec_cost;
    double				 weight_energy_dec;
    state_idx				 dec_energy;
    bool				 owns_modules;

    //! Constructor.
    codec(module_1_1<state_idx, state_idx>	*encoder_,
	  ebm_2<state_idx, state_idx>		*enc_cost_,
	  double				 weight_energy_enc_,
	  ebm_2<state_idx, state_idx>		*z_cost_,
	  double				 weight_energy_z_,
	  module_1_1<state_idx, state_idx>	*decoder_,
	  ebm_2<state_idx, state_idx>		*dec_cost_,
	  double				 weight_energy_dec_,
	  bool					 owns_modules = false);
    //! destructor.
    virtual ~codec();
    //! forward propagation of in1 and in2
    virtual void fprop(state_idx &in1, state_idx &in2, state_idx &energy);
    //! backward propagation
    virtual void bprop(state_idx &in1, state_idx &in2, state_idx &energy);
    //! second-derivative backward propagation
    virtual void bbprop(state_idx &in1, state_idx &in2, state_idx &energy);
    //! forgetting weights by replacing with random values
    virtual void forget(forget_param_linear &fp);
    //! normalize
    virtual void normalize();

  protected:
    //! forward propagation of in1 and in2, a simple one pass propagation
    virtual void fprop_one_pass(state_idx &in1, state_idx &in2, 
				state_idx &energy);
    //! simple one-pass backward propagation
    virtual void bprop_one_pass(state_idx &in1, state_idx &in2, 
				state_idx &energy);
    //! multiple-pass bprop on the decoder only to find the optimal code z
    virtual void bprop_optimal_code(state_idx &in1, state_idx &in2, 
				    state_idx &energy, gd_param &infp);
  };

  ////////////////////////////////////////////////////////////////
  //! a single layer encoder/decoder architecture with an L1 penalty
  class codec_lone: codec {
  public:
    //! constructor.
    codec_lone(module_1_1<state_idx, state_idx>		*encoder_,
	       module_1_1<state_idx, state_idx>	        *decoder_,
	       double					 weight_energy_enc_,
	       double					 weight_energy_z_,
	       double					 weight_energy_dec_,
	       double					 thres);
    //! destructor.
    virtual ~codec_lone();
  };
  
} // namespace ebl {

#endif /* EBL_CODEC_H_ */
