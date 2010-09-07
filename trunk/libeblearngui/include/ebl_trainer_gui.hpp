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

  template <class Tnet, class Tdata, class Tlabel>  
  supervised_trainer_gui<Tnet, Tdata, Tlabel>::
  supervised_trainer_gui(bool scroll_)
    : _st(NULL), _ds(NULL), _last_ds(NULL), 
      datasource_wid(-1), internals_wid(-1), internals_wid2(-1),
      internals_wid3(-1),
      scroll(scroll_), scroll_added(false), pos(0), dsgui(NULL) {
  }

  template <class Tnet, class Tdata, class Tlabel>  
  supervised_trainer_gui<Tnet, Tdata, Tlabel>::~supervised_trainer_gui() {
    if (win)
      win->replace_scroll_box_with_copy(this);
  }
  
  template <class Tnet, class Tdata, class Tlabel>  
  void supervised_trainer_gui<Tnet, Tdata, Tlabel>::
  display_datasource(supervised_trainer<Tnet, Tdata, Tlabel> &st,
		     labeled_datasource<Tnet, Tdata, Tlabel> &ds,
		     infer_param &infp,
		     unsigned int nh, unsigned int nw, unsigned int h0, 
		     unsigned int w0, double zoom, int wid, const char *title,
		     bool scrolling) {
    // copy parameters
    _st = &st;
    _infp = &infp;
    _nh = nh;
    _nw = nw;
    _h0 = h0;
    _w0 = w0;
    if (!dsgui)
      dsgui = new labeled_datasource_gui<Tnet, Tdata, Tlabel>(false);
    // do a deep copy of dataset only when necessary
    if (scroll && !scrolling && (_last_ds != &ds)) {
      if (_ds)
	delete _ds;
      _ds = new labeled_datasource<Tnet, Tdata, Tlabel>(ds);
      dsgui->_ds = _ds;
    }
    _last_ds = &ds;
    // init datasource
    st.init(ds);
    // find out sample size
    ds.fprop(*st.input, st.label);
    idx<Tnet> m = st.input->x.select(0, 0);
    _h1 = h0 + nh * (m.dim(0) + 1) * 3;
    _w1 = w0 + nw * (m.dim(1) + 1) * 3;
    _zoom = zoom;
    // if no window given, create a new one or reuse previous one
    datasource_wid = (wid >= 0) ? wid : 
      ((datasource_wid >= 0) ? datasource_wid :
       new_window((title ? title : "Supervised Trainer")));
    select_window(datasource_wid);
    if (scroll && !scroll_added) {
      gui.add_scroll_box((scroll_box*) this);
      scroll_added = true;
    }
    disable_window_updates();
    if (wid == -1) // clear only if we created the window
      clear_window();
    // top left coordinates of datasets display 1 and 2
    unsigned int w01 = nh * (m.dim(1) + 2) + 5, h01 = h0 + 35;
    unsigned int w02 = (nh * (m.dim(1) + 2) + 5) * 2, h02 = h0 + 35;
    // working variables
    unsigned int h1 = h01, w1 = w01, nh1 = 0;
    unsigned int h2 = h02, w2 = w02, i2 = 0;
    bool correct;

    // display top
    gui << set_colors(255, 0, 0, 255, 255, 255, 255, 127) << gui_only();
    gui << at(h0, w0) << ds.name();
    gui << black_on_white();
    gui << at(h0 + 17, w0) << "Groundtruth";
    gui << at(h0 + 17, w01) << "Correct & incorrect answers";
    gui << at(h0 + 17, w02) << "Incorrect only";
    gui << white_on_transparent();

    // 0. display dataset with groundtruth labels
    dsgui->display(ds, nh, nw, h0 + 35, w0, zoom, datasource_wid, NULL, true);

    // loop on nh * nw first samples
    gui << white_on_transparent();
    // loop to reach pos
    ds.seek_begin();
    for (unsigned int p = 0; p < pos; ++p)
      ds.next(); // FIXME add a seek(p) method to ds
    for (unsigned int i = 0; (i < ds.size()) && (i2 < nh * nw); ++i) {
      // test sample
      ds.fprop(*st.input, st.label);
      correct = st.test_sample(*st.input, st.label, infp);
      ds.next();
      idx<Tnet> m = st.input->x.select(0, 0);

      // 1. display dataset with incorrect and correct answers
      if (nh1 < nh) {
	draw_matrix_frame(m, (correct?0:128), 0, 0, h1, w1, zoom, zoom);
	if ((ds.lblstr) && (ds.lblstr->at(st.answer.x.get())))
	  gui << at(h1 + 2, w1 + 2)
	      << (ds.lblstr->at(st.answer.x.get()))->c_str();
	w1 += m.dim(1) + 2;
	if (((i + 1) % nw == 0) && (i > 1)) {  
	  w1 = w01;
	  h1 += m.dim(0) + 2;
	  nh1++;
	}
      }
      // 2. display first nh * nw incorrect answers
      if (!correct) {
	draw_matrix_frame(m, (correct?0:128), 0, 0, h2, w2, zoom, zoom);
	if ((ds.lblstr) && (ds.lblstr->at(st.answer.x.get())))
	  gui << at(h2 + 2, w2 + 2)
	      << (ds.lblstr->at(st.answer.x.get()))->c_str();
	w2 += m.dim(1) + 2;
	if (((i2 + 1) % nw == 0) && (i2 > 1)) {  
	  w2 = w02;
	  h2 += m.dim(0) + 2;
	}
	i2++;
      }
    }
    enable_window_updates();
  }

  template <class Tnet, class Tdata, class Tlabel>  
  void supervised_trainer_gui<Tnet, Tdata, Tlabel>::
  display_internals(supervised_trainer<Tnet, Tdata, Tlabel> &st,
		    labeled_datasource<Tnet, Tdata, Tlabel> &ds,
		    infer_param &infp, gd_param &args,
		    unsigned int ninternals, unsigned int display_h0, 
		    unsigned int display_w0, double display_zoom, 
		    int wid, const char *title) {
    internals_wid = (wid >= 0) ? wid : 
      ((internals_wid >= 0) ? internals_wid :
       new_window((title ? 
		       title : "Supervised Trainer: internal fprop states")));
    internals_wid2 = (wid >= 0) ? wid : 
      ((internals_wid2 >= 0) ? internals_wid2 :
       new_window((title ? 
		       title : "Supervised Trainer: internal bprop states")));
    internals_wid3 = (wid >= 0) ? wid : 
      ((internals_wid3 >= 0) ? internals_wid3 :
       new_window((title ? 
		       title : "Supervised Trainer: internal bbprop states")));
    // freeze and clear display updates
    select_window(internals_wid);
    disable_window_updates();
    clear_window();
    select_window(internals_wid2);
    disable_window_updates();
    clear_window();
    select_window(internals_wid3);
    disable_window_updates();
    clear_window();
    // prepare dataset
    st.init(ds);
    unsigned int wfdisp = 0, hfdisp = 0;
    unsigned int wfdisp2 = 0, hfdisp2 = 0;
    unsigned int wfdisp3 = 0, hfdisp3 = 0;
    ds.fprop(*st.input, st.label);
    idx<Tnet> m = st.input->x.select(0, 0);
    
    // display first ninternals samples
    fc_ebm2_gui mg;
    for (unsigned int i = 0; (i < ds.size()) && (i < ninternals); ++i) {
      // prepare input
      ds.fprop(*st.input, st.label);
      // fprop and bprop
      //st.test_sample(*st.input, st.label, infp);
      // TODO: display is influencing learning, remove influence
      //      st.learn_sample(*st.input, st.label.get(), args);
      st.machine.fprop(*st.input, st.label, st.energy);
      st.param.clear_dx();
      st.machine.bprop(*st.input, st.label, st.energy);
      st.param.clear_ddx();
      st.machine.bbprop(*st.input, st.label, st.energy);
      
      ds.next();
      // display fprop
      mg.display_fprop(st.machine, *st.input, st.answer, st.energy, 
		       hfdisp, wfdisp, 3.0, (Tnet) -1.0, (Tnet) 1.0, true,
		       internals_wid);
      // display bprop
      select_window(internals_wid2);
      mg.display_bprop(st.machine, *st.input, st.answer, st.energy, 
      		       hfdisp2, wfdisp2, 3.0, (Tnet) 1.0, (Tnet) 1.0, true,
		       internals_wid2);
      // display bprop
      select_window(internals_wid3);
      mg.display_bbprop(st.machine, *st.input, st.answer, st.energy, 
			hfdisp3, wfdisp3, 3.0, (Tnet) .01, (Tnet) .01, true,
			internals_wid3);
      hfdisp += 10;
      hfdisp2 += 10;
      hfdisp3 += 10;
    }
    // unfreeze display updates
    select_window(internals_wid);
    enable_window_updates();
    select_window(internals_wid2);
    enable_window_updates();
    select_window(internals_wid3);
    enable_window_updates();
  }  

  ////////////////////////////////////////////////////////////////
  // inherited methods to implement for scrolling capabilities

  template<class Tnet, class Tdata, class Tlabel>
  void supervised_trainer_gui<Tnet, Tdata, Tlabel>::display_next() {
    if (next_page()) {
      pos = MIN(_ds->size(), pos + _nh * _nw);
      display_datasource(*_st, *_ds, *_infp, _nh, _nw, _h0, _w0, _zoom,
			 -1, NULL, true);
    }
  }

  template<class Tnet, class Tdata, class Tlabel>
  void supervised_trainer_gui<Tnet, Tdata, Tlabel>::display_previous() {
    if (previous_page()) {
      pos = std::max((uint) 0, pos - _nh * _nw);
      display_datasource(*_st, *_ds, *_infp, _nh, _nw, _h0, _w0, _zoom,
			 -1, NULL, true);
    }
  }

  template<class Tnet, class Tdata, class Tlabel>
  unsigned int supervised_trainer_gui<Tnet, Tdata, Tlabel>::max_pages() {
    return dsgui->max_pages();
  }

  template<class Tnet, class Tdata, class Tlabel>
  supervised_trainer_gui<Tnet, Tdata, Tlabel>* 
  supervised_trainer_gui<Tnet, Tdata, Tlabel>::copy() {
    //  scroll_box0* supervised_trainer_gui<Tnet, Tdata, Tlabel>::copy() {
    cout << "supervsed_trainer_gui::copy."<<endl;
    supervised_trainer_gui<Tnet, Tdata, Tlabel> *stcopy = 
      new supervised_trainer_gui<Tnet, Tdata, Tlabel>(*this);
    stcopy->dsgui = dsgui->copy();
    stcopy->_ds = _ds;
    stcopy->_last_ds = _last_ds;
    stcopy->_st = _st;
    return stcopy;
  }

} // end namespace ebl
