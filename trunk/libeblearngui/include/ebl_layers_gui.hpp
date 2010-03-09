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

#ifndef ELB_LAYERS_GUI_HPP_
#define ELB_LAYERS_GUI_HPP_

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // nn_layer_full

#define NN_LAYER_FULL_GUI(name, T)					\
  template <class T>							\
  void nn_layer_full_gui::name(nn_layer_full<T> &nn,			\
			       state_idx<T> &in, state_idx<T> &out,	\
			       unsigned int &h0, unsigned int &w0,	\
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
    h0 += (uint) (MAX(10, m.dim(0) * zoom + 1));			\
  }									
  
  NN_LAYER_FULL_GUI(display_fprop, x)
  NN_LAYER_FULL_GUI(display_bprop, dx)
  NN_LAYER_FULL_GUI(display_bbprop, ddx)
  
  ////////////////////////////////////////////////////////////////
  // nn_layer_convolution

#define NN_LAYER_CONVOLUTION_GUI(name, T)				\
  template <class T>							\
  void nn_layer_convolution_gui::name(nn_layer_convolution<T> &nn,	\
				      state_idx<T> &in, state_idx<T> &out, \
				      unsigned int &h0,			\
				      unsigned int &w0,			\
				      double zoom, T vmin, T vmax,	\
				      bool show_out) {			\
    unsigned int h = h0, w = w0;					\
    /* display text */							\
    gui << gui_only() << at(h, w) << "conv. in:" << in.T;		\
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
    gui << gui_only()<< at(h, w) << "kernels:" << nn.convol.kernel.T;	\
    w += 150;								\
    /* display kernels */						\
    /* zoom *= 4; */	 						\
    idx_bloop1(mk, nn.convol.kernel.T, T) {				\
      if (w - w0 < 500) {						\
	draw_matrix(mk, h, w, zoom, zoom, vmin, vmax);			\
	w += (uint) (mk.dim(1) * zoom + 1);				\
      }									\
    }									\
    h0 += (uint) (MAX(10, mk.dim(0) * zoom + 1));			\
  }

  NN_LAYER_CONVOLUTION_GUI(display_fprop, x)
  NN_LAYER_CONVOLUTION_GUI(display_bprop, dx)
  NN_LAYER_CONVOLUTION_GUI(display_bbprop, ddx)
  
  ////////////////////////////////////////////////////////////////
  // layer_convabsnorm
  
#define LAYER_CONVABSNORM_GUI(name, T)					\
  template <class T>							\
  void layer_convabsnorm_gui::name(layer_convabsnorm<T> &layer,		\
				   state_idx<T> &in, state_idx<T> &out,	\
				   unsigned int &h0,			\
				   unsigned int &w0,			\
				   double zoom, T vmin, T vmax,		\
				   bool show_out) {			\
    nn_layer_convolution_gui::name(layer.lconv, in, out, h0, w0,	\
				   zoom, vmin, vmax, show_out);		\
    unsigned int h = h0, w = w0;					\
    /* display text */							\
    gui << gui_only() << at(h, w) << "conv out:" << layer.tmp->T;	\
    w += 150;								\
    /* display output of convolution */					\
    idx_bloop1(m, layer.tmp->T, T) {					\
      draw_matrix(m, h, w, zoom, zoom, vmin, vmax);			\
      w += (uint) (m.dim(1) * zoom + 1);				\
    }									\
    h0 += (uint) (m.dim(0) * zoom + 1);					\
    w = w0;								\
    h = h0;								\
    /* display text */							\
    gui << gui_only() << at(h, w) << "abs out:" << layer.tmp2->T;	\
    w += 150;								\
    /* display output of abs */						\
    idx_bloop1(m2, layer.tmp2->T, T) {					\
      draw_matrix(m2, h, w, zoom, zoom, vmin, vmax);			\
      w += (uint) (m2.dim(1) * zoom + 1);				\
    }									\
    h0 += (uint) (m2.dim(0) * zoom + 1);				\
    w = w0;								\
    h = h0;								\
    /* display normalization internals */				\
    weighted_std_module_gui::name(layer.norm, in, out, h0, w0,		\
				  zoom, vmin, vmax, show_out);		\
  }

  LAYER_CONVABSNORM_GUI(display_fprop, x)
  LAYER_CONVABSNORM_GUI(display_bprop, dx)
  LAYER_CONVABSNORM_GUI(display_bbprop, ddx)
  
  ////////////////////////////////////////////////////////////////
  // nn_layer_subsampling

#define NN_LAYER_SUBSAMPLING_GUI(name, T)				\
  template <class T>							\
  void nn_layer_subsampling_gui::name(nn_layer_subsampling<T> &nn,	\
				      state_idx<T> &in, state_idx<T> &out, \
				      unsigned int &h0,			\
				      unsigned int &w0,			\
				      double zoom, T vmin, T vmax,	\
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
    gui << gui_only()<< at(h, w) << "kernels:" << nn.subsampler.sub.T.dim(0); \
    gui << "x" << in.T.dim(1) / nn.subsampler.sub.T.dim(1);		\
    gui << "x" << in.T.dim(2) / nn.subsampler.sub.T.dim(2);		\
    w += 150;								\
    h0 += 10;								\
  }
  
  NN_LAYER_SUBSAMPLING_GUI(display_fprop, x)
  NN_LAYER_SUBSAMPLING_GUI(display_bprop, dx)
  NN_LAYER_SUBSAMPLING_GUI(display_bbprop, ddx)

}
  
#endif /* ELB_LAYERS_GUI_HPP_ */
