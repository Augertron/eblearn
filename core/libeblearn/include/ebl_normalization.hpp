/***************************************************************************
 *   Copyright (C) 2011 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // divisive_norm_module

  template <typename T, class Tstate>
  divisive_norm_module<T,Tstate>::
  divisive_norm_module(const char *name_) 
    : module_1_1<T,Tstate>(name_), convvar(true, name_), invmod(-1),
      thres((T) 1.0, (T) 1.0) {
  }

  template <typename T, class Tstate>
  divisive_norm_module<T,Tstate>::
  divisive_norm_module(idxdim &kerdim_, int nf, bool mirror_, bool threshold_,
		       parameter<T,Tstate> *param_, const char *name_, bool af,
		       double cgauss_, bool fsum_div_, float fsum_split_,
		       double epsilon_, double epsilon2_)
    : module_1_1<T,Tstate>(name_), convvar(true, name_), invmod(-1),
      thres((T) 1.0, (T) 1.0) {
    // common initializations
    init(kerdim_, nf, mirror_, threshold_, param_, af, cgauss_, fsum_div_, 
	 fsum_split_, epsilon_, epsilon2_);
    // local initializations
    divconv = new convolution_module<T,Tstate>
      (param, kerdim, stride, conv_table, "divnorm_conv");
    set_kernel(gaussian_kernel);
    convvar.add_module(divconv);
    //! feature sum module to sum along features 
    //! this might be implemented by making the table in above conv module
    //! all to all connection, but that would be very inefficient
    if (across_features)
      convvar.add_module(new fsum_module<T,Tstate>(fsum_div, fsum_split));
    // allocate power modules
    sqrtmod = new power_module<T,Tstate>((T) .5);
    sqmod = new power_module<T,Tstate>((T) 2);
  }

  template <typename T, class Tstate>
  void divisive_norm_module<T,Tstate>::
  init(idxdim &kerdim_, int nf, bool mirror_, bool threshold_, 
       parameter<T,Tstate> *param_, bool af, double cgauss_, bool fsum_div_, 
       float fsum_split_, double epsilon_, double epsilon2_) {
    mirror = mirror_;
    param = param_;
    threshold = threshold_;
    nfeatures = nf;
    kerdim = kerdim_;
    across_features = af;
    epsilon = epsilon_;
    epsilon2 = epsilon2_;
    cgauss = cgauss_;
    fsum_div = fsum_div_;
    fsum_split = fsum_split_;
    // create weighting kernel
    if (kerdim.order() != 2)
      eblerror("expected kernel dimensions with order 2 but found order "
	       << kerdim.order() << " in " << kerdim);
    gaussian_kernel = create_gaussian_kernel<T>(kerdim, cgauss);
    stride = idxdim(1, 1);
    // convolution table
    conv_table = one2one_table(nfeatures);
    // convvar
    if (mirror) // switch between zero and mirror padding
      padding = new mirrorpad_module<T,Tstate>
	((gaussian_kernel.dim(0) - 1)/2, (gaussian_kernel.dim(1) - 1)/2);
    else padding = new zpad_module<T,Tstate>(gaussian_kernel.get_idxdim());
    convvar.add_module(padding);
  }

  template <typename T, class Tstate>
  void divisive_norm_module<T,Tstate>::set_kernel(idx<T> &w) {
    // copy kernel to convolution param
    idx_bloop1(kx, divconv->kernel.x, T) idx_copy(w, kx);
    //! normalize the kernel
    if (across_features)
      idx_dotc(divconv->kernel.x, 1 / idx_sum(divconv->kernel.x),
	       divconv->kernel.x);
  }

  template <typename T, class Tstate>
  bool divisive_norm_module<T,Tstate>::optimize_fprop(Tstate &in, Tstate &out) {
    // memory optimization
    // if (false) {//hi && ho) { // dual buffers are provided, use them
    cout << "Using dual buffer memory optimization in divisive_norm_module"
	 << endl;
    insq = out;
    invar = in;
    instd = out;
    thstd = in;
    invstd = out;
    return false;
  }
  
  template <typename T, class Tstate>
  divisive_norm_module<T,Tstate>::~divisive_norm_module() {
    if (sqrtmod) delete sqrtmod;
    if (sqmod) delete sqmod;
  }
      
  template <typename T, class Tstate>
  void divisive_norm_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (epsilon2 != 0) 
      idx_addc(in.x, (T) epsilon2, in.x); // avoid div by 0
    //! insq = in^2 TODO: would this be faster with idx_mul(in, in, insq)?
    sqmod->fprop(in, insq);
    //! invar = sum_j (w_j in^2)
    convvar.fprop(insq, invar);
    if (epsilon != 0) idx_addc(invar.x, (T) epsilon, invar.x); // avoid div by 0
    // if learning filters, make sure they stay positive
    //    if (param) idx_abs(divconv->kernel.x, divconv->kernel.x);
    //! instd = sqrt(sum_j (w_j in^2))
    sqrtmod->fprop(invar, instd);
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
    this->invert(in, thstd, out);
  }

  template <typename T, class Tstate>
  void divisive_norm_module<T,Tstate>::invert(Tstate &in, Tstate &thstd_, 
					      Tstate &out) {
    //! invstd = 1 / thstd
    invmod.fprop(thstd_, invstd);
    //! out = in / thstd
    mcw.fprop(in, invstd, out);
  }

  template <typename T, class Tstate>
  void divisive_norm_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    //! clear derivatives
    insq.clear_dx();
    invar.clear_dx();
    instd.clear_dx();
    thstd.clear_dx();
    invstd.clear_dx();
    convvar.clear_dx();
    //! out = in/std
    mcw.bprop(in, invstd, out);
    //! 1/std
    invmod.bprop(thstd, invstd);
    //! std(std<mean(std)) = mean(std)
    thres.bprop(instd, thstd);
    //! sqrt(sum_j (w_j in^2))
    sqrtmod->bprop(invar, instd);
    //! sum_j (w_j in^2)
    convvar.bprop(insq, invar);
    //! in^2
    sqmod->bprop(in, insq);
  }

  template <typename T, class Tstate>
  void divisive_norm_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    insq.clear_ddx();
    invar.clear_ddx();
    instd.clear_ddx();
    thstd.clear_ddx();
    invstd.clear_ddx();
    convvar.clear_ddx();
    //! out = in/std
    mcw.bbprop(in, invstd, out);
    //! 1/std
    invmod.bbprop(thstd, invstd);
    //! std(std<mean(std)) = mean(std)
    thres.bbprop(instd, thstd);
    //! sqrt(sum_j (w_j in^2))
    sqrtmod->bbprop(invar, instd);
    //! sum_j (w_j in^2)
    convvar.bbprop(insq, invar);
    //! in^2
    sqmod->bbprop(in, insq);
  }

  template <typename T, class Tstate>
  void divisive_norm_module<T,Tstate>::dump_fprop(Tstate &in, Tstate &out) {  
    fprop(in, out);
    convvar.dump_fprop(insq, invar);
  }

  template <typename T, class Tstate>
  divisive_norm_module<T,Tstate>* divisive_norm_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    divisive_norm_module<T,Tstate> *d =
      new divisive_norm_module<T,Tstate>
      (kerdim, nfeatures, mirror, threshold, p, this->name(), across_features,
       cgauss, fsum_div, fsum_split, epsilon, epsilon2);
    if (!p) // assign same parameter state if no parameters were specified      
      d->divconv->kernel = divconv->kernel;
    return d;
  }
  
  template <typename T, class Tstate>
  std::string divisive_norm_module<T, Tstate>::describe() {
    std::string desc;
    desc << "divisive_norm module " << this->name() << " with kernel "
	 << kerdim << ", using " << (mirror ? "mirror" : "zero") << " padding"
	 << ", using " << (param ? "learned" : "fixed") 
	 << " filter " << gaussian_kernel;
    return desc;
  }

  template <typename T, class Tstate>
  void divisive_norm_module<T, Tstate>::set_epsilon(double eps) {
    epsilon = eps;
  }

  //////////////////////////////////////////////////////////////////////////////
  // subtractive_norm_module

  template <typename T, class Tstate>
  subtractive_norm_module<T,Tstate>::
  subtractive_norm_module(const char *name_) 
    : module_1_1<T,Tstate>(name_), convmean(true, name_) {
  }

  template <typename T, class Tstate>
  subtractive_norm_module<T,Tstate>::
  subtractive_norm_module(idxdim &kerdim_, int nf, bool mirror_,
			  bool global_norm_, parameter<T,Tstate> *param_,
			  const char *name_, bool af, double cgauss_,
			  bool fsum_div_, float fsum_split_)
    : module_1_1<T,Tstate>(name_), convmean(true, name_) {
    // common initializations
    init(kerdim_, nf, mirror_, global_norm_, param_, af, cgauss_, fsum_div_, 
	 fsum_split_);
    // local initializations
    meanconv = new convolution_module<T,Tstate>
      (param, kerdim, stride, conv_table, "subnorm_conv");
    set_kernel(gaussian_kernel);
    convmean.add_module(meanconv);
    //! feature sum module to sum along features 
    //! this might be implemented by making the table in above conv module
    //! all to all connection, but that would be very inefficient
    if (across_features)
      convmean.add_module(new fsum_module<T,Tstate>(fsum_div, fsum_split));
  }

  template <typename T, class Tstate>
  void subtractive_norm_module<T,Tstate>::
  init(idxdim &kerdim_, int nf, bool mirror_, bool global_norm_, 
       parameter<T,Tstate> *param_, bool af, double cgauss_,
       bool fsum_div_, float fsum_split_) {
    param = param_;
    global_norm = global_norm_;
    nfeatures = nf;
    kerdim = kerdim_;
    across_features = af;
    mirror = mirror_;
    cgauss = cgauss_;
    fsum_div = fsum_div_;
    fsum_split = fsum_split_;
    // create weighting kernel
    if (kerdim.order() != 2)
      eblerror("expected kernel dimensions with order 2 but found order "
	       << kerdim.order() << " in " << kerdim);
    gaussian_kernel = create_gaussian_kernel<T>(kerdim, cgauss);
    stride = idxdim(1, 1);
    // convolution table
    conv_table = one2one_table(nfeatures);
    // convvar    
    if (mirror) // switch between zero and mirror padding
      padding = new mirrorpad_module<T,Tstate>
	((gaussian_kernel.dim(0) - 1)/2, (gaussian_kernel.dim(1) - 1)/2);
    else padding = new zpad_module<T,Tstate>(gaussian_kernel.get_idxdim());
    convmean.add_module(padding);
  }

  template <typename T, class Tstate>
  void subtractive_norm_module<T,Tstate>::set_kernel(idx<T> &w) {
    // copy kernel to convolution param
    idx_bloop1(kx, meanconv->kernel.x, T) idx_copy(w, kx);
    // normalize the kernel    
    if (across_features)
      idx_dotc(meanconv->kernel.x, 1 / idx_sum(meanconv->kernel.x),
	       meanconv->kernel.x);
  }

  template <typename T, class Tstate>
  subtractive_norm_module<T,Tstate>::~subtractive_norm_module() {
  }
      
  template <typename T, class Tstate>
  bool subtractive_norm_module<T,Tstate>::
  optimize_fprop(Tstate &in, Tstate &out) {
    // memory optimization
    // if (false) {//hi && ho) { // dual buffers are provided, use them
    cout << "Using dual buffer memory optimization in subtractive_norm_module"
	 << endl;
    inmean = out;
    return true;
  }
  
  template <typename T, class Tstate>
  void subtractive_norm_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (global_norm) // global normalization
      idx_std_normalize(in.x, in.x, (T*) NULL);
    // inmean = sum_j (w_j * in_j)
    convmean.fprop(in, inmean);
    // inzmean = in - inmean
    difmod.fprop(in, inmean, out);
    EDEBUG(this->name() << ": out.x " << out.x << " min " << idx_min(out.x)
	   << " max " << idx_max(out.x));
  }

  template <typename T, class Tstate>
  void subtractive_norm_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    // clear derivatives
    inmean.clear_dx();
    convmean.clear_dx();
    // in - mean
    difmod.bprop(in, inmean, out);
    // sum_j (w_j * in_j)
    convmean.bprop(in, inmean);
  }

  template <typename T, class Tstate>
  void subtractive_norm_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    // clear derivatives
    inmean.clear_ddx();
    convmean.clear_ddx();
    //! in - mean
    difmod.bbprop(in, inmean, out);
    //! sum_j (w_j * in_j)
    convmean.bbprop(in, inmean);
  }

  template <typename T, class Tstate>
  void subtractive_norm_module<T,Tstate>::dump_fprop(Tstate &in, Tstate &out) {
    fprop(in, out);
    convmean.dump_fprop(in, inmean);
  }

  template <typename T, class Tstate>
  subtractive_norm_module<T,Tstate>* subtractive_norm_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    subtractive_norm_module<T,Tstate> *d = new subtractive_norm_module<T,Tstate>
      (kerdim, nfeatures, mirror, global_norm, p, this->name(),
       across_features, cgauss, fsum_div, fsum_split);
    if (!p) // assign same parameter state if no parameters were specified
      d->meanconv->kernel = meanconv->kernel;
    return d;
  }
  
  template <typename T, class Tstate>
  std::string subtractive_norm_module<T, Tstate>::describe() {
    std::string desc;
    desc << "subtractive_norm module with " << (param ? "learned" : "fixed")
	 << " mean weighting and kernel " << meanconv->kernel.x
	 << ", " << (global_norm ? "" : "not ")
	 << "using global normalization";
    return desc;
  }

  ////////////////////////////////////////////////////////////////
  // contrast_norm_module

  template <typename T, class Tstate>
  contrast_norm_module<T,Tstate>::contrast_norm_module(const char *name_)
    : module_1_1<T,Tstate>(name_), subnorm(NULL), divnorm(NULL) {
  }

  template <typename T, class Tstate>
  contrast_norm_module<T,Tstate>::
  contrast_norm_module(idxdim &kerdim, int nf, bool mirror, bool threshold,
		       bool gnorm_, parameter<T,Tstate> *param,
		       const char *name_, bool af, bool lm, double cgauss,
		       bool fsum_div, float fsum_split, double epsilon,
		       double epsilon2)
    : module_1_1<T,Tstate>(name_), learn_mean(lm) {
    subnorm = new subtractive_norm_module<T,Tstate>
      (kerdim, nf, mirror, gnorm_, lm ? param : NULL, name_, af, cgauss,
       fsum_div, fsum_split);
    divnorm = new divisive_norm_module<T,Tstate>
      (kerdim, nf, mirror, threshold, param, name_, af, cgauss,
       fsum_div, fsum_split, epsilon, epsilon2);
  }

  template <typename T, class Tstate>
  bool contrast_norm_module<T,Tstate>::optimize_fprop(Tstate &in, Tstate &out) {
    // memory optimization
    // if (false) {//hi && ho) { // dual buffers are provided, use them
    cout << "Using dual buffer memory optimization in contrast_norm_module"
	 << endl;
    return true;
  }
  
  template <typename T, class Tstate>
  contrast_norm_module<T,Tstate>::~contrast_norm_module() {
    if (divnorm) delete divnorm;
    if (subnorm) delete subnorm;
  }
      
  template <typename T, class Tstate>
  void contrast_norm_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    // subtractive normalization
    subnorm->fprop(in, tmp);
    // divisive normalization
    divnorm->fprop(tmp, out);
  }

  template <typename T, class Tstate>
  void contrast_norm_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    // clear derivatives
    tmp.clear_dx();
    // divisive normalization
    divnorm->bprop(tmp, out);
    // subtractive normalization
    subnorm->bprop(in, tmp);
  }

  template <typename T, class Tstate>
  void contrast_norm_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    tmp.clear_ddx();
    // divisive normalization
    divnorm->bbprop(tmp, out);
    // subtractive normalization
    subnorm->bbprop(in, tmp);
  }

  template <typename T, class Tstate>
  void contrast_norm_module<T,Tstate>::dump_fprop(Tstate &in, Tstate &out) {
    subnorm->dump_fprop(in, tmp);
    divnorm->dump_fprop(tmp, out);    
  }

  template <typename T, class Tstate>
  contrast_norm_module<T,Tstate>* contrast_norm_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    contrast_norm_module<T,Tstate> *d = new contrast_norm_module<T,Tstate>
      (divnorm->kerdim, divnorm->nfeatures, divnorm->mirror, divnorm->threshold,
       global_norm, p, this->name(), divnorm->across_features, learn_mean, 
       divnorm->cgauss, divnorm->fsum_div, divnorm->fsum_split, divnorm->epsilon,
       divnorm->epsilon2);
    if (!p) { // assign same parameter state if no parameters were specified
      d->divnorm->divconv->kernel = divnorm->divconv->kernel;
      d->subnorm->meanconv->kernel = subnorm->meanconv->kernel;
    }
    return d;
  }
  
  template <typename T, class Tstate>
  std::string contrast_norm_module<T, Tstate>::describe() {
    std::string desc;
    desc << "contrast_norm module with " << subnorm->describe()
	 << " and " << divnorm->describe();
    return desc;
  }

  template <typename T, class Tstate>
  void contrast_norm_module<T, Tstate>::set_epsilon(double eps) {
    divnorm->set_epsilon(eps);
  }

  ////////////////////////////////////////////////////////////////
  // laplacian_module

  template <typename T, class Tstate>
  laplacian_module<T,Tstate>::
  laplacian_module(int nf, bool mirror_, bool global_norm_, const char *name_)
    : module_1_1<T,Tstate>(name_), mirror(mirror_),
      global_norm(global_norm_), nfeatures(nf) {
    // create filter
    filter = create_burt_adelson_kernel<T>();
    idxdim kerdim = filter.get_idxdim();
    idxdim stride(1,1);
    // prepare convolution
    idx<intg> table = one2one_table(nfeatures);
    conv =
      new convolution_module<T,Tstate>(&param, kerdim, stride, table);
    idx_bloop1(kx, conv->kernel.x, T)
      idx_copy(filter, kx);
    // create modules
    if (mirror) // switch between zero and mirror padding
      pad = new mirrorpad_module<T,Tstate>((kerdim.dim(0) - 1)/2,
					   (kerdim.dim(1) - 1)/2);
    else
      pad = new zpad_module<T,Tstate>(filter.get_idxdim());
  }

  template <typename T, class Tstate>
  laplacian_module<T,Tstate>::~laplacian_module() {
    delete conv;
    delete pad;
  }
      
  template <typename T, class Tstate>
  void laplacian_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {  
    if (global_norm) // global normalization
      idx_std_normalize(in.x, in.x, (T*) NULL);
    if (in.x.order() != padded.x.order()
	|| in.x.order() != filtered.x.order()) {
      idxdim d(in.x.get_idxdim());
      d.setdims(1);
      padded = Tstate(d);
      filtered = Tstate(d);
    }
    pad->fprop(in, padded);
    conv->fprop(padded, filtered);
    diff.fprop(in, filtered, out);
  }

  template <typename T, class Tstate>
  laplacian_module<T,Tstate>* laplacian_module<T,Tstate>::copy() {
    return new laplacian_module<T,Tstate>(nfeatures, mirror, global_norm,
					  this->name());
  }
  
  template <typename T, class Tstate>
  std::string laplacian_module<T, Tstate>::describe() {
    std::string desc;
    desc << "laplacian module " << this->name()
	 << ", using " << (mirror ? "mirror" : "zero") << " padding"
	 << ", " << (global_norm ? "" : "not ")
	 << "using global normalization, using filter " << filter;
	 // << ": " << filter.str();
    return desc;
  }
  
} // end namespace ebl
