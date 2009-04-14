/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include "ebl_layers_gui.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // nn_layer_full

  void nn_layer_full_gui::display_fprop(nn_layer_full &nn,
					state_idx &in, state_idx &out,
					unsigned int &h0, unsigned int &w0,
					double zoom, bool show_out) {
    unsigned int h = h0, w = w0;
    // display input text
    gui << gui_only() << at(h, w) << "full in:" << in.x.dim(0) << "x";
    gui << in.x.dim(1) << "x" << in.x.dim(2);
    w += 150;
    zoom *= 5;
    // display inputs
    idx_bloop1(m, in.x, double) {
      if (w - w0 < 500) {
	gui.draw_matrix(m, h, w, zoom, zoom, -1.0, 1.0);
	w += m.dim(1) * zoom + 1;
      }
    }
    h0 += MAX(10, m.dim(0) * zoom + 1);
  }

  ////////////////////////////////////////////////////////////////
  // nn_layer_convolution

  void nn_layer_convolution_gui::display_fprop(nn_layer_convolution &nn,
					       state_idx &in, state_idx &out,
					       unsigned int &h0, 
					       unsigned int &w0,
					       double zoom, bool show_out) {
    unsigned int h = h0, w = w0;
    // display input text
    gui << gui_only() << at(h, w) << "conv. in:" << in.x.dim(0) << "x";
    gui << in.x.dim(1) << "x" << in.x.dim(2);
    w += 150;
    // display inputs
    idx_bloop1(m, in.x, double) {
      gui.draw_matrix(m, h, w, zoom, zoom);
      w += m.dim(1) * zoom + 1;
    }
    h0 += m.dim(0) * zoom + 1;
    w = w0;
    h = h0;
    // display kernels text
    gui << gui_only()<< at(h, w) << "kernels:" ;
    gui << nn.convol.kernel.x.dim(0) << "x";
    gui << nn.convol.kernel.x.dim(1) << "x" << nn.convol.kernel.x.dim(2);
    w += 150;
    // display kernels
    zoom *= 4;
    idx_bloop1(mk, nn.convol.kernel.x, double) {
      if (w - w0 < 500) {
	gui.draw_matrix(mk, h, w, zoom, zoom);
	w += mk.dim(1) * zoom + 1;
      }
    }
    h0 += MAX(10, mk.dim(0) * zoom + 1);
  }

  ////////////////////////////////////////////////////////////////
  // nn_layer_subsampling

  void nn_layer_subsampling_gui::display_fprop(nn_layer_subsampling &nn,
					       state_idx &in, state_idx &out,
					       unsigned int &h0, 
					       unsigned int &w0,
					       double zoom, bool show_out) {
    unsigned int h = h0, w = w0;
    // display input text
    gui << gui_only() << at(h, w) << "ssampl. in:" << in.x.dim(0) << "x";
    gui << in.x.dim(1) << "x" << in.x.dim(2);
    w += 150;
    // display inputs
    idx_bloop1(m, in.x, double) {
      gui.draw_matrix(m, h, w, zoom, zoom);
      w += m.dim(1) * zoom + 1;
    }
    h0 += m.dim(0) * zoom + 1;
    w = w0;
    h = h0;
    // display kernels text
    gui << gui_only()<< at(h, w) << "kernels:" << nn.subsampler.sub.x.dim(0);
    gui << "x" << in.x.dim(1) / nn.subsampler.sub.x.dim(1);
    gui << "x" << in.x.dim(2) / nn.subsampler.sub.x.dim(2);
    w += 150;
//     // display kernels
//     idx_bloop1(mk, nn.subsampler.sub.x, double) {
//       if (w < 500) {
// 	gui.draw_matrix(mk, h, w, zoom, zoom);
// 	w += mk.dim(1) * zoom + 1;
//       }
//     }
//     h0 += mk.dim(0) * zoom + 1;
    h0 += 10;
  }

} // end namespace ebl
