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

namespace ebl {

  ////////////////////////////////////////////////////////////////////////
  // trainable machine

  template<class Tin1, class Tin2, class Tout>
  trainable_machine<Tin1, Tin2, Tout>::trainable_machine(module_1_1<Tin1,Tout> 
							 *machine_,
							 module_2_1<Tin1,Tin2,
							 Tout> 
							 *cost_) {
    machine = machine_;
    cost = cost_;
    mout = new state_idx(1); // TODO
  }

  template<class Tin1, class Tin2, class Tout>
  trainable_machine<Tin1, Tin2, Tout>::~trainable_machine() {
    delete mout;
  }

  template<class Tin1, class Tin2, class Tout>
  void trainable_machine<Tin1, Tin2, Tout>::fprop(Tin1 *in1, Tin2 *in2, 
						  Tout *out) {
    machine->fprop(in1, mout);
    cost->fprop(mout, in2, out);
  }

  template<class Tin1, class Tin2, class Tout>
  void trainable_machine<Tin1, Tin2, Tout>::bprop(Tin1 *in1, Tin2 *in2, 
						  Tout *out) {
    cost->bprop(mout, in2, out);
    machine->bprop(in1, mout);
  }

  template<class Tin1, class Tin2, class Tout>
  void trainable_machine<Tin1, Tin2, Tout>::bbprop(Tin1 *in1, Tin2 *in2, 
						   Tout *out) {
    cost->bbprop(mout, in2, out);
    machine->bbprop(in1, mout);
  }

} // end namespace ebl
