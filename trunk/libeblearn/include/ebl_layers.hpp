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
  // nn_layer_full

  template <class T>
  nn_layer_full<T>::nn_layer_full(parameter<T> &p, intg indim0, intg noutputs,
				  bool btanh_)
    : btanh(btanh_),
      linear(p, indim0, noutputs),
      adder(p, noutputs),
      sigmoid(btanh ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
    // the order of sum is not yet known and this is just an internal buffer
    // that does not need to be save in the parameter, so we allocate it later
    sum = NULL; 
  }

  template <class T>
  nn_layer_full<T>::nn_layer_full(intg indim0, intg noutputs, bool btanh_)
    : param(1), btanh(btanh_),
      linear(param, indim0, noutputs),
      adder(param, noutputs),
      sigmoid(btanh ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
    // the order of sum is not yet known and this is just an internal buffer
    // that does not need to be save in the parameter, so we allocate it later
    sum = NULL; 
  }

  template <class T>
  nn_layer_full<T>::~nn_layer_full() {
    if (sum) delete sum;
    if (sigmoid) delete sigmoid;
  }

  template <class T>
  void nn_layer_full<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // resize output and sum
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, adder.bias.x.dim(0)); // except for the first one
    if (!sum) sum = new state_idx<T>(d); // we now know the order of sum

    // fprop
    linear.fprop(in, *sum);
    adder.fprop(*sum, *sum);
    sigmoid->fprop(*sum, out);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(out.x, "dump_full_layer_out.x.mat");
#endif
  }

  template <class T>
  void nn_layer_full<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bprop(*sum, out);
    adder.bprop(*sum, *sum);
    linear.bprop(in, *sum);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(in.dx, "dump_full_layer_in.dx.mat");
#endif
  }

  template <class T>
  void nn_layer_full<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bbprop(*sum, out);
    adder.bbprop(*sum, *sum);
    linear.bbprop(in, *sum);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(in.ddx, "dump_full_layer_in.ddx.mat");
#endif
  }

  template <class T>
  void nn_layer_full<T>::forget(forget_param_linear &fp) {
    linear.forget(fp);
    adder.forget(fp);
  }

  template <class T>
  idxdim nn_layer_full<T>::fprop_size(idxdim &isize) {
    //! Extract its dimensions, update output size
    idxdim osize(linear.w.x.dim(0), isize.dim(1), isize.dim(2));
    isize = bprop_size(osize);
    return osize;
  }

  template <class T>
  idxdim nn_layer_full<T>::bprop_size(const idxdim &osize) {
    idxdim isize(linear.w.x.dim(1), osize.dim(1), osize.dim(2));
    return isize;
  }

  template <class T>
  nn_layer_full<T>* nn_layer_full<T>::copy() {
    // allocate
    nn_layer_full<T>* l2 =
      new nn_layer_full<T>(linear.w.x.dim(1), linear.w.x.dim(0), btanh);
    // copy data
    idx_copy(linear.w.x, l2->linear.w.x);
    idx_copy(adder.bias.x, l2->adder.bias.x);
    return l2;
  }

  ////////////////////////////////////////////////////////////////
  // nn_layer_convolution

  template <class T>
  nn_layer_convolution<T>::nn_layer_convolution(parameter<T> &p, 
						intg kerneli, intg kernelj, 
						intg stridei_, intg stridej_, 
						idx<intg> &tbl, bool btanh_) 
    : btanh(btanh_),
      convol(p, kerneli, kernelj, stridei_, stridej_, tbl), 
      adder(p, convol.thickness),
      sigmoid(btanh ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
    sum = NULL;
  }

  template <class T>
  nn_layer_convolution<T>::nn_layer_convolution(intg kerneli, intg kernelj, 
						intg stridei_, intg stridej_, 
						idx<intg> &tbl, bool btanh_) 
    : param(1), convol(param, kerneli, kernelj, stridei_, stridej_, tbl), 
      adder(param, convol.thickness),
      sigmoid(btanh ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
    sum = NULL;
  }

  template <class T>
  nn_layer_convolution<T>::~nn_layer_convolution() {
    if (sum) delete sum;
    if (sigmoid) delete sigmoid;
  }

  template <class T>
  void nn_layer_convolution<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // 1. allocate sum
    idxdim d(in.x.spec); // use same dimensions as in
    if (!sum) sum = new state_idx<T>(d);

    // 2. fprop
    sum->clear();
    convol.fprop(in, *sum);
    adder.fprop(*sum, *sum);
    sigmoid->fprop(*sum, out);
#ifdef __DUMP_STATES__ // used to debug
    ostringstream fname;
    fname << "dump_convolution_layer_out.x_" << out.x << ".mat";
    save_matrix(out.x, fname.str());
    fname.str("");
    fname << "dump_convolution_layer_ker.x_" << convol.kernel.x << ".mat";
    save_matrix(convol.kernel.x, fname.str());
#endif
  }

  template <class T>
  void nn_layer_convolution<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bprop(*sum, out);
    adder.bprop(*sum, *sum);
    convol.bprop(in, *sum);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(in.dx, "dump_convolution_layer_in.dx.mat");
#endif
  }

  template <class T>
  void nn_layer_convolution<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bbprop(*sum, out);
    adder.bbprop(*sum, *sum);
    convol.bbprop(in, *sum);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(in.ddx, "dump_convolution_layer_in.ddx.mat");
#endif
  }

  template <class T>
  void nn_layer_convolution<T>::forget(forget_param_linear &fp) {
    convol.forget(fp);
    adder.forget(fp);
  }

  template <class T>
  idxdim nn_layer_convolution<T>::fprop_size(idxdim &isize) {
    //! Select a kernel
    idxdim kernel_size = convol.kernel.x[0].get_idxdim();
    //! Extract its dimensions, update output size
    idxdim osize(convol.thickness,
		 MAX(1, isize.dim(1) - kernel_size.dim(0) + 1),
		 MAX(1, isize.dim(2) - kernel_size.dim(1) + 1));
    isize = bprop_size(osize);
    return osize;
  }

  template <class T>
  idxdim nn_layer_convolution<T>::bprop_size(const idxdim &osize) {
    //! Select a kernel
    idxdim kernel_size = convol.kernel.x[0].get_idxdim();
    //! Extract its dimensions, update output size
    idxdim isize(convol.tablemax + 1,
		 osize.dim(1) + kernel_size.dim(0) - 1,
		 osize.dim(2) + kernel_size.dim(1) - 1);
    return isize;
  }

  template <class T>
  nn_layer_convolution<T>* nn_layer_convolution<T>::copy() {
    // allocate
    nn_layer_convolution<T> *l2 = new nn_layer_convolution<T>
      (convol.kernel.x.dim(1), convol.kernel.x.dim(2), convol.stridei,
       convol.stridej, convol.table, btanh);
    // copy data
    idx_copy(convol.kernel.x, l2->convol.kernel.x);
    idx_copy(adder.bias.x, l2->adder.bias.x);
    return l2;
  }

  ////////////////////////////////////////////////////////////////
  // layer_convabsnorm

  template <class T>
  layer_convabsnorm<T>::layer_convabsnorm(parameter<T> &p, 
					  intg kerneli, intg kernelj, 
					  intg stridei_, intg stridej_, 
					  idx<intg> &tbl, bool mirror,
					  bool btanh) 
    : lconv(p, kerneli, kernelj, stridei_, stridej_, tbl, btanh),
      abs(), norm(kerneli, kernelj, lconv.convol.thickness, mirror),
      tmp(NULL), tmp2(NULL) {
  }

  template <class T>
  layer_convabsnorm<T>::layer_convabsnorm(intg kerneli, intg kernelj, 
					  intg stridei_, intg stridej_, 
					  idx<intg> &tbl, bool mirror,
					  bool btanh) 
    : param(1), lconv(param, kerneli, kernelj, stridei_, stridej_, tbl, btanh),
      abs(), norm(kerneli, kernelj, lconv.convol.thickness, mirror),
      tmp(NULL), tmp2(NULL) {
  }

  template <class T>
  layer_convabsnorm<T>::~layer_convabsnorm() {
    if (tmp) delete tmp;
    if (tmp2) delete tmp2;
  }

  template <class T>
  void layer_convabsnorm<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // 1. resize tmp
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, lconv.convol.thickness); // except for the first one
    if (!tmp) tmp = new state_idx<T>(d);
    if (!tmp2) tmp2 = new state_idx<T>(d);

    // 2. fprop
    tmp->clear();
    tmp2->clear();
    lconv.fprop(in, *tmp);
    abs.fprop(*tmp, *tmp2);
    norm.fprop(*tmp2, out);
  }

  template <class T>
  void layer_convabsnorm<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    norm.bprop(*tmp2, out);
    abs.bprop(*tmp, *tmp2);
    lconv.bprop(in, *tmp);
  }

  template <class T>
  void layer_convabsnorm<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    norm.bbprop(*tmp2, out);
    abs.bbprop(*tmp, *tmp2);
    lconv.bbprop(in, *tmp);
  }

  template <class T>
  void layer_convabsnorm<T>::forget(forget_param_linear &fp) {
    lconv.forget(fp);
  }

  template <class T>
  idxdim layer_convabsnorm<T>::fprop_size(idxdim &isize) {
    return lconv.fprop_size(isize);
  }

  template <class T>
  idxdim layer_convabsnorm<T>::bprop_size(const idxdim &osize) {
    return lconv.bprop_size(osize);
  }

  template <class T>
  layer_convabsnorm<T>* layer_convabsnorm<T>::copy() {
    // allocate
    layer_convabsnorm<T> *l2 = new layer_convabsnorm<T>
      (lconv.convol.kernel.x.dim(1), lconv.convol.kernel.x.dim(2),
       lconv.convol.stridei, lconv.convol.stridej, lconv.convol.table,
       norm.mirror);
    // copy data
    idx_copy(lconv.convol.kernel.x, l2->lconv.convol.kernel.x);
    idx_copy(lconv.adder.bias.x, l2->lconv.adder.bias.x);
    return l2;
  }

  ////////////////////////////////////////////////////////////////
  // nn_layer_subsampling

  template <class T>
  nn_layer_subsampling<T>::nn_layer_subsampling(parameter<T> &p,
						intg stridei, intg stridej,
						intg subi, intg subj, 
						intg thick, bool btanh_)
    : btanh(btanh_),
      subsampler(p, stridei, stridej, subi, subj, thick), adder(p, thick),
      sigmoid(btanh ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
    sum = NULL;
  }

  template <class T>
  nn_layer_subsampling<T>::nn_layer_subsampling(intg stridei, intg stridej,
						intg subi, intg subj, 
						intg thick, bool btanh_)
    : param(1),
      btanh(btanh_),
      subsampler(param, stridei, stridej, subi, subj, thick),
      adder(param, thick),
      sigmoid(btanh ? (module_1_1<T>*) new tanh_module<T>()
	      : (module_1_1<T>*) new stdsigmoid_module<T>()) {
    sum = NULL;
  }

  template <class T>
  nn_layer_subsampling<T>::~nn_layer_subsampling() {
    if (sum) delete sum;
    if (sigmoid) delete sigmoid;
  }

  template <class T>
  void nn_layer_subsampling<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    // 1. resize sum
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, subsampler.thickness); // except for the first one
    if (!sum) sum = new state_idx<T>(d);

    // 2. fprop
    subsampler.fprop(in, *sum);
    adder.fprop(*sum, *sum);
    sigmoid->fprop(*sum, out);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(out.x, "dump_subsampling_layer_out.x.mat");
#endif
  }

  template <class T>
  void nn_layer_subsampling<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bprop(*sum, out);
    adder.bprop(*sum, *sum);
    subsampler.bprop(in, *sum);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(in.dx, "dump_subsampling_layer_in.dx.mat");
#endif
  }

  template <class T>
  void nn_layer_subsampling<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    sigmoid->bbprop(*sum, out);
    adder.bbprop(*sum, *sum);
    subsampler.bbprop(in, *sum);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(in.ddx, "dump_subsampling_layer_in.ddx.mat");
#endif
  }

  template <class T>
  void nn_layer_subsampling<T>::forget(forget_param_linear &fp) {
    subsampler.forget(fp);
    adder.forget(fp);
  }

  template <class T>
  idxdim nn_layer_subsampling<T>::fprop_size(idxdim &isize) {
    //! Update input size
    idxdim osize(subsampler.thickness,
		 MAX(1, isize.dim(1) / subsampler.stridei),
		 MAX(1, isize.dim(2) / subsampler.stridej));
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(osize);
    return osize;
  }

  template <class T>
  idxdim nn_layer_subsampling<T>::bprop_size(const idxdim &osize) {
    //! Update input size
    idxdim isize(subsampler.thickness,
		 osize.dim(1) * subsampler.stridei,
		 osize.dim(2) * subsampler.stridej);
    return isize;
  }

  template <class T>
  nn_layer_subsampling<T>* nn_layer_subsampling<T>::copy() {
    // allocate
    nn_layer_subsampling<T> *l2 = new nn_layer_subsampling<T>
      (subsampler.stridei, subsampler.stridej, subsampler.sub.x.dim(1),
       subsampler.sub.x.dim(2), subsampler.thickness);
    // copy data
    idx_copy(subsampler.coeff.x, l2->subsampler.coeff.x);
    idx_copy(adder.bias.x, l2->adder.bias.x);
    return l2;
  }

} // end namespace ebl
