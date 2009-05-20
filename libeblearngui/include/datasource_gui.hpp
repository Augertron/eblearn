/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
 *   All rights reserved.
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

#ifndef DATASOURCE_GUI_HPP_
#define DATASOURCE_GUI_HPP_

#include <ostream>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // labeled_datasource_gui

  template<typename Tdata, typename Tlabel>
  labeled_datasource_gui<Tdata, Tlabel>::labeled_datasource_gui(bool scroll_)
    : scroll(scroll_), scroll_added(false), pos(0), display_wid(-1),
      _zoom(1), _rmin(0), _rmax(0), _replaced(false),_last_ds(NULL), _ds(NULL) {
  }

  template<typename Tdata, typename Tlabel>
  labeled_datasource_gui<Tdata, Tlabel>::~labeled_datasource_gui() {
    if (win && !_replaced)
      win->replace_scroll_box_with_copy(this);
  }

  template<typename Tdata, typename Tlabel>
  void labeled_datasource_gui<Tdata, Tlabel>::
  display(labeled_datasource<Tdata, Tlabel> &ds,
	  unsigned int nh, unsigned int nw,
	  unsigned int h0, unsigned int w0,
	  double zoom, int wid, const char *wname, bool scrolling,
	  Tdata rangemin, Tdata rangemax){
    // do a deep copy of dataset only when necessary
    if (scroll && !scrolling && (_last_ds != &ds)) {
      if (_ds)
	delete _ds;
      _ds = new labeled_datasource<Tdata, Tlabel>(ds);
    }
    _last_ds = &ds;
    // copy parameters
    _nh = nh;
    _nw = nw;
    _h0 = h0;
    _w0 = w0;
    _zoom = zoom;
    _rmin = rangemin;
    _rmax = rangemax;
    idxdim d = ds.sample_dims();
    state_idx s(d);
    idx<double> m = s.x.select(0, 0);
    idx<Tlabel> lbl;
    _h1 = h0 + nh * (m.dim(0) + 1);
    _w1 = w0 + nw * (m.dim(1) + 1);
    display_wid = (wid >= 0) ? wid :
      (display_wid >= 0) ? display_wid :
      new_window((wname ? wname : ds.name), 
		 nh * (ds.height + 1) - 1, 
		 nw * (ds.width + 1) - 1);
    select_window(display_wid);
    if (scroll && !scroll_added) {
      gui.add_scroll_box((scroll_box*) this);
      scroll_added = true;
    }
    disable_window_updates();
    if (wid == -1) // clear only if we created the window
      clear_window();
    gui << white_on_transparent() << gui_only();
    ds.seek_begin();
    // loop to reach pos
    for (unsigned int p = 0; p < pos; ++p)
      ds.next(); // FIXME add a seek(p) method to ds
    unsigned int h = h0, w = w0;
    for (unsigned int ih = 0; ih < nh; ++ih) {
      for (unsigned int iw = 0; iw < nw; ++iw) {
	ds.fprop(s, lbl);
	ds.next();
	m = s.x.select(0, 0);
	draw_matrix(m, h, w, _zoom, _zoom, _rmin, _rmax);
	if ((ds.lblstr) && (ds.lblstr->at((int)lbl.get())))
	  gui << at(h + 1, w + 1) << (ds.lblstr->at((int)lbl.get()))->c_str();
	w += m.dim(1) + 1;
      }
      w = w0;
      h += m.dim(0) + 1;
    }
    ds.seek_begin();
    enable_window_updates();
    display_controls();
  }
  
  ////////////////////////////////////////////////////////////////
  // inherited methods to implement for scrolling capabilities

  template<typename Tdata, typename Tlabel>
  void labeled_datasource_gui<Tdata, Tlabel>::display_next() {
    if (next_page()) {
      pos = MIN(_ds->size(), pos + _nh * _nw);
      display(*_ds, _nh, _nw, _h0, _w0, _zoom, -1, NULL, true, _rmin, _rmax);
    }
  }

  template<typename Tdata, typename Tlabel>
  void labeled_datasource_gui<Tdata, Tlabel>::display_previous() {
    if (previous_page()) {
      pos = MAX(0, pos - _nh * _nw);
      display(*_ds, _nh, _nw, _h0, _w0, _zoom, -1, NULL, true, _rmin, _rmax);
    }
  }

  template<typename Tdata, typename Tlabel>
  unsigned int labeled_datasource_gui<Tdata, Tlabel>::max_pages() {
    return (unsigned int) (_ds->size() / (_nh * _nw));
  }
 
  // TODO replace with copy constructor
  template<typename Tdata, typename Tlabel>
  labeled_datasource_gui<Tdata,Tlabel>* 
  labeled_datasource_gui<Tdata, Tlabel>::copy() {
    //  scroll_box0* labeled_datasource_gui<Tdata, Tlabel>::copy() {
    //cout << "labeled_datasource_gui::copy."<<endl;
    labeled_datasource_gui<Tdata, Tlabel> *dscopy =
      new labeled_datasource_gui<Tdata, Tlabel>(*this);
    dscopy->_ds = _ds;
    dscopy->_last_ds = _last_ds;
    return dscopy;
  }

  ////////////////////////////////////////////////////////////////
  // labeled_pair_datasource_gui

  template<typename Tdata, typename Tlabel>
  labeled_pair_datasource_gui<Tdata, Tlabel>::
  labeled_pair_datasource_gui(bool scroll_)
    : labeled_datasource_gui<Tdata, Tlabel>(scroll_),
      _last_ds(NULL), _ds(NULL){
  }

  template<typename Tdata, typename Tlabel>
  labeled_pair_datasource_gui<Tdata, Tlabel>::~labeled_pair_datasource_gui() {
    if (this->win) {
      this->win->replace_scroll_box_with_copy(this);
      this->_replaced = true;
    }
  }

  template<typename Tdata, typename Tlabel>
  void labeled_pair_datasource_gui<Tdata, Tlabel>::
  display(labeled_pair_datasource<Tdata, Tlabel> &ds,
	  unsigned int nh, unsigned int nw,
	  unsigned int h0, unsigned int w0,
	  double zoom, int wid, const char *wname, bool scrolling,
	  Tdata rangemin, Tdata rangemax){
    // do a deep copy of dataset only when necessary
    if (this->scroll && !scrolling && (_last_ds != &ds)) {
      if (_ds)
	delete _ds;
      _ds = new labeled_pair_datasource<Tdata, Tlabel>(ds);
      labeled_datasource_gui<Tdata,Tlabel>::_ds = _ds;
    }
    _last_ds = &ds;
    // copy parameters
    this->_nh = nh;
    this->_nw = nw;
    this->_h0 = h0;
    this->_w0 = w0;
    this->_zoom = zoom;
    this->_rmin = rangemin;
    this->_rmax = rangemax;
    idxdim d = ds.sample_dims();
    state_idx in1(d);
    state_idx in2(d);
    idx<double> m = in1.x.select(0, 0);
    idx<Tlabel> lbl;
    this->_h1 = h0 + nh * (m.dim(0) + 1);
    this->_w1 = w0 + nw * (m.dim(1) + 1);
    this->display_wid = (wid >= 0) ? wid :
      (this->display_wid >= 0) ? this->display_wid :
      new_window((wname ? wname : ds.name), 
		 nh * (ds.height + 1) - 1, 
		 nw * (ds.width + 1) - 1);
    select_window(this->display_wid);
    if (this->scroll && !this->scroll_added) {
      gui.add_scroll_box((scroll_box*) this);
      this->scroll_added = true;
    }
    disable_window_updates();
    if (wid == -1) // clear only if we created the window
      clear_window();
    gui << white_on_transparent() << gui_only();
    ds.seek_begin();
    // loop to reach pos
    for (unsigned int p = 0; p < this->pos; ++p)
      ds.next(); // FIXME add a seek(p) method to ds
    unsigned int h = h0, w = w0;
    for (unsigned int ih = 0; ih < nh; ++ih) {
      for (unsigned int iw = 0; iw < nw; iw += 2) {
	ds.fprop(in1, in2, lbl);
	ds.next();
	m = in1.x.select(0, 0);
	draw_matrix(m, h, w, this->_zoom, this->_zoom,
		    this->_rmin, this->_rmax);
	w += m.dim(1) + 1;
	m = in2.x.select(0, 0);
	draw_matrix(m, h, w, this->_zoom, this->_zoom,
		    this->_rmin, this->_rmax);
	if ((ds.lblstr) && (ds.lblstr->at((int)lbl.get()))) {
	  string *str = ds.lblstr->at((int)lbl.get());
	  
	  gui << at(h + 1, w + 1 - (str->size() / 2) * 10) << str->c_str();
	  gui << " pair";
	}
	w += m.dim(1) + 1;
      }
      w = w0;
      h += m.dim(0) + 1;
    }
    ds.seek_begin();
    enable_window_updates();
    this->display_controls();
  }

  ////////////////////////////////////////////////////////////////
  // inherited methods to implement for scrolling capabilities

  template<typename Tdata, typename Tlabel>
  void labeled_pair_datasource_gui<Tdata, Tlabel>::display_next() {
    if (this->next_page()) {
      this->pos = MIN(_ds->size(), this->pos + this->_nh * this->_nw);
      display(*_ds, this->_nh, this->_nw, this->_h0, this->_w0, this->_zoom,
	      -1, NULL, true, this->_rmin, this->_rmax);
    }
  }

  template<typename Tdata, typename Tlabel>
  void labeled_pair_datasource_gui<Tdata, Tlabel>::display_previous() {
    if (this->previous_page()) {
      this->pos = MAX(0, this->pos - this->_nh * this->_nw);
      display(*_ds, this->_nh, this->_nw, this->_h0, this->_w0, this->_zoom,
	      -1, NULL, true, this->_rmin, this->_rmax);
    }
  }

  template<typename Tdata, typename Tlabel>
  unsigned int labeled_pair_datasource_gui<Tdata, Tlabel>::max_pages() {
    return (unsigned int) (_ds->size() / (this->_nh * (this->_nw / 2)));
  }
 
  // TODO replace with copy constructor
  template<typename Tdata, typename Tlabel>
  labeled_pair_datasource_gui<Tdata,Tlabel>* 
  labeled_pair_datasource_gui<Tdata, Tlabel>::copy() {
    //  scroll_box0* labeled_datasource_gui<Tdata, Tlabel>::copy() {
    cout << "labeled_pair_datasource_gui::copy."<<endl;
    labeled_pair_datasource_gui<Tdata, Tlabel> *dscopy =
      new labeled_pair_datasource_gui<Tdata, Tlabel>(*this);
    ((labeled_datasource_gui<Tdata, Tlabel>*)dscopy)->_ds = this->_ds;
    ((labeled_datasource_gui<Tdata, Tlabel>*)dscopy)->_last_ds = this->_last_ds;
    return dscopy;
  }

} // end namespace ebl

#endif /* DATASOURCE_GUI_HPP_ */
