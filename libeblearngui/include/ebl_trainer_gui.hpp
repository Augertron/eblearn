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
  // supervised_trainer_gui

  template <class Tdata, class Tlabel>  
  supervised_trainer_gui<Tdata, Tlabel>::
  supervised_trainer_gui(supervised_trainer<Tdata, Tlabel> &st_)
    : datasource_wid(-1), internals_wid(-1), st(st_) {
  }

  template <class Tdata, class Tlabel>  
  supervised_trainer_gui<Tdata, Tlabel>::~supervised_trainer_gui() {
  }

  template <class Tdata, class Tlabel>  
  void supervised_trainer_gui<Tdata, Tlabel>::
  display_datasource(labeled_datasource<Tdata, Tlabel> &ds, infer_param &infp,
		  unsigned int display_nh, unsigned int display_nw, 
		  unsigned int display_h0, 
		  unsigned int display_w0, double display_zoom, 
		  int wid, const char *title) {
    datasource_wid = (wid >= 0) ? wid : 
      ((datasource_wid >= 0) ? datasource_wid :
       gui.new_window((title ? title : "Supervised Trainer")));
    gui.select_window(datasource_wid);
    gui.disable_updates();
    gui.clear();
    //    log.clear();
    ds.seek_begin();
    st.resize_input(ds);
    bool correct;
    int answer;
    cout << ds.name << ": iter# " << st.iteration << " ";
    unsigned int h = display_h0 + 35, w = display_w0, nh = 0, w01 = display_w0;
    unsigned int h2 = h, w2 = display_w0, w02 = display_w0, i2 = 0;
    ds.fprop(*st.input, st.label);
    idx<double> m = st.input->x.select(0, 0);
    
    //    else cout << ds.name << ": iter# " << iteration << " ";
    for (unsigned int i = 0; i < ds.size(); ++i) {
      ds.fprop(*st.input, st.label);
      correct = st.test_sample(*st.input, st.label.get(), answer, infp);
      //      log.update(age, correct, energy);
      ds.next();
      idx<double> m = st.input->x.select(0, 0);
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
    gui << at(0, 200) << cout_and_gui();
    //    log.display();
    cout << endl;
    gui.enable_updates();
  }

  template <class Tdata, class Tlabel>  
  void supervised_trainer_gui<Tdata, Tlabel>::
  display_internals(labeled_datasource<Tdata, Tlabel> &ds, infer_param &infp,
		  unsigned int ninternals, 
		  unsigned int display_h0, 
		  unsigned int display_w0, double display_zoom, 
		  int wid, const char *title) {
    internals_wid = (wid >= 0) ? wid : 
      ((internals_wid >= 0) ? internals_wid :
       gui.new_window((title ? 
		       title : "Supervised Trainer: internal fprop states")));
    gui.select_window(internals_wid);
    gui.disable_updates();
    gui.clear();
    //    log.clear();
    ds.seek_begin();
    st.resize_input(ds);
    bool correct;
    int answer;
    cout << ds.name << ": iter# " << st.iteration << " ";
    unsigned int wfdisp = 0, hfdisp = 0;
    ds.fprop(*st.input, st.label);
    idx<double> m = st.input->x.select(0, 0);
    
    for (unsigned int i = 0; (i < ds.size()) && (i < ninternals); ++i) {
      ds.fprop(*st.input, st.label);
      correct = st.test_sample(*st.input, st.label.get(), answer, infp);
      //      log.update(age, correct, energy);
      ds.next();
      st.machine.display_fprop(*st.input, answer, st.energy, hfdisp, wfdisp, 3.0);
      hfdisp += 10;
    }
    gui.enable_updates();
  }  

} // end namespace ebl
