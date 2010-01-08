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
#include <stdlib.h>

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
					int mode,
					rect *iregion_, rect *oregion_) {
    if (image.order() < 2) eblerror("image must have at least an order of 2.");
    // iregion is optional, set it to entire image if not given
    rect iregion(0, 0, image.dim(1), image.dim(2));
    if (iregion_)
      iregion = *iregion_;
    double ratioh = w / iregion.height;
    double ratiow = h / iregion.width;
    double ratiomin = min(ratiow, ratioh);
    // if data is not contiguous, copy it to a contiguous buffer
    idx<T> contim(image);
    if (!image.contiguousp()) {
      idxdim d(image.spec);
      idx<T> tmp(d);
      idx_copy(image, tmp);
      contim = tmp;
    }
    int imw = (int) contim.dim(1);
    int imh = (int) contim.dim(0);
    int rw = 0, rh = 0;
    double ow = 0, oh = 0;
    try {
      if ((imw == 0) || (imh == 0))
	throw "cannot have dimensions of size 0";
      // determine actual size of output image
      if ((0 == w) || (0 == h)) {
	if (0 == w) {
	  if (0 == h) throw "desired width and height cannot be both zero";
	  else	w = max(1, (int) (imw * (h / imh)));
	} else	h = max(1, (int) (imh * (w / imw)));
      }
      if (mode == 0) { // preserve aspect ratio
	ratiow = ratiomin;
	ratioh = ratiomin;
      }
      else if (mode == 1) { // possibly modify aspect ratio
	ratiow = ratiow;
	ratioh = ratioh;
      }
      else if (mode == 2) { // use w and h as scaling ratios
	ratiow = w;
	ratioh = h;
      }
      else throw "illegal mode or desired dimensions";
    } catch (const char *err) {
      cerr << "error: trying to resize image " << image;
      cerr << " to " << h << "x" << w << " with mode " << mode << endl;
      eblerror(err);
    }
    // output sizes of entire image
    ow = max(1.0, imw * ratiow);
    oh = max(1.0, imh * ratioh);
    // compute closest integer subsampling ratio
    rw = MAX(1, (int) (1 / ratiow));
    rh = MAX(1, (int) (1 / ratioh));
    // compute output region
    rect oregion(iregion.h0 * ratioh, iregion.w0 * ratiow, h, w);
    if (oregion_)
      *oregion_ = oregion;
    // subsample by integer ratio if necessary
    if ((rh > 1) || (rw > 1)) {
      contim = image_subsample(contim, rh, rw);
      imw = contim.dim(1);
      imh = contim.dim(0);
    }
    // resample from subsampled image with bilinear interpolation
    idx<T> rez((intg) oh, (intg) ow, (contim.order() == 3) ? contim.dim(2) : 1);
    rez.set_chandim(contim.get_chandim());
    idx<T> bg(4);
    idx_clear(bg);
    // the 0.5 thingies are necessary because warp-bilin interprets
    // integer coordinates as being at the center of each pixel.
    float x1 = -0.5, y1 = -0.5, x3 = imw - 0.5, y3 = imh - 0.5;
    float p1 = -0.5, q1 = -0.5, p3 = ow - 0.5, q3 = oh - 0.5;
    image_warp_quad(contim, rez, bg, 1, x1, y1, x3, y1, x3, y3, x1, y3, 
		    p1, q1, p3, q3);
    if (contim.order() == 2)
      return rez.select(2, 0);
    return rez;
  }

  template<class T>
  idx<T> image_gaussian_resize(idx<T> &im_, uint oheight, uint owidth,
			       float margin, rect *iregion_, rect *oregion) {
    idx<T> im = im_.shift_chan(0);
    // only accept 2D images or 3D with channel dim to 0.
    if ((im.order() != 2) && ((im.order() == 3) && im.get_chandim() != 0)) {
      cerr << "error: gaussian_pyramid only accepts 2D images ";
      cerr << "or 3D images with channel dimension (chandim) set to 0. ";
      cerr << "input image is " << im << " with chandim = " << im.get_chandim();
      cerr << endl;
      eblerror("unexpected image format");
    }
    // iregion is optional, set it to entire image if not given
    rect iregion(0, 0, im.dim(1), im.dim(2));
    if (iregion_)
      iregion = *iregion_;
    // if region's height and width are already within the margin below
    // oheight and owidth, then just return the current image
    // and set out_region to input region
    uint oheight0 = MAX(0, (int) oheight - oheight * margin);
    uint owidth0 = MAX(0, (int) owidth - owidth * margin);
    if ((iregion.height <= oheight) && (iregion.height >= oheight0) &&
	(iregion.width <= owidth) && (iregion.width >= owidth0)) {
      if (oregion)
	*oregion = iregion;
      return im_;
    }
    // else down/up-sample with gaussians
    gaussian_pyramid<T> gp;
    idx<T> rim;
    rect rr;
    // compute edges that need most reduction
    uint imax, omax;
    if ((iregion.height / (float)oheight) > (iregion.width / (float)owidth)) {
      imax = iregion.height;
      omax = oheight;
    } else {
      imax = iregion.width;
      omax = owidth;
    }
    // compute how many gaussian reduction/expansions necessary to reach target
    if ((iregion.height > oheight) || (iregion.width > owidth)) { // reduce
      // compute a regular resize of 1/sqrt(2) of the original size
      // to add more scales than just by a factor of 2.
      idx<T> im_sqrt2 = im.shift_chan(2); // image_resize expect chan in 2
      im_sqrt2 = image_resize(im_sqrt2, 1/sqrt(2), 1/sqrt(2), 2);
      im_sqrt2 = im_sqrt2.shift_chan(0);
      rect r_sqrt2(iregion.h0 * 1/sqrt(2), iregion.w0 * 1/sqrt(2),
		   iregion.height * 1/sqrt(2), iregion.width * 1/sqrt(2));
      // compute sqrt2 edges that need most reduction
      uint imax_sqrt2;
      if ((r_sqrt2.height / (float)oheight) < (r_sqrt2.width / (float)owidth)) {
	imax_sqrt2 = r_sqrt2.height;
      } else {
	imax_sqrt2 = r_sqrt2.width;
      }
      uint dist, dist_sqrt2;
      uint reductions = MAX(0, (int) gp.count_reductions(imax, omax, dist));
      uint reductions_sqrt2 =
	MAX(0, (int) gp.count_reductions(imax_sqrt2, omax, dist_sqrt2));
      // switch between original and sqrt2 based on distance to outwidth
      if (dist > dist_sqrt2) { // sqrt2 scale is closer to target
	rim = gp.reduce(im_sqrt2, reductions_sqrt2);
	rr = gp.reduce_rect(r_sqrt2, reductions_sqrt2);
      } else { // original scale is closer to target
	// reduce image reductions time
	rim = gp.reduce(im, reductions);
	rr = gp.reduce_rect(iregion, reductions);
      }
    } else { // expand
      // compute a regular resize of 1/sqrt(2) of the original size
      // to add more scales than just by a factor of 2.
      idx<T> im_sqrt2 = im.shift_chan(2); // image_resize expect chan in 2
      im_sqrt2 = image_resize(im_sqrt2, sqrt(2), sqrt(2), 2);
      im_sqrt2 = im_sqrt2.shift_chan(0);
      rect r_sqrt2(iregion.h0 * sqrt(2), iregion.w0 * sqrt(2),
		   iregion.height * sqrt(2), iregion.width * sqrt(2));
      // compute sqrt2 edges that need most reduction
      uint imax_sqrt2;
      if ((r_sqrt2.height / (float)oheight) < (r_sqrt2.width / (float)owidth)) {
	imax_sqrt2 = r_sqrt2.height;
      } else {
	imax_sqrt2 = r_sqrt2.width;
      }
      uint dist = 0, dist_sqrt2 = 0;
      // number of expansions to stay below outwidth
      uint expansions =
	MAX(0, (int) gp.count_expansions(imax, omax, dist));
      uint expansions_sqrt2 =
	MAX(0, (int) gp.count_expansions(imax_sqrt2, omax, dist_sqrt2));
      // switch between original and sqrt2 based on distance to outwidth
      if (dist > dist_sqrt2) { // sqrt2 scale is closer to target
	rim = gp.expand(im_sqrt2, expansions_sqrt2);
	rr = gp.expand_rect(r_sqrt2, expansions_sqrt2);
      } else { // original scale is closer to target
	// expand
	rim = gp.expand(im, expansions);
	rr = gp.expand_rect(iregion, expansions);
      }
    }
    // save resized input region into oregion, if defined
    if (oregion)
      *oregion = rr;
    // put channel dim back to dimension 2
    rim = rim.shift_chan(2);
    return rim;
  }

  template<class T> 
  idx<T> image_region_to_rect(idx<T> &im, const rect &r, uint oheight,
			      uint owidth, rect &cropped) {
    // TODO: check expecting 2D or 3D
    // TODO: check that rectangle is within image
    idxdim d(im);
    uint dh = (d.get_chandim() == 0) ? 1 : 0; // handle channels position
    uint dw = (d.get_chandim() == 0) ? 2 : 1; // handle channels position
    d.setdim(dh, oheight);
    d.setdim(dw, owidth);
    idx<T> res(d);

    int hcenter = r.h0 + r.height / 2; // input height center
    int wcenter = r.w0 + r.width / 2; // input width center
    // limit centers to half the width/height away from borders
    // to handle incorrect regions
    hcenter = MIN((int)im.dim(dh)-1 - (int)r.height/2,
		  MAX((int)r.height/2, hcenter));
    wcenter = MIN((int)im.dim(dw)-1 - (int)r.width/2,
		  MAX((int)r.width/2, wcenter));
    int h0 = hcenter - oheight / 2; // out height offset in input
    int w0 = wcenter - owidth / 2; // out width offset in input
    int h1 = hcenter + oheight / 2;
    int w1 = wcenter + owidth / 2;
    int gh0 = MAX(0, MIN((int) im.dim(dh)-1, (int) h0)); // input h offset
    int gw0 = MAX(0, MIN((int) im.dim(dw)-1, (int) w0)); // input w offset
    int gh1 = MAX(0, MIN((int) im.dim(dh)-1, (int) h1));
    int gw1 = MAX(0, MIN((int) im.dim(dw)-1, (int) w1));
    int h = gh1 - gh0; // out height narrow
    int w = gw1 - gw0; // out width narrow
    int fh0 = MAX(0, gh0 - h0); // out height offset narrow
    int fw0 = MAX(0, gw0 - w0); // out width offset narrow

    idx<T> tmpres = res.narrow(dh, h, fh0);
    tmpres = tmpres.narrow(dw, w, fw0);
    idx<T> tmpim = im.narrow(dh, h, gh0);
    tmpim = tmpim.narrow(dw, w, gw0);
    idx_clear(res);
    idx_copy(tmpim, tmpres);
    // set cropped rectangle to region in the output image containing input
    cropped.h0 = fh0;
    cropped.w0 = fw0;
    cropped.height = h;
    cropped.width = w;
    return res;
  }
  
  template<class T> 
  idx<T> image_region_to_square(idx<T> &im, const rect &r) {
    // TODO: check expecting 2D or 3D
    // TODO: check that rectangle is within image
    uint sz = MAX(r.height, r.width);
    idxdim d(im);
    uint dh = (d.get_chandim() == 0) ? 1 : 0; // handle channels position
    uint dw = (d.get_chandim() == 0) ? 2 : 1; // handle channels position
    d.setdim(dh, sz);
    d.setdim(dw, sz);
    idx<T> res(d);
    uint tmp_h = MIN(sz, r.height);
    uint tmp_w = MIN(sz, r.width);
    idx<T> tmpres = res.narrow(dh, tmp_h, res.dim(dh) / 2 - tmp_h / 2);
    tmpres = tmpres.narrow(dw, tmp_w, res.dim(dw) / 2 - tmp_w / 2);
    idx<T> tmpim = im.narrow(dh, tmp_h, r.h0);
    tmpim = tmpim.narrow(dw, tmp_w, r.w0);
    idx_clear(res);
    idx_copy(tmpim, tmpres);
    return res;
  }
  
  template<class T> 
  idx<ubyte> image_to_ubyte(idx<T> &im, double zoomh, double zoomw,
			    T minv, T maxv) {
    // check the order and dimensions
    if ((im.order() < 2) || (im.order() > 3) || 
	((im.order() == 3) && (im.dim(2) != 1) && (im.dim(2) != 3))) {
      cerr << "converting image_to_ubyte with dims " << im << " failed." <<endl;
      eblerror("expecting a 2D idx or a 3D idx with 1 or 3 channels only");
    }
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
    if (minv == maxv) { // if minv still == maxv, use 0 and 1
      minv = 0;
      maxv = 1;
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
    out.set_chandim(in.get_chandim()); // preserve channels dimension info
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

  template<class T>
  void image_warp_quad(idx<T> &in, idx<T> &out,
		       idx<T> &background, int mode,
		       float x1, float y1, float x2, float y2,
		       float x3, float y3, float x4, float y4,
		       float p1, float q1, float p3, float q3) {
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


//   template<class T> void compute_bilin_transform2(idx<int> &dispi,
// 						  idx<int> &dispj,
// 						  float x1, float y1,
// 						  float x2, float y2,
// 						  float x3, float y3,
// 						  float x4, float y4,
// 						  float p1, float q1,
// 						  float p2, float q2,
// 						  float p3, float q3,
// 						  float p4, float q4) {
//     // compute transformation matrix from coordinates
//     // in target (rectangular) space to coordinates
//     // in original (irregular quadrilateral) image
//     // transformation matrix is in 16.16 fixed point format.
//     float k = 65536 / ((p3 - p1) * (q3 - q1));
//     float x41 = x4 - x1;
//     float x21 = x2 - x1;
//     float x43 = x4 - x3;
//     float x23 = x2 - x3;
//     int mx0 = (int) (k * ((q3 * x21) + (q1 * x43)));
//     int mx1 = (int) (k * ((p3 * x41) + (p1 * x23)));
//     int mx2 = (int) ((-k) * (x41 + x23));
//     int mx3 = (int) (k * (((p3 * q3 * x1) + (p1 * q1 * x3)) -
// 			  ((p1 * q3 * x2) + (p3 * q1 * x4))));
//     float y41 = y4 - y1;
//     float y21 = y2 - y1;
//     float y43 = y4 - y3;
//     float y23 = y2 - y3;
//     int my0 = (int) (k * ((q3 * y21) + (q1 * y43)));
//     int my1 = (int) (k * ((p3 * y41) + (p1 * y23)));
//     int my2 = (int) ((-k) * (y41 + y23));
//     int my3 = (int) (k * (((p3 * q3 * y1) + (p1 * q1 * y3)) -
// 			  ((p1 * q3 * y2) + (p3 * q1 * y4))));
//     int q = 0, p = 0;
//     { idx_bloop2(ispi, dispi, int, ispj, dispj, int) {
// 	p = 0;
// 	{ idx_bloop2(di, ispi, int, dj, ispj, int) {
// 	    di.set(my0 * p + my1 * q + my2 * p * q + my3);
// 	    dj.set(mx0 * p + mx1 * q + mx2 * p * q + mx3);
// 	    p++;
// 	  }}
// 	q++;
//       }}
//   }

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

  template<class T> void pnm_fread_into_rgbx(const char *fname, idx<T> &out) {
    idx<ubyte> tmp(1,1,1);
    pnm_fread_into_rgbx(fname, tmp);
    out.resize(tmp.dim(0), tmp.dim(1), tmp.dim(2));
    idx_copy(tmp, out);
  }

  template<class T>
  bool image_read_rgbx(const char *fname, idx<T> &out) {
    idx<ubyte> tmp(1,1,1);
    bool ret = image_read_rgbx(fname, tmp);
    out.resize(tmp.dim(0), tmp.dim(1), tmp.dim(2));
    idx_copy(tmp, out);
    return ret;
  }

  template<class T>
  bool load_image(const char *fname, idx<T> &out) {
    return image_read_rgbx(fname, out);
  }

  template<class T>
  idx<T> load_image(const char *fname) {
    idx<T> out(1,1,1);
    if (!image_read_rgbx(fname, out))
      throw "load_image failed";
    return out;
  }

  template<class T>
  idx<T> load_image(const string &fname) {
    idx<T> out(1,1,1);
    if (!image_read_rgbx(fname.c_str(), out))
      throw "load_image failed";
    return out;
  }

  template<class T>
  bool save_image_ppm(const string &fname, idx<T> &in) {
    // check order
    // TODO: support grayscale
    if (in.order() != 3) {
      cerr << "error: image order (" << in.order() << " not supported." << endl;
      return false;
    }
    // save as ppm
    FILE *fp = fopen(fname.c_str(), "wb");
    if (!fp) {
      cerr << "error: failed to open file " << fname << endl;
      return false;
    }
    fprintf(fp,"P6 %d %d 255\n", (int) in.dim(1), (int) in.dim(0));
    idx_bloop1(inn, in, ubyte) {
      idx_bloop1(innn, inn, ubyte) {
	fputc(innn.get(0), fp);
	fputc(innn.get(1), fp);
	fputc(innn.get(2), fp);
      }
    }
    fclose(fp);
    return true;
  }

  template<class T>
  bool save_image_jpg(const string &fname, idx<T> &in) {
    return save_image(fname, in, "JPG");
  }

  template<class T>
  bool save_image(const string &fname, idx<T> &in, const char *format) {
    // save as ppm
    string fname2 = fname;
    fname2 += ".ppm";
    if (!save_image_ppm(fname2, in))
      return false;
    // convert ppm to jpg
    string cmd = "convert PPM:";
    cmd += fname2;
    cmd += " ";
    cmd += format;
    cmd += ":";
    cmd += fname;
    int n = ::system(cmd.c_str());
    if (n != 0) {
      cerr << "error (" << n << "): failed to save image " << fname;
      cerr << " to format " << format << endl;
      return false;
    }
    remove(fname2.c_str());
    return true;
  }

  ////////////////////////////////////////////////////////////////
  // Filters

  // TODO: cleanup
  template<class T>
  idx<T> create_mexican_hat(double s, int n) {
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

  // TODO: cleanup
  template<class T>
  idx<T> create_gaussian_kernel(int n) {
    idx<T> m(n, n);
    double s = n/4;
    T vinv = 1/(s*s);
    T total = 0;
    int cx = n/2;
    int cy = n/2;
    for(int x = 0; x < n; x++){
      for(int y = 0; y < n; y++){
	int dx = x - cx;
	int dy = y - cy;
	m.set(-exp(-(vinv*(dx*dx + dy*dy))), x, y);
	total += m.get(x, y);
      }
    }
    //! set center valus so it's zero sum
    //    m.set(m.get(cx, cy) - total, cx, cy);
    //! normalize so that energy is 1
    //    T energy = sqrt(idx_sumsqr(m));
    idx_dotc(m, 1/total, m);
    return m;
  }

  // TODO: cleanup
  template<class T>
  idx<T> create_gaussian_kernel(uint h, uint w) {
    idx<T> m(h, w);
    uint min = MIN(h, w); // use smallest dim for gaussian
    double s = min/4;
    T vinv = 1/(s*s);
    T total = 0;
    int cx = min/2;
    int cy = min/2;
    for(uint x = 0; x < h; x++){
      for(uint y = 0; y < w; y++){
	int dx = x - cx;
	int dy = y - cy;
	m.set(-exp(-(vinv*(dx*dx + dy*dy))), x, y);
	total += m.get(x, y);
      }
    }
    //! set center valus so it's zero sum
    //    m.set(m.get(cx, cy) - total, cx, cy);
    //! normalize so that energy is 1
    //    T energy = sqrt(idx_sumsqr(m));
    idx_dotc(m, 1/total, m);
    return m;
  }

  template<class T>
  void image_mexican_filter(idx<T> &in, idx<T> &out, double s, int n,
			    idx<T> *filter_, idx<T> *tmp_) {
    idx<T> filter = filter_ ? *filter_ : create_mexican_hat<T>(s, n);
    idxdim d(in);
    idx<T> tmp = tmp_ ? *tmp_ : idx<T>(d);
    idx_checkorder3(in, 2, filter, 2, out, 2);
    image_apply_filter(in, out, filter, &tmp);
  }

  // TODO: handle empty sides
  // TODO: check for tmp size incompatibilities
  // TODO: THIS ASSUMES DATA IS IN LAST DIMENSION. MAKE IT GENERIC 
  template<class T>
  void image_global_normalization(idx<T> &in) {
    switch (in.order()) {
    case 2:
      idx_std_normalize(in); // zero-mean and divide by standard deviation
      break ;
    case 3:
      // normalize layer by layer
      if (in.get_chandim() == 0) { // data in 1st dimension
	idx_bloop1(i, in, T) {
	  idx_std_normalize(i); // zero-mean and divide by standard deviation
	}
      } else {
	idx_eloop1(i, in, T) {
	  idx_std_normalize(i); // zero-mean and divide by standard deviation
	}
      }
      break ;
    default:
      eblerror("image_global_normalization: dimension not implemented");
    }
  }

  // TODO: handle empty sides
  // TODO: check for tmp size incompatibilities
  // TODO: cleanup
  template<class T>
  void image_local_normalization(idx<T> &in, idx<T> &out, int n) {
    // 1. create normalized gaussian kernel (kernel / sum(kernel))
    idx<T> kernel = create_gaussian_kernel<T>(n);
    idx<T> tmp(in.dim(0) + n - 1, in.dim(1) + n - 1);
    idx<T> tmp2 = tmp.narrow(0, in.dim(0), floor(n / 2));
    tmp2 = tmp2.narrow(1, in.dim(1), floor(n / 2));
    idx<T> tmp3(n, n);
    idxdim d(in);
    idx<T> tmp4(d);
    idx<T> tmp5(d);

    // sum_j (w_j * in_j)
    idx<T> out1 = image_filter(in, kernel);
    idx<T> in2 = in.narrow(0, out1.dim(0), floor(kernel.dim(0)/2));
    in2 = in2.narrow(1, out1.dim(1), floor(kernel.dim(1)/2));
    idxdim outd(out1);
    idx<T> out2(outd);
    // in - mean
    idx_sub(in2, out1, out1);
    // (in - mean)^2
    idx_mul(out1, out1, out2);
    // sum_j (w_j * (in - mean)^2)
    idx<T> out3 = image_filter(out2, kernel);
    // sqrt(sum_j (w_j (in - mean)^2))
    idx_sqrt(out3, out3);    
    // std(std < 1) = 1
    idx_threshold(out3, (T)1.0, out3);
    // 1/std
    idx_inv(out3, out3);
    // out = (in - mean) / std
    idx<T> out4 = out1.narrow(0, out3.dim(0), floor(kernel.dim(0)/2));
    out4 = out4.narrow(1, out3.dim(1), floor(kernel.dim(1)/2));
    idx_mul(out4, out3, out3);
    // finally copy result centered on an image the same size as in
    idx<T> out5 = out.narrow(0, out3.dim(0), (out.dim(0) - out3.dim(0)) / 2);
    out5 = out5.narrow(1, out3.dim(1), (out.dim(1) - out3.dim(1)) / 2);
    idx_clear(out);
    idx_copy(out3, out5);
    
//     // in - mean
//     idx_sub(in, tmp5, tmp5);
//     // (in - mean)^2
//     idx_mul(tmp5, tmp5, tmp4);
//     // sum_j (w_j * (in - mean)^2)
//     image_apply_filter(tmp4, out, kernel, &tmp);
//     // sqrt(sum_j (w_j (in - mean)^2))
//     idx_sqrt(out, out);    
//     // std(std < 1) = 1
//     idx_threshold(out, (T)1.0, out);
//     // 1/std
//     idx_inv(out, out);
//     // out = (in - mean) / std
//     idx_mul(tmp5, out, out);
  }

  // TODO: get rid of this function
  template<class T>
  void image_apply_filter(idx<T> &in, idx<T> &out, idx<T> &filter,
			  idx<T> *tmp_) {
    idxdim d(in);
    if ((out.dim(0) != d.dim(0)) || (out.dim(1) != d.dim(1)))
      out.resize(d);
    idx_clear(out);
    idx<T> tmp = out.narrow(0, in.dim(0) - filter.dim(0) + 1,
			    floor(filter.dim(0)/2));
    tmp = tmp.narrow(1, in.dim(1) - filter.dim(1) + 1,
		     floor(filter.dim(1)/2));
    idx_2dconvol(in, filter, tmp);   
    
//     // compute sizes of the temporary buffer
//     d.setdim(0, in.dim(0) + filter.dim(0) - 1);
//     d.setdim(1, in.dim(1) + filter.dim(1) - 1);
//     idx<T> tmp;
//     if (tmp_) {
//       tmp = *tmp_;
//       if ((tmp.dim(0) != d.dim(0)) || (tmp.dim(1) != d.dim(1))) {
// 	tmp.resize(d);
// 	*tmp_ = tmp;
//       }
//     } else
//       tmp = idx<T>(d);
//     // copy input into temporary buffer
//     idx_clear(tmp);
//     idx<T> tmp2 = tmp.narrow(0, in.dim(0), floor(filter.dim(0)/2));
//     tmp2 = tmp2.narrow(1, in.dim(1), floor(filter.dim(1)/2));
//     idx_copy(in, tmp2);
//     idx_2dconvol(tmp, filter, out);
  }
  
  template<class T>
  idx<T> image_filter(idx<T> &in, idx<T> &filter) {
    // check that image is bigger than filter
    if ((in.dim(0) < filter.dim(0)) ||
	(in.dim(1) < filter.dim(1))) {
      cerr << "error: image " << in << " is too small to be convolved with ";
      cerr << filter << " filter." << endl;
      eblerror("too small image for convolution");
    }
    idxdim d(in);
    d.setdim(0, in.dim(0) - filter.dim(0) + 1);
    d.setdim(1, in.dim(1) - filter.dim(1) + 1);
    idx<T> out(d);
    idx_clear(out);
    idx_2dconvol(in, filter, out);
    return out;
  }
  
  ////////////////////////////////////////////////////////////////
  // Deformations

  template<class T>
  void image_deformation_ranperspective(idx<T> &in, idx<T> &out,
					int hrange, int wrange, T background) {
    idx<float> bg(1);
    bg.set(background, 0);
    float x1diff = (float) drand( wrange/2, -wrange/2);
    float y1diff = (float) drand( hrange/2, -hrange/2);
    float x2diff = (float) drand(-wrange/2,  wrange/2);
    float y2diff = (float) drand( hrange/2, -hrange/2);
    float x3diff = (float) drand(-wrange/2,  wrange/2);
    float y3diff = (float) drand(-hrange/2,  hrange/2);
    float x4diff = (float) drand( wrange/2, -wrange/2);
    float y4diff = (float) drand(-hrange/2,  hrange/2);
    image_warp_quad(in, out, bg, 1,
		    -.5 + x1diff, -.5 + y1diff, // x1 y1
		    in.dim(1) -.5 + x2diff, -.5 + y2diff, // x2 y2
		    in.dim(1) -.5 + x3diff, in.dim(0) -.5 + y3diff, // x3 y3
		    -.5 + x4diff, in.dim(0) - .5 + y4diff, // x4 y4
		    -.5, -.5, in.dim(1) - .5, in.dim(0) - .5);
  }

} // end namespace ebl

#endif /* IMAGE_HPP_ */
