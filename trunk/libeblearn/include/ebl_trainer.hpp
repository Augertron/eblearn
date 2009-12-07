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
  // stochastic_gd_trainer

  template <class Tin1, class Tin2>  
  stochastic_gd_trainer<Tin1, Tin2>::
  stochastic_gd_trainer(fc_ebm2<Tin1, Tin2, state_idx> &m, parameter &p)
    : iteration(-1), iteration_ptr(NULL), prettied(false),
      machine(m), param(p), energy(), age(0),
      in1(NULL), // allocated when input is passed, based in its order/dims
      in2(NULL) // allocated when input is passed, based in its order/dims
  {
    energy.dx.set(1.0);
    energy.ddx.set(0.0);
  }

  template <class Tin1, class Tin2>
  stochastic_gd_trainer<Tin1, Tin2>::~stochastic_gd_trainer() {
    if (in1) delete in1;
    if (in2) delete in2;
  }

  // train: train on all samples
  template <class Tin1, class Tin2>  
  void stochastic_gd_trainer<Tin1, Tin2>::
  train(datasource<Tin1, Tin2> &ds, classifier_meter &log, 
	gd_param &args, int niter,
	bool compute_hessian, int hessian_interval,
	int niter_hessian, double mu_hessian, intg max) {
    // initialize
    ds.seek_begin();
    log.clear();
    resize_input(ds);
    // if not computing hessian, fill the epsilons with 1
    if (!compute_hessian)
      param.set_epsilons(1.0);
    // loop over samples for niter iterations
    for (int i = 0; i < niter; ++i) { // niter iterations
      for (unsigned int j = 0; j < ds.size(); ++j) { // training on entire set
	// compute hessian after hessian_interval iterations
	if (compute_hessian && (age % hessian_interval == 0))
	  compute_diaghessian(ds, niter_hessian, mu_hessian);
	// do one step of training
	train_sample(ds, args);
	// advance data and age by one
	ds.next();
	age++;
      }
    }
  }

  // train_sample: train on one sample
  template <class Tin1, class Tin2>
  void stochastic_gd_trainer<Tin1, Tin2>::
  train_sample(datasource<Tin1, Tin2> &ds, gd_param &args) {
    // fprop input
    ds.fprop(*in1, *in2);
    // fprop machine
    machine.fprop(*in1, *in2, energy);
    // bprop machine
    param.clear_dx();
    machine.bprop(*in1, *in2, energy);
    // update parameters
    param.update(args);
    // update machine
    machine.normalize();
    // return total energy
    return energy.x.get();
  }

  // compute_diaghessian
  template <class Tin1, class Tin2>  
  void stochastic_gd_trainer<Tin1, Tin2>::
  compute_diaghessian(datasource<Tin1, Tin2> &ds, intg niter, 
		      double mu) {
    resize_input(ds);
    param.clear_ddeltax();
    for (int i = 0; i < niter; ++i) {
      ds.fprop(*in1, *in2);
      machine.fprop(*in1, *in2, energy);
      param.clear_dx();
      machine.bprop(*in1, *in2, energy);
      param.clear_ddx();
      machine.bbprop(*in1, *in2, energy);
      param.update_ddeltax((1 / (double) niter), 1.0);
      ds.next();
    }
    param.compute_epsilons(mu);
    cout << "diaghessian inf: " << idx_min(param.epsilons);
    cout << " sup: " << idx_max(param.epsilons) << endl;
  }

  // resize_input
  template <class Tin1, class Tin2>  
  void stochastic_gd_trainer<Tin1, Tin2>::
  resize_input(datasource<Tin1, Tin2> &ds) {
    idxdim d = ds.sample_dims();
    if (!in1)
      in1 = new state_idx(d);
    else
      in1->resize(d);
    if (!in2)
      in2 = new state_idx(d);
    else
      in2->resize(d);
    // pretty sizes of input/output for each module the first time
    if (!prettied) {
      machine.pretty(d);
      prettied = true;
    }
  }

  ////////////////////////////////////////////////////////////////
  // supervised_trainer

  template <class Tdata, class Tlabel>  
  supervised_trainer<Tdata, Tlabel>::
  supervised_trainer(fc_ebm2<state_idx,	int, state_idx> &m, parameter &p)
    : iteration(-1), iteration_ptr(NULL), prettied(false),
      machine(m), param(p), energy(), label(), age(0) {
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
  double supervised_trainer<Tdata, Tlabel>::
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
    cout << iteration << " (" << ds.name << "): ";
    for (unsigned int i = 0; i < ds.size(); ++i) {
      ds.fprop(*input, label);
      correct = test_sample(*input, label.get(), answer, infp);
      log.update(age, label.get(), answer, energy);
      ds.next();
    }
    log.display(ds.lblstr);
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
    // pretty sizes of input/output for each module the first time
    if (!prettied) {
      cout << "machine sizes: "; machine.fmod.pretty(d);
      cout << "trainable parameters: " << param.x << endl;
      prettied = true;
    }
  }

//   supervised_trainer<Tdata, Tlabel>* copy() {
//     fc_ebm2<state_idx,int,state_idx>	&machine;
//     parameter				&param;
//   }

} // end namespace ebl
