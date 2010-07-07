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

  template <class T>
  channorm_module<T>::channorm_module(uint normalization_size_)
    : normalization_size(normalization_size_), tmp(1,1,1),
      norm(normalization_size, normalization_size, 1, "norm", true, false,
	   true) {
  }

  template <class T>
  channorm_module<T>::~channorm_module() {
  }

  template <class T>
  void channorm_module<T>::resize_output(state_idx<T> &in, state_idx<T> &out,
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

  template <class T>
  rgb_to_ypuv_module<T>::rgb_to_ypuv_module(uint normalization_size)
    : channorm_module<T>(normalization_size) {
  }

  template <class T>
  rgb_to_ypuv_module<T>::~rgb_to_ypuv_module() {
  }

  template <class T>
  void rgb_to_ypuv_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
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
    // convert Y to Yp
    this->tmp.x = out.x.narrow(0, 1, 0);
    this->norm.fprop(this->tmp, this->tmp); // local
  }

  template <class T>
  rgb_to_ypuv_module<T>* rgb_to_ypuv_module<T>::copy() {
    return new rgb_to_ypuv_module<T>(this->normalization_size);
  }
  
  ////////////////////////////////////////////////////////////////
  // rgb_to_yp_module

  template <class T>
  rgb_to_yp_module<T>::rgb_to_yp_module(uint normalization_size)
    : channorm_module<T>(normalization_size) {
  }

  template <class T>
  rgb_to_yp_module<T>::~rgb_to_yp_module() {
  }

  template <class T>
  void rgb_to_yp_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (this->bResize) resize_output(in, out, 1); // resize (iff necessary)
    // RGB to YUV
    idx_eloop2(inx, in.x, T, tmpx, this->tmp.x, T) {
      idx_eloop2(inxx, inx, T, tmpxx, tmpx, T) {
	rgb_to_y_1D(inxx, tmpxx);
      }
    }
    // convert Y to Yp
    this->norm.fprop(this->tmp, out); // local
  }

  template <class T>
  rgb_to_yp_module<T>* rgb_to_yp_module<T>::copy() {
    return new rgb_to_yp_module<T>(this->normalization_size);
  }
    
  ////////////////////////////////////////////////////////////////
  // bgr_to_ypuv_module

  template <class T>
   bgr_to_ypuv_module<T>::bgr_to_ypuv_module(uint normalization_size)
    : channorm_module<T>(normalization_size) {
  }

  template <class T>
  bgr_to_ypuv_module<T>::~bgr_to_ypuv_module() {
  }

  template <class T>
  void bgr_to_ypuv_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
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

  template <class T>
  bgr_to_ypuv_module<T>* bgr_to_ypuv_module<T>::copy() {
    return new bgr_to_ypuv_module<T>(this->normalization_size);
  }
  
  ////////////////////////////////////////////////////////////////
  // bgr_to_yp_module

  template <class T>
  bgr_to_yp_module<T>::bgr_to_yp_module(uint normalization_size)
    : channorm_module<T>(normalization_size) {
  }

  template <class T>
  bgr_to_yp_module<T>::~bgr_to_yp_module() {
  }

  template <class T>
  void bgr_to_yp_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
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

  template <class T>
  bgr_to_yp_module<T>* bgr_to_yp_module<T>::copy() {
    return new bgr_to_yp_module<T>(this->normalization_size);
  }
       
  ////////////////////////////////////////////////////////////////
  // rgb_to_hp_module

  template <class T>
  rgb_to_hp_module<T>::rgb_to_hp_module(uint normalization_size)
    : channorm_module<T>(normalization_size) {
  }

  template <class T>
  rgb_to_hp_module<T>::~rgb_to_hp_module() {
  }

  template <class T>
  void rgb_to_hp_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
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
 
  template <class T>
  rgb_to_hp_module<T>* rgb_to_hp_module<T>::copy() {
    return new rgb_to_hp_module<T>(this->normalization_size);
  }
     
  ////////////////////////////////////////////////////////////////
  // resizepp_module

  template <class T>
  resizepp_module<T>::
  resizepp_module(intg height_, intg width_, uint mode_,
		  module_1_1<T> *pp_, uint kernelsz_, bool own_pp_)
    : pp(pp_), own_pp(own_pp_), kernelsz(kernelsz_), inpp(1,1,1), outpp(1,1,1),
      mode(mode_), inrect(0, 0, 0, 0), inrect_set(false) {
    set_dimensions(height_, width_);
  }
  
  template <class T>
  resizepp_module<T>::~resizepp_module() {
    if (pp && own_pp)
      delete pp;
  }
  
  template <class T>
  void resizepp_module<T>::set_dimensions(intg height_, intg width_) {
    height = height_;
    width = width_;
  }

  template <class T>
  void resizepp_module<T>::set_input_region(const rect &inr) {
    inrect = inr;
    inrect_set = true;
  }

  template <class T>
  void resizepp_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // set input region to entire image if no input region is given
    if (!inrect_set)
      inrect = rect(0, 0, in.x.dim(1), in.x.dim(2));
    // resize input while preserving aspect ratio
    tmp = in.x.shift_dim(0, 2);
    idx<T> resized;
    switch (mode) {
    case MEAN_RESIZE:
      resized =	image_mean_resize(tmp, height,width, 0, &inrect, &outrect);
      break ;
    case GAUSSIAN_RESIZE:
      resized =	image_gaussian_resize(tmp, height,width, 0, &inrect, &outrect);
      break ;
    case BILINEAR_RESIZE:
      resized =	image_resize(tmp, height, width, 0, &inrect, &outrect);
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
    // copy out region to output
    original_bbox = outrect;
    resized = resized.shift_dim(0, 2);
    tmp2 = image_region_to_rect(resized, outrect, out.x.dim(1),
				out.x.dim(2), original_bbox);
    tmp2 = tmp2.shift_dim(2, 0);
    //idx_copy(tmp2, tmp);
    idx_copy(tmp2, out.x);
//     // copy pp output into output with target dimensions, removing ker borders
//     tmp2 = resized.narrow(1, outrect.height, outrect.h0 + kernelsz / 2);
//     tmp2 = tmp2.narrow(2, outrect.width, outrect.w0 + kernelsz / 2);
//     idx_copy(tmp2, tmp);
//     // remember where the original image has been placed in output
//     original_bbox = rect((height - resized0.dim(1)) / 2,
// 			 (width - resized0.dim(2)) / 2,
// 			 resized0.dim(1), resized0.dim(2));
  }
  
  template <class T>
  rect resizepp_module<T>::get_original_bbox() {
    return original_bbox;
  }
  
  template <class T>
  resizepp_module<T>* resizepp_module<T>::copy() {
    module_1_1<T> *newpp = NULL;
    if (pp)
      newpp = (module_1_1<T>*) pp->copy();
    return new resizepp_module(height, width, mode, newpp, kernelsz);
  }
  
} // end namespace ebl
