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

#include "EblCost.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // euclidean_module
  
  euclidean_module::euclidean_module(Idx<double> &targets_)
    : cost_module<state_idx,int>(targets_) {
  }

  euclidean_module::~euclidean_module() {
  }

  void euclidean_module::fprop(state_idx &in1, int &label, state_idx &out) {
    Idx<double> target = targets.select(0, label);
    idx_copy(target, in2.x);
    // squared distance between in1 and target
    idx_sqrdist(in1.x, in2.x, out.x);
    idx_dotc(out.x, 0.5, out.x); // multiply by .5
  }
  
  void euclidean_module::bprop(state_idx &in1, int &label, state_idx &out) {
    idx_checkorder1(out.x, 0); // out.x must have an order of 0
    idx_sub(in1.x, in2.x, in1.dx); // derivative with respect to in1
    idx_dotc(in1.dx, out.dx.get(), in1.dx); // multiply by energy derivative
    idx_minus(in1.dx, in2.dx); // derivative with respect to in2
  }

  double euclidean_module::infer2(state_idx &i1, int &label, state_idx &energy,
				  infer_param &ip) {
    label = 0;
    idx_bloop1(e, energies, double) {
      fprop(i1, label, out);
      idx_copy(out.x, e);
      label++;
    }
    label = idx_indexmin(energies);
    return 0.0;
  }

} // end namespace ebl
