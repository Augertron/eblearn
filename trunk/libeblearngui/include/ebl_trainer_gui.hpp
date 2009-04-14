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
  void supervised_trainer_gui::
  display_datasource(supervised_trainer<Tdata, Tlabel> &st,
		     labeled_datasource<Tdata, Tlabel> &ds, infer_param &infp,
		     unsigned int nh, unsigned int nw, unsigned int h0, 
		     unsigned int w0, double zoom, int wid, const char *title) {
    // if no window given, create a new one or reuse previous one
    datasource_wid = (wid >= 0) ? wid : 
      ((datasource_wid >= 0) ? datasource_wid :
       gui.new_window((title ? title : "Supervised Trainer")));
    gui.select_window(datasource_wid);
    gui.disable_updates();
    gui.clear();
    // init datasource
    ds.seek_begin();
    st.resize_input(ds);
    // find out sample size
    ds.fprop(*st.input, st.label);
    idx<double> m = st.input->x.select(0, 0);
    // top left coordinates of datasets display 1 and 2
    unsigned int w01 = nh * (m.dim(0) + 2) + 5, h01 = h0 + 35;
    unsigned int w02 = (nh * (m.dim(0) + 2) + 5) * 2, h02 = h0 + 35;
    // working variables
    unsigned int h1 = h01, w1 = w01, nh1 = 0;
    unsigned int h2 = h02, w2 = w02, i2 = 0;
    bool correct;
    int answer;

    gui << set_colors(255, 0, 0, 255, 255, 255, 255, 127) << gui_only();
    gui << at(h0, w0) << ds.name;
    gui << black_on_white();
    gui << at(h0 + 17, w0) << "Groundtruth";
    gui << at(h0 + 17, w01) << "Correct & incorrect answers";
    gui << at(h0 + 17, w02) << "Incorrect only";
    gui << white_on_transparent();
    // 0. display dataset with groundtruth labels
    dsgui.display(ds, nh, nw, h0 + 35, w0, zoom, datasource_wid);

    // loop on nh * nw first samples
    for (unsigned int i = 0; (i < ds.size()) && (i2 < nh * nw); ++i) {
      // test sample
      ds.fprop(*st.input, st.label);
      correct = st.test_sample(*st.input, st.label.get(), answer, infp);
      ds.next();
      idx<double> m = st.input->x.select(0, 0);

      // 1. display dataset with incorrect and correct answers
      if (nh1 < nh) {
	gui.draw_matrix_frame(m, (correct?0:128), 0, 0, h1, w1, zoom, zoom);
	if ((ds.lblstr) && (ds.lblstr->at(answer)))
	  gui << at(h1 + 2, w1 + 2) << (ds.lblstr->at(answer))->c_str();
	w1 += m.dim(1) + 2;
	if (((i + 1) % nw == 0) && (i > 1)) {  
	  w1 = w01;
	  h1 += m.dim(0) + 2;
	  nh1++;
	}
      }
      // 2. display first nh * nw incorrect answers
      if (!correct) {
	gui.draw_matrix_frame(m, (correct?0:128), 0, 0, h2, w2, zoom, zoom);
	if ((ds.lblstr) && (ds.lblstr->at(answer)))
	  gui << at(h2 + 2, w2 + 2) << (ds.lblstr->at(answer))->c_str();
	w2 += m.dim(1) + 2;
	if (((i2 + 1) % nw == 0) && (i2 > 1)) {  
	  w2 = w02;
	  h2 += m.dim(0) + 2;
	}
	i2++;
      }
    }
    gui.enable_updates();
  }

  template <class Tdata, class Tlabel>  
  void supervised_trainer_gui::
  display_internals(supervised_trainer<Tdata, Tlabel> &st,
		    labeled_datasource<Tdata, Tlabel> &ds, infer_param &infp,
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
    ds.seek_begin();
    st.resize_input(ds);
    bool correct;
    int answer;
    unsigned int wfdisp = 0, hfdisp = 0;
    ds.fprop(*st.input, st.label);
    idx<double> m = st.input->x.select(0, 0);
    
    // display first ninternals samples
    fc_ebm2_gui mg;
    for (unsigned int i = 0; (i < ds.size()) && (i < ninternals); ++i) {
      ds.fprop(*st.input, st.label);
      correct = st.test_sample(*st.input, st.label.get(), answer, infp);
      //      log.update(age, correct, energy);
      ds.next();
      mg.display_fprop(st.machine, *st.input, answer, st.energy, 
		       hfdisp, wfdisp, 3.0, true, internals_wid);
      hfdisp += 10;
    }
    gui.enable_updates();
  }  

} // end namespace ebl
