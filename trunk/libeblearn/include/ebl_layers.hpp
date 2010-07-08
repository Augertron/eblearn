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

namespace ebl {

  ////////////////////////////////////////////////////////////////////////
  // full_layer

  template <class T>
  full_layer<T>::full_layer(parameter<T> *p, intg indim0, intg noutputs,
			    bool btanh_, const char *name_)
    : btanh(btanh_),
      linear(p, indim0, noutputs, name_),
      adder(p, noutputs, name_),
      sigmoid(btanh_ ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
    // the order of sum is not yet known and this is just an internal buffer
    // that does not need to be save in the parameter, so we allocate it later
    sum = NULL; 
    this->name = name_;
  }

  template <class T>
  full_layer<T>::~full_layer() {
    if (sum) delete sum;
    if (sigmoid) delete sigmoid;
  }

  template <class T>
  void full_layer<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // resize output and sum
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, adder.bias.x.dim(0)); // except for the first one
    if (!sum) sum = new state_idx<T>(d); // we now know the order of sum

    // fprop
    linear.fprop(in, *sum);
    adder.fprop(*sum, *sum);
    sigmoid->fprop(*sum, out);
  }

  template <class T>
  void full_layer<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bprop(*sum, out);
    adder.bprop(*sum, *sum);
    linear.bprop(in, *sum);
  }

  template <class T>
  void full_layer<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bbprop(*sum, out);
    adder.bbprop(*sum, *sum);
    linear.bbprop(in, *sum);
  }

  template <class T>
  void full_layer<T>::forget(forget_param_linear &fp) {
    linear.forget(fp);
    adder.forget(fp);
  }

  template <class T>
  idxdim full_layer<T>::fprop_size(idxdim &isize) {
    return linear.fprop_size(isize);
  }

  template <class T>
  idxdim full_layer<T>::bprop_size(const idxdim &osize) {
    return linear.bprop_size(osize);
  }

  template <class T>
  full_layer<T>* full_layer<T>::copy() {
    // allocate
    full_layer<T>* l2 =
      new full_layer<T>(NULL, linear.w.x.dim(1), linear.w.x.dim(0), btanh);
    // copy data
    idx_copy(linear.w.x, l2->linear.w.x);
    idx_copy(adder.bias.x, l2->adder.bias.x);
    return l2;
  }

  ////////////////////////////////////////////////////////////////
  // convolution_layer

  template <class T>
  convolution_layer<T>::convolution_layer(parameter<T> *p, 
					  intg kerneli, intg kernelj, 
					  intg stridei_, intg stridej_, 
					  idx<intg> &tbl, bool btanh_,
					  const char *name_) 
    : btanh(btanh_),
      convol(p, kerneli, kernelj, stridei_, stridej_, tbl, name_), 
      adder(p, convol.thickness, name_),
      sigmoid(btanh_ ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
    sum = NULL;
    this->name = name_;
  }

  template <class T>
  convolution_layer<T>::~convolution_layer() {
    if (sum) delete sum;
    if (sigmoid) delete sigmoid;
  }

  template <class T>
  void convolution_layer<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // 1. allocate sum
    idxdim d(in.x.spec); // use same dimensions as in
    if (!sum) sum = new state_idx<T>(d);

    // 2. fprop
    //    sum->clear();
    convol.fprop(in, *sum);
    adder.fprop(*sum, *sum);
    sigmoid->fprop(*sum, out);
  }

  template <class T>
  void convolution_layer<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bprop(*sum, out);
    adder.bprop(*sum, *sum);
    convol.bprop(in, *sum);
  }

  template <class T>
  void convolution_layer<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bbprop(*sum, out);
    adder.bbprop(*sum, *sum);
    convol.bbprop(in, *sum);
  }

  template <class T>
  void convolution_layer<T>::forget(forget_param_linear &fp) {
    convol.forget(fp);
    adder.forget(fp);
  }

  template <class T>
  idxdim convolution_layer<T>::fprop_size(idxdim &isize) {
    return convol.fprop_size(isize);
  }

  template <class T>
  idxdim convolution_layer<T>::bprop_size(const idxdim &osize) {
    return convol.bprop_size(osize);
  }

  template <class T>
  convolution_layer<T>* convolution_layer<T>::copy() {
    // allocate
    convolution_layer<T> *l2 = new convolution_layer<T>
      (NULL, convol.kernel.x.dim(1), convol.kernel.x.dim(2), convol.stridei,
       convol.stridej, convol.table, btanh);
    // copy data
    idx_copy(convol.kernel.x, l2->convol.kernel.x);
    idx_copy(adder.bias.x, l2->adder.bias.x);
    return l2;
  }

  ////////////////////////////////////////////////////////////////
  // convabsnorm_layer

  template <class T>
  convabsnorm_layer<T>::convabsnorm_layer(parameter<T> *p, 
					  intg kerneli, intg kernelj, 
					  intg stridei_, intg stridej_, 
					  idx<intg> &tbl, bool mirror,
					  bool btanh_, const char *name_) 
    : btanh(btanh_),
      lconv(p, kerneli, kernelj, stridei_, stridej_, tbl, btanh_, name_),
      abs(), norm(kerneli, kernelj, lconv.convol.thickness, name_, mirror),
      tmp(NULL), tmp2(NULL) {
    this->name = name_;
  }

  template <class T>
  convabsnorm_layer<T>::~convabsnorm_layer() {
    if (tmp) delete tmp;
    if (tmp2) delete tmp2;
  }

  template <class T>
  void convabsnorm_layer<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // 1. resize tmp
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, lconv.convol.thickness); // except for the first one
    if (!tmp) tmp = new state_idx<T>(d);
    if (!tmp2) tmp2 = new state_idx<T>(d);

    // 2. fprop
    // tmp->clear();
    // tmp2->clear();
    lconv.fprop(in, *tmp);
    abs.fprop(*tmp, *tmp2);
    norm.fprop(*tmp2, out);
  }

  template <class T>
  void convabsnorm_layer<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    norm.bprop(*tmp2, out);
    abs.bprop(*tmp, *tmp2);
    lconv.bprop(in, *tmp);
  }

  template <class T>
  void convabsnorm_layer<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    norm.bbprop(*tmp2, out);
    abs.bbprop(*tmp, *tmp2);
    lconv.bbprop(in, *tmp);
  }

  template <class T>
  void convabsnorm_layer<T>::forget(forget_param_linear &fp) {
    lconv.forget(fp);
  }

  template <class T>
  idxdim convabsnorm_layer<T>::fprop_size(idxdim &isize) {
    return lconv.fprop_size(isize);
  }

  template <class T>
  idxdim convabsnorm_layer<T>::bprop_size(const idxdim &osize) {
    return lconv.bprop_size(osize);
  }

  template <class T>
  convabsnorm_layer<T>* convabsnorm_layer<T>::copy() {
    // allocate
    convabsnorm_layer<T> *l2 = new convabsnorm_layer<T>
      (NULL, lconv.convol.kernel.x.dim(1), lconv.convol.kernel.x.dim(2),
       lconv.convol.stridei, lconv.convol.stridej, lconv.convol.table,
       norm.mirror, btanh);
    // copy data
    idx_copy(lconv.convol.kernel.x, l2->lconv.convol.kernel.x);
    idx_copy(lconv.adder.bias.x, l2->lconv.adder.bias.x);
    return l2;
  }

  ////////////////////////////////////////////////////////////////
  // subsampling_layer

  template <class T>
  subsampling_layer<T>::subsampling_layer(parameter<T> *p,
					  intg stridei, intg stridej,
					  intg subi, intg subj, 
					  intg thick, bool btanh_,
					  const char *name_)
    : btanh(btanh_),
      subsampler(p, stridei, stridej, subi, subj, thick, name_),
      adder(p, thick, name_),
      sigmoid(btanh_ ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
    sum = NULL;
    this->name = name_;
  }

  template <class T>
  subsampling_layer<T>::~subsampling_layer() {
    if (sum) delete sum;
    if (sigmoid) delete sigmoid;
  }

  template <class T>
  void subsampling_layer<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // 1. resize sum
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, subsampler.thickness); // except for the first one
    if (!sum) sum = new state_idx<T>(d);

    // 2. fprop
    subsampler.fprop(in, *sum);
    adder.fprop(*sum, *sum);
    sigmoid->fprop(*sum, out);
  }

  template <class T>
  void subsampling_layer<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bprop(*sum, out);
    adder.bprop(*sum, *sum);
    subsampler.bprop(in, *sum);
  }

  template <class T>
  void subsampling_layer<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bbprop(*sum, out);
    adder.bbprop(*sum, *sum);
    subsampler.bbprop(in, *sum);
  }

  template <class T>
  void subsampling_layer<T>::forget(forget_param_linear &fp) {
    subsampler.forget(fp);
    adder.forget(fp);
  }

  template <class T>
  idxdim subsampling_layer<T>::fprop_size(idxdim &isize) {
    return subsampler.fprop_size(isize);
  }

  template <class T>
  idxdim subsampling_layer<T>::bprop_size(const idxdim &osize) {
    return subsampler.bprop_size(osize);
  }

  template <class T>
  subsampling_layer<T>* subsampling_layer<T>::copy() {
    // allocate
    subsampling_layer<T> *l2 = new subsampling_layer<T>
      (NULL, subsampler.stridei, subsampler.stridej, subsampler.sub.x.dim(1),
       subsampler.sub.x.dim(2), subsampler.thickness, btanh);
    // copy data
    idx_copy(subsampler.coeff.x, l2->subsampler.coeff.x);
    idx_copy(adder.bias.x, l2->adder.bias.x);
    return l2;
  }

} // end namespace ebl
