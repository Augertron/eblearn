/***************************************************************************
 *   Copyright (C) 2011 by Yann LeCun, Pierre Sermanet and Soumith Chintala*
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com, soumith@gmail.com  *
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

namespace ebl {

  //////////////////////////////////////////////////////////////////////////////
  // subsampling_module

  template <typename T, class Tstate>
  subsampling_module<T,Tstate>::
  subsampling_module(parameter<T,Tstate> *p, uint thick, idxdim &kernel_,
		     idxdim &stride_, const char *name_, bool crop_, bool pad_)
    : module_1_1<T,Tstate>(name_), coeff(p, thick), thickness(thick),
      kernel(kernel_), stride(stride_), crop(crop_), pad(pad_) {
    // insert thickness dimension
    idxdim d = kernel;
    d.insert_dim(0, thickness);
    // allocate buffer
    sub = Tstate(d);
    // default coefficients are: 1/(stridei*stridej)^.5
    forget_param_linear fgp(1, .5); 
    forget(fgp);
  }

  template <typename T, class Tstate>
  subsampling_module<T,Tstate>::~subsampling_module() {
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize (iff necessary)
    // subsampling ( coeff * average )
    idx_clear(sub.x);
    idx<T> inx = in.x, outx = out.x;
    bool clear = false;
    intg in1 = inx.dim(1) - (inx.dim(1) % stride.dim(0));
    intg in2 = inx.dim(2) - (inx.dim(2) % stride.dim(1));
    // input too small, just clear output
    if (in1 == 0 || in2 == 0) {
      idx_clear(out.x);
      return ;
    }
    // temporarly crop input if mismatch in size      
    if (crop && (inx.dim(1) % stride.dim(0)) != 0) {
      inx = inx.narrow(1, in1, 0);
      outx = outx.narrow(1, (intg) floor((float)(inx.dim(1)/stride.dim(0))), 0);
      clear = true; // clear outx for untouched padded area
    }
    if (crop && (inx.dim(2) % stride.dim(1)) != 0) {
      inx = inx.narrow(2, in2, 0);
      outx = outx.narrow(2, (intg) floor((float)(inx.dim(2)/stride.dim(1))), 0);
      clear = true; // clear outx for untouched padded area
    }
    if (clear) idx_clear(out.x);
    // subsample
    { idx_bloop4(lix, inx, T, lsx, sub.x, T, lcx, coeff.x, T, ltx, outx, T) {
	idx<T> uuin(lix.unfold(1, stride.dim(1), stride.dim(1)));
	uuin = uuin.unfold(0, stride.dim(0), stride.dim(0));
	// loop on each pixel of subs kernel, assuming that idx_add is faster 
	// than looping on each kernel
	idx_eloop1(z1, uuin, T) {
	  idx_eloop1(z2, z1, T) {
	    idx_add(z2, lsx); // average
	  }
	}
	idx_dotc(lsx, lcx.get(), ltx); // coeff
      }}
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    idx<T> indx = in.dx, outdx = out.dx;
    intg in1 = indx.dim(1) - (indx.dim(1) % stride.dim(0));
    intg in2 = indx.dim(2) - (indx.dim(2) % stride.dim(1));
    // input too small, do nothing
    if (in1 == 0 || in2 == 0) return ;
    // temporarly crop input if mismatch in size
    if (crop && (indx.dim(1) % stride.dim(0)) != 0) {
      indx = indx.narrow(1, in1, 0);
      outdx = outdx.narrow(1, (int)floor((float)(indx.dim(1)/stride.dim(0))),0);
    }
    if (crop && (indx.dim(2) % stride.dim(1)) != 0) {
      indx = indx.narrow(2, in2, 0);
      outdx = outdx.narrow(2,(intg)floor((float)(indx.dim(2)/stride.dim(1))),0);
    }
    // update internal coefficient's dx
    idx_bloop3(lcdx, coeff.dx, T, ltdx, outdx, T, lsx, sub.x, T) {
      idx_dotacc(lsx, ltdx, lcdx);
    }
    // oversampling and accumulate to input's dx
    idx_bloop4(lidx, indx, T, lsdx, sub.dx, T,
	       lcx, coeff.x, T, ltdx2, outdx, T) {
      idx_dotc(ltdx2, lcx.get(), lsdx);
      idx_m2oversampleacc(lsdx, stride.dim(0), stride.dim(1), lidx);
    }
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {	
    idx<T> inddx = in.ddx, outddx = out.ddx;
    intg in1 = inddx.dim(1) - (inddx.dim(1) % stride.dim(0));
    intg in2 = inddx.dim(2) - (inddx.dim(2) % stride.dim(1));
    // input too small, do nothing
    if (in1 == 0 || in2 == 0) return ;
    // temporarly crop input if mismatch in size
    if (crop && (inddx.dim(1) % stride.dim(0)) != 0) {
      inddx = inddx.narrow(1, in1,0);
      outddx = outddx.narrow(1, (intg) floor((float)(inddx.dim(1)
						     / stride.dim(0))), 0);
    }
    if (crop && (inddx.dim(2) % stride.dim(1)) != 0) {
      inddx = inddx.narrow(2, in2,0);
      outddx = outddx.narrow(2, (intg) floor((float)(inddx.dim(2)
						     / stride.dim(1))), 0);
    }
    // update internal coefficient's ddx
    idx_bloop3(lcdx, coeff.ddx, T, ltdx, outddx, T, lsx, sub.x, T) {
      idx_m2squdotm2acc(lsx, ltdx, lcdx);
   }
    // oversampling and accumulte to input's ddx
    idx_bloop4(lidx, inddx, T, lsdx, sub.ddx, T,
	       lcx, coeff.x, T, ltdx2, outddx, T) {
      T cf = lcx.get();
      idx_dotc(ltdx2, cf * cf, lsdx);
      idx_m2oversampleacc(lsdx, stride.dim(0), stride.dim(1), lidx);
    }
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::forget(forget_param_linear &fp) {
    double c = fp.value / pow(stride.dim(0) * stride.dim(1), fp.exponent);
    idx_fill(coeff.x, (T) c);
  }

  template <typename T, class Tstate>
  bool subsampling_module<T,Tstate>::resize_output(Tstate &in, Tstate &out) {
    //if (this->bresize) return false;
    TIMING_RESIZING_ACCSTART(); // start accumulating resizing time
    intg sin_i = in.x.dim(1);
    intg sin_j = in.x.dim(2);
    intg si = (intg) floor(sin_i / (float) stride.dim(0));
    intg sj = (intg) floor(sin_j / (float) stride.dim(1));
    // check sizes
    if (!crop && !pad &&
	((sin_i % stride.dim(0)) != 0 || (sin_j % stride.dim(2)) != 0)) {
      cerr << "subsampling " << sin_i << "x" << sin_j << " with stride "
	   << stride << endl;
      eblerror("inconsistent input size and subsampling ratio");
    }
    // resize output and sub based in input dimensions
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(1, si); // new size after subsampling
    d.setdim(2, sj); // new size after subsampling
    if (out.x.get_idxdim() != d) { // resize only if necessary
      // resize internal buffer
      if (sub.x.order() != d.order())
	sub = Tstate(d);
      else
	sub.resize(d);
      // pad output if not matching strides
      if (pad && sin_i % stride.dim(0) != 0) d.setdim(1, d.dim(1) + 1);
      if (pad && sin_j % stride.dim(1) != 0) d.setdim(2, d.dim(2) + 1);
      // resize out
      EDEBUG(this->name() << ": resizing output from " << out.x << " to " << d);
      if (out.x.order() != d.order()) out = Tstate(d);
      else out.resize(d);
      this->update_outdims(out);
      TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
      return true;
    }
    TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
    return false;
  }
 
  template <typename T, class Tstate>
  fidxdim subsampling_module<T,Tstate>::fprop_size(fidxdim &isize) {
    fidxdim osize = isize;
    // update spatial dimensions
    for (uint i = 1; i < isize.order(); ++i)
      osize.setdim(i, std::max((intg) 1,
			       (intg) floor(isize.dim(i) /
					    (float) stride.dim(i - 1))));
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim subsampling_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    //! Just multiply each dimension by its stride
    fidxdim d = stride;
    d.insert_dim(0, 1);
    fidxdim isize = osize * d;
    return isize;
  }

  template <typename T, class Tstate>
  subsampling_module<T,Tstate>* subsampling_module<T,Tstate>::copy() {
    // new module
    subsampling_module<T,Tstate> *l2 =
      new subsampling_module<T, Tstate>(NULL, thickness, kernel, stride,
					this->name());
    // assign same parameter state
    l2->coeff = coeff;
    l2->sub = sub;
    return l2;
  }
 
  template <typename T, class Tstate>
  std::string subsampling_module<T, Tstate>::describe() {
    std::string desc;
    desc << "subsampling module " << this->name() << " with thickness "
	 << thickness << ", kernel " << kernel << " and stride " << stride;
    return desc;
  }
  
  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::dump_fprop(Tstate &in, Tstate &out) {
    fprop(in, out);
    DUMP(in.x, this->name() << "_subsampling_module_in.x");
    DUMP(coeff.x, this->name() << "_subsampling_module_coeff");
    DUMP(out.x, this->name() << "_subsampling_module_out.x");
  }

  //////////////////////////////////////////////////////////////////////////////
  // lppooling_module

  template <typename T, class Tstate>
  lppooling_module<T,Tstate>::
  lppooling_module(uint thick, idxdim &kernel_, idxdim &stride_, uint lppower_,
		   const char *name_, bool crop_
#ifdef __CUDA__
, bool use_gpu_, int gpu_id_
#endif
)
    : module_1_1<T,Tstate>(name_), thickness(thick), 
      kernel(kernel_), stride(stride_), crop(crop_), lp_pow(lppower_), 
      sqmod((T)lppower_), sqrtmod((T)(1.0/(T)lppower_))
#ifdef __CUDA__
    ,use_gpu(use_gpu_), gpu_id(gpu_id_) 
#endif
{
    // insert thickness dimension
    idxdim d = kernel;
    d.insert_dim(0, thick);
    // allocate buffers
    squared = Tstate(d);
    convolved = Tstate(d);
    // prepare convolution
    idx<intg> table = one2one_table(thick);
    conv = new convolution_module<T,Tstate>
      (&param, kernel, stride, table, "lppooling_convolution", crop
#ifdef __CUDA__
       , use_gpu, gpu_id
#endif
);

#ifdef __CUDA__
    sqmod.use_gpu = use_gpu;
    sqmod.gpu_id = gpu_id;
    sqrtmod.use_gpu = use_gpu;
    sqrtmod.gpu_id = gpu_id;
#endif
    // gaussian kernel
    idx<T> filter = create_gaussian_kernel<T>(kernel);
    idx_bloop1(k, conv->kernel.x, T) {
      idx_copy(filter, k); }

  }

  template <typename T, class Tstate>
  lppooling_module<T,Tstate>::~lppooling_module() {
    delete conv;
  }

  template <typename T, class Tstate>
  void lppooling_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    // avoid div by 0 when bproping
    idx_addc(in.x, 1e-10, in.x); // TODO: temporary

    // special case for l1pool
    if (lp_pow == 1) {
      // gaussian-weighted neighborhood of in^2
      conv->fprop(in, out);
      return;
    }
    // if it is an odd lp, example l3, l5 take an abs of the input
    if ((lp_pow % 2) == 1) idx_abs(in.x, in.x);
    // in^p
    sqmod.fprop(in, squared);
    // gaussian-weighted neighborhood of in^p
    conv->fprop(squared, convolved);
    // in^1/p(gaussian-weighted neighborhood of in^p)
    sqrtmod.fprop(convolved, out);
    // update last output size
    this->update_outdims(out);
  }

  template <typename T, class Tstate>
  void lppooling_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    // special case for l1pool
    if (lp_pow == 1) {
      conv->bprop(in, out);
      return;
    }
    squared.clear_dx();
    convolved.clear_dx();
    if ((lp_pow % 2) == 1) idx_abs(convolved.dx, convolved.dx);
    sqrtmod.bprop(convolved, out);
    conv->bprop(squared, convolved);
    sqmod.bprop(in, squared);
  }

  template <typename T, class Tstate>
  void lppooling_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {	
    // special case for l1pool
    if (lp_pow == 1) {
      conv->bbprop(in, out);
      return;
    }
    squared.clear_ddx();
    convolved.clear_ddx();
    if ((lp_pow % 2) == 1) idx_abs(convolved.ddx, convolved.ddx);
    sqrtmod.bbprop(convolved, out);
    conv->bbprop(squared, convolved);
    sqmod.bbprop(in, squared);
  }

  template <typename T, class Tstate>
  fidxdim lppooling_module<T,Tstate>::fprop_size(fidxdim &isize) {
    fidxdim osize = isize;
    // update spatial dimensions
    for (uint i = 1; i < isize.order(); ++i) {
      uint diff = kernel.dim(i - 1) - stride.dim(i - 1);
      osize.setdim(i, std::max((intg) 1, (intg) (floor(isize.dim(i) - diff)
						 / (float) stride.dim(i - 1))));
    }
    // recompute the input size to be compliant with the output
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim lppooling_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    fidxdim isize;
    if (osize.empty()) return isize;
    // multiply by stride
    fidxdim d = stride;
    d.insert_dim(0, 1);
    isize = osize * d;
    // add borders
    for (uint i = 1; i < isize.order(); ++i)
      isize.setdim(i, isize.dim(i) + kernel.dim(i - 1) - stride.dim(i - 1));
    return isize;
  }

  template <typename T, class Tstate>
  lppooling_module<T,Tstate>* lppooling_module<T,Tstate>::copy() {
    lppooling_module<T,Tstate> *l2 =
      new lppooling_module<T, Tstate>(thickness, kernel, stride, lp_pow,
				      this->name());
    return l2;
  }
 
  template <typename T, class Tstate>
  std::string lppooling_module<T, Tstate>::describe() {
    std::string desc;
    desc << "lppooling module " << this->name() << " with thickness "
	 << thickness << ", kernel " << kernel << " ,stride " << stride
         << " and Power "<< lp_pow;
    return desc;
  }
    
  template <typename T, class Tstate>
  void lppooling_module<T,Tstate>::dump_fprop(Tstate &in, Tstate &out) {
    fprop(in, out);
    conv->dump_fprop(in, out);
  }

  ////////////////////////////////////////////////////////////////
  // wavg_pooling_module

  template <typename T, class Tstate>
  wavg_pooling_module<T,Tstate>::
  wavg_pooling_module(uint thick, idxdim &kernel_,
		   idxdim &stride_, const char *name_, bool crop_)
    : module_1_1<T,Tstate>(name_), thickness(thick),
      kernel(kernel_), stride(stride_), crop(crop_) {
    // prepare convolution
    idx<intg> table = one2one_table(thick);
    conv = new convolution_module<T,Tstate>
      (&param, kernel, stride, table, "wavg_pooling_convolution", crop);
    // gaussian kernel
    idx<T> filter = create_gaussian_kernel<T>(kernel);
    idx_bloop1(k, conv->kernel.x, T) {
      idx_copy(filter, k); }
  }

  template <typename T, class Tstate>
  wavg_pooling_module<T,Tstate>::~wavg_pooling_module() {
    delete conv;
  }

  template <typename T, class Tstate>
  void wavg_pooling_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    conv->fprop(in, out);
  }

  template <typename T, class Tstate>
  void wavg_pooling_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    conv->bprop(in, out);
  }

  template <typename T, class Tstate>
  void wavg_pooling_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {	
    conv->bbprop(in, out);
  }

  template <typename T, class Tstate>
  fidxdim wavg_pooling_module<T,Tstate>::fprop_size(fidxdim &isize) {
    fidxdim osize = isize;
    // update spatial dimensions
    for (uint i = 1; i < isize.order(); ++i) {
      uint diff = kernel.dim(i - 1) - stride.dim(i - 1);
      osize.setdim(i, std::max((intg) 1, (intg) (floor(isize.dim(i) - diff)
						 / (float) stride.dim(i - 1))));
    }
    // recompute the input size to be compliant with the output
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim wavg_pooling_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    // just multiply each dimension by its stride
    fidxdim d = stride;
    d.insert_dim(0, 1);
    fidxdim isize = osize * d;
    // add borders
    for (uint i = 1; i < isize.order(); ++i)
      isize.setdim(i, isize.dim(i) + kernel.dim(i - 1) - stride.dim(i - 1));
    return isize;
  }

  template <typename T, class Tstate>
  wavg_pooling_module<T,Tstate>* wavg_pooling_module<T,Tstate>::copy() {
    wavg_pooling_module<T,Tstate> *l2 =
      new wavg_pooling_module<T, Tstate>
      (thickness, kernel, stride, this->name());
    return l2;
  }
 
  template <typename T, class Tstate>
  std::string wavg_pooling_module<T, Tstate>::describe() {
    std::string desc;
    desc << "wavg_pooling module " << this->name() << " with thickness "
	 << thickness << ", kernel " << kernel << " and stride " << stride;
    return desc;
  }
    
  template <typename T, class Tstate>
  void wavg_pooling_module<T,Tstate>::dump_fprop(Tstate &in, Tstate &out) {
    fprop(in, out);
    conv->dump_fprop(in, out);
  }

  ////////////////////////////////////////////////////////////////
  // pyramid_module

  template <typename T, class Tstate>
  pyramid_module<T,Tstate>::
  pyramid_module(uint nscales_, float scaling_ratio_, idxdim &size_,
		 uint mode_, module_1_1<T,Tstate> *pp_,
		 bool own_pp_, idxdim *dzpad_, const char *name_)
    : resizepp_module<T,Tstate>(size_, mode_, pp_, own_pp_, dzpad_),
      s2m_module<T,Tstate>(nscales_, name_),
      nscales(nscales_), scaling_ratio(scaling_ratio_) {
  }

  template <typename T, class Tstate>
  pyramid_module<T,Tstate>::
  pyramid_module(uint nscales_, float scaling_ratio_, uint mode_,
		 module_1_1<T,Tstate> *pp_,
		 bool own_pp_, idxdim *dzpad_, const char *name_)
    : resizepp_module<T,Tstate>(mode_, pp_, own_pp_, dzpad_),
      s2m_module<T,Tstate>(nscales, name_),
      nscales(nscales_), scaling_ratio(scaling_ratio_) {
  }

  template <typename T, class Tstate>
  pyramid_module<T,Tstate>::~pyramid_module() {}
  
  template <typename T, class Tstate>
  void pyramid_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tstate>
  void pyramid_module<T,Tstate>::
  fprop(Tstate &in, mstate<Tstate> &out) {    
    eblerror("not implemented");
  }

  template <typename T, class Tstate>
  void pyramid_module<T,Tstate>::
  fprop(Tstate &in, midx<T> &out) {
    // expecting a 1D midx
    if (out.order() != 1)
      eblerror("expected a 1-dimensional midx but got order " << out.order());
    if (out.dim(0) != nscales)
      out.resize(nscales);
    idxdim d(in.x);
    d.setdims(1);
    idxdim tgt = this->size;
    out.clear();
    for (uint i = 0; i < nscales; ++i) {
      idx<T> tmp(d);
      this->set_dimensions(tgt.dim(0), tgt.dim(1));
      resizepp_module<T,Tstate>::fprop(in, tmp);
      out.set(tmp, i);
      tgt.setdim(0, (intg) (tgt.dim(0) * scaling_ratio));
      tgt.setdim(1, (intg) (tgt.dim(1) * scaling_ratio));
    }
  }

  template <typename T, class Tstate>
  void pyramid_module<T,Tstate>::
  bprop(Tstate &in, mstate<Tstate> &out) {
  }

  template <typename T, class Tstate>
  void pyramid_module<T,Tstate>::
  bbprop(Tstate &in, mstate<Tstate> &out) {    
  }

  template <typename T, class Tstate>
  mfidxdim pyramid_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    this->msize = osize;
    if (osize.size() <= 0)
      eblerror("expected at least 1 element but found " << osize.size());
    return osize[0];
  }
  
  template <typename T, class Tstate>
  std::string pyramid_module<T,Tstate>::describe() {
    std::string desc = "Laplacian pyramid ";
    desc << resizepp_module<T,Tstate>::describe()
	 << ", with " << nscales << " scales";
    return desc;
  }

  // template <typename T, class Tstate>
  // const std::vector<rect<int> >& pyramid_module<T,Tstate>::
  // get_original_bboxes() {
  //   return obboxes;
  // }

  // template <typename T, class Tstate>
  // const std::vector<rect<int> >& pyramid_module<T,Tstate>::
  // get_input_bboxes() {
  //   return ibboxes;
  // }

  //////////////////////////////////////////////////////////////////////////////
  // average_pyramid_module

  template <typename T, class Tstate>
  average_pyramid_module<T,Tstate>::
    average_pyramid_module(parameter<T,Tstate> *p, uint thickness,
			   midxdim &strides_, const char *name_)
    : s2m_module<T,Tstate>(strides_.size(), name_), strides(strides_) {
    // check that we can reuse results of previous subsamplings,
    // i.e. when all strides are a multiple of the previous scale's stride
    well_behaved = true;
    for (uint i = 1; i < strides.size(); ++i) {
      idxdim d0 = strides[i-1];
      idxdim d1 = strides[i];
      idx_checkorder2(d0, d0.order(), d1, d0.order());
      // check that each dimension of d0 is a multiple of d1
      for (uint j = 0; j < d0.order(); ++j) {
	if (d1.dim(j) % d0.dim(j) != 0)
	  well_behaved = false;
      }
    }    
    // allocate subsampling modules
    for (uint i = 0; i < strides.size(); ++i) {
      idxdim d = strides[i];
      string name;
      name << "average_pyramid" << d;
      mods.push_back(new subsampling_module<T,Tstate>
		     (p, thickness, d, d, name.c_str(), true, true));
    }
  }

  template <typename T, class Tstate>
  average_pyramid_module<T,Tstate>::~average_pyramid_module() {
    for (uint i = 0; i < mods.size(); ++i)
      delete mods[i];
  }
  
  template <typename T, class Tstate>
  void average_pyramid_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tstate>
  void average_pyramid_module<T,Tstate>::fprop(Tstate &in, mstate<Tstate> &out){
    this->resize_output(in, out);
    if (well_behaved) { // we can reuse results of previous scale
      for (uint i = 0; i < mods.size(); ++i) {
	Tstate &o = out[i];
	if (i == 0) // first time, start from in
	  mods[i]->fprop(in, o);
	else { // start from previous scale
	  Tstate i0 = out[i - 1];
	  mods[i]->fprop(i0, o);
	}
      }      
    } else { // generate each scale from scratch (less efficient)
      for (uint i = 0; i < mods.size(); ++i) {
	Tstate &o = out[i];
	mods[i]->fprop(in, o);
      }
    }
  }

  template <typename T, class Tstate>
  void average_pyramid_module<T,Tstate>::
  bprop(Tstate &in, mstate<Tstate> &out) {    
    if (well_behaved) { // we can reuse results of previous scale
      for (int i = (int) mods.size() - 1; i >= 0; --i) {
	Tstate &o = out[i];
	if (i == 0) // first time, start from in
	  mods[i]->bprop(in, o);
	else { // start from previous scale
	  Tstate i0 = out[i - 1];
	  mods[i]->bprop(i0, o);
	}
      }
    } else { // generate each scale from scratch (less efficient)
      for (int i = (int) mods.size() - 1; i >= 0; --i) {
	Tstate &o = out[i];
	mods[i]->bprop(in, o);
      }
    }
  }

  template <typename T, class Tstate>
  void average_pyramid_module<T,Tstate>::
  bbprop(Tstate &in, mstate<Tstate> &out) {    
    if (well_behaved) { // we can reuse results of previous scale
      for (int i = (int) mods.size() - 1; i >= 0; --i) {
	Tstate &o = out[i];
	if (i == 0) // first time, start from in
	  mods[i]->bbprop(in, o);
	else { // start from previous scale
	  Tstate i0 = out[i - 1];
	  mods[i]->bbprop(i0, o);
	}
      }
    } else { // generate each scale from scratch (less efficient)
      for (int i = (int) mods.size() - 1; i >= 0; --i) {
	Tstate &o = out[i];
	mods[i]->bbprop(in, o);
      }
    }
  }

  template <typename T, class Tstate>
  std::string average_pyramid_module<T,Tstate>::describe() {
    std::string desc;
    desc << "Average pyramid with " << (int) strides.size()
	 << " scales with strides " << strides
	 << " (" << (well_behaved ? "" : "not ") << "well-behaved)";
    return desc;
  }

  template <typename T, class Tstate>
  mfidxdim average_pyramid_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    if (osize.size() != strides.size())
      eblerror("expected same number of outputs and strides but got "
	       << osize << " and " << strides);
    // multiply all output by strides, they should all match
    fidxdim s = strides[0];
    s.insert_dim(0, 1);
    fidxdim d1 = osize[0] * s;
    for (uint i = 1; i < osize.size(); ++i) {
      s = strides[i];
      s.insert_dim(0, 1);
      fidxdim d2 = osize[i] * s;
      // TEMPORARY CHECK REMOVE
      // if (d1 != d2)
      //   eblerror("all inputs should match but got " << d1 << " and " << d2);
    }
    mfidxdim m(d1);
    return m;
  }

  //////////////////////////////////////////////////////////////////////////////
  // maxss_module

  template <typename T, class Tstate>
  maxss_module<T,Tstate>::
  maxss_module(uint thick, idxdim &kernel_, idxdim &stride_, const char *name_)
    : module_1_1<T,Tstate>(name_), thickness(thick), kernel(kernel_),
      stride(stride_), switches(thickness, 1, 1, 2), 
      float_precision(false), double_precision(false),
      indices(thickness, 1, 1, 2) {
#ifdef __TH__
    // check precision to decide if we use TH or not
    Tstate *temp = new Tstate(1,1);
    fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(temp);
    if (cont) {
      float_precision = true;
    }
    else {
      fstate_idx<double> *cont_d = dynamic_cast<fstate_idx<double>*>(temp);
      if(cont_d) {
        double_precision = true;
      }
    }
    delete temp;
#else
    eblerror("Maxss is implemented only using the TH library. ");
#endif
  }

  template <typename T, class Tstate>
  maxss_module<T,Tstate>::~maxss_module() {
  }

  template <typename T, class Tstate>
  void maxss_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize (iff necessary)
#ifdef __TH__
    if((float_precision || double_precision) && in.x.order()==3) {
      th_maxpool_3d(in.x, kernel.dim(0), kernel.dim(1), out.x, stride.dim(0), 
                    stride.dim(1), indices);
      return;
    }
#endif
    { idx_bloop3(lix, in.x, T, sw, switches, int, ltx, out.x, T) {
	int i = 0, j = 0;
	idx<T> uuin(lix.unfold(1, kernel.dim(2), stride.dim(1)));
	uuin = uuin.unfold(0, kernel.dim(1), stride.dim(0));
	idx_bloop3(z1, uuin, T, sw1, sw, int, o1, ltx, T) {
	  idx_bloop3(z2, z1, T, sw2, sw1, int, o2, o1, T) {
	    intg indx = idx_indexmax(z2); // find index of max
	    // height in input
	    sw2.set((int) (indx / kernel.dim(2) + stride.dim(0) * i), 0);
	    // width in input
	    sw2.set((int) (indx / kernel.dim(2) + stride.dim(1) * j), 1);
	    o2.set(z2.get(indx)); // copy max to output
	  }
	  j++;
	}
	i++;
      }}
  }

  template <typename T, class Tstate>
  void maxss_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
#ifdef __TH__
    if((float_precision || double_precision) && in.x.order()==3) {
      th_maxpool_3d_bprop(in.x, kernel.dim(0), kernel.dim(1), out.dx, in.dx, 
                          stride.dim(0), stride.dim(1), indices);
      return;
    }
#endif
    // copy derivatives in the position given by the switches
    int i = 0, j = 0;
    idx_bloop3(di1, in.dx, T, s1, switches, int, do1, out.dx, T) {
      idx_bloop2(s2, s1, int, do2, do1, T) {
	idx_bloop2(s3, s2, int, do3, do2, T) {
	  i = s3.get(0); j = s3.get(1);
	  di1.set(di1.get(i, j) + do3.get(), i, j);
	}}}
  }

  template <typename T, class Tstate>
  void maxss_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
#ifdef __TH__
    if((float_precision || double_precision) && in.x.order()==3) {
      th_maxpool_3d_bprop(in.x, kernel.dim(0), kernel.dim(1), out.ddx, in.ddx, 
                          stride.dim(0), stride.dim(1), indices);
      return;
    }
#endif
    // copy derivatives in the position given by the switches
    int i = 0, j = 0;
    idx_bloop3(di1, in.ddx, T, s1, switches, int, do1, out.ddx, T) {
      idx_bloop2(s2, s1, int, do2, do1, T) {
	idx_bloop2(s3, s2, int, do3, do2, T) {
	  i = s3.get(0); j = s3.get(1);
	  di1.set(di1.get(i, j) + do3.get(), i, j);
	}}}
  }

  template <typename T, class Tstate>
  bool maxss_module<T,Tstate>::resize_output(Tstate &in, Tstate &out) {
    if (!this->bresize) return false;
    TIMING_RESIZING_ACCSTART(); // start accumulating resizing time
    intg sin_i = in.x.dim(1);
    intg sin_j = in.x.dim(2);
    intg si = sin_i / stride.dim(0);
    intg sj = sin_j / stride.dim(1);
    // TH Handles inconsistent sizes by adding a slight overlap
// #ifndef __TH__
    // check sizes
    if ((sin_i % stride.dim(0)) != 0 || (sin_j % stride.dim(1)) != 0) {
      cerr << "maxss " << sin_i << "x" << sin_j << " with stride "
	   << stride << endl;
      eblerror("inconsistent input size and maxss ratio");
    }
// #endif
    // resize output and sub based in input dimensions
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(1, si); // new size after maxss
    d.setdim(2, sj); // new size after maxss
    if (out.x.get_idxdim() != d) { // resize only if necessary
      EDEBUG(this->name() << ": resizing output from " << out.x << " to " << d);
      out.resize(d);
      d.insert_dim(2, d.order());
      switches.resize(d);
      indices.resize(d);
    }
    TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
    return true;
  }

  template <typename T, class Tstate>
  fidxdim maxss_module<T,Tstate>::fprop_size(fidxdim &isize) {
    fidxdim osize = isize;
    // update spatial dimensions
    for (uint i = 1; i < isize.order(); ++i)
      osize.setdim(i, std::max((intg) 1,
			       (intg) floor((float) (isize.dim(i)
						     / stride.dim(i - 1)))));
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim maxss_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    //! Just multiply each dimension by its stride
    fidxdim d = stride;
    d.insert_dim(0, 1);
    fidxdim isize = osize * d;
    return isize;
  }

  template <typename T, class Tstate>
  maxss_module<T,Tstate>* maxss_module<T,Tstate>::copy(parameter<T,Tstate> *p) {
    // new module (with its own local parameter buffers)
    maxss_module<T,Tstate> *l2 =
      new maxss_module<T, Tstate>(thickness, kernel, stride, this->name());
    return l2;
  }

  template <typename T, class Tstate>
  std::string maxss_module<T, Tstate>::describe() {
    std::string desc;
    desc << "maxss module " << this->name() << " with thickness " << thickness
	 << ", kernel " << kernel << " and stride " << stride;
    return desc;
  }

} // end namespace ebl
