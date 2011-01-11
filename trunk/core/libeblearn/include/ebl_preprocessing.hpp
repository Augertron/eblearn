/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
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

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // channorm_module

  template <typename T, class Tstate>
  channorm_module<T,Tstate>::channorm_module(uint normalization_size_, 
					     const char *name_)
    : module_1_1<T,Tstate>(name_), normalization_size(normalization_size_), 
      tmp(1,1,1),
      norm(normalization_size, normalization_size, 1, 
	   "norm", true, false, true) {
  }

  template <typename T, class Tstate>
  channorm_module<T,Tstate>::~channorm_module() {
  }

  template <typename T, class Tstate>
  void channorm_module<T,Tstate>::resize_output(Tstate &in, Tstate &out,
					 int dim0) {
    // resize output based on input dimensions
    idxdim d(in.x);
    if (dim0 > 0)
      d.setdim(0, dim0);
    if (d != out.x.get_idxdim()) {
#ifdef __DEBUG__
      cout << "channorm_module: resizing output from " << out.x.get_idxdim();
      cout << " to " << d << endl;
#endif
      out.x.resize(d); // resize only x, as bprop and bbprop are not defined
    }
    // resize temporary buffer
    if (d != tmp.x.get_idxdim())
      tmp.x.resize(d);
  }
  
  ////////////////////////////////////////////////////////////////
  // rgb_to_ypuv_module

  template <typename T, class Tstate>
  rgb_to_ypuv_module<T,Tstate>::rgb_to_ypuv_module(uint normalization_size)
    : channorm_module<T,Tstate>(normalization_size, "rgb_to_ypuv") {
  }

  template <typename T, class Tstate>
  rgb_to_ypuv_module<T,Tstate>::~rgb_to_ypuv_module() {
  }

  template <typename T, class Tstate>
  void rgb_to_ypuv_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    if (in.x.dim(0) != 3) {
      // cerr << "warning: in rgb_to_ypuv, input is not 3-channel, "
      // 	   << "ignoring color." << endl;
    } else {
      idx<T> uv, yuv;
      
      // RGB to YUV
      idx_eloop2(inx, in.x, T, outx, out.x, T) {
	idx_eloop2(inxx, inx, T, outxx, outx, T) {
	  rgb_to_yuv_1D(inxx, outxx);
	}
      }
      // bring UV between -1 and 1
      uv = out.x.narrow(0, 2, 1);
      idx_addc(uv, (T)-128, uv);
      idx_dotc(uv, (T).01, uv);
    }
    // convert Y to Yp
    this->tmp.x = out.x.narrow(0, 1, 0);
    this->norm.fprop(this->tmp, this->tmp); // local
  }

  template <typename T, class Tstate>
  rgb_to_ypuv_module<T,Tstate>* rgb_to_ypuv_module<T,Tstate>::copy() {
    return new rgb_to_ypuv_module<T,Tstate>(this->normalization_size);
  }
  
  ////////////////////////////////////////////////////////////////
  // rgb_to_yp_module

  template <typename T, class Tstate>
  rgb_to_yp_module<T,Tstate>::rgb_to_yp_module(uint normalization_size)
    : channorm_module<T,Tstate>(normalization_size, "rgb_to_yp") {
  }

  template <typename T, class Tstate>
  rgb_to_yp_module<T,Tstate>::~rgb_to_yp_module() {
  }

  template <typename T, class Tstate>
  void rgb_to_yp_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out, 1); // resize (iff necessary)
    if (in.x.dim(0) != 3) {
      // cerr << "warning: in rgb_to_yp, input is not 3-channel, "
      // 	   << "ignoring color." << endl;
      // convert Y to Yp
      this->norm.fprop(in, out); // local
    } else {
      // RGB to YUV
      idx_eloop2(inx, in.x, T, tmpx, this->tmp.x, T) {
	idx_eloop2(inxx, inx, T, tmpxx, tmpx, T) {
	  rgb_to_y_1D(inxx, tmpxx);
	}
      }
      // convert Y to Yp
      this->norm.fprop(this->tmp, out); // local
    }
  }

  template <typename T, class Tstate>
  rgb_to_yp_module<T,Tstate>* rgb_to_yp_module<T,Tstate>::copy() {
    return new rgb_to_yp_module<T,Tstate>(this->normalization_size);
  }
    
  ////////////////////////////////////////////////////////////////
  // y_to_yp_module

  template <typename T, class Tstate>
  y_to_yp_module<T,Tstate>::y_to_yp_module(uint normalization_size)
    : channorm_module<T,Tstate>(normalization_size, "y_to_yp") {
  }

  template <typename T, class Tstate>
  y_to_yp_module<T,Tstate>::~y_to_yp_module() {
  }

  template <typename T, class Tstate>
  void y_to_yp_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->norm.fprop(in, out); // local
  }

  template <typename T, class Tstate>
  y_to_yp_module<T,Tstate>* y_to_yp_module<T,Tstate>::copy() {
    return new y_to_yp_module<T,Tstate>(this->normalization_size);
  }
    
  ////////////////////////////////////////////////////////////////
  // bgr_to_ypuv_module

  template <typename T, class Tstate>
   bgr_to_ypuv_module<T,Tstate>::bgr_to_ypuv_module(uint normalization_size)
     : channorm_module<T,Tstate>(normalization_size, "bgr_to_ypuv") {
  }

  template <typename T, class Tstate>
  bgr_to_ypuv_module<T,Tstate>::~bgr_to_ypuv_module() {
  }

  template <typename T, class Tstate>
  void bgr_to_ypuv_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    idx<T> uv, yp, yuv;

    // BGR to YUV
    idx_eloop2(inx, in.x, T, outx, out.x, T) {
      idx_eloop2(inxx, inx, T, outxx, outx, T) {
	bgr_to_yuv_1D(inxx, outxx);
      }
    }
    // bring UV between -1 and 1
    uv = out.x.narrow(0, 2, 1);
    idx_addc(uv, (T)-128, uv);
    idx_dotc(uv, (T).01, uv);
    // convert Y to Yp
    this->tmp.x = out.x.narrow(0, 1, 0);
    this->norm.fprop(this->tmp, this->tmp); // local
  }

  template <typename T, class Tstate>
  bgr_to_ypuv_module<T,Tstate>* bgr_to_ypuv_module<T,Tstate>::copy() {
    return new bgr_to_ypuv_module<T,Tstate>(this->normalization_size);
  }
  
  ////////////////////////////////////////////////////////////////
  // bgr_to_yp_module

  template <typename T, class Tstate>
  bgr_to_yp_module<T,Tstate>::bgr_to_yp_module(uint normalization_size)
    : channorm_module<T,Tstate>(normalization_size, "bgr_to_yp") {
  }

  template <typename T, class Tstate>
  bgr_to_yp_module<T,Tstate>::~bgr_to_yp_module() {
  }

  template <typename T, class Tstate>
  void bgr_to_yp_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out, 1); // resize (iff necessary)
    // BGR to YUV
    idx_eloop2(inx, in.x, T, tmpx, this->tmp.x, T) {
      idx_eloop2(inxx, inx, T, tmpxx, tmpx, T) {
	bgr_to_y_1D(inxx, tmpxx);
      }
    }
    // convert Y to Yp
    this->norm.fprop(this->tmp, out); // local
  }

  template <typename T, class Tstate>
  bgr_to_yp_module<T,Tstate>* bgr_to_yp_module<T,Tstate>::copy() {
    return new bgr_to_yp_module<T,Tstate>(this->normalization_size);
  }
       
  ////////////////////////////////////////////////////////////////
  // rgb_to_hp_module

  template <typename T, class Tstate>
  rgb_to_hp_module<T,Tstate>::rgb_to_hp_module(uint normalization_size)
    : channorm_module<T,Tstate>(normalization_size, "rgb_to_hp") {
  }

  template <typename T, class Tstate>
  rgb_to_hp_module<T,Tstate>::~rgb_to_hp_module() {
  }

  template <typename T, class Tstate>
  void rgb_to_hp_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out, 1); // resize (iff necessary)
    // RGB to YUV
    idx_eloop2(inx, in.x, T, tmpx, this->tmp.x, T) {
      idx_eloop2(inxx, inx, T, tmpxx, tmpx, T) {
	rgb_to_h_1D(inxx, tmpxx);
      }
    }
    // convert H to Hp
    this->norm.fprop(this->tmp, out); // local
  }
 
  template <typename T, class Tstate>
  rgb_to_hp_module<T,Tstate>* rgb_to_hp_module<T,Tstate>::copy() {
    return new rgb_to_hp_module<T,Tstate>(this->normalization_size);
  }
     
  ////////////////////////////////////////////////////////////////
  // resizepp_module

  template <typename T, class Tstate>
  resizepp_module<T,Tstate>::
  resizepp_module(intg height_, intg width_, uint mode_,
		  module_1_1<T,Tstate> *pp_, uint kernelsz_, bool own_pp_,
		  uint hzpad_, uint wzpad_)
    : module_1_1<T,Tstate>("resizepp"), 
      pp(pp_), own_pp(own_pp_), kernelsz(kernelsz_), inpp(1,1,1), outpp(1,1,1),
      tmp3(1,1,1), mode(mode_), inrect(0, 0, 0, 0), inrect_set(false),
      outrect_set(false), hzpad(hzpad_), wzpad(wzpad_), zpad(NULL),
      hjitter(0), wjitter(0), sjitter(1.0), rjitter(0.0) {
    set_dimensions(height_, width_);
    set_zpads(hzpad_, wzpad_);
  }
  
  template <typename T, class Tstate>
  resizepp_module<T,Tstate>::
  resizepp_module(uint mode_, module_1_1<T,Tstate> *pp_, uint kernelsz_,
		  bool own_pp_, uint hzpad_, uint wzpad_)
    : pp(pp_), own_pp(own_pp_), kernelsz(kernelsz_), height(0), width(0),
      inpp(1,1,1), outpp(1,1,1), tmp3(1,1,1), mode(mode_), inrect(0, 0, 0, 0),
      inrect_set(false), outrect_set(false), hzpad(hzpad_), wzpad(wzpad_),
      zpad(NULL), hjitter(0), wjitter(0), sjitter(1.0), rjitter(0.0) {
    set_zpads(hzpad_, wzpad_);
  }
  
  template <typename T, class Tstate>
  resizepp_module<T,Tstate>::~resizepp_module() {
    if (pp && own_pp)
      delete pp;
    if (zpad)
      delete zpad;
  }
  
  template <typename T, class Tstate>
  void resizepp_module<T,Tstate>::set_dimensions(intg height_, intg width_) {
    height = height_ - hzpad * 2;
    width = width_ - wzpad * 2;
  }

  template <typename T, class Tstate>
  void resizepp_module<T,Tstate>::set_zpads(intg hpad, intg wpad) {
    // reset height/width without current zpad
    height += hzpad * 2;
    width += wzpad * 2;
    // update zpads and height/width
    hzpad = hpad;
    wzpad = wpad;
    height -= hzpad * 2;
    width -= wzpad * 2;
    // update zpad module
    if (zpad)
      delete zpad;
    if (hzpad > 0 || wzpad > 0)
      zpad = new zpad_module<T,Tstate>(hzpad, wzpad);
  }

  template <typename T, class Tstate>
  void resizepp_module<T,Tstate>::set_jitter(int h, int w, float s, float r) {
    hjitter = h;
    wjitter = w;
    sjitter = s;
    rjitter = r;
  }
  
  template <typename T, class Tstate>
  void resizepp_module<T,Tstate>::set_input_region(const rect<int> &inr) {
    inrect = inr;
    inrect_set = true;
  }

  template <typename T, class Tstate>
  void resizepp_module<T,Tstate>::set_output_region(const rect<int> &outr) {
    outrect = outr;
    outrect.height -= hzpad * 2;
    outrect.width -= wzpad * 2;
    outrect_set = true;
  }

  template <typename T, class Tstate>
  void resizepp_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    // set input region to entire image if no input region is given
    if (!inrect_set)
      inrect = rect<int>(0, 0, in.x.dim(1), in.x.dim(2));
    // apply scale jitter (keeping same center)
    if (sjitter != 1.0)
      inrect.scale_centered(sjitter, sjitter);
    if (!outrect_set)
      outrect = rect<int>(0, 0, height, width);
    rect<int> outr;
    // resize input while preserving aspect ratio
    tmp = in.x.shift_dim(0, 2);
    idx<T> resized;
    switch (mode) {
    case MEAN_RESIZE:
      resized = image_mean_resize(tmp, outrect.height,
				  outrect.width, 0, &inrect, &outr);
      break ;
    case GAUSSIAN_RESIZE:
      resized = image_gaussian_resize(tmp, outrect.height,
				      outrect.width, 0, &inrect,&outr);
      break ;
    case BILINEAR_RESIZE:
      resized = image_resize(tmp, outrect.height, outrect.width, 0,
			     &inrect, &outr);
      break ;
    default:
      eblerror("unknown resizing mode");
    }
    resized = resized.shift_dim(2, 0); 
    // call preprocessing
    if (pp) { // no preprocessing if NULL module
      inpp.x = resized;
      pp->fprop(inpp, outpp);
      resized = outpp.x;
    }
    // resize out to target dimensions if necessary
    if (((out.x.dim(1) != height) || (out.x.dim(2) != width)) && !pp)
      out.x.resize(in.x.dim(0), height, width);
    else if (((out.x.dim(1) != height) || (out.x.dim(2) != width)
	      || (out.x.dim(0) != outpp.x.dim(0))) && pp)
      out.x.resize(outpp.x.dim(0), height, width);
    idx_clear(out.x);
    resized = resized.shift_dim(0, 2);
    // apply rotation (around center of roi)
    if (rjitter != 0.0) {
      idx<T> r2 = idx_copy(resized); // make a contiguous copy
      resized = image_rotate(r2, rjitter, (int) outr.hcenter(), 
			     (int) outr.wcenter());
    }
    // apply spatial jitter
    outr.h0 += hjitter;
    outr.w0 += wjitter;
    // copy out region to output
    original_bbox = outr;
    tmp2 = image_region_to_rect(resized, outr, out.x.dim(1),
				out.x.dim(2), original_bbox);
    tmp2 = tmp2.shift_dim(2, 0);
    //idx_copy(tmp2, tmp);
    if (!zpad)
      idx_copy(tmp2, out.x);
    else { // zero padding
      original_bbox.shift(hzpad, wzpad);
      tmp3.resize(tmp2.get_idxdim());
      idx_copy(tmp2, tmp3.x);
      zpad->fprop(tmp3, out);
    }
  }
  
  template <typename T, class Tstate>
  rect<int> resizepp_module<T,Tstate>::get_original_bbox() {
    return original_bbox;
  }
  
  template <typename T, class Tstate>
  resizepp_module<T,Tstate>* resizepp_module<T,Tstate>::copy() {
    module_1_1<T,Tstate> *newpp = NULL;
    if (pp)
      newpp = (module_1_1<T,Tstate>*) pp->copy();
    return new resizepp_module(height, width, mode, newpp, kernelsz, true,
			       hzpad, wzpad);
  }
  
  ////////////////////////////////////////////////////////////////
  // resize_module

  template <typename T, class Tstate>
  resize_module<T,Tstate>::
  resize_module(double hratio_, double wratio_, uint mode_,
		uint hzpad_, uint wzpad_, bool pratio)
    : module_1_1<T,Tstate>("resize"),
      tmp3(1,1,1), mode(mode_), input_mode(0), inrect(0, 0, 0, 0),
      inrect_set(false),
      outrect_set(false), hzpad(hzpad_), wzpad(wzpad_), zpad(NULL),
      hjitter(0), wjitter(0), sjitter(1.0), preserve_ratio(pratio),
      hratio(hratio_), wratio(wratio_) {
    set_zpads(hzpad_, wzpad_);
    if (preserve_ratio)
      input_mode = 2;
    else
      input_mode = 1;
  }
  
  template <typename T, class Tstate>
  resize_module<T,Tstate>::
  resize_module(intg height_, intg width_, uint mode_, uint hzpad_, uint wzpad_,
		bool pratio)
    : module_1_1<T,Tstate>("resize"), 
      tmp3(1,1,1), mode(mode_), input_mode(0), inrect(0, 0, 0, 0),
      inrect_set(false),
      outrect_set(false), hzpad(hzpad_), wzpad(wzpad_), zpad(NULL),
      hjitter(0), wjitter(0), sjitter(1.0), preserve_ratio(pratio) {
    set_dimensions(height_, width_);
    set_zpads(hzpad_, wzpad_);
  }
  
  template <typename T, class Tstate>
  resize_module<T,Tstate>::
  resize_module(uint mode_, uint hzpad_, uint wzpad_, bool pratio)
    : tmp3(1,1,1), mode(mode_), input_mode(0), inrect(0, 0, 0, 0),
      inrect_set(false), outrect_set(false), hzpad(hzpad_), wzpad(wzpad_),
      zpad(NULL), hjitter(0), wjitter(0), sjitter(1.0), preserve_ratio(pratio) {
    set_zpads(hzpad_, wzpad_);
  }
  
  template <typename T, class Tstate>
  resize_module<T,Tstate>::~resize_module() {
    if (zpad)
      delete zpad;
  }
  
  template <typename T, class Tstate>
  void resize_module<T,Tstate>::set_dimensions(intg height_, intg width_) {
    height = height_ - hzpad * 2;
    width = width_ - wzpad * 2;
  }

  template <typename T, class Tstate>
  void resize_module<T,Tstate>::set_zpads(intg hpad, intg wpad) {
    // reset height/width without current zpad
    height += hzpad * 2;
    width += wzpad * 2;
    // update zpads and height/width
    hzpad = hpad;
    wzpad = wpad;
    height -= hzpad * 2;
    width -= wzpad * 2;
    // update zpad module
    if (zpad)
      delete zpad;
    if (hzpad > 0 || wzpad > 0)
      zpad = new zpad_module<T,Tstate>(hzpad, wzpad);
  }

  template <typename T, class Tstate>
  void resize_module<T,Tstate>::set_jitter(int h, int w, float s, float r) {
    hjitter = h;
    wjitter = w;
    sjitter = s;
    rjitter = r;
  }
  
  template <typename T, class Tstate>
  void resize_module<T,Tstate>::set_input_region(const rect<int> &inr) {
    inrect = inr;
    inrect_set = true;
  }

  template <typename T, class Tstate>
  void resize_module<T,Tstate>::set_output_region(const rect<int> &outr) {
    outrect = outr;
    outrect.height -= hzpad * 2;
    outrect.width -= wzpad * 2;
    outrect_set = true;
  }

  template <typename T, class Tstate>
  void resize_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    // TODO: TMP FIX
    float th = (in.x.dim(1) - 6) / 3 + 4;
    float tw = (in.x.dim(2) - 6) / 3 + 4;
    hratio = th / (float) in.x.dim(1);
    wratio = tw / (float) in.x.dim(2);

    // set input region to entire image if no input region is given
    if (!inrect_set)
      inrect = rect<int>(0, 0, in.x.dim(1), in.x.dim(2));
    // apply scale jitter (keeping same center)
    if (sjitter != 1.0)
      inrect.scale_centered(sjitter, sjitter);

    if (!outrect_set)
      outrect = rect<int>(0, 0, height, width);
    rect<int> outr;
    // resize input while preserving aspect ratio
    tmp = in.x.shift_dim(0, 2);    
    idx<T> resized;
    switch (mode) {
    case MEAN_RESIZE:
      resized = image_mean_resize(tmp, outrect.height,
				  outrect.width, input_mode, &inrect, &outr);
      break ;
    case GAUSSIAN_RESIZE:
      resized = image_gaussian_resize(tmp, outrect.height,
				      outrect.width, input_mode, &inrect,&outr);
      break ;
    case BILINEAR_RESIZE:
      if (input_mode == 1 || input_mode == 2) { // use ratios
	resized = image_resize(tmp, hratio, wratio, input_mode,
			       &inrect, &outr);	
	DEBUG(this->name() << ": resizing with ratios " << hratio
	      << " and " << wratio);
      }
      else // use pixels
	resized = image_resize(tmp, (double) outrect.height,
			       (double) outrect.width, input_mode,
			       &inrect, &outr);
      break ;
    default:
      eblerror("unknown resizing mode");
    }
    resized = resized.shift_dim(2, 0); 
    // resize out to target dimensions if necessary
    if (out.x.dim(0) != in.x.dim(0) || out.x.dim(1) != resized.dim(1)
	 || out.x.dim(2) != resized.dim(2))
      out.resize(in.x.dim(0), resized.dim(1), resized.dim(2));
    idx_clear(out.x);
    resized = resized.shift_dim(0, 2);
    // apply rotation (around center of roi)
    if (rjitter != 0.0) {
      idx<T> r2 = idx_copy(resized); // make a contiguous copy
      resized = image_rotate(r2, rjitter, (int) outr.hcenter(), 
			     (int) outr.wcenter());
    }
    // apply spatial jitter
    outr.h0 += hjitter;
    outr.w0 += wjitter;
    // copy out region to output
    original_bbox = outr;
    tmp2 = image_region_to_rect(resized, outr, out.x.dim(1),
    				out.x.dim(2), original_bbox);
    tmp2 = tmp2.shift_dim(2, 0);
    //idx_copy(tmp2, tmp);
    if (!zpad)
      idx_copy(tmp2, out.x);
    else { // zero padding
      original_bbox.shift(hzpad, wzpad);
      tmp3.resize(tmp2.get_idxdim());
      idx_copy(tmp2, tmp3.x);
      zpad->fprop(tmp3, out);
    }
  }

  template <typename T, class Tstate>
  void resize_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    // do nothing
    // unlikely to be needed by anyone
  }

  template <typename T, class Tstate>
  void resize_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    // do nothing
    // unlikely to be needed by anyone
  }
  
  template <typename T, class Tstate>
  rect<int> resize_module<T,Tstate>::get_original_bbox() {
    return original_bbox;
  }
  
  template <typename T, class Tstate>
  resize_module<T,Tstate>* resize_module<T,Tstate>::copy() {
    return new resize_module(*this);
  }
  
  template <typename T, class Tstate>
  std::string resize_module<T,Tstate>::describe() {
    std::string desc;
    desc << "resize module " << this->name() << ", resizing with method "
	 << mode;
    if (input_mode == 1 || input_mode == 2) // using ratios
      desc << " with height ratio " << hratio << " and width ratio " << wratio;
    else
      desc << " to " << height << "x" << width;
    desc << " while "
	 << (preserve_ratio ? "" : "not ") << "preserving aspect ratio";
    return desc;
  }
    
} // end namespace ebl
