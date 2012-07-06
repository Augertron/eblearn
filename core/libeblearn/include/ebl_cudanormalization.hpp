/***************************************************************************
 *   Copyright (C) 2012 by Soumith Chintala *
 *   soumith@gmail.com *
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

#ifndef EBL_CUDANORMALIZATION_HPP_
#define EBL_CUDANORMALIZATION_HPP_

#ifdef __CUDA__

namespace ebl {
  ////////////////////////////////////////////////////////////////
  //// cuda_divisive_norm_module

    template <typename T, class Tstate>
    cuda_divisive_norm_module<T,Tstate>::
    cuda_divisive_norm_module(idxdim &kerdim_, int nf, bool mirror_, bool threshold_,
		       parameter<T,Tstate> *param_, const char *name_, bool af,
		       double cgauss_, bool fsum_div_, float fsum_split_,
                              double epsilon_, int gpu_id_)
          : module_1_1<T,Tstate>(name_), param(param_),
      convvar(true, name_),
      sqrtmod((T) .5, gpu_id_), // square root module
      sqmod(2, gpu_id_), // square module
      // by default, pass 0 for threshold and values
      // but every fprop updates these values
      thres((T) 1.0, (T) 1.0), // threshold module
      // create internal states, explanations are in fprop
      threshold(threshold_), nfeatures(nf), kerdim(kerdim_),
      across_features(af), epsilon(epsilon_),
      cgauss(cgauss_), fsum_div(fsum_div_),
      fsum_split(fsum_split_), gpu_id(gpu_id_) {

      if(mirror_) eblerror("Mirror padding not implemented for CUDA");
      // create weighting kernel
      if (kerdim.order() != 2)
        eblerror("expected kernel dimensions with order 2 but found order "
                 << kerdim.order() << " in " << kerdim);
      w = create_gaussian_kernel<T>(kerdim, cgauss);
      idxdim stride(1, 1);
      // prepare convolutions and their kernels
      idx<intg> table = one2one_table(nfeatures);
      divconv = new cuda_convolution_module<T,Tstate>(param, kerdim, stride, table, 
                                                      "divnorm_conv", true, gpu_id_);
      idx_bloop1(kx, divconv->kernel.x, T)
        idx_copy(w, kx);
      //! normalize the kernel    
      if (across_features)
        idx_dotc(divconv->kernel.x, 1 / idx_sum(divconv->kernel.x),
                 divconv->kernel.x);
      // convvar
      padding = new zpad_module<T,Tstate>(w.get_idxdim());
      convvar.add_module(padding);
      convvar.add_module(divconv);
      //! feature sum module to sum along features 
      //! this might be implemented by making the table in above conv module
      //! all to all connection, but that would be very inefficient
      if (across_features)
        convvar.add_module(new fsum_module<T,Tstate>(fsum_div, fsum_split));
    }

  template <typename T, class Tstate>
  cuda_divisive_norm_module<T,Tstate>::~cuda_divisive_norm_module() {
  }

  template <typename T, class Tstate>
  void cuda_divisive_norm_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {  
    //! insq = in^2 
    sqmod.fprop(in, insq);
    //! invar = sum_j (w_j in^2)
    convvar.fprop(insq, invar);
    idx_addc(invar.x, (T) epsilon, invar.x); // TODO: temporary, avoid div by 0
    //! instd = sqrt(sum_j (w_j in^2))
    sqrtmod.fprop(invar, instd);
    // the threshold is the average of all the standard deviations over
    // the entire input. values below it will be set to the threshold.
    if (threshold) { // don't update threshold for inputs
      //! update the threshold values in thres
      T mm = (T) (idx_sum(instd.x) / (T) instd.size());
      thres.thres = mm;
      thres.val = mm;
    }
    //! std(std<mean(std)) = mean(std)
    thres.fprop(instd, thstd);
    
    //! out = in / thstd
    this->resize_output(in, out);
    cuda_div(in.x, thstd.x, out.x, gpu_id);
  }

  template <typename T, class Tstate>
  cuda_divisive_norm_module<T,Tstate>* cuda_divisive_norm_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    cuda_divisive_norm_module<T,Tstate> *d =
      new cuda_divisive_norm_module<T,Tstate>(kerdim, nfeatures, false,
                                              threshold, p, this->name(), 
                                              across_features, cgauss, fsum_div,
                                              fsum_split, epsilon, gpu_id);
    if (!p) // assign same parameter state if no parameters were specified      
      d->divconv->kernel = divconv->kernel;
    return d;
  }
  
  template <typename T, class Tstate>
  std::string cuda_divisive_norm_module<T, Tstate>::describe() {
    std::string desc;
    desc << "cuda_divisive_norm module " << this->name() << " with kernel "
	 << kerdim << ", using " << "zero padding"
	 << ", using " << (param ? "learned" : "fixed") << " filter " << w;
    return desc;
  }

  ////////////////////////////////////////////////////////////////
  // cuda_subtractive_norm_module

  template <typename T, class Tstate>
  cuda_subtractive_norm_module<T,Tstate>::
  cuda_subtractive_norm_module(idxdim &kerdim_, int nf, bool mirror_,
			  bool global_norm_, parameter<T,Tstate> *param_,
			  const char *name_, bool af, double cgauss_,
			  bool fsum_div_, float fsum_split_, int gpu_id_)
    : module_1_1<T,Tstate>(name_), param(param_), convmean(true, name_), 
      global_norm(global_norm_), nfeatures(nf),
      kerdim(kerdim_), across_features(af), cgauss(cgauss_), 
      fsum_div(fsum_div_), fsum_split(fsum_split_), gpu_id(gpu_id_) {
    if(mirror_) eblerror("Mirror padding not implemented for CUDA");
    // create weighting kernel
    if (kerdim.order() != 2)
      eblerror("expected kernel dimensions with order 2 but found order "
	       << kerdim.order() << " in " << kerdim);
    w = create_gaussian_kernel<T>(kerdim, cgauss);
    idxdim stride(1, 1);
    // prepare convolutions and their kernels
    idx<intg> table = one2one_table(nfeatures);
    meanconv = new cuda_convolution_module<T,Tstate>(param, kerdim, stride, table, 
                                                "subnorm_conv");
    idx_bloop1(kx, meanconv->kernel.x, T)
      idx_copy(w, kx);
    //! normalize the kernel    
    if (across_features)
      idx_dotc(meanconv->kernel.x, 1 / idx_sum(meanconv->kernel.x),
	       meanconv->kernel.x);
    // convvar    
    padding = new zpad_module<T,Tstate>(w.get_idxdim());
    convmean.add_module(padding);
    convmean.add_module(meanconv);
    //! feature sum module to sum along features 
    //! this might be implemented by making the table in above conv module
    //! all to all connection, but that would be very inefficient
    if (across_features)
      convmean.add_module(new fsum_module<T,Tstate>(fsum_div, fsum_split));
  }

    template <typename T, class Tstate>
    cuda_subtractive_norm_module<T,Tstate>::~cuda_subtractive_norm_module() {
  }

    template <typename T, class Tstate>
  void cuda_subtractive_norm_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (global_norm) // global normalization
      idx_std_normalize(in.x, in.x, (T*) NULL);
    // inmean = sum_j (w_j * in_j)
    convmean.fprop(in, inmean);
    // inzmean = in - inmean
    difmod.fprop(in, inmean, out);
  }

  template <typename T, class Tstate>
  cuda_subtractive_norm_module<T,Tstate>* cuda_subtractive_norm_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    cuda_subtractive_norm_module<T,Tstate> *d = new cuda_subtractive_norm_module<T,Tstate>
      (kerdim, nfeatures, false, global_norm, p, this->name(),
       across_features, cgauss, fsum_div, fsum_split, gpu_id);
    if (!p) // assign same parameter state if no parameters were specified
      d->meanconv->kernel = meanconv->kernel;
    return d;
  }
  
  template <typename T, class Tstate>
  std::string cuda_subtractive_norm_module<T, Tstate>::describe() {
    std::string desc;
    desc << "cuda_subtractive_norm module with " << (param ? "learned" : "fixed")
	 << " mean weighting and kernel " << meanconv->kernel.x
	 << ", " << (global_norm ? "" : "not ")
	 << "using global normalization";
    return desc;
  }


  ////////////////////////////////////////////////////////////////
  // cuda_contrast_norm_module

  template <typename T, class Tstate>
  cuda_contrast_norm_module<T,Tstate>::
  cuda_contrast_norm_module(idxdim &kerdim, int nf, bool mirror, bool threshold,
		       bool gnorm_, parameter<T,Tstate> *param,
		       const char *name_, bool af, bool lm, double cgauss,
                            bool fsum_div, float fsum_split, double epsilon, int gpu_id_)
    : module_1_1<T,Tstate>(name_),
      subnorm(kerdim, nf, mirror, gnorm_, lm ? param : NULL, name_, af, cgauss,
	      fsum_div, fsum_split, gpu_id_),
      divnorm(kerdim, nf, mirror, threshold, param, name_, af, cgauss,
	      fsum_div, fsum_split, epsilon, gpu_id_),
      learn_mean(lm) {
    if(mirror) eblerror("Mirror padding not implemented for CUDA");
  }

  template <typename T, class Tstate>
  cuda_contrast_norm_module<T,Tstate>::~cuda_contrast_norm_module() {
  }
      
  template <typename T, class Tstate>
  void cuda_contrast_norm_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    // subtractive normalization
    subnorm.fprop(in, tmp);
    // divisive normalization
    divnorm.fprop(tmp, out);
  }

  template <typename T, class Tstate>
  cuda_contrast_norm_module<T,Tstate>* cuda_contrast_norm_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    cuda_contrast_norm_module<T,Tstate> *d = new cuda_contrast_norm_module<T,Tstate>
      (divnorm.kerdim, divnorm.nfeatures, false, divnorm.threshold,
       global_norm, p, this->name(), divnorm.across_features, learn_mean, gpu_id);
    if (!p) { // assign same parameter state if no parameters were specified
      d->divnorm.divconv->kernel = divnorm.divconv->kernel;
      d->subnorm.meanconv->kernel = subnorm.meanconv->kernel;
    }
    return d;
  }
  
  template <typename T, class Tstate>
  std::string cuda_contrast_norm_module<T, Tstate>::describe() {
    std::string desc;
    desc << "cuda_contrast_norm module with " << subnorm.describe()
	 << " and " << divnorm.describe();
    return desc;
  }





} // end ebl namespace

#endif // end __CUDA__

#endif // EBL_CUDANORMALIZATION_HPP_
