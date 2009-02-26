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

  ////////////////////////////////////////////////////////////////
  // supervised_trainer

  template <class Tdata, class Tlabel>  
  supervised_trainer<Tdata, Tlabel>::
  supervised_trainer(fc_ebm2<state_idx,	state_idx,int> &tm, parameter &p)
    : tmachine(tm), param(p), energy(), label(), age(0) {
    input = new state_idx(1,1,1); // TODO
  }

  template <class Tdata, class Tlabel>  
  supervised_trainer<Tdata, Tlabel>::~supervised_trainer() {
    delete input;
  }
  
  template <class Tdata, class Tlabel>  
  int supervised_trainer<Tdata, Tlabel>::run(Idx<double> &sample, 
					     Idx<double> &energies) {
    // input->resize like sample in dim0 // TODO
    idx_copy(sample, input->x); // copy sample in input state
    int answer = -1;
    tmachine.infer2(*input, answer, energy); // infer answer from energies
    return answer;
  }
  
  template <class Tdata, class Tlabel>  
  bool supervised_trainer<Tdata, Tlabel>::
  test_sample(Idx<double> &sample, int label) {
    Idx<double> energies();
    int answer = run(sample, energies);
    return (label == answer); // return true if correct answer
  }

  template <class Tdata, class Tlabel>  
  Idx<double> supervised_trainer<Tdata, Tlabel>::
  learn_sample(Idx<double> &sample, int label, gd_param &args) {
    // input->resize like sample in dim0 // TODO
    idx_copy(sample, input->x); // copy sample in input state
    tmachine.fprop(*input, label, energy);
    param.clear_dx();
    tmachine.bprop(*input, label, energy);
    param.update(args);
    return energy.x.get();
  }

  template <class Tdata, class Tlabel>  
  Idx<double> supervised_trainer<Tdata, Tlabel>::
  test(LabeledDataSource<Tdata, Tlabel> &ds, classifier_meter &log) {
    ds.seek_begin();
    mtr.clear();
    for (int i = 0; i < ds.size(); ++i) {
      ds.fprop(*input, label);
      tmachine.fprop(*input, label.get(), energy);
      log.update(age, output, label.get(), energy);
      ds.next();
    }
  }
  
  template <class Tdata, class Tlabel>  
  void supervised_trainer<Tdata, Tlabel>::
  train(LabeledDataSource<Tdata, Tlabel> &ds, classifier_meter &log, 
	gd_param &args, int niter) {
    ds.seek_begin();
    mtr.clear();
    for (int i = 0; i < niter; ++i) {
      ds.fprop(*input, desired);
      learn_sample(*input, label, args);
      //      log.update(age, output, label.get(), energy);
      age++;
      ds.next();
    }
  }

  template <class Tdata, class Tlabel>  
  void supervised_trainer<Tdata, Tlabel>::
  compute_diaghessian(LabeledDataSource<Tdata, Tlabel> &ds, intg niter, 
		      double mu) {
    param.clear_ddeltax();
    for (int i = 0; i < niter; ++i) {
      ds.fprop(*input, label);
      tmachine.fprop(*input, label, energy);
      param.clear_dx();
      tmachine.bprop(*input, label, energy);
      param.clear_ddx();
      tmachine.bbprop(*input, label, energy);
      param.update_ddeltax((1 / (double) niter), 1.0);
      ds.next();
    }
    param.compute_epsilons(mu);
    std::cout << "diaghessian inf: " << idx_min(param.epsilons);
    std::cout << " sup: " << idx_max(param.epsilons) << std::endl;
  }

} // end namespace ebl
