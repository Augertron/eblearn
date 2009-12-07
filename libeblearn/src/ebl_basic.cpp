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

#include "ebl_basic.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // linear_module

  linear_module::linear_module(parameter &p, intg in, intg out)
    : w(p, out, in) {
  }

  linear_module::~linear_module() {
  }

  void linear_module::fprop(state_idx &in, state_idx &out) {
    if (bResize) resize_output(in, out); // resize (iff necessary)
    idx<double> inx = in.x.view_as_order(1); // view as 1D idx
    idx<double> outx = out.x.view_as_order(1); // view as 1D idx

    idx_m2dotm1(w.x, inx, outx); // linear combination
  }

  void linear_module::bprop(state_idx &in, state_idx &out) {
    idx<double> inx = in.x.view_as_order(1); // view as 1D idx
    idx<double> indx = in.dx.view_as_order(1); // view as 1D idx
    idx<double> outdx = out.dx.view_as_order(1); // view as 1D idx
    idx<double> twx(w.x.transpose(0, 1)); // switch dimensions 0 and 1
    if (outdx.nelements() != w.dx.dim(0)) eblerror("output has wrong size");

    idx_m1extm1acc(outdx, inx, w.dx); // backprop to weights
    idx_m2dotm1acc(twx, outdx, indx); // backprop to input
  }

  void linear_module::bbprop(state_idx &in, state_idx &out) {
    idx<double> inx = in.x.view_as_order(1); // view as 1D idx
    idx<double> inddx = in.ddx.view_as_order(1); // view as 1D idx
    idx<double> outddx = out.ddx.view_as_order(1); // view as 1D idx
    idx<double> twx = w.x.transpose(0, 1); // switch dimensions 0 and 1
    if (outddx.nelements() != w.ddx.dim(0)) eblerror("output has wrong size");

    idx_m1squextm1acc(outddx, inx, w.ddx); // backprop to weights
    idx_m2squdotm1acc(twx, outddx, inddx); // backprop to input
  }

  void linear_module::forget(forget_param_linear &fp) {
    double fanin = w.x.dim(1);
    double z = fp.value / pow(fanin, fp.exponent);
    check_drand_ini(); // check that the random seed was initialized
    idx_aloop1(lx,w.x,double)
      {	*lx = drand(-z, z);}
  }

  void linear_module::normalize() {
    norm_columns(w.x);
  }

  void linear_module::resize_output(state_idx &in, state_idx &out) {
    // resize output based on input dimensions
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, w.x.dim(0)); // except for the first one
    out.resize(d);
  }

  ////////////////////////////////////////////////////////////////
  // convolution_module_2D

  convolution_module_2D::convolution_module_2D(parameter &p, 
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

  convolution_module_2D::~convolution_module_2D() {
  }

  void convolution_module_2D::fprop(state_idx &in, state_idx &out){
    if (bResize) resize_output(in, out); // resize (iff necessary)
    // unfolding input for a faster convolution operation
    idx<double> uuin(in.x.unfold(1, kernel.x.dim(1), stridei));
    uuin = uuin.unfold(2, kernel.x.dim(2), stridej);
    idx_clear(out.x);
    // convolve 2D slice for each convolution kernel
    { idx_bloop2(lk, kernel.x, double, lt, table, intg) {
      idx<double> suin(uuin.select(0, lt.get(0)));
      idx<double> sout((out.x).select(0, lt.get(1)));

      idx_m4dotm2acc(suin, lk, sout); // 2D convolution
      }}
  }
  
  void convolution_module_2D::bprop(state_idx &in, state_idx &out) {
    // backprop through convolution
    idx_clear(in.dx);
    idx<double> uuin(in.dx.unfold(1, (kernel.dx).dim(1), stridei));
    uuin = uuin.unfold(2, (kernel.dx).dim(2), stridej);
    idx<double> uuinf(in.x.unfold(1, (kernel.dx).dim(1), stridei));
    uuinf = uuinf.unfold(2, (kernel.dx).dim(2), stridej);
    int transp[5] = { 0, 3, 4, 1, 2 };
    idx<double> borp(uuinf.transpose(transp));
    { idx_bloop3 (lk, kernel.dx, double, lkf, kernel.x, double, 
		  lt, table, intg) {
	intg islice = lt.get(0);
	idx<double> suin(uuin.select(0, islice));
	idx<double> sborp(borp.select(0, islice));
	idx<double> sout(out.dx.select(0, lt.get(1)));

	idx_m2extm2acc(sout, lkf, suin); // backward convolution
	idx_m4dotm2acc(sborp, sout, lk); // compute gradient for kernel
      }}
  }

  void convolution_module_2D::bbprop(state_idx &in, state_idx &out) {
    // backprop through convolution
    idx_clear(in.ddx);
    idx<double> uuin(in.ddx.unfold(1, (kernel.ddx).dim(1), stridei));
    uuin = uuin.unfold(2, (kernel.ddx).dim(2), stridej);
    idx<double> uuinf(in.x.unfold(1, (kernel.ddx).dim(1), stridei));
    uuinf = uuinf.unfold(2, (kernel.ddx).dim(2), stridej);
    int transp[5] = { 0, 3, 4, 1, 2 };
    idx<double> borp(uuinf.transpose(transp));
    { idx_bloop3 (lk, kernel.ddx, double, lkf, kernel.x, double, 
		  lt, table, intg) {
	intg islice = lt.get(0);
	idx<double> suin(uuin.select(0, islice));
	idx<double> sborp(borp.select(0, islice));
	idx<double> sout((out.ddx).select(0, lt.get(1)));
	    
	idx_m2squextm2acc(sout, lkf, suin); // backward convolution
	idx_m4squdotm2acc(sborp, sout, lk); // compute gradient for kernel
      }}
  }

  void convolution_module_2D::forget(forget_param_linear &fp) {
    idx<double> kx(kernel.x);
    intg vsize = kx.dim(1);
    intg hsize = kx.dim(2);
    idx<intg> ts(table.select(1, 1));
    idx<int> fanin(1 + idx_max(ts));
    check_drand_ini();
    idx_clear(fanin);
    { idx_bloop1(tab, table, intg)	{
	fanin.set(1 + fanin.get(tab.get(1)), tab.get(1)); }}
    { idx_bloop2(tab, table, intg, x, kx, double) {
	double s = fp.value / pow((vsize * hsize * fanin.get(tab.get(1))), 
				  fp.exponent);
	{ idx_bloop1(lx, x, double) {
	    { idx_bloop1(llx, lx, double) {
		double n = drand(-s, s);
		llx.set(n);
	      }}
	  }}
      }}
  }

  void convolution_module_2D::resize_output(state_idx &in, state_idx &out) {
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
	((sinj - (kj - stridej)) % stridej != 0)) {
      cerr << "input: " << in.x << " kernel: " << kernel.x << " stride: ";
      cerr << stridei << "x" << stridej << endl;
      eblerror("inconsistent input size, kernel size, and subsampling ratio.");
    }
    if ((stridei != 1) || (stridej != 1))
      eblerror("stride > 1 not implemented yet.");
    // unfolding input for a faster convolution operation
    idx<double> uuin(in.x.unfold(1, ki, stridei));
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

  subsampling_module_2D::subsampling_module_2D(parameter &p, 
					       intg stridei_, intg stridej_,
					       intg subi, intg subj, 
					       intg thick)
    : coeff(p, thick), sub(thick, subi, subj), thickness(thick), 
      stridei(stridei_), stridej(stridej_) {
  }

  subsampling_module_2D::~subsampling_module_2D() {
  }

  void subsampling_module_2D::fprop(state_idx &in, state_idx &out) {
    if (bResize) resize_output(in, out); // resize (iff necessary)
    // subsampling ( coeff * average )
    idx_clear(sub.x);
    { idx_bloop4(lix, in.x, double, lsx, sub.x, double,
		 lcx, coeff.x, double, ltx, out.x, double) {
	idx<double> uuin(lix.unfold(1, stridej, stridej));
	uuin = uuin.unfold(0, stridei, stridei);
	idx_eloop1(z1, uuin, double) {
	  idx_eloop1(z2, z1, double) {
	    idx_add(z2, lsx, lsx); // average
	  }
	}
	idx_dotc(lsx, lcx.get(), ltx); // coeff
      }}
  }

  void subsampling_module_2D::bprop(state_idx &in, state_idx &out) {
    // oversampling
    idx_bloop3(lcdx, coeff.dx, double, ltdx, out.dx, double,
	       lsx, sub.x, double) {
      idx_dotacc(lsx, ltdx, lcdx);
    }
    idx_bloop4(lidx, in.dx, double, lsdx, sub.dx, double,
	       lcx, coeff.x, double, ltdx2, out.dx, double) {
      idx_dotc(ltdx2, lcx.get(), lsdx);
      idx_m2oversample(lsdx, stridei, stridej, lidx);
    }
  }

  void subsampling_module_2D::bbprop(state_idx &in, state_idx &out) {	
    // oversampling
    idx_bloop3(lcdx, coeff.ddx, double, ltdx, out.ddx, double,
	       lsx, sub.x, double) {
      idx_m2squdotm2acc(lsx, ltdx, lcdx);
    }
    idx_bloop4(lidx, in.ddx, double, lsdx, sub.ddx, double,
	       lcx, coeff.x, double, ltdx2, out.ddx, double) {
      double cf = lcx.get();
      idx_dotc(ltdx2, cf * cf, lsdx);
      idx_m2oversample(lsdx, stridei, stridej, lidx);
    }
  }

  void subsampling_module_2D::forget(forget_param_linear &fp) {
    double c = fp.value / pow(stridei * stridej, fp.exponent);
    idx_fill(coeff.x, c);
  }

  void subsampling_module_2D::resize_output(state_idx &in, state_idx &out) {
    intg sin_i = in.x.dim(1);
    intg sin_j = in.x.dim(2);
    intg si = sin_i / stridei;
    intg sj = sin_j / stridej;
    // check sizes
    if ((sin_i % stridei) != 0 || (sin_j % stridej) != 0) {
      cerr << "input: " << in.x << " stride: ";
      cerr << stridei << "x" << stridej << endl;
      eblerror("inconsistent input size and subsampling ratio");
    }
    // resize output and sub based in input dimensions
    idxdim d(in.x); // use same dimensions as in
    d.setdim(1, si); // new size after subsampling
    d.setdim(2, sj); // new size after subsampling
    out.resize(d);
    sub.resize(d);
  }

  ////////////////////////////////////////////////////////////////
  // addc_module

  addc_module::addc_module(parameter &p, intg size)
    : bias(p, size) {
  }

  addc_module::~addc_module() {
  }

  void addc_module::fprop(state_idx& in, state_idx& out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      d.setdim(0, bias.x.dim(0)); // except for the first one
      out.resize(d);
    }
    // add each bias to entire slices cut from the first dimension
    idx_bloop3(inx, in.x, double, biasx, bias.x, double, outx, out.x, double)
      {
	idx_addc(inx, biasx.get(), outx);
      }
  }

  void addc_module::bprop(state_idx& in, state_idx& out) {
    if ((&in != &out) && (in.dx.nelements() != out.dx.nelements()))
      eblerror("output has wrong size");
    
    idx_bloop3(indx, in.dx, double, biasdx, bias.dx, double, 
	       outdx, out.dx, double) {
      if (&in != &out)
	idx_copy(outdx, indx); // only pass on info if necessary
      idx_sumacc(outdx, biasdx);
    }
  }

  void addc_module::bbprop(state_idx& in, state_idx& out) {
    if ((&in != &out) && (in.ddx.nelements() != out.ddx.nelements()))
      eblerror("output has wrong size");
    
    idx_bloop3(inddx, in.ddx, double, biasddx, bias.ddx, double, 
	       outddx, out.ddx, double) {
      if (&in != &out)
	idx_copy(outddx, inddx); // only pass on info if necessary
      idx_sumacc(outddx, biasddx);
    }
  }
  
  void addc_module::forget(forget_param_linear& fp) {
    idx_clear(bias.x);
  }

  ////////////////////////////////////////////////////////////////
  // power_module

  power_module::power_module(double p_) : p(p_) {
  }

  power_module::~power_module() {
  }
    
  void power_module::fprop(state_idx &in, state_idx &out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      out.resize(d);
    }
    idx_power(in.x, p, out.x);
  }
  
  void power_module::bprop(state_idx &in, state_idx &out) {
    if ((&in != &out) && (in.dx.nelements() != out.dx.nelements()))
      eblerror("output has wrong size");
    if (!tt.same_dim(in.x.get_idxdim())) { // resize temp buffer
      idxdim d(in.x);
      tt = idx<double>(d);
    }
    // compute derivative
    idx_div(out.x, in.x, tt); //! tt = x^(p-1)
    idx_mul(out.dx, tt, tt); //! tt = outdx*x^(p-1)
    idx_dotcacc(tt, p, in.dx);
  }

  void power_module::bbprop(state_idx &in, state_idx &out) {
    if ((&in != &out) && (in.dx.nelements() != out.dx.nelements()))
      eblerror("output has wrong size");
    if (!tt.same_dim(in.x.get_idxdim())) { // resize temp buffer
      idxdim d(in.x);
      tt = idx<double>(d);
    }
    // compute 2nd derivative
    idx_div(out.x, in.x, tt); //! tt = x^(p-1)
    idx_mul(tt, tt, tt); //! tt = (x^(p-1))^2
    idx_mul(out.ddx, tt, tt); //! tt = outddx*(x^(p-1))^2
    idx_dotcacc(tt, p * p, in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // diff_module

  diff_module::diff_module() {
  }
  
  diff_module::~diff_module() {
  }

  void diff_module::fprop(state_idx &in1, state_idx &in2, state_idx &out) {
    if (&in1 != &out) { // resize only when input and output are different
      idxdim d(in1.x); // use same dimensions as in
      out.resize(d);
    }
    idx_sub(in1.x, in2.x, out.x);
  }

  void diff_module::bprop(state_idx &in1, state_idx &in2, state_idx &out) {
    idx_add(in1.dx, out.dx, in1.dx); // derivative wrt in1
    idx_add(in2.dx, out.dx, in2.dx); // derivative wrt in2
  }

  void diff_module::bbprop(state_idx &in1, state_idx &in2, state_idx &out) {
    idx_add(in1.ddx, out.ddx, in1.ddx); // derivative wrt in1
    idx_add(in2.ddx, out.ddx, in2.ddx); // derivative wrt in2
  }

  ////////////////////////////////////////////////////////////////
  // mul_module

  mul_module::mul_module() {
  }
  
  mul_module::~mul_module() {
  }

  //! {<code>
  //!    y = x1 .* x2
  //! </code>}
  void mul_module::fprop(state_idx &in1, state_idx &in2, state_idx &out) {
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
  void mul_module::bprop(state_idx &in1, state_idx &in2, state_idx &out) {
    if (!tmp.same_dim(in1.x.get_idxdim())) { // resize temp buffer
      idxdim d(in1.x);
      tmp = idx<double>(d);
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
  void mul_module::bbprop(state_idx &in1, state_idx &in2, state_idx &out) {
    if (!tmp.same_dim(in1.x.get_idxdim())) { // resize temp buffer
      idxdim d(in1.x);
      tmp = idx<double>(d);
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

  thres_module::thres_module(double thres_, double val_)
    : thres(thres_), val(val_) {
  }

  thres_module::~thres_module() {
  }

  void thres_module::fprop(state_idx &in, state_idx &out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      out.resize(d);
    }
    idx_aloop2(inx, in.x, double, outx, out.x, double) {
      if (*inx > thres)
	*outx = *inx;
      else
	*outx = val;
    }
  }

  void thres_module::bprop(state_idx &in, state_idx &out) {
    idx_aloop3(inx, in.x, double, indx, in.dx, double, outdx, out.dx, double) {
      if (*inx > thres)
	*indx += *outdx;
    }
  }

  void thres_module::bbprop(state_idx &in, state_idx &out) {
    idx_add(in.ddx, out.ddx, in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // cutborder_module

  cutborder_module::cutborder_module(int nr_, int nc_)
    : nrow(nr_), ncol(nc_) {
  }

  cutborder_module::~cutborder_module() {
  }

  void cutborder_module::fprop(state_idx &in, state_idx &out) {
    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    intg outr = inr - 2 * nrow;
    intg outc = inc - 2 * ncol;
    idxdim d(in.x.dim(0), outr, outc);
    if (!out.x.same_dim(d)) // resize only when necessary
      out.resize(d);
    out.clear();
    idx<double> tmp = in.x.narrow(1, outr, nrow);
    tmp = tmp.narrow(2, outc, ncol);
    idx_copy(tmp, out.x);
  }

  void cutborder_module::bprop(state_idx &in, state_idx &out) {
    intg inr = out.x.dim(1);
    intg inc = out.x.dim(2);
    idx<double> tmp = in.dx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(out.dx, tmp, tmp);
  }

  void cutborder_module::bbprop(state_idx &in, state_idx &out) {
    intg inr = out.x.dim(1);
    intg inc = out.x.dim(2);
    idx<double> tmp = in.ddx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(out.ddx, tmp, tmp);
  }

  ////////////////////////////////////////////////////////////////
  // zpad_module

  zpad_module::zpad_module(int nr, int nc)
    : nrow(nr), ncol(nc) {
  }

  zpad_module::~zpad_module() {
  }

  void zpad_module::fprop(state_idx &in, state_idx &out) {
    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idxdim d(in.x.dim(0), inr + 2 * nrow, inc + 2 * ncol);
    if (!out.x.same_dim(d)) // resize only when necessary
      out.resize(d);
    out.clear();
    idx<double> tmp = out.x.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_copy(tmp, in.x);
  }

  void zpad_module::bprop(state_idx &in, state_idx &out) {
    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idx<double> tmp = out.dx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(tmp, in.dx, in.dx);
  }

  void zpad_module::bbprop(state_idx &in, state_idx &out) {
    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idx<double> tmp = out.ddx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(tmp, in.ddx, in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // fsum_module

  fsum_module::fsum_module() {
  }

  fsum_module::~fsum_module() {
  }
  
  void fsum_module::fprop(state_idx &in, state_idx &out) { 
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      out.resize(d);
    }
    idx_eloop2(inx2, in.x, double, outx2, out.x, double) {
      idx_eloop2(inx1, inx2, double, outx1, outx2, double) {
	idx_fill(outx1, idx_sum(inx1));
      }
    }
  }

  void fsum_module::bprop(state_idx &in, state_idx &out) {
    idx_eloop2(indx2, in.dx, double, outdx2, out.dx, double) {
      idx_eloop2(indx1, indx2, double, outdx1, outdx2, double) {
	idx_addc(indx1, idx_sum(outdx1), indx1);
      }
    }
  }

  void fsum_module::bbprop(state_idx &in, state_idx &out) {
    idx_eloop2(inddx2, in.ddx, double, outddx2, out.ddx, double) {
      idx_eloop2(inddx1, inddx2, double, outddx1, outddx2, double) {
	idx_addc(inddx1, idx_sum(outddx1), inddx1);
      }
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // helper function
  
  idx<intg> full_table(intg a, intg b) {
    idx<intg> m(a * b, 2);
    intg p = 0;
    for (intg j = 0; j < b; ++j) {
      for (intg i = 0; i < a; ++i) {
	m.set(i, p, 0);
	m.set(j, p, 1);
	p++;
      }
    }
    return m;
  }

  idx<intg> one2one_table(intg n) {
    idx<intg> m(n, 2);
    for (intg i = 0; i < n; ++i) {
      m.set(i, i, 0);
      m.set(i, i, 1);
    }
    return m;
  }
  
  ////////////////////////////////////////////////////////////////

#ifdef USE_IPP

  // TODO: enable IPP for float convolutions
  // TODO: ipp 64 for doubles?

//   void c_layer_ipp::fprop (state_idx *in, state_idx *out) {
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

//   void c_layer_ipp::bprop (state_idx *in, state_idx *out) {
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
