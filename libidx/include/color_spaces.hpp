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

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than yuv matrix.
  template<class T> void rgb_to_yuv_1D(idx<T> &rgb, idx<T> &yuv) {
    if (rgb.idx_ptr() == yuv.idx_ptr()) {
      eblerror("rgb_to_yuv: dst must be different than src");
      return ;
    }
    static double r, g, b;
    r = rgb.get(0);
    g = rgb.get(1);
    b = rgb.get(2);
    yuv.set(  0.299 * r + 0.587 * g + 0.114 * b, 0);
    yuv.set((-0.147 * r - 0.289 * g + 0.437 * b + 111) * 1.14678, 1);
    yuv.set(( 0.615 * r - 0.515 * g - 0.100 * b + 157) * 0.81300, 2);
  }

  template<class T> void rgb_to_yuv(idx<T> &rgb, idx<T> &yuv) {
    idx_checknelems2_all(rgb, yuv);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_yuv_1D(rgb, yuv);
      //      idx_m2dotm1(rgb_yuv, rgb, yuv);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, yuv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      rgb_to_yuv_1D(r, y);
	      //	      idx_m2dotm1(rgb_yuv, r, y);
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
  
  template<class T> void yuv_to_rgb_1D(idx<T> &yuv, idx<T> &rgb) {
    if (rgb.idx_ptr() == yuv.idx_ptr()) {
      eblerror("yub_to_rgb: dst must be different than src");
      return ;
    }
    static double y, u, v;
    y = yuv.get(0);
    u = (yuv.get(1) / 1.14678) - 111;
    v = (yuv.get(2) / 0.81300) - 157;
    rgb.set(1.0 * y + 0.00000 * u + 1.13983 * v, 0);
    rgb.set(1.0 * y - 0.39465 * u - 0.58060 * v, 1);
    rgb.set(1.0 * y + 2.03211 * u + 0.00000 * v, 2);
  }

  template<class T> void yuv_to_rgb(idx<T> &yuv, idx<T> &rgb) {
    idx_checknelems2_all(rgb, yuv);
    switch (yuv.order()) {
    case 1: // process 1 pixel
      yuv_to_rgb_1D(yuv, rgb);
      //idx_m2dotm1(yuv_rgb, yuv, rgb);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, yuv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      yuv_to_rgb_1D(y, r);
	      //idx_m2dotm1(yuv_rgb, y, r);
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

  ////////////////////////////////////////////////////////////////
  // HSV

  // ######################################################################
  // T. Nathan Mundhenk
  // mundhenk@usc.edu
  // C/C++ Macro RGB to HSV
#define PIX_RGB_TO_HSV_COMMON(R,G,B,H,S,V,NORM)		\
  if((B > G) && (B > R))				\
    {							\
      V = B;						\
      if(V != 0)					\
	{						\
	  double min;					\
	  if(R > G) min = G;				\
	  else      min = R;				\
	  const double delta = V - min;			\
	  if(delta != 0)				\
	    { S = (delta/V); H = 5 + (R - G) / delta; }	\
	  else						\
	    { S = 0;         H = 5 + (R - G); }		\
	  H *=   60;					\
	  if (H >= 360) H -= 360;			\
	  if(H < 0) H += 360;				\
	  if(!NORM) V =  (V/255);			\
	  else      S *= (100);				\
	}						\
      else						\
	{ S = 0; H = 0;}				\
    }							\
  else if(G > R)					\
    {							\
      V = G;						\
      if(V != 0)					\
	{						\
	  double min;					\
	  if(R > B) min = B;				\
	  else      min = R;				\
	  const double delta = V - min;			\
	  if(delta != 0)				\
	    { S = (delta/V); H = 3 + (B - R) / delta; }	\
	  else						\
	    { S = 0;         H = 3 + (B - R); }		\
	  H *=   60;					\
	  if (H >= 360) H -= 360;			\
	  if(H < 0) H += 360;				\
	  if(!NORM) V =  (V/255);			\
	  else      S *= (100);				\
	}						\
      else						\
	{ S = 0; H = 0;}				\
    }							\
  else							\
    {							\
      V = R;						\
      if(V != 0)					\
	{						\
	  double min;					\
	  if(G > B) min = B;				\
	  else      min = G;				\
	  const double delta = V - min;			\
	  if(delta != 0)				\
	    { S = (delta/V); H = 1 + (G - B) / delta; }	\
	  else						\
	    { S = 0;         H = 1 + (G - B); }	\
	  H *=   60;					\
	  if (H >= 360) H -= 360;			\
	  if(H < 0) H += 360;				\
	  if(!NORM) V =  (V/255);			\
	  else      S *= (100);				\
	}						\
      else						\
	{ S = 0; H = 0;}				\
    }

  // ######################################################################
  // T. Nathan Mundhenk
  // mundhenk@usc.edu
  // C/C++ Macro HSV to RGB
#define PIX_HSV_TO_RGB_COMMON(H,S,V,R,G,B)				\
  if( V == 0 )								\
    { R = 0; G = 0; B = 0; }                                            \
  else if( S == 0 )							\
    {                                                                   \
      R = V;                                                            \
      G = V;                                                            \
      B = V;                                                            \
    }                                                                   \
  else									\
    {                                                                   \
      const double hf = H / 60.0;                                       \
      const int    i  = (int) floor( hf );                              \
      const double f  = hf - i;                                         \
      const double pv  = V * ( 1 - S );                                 \
      const double qv  = V * ( 1 - S * f );                             \
      const double tv  = V * ( 1 - S * ( 1 - f ) );                     \
      switch( i )                                                       \
	{                                                               \
	case 1:                                                         \
	  R = V;                                                        \
	  G = tv;                                                       \
	  B = pv;                                                       \
	  break;                                                        \
	case 2:                                                         \
	  R = qv;                                                       \
	  G = V;                                                        \
	  B = pv;                                                       \
	  break;                                                        \
	case 3:                                                         \
	  R = pv;                                                       \
	  G = V;                                                        \
	  B = tv;                                                       \
	  break;                                                        \
	case 4:                                                         \
	  R = pv;                                                       \
	  G = qv;                                                       \
	  B = V;                                                        \
	  break;                                                        \
	case 5:                                                         \
	  R = tv;                                                       \
	  G = pv;                                                       \
	  B = V;                                                        \
	  break;                                                        \
	case 0:                                                         \
	  R = V;                                                        \
	  G = pv;                                                       \
	  B = qv;                                                       \
	  break;                                                        \
	case 6:                                                         \
	  R = V;                                                        \
	  G = tv;                                                       \
	  B = pv;                                                       \
	  break;                                                        \
	case -1:                                                        \
	  R = V;                                                        \
	  G = pv;                                                       \
	  B = qv;                                                       \
	  break;                                                        \
	default:                                                        \
	  cerr << "i Value error in Pixel conversion, Value is " << i << endl; \
	  eblerror("hsv to rgb error");					\
	  break;                                                        \
	}                                                               \
    }                                                                   \
  R *= 255.0F;								\
  G *= 255.0F;								\
  B *= 255.0F;


  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than hsv matrix.
  template<class T> void rgb_to_hsv_1D(idx<T> &rgb, idx<T> &hsv) {
    if (rgb.idx_ptr() == hsv.idx_ptr()) {
      eblerror("rgb_to_hsv: dst must be different than src");
      return ;
    }
    static double r, g, b;
    r = rgb.get(0);
    g = rgb.get(1);
    b = rgb.get(2);
    static double h, s, v;
    PIX_RGB_TO_HSV_COMMON(r, g, b, h, s, v, false);
    hsv.set(h, 0);
    hsv.set(s, 1);
    hsv.set(v, 2);
  }

  template<class T> void rgb_to_hsv(idx<T> &rgb, idx<T> &hsv) {
    idx_checknelems2_all(rgb, hsv);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_hsv_1D(rgb, hsv);
      //      idx_m2dotm1(rgb_hsv, rgb, hsv);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, hsv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      rgb_to_hsv_1D(r, y);
	      //	      idx_m2dotm1(rgb_hsv, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_hsv dimension not implemented");
    }
  }

  template<class T> idx<T> rgb_to_hsv(idx<T> &rgb) {
    idxdim d(rgb);
    idx<T> hsv(d);
    rgb_to_hsv(rgb, hsv);
    return hsv;
  }
  
  template<class T> void hsv_to_rgb_1D(idx<T> &hsv, idx<T> &rgb) {
    if (rgb.idx_ptr() == hsv.idx_ptr()) {
      eblerror("hsv_to_rgb: dst must be different than src");
      return ;
    }
    static double h, s, v;
    h = hsv.get(0);
    s = hsv.get(1);
    v = hsv.get(2);
    static double r, g, b;
    PIX_HSV_TO_RGB_COMMON(h, s, v, r, g, b);
    rgb.set(r, 0);
    rgb.set(g, 1);
    rgb.set(b, 2);
  }

  template<class T> void hsv_to_rgb(idx<T> &hsv, idx<T> &rgb) {
    idx_checknelems2_all(rgb, hsv);
    switch (hsv.order()) {
    case 1: // process 1 pixel
      hsv_to_rgb_1D(hsv, rgb);
      //idx_m2dotm1(hsv_rgb, hsv, rgb);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, hsv, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      hsv_to_rgb_1D(y, r);
	      //idx_m2dotm1(hsv_rgb, y, r);
	    }}
	}}
      return ;
    default:
      eblerror("hsv_to_rgb dimension not implemented");
    }
  }

  template<class T> idx<T> hsv_to_rgb(idx<T> &hsv) {
    idxdim d(hsv);
    idx<T> rgb(d);
    hsv_to_rgb(hsv, rgb);
    return rgb;
  }

  ////////////////////////////////////////////////////////////////
  // HSV3

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than hsv3 matrix.
  template<class T> void rgb_to_hsv3_1D(idx<T> &rgb, idx<T> &hsv3,
					double threshold1, double threshold2) {
    if (rgb.idx_ptr() == hsv3.idx_ptr()) {
      eblerror("rgb_to_hsv3: dst must be different than src");
      return ;
    }
    static double r, g, b;
    r = rgb.get(0);
    g = rgb.get(1);
    b = rgb.get(2);
    static double h, s, v;
    PIX_RGB_TO_HSV_COMMON(r, g, b, h, s, v, false);
    if ((s < threshold1) || (v < threshold2)) {
      h = 360 + 60 * v;
    }
    hsv3.set(h, 0);
    hsv3.set(s, 1);
    hsv3.set(v, 2);
  }

  template<class T> void rgb_to_hsv3(idx<T> &rgb, idx<T> &hsv3,
				     double threshold1, double threshold2) {
    idx_checknelems2_all(rgb, hsv3);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_hsv3_1D(rgb, hsv3, threshold1, threshold2);
      //      idx_m2dotm1(rgb_hsv3, rgb, hsv3);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, hsv3, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      rgb_to_hsv3_1D(r, y, threshold1, threshold2);
	      //	      idx_m2dotm1(rgb_hsv3, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_hsv3 dimension not implemented");
    }
  }

  template<class T> idx<T> rgb_to_hsv3(idx<T> &rgb, double threshold1,
				       double threshold2) {
    idxdim d(rgb);
    idx<T> hsv3(d);
    rgb_to_hsv3(rgb, hsv3, threshold1, threshold2);
    return hsv3;
  }
  
  template<class T> void hsv3_to_rgb_1D(idx<T> &hsv3, idx<T> &rgb) {
    if (rgb.idx_ptr() == hsv3.idx_ptr()) {
      eblerror("hsv3_to_rgb: dst must be different than src");
      return ;
    }
    static double h, s, v;
    h = hsv3.get(0);
    s = hsv3.get(1);
    v = hsv3.get(2);
    static double r, g, b;
    PIX_HSV3_TO_RGB_COMMON(h, s, v, r, g, b);
    rgb.set(r, 0);
    rgb.set(g, 1);
    rgb.set(b, 2);
  }

  template<class T> void hsv3_to_rgb(idx<T> &hsv3, idx<T> &rgb) {
    idx_checknelems2_all(rgb, hsv3);
    switch (hsv3.order()) {
    case 1: // process 1 pixel
      hsv3_to_rgb_1D(hsv3, rgb);
      //idx_m2dotm1(hsv3_rgb, hsv3, rgb);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, hsv3, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      hsv3_to_rgb_1D(y, r);
	      //idx_m2dotm1(hsv3_rgb, y, r);
	    }}
	}}
      return ;
    default:
      eblerror("hsv3_to_rgb dimension not implemented");
    }
  }

  template<class T> idx<T> hsv3_to_rgb(idx<T> &hsv3) {
    idxdim d(hsv3);
    idx<T> rgb(d);
    hsv3_to_rgb(hsv3, rgb);
    return rgb;
  }
  
  ////////////////////////////////////////////////////////////////
  // YH3

  // TODO: find a cleaner way with matrix multiplication that can handle
  // different output type than yh3 matrix.
  template<class T> void rgb_to_yh3_1D(idx<T> &rgb, idx<T> &yh3,
					double threshold1, double threshold2) {
    if (rgb.idx_ptr() == yh3.idx_ptr()) {
      eblerror("rgb_to_yh3: dst must be different than src");
      return ;
    }
    static idx<T> hsv3(3);
    rgb_to_hsv3_1D(rgb, hsv3, threshold1, threshold2);
    yh3.set((0.299 * rgb.get(0) + 0.587 * rgb.get(1) + 0.114 * rgb.get(2))
	    /255.0, 0);
    yh3.set(  hsv3.get(0) / 420.0, 1);
  }

  template<class T> void rgb_to_yh3(idx<T> &rgb, idx<T> &yh3,
				     double threshold1, double threshold2) {
    //    idx_checknelems2_all(rgb, yh3);
    switch (rgb.order()) {
    case 1: // process 1 pixel
      rgb_to_yh3_1D(rgb, yh3, threshold1, threshold2);
      //      idx_m2dotm1(rgb_yh3, rgb, yh3);
      return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, yh3, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      rgb_to_yh3_1D(r, y, threshold1, threshold2);
	      //	      idx_m2dotm1(rgb_yh3, r, y);
	    }}
	}}
      return ;
    default:
      eblerror("rgb_to_yh3 dimension not implemented");
    }
  }

  template<class T> idx<T> rgb_to_yh3(idx<T> &rgb, double threshold1,
				       double threshold2) {
    idxdim d(rgb);
    idx<T> yh3(d);
    rgb_to_yh3(rgb, yh3, threshold1, threshold2);
    return yh3;
  }

  template<class T> void h3_to_rgb_1D(idx<T> &h3, idx<T> &rgb) {
    if (rgb.idx_ptr() == h3.idx_ptr()) {
      eblerror("h3_to_rgb: dst must be different than src");
      return ;
    }
    static double r, g, b;
    PIX_HSV3_TO_RGB_COMMON(h3.get(), .5, .5, r, g, b);
    rgb.set(r, 0);
    rgb.set(g, 1);
    rgb.set(b, 2);
  }

  template<class T> void h3_to_rgb(idx<T> &h3, idx<T> &rgb) {
    //    idx_checknelems2_all(rgb, h3);
    switch (h3.order()) {
    case 2: // process 1 pixel
//       h3_to_rgb_1D(h3, rgb);
//       //idx_m2dotm1(h3_rgb, h3, rgb);
//       return ;
    case 3: // process 2D image
      { idx_bloop2(rg, rgb, T, yu, h3, T) {
	  { idx_bloop2(r, rg, T, y, yu, T) {
	      h3_to_rgb_1D(y, r);
	      //idx_m2dotm1(h3_rgb, y, r);
	    }}
	}}
      return ;
    default:
      eblerror("h3_to_rgb dimension not implemented");
    }
  }

  template<class T> idx<T> h3_to_rgb(idx<T> &h3) {
    idxdim d(h3);
    idx<T> rgb(d);
    h3_to_rgb(h3, rgb);
    return rgb;
  }  
  
} // end namespace ebl

#endif /* COLOR_SPACES_HPP_ */
