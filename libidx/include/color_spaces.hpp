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

#ifndef COLOR_SPACES_HPP_
#define COLOR_SPACES_HPP_

#include <algorithm>
#include <math.h>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // YUV

  template<class T> void rgb_to_yuv(idx<T> &rgb, idx<T> &yuv) {
    idx_checknelems2_all(rgb, yuv);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      idx_m2dotm1(rgb_yuv, rgb, yuv);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, yuv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      idx_m2dotm1(rgb_yuv, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_yuv dimension not implemented");
    }
  }

  template<class T> idx<T> rgb_to_yuv(idx<T> &rgb) {
    idxdim d(rgb);
    idx<T> yuv(d);
    rgb_to_yuv(rgb, yuv);
    return yuv;
  }
  
  template<class T> void yuv_to_rgb(idx<T> &yuv, idx<T> &rgb) {
    idx_checknelems2_all(rgb, yuv);
    switch (yuv.order()) {
    case 1: // process 1 pixel
      idx_m2dotm1(yuv_rgb, yuv, rgb);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, yuv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      idx_m2dotm1(yuv_rgb, y, r);
	    }}
	}}
      return ;
    default:
      eblerror("yuv_to_rgb dimension not implemented");
    }
  }

  template<class T> idx<T> yuv_to_rgb(idx<T> &yuv) {
    idxdim d(yuv);
    idx<T> rgb(d);
    yuv_to_rgb(yuv, rgb);
    return rgb;
  }
  
} // end namespace ebl

#endif /* COLOR_SPACES_HPP_ */
