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

#define DISPLAY_1_1(name1, name2)					\
  template<class Tin, class Tout>					\
  void module_1_1_gui::name1(module_1_1<Tin, Tout> &m,			\
			     Tin &in, Tout &out,			\
			     unsigned int h0, unsigned int w0,		\
			     double zoom, double vmin, double vmax,	\
			     bool show_out, int wid,			\
			     const char *wname) {			\
    name2(m, in, out, h0, w0, zoom, vmin, vmax, show_out, wid, wname);	\
  }

#define DISPLAY2_1_1(name1, name2)					\
  template<class Tin, class Tout>					\
  void module_1_1_gui::name2(module_1_1<Tin, Tout> &m,			\
			     Tin &in, Tout &out,			\
			     unsigned int &h0, unsigned int &w0,	\
			     double zoom, double vmin, double vmax,	\
			     bool show_out, int wid,			\
			     const char *wname) {			\
    display_wid_fprop = (wid >= 0) ? wid :				\
      new_window((wname ? wname : "module_1_1 display"));		\
    select_window(display_wid_fprop);					\
    gui << black_on_white(255, 0) << gui_only();			\
									\
    if (dynamic_cast< layers_n<Tin>* >(&m)) {				\
      /* layers_n */							\
      layers_n_gui::name1(*this, dynamic_cast< layers_n<Tin>& >(m),	\
				  in, out, h0, w0, zoom, vmin, vmax, show_out);\
    } else if (dynamic_cast< nn_layer_full* >(&m)) {			\
      /* nn_layer_full */						\
      nn_layer_full_gui::name1(dynamic_cast< nn_layer_full& >(m), in,	\
				       out, h0, w0, zoom, vmin, vmax,show_out);\
    } else if (dynamic_cast< nn_layer_convolution* >(&m)) {		\
      /* nn_layer_convolution */					\
      nn_layer_convolution_gui::					\
	name1(dynamic_cast< nn_layer_convolution& >(m),			\
	      in, out, h0, w0, zoom, vmin, vmax, show_out);		\
    } else if (dynamic_cast< layer_convabsnorm* >(&m)) {		\
      /* layer_convabsnorm */						\
      layer_convabsnorm_gui::						\
	name1(dynamic_cast< layer_convabsnorm& >(m),			\
		      in, out, h0, w0, zoom, vmin, vmax, show_out);	\
    } else if (dynamic_cast< nn_layer_subsampling* >(&m)) {		\
      /* nn_layer_subsampling */					\
      nn_layer_subsampling_gui::					\
	name1(dynamic_cast< nn_layer_subsampling& >(m),			\
		      in, out, h0, w0, zoom, vmin, vmax, show_out);	\
    } else {								\
      cerr << "Warning: unknown display function for module_1_1 object"; \
      cerr << "(" << typeid(m).name() << ")." << endl;			\
    }									\
  }

  DISPLAY_1_1(display_fprop, display_fprop2)
  DISPLAY2_1_1(display_fprop, display_fprop2)
  DISPLAY_1_1(display_bprop, display_bprop2)
  DISPLAY2_1_1(display_bprop, display_bprop2)
  DISPLAY_1_1(display_bbprop, display_bbprop2)
  DISPLAY2_1_1(display_bbprop, display_bbprop2)

  ////////////////////////////////////////////////////////////////
  // module_2_1_gui

#define DISPLAY_2_1(name1)						\
  template<class Tin1, class Tin2, class Tout>				\
  void module_2_1_gui::name1(module_2_1<Tin1, Tin2, Tout> &m,		\
			     Tin1 &in1, Tin2 &in2, Tout &out,		\
			     unsigned int &h0, unsigned int &w0,	\
			     double zoom, double vmin, double vmax,	\
			     bool show_out, int wid,			\
			     const char *wname) {			\
    display_wid_fprop = (wid >= 0) ? wid :				\
      new_window((wname ? wname : "module_2_1 display"));		\
    select_window(display_wid_fprop);					\
    disable_window_updates();						\
    gui << black_on_white(255, 0) << gui_only();			\
									\
    if (dynamic_cast< fc_ebm2<Tin1, Tin2, Tout>* >(&m)) {		\
      /* fc_ebm2 */							\
      fc_ebm2_gui::							\
	name1(*this, dynamic_cast< fc_ebm2<Tin1, Tin2, Tout>& >(m),	\
	     in1, in2, out, h0, w0, zoom, vmin, vmax, show_out, wid);	\
    } else {								\
      cerr << "Warning: unknown display function for module_2_1 object"; \
      cerr << "(" << typeid(m).name() << ")." << endl;			\
    }									\
    enable_window_updates();						\
  }

  DISPLAY_2_1(display_fprop)
  DISPLAY_2_1(display_bprop)
  DISPLAY_2_1(display_bbprop)
  
  ////////////////////////////////////////////////////////////////
  // fc_ebm2_gui

#define DISPLAY_FCEBM2(name1, name2)					\
  template<class Tin1, class Tin2, class Tout>				\
  void fc_ebm2_gui::name1(fc_ebm2<Tin1, Tin2, Tout> &fc,		\
			  Tin1 &i1, Tin2 &i2,				\
			  state_idx &energy,				\
			  unsigned int &h0, unsigned int &w0,		\
			  double zoom, double vmin, double vmax,	\
			  bool show_out,				\
			  int wid, const char *wname) {			\
    module_1_1_gui m;							\
    m.name2(fc.fmod, i1, fc.fout, h0, w0, zoom, vmin, vmax,		\
	    show_out, wid, wname);					\
    /* TODO add energy, answer display */				\
  }

  DISPLAY_FCEBM2(display_fprop, display_fprop2)
  DISPLAY_FCEBM2(display_bprop, display_bprop2)
  DISPLAY_FCEBM2(display_bbprop, display_bbprop2)

  ////////////////////////////////////////////////////////////////
  // layers_n_gui

#define DISPLAY_LAYERSN(name1, name2, T)				\
  template<class T>							\
  void layers_n_gui::name1(module_1_1_gui &g,				\
			   layers_n<T> &ln, T &in, T &out,		\
			   unsigned int &h0, unsigned int &w0,		\
			   double zoom, double vmin, double vmax,	\
			   bool show_out) {				\
    if (ln.modules->empty())						\
      return ;								\
    T* hi = &in;							\
    T* ho = &in;							\
    /* last will be manual */						\
    int niter = ln.modules->size()-1;					\
    for(int i=0; i<niter; i++){						\
      ho = (*ln.hiddens)[i];						\
      g.name2(*(*ln.modules)[i], *hi, *ho,				\
	      h0, w0, zoom, vmin, vmax, false, g.display_wid_fprop);	\
      hi = ho;								\
    }									\
    g.name2(*(*ln.modules)[niter], *ho, out,				\
	    h0, w0, zoom, vmin, vmax, false, g.display_wid_fprop);	\
    if (show_out) {							\
      unsigned int h = h0, w = w0, zoomf = 3;				\
      /* display outputs text */					\
      gui << gui_only() << at(h, w) << "outputs:" << out.x;		\
      w += 150;								\
      /* display outputs */						\
      idx_bloop1(m, out.T, double) {					\
	draw_matrix(m, h, w, zoom * zoomf, zoom * zoomf, vmin, vmax);	\
	w += m.dim(1) * zoom * zoomf + 1;				\
      }									\
      h0 += m.dim(0) * zoom * zoomf + 1;				\
    }									\
  }

  DISPLAY_LAYERSN(display_fprop, display_fprop2, x)
  DISPLAY_LAYERSN(display_bprop, display_bprop2, dx)
  DISPLAY_LAYERSN(display_bbprop, display_bbprop2, ddx)

} // end namespace ebl

#endif /* ELB_ARCH_GUI_HPP_ */
