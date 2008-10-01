/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet  *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com   *
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

#include "Trainer.h"
#include "IdxIO.h"

using namespace std;

namespace ebl {

eb_trainer::eb_trainer(idx3_supervised_module *m, parameter *p,
		state_idx *e, state_idx *in) {
  machine = m;
  param = p;
  age = 0;
  energy_owned = false;
  input_owned = false;
  if (e != NULL)
  	energy = e;
  else {
    // set energy slot to default idx0-ddstate
    energy = new state_idx();
    energy_owned = true;
    energy->dx.set(1.0);
    energy->ddx.set(0.0);
  }
  if (in != NULL)
  	input = in;
  else {
  	input = new state_idx(1, 1, 1);
  	input_owned = true;
  }
}

eb_trainer::~eb_trainer() {
	if (energy_owned) delete energy;
	if (input_owned) delete input;
}

////////////////////////////////////////////////////////////////

supervised::supervised(idx3_supervised_module *m, parameter *p,
		state_idx *e, state_idx *in,
		class_state *out, Idx<ubyte> *des)
: eb_trainer(m, p, e, in) {
  output_owned = true;
  desired_owned = true;
  if (out != NULL)
  	output = out;
  else {
    output = new class_state(2);
    output_owned = true;
  }
  if (des != NULL)
    desired = des;
  else {
    desired = new Idx<ubyte>();
    desired_owned = true;
  }
}

supervised::~supervised() {
	if (output_owned) delete output;
	if (desired_owned) delete desired;
}



////////////////////////////////////////////////////////////////

supervised_gradient::supervised_gradient(idx3_supervised_module *m, parameter *p,
		state_idx *e, state_idx *in,
		class_state *out, Idx<ubyte> *des)
: supervised(m, p, e, in, out, des) {
}

supervised_gradient::~supervised_gradient() {
}


} // end namespace ebl
