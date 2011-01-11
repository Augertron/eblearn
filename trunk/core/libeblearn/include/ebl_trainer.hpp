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

#include <typeinfo>
#include "utils.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // stochastic_gd_trainer

//   template <class Tin1, class Tin2, class T>  
//   stochastic_gd_trainer<Tin1, Tin2, T>::
//   stochastic_gd_trainer(fc_ebm2_gen<Tin1, Tin2, T> &m, parameter<T> &p)
//     : iteration(-1), iteration_ptr(NULL), prettied(false),
//       machine(m), param(p), energy(), age(0),
//       in1(NULL), // allocated when input is passed, based in its order/dims
//       in2(NULL) // allocated when input is passed, based in its order/dims
//   {
//     energy.dx.set(1.0);
//     energy.ddx.set(0.0);
//   }

//   template <class Tin1, class Tin2, class T>
//   stochastic_gd_trainer<Tin1, Tin2, T>::~stochastic_gd_trainer() {
//     if (in1) delete in1;
//     if (in2) delete in2;
//   }

//   // train: train on all samples
//   template <class Tin1, class Tin2, class T>  
//   void stochastic_gd_trainer<Tin1, Tin2, T>::
//   train(datasource<Tin1, Tin2> &ds, classifier_meter &log, 
// 	gd_param &args, int niter,
// 	bool compute_hessian, int hessian_interval,
// 	int niter_hessian, double mu_hessian, intg max) {
//     // initialize
//     ds.seek_begin();
//     log.clear();
//     resize_input(ds);
//     // if not computing hessian, fill the epsilons with 1
//     if (!compute_hessian)
//       param.set_epsilons(1.0);
//     // loop over samples for niter iterations
//     for (int i = 0; i < niter; ++i) { // niter iterations
//       for (unsigned int j = 0; j < ds.size(); ++j) { // training on entire set
// 	// compute hessian after hessian_interval iterations
// 	if (compute_hessian && (age % hessian_interval == 0))
// 	  compute_diaghessian(ds, niter_hessian, mu_hessian);
// 	// do one step of training
// 	train_sample(ds, args);
// 	// advance data and age by one
// 	ds.next();
// 	age++;
//       }
//     }
//   }

//   // train_sample: train on one sample
//   template <class Tin1, class Tin2, class T>
//   void stochastic_gd_trainer<Tin1, Tin2, T>::
//   train_sample(datasource<Tin1, Tin2> &ds, gd_param &args) {
//     // fprop input
//     ds.fprop(*in1, *in2);
//     // fprop machine
//     machine.fprop(*in1, *in2, energy);
//     // bprop machine
//     param.clear_dx();
//     machine.bprop(*in1, *in2, energy);
//     // update parameters
//     param.update(args);
//     // update machine
//     machine.normalize();
//     // return total energy
//     return energy.x.get();
//   }

//   // compute_diaghessian
//   template <class Tin1, class Tin2, class T>  
//   void stochastic_gd_trainer<Tin1, Tin2, T>::
//   compute_diaghessian(datasource<Tin1, Tin2> &ds, intg niter, 
// 		      double mu) {
//     resize_input(ds);
//     param.clear_ddeltax();
//     for (int i = 0; i < niter; ++i) {
//       ds.fprop(*in1, *in2);
//       machine.fprop(*in1, *in2, energy);
//       param.clear_dx();
//       machine.bprop(*in1, *in2, energy);
//       param.clear_ddx();
//       machine.bbprop(*in1, *in2, energy);
//       param.update_ddeltax((1 / (double) niter), 1.0);
//       ds.next();
//     }
//     param.compute_epsilons(mu);
//     cout << "diaghessian inf: " << idx_min(param.epsilons);
//     cout << " sup: " << idx_max(param.epsilons) << endl;
//   }

//   // resize_input
//   template <class Tin1, class Tin2, class T>  
//   void stochastic_gd_trainer<Tin1, Tin2, T>::
//   resize_input(datasource<Tin1, Tin2> &ds) {
//     idxdim d = ds.sample_dims();
//     if (!in1)
//       in1 = new state_idx<T>(d);
//     else
//       in1->resize(d);
//     if (!in2)
//       in2 = new state_idx<T>(d);
//     else
//       in2->resize(d);
//     // pretty sizes of input/output for each module the first time
//     if (!prettied) {
//       machine.pretty(d);
//       prettied = true;
//     }
//   }

  ////////////////////////////////////////////////////////////////
  // supervised_trainer

  template <class Tnet, class Tdata, class Tlabel>  
  supervised_trainer<Tnet, Tdata, Tlabel>::
  supervised_trainer(fc_ebm2<Tnet,bbstate_idx<Tnet>,bbstate_idx<Tlabel> > &m,
		     parameter<Tnet, bbstate_idx<Tnet> > &p)
    : machine(m), param(p), input(NULL), energy(), label(), answer(), age(0),
      iteration(-1), iteration_ptr(NULL), prettied(false) {
    energy.dx.set(1.0);
    energy.ddx.set(0.0);
  }

  template <class Tnet, class Tdata, class Tlabel>  
  supervised_trainer<Tnet, Tdata, Tlabel>::~supervised_trainer() {
  }
		     
  template <class Tnet, class Tdata, class Tlabel>  
  void supervised_trainer<Tnet, Tdata, Tlabel>::
  run(bbstate_idx<Tnet> &input, bbstate_idx<Tlabel> &label, infer_param &infp) {
    answer.x.set(-1); // TODO: is this necessary?
    // infer answer from energies and fill energy of correct answer
    machine.infer2(input, answer, infp, &label, &energy);
  }
  
  template <class Tnet, class Tdata, class Tlabel>  
  bool supervised_trainer<Tnet, Tdata, Tlabel>::
  test_sample(bbstate_idx<Tnet> &input, bbstate_idx<Tlabel> &label,
	      infer_param &infp) {
    run(input, label, infp);
    // TODO: for now, simple comparison, make this more generic
    return (label.x.get() == answer.x.get()); // return true if correct answer
  }

  template <class Tnet, class Tdata, class Tlabel>  
  double supervised_trainer<Tnet, Tdata, Tlabel>::
  learn_sample(bbstate_idx<Tnet> &input, bbstate_idx<Tlabel> &label,
	       gd_param &args) {
    machine.fprop(input, label, energy);
    param.clear_dx();
    machine.bprop(input, label, energy);
    param.update(args);
#ifdef __DUMP_STATES__
    save_matrix(input.x, "dump_trainer_net_in.x.mat");
    save_matrix(energy.x, "dump_trainer_net_energy.x.mat");
#endif
    return energy.x.get();
  }

  template <class Tnet, class Tdata, class Tlabel> 
  void supervised_trainer<Tnet, Tdata, Tlabel>::
  test(labeled_datasource<Tnet, Tdata, Tlabel> &ds, classifier_meter &log,
       infer_param &infp) {
    bool correct;
    init(ds, &log, true);
    // loop
    for (unsigned int i = 0; i < ds.size(); ++i) {
      ds.fprop(*input, label);
      correct = test_sample(*input, label, infp);
      log.update(age, (uint) label.x.get(), (uint) answer.x.get(),
		 (double) energy.x.get());
      // use energy as distance for samples probabilities to be used
      ds.set_sample_energy(energy.x.get(), correct, answer.x.get());
      ds.next();
    }
    ds.normalize_all_probas();
    log.display(iteration, ds.name(), ds.lblstr, ds.is_test());
    cout << endl;
  }

  // template <class Tnet, class Tdata, class Tlabel> 
  // void supervised_trainer<Tnet, Tdata, Tlabel>::
  // test_threshold(labeled_datasource<Tnet, Tdata, Tlabel> &ds,
  // 		 classifier_meter &log, infer_param &infp, double threshold,
  // 		 Tlabel defclass) {
  //   Tlabel answer, lab;
  //   idx<Tnet> raw;
  //   Tnet rawmax;
  //   init(ds, &log, true);
  //   // loop
  //   for (unsigned int i = 0; i < ds.size(); ++i) {
  //     ds.fprop(*input, label); // get sample
  //     lab = label.get();
  //     machine.fprop(*input, lab, energy); // fprop sample
  //     raw = machine.fout.x; // raw network outputs
  //     rawmax = idx_max(raw);
  //     if (rawmax < threshold) // no response stronger than threshold
  // 	answer = defclass; // default class when no detection
  //     else // stronger responses
  // 	answer = idx_indexmax(raw); // answer = maximum response
  //     // update TPR and FPR
  //     log.update(age, (uint) label.get(), (uint) answer,
  // 		 (double) energy.x.get());
  //     ds.next();
  //   }
  //   cout << "threshold: " << threshold << " default class: " << defclass <<endl;
  //   log.display_positive_rates(threshold, ds.lblstr);
  // }
  
  template <class Tnet, class Tdata, class Tlabel> 
  void supervised_trainer<Tnet, Tdata, Tlabel>::
  train(labeled_datasource<Tnet, Tdata, Tlabel> &ds, classifier_meter &log, 
	gd_param &args, int niter, infer_param &infp) {
    timer t;
    init(ds, &log);
    bool selected = true, correct;
    for (int i = 0; i < niter; ++i) { // niter iterations
      t.start();
      ds.init_epoch();
      // training on lowest size common to all classes (times # classes)
      while (!ds.epoch_done()) {
	ds.fprop(*input, label);
	if (selected) // selected for training
	  learn_sample(*input, label, args);
	// test if answer is correct
	correct = test_sample(*input, label, infp);
	// use energy and answer as distance for samples probabilities
	ds.set_sample_energy(energy.x.get(), correct, answer.x.get());
	//      log.update(age, output, label.get(), energy);
	age++;
	selected = ds.next_train();
      }
      ds.normalize_all_probas();
      cout << "epoch_count=" << ds.get_epoch_count() << endl;
      cout << "training_time="; t.pretty_elapsed();
      cout << " train_minutes=" << t.elapsed_minutes() << endl;
    }
  }

  template <class Tnet, class Tdata, class Tlabel> 
  void supervised_trainer<Tnet, Tdata, Tlabel>::
  compute_diaghessian(labeled_datasource<Tnet, Tdata, Tlabel> &ds, intg niter, 
		      double mu) {
    timer t;
    t.start();
    init(ds, NULL);
    ds.init_epoch();
    ds.save_state(); // save current ds state
    ds.set_count_pickings(false); // do not counts those samples in training
    param.clear_ddeltax();
    // loop
    for (int i = 0; i < niter; ++i) {
      ds.fprop(*input, label);
      machine.fprop(*input, label, energy);
      param.clear_dx();
      machine.bprop(*input, label, energy);
      param.clear_ddx();
      machine.bbprop(*input, label, energy);
      param.update_ddeltax((1 / (double) niter), 1.0);
      while (!ds.next_train()) ; // skipping all non selected samples
    }
    ds.restore_state(); // set ds state back
    param.compute_epsilons(mu);
    cout << "diaghessian inf: " << idx_min(param.epsilons);
    cout << " sup: " << idx_max(param.epsilons);
    cout << " diaghessian_minutes=" << t.elapsed_minutes() << endl;
  }

  template <class Tnet, class Tdata, class Tlabel>
  void supervised_trainer<Tnet, Tdata, Tlabel>::
  init(labeled_datasource<Tnet, Tdata, Tlabel> &ds,
       classifier_meter *log, bool new_iteration) {
    pretty(ds); // pretty info
    // if not allocated, allocate input. input is allocated dynamically
    // based on ds dimensions because fstate_idx cannot change orders.
    if (!input)
      input = new bbstate_idx<Tnet>(ds.sample_dims());
    else
      input->resize(ds.sample_dims());
    // reinit ds
    ds.seek_begin();
    if (log) // reinit logger
      log->init(ds.get_nclasses());
    // new iteration
    if (new_iteration) {
      if (!iteration_ptr) 
	iteration_ptr = (void *) &ds;
      if (iteration_ptr == (void *) &ds)
	++iteration;
    }
  }

  template <class Tnet, class Tdata, class Tlabel>
  void supervised_trainer<Tnet, Tdata, Tlabel>::
  pretty(labeled_datasource<Tnet, Tdata, Tlabel> &ds) {
    if (!prettied) {
      // pretty sizes of input/output for each module the first time
      cout << "machine sizes: ";
      idxdim d(ds.sample_dims());
      machine.fmod.pretty(d);
      cout << "trainable parameters: " << param.x << endl;
      prettied = true;
    }
  }

//   supervised_trainer<Tnet, Tdata, Tlabel>* copy() {
//     fc_ebm2_gen<fstate_idx,int,fstate_idx>	&machine;
//     parameter				&param;
//   }

} // end namespace ebl
