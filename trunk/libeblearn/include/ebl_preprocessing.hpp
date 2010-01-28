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
  // rgb_to_ypuv_module

  template <class T>
  rgb_to_ypuv_module<T>::rgb_to_ypuv_module(uint normalization_size_)
    : normalization_size(normalization_size_), tmp(1,1) {
  }

  template <class T>
  rgb_to_ypuv_module<T>::~rgb_to_ypuv_module() {
  }

  template <class T>
  void rgb_to_ypuv_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    idx<T> uv, yp, yuv;

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
    yp = out.x.select(0, 0);
    idx_std_normalize(yp, tmp);
    image_local_normalization(tmp, yp, normalization_size);
  }
  
  template <class T>
  void rgb_to_ypuv_module<T>::resize_output(state_idx<T> &in,
					    state_idx<T> &out) {
    // resize output based on input dimensions
    idxdim d(in.x);
    if (d != out.x.get_idxdim())
      out.x.resize(d); // resize only x, as bprop and bbprop are not defined
    // resize temporary y buffer
    if ((tmp.dim(0) != d.dim(1)) || (tmp.dim(1) != d.dim(2)))
      tmp.resize(d.dim(1), d.dim(2));
  }
  
  ////////////////////////////////////////////////////////////////
  // rgb_to_yp_module

  template <class T>
  rgb_to_yp_module<T>::rgb_to_yp_module(uint normalization_size_)
    : normalization_size(normalization_size_), tmp(1,1) {
  }

  template <class T>
  rgb_to_yp_module<T>::~rgb_to_yp_module() {
  }

  template <class T>
  void rgb_to_yp_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    // RGB to YUV
    idx_eloop2(inx, in.x, T, outx, out.x, T) {
      idx_eloop2(inxx, inx, T, outxx, outx, T) {
	rgb_to_y_1D(inxx, outxx);
      }
    }
    // convert Y to Yp
    idx<T> yp = out.x.select(0, 0);
    idx_std_normalize(yp, tmp);
    image_local_normalization(tmp, yp, normalization_size);
  }
  
  template <class T>
  void rgb_to_yp_module<T>::resize_output(state_idx<T> &in,
					    state_idx<T> &out) {
    // resize output based on input dimensions
    idxdim d(in.x);
    d.setdim(0, 1);
    if (d != out.x.get_idxdim())
      out.x.resize(d); // resize only x, as bprop and bbprop are not defined
    // resize temporary y buffer
    if ((tmp.dim(0) != d.dim(1)) || (tmp.dim(1) != d.dim(2)))
      tmp.resize(d.dim(1), d.dim(2));
  }
  
  ////////////////////////////////////////////////////////////////
  // bgr_to_ypuv_module

  template <class T>
  bgr_to_ypuv_module<T>::bgr_to_ypuv_module(uint normalization_size_)
    : normalization_size(normalization_size_), tmp(1,1) {
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
    yp = out.x.select(0, 0);
    idx_std_normalize(yp, tmp);
    image_local_normalization(tmp, yp, normalization_size);
  }
  
  template <class T>
  void bgr_to_ypuv_module<T>::resize_output(state_idx<T> &in,
					    state_idx<T> &out) {
    // resize output based on input dimensions
    idxdim d(in.x);
    if (d != out.x.get_idxdim())
      out.x.resize(d); // resize only x, as bprop and bbprop are not defined
    // resize temporary y buffer
    if ((tmp.dim(0) != d.dim(1)) || (tmp.dim(1) != d.dim(2)))
      tmp.resize(d.dim(1), d.dim(2));
  }
  
  ////////////////////////////////////////////////////////////////
  // bgr_to_yp_module

  template <class T>
  bgr_to_yp_module<T>::bgr_to_yp_module(uint normalization_size_)
    : normalization_size(normalization_size_), tmp(1,1) {
  }

  template <class T>
  bgr_to_yp_module<T>::~bgr_to_yp_module() {
  }

  template <class T>
  void bgr_to_yp_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    // BGR to YUV
    idx_eloop2(inx, in.x, T, outx, out.x, T) {
      idx_eloop2(inxx, inx, T, outxx, outx, T) {
	bgr_to_y_1D(inxx, outxx);
      }
    }
    // convert Y to Yp
    idx<T> yp = out.x.select(0, 0);
    idx_std_normalize(yp, tmp);
    image_local_normalization(tmp, yp, normalization_size);
  }
  
  template <class T>
  void bgr_to_yp_module<T>::resize_output(state_idx<T> &in,
					    state_idx<T> &out) {
    // resize output based on input dimensions
    idxdim d(in.x);
    d.setdim(0, 1);
    if (d != out.x.get_idxdim())
      out.x.resize(d); // resize only x, as bprop and bbprop are not defined
    // resize temporary y buffer
    if ((tmp.dim(0) != d.dim(1)) || (tmp.dim(1) != d.dim(2)))
      tmp.resize(d.dim(1), d.dim(2));
  }
  
  ////////////////////////////////////////////////////////////////
  // resizepp_module

  template <class T>
  resizepp_module<T>::
  resizepp_module(intg height_, intg width_, module_1_1<T> *pp_, uint kernelsz_)
    : pp(pp_), kernelsz(kernelsz_), inpp(1,1,1), outpp(1,1,1) {
    set_dimensions(height_, width_);
  }
  
  template <class T>
  resizepp_module<T>::~resizepp_module() {
  }
  
  template <class T>
  void resizepp_module<T>::set_dimensions(intg height_, intg width_) {
    height = height_;
    width = width_;
  }

  template <class T>
  void resizepp_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // resize input while preserving aspect ratio
    tmp = in.x.shift_dim(0, 2);
    idx<T> resized = image_gaussian_resize2(tmp, height + MAX(0, (int) kernelsz - 1),
				  width + MAX(0, (int) kernelsz - 1), 0);
    resized = resized.shift_dim(2, 0);
    // dimensions of ratio-kept resized without border compensations
    idxdim resized0(resized.dim(0), resized.dim(1) - MAX(0, (int) kernelsz - 1),
		    resized.dim(2) - MAX(0, (int) kernelsz - 1));
    // resize out to target dimensions if necessary
    if (((out.x.dim(1) != height) || (out.x.dim(2) != width)) && !pp)
      out.x.resize(in.x.dim(0), height, width);
    else if (((out.x.dim(1) != height) || (out.x.dim(2) != width)
	      || (out.x.dim(0) != outpp.x.dim(0))) && pp)
      out.x.resize(outpp.x.dim(0), height, width);
    idx_clear(out.x);
    tmp = out.x.narrow(1, resized0.dim(1), (height - resized0.dim(1)) / 2);
    tmp = tmp.narrow(2, resized0.dim(2), (width - resized0.dim(2)) / 2);
    // call preprocessing
    if (pp) { // no preprocessing if NULL module
      inpp.x = resized;
      pp->fprop(inpp, outpp);
      // copy pp output into output with target dimensions
      if (kernelsz > 0) { // remove borders if using kernel
	tmp2 = outpp.x.narrow(1, resized0.dim(1), kernelsz / 2);
	tmp2 = tmp2.narrow(2, resized0.dim(2), kernelsz / 2);
	idx_copy(tmp2, tmp);
      } else // simple copy
	idx_copy(outpp.x, tmp);
    } else // no pp, copy directly to output
      idx_copy(resized, tmp);
    // remember where the original image has been placed in output
    original_bbox = rect((height - resized0.dim(1)) / 2,
			 (width - resized0.dim(2)) / 2,
			 resized0.dim(1), resized0.dim(2));
  }
  
  template <class T>
  rect resizepp_module<T>::get_original_bbox() {
    return original_bbox;
  }
  
} // end namespace ebl
