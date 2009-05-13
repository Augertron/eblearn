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

#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <algorithm>
#include <math.h>

using namespace std;

#define BLK_AVRG(nlin, ncol) {						\
    int k,l; int norm = ncol * nlin;					\
    int acc0=0, acc1=0, acc2=0;						\
    for (k=0; k<nlin; k++) {						\
      register T *pinptr = pin+k*in_mod0;				\
      for (l=0; l<ncol; l++) {						\
	acc0 += pinptr[0];acc1 += pinptr[1];acc2 += pinptr[2];		\
	pinptr += in_mod1; }}						\
    pout[0] = acc0 / norm;pout[1] = acc1 / norm;pout[2] = acc2 / norm;}

namespace ebl {

  template<class T>
  idx<T> image_crop(idx<T> &in, int x, int y, int w, int h) {
    idx<T> bla = in.narrow(0, h, y).narrow(1, w, x);
    idx<T> bla3(bla.dim(0), bla.dim(1), bla.order() < 3 ? -1 : bla.dim(2));
    idx_copy(bla, bla3);
    return bla3;
  }

  template<class T> idx<T> image_resize(idx<T> &image, double w, double h, 
					int mode) {
    if (image.order() < 2) eblerror("image must have at least an order of 2.");
    // if data is not contiguous, copy it to a contiguous buffer
    idx<T> contim(image);
    if (!image.contiguousp()) {
      idxdim d(image.spec);
      idx<T> tmp(d);
      idx_copy(image, tmp);
      contim = tmp;
    }
    intg imw = contim.dim(1);
    intg imh = contim.dim(0);
    if ((imw == 0) || (imh == 0))
      eblerror("cannot have dimensions of size 0");
    int rw = 0;
    int rh = 0;
    // determine actual size of output image
    if ((0 == w) || (0 == h)) {
      if (0 == w) {
	if (0 == h) {
	  eblerror("desired width and height cannot be both zero");
	} else {
	  w = max(1, (int) (imw * (h / imh)));
	}
      } else h = max(1, (int) (imh * (w / imw)));
    }
    else if (mode == 0) {
      double r = min(w / imw, h / imh);
      w = max(1, (int) (r * imw));
      h = max(1, (int) (r * imh));
    }
    else if (mode == 1) {
      w = max(1, (int) w);
      h = max(1, (int) h);
    }
    else if (mode == 2) {
      w = max(1, (int) (w * imw));
      h = max(1, (int) (h * imh));
    }
    else eblerror("image_resize: illegal mode or desired dimensions");
    // compute closest integer subsampling ratio
    rw = (int) (imw / w);
    rh = (int) (imh / h);
    // subsample by integer ratio if necessary
    if ((rh > 1) || (rw > 1)) {
      contim = image_subsample(contim, rh, rw);
      imw = contim.dim(1);
      imh = contim.dim(0);
    }
    // resample from subsampled image with bilinear interpolation
    idx<T> rez((intg) h, (intg) w, (contim.order() == 3) ? contim.dim(2) : 1);
    idx<T> bg(4);
    idx_clear(bg);
    // the 0.5 thingies are necessary because warp-bilin interprets
    // integer coordinates as being at the center of each pixel.
    float x1 = -0.5, y1 = -0.5, x3 = imw - 0.5, y3 = imh - 0.5;
    float p1 = -0.5, q1 = -0.5, p3 = w - 0.5, q3 = h - 0.5;
    image_warp_quad(contim, rez, bg, 1, x1, y1, x3, y1, x3, y3, x1, y3, 
		    p1, q1, p3, q3);
    if (contim.order() == 2)
      return rez.select(2, 0);
    return rez;
  }

  template<class T> 
  idx<ubyte> image_to_ubyte(idx<T> &im, double zoomh, double zoomw,
			    T minv, T maxv) {
    // check the order and dimensions
    if ((im.order() < 2) || (im.order() > 3) || 
	((im.order() == 3) && (im.dim(2) != 1) && (im.dim(2) != 3))) 
      eblerror("expecting a 2D idx or a 3D idx with 1 or 3 channels only");
    // create a copy
    idxdim d(im);
    idx<T> im1(d);
    idx_copy(im, im1);
    // check zoom factor
    if ((zoomw <= 0.0) || (zoomh <= 0.0))
      eblerror("cannot zoom by a factor <= 0.0");
    // check minv maxv
    if (minv > maxv) {
      T tmp = minv;
      minv = maxv;
      maxv = tmp;
    }
    // if minv and maxv are defaults, take actual min and max of the image
    if (minv == maxv) {
      minv = idx_min(im1);
      maxv = idx_max(im1);
    }
    // create target image
    int newh = im1.dim(0) * zoomh;
    int neww = im1.dim(1) * zoomw;
    idx<T> im2 = ((newh == im1.dim(0)) && (neww == im1.dim(1))) ?
      im1 : image_resize(im1, neww, newh);
    d.setdim(0, newh);
    d.setdim(1, neww);
    idx<ubyte> image(d);
    // map values between minv and maxv to 0 .. 255
    idx_subc_bounded(im2, minv, im2);
    idx_dotc_bounded(im2, (T) (255.0 / (double) (maxv - minv)), im2);
    idx_copy_clip(im2, image);
    return image;
  }

  template<class T>
  idx<T> image_subsample_grayscale(idx<T> &in, int nlin, int ncol) {
    intg h = in.dim(0);
    intg w = in.dim(1);
    intg nh = h / nlin;
    intg nw = w / ncol;
    idx<T> out(nh, nw);
    if ((nlin == 1) && (ncol == 1)) {
      idx_copy(in, out);
      return out;
    }
    idx<T> inp = in.narrow(0, nlin * nh, 0);
    inp = inp.narrow(1, ncol * nw, 0);
    T *_idx2loopc1, *pin;
    T *_idx2loopc2, *pout;
    int i, _imax = out.dim(0);
    int j, _jmax = out.dim(1);
    int _imat1_m0 = inp.mod(0);
    int _imat1_m1 = inp.mod(1);
    int _imat2_m0 = out.mod(0);
    int _imat2_m1 = out.mod(1);
    int pin_incr = ncol * _imat1_m1;
    int norm = ncol * nlin;
    register int acc0;
    register int k,l;
    register T *pinptr;
    register int pinptr_incr = _imat1_m0 - ncol* _imat1_m1;
    _idx2loopc1 = inp.idx_ptr();
    _idx2loopc2 = out.idx_ptr();
    for (i = 0; i < _imax; i++) {
      pin = _idx2loopc1;
      pout = _idx2loopc2;
      for (j = 0; j < _jmax; j++) {
	acc0 =0;
	pinptr = pin;
	for (k=0; k<nlin; k++) {
	  for (l=0; l<ncol; l++) {
	    acc0 += pinptr[0];
	    pinptr += _imat1_m1;
	  }
	  pinptr += pinptr_incr;
	}
	pout[0] = acc0/norm;
	pin += pin_incr;
	pout += _imat2_m1;
      }
      _idx2loopc1 += _imat1_m0 * nlin;
      _idx2loopc2 += _imat2_m0;
    }
    return out;
  }

  template<class T> idx<T> image_subsample_rgb(idx<T> &in, int nlin, int ncol) {
    intg h = in.dim(0);
    intg w = in.dim(1);
    intg nh = h / nlin;
    intg nw = w / ncol;
    idx<T> out(nh, nw, in.dim(2));
    if ((nlin == 1) && (ncol == 1)) {
      idx_copy(in, out);
      return out;
    }
    idx<T> inp = in.narrow(0, nlin * nh, 0).narrow(1, ncol * nw, 0);
    T *in_line, *pin;
    T *out_line, *pout;
    int i, _imax = out.dim(0);
    int j, _jmax = out.dim(1);
    int in_mod0 = inp.mod(0);
    int in_mod1 = inp.mod(1);
    int out_mod0 = out.mod(0);
    int out_mod1 = out.mod(1);
    int pin_incr = ncol * in_mod1;
    in_line = inp.idx_ptr();
    out_line = out.idx_ptr();
    for (i = 0; i < _imax; i++) {
      pin = in_line;
      pout = out_line;
      for (j = 0; j < _jmax; j++) {
	BLK_AVRG(nlin, ncol);
	pin += pin_incr;
	pout += out_mod1;
      }
      in_line += in_mod0 * nlin;
      out_line += out_mod0;
    }
    return out;
  }

  template<class T> idx<T> image_subsample(idx<T> &in, int nlin, int ncol) {
    switch (in.order()) {
    case 2:
      return image_subsample_grayscale(in, nlin, ncol);
    case 3:
      return image_subsample_rgb(in, nlin, ncol);
    default:
      eblerror("image must have at least an order of 2.");
      return in;
    }
  }

  template<class T> void image_warp_quad(idx<T> &in, idx<T> &out,
					 idx<T> &background, int mode,
					 float x1, float y1, float x2, float y2, float x3, float y3,
					 float x4, float y4, float p1, float q1, float p3, float q3) {
    intg outi = out.dim(0);
    intg outj = out.dim(1);
    idx<int> dispi(outi, outj);
    idx<int> dispj(outi, outj);
    compute_bilin_transform<float>(dispi, dispj, x1, y1, x2, y2, x3, y3,
				   x4, y4, p1, q1, p3, q3);
    if (0 == mode)
      image_warp_fast(in, out, background.idx_ptr(), dispi, dispj);
    else
      image_warp(in, out, background, dispi, dispj);
  }


  template<class T> void image_warp(idx<T> &in, idx<T> &out, idx<T> &background,
				    idx<int> &pi, idx<int> &pj) {
    T* pin = in.idx_ptr();
    int indimi = in.dim(0);
    int indimj = in.dim(1);
    int inmodi = in.mod(0);
    int inmodj = in.mod(1);
    int ppi, ppj;
    { idx_bloop3(lout, out, T, lpi, pi, int, lpj, pj, int) {
  	{ idx_bloop3(llout, lout, T, llpi, lpi, int, llpj, lpj, int) {
	    ppi = llpi.get();
	    ppj = llpj.get();
	    image_interpolate_bilin(background.idx_ptr(), pin, indimi, indimj,
				    inmodi, inmodj, ppi, ppj, llout.idx_ptr(), (int) out.dim(2));
	  }}
      }}
  }

  template<class T> void image_warp_fast(idx<T> &in, idx<T> &out, T *background,
					 idx<int> &pi, idx<int> &pj) {
    T* pin = in.idx_ptr();
    int indimi = in.dim(0);
    int indimj = in.dim(1);
    int inmodi = in.mod(0);
    int inmodj = in.mod(1);
    int ppi, ppj;
    register int li, lj;
    register T *inn, *outt;
    int outsize = out.dim(2);
    { idx_bloop3(lout, out, T, lpi, pi, int, lpj, pj, int) {
	{ idx_bloop3(llout, lout, T, llpi, lpi, int, llpj, lpj, int) {
	    outt = llout.idx_ptr();
	    ppi = llpi.get();
	    ppj = llpj.get();
	    li = (ppi+0x7f) >> 16;
	    lj = (ppj+0x7f) >> 16;
	    if ((li>=0) && (li<indimi) && (lj>=0) && (lj<indimj)) {
	      inn = (T*)(pin) + inmodi * li + inmodj * lj;
	      outt[0] = inn[0];
	      if (outsize >= 3) {
		outt[1] = inn[1];
		outt[2] = inn[2];
	      }
	      if (outsize >= 4)
		outt[3] = inn[3];
	    } else {
	      outt[0] = *(background);
	      if (outsize >= 3) {
		outt[1] = *(background+1);
		outt[2] = *(background+2);
	      }
	      if (outsize >= 4)
		outt[3] = *(background+3);
	    }
	  }}
      }}
  }

  template<class T> void image_interpolate_bilin(T* background, T *pin,
						 int indimi, int indimj,
						 int inmodi, int inmodj,
						 int ppi, int ppj,
						 T* out, int outsize) {
    int li0, lj0;
    register int li1, lj1;
    int deltai, ndeltai;
    int deltaj, ndeltaj;
    register T *pin00;
    register T *v00, *v01, *v10, *v11;
    li0 = ppi >> 16;
    li1 = li0+1;
    deltai = ppi & 0x0000ffff;
    ndeltai = 0x00010000 - deltai;
    lj0 = ppj  >> 16;
    lj1 = lj0+1;
    deltaj = ppj & 0x0000ffff;
    ndeltaj = 0x00010000 - deltaj;
    pin00 = (T*)(pin) + inmodi * li0 + inmodj * lj0;
    if ((li1>0)&&(li1<indimi)) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==0) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==indimi) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = background;
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else {
      v00 = background;
      v01 = background;
      v11 = background;
      v10 = background;
    }
    // TODO: this does not work for ubyte (cf ubyte implementation in image.cpp)
    // make it generic to avoid code redondancy.
    double dd = 1.0 / 65536.0;
    T d = (T) dd;
    if (sizeof (T) <= 2)
      d = 0;
    if (outsize == 1)
      *out = (ndeltaj * (( *v10*deltai + *v00*ndeltai )*d) +
	      deltaj  * (( *v11*deltai + *v01*ndeltai )*d))*d;
    else {
      if (outsize >= 3) {
	*out = (ndeltaj * (( v10[0]*deltai + v00[0]*ndeltai )*d) +
		deltaj  * (( v11[0]*deltai + v01[0]*ndeltai )*d))*d;
	*(out + 1) = (ndeltaj * (( v10[1]*deltai + v00[1]*ndeltai )*d) +
		      deltaj  * (( v11[1]*deltai + v01[1]*ndeltai )*d))*d;
	*(out + 2) = (ndeltaj * (( v10[2]*deltai + v00[2]*ndeltai )*d) +
		      deltaj  * (( v11[2]*deltai + v01[2]*ndeltai )*d))*d;
      }
      if (outsize >= 4) {
	*(out + 3) = (ndeltaj * (( v10[3]*deltai + v00[3]*ndeltai )*d) +
		      deltaj  * (( v11[3]*deltai + v01[3]*ndeltai )*d))*d;
      }
    }
  }

  template<class T> void compute_bilin_transform(idx<int> &dispi,
						 idx<int> &dispj,
						 float x1, float y1, float x2,
						 float y2, float x3, float y3,
						 float x4, float y4, float p1,
						 float q1, float p3, float q3) {
    // compute transformation matrix from coordinates
    // in target (rectangular) space to coordinates
    // in original (irregular quadrilateral) image
    // transformation matrix is in 16.16 fixed point format.
    float k = 65536 / ((p3 - p1) * (q3 - q1));
    float x41 = x4 - x1;
    float x21 = x2 - x1;
    float x43 = x4 - x3;
    float x23 = x2 - x3;
    int mx0 = (int) (k * ((q3 * x21) + (q1 * x43)));
    int mx1 = (int) (k * ((p3 * x41) + (p1 * x23)));
    int mx2 = (int) ((-k) * (x41 + x23));
    int mx3 = (int) (k * (((p3 * q3 * x1) + (p1 * q1 * x3)) -
			  ((p1 * q3 * x2) + (p3 * q1 * x4))));
    float y41 = y4 - y1;
    float y21 = y2 - y1;
    float y43 = y4 - y3;
    float y23 = y2 - y3;
    int my0 = (int) (k * ((q3 * y21) + (q1 * y43)));
    int my1 = (int) (k * ((p3 * y41) + (p1 * y23)));
    int my2 = (int) ((-k) * (y41 + y23));
    int my3 = (int) (k * (((p3 * q3 * y1) + (p1 * q1 * y3)) -
			  ((p1 * q3 * y2) + (p3 * q1 * y4))));
    int q = 0, p = 0;
    { idx_bloop2(ispi, dispi, int, ispj, dispj, int) {
	p = 0;
	{ idx_bloop2(di, ispi, int, dj, ispj, int) {
	    di.set(my0 * p + my1 * q + my2 * p * q + my3);
	    dj.set(mx0 * p + mx1 * q + mx2 * p * q + mx3);
	    p++;
	  }}
	q++;
      }}
  }

  template<class T> void image_rotscale(idx<T> &src, idx<T> &out,
					double sx, double sy, double dx,
					double dy, double angle, double coeff,
					idx<ubyte> &bg){
    double q = 1000;
    double coeff_inv = 1/coeff;
    double sa = q*sin(angle * 0.017453292);
    double ca = q*cos(angle * 0.017453292);
    double ca_plus_sa = coeff_inv * (sa + ca);
    double ca_minus_sa = coeff_inv * (ca - sa);
    float x1 = sx - ca_plus_sa;
    float y1 = sy - ca_minus_sa;
    float x2 = sx + ca_minus_sa;
    float y2 = sy - ca_plus_sa;
    float x3 = sx + ca_plus_sa;
    float y3 = sy + ca_minus_sa;
    float x4 = sx - ca_minus_sa;
    float y4 = sy + ca_plus_sa;
    float p1 = dx-q;
    float q1 = dy-q;
    float p3 = dx + q;
    float q3 = dy + q;
    image_warp_quad(src, out, bg, 1, x1, y1, x2, y2, x3, y3, x4, y4, p1, q1, p3, q3);
  }



  template<class T> void image_draw_box(idx<T> &img, T val,
					unsigned int x, unsigned int y, unsigned int dx, unsigned int dy) {
    idx_checkorder1(img, 2);
    for (unsigned int i = x; i < x + dx; ++i) {
      img.set(val, i, y);
      img.set(val, i, y + dy);
    }
    for (unsigned int j = y; j < y + dy; ++j) {
      img.set(val, x, j);
      img.set(val, x + dx, j);
    }
  }

  ////////////////////////////////////////////////////////////////
  // I/O

  template<class T> bool pnm_fread_into_rgbx(const char *fname, idx<T> &out) {
    idx<ubyte> tmp(1,1,1);
    bool ret = pnm_fread_into_rgbx(fname, tmp);
    out.resize(tmp.dim(0), tmp.dim(1), tmp.dim(2));
    idx_copy(tmp, out);
    return ret;
  }

  template<class T>
  bool image_read_rgbx(const char *fname, idx<T> &out) {
    idx<ubyte> tmp(1,1,1);
    bool ret = image_read_rgbx(fname, tmp);
    out.resize(tmp.dim(0), tmp.dim(1), tmp.dim(2));
    idx_copy(tmp, out);
    return ret;
  }

  ////////////////////////////////////////////////////////////////
  // Filters

  template<class T>
  idx<T> create_mexican_hat(double s, int n){
    idx<T> m(n, n);
    T vinv = 1/(s*s);
    T total = 0;
    int cx = n/2;
    int cy = n/2;
    for(int x = 0; x < n; x++){
      for(int y = 0; y < n; y++){
	int dx = x - cx;
	int dy = y - cy;
	m.set(-exp(-sqrt(vinv*(dx*dx + dy*dy))), x, y);
	total += m.get(x, y);
      }
    }
    //! set center valus so it's zero sum
    m.set(m.get(cx, cy) - total, cx, cy);
    //! normalize so that energy is 1
    T energy = sqrt(idx_sumsqr(m));
    idx_dotc(m, 1/energy, m);
    return m;
  }

  template<class T>
  void image_mexican_filter(idx<T> &in, idx<T> &out, double s, int n,
			    int lnorm_size,
			    idx<T> *filter_, idx<T> *tmp_) {
    idx<T> filter = filter_ ? *filter_ : create_mexican_hat<T>(s, n);
    idxdim d(in);
    idx<T> tmp = tmp_ ? *tmp_ : idx<T>(d);
    idx_checkorder3(in, 2, filter, 2, out, 2);
    image_apply_filter(in, out, filter, &tmp);
    image_local_normalization(out, out, lnorm_size); 
  }

  // TODO: handle empty sides
  // TODO: check for tmp size incompatibilities
  template<class T>
  void image_local_normalization(idx<T> &in, idx<T> &out, int n) {
    idx_std_normalize(in); // zero-mean and divide by standard deviation
    //    idx_copy(in, out);
  }

//     // TODO: handle empty sides
//   // TODO: check for tmp size incompatibilities
//   template<class T>
//   void image_local_normalization(idx<T> &in, idx<T> &out, int n) {
//     T mean;
//     T coeff;
//     idx<T> tmp(in.dim(0) + 2 * floor(n / 2),
// 	       in.dim(1) + 2 * floor(n / 2));
//     idx<T> tmp2 = tmp.narrow(0, in.dim(0), floor(n / 2));
//     tmp2 = tmp2.narrow(1, in.dim(1), floor(n / 2));
//     idx_copy(in, tmp2);
//     tmp = tmp.unfold(0, n, 1);
//     tmp = tmp.unfold(1, n, 1);
//     idx<T> tmp3(n, n);
    
//     idx_bloop3(tm, tmp, T, ou, out, T, iin, in, T) {
//       idx_bloop3(t, tm, T, o, ou, T, iiin, iin, T) {
// 	idx_copy(t, tmp3);
// 	mean = idx_mean(tmp3);
// 	idx_addc(tmp3, -mean, tmp3);
// 	coeff = 1 / sqrt(idx_sumsqr(tmp3) / (n * n));
// 	o.set((iiin.get() - mean) * coeff);
//       }
//     }
//   }

  template<class T>
  void image_apply_filter(idx<T> &in, idx<T> &out, idx<T> &filter,
			  idx<T> *tmp_) {
    idxdim d(in);
    if ((out.dim(0) != d.dim(0)) || (out.dim(1) != d.dim(1)))
      out.resize(d);
    // compute sizes of the temporary buffer
    d.setdim(0, in.dim(0) + 2 * floor(filter.dim(0) / 2));
    d.setdim(1, in.dim(1) + 2 * floor(filter.dim(1) / 2));
    idx<T> tmp;
    if (tmp_) {
      tmp = *tmp_;
      if ((tmp.dim(0) != d.dim(0)) || (tmp.dim(1) != d.dim(1))) {
	tmp.resize(d);
	*tmp_ = tmp;
      }
    } else
      tmp = idx<T>(d);
    // copy input into temporary buffer
    idx_clear(tmp);
    idx<T> tmp2 = tmp.narrow(0, in.dim(0), floor(filter.dim(0)/2));
    tmp2 = tmp2.narrow(1, in.dim(1), floor(filter.dim(1)/2));
    idx_copy(in, tmp2);
    idx_2dconvol(tmp, filter, out);
  }
  
} // end namespace ebl

#endif /* IMAGE_HPP_ */
