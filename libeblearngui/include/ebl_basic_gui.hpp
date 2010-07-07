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

#ifndef ELB_BASIC_GUI_HPP_
#define ELB_BASIC_GUI_HPP_

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // linear_module
  
#define LINEAR_MODULE_GUI(name, T)						\
  template <class T>							\
  void linear_module_gui::name(linear_module<T> &nn,				\
			    state_idx<T> &in, state_idx<T> &out,	\
			    unsigned int &h0, unsigned int &w0,		\
			    double zoom, T vmin, T vmax,		\
			    bool show_out) {				\
    unsigned int h = h0, w = w0;					\
    /* display text */							\
    gui << gui_only() << at(h, w) << "full in:" << in.T;		\
    w += 150;								\
    zoom *= 3;								\
    /* display inputs */						\
    idx_bloop1(m, in.T, T) {						\
      if (w - w0 < 500) {						\
	draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
	w += (uint) (m.dim(1) * zoom + 1);				\
      }									\
    }									\
    h0 += (uint) (std::max((uint) 10, (uint) (m.dim(0) * zoom + 1)));	\
  }									
  
  // LINEAR_MODULE_GUI(display_fprop, x)
  // LINEAR_MODULE_GUI(display_bprop, dx)
  // LINEAR_MODULE_GUI(display_bbprop, ddx)
  
  ////////////////////////////////////////////////////////////////
  // convolution_module

#define CONVOLUTION_MODULE_GUI(name, T)					\
  template <class T>							\
  void convolution_module_gui::name(convolution_module<T> &nn,		\
				    state_idx<T> &in, state_idx<T> &out, \
				    unsigned int &h0,			\
				    unsigned int &w0,			\
				    double zoom, T vmin, T vmax,	\
				    bool show_out) {			\
    unsigned int h = h0, w = w0;					\
    /* display text */							\
    gui << gui_only() << at(h, w) << "conv. in:" << in.T		\
	<< at(h + 15, w) << "min:" << idx_min(in.T)			\
	<< at(h + 30, w) << "max:" << idx_max(in.T);			\
    w += 150;								\
    /* display inputs */						\
    idx_bloop1(m, in.T, T) {						\
      draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
      w += (uint) (m.dim(1) * zoom + 1);				\
    }									\
    h0 += (uint) (m.dim(0) * zoom + 1);					\
    w = w0;								\
    h = h0;								\
    /* display kernels text */						\
    gui << gui_only()<< at(h, w) << "kernels:" << nn.kernel.T;		\
    w += 150;								\
    /* display kernels */						\
    /* zoom *= 4; */	 						\
    idx_bloop1(mk, nn.kernel.T, T) {					\
      if (w - w0 < 500) {						\
	draw_matrix(mk, h, w, zoom, zoom, vmin, vmax);			\
	w += (uint) (mk.dim(1) * zoom + 1);				\
      }									\
    }									\
    h0 += (uint) (std::max((uint) 10, (uint) (mk.dim(0) * zoom + 1)));	\
  }

  CONVOLUTION_MODULE_GUI(display_fprop, x)
  CONVOLUTION_MODULE_GUI(display_bprop, dx)
  CONVOLUTION_MODULE_GUI(display_bbprop, ddx)
    
  ////////////////////////////////////////////////////////////////
  // subsampling_module

#define SUBSAMPLING_MODULE_GUI(name, T)					\
  template <class T>							\
  void subsampling_module_gui::name(subsampling_module<T> &nn,		\
				   state_idx<T> &in, state_idx<T> &out, \
				   unsigned int &h0,			\
				   unsigned int &w0,			\
				   double zoom, T vmin, T vmax,		\
				   bool show_out) {			\
    unsigned int h = h0, w = w0;					\
    /* display input text	*/					\
    gui << gui_only() << at(h, w) << "ssampl. in:" << in.T;		\
    w += 150;								\
    /* display inputs */						\
    idx_bloop1(m, in.T, T) {						\
      draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
      w += (uint) (m.dim(1) * zoom + 1);				\
    }									\
    h0 += (uint) (m.dim(0) * zoom + 1);					\
    w = w0;								\
    h = h0;								\
    /* display kernels text */						\
    gui << gui_only()<< at(h, w) << "kernels:" << nn.sub.T.dim(0); \
    gui << "x" << in.T.dim(1) / nn.sub.T.dim(1);		\
    gui << "x" << in.T.dim(2) / nn.sub.T.dim(2);		\
    w += 150;								\
    h0 += 10;								\
  }
  
  SUBSAMPLING_MODULE_GUI(display_fprop, x)
  SUBSAMPLING_MODULE_GUI(display_bprop, dx)
  SUBSAMPLING_MODULE_GUI(display_bbprop, ddx)

}
  
#endif /* ELB_BASIC_GUI_HPP_ */
