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

#include "ebl_codec.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // codec

  codec::codec() {
  }

  codec::~codec() {
  }

  void codec::fprop(state_idx &in1, state_idx &in2, state_idx &energy) {
    // initialize z with a simple one-pass fprop through whole machine
    fprop_one_pass(in1, in2, energy);
    // now do gradient descent to find optimal code z
    bprop_dec(in1, in2, energy);
  }
  
  // simple one-pass forward propagation
  void codec::fprop_one_pass(state_idx &in1, state_idx &in2, 
			     state_idx &energy) {
    encoder.fprop(in1, enc_out);
    // let the enc-cost produce its best guess
    // for what the code should be. If the cost
    // is an l2-distance, this will simply 
    // copy :enc-out:x into :z:x
    // There is no need to do an fprop of
    // the encoder afterward, because infer2 does it.
    enc_cost.infer2_copy(enc_out, z, enc_energy);
    // compute cost penalty
    z_cost.fprop(z, z_energy);
    // fprop through decoder
    decoder.fprop(z, dec_out);
    // fprop through decoder cost.
    dec_cost.fprop(dec_out, in2, dec_energy);
    // add up energy terms
    energy.x.clear();
    idx_dotcacc(enc_energy.x, weight_energy_enc, energy.x);
    idx_dotcacc(dec_energy.x, weight_energy_dec, energy.x);
    idx_dotcacc(z_energy.x, weight_energy_z, energy.x);
  }
  
  // multiple-pass bprop on the decoder only to find the optimal code z
  void codec::bprop_dec(state_idx &in1, state_idx &in2, 
			state_idx &energy) {
    z.dx.clear();
  }

  void codec::bprop(state_idx &in1, state_idx &in2, state_idx &energy) {
  }

  void codec::bbprop(state_idx &in1, state_idx &in2, state_idx &energy) {
  }

  void codec::forget(forget_param_linear &fp) {
  }

  void codec::normalize() {
  }

} // end namespace ebl
