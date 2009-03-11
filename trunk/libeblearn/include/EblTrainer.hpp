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
    : display(false), display_nh(0), display_nw(0), display_h0(0), 
      display_w0(0), display_zoom(1.0), machine(m), param(p), energy(), 
      label(), age(0), cout(cout_) {
    input = NULL; // allocated when input is passed, based in its order/dims
    energy.dx.set(1.0);
    energy.ddx.set(0.0);
  }

  template <class Tdata, class Tlabel>  
  supervised_trainer<Tdata, Tlabel>::~supervised_trainer() {
    if (input) delete input;
  }

  template <class Tdata, class Tlabel>  
  void supervised_trainer<Tdata, Tlabel>::set_display(unsigned int nh,
						      unsigned int nw,
						      unsigned int h0, 
						      unsigned int w0,
						      double zoom) {
    display = true;
    display_nh = nh;
    display_nw = nw;
    display_h0 = h0;
    display_w0 = w0;
    display_zoom = zoom;
  }
  
  template <class Tdata, class Tlabel>  
  int supervised_trainer<Tdata, Tlabel>::run(state_idx &input, 
					     infer_param &infp) {
    int answer = -1;
    machine.infer2(input, answer, energy, infp); // infer answer from energies
    return answer;
  }
  
  template <class Tdata, class Tlabel>  
  bool supervised_trainer<Tdata, Tlabel>::
  test_sample(state_idx &input, int label, int &answer, infer_param &infp) {
    answer = run(input, infp);
    return (label == answer); // return true if correct answer
  }

  template <class Tdata, class Tlabel>  
  Idx<double> supervised_trainer<Tdata, Tlabel>::
  learn_sample(state_idx &input, int label, gd_param &args) {
    machine.fprop(input, label, energy);
    param.clear_dx();
    machine.bprop(input, label, energy);
    param.update(args);
    return energy.x.get();
  }

  template <class Tdata, class Tlabel>  
  void supervised_trainer<Tdata, Tlabel>::
  test(LabeledDataSource<Tdata, Tlabel> &ds, classifier_meter &log,
       infer_param &infp) {
#ifdef __GUI__
    gui.new_window("Supervised Trainer: Classification Results");
    unsigned int h = display_h0, w = display_w0, nh = 0;
#endif
    ds.seek_begin();
    log.clear();
    resize_input(ds);
    bool correct;
    int answer;
    for (int i = 0; i < ds.size(); ++i) {
      ds.fprop(*input, label);
      correct = test_sample(*input, label.get(), answer, infp);
      log.update(age, correct, energy);
      ds.next();
#ifdef __GUI__
      if ((display) && (nh < display_nh)) {
	Idx<double> m = input->x.select(0, 0);
	gui.draw_matrix_frame(m, (correct?0:128), 0, 0, h, w, 0.0, 0.0, 
			      display_zoom, display_zoom);
	if ((ds.lblstr) && (ds.lblstr->at(answer)))
	  gui << at(h + 1, w + 1) << (ds.lblstr->at(answer))->c_str();
	w += m.dim(1) + 1;
	if (((i + 1) % display_nw == 0) && (i > 1)) {  
	  w = display_w0;
	  h += m.dim(0) + 1;
	  nh++;
	}
      }
#endif
    }
  }
  
  template <class Tdata, class Tlabel>  
  void supervised_trainer<Tdata, Tlabel>::
  train(LabeledDataSource<Tdata, Tlabel> &ds, classifier_meter &log, 
	gd_param &args, int niter) {
    ds.seek_begin();
    log.clear();
    resize_input(ds);
    for (int i = 0; i < niter; ++i) { // niter iterations
      for (int j = 0; j < ds.size(); ++j) { // training on entire training set
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
  compute_diaghessian(LabeledDataSource<Tdata, Tlabel> &ds, intg niter, 
		      double mu) {
    IdxDim d(ds.data.spec);
    if (!input) input = new state_idx(d);
    else input->resize(d);
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
  resize_input(LabeledDataSource<Tdata, Tlabel> &ds) {
    IdxDim d(ds.data.spec);
    if (!input) input = new state_idx(d);
    else input->resize(d);
  }

} // end namespace ebl
