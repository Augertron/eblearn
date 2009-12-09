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

#ifndef ELB_ARCH_GUI_HPP_
#define ELB_ARCH_GUI_HPP_

#include <ostream>
#include <typeinfo>

#include "ebl_layers_gui.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // module_1_1_gui

  template<class Tin, class Tout>
  void module_1_1_gui::display_fprop2(module_1_1<Tin, Tout> &m, 
				      Tin &in, Tout &out,
				      unsigned int &h0, unsigned int &w0,
				      double zoom, double vmin, double vmax,
				      bool show_out, int wid,
				     const char *wname) {
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "module_1_1 fprop display"));
    select_window(display_wid_fprop);
    gui << black_on_white(255, 0) << gui_only();

    if (dynamic_cast< layers_n<Tin>* >(&m)) {    
      // layers_n
      layers_n_gui::display_fprop(*this, dynamic_cast< layers_n<Tin>& >(m), 
				  in, out, h0, w0, zoom, vmin, vmax, show_out);
    } else if (dynamic_cast< nn_layer_full* >(&m)) {
      // nn_layer_full
      nn_layer_full_gui::display_fprop(dynamic_cast< nn_layer_full& >(m), in,
				       out, h0, w0, zoom, vmin, vmax,show_out);
    } else if (dynamic_cast< nn_layer_convolution* >(&m)) {
      // nn_layer_convolution
      nn_layer_convolution_gui::
	display_fprop(dynamic_cast< nn_layer_convolution& >(m),
		      in, out, h0, w0, zoom, vmin, vmax, show_out);
    } else if (dynamic_cast< layer_convabsnorm* >(&m)) {
      // layer_convabsnorm
      layer_convabsnorm_gui::
	display_fprop(dynamic_cast< layer_convabsnorm& >(m),
		      in, out, h0, w0, zoom, vmin, vmax, show_out);
    } else if (dynamic_cast< nn_layer_subsampling* >(&m)) {
      // nn_layer_subsampling
      nn_layer_subsampling_gui::
	display_fprop(dynamic_cast< nn_layer_subsampling& >(m),
		      in, out, h0, w0, zoom, vmin, vmax, show_out);
    } else {
      cerr << "Warning: unknown display function for module_1_1 object";
      cerr << "(" << typeid(m).name() << ")." << endl;
    }
  }

  template<class Tin, class Tout>
  void module_1_1_gui::display_fprop(module_1_1<Tin, Tout> &m, 
				     Tin &in, Tout &out,
				     unsigned int h0, unsigned int w0,
				     double zoom, double vmin, double vmax,
				     bool show_out, int wid,
				     const char *wname) {
    display_fprop2(m, in, out, h0, w0, zoom, vmin, vmax, show_out, wid, wname);
  }

  template<class Tin, class Tout>
  void module_1_1_gui::display_bprop2(module_1_1<Tin, Tout> &m, 
				      Tin &in, Tout &out,
				      unsigned int &h0, unsigned int &w0,
				      double zoom, double vmin, double vmax,
				      bool show_in, int wid,
				      const char *wname) {
    display_wid_bprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "module_1_1 bprop display"));
    select_window(display_wid_bprop);
    gui << black_on_white(255, 0) << gui_only();

    if (dynamic_cast< layers_n<Tin>* >(&m)) {    
      // layers_n
      layers_n_gui::display_bprop(*this, dynamic_cast< layers_n<Tin>& >(m), 
				  in, out, h0, w0, zoom, vmin, vmax, show_in);
    } else if (dynamic_cast< nn_layer_full* >(&m)) {
      // nn_layer_full
      nn_layer_full_gui::display_bprop(dynamic_cast< nn_layer_full& >(m), in,
				       out, h0, w0, zoom, vmin, vmax,show_in);
    } else if (dynamic_cast< nn_layer_convolution* >(&m)) {
      // nn_layer_convolution
      nn_layer_convolution_gui::
	display_bprop(dynamic_cast< nn_layer_convolution& >(m),
		      in, out, h0, w0, zoom, vmin, vmax, show_in);
    } else if (dynamic_cast< layer_convabsnorm* >(&m)) {
      // layer_convabsnorm
      layer_convabsnorm_gui::
	display_bprop(dynamic_cast< layer_convabsnorm& >(m),
		      in, out, h0, w0, zoom, vmin, vmax, show_in);
    } else if (dynamic_cast< nn_layer_subsampling* >(&m)) {
      // nn_layer_subsampling
      nn_layer_subsampling_gui::
	display_bprop(dynamic_cast< nn_layer_subsampling& >(m),
		      in, out, h0, w0, zoom, vmin, vmax, show_in);
    } else {
      cerr << "Warning: unknown display function for module_1_1 object";
      cerr << "(" << typeid(m).name() << ")." << endl;
    }
  }

  template<class Tin, class Tout>
  void module_1_1_gui::display_bprop(module_1_1<Tin, Tout> &m, 
				     Tin &in, Tout &out,
				     unsigned int h0, unsigned int w0,
				     double zoom, double vmin, double vmax,
				     bool show_in, int wid,
				     const char *wname) {
    display_bprop2(m, in, out, h0, w0, zoom, vmin, vmax, show_in, wid, wname);
  }

  ////////////////////////////////////////////////////////////////
  // module_2_1_gui

  template<class Tin1, class Tin2, class Tout>
  void module_2_1_gui::display_fprop(module_2_1<Tin1, Tin2, Tout> &m, 
				     Tin1 &in1, Tin2 &in2, Tout &out,
				     unsigned int &h0, unsigned int &w0,
				     double zoom, double vmin, double vmax,
				     bool show_out, int wid,
				     const char *wname) {
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "module_2_1 fprop display"));
    select_window(display_wid_fprop);
    disable_window_updates();
    gui << black_on_white(255, 0) << gui_only();

    if (dynamic_cast< fc_ebm2<Tin1, Tin2, Tout>* >(&m)) {    
      // fc_ebm2
      fc_ebm2_gui::
	display_fprop(*this, dynamic_cast< fc_ebm2<Tin1, Tin2, Tout>& >(m), 
		      in1, in2, out, h0, w0, zoom, vmin, vmax, show_out, wid);
    } else {
      cerr << "Warning: unknown display function for module_2_1 object";
      cerr << "(" << typeid(m).name() << ")." << endl;
    }
    enable_window_updates();
  }

  template<class Tin1, class Tin2, class Tout>
  void module_2_1_gui::display_bprop(module_2_1<Tin1, Tin2, Tout> &m, 
				     Tin1 &in1, Tin2 &in2, Tout &out,
				     unsigned int &h0, unsigned int &w0,
				     double zoom, double vmin, double vmax,
				     bool show_out, int wid,
				     const char *wname) {
    display_wid_fprop = (wid >= 0) ? wid : 
      new_window((wname ? wname : "module_2_1 bprop display"));
    select_window(display_wid_fprop);
    disable_window_updates();
    gui << black_on_white(255, 0) << gui_only();

    if (dynamic_cast< fc_ebm2<Tin1, Tin2, Tout>* >(&m)) {    
      // fc_ebm2
      fc_ebm2_gui::
	display_bprop(*this, dynamic_cast< fc_ebm2<Tin1, Tin2, Tout>& >(m), 
		      in1, in2, out, h0, w0, zoom, vmin, vmax, show_out, wid);
    } else {
      cerr << "Warning: unknown display function for module_2_1 object";
      cerr << "(" << typeid(m).name() << ")." << endl;
    }
    enable_window_updates();
  }

  ////////////////////////////////////////////////////////////////
  // fc_ebm2_gui

  template<class Tin1, class Tin2, class Tout>
  void fc_ebm2_gui::display_fprop(fc_ebm2<Tin1, Tin2, Tout> &fc,
				  Tin1 &i1, Tin2 &i2, 
				  state_idx &energy, 
				  unsigned int &h0, unsigned int &w0,
				  double zoom, double vmin, double vmax,
				  bool show_out,
				  int wid, const char *wname) {
    module_1_1_gui m;
    m.display_fprop2(fc.fmod, i1, fc.fout, h0, w0, zoom, vmin, vmax,
		     show_out, wid, wname);
    // TODO add energy, answer display
  }

  template<class Tin1, class Tin2, class Tout>
  void fc_ebm2_gui::display_bprop(fc_ebm2<Tin1, Tin2, Tout> &fc,
				  Tin1 &i1, Tin2 &i2, 
				  state_idx &energy, 
				  unsigned int &h0, unsigned int &w0,
				  double zoom, double vmin, double vmax,
				  bool show_out,
				  int wid, const char *wname) {
    module_1_1_gui m;
    m.display_bprop2(fc.fmod, i1, fc.fout, h0, w0, zoom, vmin, vmax,
		     show_out, wid, wname);
    // TODO add energy, answer display
  }

  ////////////////////////////////////////////////////////////////
  // layers_n_gui

  template<class T>
  void layers_n_gui::display_fprop(module_1_1_gui &g, 
				   layers_n<T> &ln, T &in, T &out,
				   unsigned int &h0, unsigned int &w0, 
				   double zoom, double vmin, double vmax,
				   bool show_out) {
    if (ln.modules->empty())
      return ;
    T* hi = &in;
    T* ho = &in;
    // last will be manual
    int niter = ln.modules->size()-1;
    for(int i=0; i<niter; i++){
      ho = (*ln.hiddens)[i];
      g.display_fprop2(*(*ln.modules)[i], *hi, *ho, 
		      h0, w0, zoom, vmin, vmax, false, g.display_wid_fprop);
      hi = ho;
    }
    g.display_fprop2(*(*ln.modules)[niter], *ho, out, 
		     h0, w0, zoom, vmin, vmax, false, g.display_wid_fprop);
    if (show_out) { 
      unsigned int h = h0, w = w0, zoomf = 3;
      // display outputs text
      gui << gui_only() << at(h, w) << "outputs:" << out.x;
      w += 150;
      // display outputs
      idx_bloop1(m, out.x, double) {
	draw_matrix(m, h, w, zoom * zoomf, zoom * zoomf, vmin, vmax);
	w += m.dim(1) * zoom * zoomf + 1;
      }
      h0 += m.dim(0) * zoom * zoomf + 1;
    }
  }

  template<class T>
  void layers_n_gui::display_bprop(module_1_1_gui &g, 
				   layers_n<T> &ln, T &in, T &out,
				   unsigned int &h0, unsigned int &w0, 
				   double zoom, double vmin, double vmax,
				   bool show_out) {
    if (ln.modules->empty())
      return ;
    T* hi = &in;
    T* ho = &in;
    // last will be manual
    int niter = ln.modules->size()-1;
    for(int i=0; i<niter; i++){
      ho = (*ln.hiddens)[i];
      g.display_bprop2(*(*ln.modules)[i], *hi, *ho, 
		       h0, w0, zoom, vmin, vmax, false, g.display_wid_bprop);
      hi = ho;
    }
    g.display_bprop2(*(*ln.modules)[niter], *ho, out, 
		    h0, w0, zoom, vmin, vmax, false, g.display_wid_bprop);
    if (show_out) { 
      unsigned int h = h0, w = w0, zoomf = 3;
      // display outputs text
      gui << gui_only() << at(h, w) << "outputs:" << out.dx;
      w += 150;
      // display outputs
      idx_bloop1(m, out.dx, double) {
	draw_matrix(m, h, w, zoom * zoomf, zoom * zoomf, vmin, vmax);
	w += m.dim(1) * zoom * zoomf + 1;
      }
      h0 += m.dim(0) * zoom * zoomf + 1;
    }
  }

} // end namespace ebl

#endif /* ELB_ARCH_GUI_HPP_ */
