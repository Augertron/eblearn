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

#include "EblMachines.h"

using namespace std;

namespace ebl {

  trainer::trainer(trainable_machine<state_idx,state_idx,state_idx> &tm,
		   parameter &p)
    : tmachine(tm), param(p) {
    input1 = new state_idx(1); // TODO
    input2 = new state_idx(1); // TODO
  }

  trainer::~trainer() {
    delete input1;
    delete input2;
  }
  
  intg trainer::run(Idx<double> &sample, Idx<double> &energies) {
    // input->resize like sample in dim0 // TODO
    idx_copy(sample, input->x); // copy sample in input state
    tmachine.fprop_energies(input, energies);
    return tmachine.infer2(); // infer answer from energies
  }
  
  bool trainer::test_sample(Idx<double> &sample, int label) {
    Idx<double> energies();
    int answer = run(sample, energies);
    return (label == answer); // return true if correct answer
  }

  Idx<double> trainer::learn_sample(Idx<double> &sample, int label, 
				    gd_param &args) {
    // input->resize like sample in dim0 // TODO
    idx_copy(sample, input->x); // copy sample in input state
    tmachine.fprop(input, label, energy);
    tmachine.bprop(input, label, energy);
    param.update(args);
    //    return energy
  }

  Idx<double> trainer::test(LabeledDataSource &ds) {
  }
  
  double trainer::train(LabeledDataSource &ds, int niter, gd_param &args) {
  }

} // end namespace ebl
