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
    : display_nh(0), display_nw(0), display_h0(0), 
      display_w0(0), display_zoom(1.0), iteration(-1), iteration_ptr(NULL),
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
  void supervised_trainer<Tdata, Tlabel>::set_display(unsigned int nh,
						      unsigned int nw,
						      unsigned int h0, 
						      unsigned int w0,
						      double zoom, int wid,
						      const char *title) {
#ifdef __GUI__
    display_nh = nh;
    display_nw = nw;
    display_h0 = h0;
    display_w0 = w0;
    display_zoom = zoom;
    display_wid = (wid >= 0) ? wid : 
      gui.new_window((title ? title : "Supervised Trainer"));
    fpropdisplay_wid = gui.new_window("Supervised Trainer: fprop");
#endif
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
       infer_param &infp, bool display) {
    ds.seek_begin();
    log.clear();
    resize_input(ds);
    bool correct;
    int answer;
    if (!iteration_ptr) 
      iteration_ptr = (void *) &ds;
    if (iteration_ptr == (void *) &ds)
      ++iteration;
#ifdef __GUI__
    unsigned int h = display_h0 + 35, w = display_w0, nh = 0, w01 = display_w0;
    unsigned int h2 = h, w2 = display_w0, w02 = display_w0, i2 = 0;
    int nfdisp = 4;
    unsigned int wfdisp = 0, hfdisp = 0;
    if (display) {
      ds.fprop(*input, label);
      Idx<double> m = input->x.select(0, 0);

      // fprop display
      gui.select_window(fpropdisplay_wid);
      gui.clear();

      // datasets displays
      gui.select_window(display_wid);
      gui.clear();
      w01 = display_w0 + display_nw * (m.dim(1) + 1) + 10;
      w02 = display_w0 + (display_nw * (m.dim(1) + 2) + 10) * 2;
      w = w01;
      w2 = w02;
      gui << cout_and_gui() << at(0, 0) << ds.name;
      gui << ": iter# " << iteration << " ";
      gui << gui_only();
      gui << at(display_h0 + 17, display_w0) << "Groundtruth";
      gui << at(display_h0 + 17, w01) << "Correct & incorrect answers";
      gui << at(display_h0 + 17, w02) << "Incorrect only";
      ds.display(display_nh, display_nw, h, display_w0, display_zoom,
		 display_wid);
    } else
      cout << ds.name << ": iter# " << iteration << " ";
#endif
    for (int i = 0; i < ds.size(); ++i) {
      ds.fprop(*input, label);
      correct = test_sample(*input, label.get(), answer, infp);
      log.update(age, correct, energy);
      ds.next();
#ifdef __GUI__
      if (display) {
	// display fprop
	if (i < nfdisp) {
	  gui.select_window(fpropdisplay_wid);
	  machine.fmod.display_fprop(*input, machine.fout, hfdisp, wfdisp, 2.0);
	  wfdisp += 10;
	}

	gui.select_window(display_wid);
	Idx<double> m = input->x.select(0, 0);
	// display all display_nh*display_nw incorrect or correct answers
	if (nh < display_nh) {
	  gui.draw_matrix_frame(m, (correct?0:128), 0, 0, h, w,
				0.0, 0.0, 
				display_zoom, display_zoom);
	  if ((ds.lblstr) && (ds.lblstr->at(answer)))
	    gui << at(h + 2, w + 2) << (ds.lblstr->at(answer))->c_str();
	  w += m.dim(1) + 2;
	  if (((i + 1) % display_nw == 0) && (i > 1)) {  
	    w = w01;
	    h += m.dim(0) + 2;
	    nh++;
	  }
	}
	// display first display_nh*display_nw incorrect answers
	if ((i2 < display_nh * display_nw) && (!correct)) {
	  gui.draw_matrix_frame(m, (correct?0:128), 0, 0, h2, w2, 0.0, 0.0, 
				display_zoom, display_zoom);
	  if ((ds.lblstr) && (ds.lblstr->at(answer)))
	    gui << at(h2 + 2, w2 + 2) << (ds.lblstr->at(answer))->c_str();
	  w2 += m.dim(1) + 2;
	  if (((i2 + 1) % display_nw == 0) && (i2 > 1)) {  
	    w2 = w02;
	    h2 += m.dim(0) + 2;
	  }
	}
	if (!correct)
	  i2++;
      }
#endif
    }
#ifdef __GUI__
    gui << at(0, 200) << cout_and_gui();
#endif
    log.display();
    cout << endl;
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
