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

#ifndef COLOR_SPACES_H_
#define COLOR_SPACES_H_

#include "libidx.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // YUV

  extern idx<double> rgb_yuv;
  extern idx<double> yuv_rgb;
  
  //! Convert all pixels of rgb idx to yuv pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T> void rgb_to_yuv(idx<T> &rgb, idx<T> &yuv);
  //! Same as yuv_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T> idx<T> rgb_to_yuv(idx<T> &rgb);

  //! Convert all pixels of yuv idx to rgb pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T> void yuv_to_rgb(idx<T> &yuv, idx<T> &rgb);
  //! Same as yuv_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T> idx<T> yuv_to_rgb(idx<T> &yuv);

  //! Normalize a YUV image (wxhx3), centered between on [-2.5 .. 2.5]
  void YUVGlobalNormalization(idx<float> &yuv);
  
  ////////////////////////////////////////////////////////////////
  // HSV

  //! Convert all pixels of rgb idx to hsv pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T> void rgb_to_hsv(idx<T> &rgb, idx<T> &hsv);
  //! Same as hsv_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T> idx<T> rgb_to_hsv(idx<T> &rgb);

  //! Convert all pixels of hsv idx to rgb pixels.
  //! If the input idx has order of 1, it converts only 1 pixel.
  //! If the order is 3, it converts all pixels.
  template<class T> void hsv_to_rgb(idx<T> &hsv, idx<T> &rgb);
  //! Same as hsv_to_rgb with 2 arguments except that it allocates a target
  //! image and returns it.
  template<class T> idx<T> hsv_to_rgb(idx<T> &hsv);

} // end namespace ebl

#include "color_spaces.hpp"

#endif /* COLOR_SPACES_H_ */
