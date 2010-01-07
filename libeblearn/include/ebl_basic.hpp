/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun, Pierre Sermanet *
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

  ////////////////////////////////////////////////////////////////
  // linear_module

  template <class T>
  linear_module<T>::linear_module(parameter<T> &p, intg in, intg out)
    : w(p, out, in) {
  }

  template <class T>
  linear_module<T>::~linear_module() {
  }

  template <class T>
  void linear_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    idx<T> inx = in.x.view_as_order(1); // view as 1D idx
    idx<T> outx = out.x.view_as_order(1); // view as 1D idx

    idx_m2dotm1(w.x, inx, outx); // linear combination
  }

  template <class T>
  void linear_module<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    idx<T> inx = in.x.view_as_order(1); // view as 1D idx
    idx<T> indx = in.dx.view_as_order(1); // view as 1D idx
    idx<T> outdx = out.dx.view_as_order(1); // view as 1D idx
    idx<T> twx(w.x.transpose(0, 1)); // switch dimensions 0 and 1
    if (outdx.nelements() != w.dx.dim(0)) eblerror("output has wrong size");

    idx_m1extm1acc(outdx, inx, w.dx); // backprop to weights
    idx_m2dotm1acc(twx, outdx, indx); // backprop to input
  }

  template <class T>
  void linear_module<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    idx<T> inx = in.x.view_as_order(1); // view as 1D idx
    idx<T> inddx = in.ddx.view_as_order(1); // view as 1D idx
    idx<T> outddx = out.ddx.view_as_order(1); // view as 1D idx
    idx<T> twx = w.x.transpose(0, 1); // switch dimensions 0 and 1
    if (outddx.nelements() != w.ddx.dim(0)) eblerror("output has wrong size");

    idx_m1squextm1acc(outddx, inx, w.ddx); // backprop to weights
    idx_m2squdotm1acc(twx, outddx, inddx); // backprop to input
  }

  template <class T>
  void linear_module<T>::forget(forget_param_linear &fp) {
    double fanin = w.x.dim(1);
    double z = fp.value / pow(fanin, fp.exponent);
    check_drand_ini(); // check that the random seed was initialized
    idx_aloop1(lx, w.x, T)
      {	*lx = (T) drand(-z, z);}
  }

  template <class T>
  void linear_module<T>::normalize() {
    norm_columns(w.x);
  }

  template <class T>
  void linear_module<T>::resize_output(state_idx<T> &in, state_idx<T> &out) {
    // resize output based on input dimensions
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, w.x.dim(0)); // except for the first one
    out.resize(d);
  }

  ////////////////////////////////////////////////////////////////
  // convolution_module_2D

  template <class T>
  convolution_module_2D<T>::convolution_module_2D(parameter<T> &p, 
						  intg kerneli, intg kernelj, 
						  intg stridei_, intg stridej_, 
						  idx<intg> &tbl)
    : warnings_shown(false), kernel(p, tbl.dim(0), kerneli, kernelj), 
      stridei(stridei_), stridej(stridej_), table(tbl) {
    // check sanity of connection table
    if (table.dim(1) != 2) { // check table order
      cerr << "error: expecting a table with dim 1 equal to 2 but found: ";
      cerr << table << endl;
      eblerror("connection table error");
    }
    idx<intg> tbl0 = table.select(1, 0);
    tablemax = idx_max(tbl0);
    idx<intg> tbl1 = table.select(1, 1);
    thickness = idx_max(tbl1) + 1;
    // check table uses all inputs
    idx<bool> tblcount(tablemax + 1);
    idx_bloop1(tb, table, intg) {
      tblcount.set(true, tb.get(0));
    }
    for (int i = 0; i <= tablemax; ++i) {
      if (tblcount.get(i) == false) {
	cerr << "warning: input " << i;
	cerr << " not used by connection table in convolution module." << endl;
      }
    }
  }

  template <class T>
  convolution_module_2D<T>::~convolution_module_2D() {
  }

  template <class T>
  void convolution_module_2D<T>::fprop(state_idx<T> &in, state_idx<T> &out){
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    // unfolding input for a faster convolution operation
    idx<T> uuin(in.x.unfold(1, kernel.x.dim(1), stridei));
    uuin = uuin.unfold(2, kernel.x.dim(2), stridej);
    idx_clear(out.x);
    // convolve 2D slice for each convolution kernel
    { idx_bloop2(lk, kernel.x, T, lt, table, intg) {
	idx<T> suin(uuin.select(0, lt.get(0)));
	idx<T> sout((out.x).select(0, lt.get(1)));

	idx_m4dotm2acc(suin, lk, sout); // 2D convolution
      }}
  }
  
  template <class T>
  void convolution_module_2D<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    // backprop through convolution
    idx_clear(in.dx);
    idx<T> uuin(in.dx.unfold(1, (kernel.dx).dim(1), stridei));
    uuin = uuin.unfold(2, (kernel.dx).dim(2), stridej);
    idx<T> uuinf(in.x.unfold(1, (kernel.dx).dim(1), stridei));
    uuinf = uuinf.unfold(2, (kernel.dx).dim(2), stridej);
    int transp[5] = { 0, 3, 4, 1, 2 };
    idx<T> borp(uuinf.transpose(transp));
    { idx_bloop3 (lk, kernel.dx, T, lkf, kernel.x, T, 
		  lt, table, intg) {
	intg islice = lt.get(0);
	idx<T> suin(uuin.select(0, islice));
	idx<T> sborp(borp.select(0, islice));
	idx<T> sout(out.dx.select(0, lt.get(1)));

	idx_m2extm2acc(sout, lkf, suin); // backward convolution
	idx_m4dotm2acc(sborp, sout, lk); // compute gradient for kernel
      }}
  }

  template <class T>
  void convolution_module_2D<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    // backprop through convolution
    idx_clear(in.ddx);
    idx<T> uuin(in.ddx.unfold(1, (kernel.ddx).dim(1), stridei));
    uuin = uuin.unfold(2, (kernel.ddx).dim(2), stridej);
    idx<T> uuinf(in.x.unfold(1, (kernel.ddx).dim(1), stridei));
    uuinf = uuinf.unfold(2, (kernel.ddx).dim(2), stridej);
    int transp[5] = { 0, 3, 4, 1, 2 };
    idx<T> borp(uuinf.transpose(transp));
    { idx_bloop3 (lk, kernel.ddx, T, lkf, kernel.x, T, 
		  lt, table, intg) {
	intg islice = lt.get(0);
	idx<T> suin(uuin.select(0, islice));
	idx<T> sborp(borp.select(0, islice));
	idx<T> sout((out.ddx).select(0, lt.get(1)));
	    
	idx_m2squextm2acc(sout, lkf, suin); // backward convolution
	idx_m4squdotm2acc(sborp, sout, lk); // compute gradient for kernel
      }}
  }

  template <class T>
  void convolution_module_2D<T>::forget(forget_param_linear &fp) {
    idx<T> kx(kernel.x);
    intg vsize = kx.dim(1);
    intg hsize = kx.dim(2);
    idx<intg> ts(table.select(1, 1));
    idx<int> fanin(1 + idx_max(ts));
    check_drand_ini();
    idx_clear(fanin);
    { idx_bloop1(tab, table, intg)	{
	fanin.set(1 + fanin.get(tab.get(1)), tab.get(1)); }}
    { idx_bloop2(tab, table, intg, x, kx, T) {
	double s = fp.value / pow((vsize * hsize * fanin.get(tab.get(1))), 
				  fp.exponent);
	{ idx_bloop1(lx, x, T) {
	    { idx_bloop1(llx, lx, T) {
		T n = (T) drand(-s, s);
		llx.set(n);
	      }}
	  }}
      }}
  }

  template <class T>
  void convolution_module_2D<T>::resize_output(state_idx<T> &in,
					       state_idx<T> &out) {
    intg ki = kernel.x.dim(1);
    intg kj = kernel.x.dim(2);
    intg sini = in.x.dim(1);
    intg sinj = in.x.dim(2);

    // check input size for table
    if (in.x.dim(0) < tablemax + 1) {
      cerr << "error: expecting input with size " << tablemax + 1;
      cerr << " in dimension 0 but found: " << in.x << endl;
      eblerror("input size error");
    }
    if (!warnings_shown && (in.x.dim(0) > tablemax + 1)) {
      warnings_shown = true;
      cerr << "warning: convolution connection table is not using all inputs,";
      cerr << " the maximum input index used by the table is " << tablemax;
      cerr << " but the input is " << in.x << endl;
    }
    // check sizes
    if (((sini - (ki - stridei)) % stridei != 0) || 
	((sinj - (kj - stridej)) % stridej != 0))
      eblerror("inconsistent input size, kernel size, and subsampling ratio.");
    if ((stridei != 1) || (stridej != 1))
      eblerror("stride > 1 not implemented yet.");
    // unfolding input for a faster convolution operation
    idx<T> uuin(in.x.unfold(1, ki, stridei));
    uuin = uuin.unfold(2, kj, stridej);
    // resize output based in input dimensions
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, thickness); // except for the first one
    d.setdim(1, uuin.dim(1)); // convolution trims dimensions a bit
    d.setdim(2, uuin.dim(2)); // convolution trims dimensions a bit
    out.resize(d);
  }

  ////////////////////////////////////////////////////////////////
  // subsampling_module_2D

  template <class T>
  subsampling_module_2D<T>::subsampling_module_2D(parameter<T> &p, 
						  intg stridei_, intg stridej_,
						  intg subi, intg subj, 
						  intg thick)
    : coeff(p, thick), sub(thick, subi, subj), thickness(thick), 
      stridei(stridei_), stridej(stridej_) {
  }

  template <class T>
  subsampling_module_2D<T>::~subsampling_module_2D() {
  }

  template <class T>
  void subsampling_module_2D<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    // subsampling ( coeff * average )
    idx_clear(sub.x);
    { idx_bloop4(lix, in.x, T, lsx, sub.x, T, lcx, coeff.x, T, ltx, out.x, T) {
	idx<T> uuin(lix.unfold(1, stridej, stridej));
	uuin = uuin.unfold(0, stridei, stridei);
	idx_eloop1(z1, uuin, T) {
	  idx_eloop1(z2, z1, T) {
	    idx_add(z2, lsx, lsx); // average
	  }
	}
	idx_dotc(lsx, lcx.get(), ltx); // coeff
      }}
  }

  template <class T>
  void subsampling_module_2D<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    // oversampling
    idx_bloop3(lcdx, coeff.dx, T, ltdx, out.dx, T, lsx, sub.x, T) {
      idx_dotacc(lsx, ltdx, lcdx);
    }
    idx_bloop4(lidx, in.dx, T, lsdx, sub.dx, T,
	       lcx, coeff.x, T, ltdx2, out.dx, T) {
      idx_dotc(ltdx2, lcx.get(), lsdx);
      idx_m2oversample(lsdx, stridei, stridej, lidx);
    }
  }

  template <class T>
  void subsampling_module_2D<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {	
    // oversampling
    idx_bloop3(lcdx, coeff.ddx, T, ltdx, out.ddx, T, lsx, sub.x, T) {
      idx_m2squdotm2acc(lsx, ltdx, lcdx);
    }
    idx_bloop4(lidx, in.ddx, T, lsdx, sub.ddx, T,
	       lcx, coeff.x, T, ltdx2, out.ddx, T) {
      T cf = lcx.get();
      idx_dotc(ltdx2, cf * cf, lsdx);
      idx_m2oversample(lsdx, stridei, stridej, lidx);
    }
  }

  template <class T>
  void subsampling_module_2D<T>::forget(forget_param_linear &fp) {
    double c = fp.value / pow(stridei * stridej, fp.exponent);
    idx_fill(coeff.x, (T) c);
  }

  template <class T>
  void subsampling_module_2D<T>::resize_output(state_idx<T> &in,
					       state_idx<T> &out) {
    intg sin_i = in.x.dim(1);
    intg sin_j = in.x.dim(2);
    intg si = sin_i / stridei;
    intg sj = sin_j / stridej;
    // check sizes
    if ((sin_i % stridei) != 0 || (sin_j % stridej) != 0)
      eblerror("inconsistent input size and subsampling ratio");
    // resize output and sub based in input dimensions
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(1, si); // new size after subsampling
    d.setdim(2, sj); // new size after subsampling
    out.resize(d);
    sub.resize(d);
  }

  ////////////////////////////////////////////////////////////////
  // addc_module

  template <class T>
  addc_module<T>::addc_module(parameter<T> &p, intg size)
    : bias(p, size) {
  }

  template <class T>
  addc_module<T>::~addc_module() {
  }

  template <class T>
  void addc_module<T>::fprop(state_idx<T>& in, state_idx<T>& out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x.spec); // use same dimensions as in
      d.setdim(0, bias.x.dim(0)); // except for the first one
      out.resize(d);
    }

    // add each bias to entire slices cut from the first dimension
    idx_bloop3(inx, in.x, T, biasx, bias.x, T, outx, out.x, T)
      {
	idx_addc(inx, biasx.get(), outx);
      }
  }

  template <class T>
  void addc_module<T>::bprop(state_idx<T>& in, state_idx<T>& out) {
    if ((&in != &out) && (in.dx.nelements() != out.dx.nelements()))
      eblerror("output has wrong size");
    
    idx_bloop3(indx, in.dx, T, biasdx, bias.dx, T, 
	       outdx, out.dx, T) {
      if (&in != &out)
	idx_copy(outdx, indx); // only pass on info if necessary
      idx_sumacc(outdx, biasdx);
    }
  }

  template <class T>
  void addc_module<T>::bbprop(state_idx<T>& in, state_idx<T>& out) {
    if ((&in != &out) && (in.ddx.nelements() != out.ddx.nelements()))
      eblerror("output has wrong size");
    
    idx_bloop3(inddx, in.ddx, T, biasddx, bias.ddx, T, 
	       outddx, out.ddx, T) {
      if (&in != &out)
	idx_copy(outddx, inddx); // only pass on info if necessary
      idx_sumacc(outddx, biasddx);
    }
  }
  
  template <class T>
  void addc_module<T>::forget(forget_param_linear& fp) {
    idx_clear(bias.x);
  }

  ////////////////////////////////////////////////////////////////
  // power_module

  template <class T>
  power_module<T>::power_module(T p_) : p(p_) {
  }

  template <class T>
  power_module<T>::~power_module() {
  }
    
  template <class T>
  void power_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      out.resize(d);
    }
    idx_power(in.x, p, out.x);
  }
  
  template <class T>
  void power_module<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions

    if (!tt.same_dim(in.x.get_idxdim())) { // resize temp buffer
      idxdim d(in.x);
      tt = idx<T>(d);
    }
    // compute derivative
    idx_div(out.x, in.x, tt); //! tt = x^(p-1)
    idx_mul(out.dx, tt, tt); //! tt = outdx*x^(p-1)
    idx_dotcacc(tt, p, in.dx);
  }

  template <class T>
  void power_module<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions

    if (!tt.same_dim(in.x.get_idxdim())) { // resize temp buffer
      idxdim d(in.x);
      tt = idx<T>(d);
    }
    // compute 2nd derivative
    idx_div(out.x, in.x, tt); //! tt = x^(p-1)
    idx_mul(tt, tt, tt); //! tt = (x^(p-1))^2
    idx_mul(out.ddx, tt, tt); //! tt = outddx*(x^(p-1))^2
    idx_dotcacc(tt, (p * p), in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // diff_module

  template <class T>
  diff_module<T>::diff_module() {
  }
  
  template <class T>
  diff_module<T>::~diff_module() {
  }

  template <class T>
  void diff_module<T>::fprop(state_idx<T> &in1, state_idx<T> &in2, state_idx<T> &out) {
    if (&in1 != &out) { // resize only when input and output are different
      idxdim d(in1.x); // use same dimensions as in
      out.resize(d);
    }
    idx_sub(in1.x, in2.x, out.x);
  }

  template <class T>
  void diff_module<T>::bprop(state_idx<T> &in1, state_idx<T> &in2, state_idx<T> &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.dx, in2.dx, out.dx);// must have same dimensions

    idx_add(in1.dx, out.dx, in1.dx); // derivative wrt in1
    idx_add(in2.dx, out.dx, in2.dx); // derivative wrt in2
  }

  template <class T>
  void diff_module<T>::bbprop(state_idx<T> &in1, state_idx<T> &in2, state_idx<T> &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.ddx, in2.ddx, out.ddx);// must have same dimensions

    idx_add(in1.ddx, out.ddx, in1.ddx); // derivative wrt in1
    idx_add(in2.ddx, out.ddx, in2.ddx); // derivative wrt in2
  }

  ////////////////////////////////////////////////////////////////
  // mul_module

  template <class T>
  mul_module<T>::mul_module() {
  }
  
  template <class T>
  mul_module<T>::~mul_module() {
  }

  //! {<code>
  //!    y = x1 .* x2
  //! </code>}
  template <class T>
  void mul_module<T>::fprop(state_idx<T> &in1, state_idx<T> &in2,
			    state_idx<T> &out) {
    if (&in1 != &out) { // resize only when input and output are different
      idxdim d(in1.x); // use same dimensions as in
      out.resize(d);
    }
    idx_mul(in1.x, in2.x, out.x);
  }
  
  //! {<code>
  //!    dy\dx1 = x2
  //!    dy\dx2 = x1
  //! </code>}
  template <class T>
  void mul_module<T>::bprop(state_idx<T> &in1, state_idx<T> &in2,
			    state_idx<T> &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.dx, in2.dx, out.dx);// must have same dimensions

    if (!tmp.same_dim(in1.x.get_idxdim())) { // resize temp buffer
      idxdim d(in1.x);
      tmp = idx<T>(d);
    }
    idx_mul(out.dx, in2.x, tmp);
    idx_add(in1.dx, tmp, in1.dx);
    idx_mul(out.dx, in1.x, tmp);
    idx_add(in2.dx, tmp, in2.dx);
  }
   
  //! {<code>
  //!    d^2y\dx1^2 = (x2).^2
  //!    d^2y\dx2^2 = (x1).^2
  //! </code>}
  template <class T>
  void mul_module<T>::bbprop(state_idx<T> &in1, state_idx<T> &in2,
			     state_idx<T> &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.ddx, in2.ddx, out.ddx);// must have same dimensions

    if (!tmp.same_dim(in1.x.get_idxdim())) { // resize temp buffer
      idxdim d(in1.x);
      tmp = idx<T>(d);
    }
    idx_mul(in2.x, in2.x, tmp);
    idx_mul(out.ddx, tmp, tmp);
    idx_add(in1.ddx, tmp, in1.ddx);
    idx_mul(in1.x, in1.x, tmp);
    idx_mul(out.ddx, tmp, tmp);
    idx_add(in2.ddx, tmp, in2.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // thres_module

  template <class T>
  thres_module<T>::thres_module(T thres_, T val_)
    : thres(thres_), val(val_) {
  }

  template <class T>
  thres_module<T>::~thres_module() {
  }

  template <class T>
  void thres_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      out.resize(d);
    }
    idx_aloop2(inx, in.x, T, outx, out.x, T) {
      if (*inx > thres)
	*outx = *inx;
      else
	*outx = val;
    }
  }

  template <class T>
  void thres_module<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions

    idx_aloop3(inx, in.x, T, indx, in.dx, T, outdx, out.dx, T) {
      if (*inx > thres)
	*indx += *outdx;
    }
  }

  template <class T>
  void thres_module<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions

    idx_add(in.ddx, out.ddx, in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // cutborder_module

  template <class T>
  cutborder_module<T>::cutborder_module(int nr_, int nc_)
    : nrow(nr_), ncol(nc_) {
  }

  template <class T>
  cutborder_module<T>::~cutborder_module() {
  }

  template <class T>
  void cutborder_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    intg outr = inr - 2 * nrow;
    intg outc = inc - 2 * ncol;
    idxdim d(in.x.dim(0), outr, outc);
    if (!out.x.same_dim(d)) // resize only when necessary
      out.resize(d);
    out.clear();
    idx<T> tmp = in.x.narrow(1, outr, nrow);
    tmp = tmp.narrow(2, outc, ncol);
    idx_copy(tmp, out.x);
  }

  template <class T>
  void cutborder_module<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out
    
    intg inr = out.x.dim(1);
    intg inc = out.x.dim(2);
    idx<T> tmp = in.dx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(out.dx, tmp, tmp);
  }

  template <class T>
  void cutborder_module<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = out.x.dim(1);
    intg inc = out.x.dim(2);
    idx<T> tmp = in.ddx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(out.ddx, tmp, tmp);
  }

  ////////////////////////////////////////////////////////////////
  // zpad_module

  template <class T>
  zpad_module<T>::zpad_module(int nr, int nc)
    : nrow(nr), ncol(nc) {
  }

  template <class T>
  zpad_module<T>::~zpad_module() {
  }

  template <class T>
  void zpad_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idxdim d(in.x.dim(0), inr + 2 * nrow, inc + 2 * ncol);
    if (!out.x.same_dim(d)) // resize only when necessary
      out.resize(d);
    out.clear();
    idx<T> tmp = out.x.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_copy(in.x, tmp);
  }

  template <class T>
  void zpad_module<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idx<T> tmp = out.dx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(tmp, in.dx, in.dx);
  }

  template <class T>
  void zpad_module<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idx<T> tmp = out.ddx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(tmp, in.ddx, in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // fsum_module

  template <class T>
  fsum_module<T>::fsum_module() {
  }

  template <class T>
  fsum_module<T>::~fsum_module() {
  }
  
  template <class T>
  void fsum_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) { 
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      out.resize(d);
    }
    idx_eloop2(inx2, in.x, T, outx2, out.x, T) {
      idx_eloop2(inx1, inx2, T, outx1, outx2, T) {
	idx_fill(outx1, idx_sum(inx1));
      }
    }
  }

  template <class T>
  void fsum_module<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions
    
    idx_eloop2(indx2, in.dx, T, outdx2, out.dx, T) {
      idx_eloop2(indx1, indx2, T, outdx1, outdx2, T) {
	idx_addc(indx1, idx_sum(outdx1), indx1);
      }
    }
  }

  template <class T>
  void fsum_module<
    T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions
    
    idx_eloop2(inddx2, in.ddx, T, outddx2, out.ddx, T) {
      idx_eloop2(inddx1, inddx2, T, outddx1, outddx2, T) {
	idx_addc(inddx1, idx_sum(outddx1), inddx1);
      }
    }
  }
    
  ////////////////////////////////////////////////////////////////

#ifdef USE_IPP

  // TODO: enable IPP for float convolutions
  // TODO: ipp 64 for doubles?

  //   void c_layer_ipp<T>::fprop (state_idx<T> *in, state_idx<T> *out) {
  //     intg ki = kernel->x.dim(1);
  //     intg kj = kernel->x.dim(2);
  //     intg sini = in->x.dim(1);
  //     intg sinj = in->x.dim(2);

  //     if (((sini - (ki - stridei) % stridei) != 0) ||
  // 	((sinj - (kj - stridej) % stridej) != 0))
  //       eblerror("inconsistent input size, kernel size, and subsampling ratio.");
  //     if ((stridei != 1) || (stridej != 1))
  //       eblerror("stride > 1 not implemented yet.");
  //     idx<double> uuin = in->x.unfold(1, ki, stridei);
  //     uuin = uuin.spec.unfold_inplace(2, kj, stridej);
  //     idx<double> lki = idx<double>(kernel->x.dim(1), kernel->x.dim(2));
  //     // resize output if necessary
  //     sum->resize(thickness, uuin.dim(1), uuin.dim(2));
  //     out->resize(thickness, uuin.dim(1), uuin.dim(2));
  //     idx_clear(sum->x);
  //     // generic convolution
  //     idx<double> tout = idx<double>(sum->x.dim(1), sum->x.dim(2));
  //     { idx_bloop2(lk, kernel->x, double, lt, *table, intg) {
  // 	rev_idx2_tr(*lk, lki);
  // 	//      ipp_convolution_float(in->x.select(0, lt.get(0)), lki, tout);
  // 	//      ipp_add_float(tout, sum->x.select(0, lt.get(1)));
  //       }
  //     }
  //     // add bias
  //     { idx_bloop3(sumx, sum->x, double, biasx, bias->x, double,
  // 		 outx, out->x, double) {
  // 	//      ipp_addc_nip_float(sumx, biasx.get(), outx);
  //       }
  //     }
  //     // call squashing function
  //     squash->fprop(*sum, *out);
  //   }

  //   void c_layer_ipp<T>::bprop (state_idx<T> *in, state_idx<T> *out) {
  //     // backprop gradient through squasher
  //     squash->bprop(*sum, *out);
  //     // compute gradient of bias
  //     { idx_bloop2(lha, sum->dx, double, lb, bias->dx, double) {
  // 	idx_sumacc(lha, lb);
  //       }
  //     }
  //     // backprop through convolution
  //     idx_clear(in->dx);
  //     /*
  //       (let* ((ki (idx-dim :kernel:dx 1))	 (kj (idx-dim :kernel:dx 2))
  //       (ini (idx-dim :in:dx 1))	 (inj (idx-dim :in:dx 2))
  //       (outi (idx-dim :out:dx 1))	 (outj (idx-dim :out:dx 2))
  //       (sumi (idx-dim :sum:dx 1))	 (sumj (idx-dim :sum:dx 2))
  //       (souti (gbtype-matrix sumi sumj))
  //       (tout (gbtype-matrix ki kj)))
  //       (idx-bloop ((lk :kernel:dx) (lkf :kernel:x) (lt table))
  //       (let* ((islice (lt 0))
  //       (sout  (select :sum:dx 0 (lt 1)))
  //       )
  //       ;; backward convolution
  //       (ipp-convolution-full-float sout lkf (select :in:dx 0 islice))
  //       ;; compute gradient for kernel
  //       (rev-idx2-tr-float sout souti)
  //       (ipp-convolution-float (select :in:x 0 islice) souti tout)
  //       (ipp-add-float tout lk)
  //       )))
  //     */
  //   }

#endif

} // end namespace ebl
