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
  supervised_trainer(fc_ebm2<state_idx,	int, state_idx> &m, parameter &p,
		     ostream &cout_)
    : iteration(-1), iteration_ptr(NULL),
      machine(m), param(p), energy(), label(), age(0), cout(cout_) {
    input = NULL; // allocated when input is passed, based in its order/dims
    energy.dx.set(1.0);
    energy.ddx.set(0.0);
  }

  template <class Tdata, class Tlabel>  
  supervised_trainer<Tdata, Tlabel>::~supervised_trainer() {
    if (input) delete input;
  }
  
  template <class Tdata, class Tlabel>  
  int supervised_trainer<Tdata, Tlabel>::run(state_idx &input, 
					     infer_param &infp,
					     int *label) {
    int answer = -1;
    // infer answer from energies and fill energy of correct answer
    machine.infer2(input, answer, infp, label, &energy);
    return answer;
  }
  
  template <class Tdata, class Tlabel>  
  bool supervised_trainer<Tdata, Tlabel>::
  test_sample(state_idx &input, int label, int &answer, infer_param &infp) {
    answer = run(input, infp, &label);
    return (label == answer); // return true if correct answer
  }

  template <class Tdata, class Tlabel>  
  idx<double> supervised_trainer<Tdata, Tlabel>::
  learn_sample(state_idx &input, int label, gd_param &args) {
    machine.fprop(input, label, energy);
    param.clear_dx();
    machine.bprop(input, label, energy);
    param.update(args);
    return energy.x.get();
  }

  template <class Tdata, class Tlabel>  
  void supervised_trainer<Tdata, Tlabel>::
  test(labeled_datasource<Tdata, Tlabel> &ds, classifier_meter &log,
       infer_param &infp) {
    ds.seek_begin();
    log.clear();
    resize_input(ds);
    bool correct;
    int answer;
    if (!iteration_ptr) 
      iteration_ptr = (void *) &ds;
    if (iteration_ptr == (void *) &ds)
      ++iteration;
    cout << ds.name << ": iter# " << iteration << " ";
    for (unsigned int i = 0; i < ds.size(); ++i) {
      ds.fprop(*input, label);
      correct = test_sample(*input, label.get(), answer, infp);
      log.update(age, correct, energy);
      ds.next();
    }
    log.display();
    cout << endl;
  }
  
  template <class Tdata, class Tlabel>  
  void supervised_trainer<Tdata, Tlabel>::
  train(labeled_datasource<Tdata, Tlabel> &ds, classifier_meter &log, 
	gd_param &args, int niter) {
    ds.seek_begin();
    log.clear();
    resize_input(ds);
    for (int i = 0; i < niter; ++i) { // niter iterations
      for (unsigned int j = 0; j < ds.size(); ++j) { // training on entire set
	ds.fprop(*input, label);
	int lab = label.get();
	learn_sample(*input, lab, args);
	//      log.update(age, output, label.get(), energy);
	age++;
	ds.next();
      }
    }
  }

  template <class Tdata, class Tlabel>  
  void supervised_trainer<Tdata, Tlabel>::
  compute_diaghessian(labeled_datasource<Tdata, Tlabel> &ds, intg niter, 
		      double mu) {
    resize_input(ds);
    param.clear_ddeltax();
    for (int i = 0; i < niter; ++i) {
      ds.fprop(*input, label);
      int lab = label.get();
      machine.fprop(*input, lab, energy);
      param.clear_dx();
      machine.bprop(*input, lab, energy);
      param.clear_ddx();
      machine.bbprop(*input, lab, energy);
      param.update_ddeltax((1 / (double) niter), 1.0);
      ds.next();
    }
    param.compute_epsilons(mu);
    cout << "diaghessian inf: " << idx_min(param.epsilons);
    cout << " sup: " << idx_max(param.epsilons) << endl;
  }

  template <class Tdata, class Tlabel>  
  void supervised_trainer<Tdata, Tlabel>::
  resize_input(labeled_datasource<Tdata, Tlabel> &ds) {
    idxdim d = ds.sample_dims();
    if (!input) input = new state_idx(d);
    else input->resize(d);
  }

} // end namespace ebl
