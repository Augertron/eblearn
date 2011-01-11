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

  template <typename T, class Tstate>
  linear_module<T, Tstate>::linear_module(parameter<T,Tstate> *p,
					  intg in, intg out,
					  const char *name_)
    : module_1_1<T,Tstate>(name_), w(p, out, in) {
  }

  template <typename T, class Tstate>
  linear_module<T, Tstate>::~linear_module() {
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    idx<T> inx = in.x.view_as_order(1); // view as 1D idx
    idx<T> outx = out.x.view_as_order(1); // view as 1D idx

    idx_m2dotm1(w.x, inx, outx); // linear combination

#ifdef __DUMP_STATES__ // used to debug
    DUMP(in.x, this->name() << "_linear_module_in.x");
    DUMP(w.x, this->name() << "_linear_module_weights");
#endif
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    idx<T> inx = in.x.view_as_order(1); // view as 1D idx
    idx<T> indx = in.dx.view_as_order(1); // view as 1D idx
    idx<T> outdx = out.dx.view_as_order(1); // view as 1D idx
    idx<T> twx(w.x.transpose(0, 1)); // switch dimensions 0 and 1
    if (outdx.nelements() != w.dx.dim(0))
      eblerror("output has wrong size");

    idx_m1extm1acc(outdx, inx, w.dx); // backprop to weights
    idx_m2dotm1acc(twx, outdx, indx); // backprop to input
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::bbprop(Tstate &in,
					Tstate &out) {
    idx<T> inx = in.x.view_as_order(1); // view as 1D idx
    idx<T> inddx = in.ddx.view_as_order(1); // view as 1D idx
    idx<T> outddx = out.ddx.view_as_order(1); // view as 1D idx
    idx<T> twx = w.x.transpose(0, 1); // switch dimensions 0 and 1
    if (outddx.nelements() != w.ddx.dim(0))
      eblerror("output has wrong size");
    // backprop
    idx_m1squextm1acc(outddx, inx, w.ddx); // backprop to weights
    idx_m2squdotm1acc(twx, outddx, inddx); // backprop to input
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::forget(forget_param_linear &fp) {
    double fanin = w.x.dim(1);
    double z = fp.value / pow(fanin, fp.exponent);
    check_drand_ini(); // check that the random seed was initialized
    idx_aloop1(lx, w.x, T)
      {	*lx = (T) drand(-z, z);}
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::normalize() {
#ifdef __CBLAS__
    norm_columns(w.x);
#else
    eblerror("norm_columns not implemented without cblas");
#endif
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::resize_output(Tstate &in, Tstate &out) {
    // resize output based on input dimensions
    idxdim d(in.x); // use same dimensions as in
    d.setdim(0, w.x.dim(0)); // except for the first one
    if (out.x.get_idxdim() != d) { // resize only if necessary
#ifdef __DEBUG__
      cout << "linear: resizing output from " << out.x.get_idxdim();
      cout << " to " << d << endl;
#endif
      out.resize(d);
    }
  }

  template <typename T, class Tstate>
  idxdim linear_module<T, Tstate>::fprop_size(idxdim &isize) {
    //! Update output size based on weight dimensions
    idxdim osize(w.x.dim(0), isize.dim(1), isize.dim(2));
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  idxdim linear_module<T, Tstate>::bprop_size(const idxdim &osize) {
    idxdim isize(w.x.dim(1), osize.dim(1), osize.dim(2));
    return isize;
  }

  template <typename T, class Tstate>
  linear_module<T, Tstate>* linear_module<T, Tstate>::copy() {
    // new module (with its own local parameter buffers)
    linear_module<T, Tstate> *l2 =
      new linear_module<T, Tstate>(NULL, w.x.dim(1), w.x.dim(0));
    // copy data
    idx_copy(w.x, l2->w.x);
    return l2;
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::load_x(idx<T> &weights) {
    if (!w.x.same_dim(weights))
      eblthrow("expected same dimension weights but got " << w.x << " and "
	       << weights << " instead");
    idx_copy(weights, w.x);
  }

  template <typename T, class Tstate>
  std::string linear_module<T, Tstate>::describe() {
    std::string desc;
    desc << "linear module " << this->name() << " "
	 << w.x.dim(1) << " -> " << w.x.dim(0);
    return desc;
  }
  
  ////////////////////////////////////////////////////////////////
  // convolution_module

  template <typename T, class Tstate>
  convolution_module<T, Tstate>::convolution_module(parameter<T,Tstate> *p, 
					    intg kerneli, intg kernelj, 
					    intg stridei_, intg stridej_, 
					    idx<intg> &tbl, const char *name_)
    : module_1_1<T,Tstate>(name_), kernel(p, tbl.dim(0), kerneli, kernelj), 
      stridei(stridei_), stridej(stridej_), table(tbl), warnings_shown(false),
      float_precision(false) {
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
#ifdef __IPP__
    // check precision to decide if we use IPP or not
    fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&kernel);
    if (cont) {
      float_precision = true;
      revkernel = idx<T>(kerneli, kernelj); // allocate reversed kernel
      outtmp = idx<T>(1, 1);
    }
#endif
  }

  template <typename T, class Tstate>
  convolution_module<T, Tstate>::~convolution_module() {
  }

  template <typename T, class Tstate>
  void convolution_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    // unfolding input for a faster convolution operation
    idx<T> uuin(in.x.unfold(1, kernel.x.dim(1), stridei));
    uuin = uuin.unfold(2, kernel.x.dim(2), stridej);
    idx_clear(out.x);
    // convolve 2D slice for each convolution kernel
    { idx_bloop2(lk, kernel.x, T, lt, table, intg) {
	idx<T> sout((out.x).select(0, lt.get(1)));
#ifdef __IPP__
	if (float_precision) {
	  rev_idx2_tr(lk, revkernel);
	  //	  idx_clear(outtmp);
	  idx<T> suin(in.x.select(0, lt.get(0)));
	  ipp_convolution(suin, revkernel, outtmp);
	  ipp_add(outtmp, sout);
	}
	else {// not using IPP
	  idx<T> suin(uuin.select(0, lt.get(0)));
	  idx_m4dotm2acc(suin, lk, sout); // 2D convolution
	}
#else
	  idx<T> suin(uuin.select(0, lt.get(0)));
	  idx_m4dotm2acc(suin, lk, sout); // 2D convolution
#endif
      }
    }
#ifdef __DUMP_STATES__ // used to debug
    DUMP(in.x, this->name() << "_convolution_module_in.x");
    DUMP(kernel.x, this->name() << "_convolution_module_ker.x");
    DUMP(table, this->name() << "_convolution_module_table");
    DUMP(out.x, this->name() << "_convolution_module_out.x");
#endif
  }
  
  template <typename T, class Tstate>
  void convolution_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    // backprop through convolution
    idx_clear(in.dx);
    idx<T> uuin(in.dx.unfold(1, kernel.dx.dim(1), stridei));
    uuin = uuin.unfold(2, kernel.dx.dim(2), stridej);
    idx<T> uuinf(in.x.unfold(1, kernel.dx.dim(1), stridei));
    uuinf = uuinf.unfold(2, kernel.dx.dim(2), stridej);
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

  template <typename T, class Tstate>
  void convolution_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    // backprop through convolution
    idx_clear(in.ddx);
    idx<T> uuin(in.ddx.unfold(1, kernel.ddx.dim(1), stridei));
    uuin = uuin.unfold(2, kernel.ddx.dim(2), stridej);
    idx<T> uuinf(in.x.unfold(1, kernel.ddx.dim(1), stridei));
    uuinf = uuinf.unfold(2, kernel.ddx.dim(2), stridej);
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

  template <typename T, class Tstate>
  void convolution_module<T,Tstate>::forget(forget_param_linear &fp) {
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

  template <typename T, class Tstate>
  void convolution_module<T,Tstate>::resize_output(Tstate &in, Tstate &out) {
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
      cerr << "warning: convolution connection table is not using all inputs "
	   << "in layer " << this->name() << " the maximum input index used by the "
	   << "table is " << tablemax << " but the input is " << in.x << endl;
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
    if (out.x.get_idxdim() != d) { // resize only if necessary
#ifdef __DEBUG__
      cout << "convolution: resizing output from " << out.x.get_idxdim();
      cout << " to " << d << endl;
#endif
#ifdef __IPP__
      if (float_precision) {
	outtmp.resize(d.dim(1), d.dim(2));
      }
#endif
      out.resize(d);
    }
  }

  template <typename T, class Tstate>
  idxdim convolution_module<T,Tstate>::fprop_size(idxdim &isize) {
    //! Select a kernel
    idxdim kernel_size = kernel.x[0].get_idxdim();
    //! Extract its dimensions, update output size
    idxdim osize(thickness,
		 std::max((intg) 1, isize.dim(1) - kernel_size.dim(0) + 1),
		 std::max((intg) 1, isize.dim(2) - kernel_size.dim(1) + 1));
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  idxdim convolution_module<T,Tstate>::bprop_size(const idxdim &osize) {
    //! Select a kernel
    idxdim kernel_size = kernel.x[0].get_idxdim();
    //! Extract its dimensions, update output size
    idxdim isize(tablemax + 1,
		 osize.dim(1) + kernel_size.dim(0) - 1,
		 osize.dim(2) + kernel_size.dim(1) - 1);
    return isize;
  }

  template <typename T, class Tstate>
  convolution_module<T,Tstate>* convolution_module<T,Tstate>::copy() {
    // new module (with its own local parameter buffers)
    convolution_module<T,Tstate> *l2 =
      new convolution_module<T,Tstate>(NULL, kernel.x.dim(1), kernel.x.dim(2),
				       stridei, stridej, table);
    // copy data
    idx_copy(kernel.x, l2->kernel.x);
    return l2;
  }

  template <typename T, class Tstate>
  void convolution_module<T, Tstate>::load_x(idx<T> &weights) {
    if (!kernel.x.same_dim(weights))
      eblthrow("expected same dimension weights but got " << kernel.x
	       << " and " << weights << " instead");
    idx_copy(weights, kernel.x);
  }

  template <typename T, class Tstate>
  std::string convolution_module<T, Tstate>::describe() {
    std::string desc;
    desc << "convolution module " << this->name() << " with kernel "
	 << kernel.x
	 << ", stride " << stridei << "x" << stridej << " and table " << table;
    return desc;
  }
  
  ////////////////////////////////////////////////////////////////
  // subsampling_module

  template <typename T, class Tstate>
  subsampling_module<T,Tstate>::
  subsampling_module(parameter<T,Tstate> *p, intg stridei_, intg stridej_,
		     intg subi, intg subj, intg thick, const char *name_)
    : module_1_1<T,Tstate>(name_), 
      coeff(p, thick), sub(thick, subi, subj), thickness(thick), 
      stridei(stridei_), stridej(stridej_) {
  }

  template <typename T, class Tstate>
  subsampling_module<T,Tstate>::~subsampling_module() {
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    // subsampling ( coeff * average )
    idx_clear(sub.x);
    { idx_bloop4(lix, in.x, T, lsx, sub.x, T, lcx, coeff.x, T, ltx, out.x, T) {
	idx<T> uuin(lix.unfold(1, stridej, stridej));
	uuin = uuin.unfold(0, stridei, stridei);
	// loop on each pixel of subs kernel, assuming that idx_add is faster 
	// than looping on each kernel
	idx_eloop1(z1, uuin, T) {
	  idx_eloop1(z2, z1, T) {
	    idx_add(z2, lsx); // average
	  }
	}
	idx_dotc(lsx, lcx.get(), ltx); // coeff
      }}

#ifdef __DUMP_STATES__ // used to debug
    DUMP(in.x, this->name() << "_subsampling_module_in.x");
    DUMP(coeff.x, this->name() << "_subsampling_module_coeff");
    DUMP(out.x, this->name() << "_subsampling_module_out.x");
#endif
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    // update internal coefficient's dx
    idx_bloop3(lcdx, coeff.dx, T, ltdx, out.dx, T, lsx, sub.x, T) {
      idx_dotacc(lsx, ltdx, lcdx);
    }
    // oversampling and accumulate to input's dx
    idx_bloop4(lidx, in.dx, T, lsdx, sub.dx, T,
	       lcx, coeff.x, T, ltdx2, out.dx, T) {
      idx_dotc(ltdx2, lcx.get(), lsdx);
      idx_m2oversampleacc(lsdx, stridei, stridej, lidx);
    }
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::bbprop(Tstate &in,
					    Tstate &out) {	
    // update internal coefficient's ddx
    idx_bloop3(lcdx, coeff.ddx, T, ltdx, out.ddx, T, lsx, sub.x, T) {
      idx_m2squdotm2acc(lsx, ltdx, lcdx);
    }
    // oversampling and accumulte to input's ddx
    idx_bloop4(lidx, in.ddx, T, lsdx, sub.ddx, T,
	       lcx, coeff.x, T, ltdx2, out.ddx, T) {
      T cf = lcx.get();
      idx_dotc(ltdx2, cf * cf, lsdx);
      idx_m2oversampleacc(lsdx, stridei, stridej, lidx);
    }
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::forget(forget_param_linear &fp) {
    double c = fp.value / pow(stridei * stridej, fp.exponent);
    idx_fill(coeff.x, (T) c);
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::resize_output(Tstate &in, Tstate &out) {
    intg sin_i = in.x.dim(1);
    intg sin_j = in.x.dim(2);
    intg si = sin_i / stridei;
    intg sj = sin_j / stridej;
    // check sizes
    if ((sin_i % stridei) != 0 || (sin_j % stridej) != 0) {
      cerr << "subsampling " << sin_i << "x" << sin_j << " with stride "
	   << stridei << "x" << stridej << endl;
      eblerror("inconsistent input size and subsampling ratio");
    }
    // resize output and sub based in input dimensions
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(1, si); // new size after subsampling
    d.setdim(2, sj); // new size after subsampling
    if (out.x.get_idxdim() != d) { // resize only if necessary
#ifdef __DEBUG__
      cout << "subsampling: resizing output from " << out.x.get_idxdim();
      cout << " to " << d << endl;
#endif
      out.resize(d);
      sub.resize(d);
    }
  }
 
  template <typename T, class Tstate>
  idxdim subsampling_module<T,Tstate>::fprop_size(idxdim &isize) {
    //! Update input size
    idxdim osize(thickness,
		 std::max((intg) 1, isize.dim(1) / stridei),
		 std::max((intg) 1, isize.dim(2) / stridej));
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  idxdim subsampling_module<T,Tstate>::bprop_size(const idxdim &osize) {
    //! Update input size
    idxdim isize(thickness,
		 osize.dim(1) * stridei,
		 osize.dim(2) * stridej);
    return isize;
  }

  template <typename T, class Tstate>
  subsampling_module<T,Tstate>* subsampling_module<T,Tstate>::copy() {
    // new module (with its own local parameter buffers)
    subsampling_module<T,Tstate> *l2 =
      new subsampling_module<T, Tstate>(NULL, stridei, stridej,
			     sub.x.dim(1), sub.x.dim(2), thickness);
    // copy data
    idx_copy(coeff.x, l2->coeff.x);
    idx_copy(sub.x, l2->sub.x);
    return l2;
  }
 
  template <typename T, class Tstate>
  std::string subsampling_module<T, Tstate>::describe() {
    std::string desc;
    desc << "subsampling module " << this->name() << " with kernel "
	 << sub.x
	 << " and stride " << stridei << "x" << stridej;
    return desc;
  }
  
  ////////////////////////////////////////////////////////////////
  // addc_module

  template <typename T, class Tstate>
  addc_module<T,Tstate>::addc_module(parameter<T,Tstate> *p, intg size,
				     const char *name_)
    : module_1_1<T,Tstate>(name_), bias(p, size) {
  }

  template <typename T, class Tstate>
  addc_module<T,Tstate>::~addc_module() {
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::fprop(Tstate& in, Tstate& out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x.spec); // use same dimensions as in
      d.setdim(0, bias.x.dim(0)); // except for the first one
      if (out.x.get_idxdim() != d) { // resize only if necessary
#ifdef __DEBUG__
	cout << "addc: resizing output from " << out.x.get_idxdim();
	cout << " to " << d << endl;
#endif
	out.resize(d);
      }
    }
    // add each bias to entire slices cut from the first dimension
    idx_bloop3(inx, in.x, T, biasx, bias.x, T, outx, out.x, T) {
      idx_addc(inx, biasx.get(), outx);
    }
      
#ifdef __DUMP_STATES__ // used to debug
    DUMP(in.x, this->name() << "_addc_module_in.x");
    DUMP(bias.x, this->name() << "_addc_module_weights");
    DUMP(out.x, this->name() << "_addc_module_out.x");
#endif
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::bprop(Tstate& in, Tstate& out) {
    if ((&in != &out) && (in.dx.nelements() != out.dx.nelements()))
      eblerror("output has wrong size");
    
    idx_bloop3(indx, in.dx, T, biasdx, bias.dx, T, 
	       outdx, out.dx, T) {
      if (&in != &out) // only pass on info if necessary
	idx_add(outdx, indx, indx); // accumulate gradients to input
      idx_sumacc(outdx, biasdx); // accumulate gradients to weights
    }
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::bbprop(Tstate& in, Tstate& out) {
    if ((&in != &out) && (in.ddx.nelements() != out.ddx.nelements()))
      eblerror("output has wrong size");
    
    idx_bloop3(inddx, in.ddx, T, biasddx, bias.ddx, T, 
	       outddx, out.ddx, T) {
      if (&in != &out) // only pass on info if necessary
	idx_add(outddx, inddx, inddx); // accumulate 2nd gradients to input
      idx_sumacc(outddx, biasddx); // accumulate 2nd gradients to weights
    }
  }
  
  template <typename T, class Tstate>
  void addc_module<T,Tstate>::forget(forget_param_linear& fp) {
    idx_clear(bias.x);
  }

  template <typename T, class Tstate>
  addc_module<T,Tstate>* addc_module<T,Tstate>::copy() {
    // new module (with its own local parameter buffers)
    addc_module<T,Tstate> *l2 = new addc_module<T, Tstate>(NULL, bias.x.dim(0));
    // copy data
    idx_copy(bias.x, l2->bias.x);
    return l2;
  }

  template <typename T, class Tstate>
  void addc_module<T, Tstate>::load_x(idx<T> &weights) {
    if (!bias.x.same_dim(weights))
      eblthrow("expected same dimension weights but got " << bias.x << " and "
	       << weights << " instead");
    idx_copy(weights, bias.x);
  }

  template <typename T, class Tstate>
  std::string addc_module<T, Tstate>::describe() {
    std::string desc;
    desc << "bias module " << this->name() << " with "
	 << bias.x.dim(0) << " biases.";
    return desc;
  }
  
  ////////////////////////////////////////////////////////////////
  // power_module

  template <typename T, class Tstate>
  power_module<T,Tstate>::power_module(T p_)
    : module_1_1<T,Tstate>("power"), p(p_) {
  }

  template <typename T, class Tstate>
  power_module<T,Tstate>::~power_module() {
  }
    
  template <typename T, class Tstate>
  void power_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      out.resize(d);
    }
    idx_power(in.x, p, out.x);
  }
  
  template <typename T, class Tstate>
  void power_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
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

  template <typename T, class Tstate>
  void power_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
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

  template <typename T, class Tstate>
  diff_module<T,Tstate>::diff_module() : module_2_1<T,Tstate>("diff") {
  }
  
  template <typename T, class Tstate>
  diff_module<T,Tstate>::~diff_module() {
  }

  template <typename T, class Tstate>
  void diff_module<T,Tstate>::fprop(Tstate &in1, Tstate &in2,
			     Tstate &out) {
    if (&in1 != &out) { // resize only when input and output are different
      idxdim d(in1.x); // use same dimensions as in
      out.resize(d);
    }
    idx_sub(in1.x, in2.x, out.x);
  }

  template <typename T, class Tstate>
  void diff_module<T,Tstate>::bprop(Tstate &in1, Tstate &in2,
			     Tstate &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.dx, in2.dx, out.dx);// must have same dimensions

    idx_add(out.dx, in1.dx); // derivative wrt in1
    idx_add(out.dx, in2.dx); // derivative wrt in2
  }

  template <typename T, class Tstate>
  void diff_module<T,Tstate>::bbprop(Tstate &in1, Tstate &in2,
			      Tstate &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.ddx, in2.ddx, out.ddx);// must have same dimensions

    idx_add(out.ddx, in1.ddx); // derivative wrt in1
    idx_add(out.ddx, in2.ddx); // derivative wrt in2
  }

  ////////////////////////////////////////////////////////////////
  // mul_module

  template <typename T, class Tstate>
  mul_module<T,Tstate>::mul_module() : module_2_1<T,Tstate>("mul") {
  }
  
  template <typename T, class Tstate>
  mul_module<T,Tstate>::~mul_module() {
  }

  //! {<code>
  //!    y = x1 .* x2
  //! </code>}
  template <typename T, class Tstate>
  void mul_module<T,Tstate>::fprop(Tstate &in1, Tstate &in2, Tstate &out) {
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
  template <typename T, class Tstate>
  void mul_module<T,Tstate>::bprop(Tstate &in1, Tstate &in2, Tstate &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.dx, in2.dx, out.dx);// must have same dimensions

    if (!tmp.same_dim(in1.x.get_idxdim())) { // resize temp buffer
      idxdim d(in1.x);
      tmp = idx<T>(d);
    }
    idx_mul(out.dx, in2.x, tmp);
    idx_add(tmp, in1.dx);
    idx_mul(out.dx, in1.x, tmp);
    idx_add(tmp, in2.dx);
  }
   
  //! {<code>
  //!    d^2y\dx1^2 = (x2).^2
  //!    d^2y\dx2^2 = (x1).^2
  //! </code>}
  template <typename T, class Tstate>
  void mul_module<T,Tstate>::bbprop(Tstate &in1, Tstate &in2,
			     Tstate &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.ddx, in2.ddx, out.ddx);// must have same dimensions

    if (!tmp.same_dim(in1.x.get_idxdim())) { // resize temp buffer
      idxdim d(in1.x);
      tmp = idx<T>(d);
    }
    idx_mul(in2.x, in2.x, tmp);
    idx_mul(out.ddx, tmp, tmp);
    idx_add(tmp, in1.ddx);
    idx_mul(in1.x, in1.x, tmp);
    idx_mul(out.ddx, tmp, tmp);
    idx_add(tmp, in2.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // thres_module

  template <typename T, class Tstate>
  thres_module<T,Tstate>::thres_module(T thres_, T val_)
    : module_1_1<T,Tstate>("thres"), thres(thres_), val(val_) {
  }

  template <typename T, class Tstate>
  thres_module<T,Tstate>::~thres_module() {
  }

  template <typename T, class Tstate>
  void thres_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
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

  template <typename T, class Tstate>
  void thres_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions

    idx_aloop3(inx, in.x, T, indx, in.dx, T, outdx, out.dx, T) {
      if (*inx > thres)
	*indx += *outdx;
    }
  }

  template <typename T, class Tstate>
  void thres_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions

    idx_add(out.ddx, in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // cutborder_module

  template <typename T, class Tstate>
  cutborder_module<T,Tstate>::cutborder_module(int nr_, int nc_)
    : module_1_1<T,Tstate>("cutborder"), nrow(nr_), ncol(nc_) {
  }

  template <typename T, class Tstate>
  cutborder_module<T,Tstate>::~cutborder_module() {
  }

  template <typename T, class Tstate>
  void cutborder_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
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

  template <typename T, class Tstate>
  void cutborder_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    
    intg inr = out.x.dim(1);
    intg inc = out.x.dim(2);
    idx<T> tmp = in.dx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(out.dx, tmp);
  }

  template <typename T, class Tstate>
  void cutborder_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = out.x.dim(1);
    intg inc = out.x.dim(2);
    idx<T> tmp = in.ddx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(out.ddx, tmp);
  }

  ////////////////////////////////////////////////////////////////
  // zpad_module

  template <typename T, class Tstate>
  zpad_module<T,Tstate>::zpad_module(int nr, int nc)
    : module_1_1<T,Tstate>("zpad"), nrow(nr), ncol(nc), nrow2(nr), ncol2(nc) {
  }

  template <typename T, class Tstate>
  zpad_module<T,Tstate>::zpad_module(idxdim kerdims)
    : nrow((int) (floor(kerdims.dim(0) / (float) 2.0))),
      ncol((int) (floor(kerdims.dim(1) / (float) 2.0))),
      nrow2(nrow), ncol2(ncol) {
    // remove 1 pixel on right and bottom borders if even.
    if (kerdims.dim(0) % 2 == 0)
      nrow2 -= 1;
    if (kerdims.dim(1) % 2 == 0)
      ncol2 -= 1;
  }

  template <typename T, class Tstate>
  zpad_module<T,Tstate>::~zpad_module() {
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idxdim d(in.x.dim(0), inr + nrow + nrow2, inc + ncol + ncol2);
    if (!out.x.same_dim(d)) // resize only when necessary
      out.resize(d);
    out.clear();
    idx<T> tmp = out.x.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_copy(in.x, tmp);

#ifdef __DUMP_STATES__ // used to debug
    DUMP(out.x, this->name() << "_zpad_module_out");
#endif
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idx<T> tmp = out.dx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(tmp, in.dx);
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idx<T> tmp = out.ddx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(tmp, in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // mirrorpad_module

  template <typename T, class Tstate>
  mirrorpad_module<T,Tstate>::mirrorpad_module(int nr, int nc)
    : module_1_1<T,Tstate>("mirrorpad"), nrow(nr), ncol(nc) {
  }

  template <typename T, class Tstate>
  mirrorpad_module<T,Tstate>::~mirrorpad_module() {
  }

  template <typename T, class Tstate>
  void mirrorpad_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idxdim d(in.x.dim(0), inr + 2 * nrow, inc + 2 * ncol);
    if (!out.x.same_dim(d)) // resize only when necessary
      out.resize(d);
    idx<T> tmp, tmp2;
    int i;
    tmp = out.x.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_copy(in.x, tmp);
    // mirror border left
    for (i = 0; i < ncol; ++i) {
      tmp2 = in.x.narrow(1, 1, ncol - i - 1);
      tmp = out.x.narrow(1, 1, i);
      tmp = tmp.narrow(2, in.x.dim(2), ncol);
      idx_copy(tmp2, tmp);
    }
    // mirror border right
    for (i = 0; i < ncol; ++i) {
      tmp2 = in.x.narrow(1, 1, in.x.dim(1) - ncol - 1 + i);
      tmp = out.x.narrow(1, 1, out.x.dim(1) - 1 - i);
      tmp = tmp.narrow(2, in.x.dim(2), ncol);
      idx_copy(tmp2, tmp);
    }
    // mirror border top using out as input
    for (i = 0; i < nrow; ++i) {
      tmp2 = out.x.narrow(2, 1, nrow + nrow - i - 1);
      tmp = out.x.narrow(2, 1, i);
      idx_copy(tmp2, tmp);
    }
    // mirror border bottom using out as input
    for (i = 0; i < nrow; ++i) {
      tmp2 = out.x.narrow(2, 1, out.x.dim(2) - nrow * 2 - 1 + i);
      tmp = out.x.narrow(2, 1, out.x.dim(2) - 1 - i);
      idx_copy(tmp2, tmp);
    }
  }

  template <typename T, class Tstate>
  void mirrorpad_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idx<T> tmp = out.dx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(tmp, in.dx);
  }

  template <typename T, class Tstate>
  void mirrorpad_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    idx<T> tmp = out.ddx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(tmp, in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // fsum_module

  template <typename T, class Tstate>
  fsum_module<T,Tstate>::fsum_module() : module_1_1<T,Tstate>("fsum") {
  }

  template <typename T, class Tstate>
  fsum_module<T,Tstate>::~fsum_module() {
  }
  
  template <typename T, class Tstate>
  void fsum_module<T,Tstate>::fprop(Tstate &in, Tstate &out) { 
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

  template <typename T, class Tstate>
  void fsum_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions
    
    idx_eloop2(indx2, in.dx, T, outdx2, out.dx, T) {
      idx_eloop2(indx1, indx2, T, outdx1, outdx2, T) {
	idx_addc(indx1, idx_sum(outdx1), indx1);
      }
    }
  }

  template <typename T, class Tstate>
  void fsum_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions
    
    idx_eloop2(inddx2, in.ddx, T, outddx2, out.ddx, T) {
      idx_eloop2(inddx1, inddx2, T, outddx1, outddx2, T) {
	idx_addc(inddx1, idx_sum(outddx1), inddx1);
      }
    }
  }
    
  ////////////////////////////////////////////////////////////////
  // binarize_module

  template <typename T, class Tstate>
  binarize_module<T,Tstate>::binarize_module(T threshold_, T false_value_,
				      T true_value_)
    : threshold(threshold_), false_value(false_value_), true_value(true_value_){
  }

  template <typename T, class Tstate>
  binarize_module<T,Tstate>::~binarize_module() {
  }
  
  template <typename T, class Tstate>
  void binarize_module<T,Tstate>::fprop(Tstate &in, Tstate &out) { 
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      out.resize(d);
    }
    idx_aloop2(inx, in.x, T, outx, out.x, T) {
      if (*inx > threshold)
	*outx = true_value;
      else
	*outx = false_value;
    }
  }

  ////////////////////////////////////////////////////////////////
  // range_lut_module

  template <typename T, class Tstate>
  range_lut_module<T,Tstate>::range_lut_module(idx<T> *value_range_) {
    if (value_range_ == NULL)
      eblerror("expected non null range matrix");
    value_range = idx<T>(value_range_->get_idxdim());
    idx_copy(*value_range_, value_range);
  }

  template <typename T, class Tstate>
  range_lut_module<T,Tstate>::~range_lut_module() {
  }
  
  template <typename T, class Tstate>
  void range_lut_module<T,Tstate>::fprop(Tstate &in, Tstate &out) { 
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x); // use same dimensions as in
      out.resize(d);
    }
    idx_aloop2(inx, in.x, T, outx, out.x, T) {
      //      cout << "v0: " << *inx;
      idx_bloop1(vr, value_range, T) {
	if (*inx < vr.get(1)) {
	  *outx = vr.get(0);
	  break ;
	}
      }
      //      cout << " v1: " << *outx << endl;
    }
  }

  ////////////////////////////////////////////////////////////////
  // diag_module

  template <typename T, class Tstate>
  diag_module<T, Tstate>::diag_module(parameter<T,Tstate> *p, intg thick,
				      const char *name_)
    : module_1_1<T,Tstate>(name_), coeff(p, thick) {
    // initialize coeffs to 1
    idx_fill(coeff.x, (T)1.0);
  }

  template <typename T, class Tstate>
  diag_module<T, Tstate>::~diag_module() {
  }

  template <typename T, class Tstate>
  void diag_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)

    idx_bloop3(c, coeff.x, T, i, in.x, T, o, out.x, T) {
      idx_dotc(i, c.get(), o);
    }

#ifdef __DUMP_STATES__ // used to debug
    DUMP(in.x, this->name() << "_diag_module_in.x");
    DUMP(coeff.x, this->name() << "_diag_module_coeff");
#endif
  }

  template <typename T, class Tstate>
  void diag_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    idx_bloop5(c, coeff.x, T, cd, coeff.dx, T, i, in.x, T, id, in.dx, T, 
	       od, out.dx, T) {
      idx_dotcacc(od, c.get(), id); // bprop to input
      idx_dotacc(i, od, cd); // bprop to weights
    }
  }

  template <typename T, class Tstate>
  void diag_module<T, Tstate>::bbprop(Tstate &in,
					Tstate &out) {
    idx_bloop5(c, coeff.x, T, cdd, coeff.ddx, T, i, in.x, T, idd, in.ddx, T, 
	       odd, out.ddx, T) {
      idx_dotcacc(odd, c.get() * c.get(), idd); // bprop to input
      idx_m2squdotm2acc(i, odd, cdd); // bprop to weights
    }
  }

  template <typename T, class Tstate>
  void diag_module<T, Tstate>::resize_output(Tstate &in, Tstate &out) {
    // resize output based on input dimensions
    idxdim d(in.x); // use same dimensions as in
    d.setdim(0, coeff.x.dim(0)); // except for the first one
    if (out.x.get_idxdim() != d) { // resize only if necessary
#ifdef __DEBUG__
      cout << "linear: resizing output from " << out.x.get_idxdim();
      cout << " to " << d << endl;
#endif
      out.resize(d);
    }
  }

  template <typename T, class Tstate>
  void diag_module<T, Tstate>::load_x(idx<T> &weights) {
    if (!coeff.x.same_dim(weights))
      eblthrow("expected same dimension weights but got " << coeff.x << " and "
	       << weights << " instead");
    idx_copy(weights, coeff.x);
  }

  template <typename T, class Tstate>
  std::string diag_module<T, Tstate>::describe() {
    std::string desc;
    desc << "diag module " << this->name() << " with "
	 << coeff.x << " coefficients";
    return desc;
  }
  
  ////////////////////////////////////////////////////////////////
  // copy_module

  template <typename T, class Tstate>
  copy_module<T, Tstate>::copy_module(const char *name_)
    : module_1_1<T,Tstate>(name_) {
  }

  template <typename T, class Tstate>
  copy_module<T, Tstate>::~copy_module() {
  }

  template <typename T, class Tstate>
  void copy_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    idx_copy(in.x, out.x);
  }

  template <typename T, class Tstate>
  void copy_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    idx_copy(out.dx, in.dx);
  }

  template <typename T, class Tstate>
  void copy_module<T, Tstate>::bbprop(Tstate &in, Tstate &out) {
    idx_copy(out.ddx, in.ddx);
  }

  template <typename T, class Tstate>
  void copy_module<T, Tstate>::resize_output(Tstate &in, Tstate &out) {
    // resize output based on input dimensions
    idxdim d(in.x); // use same dimensions as in
    if (out.x.get_idxdim() != d) { // resize only if necessary
#ifdef __DEBUG__
      cout << "copy: resizing output from " << out.x.get_idxdim();
      cout << " to " << d << endl;
#endif
      out.resize(d);
    }
  }

  template <typename T, class Tstate>
  std::string copy_module<T, Tstate>::describe() {
    std::string desc;
    desc << "copy module " << this->name();
    return desc;
  }
  
  ////////////////////////////////////////////////////////////////
  // back_module

#define BACK_MIN -10.0

  template <typename T, class Tstate>
  back_module<T, Tstate>::back_module(const char *name_)
    : module_1_1<T,Tstate>(name_), s0(NULL), s1(NULL), s2(NULL) {
  }

  template <typename T, class Tstate>
  back_module<T, Tstate>::~back_module() {
  }

  template <typename T, class Tstate>
  void back_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    // copy input to s0
    idx_copy(in.x, *s0);
    cout << "back: mins: so: " << idx_min(*s0) << " s1: " << idx_min(*s1) << " s2: "
	 << idx_min(*s2) << endl;
    cout << "back: maxs: so: " << idx_max(*s0) << " s1: " << idx_max(*s1) << " s2: "
	 << idx_max(*s2) << endl;
    // put max of all buffers in output
//     idx_aloop3(x0, *s0, T, x1, *s1, T, o, out.x, T) {
//       *o = std::max(*x0, *x1);
//     }
    // put max of all buffers in output
    idx_aloop4(x0, *s0, T, x1, *s1, T, x2, *s2, T, o, out.x, T) {
      *o = std::max(*x0, std::max(*x1, *x2));
    }
  }

  template <typename T, class Tstate>
  void back_module<T, Tstate>::bb(std::vector<bbox*> &boxes) {
    cout << "back: " << boxes.size() << " boxes" << endl;
    // shift internal buffers and clear first one
    idx_copy(*s1, *s2);
    idx_fill(*s1, (T)BACK_MIN);
    // copy all boxes features to s1
    int height = s0->dim(1);
    int width = s0->dim(2);
    for (uint i = 0; i < boxes.size(); ++i) {
      bbox &b = *(boxes[i]);
      // find box's location at this stage
      float rho = b.oh0 / (float) b.oheight;
      float rwo = b.ow0 / (float) b.owidth;
      int h0 = (int) (height * rho);
      int w0 = (int) (width * rwo);
      int h = pixel_size.dim(1);
      int w = pixel_size.dim(2);
      // cut bbox if outside of buffers
      if (h0 < 0) { h -= h0; h0 = 0; }
      if (w0 < 0) { w -= w0; w0 = 0; }
      if (h0 + h > height) h -= h0 + h - height;
      if (w0 + w > width) w -= w0 + w - width;
      // max-copy box features from s0 to s1
      idx<T> b1 = s1->narrow(1, h, h0);
      b1 = b1.narrow(2, w, w0);
      idx<T> b0 = s0->narrow(1, h, h0);
      b0 = b0.narrow(2, w, w0);
      idx_max(b0, b1);      
    }
    // shift buffers for horizontal motion
    int wshift = (int) (.02 * width);
    cout << "back: shift by " << wshift << " pixels (width: " 
	 << width << ")" << endl;
    idx<T> tmp(s1->get_idxdim());
    idx_fill(tmp, (T)BACK_MIN);
    idx<T> shifted = tmp.narrow(2, width - wshift, wshift);
    idx<T> original = s1->narrow(2, width - wshift, 0);
    idx_copy(original, shifted);
    idx_copy(tmp, *s1);
    // shift s2
    idx_fill(tmp, (T)BACK_MIN);
    shifted = tmp.narrow(2, width - wshift, wshift);
    original = s2->narrow(2, width - wshift, 0);
    idx_copy(original, shifted);
    idx_copy(tmp, *s2);
    // decay buffers
    //    idx_addc(*s1, (T) -0.2, *s1);
    //    idx_addc(*s2, (T) -0.2, *s2);
  }

  template <typename T, class Tstate>
  void back_module<T, Tstate>::resize_output(Tstate &in, Tstate &out) {
    // resize output based on input dimensions
    idxdim d(in.x); // use same dimensions as in
    if (out.x.get_idxdim() != d) { // resize only if necessary
#ifdef __DEBUG__
      cout << "back: resizing output from " << out.x.get_idxdim();
      cout << " to " << d << endl;
#endif
      out.resize(d);
    }
    if (!s0 || s0->get_idxdim() != d) {
      cout << "back: resizing internal buffers to " << d << endl;
      if (s0) s0->resize(d); else s0 = new idx<T>(d);
      if (s1) s1->resize(d); else s1 = new idx<T>(d);
      if (s2) s2->resize(d); else s2 = new idx<T>(d);
      idx_fill(*s0, (T)BACK_MIN);
      idx_fill(*s1, (T)BACK_MIN);
      idx_fill(*s2, (T)BACK_MIN);
    }
  }

  template <typename T, class Tstate>
  idxdim back_module<T, Tstate>::bprop_size(const idxdim &osize) {
    pixel_size = osize;
    cout << "back_module: 1 output pixel corresponds here to " << pixel_size
	 << endl;
    return osize;
  }

  template <typename T, class Tstate>
  std::string back_module<T, Tstate>::describe() {
    std::string desc;
    desc << "back module " << this->name();
    return desc;
  }

  ////////////////////////////////////////////////////////////////
  // maxss_module

  template <typename T, class Tstate>
  maxss_module<T,Tstate>::
  maxss_module(intg stridei_, intg stridej_,
	       intg subi_, intg subj_, intg thick, const char *name_)
    : module_1_1<T,Tstate>(name_), 
      subi(subi_), subj(subj_), thickness(thick), 
      stridei(stridei_), stridej(stridej_),
      switches(thick, 1, 1, 2) {
  }

  template <typename T, class Tstate>
  maxss_module<T,Tstate>::~maxss_module() {
  }

  template <typename T, class Tstate>
  void maxss_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    { idx_bloop3(lix, in.x, T, sw, switches, int, ltx, out.x, T) {
	int i = 0, j = 0;
	idx<T> uuin(lix.unfold(1, subj, stridej));
	uuin = uuin.unfold(0, subi, stridei);
	idx_bloop3(z1, uuin, T, sw1, sw, int, o1, ltx, T) {
	  idx_bloop3(z2, z1, T, sw2, sw1, int, o2, o1, T) {
	    intg indx = idx_indexmax(z2); // find index of max
	    sw2.set((int) (indx / subj + stridei * i), 0); // height in input
	    sw2.set((int) (indx / subj + stridej * j), 1); // width in input
	    o2.set(z2.get(indx)); // copy max to output
	  }
	  j++;
	}
	i++;
      }}

#ifdef __DUMP_STATES__ // used to debug
    DUMP(in.x, this->name() << "_maxss_module_in.x");
    DUMP(out.x, this->name() << "_maxss_module_out.x");
#endif
  }

  template <typename T, class Tstate>
  void maxss_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
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
  void maxss_module<T,Tstate>::bbprop(Tstate &in,
					    Tstate &out) {	
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
  void maxss_module<T,Tstate>::resize_output(Tstate &in, Tstate &out) {
    intg sin_i = in.x.dim(1);
    intg sin_j = in.x.dim(2);
    intg si = sin_i / stridei;
    intg sj = sin_j / stridej;
    // check sizes
    if ((sin_i % stridei) != 0 || (sin_j % stridej) != 0) {
      cerr << "maxss " << sin_i << "x" << sin_j << " with stride "
	   << stridei << "x" << stridej << endl;
      eblerror("inconsistent input size and maxss ratio");
    }
    // resize output and sub based in input dimensions
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(1, si); // new size after maxss
    d.setdim(2, sj); // new size after maxss
    if (out.x.get_idxdim() != d) { // resize only if necessary
#ifdef __DEBUG__
      cout << "maxss: resizing output from " << out.x.get_idxdim();
      cout << " to " << d << endl;
#endif
      out.resize(d);
      d.insert_dim(2, d.order());
      switches.resize(d);
    }
  }
 
  template <typename T, class Tstate>
  idxdim maxss_module<T,Tstate>::fprop_size(idxdim &isize) {
    //! Update input size
    idxdim osize(thickness,
		 std::max((intg) 1, isize.dim(1) / stridei),
		 std::max((intg) 1, isize.dim(2) / stridej));
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  idxdim maxss_module<T,Tstate>::bprop_size(const idxdim &osize) {
    //! Update input size
    idxdim isize(thickness,
		 osize.dim(1) * stridei,
		 osize.dim(2) * stridej);
    return isize;
  }

  template <typename T, class Tstate>
  maxss_module<T,Tstate>* maxss_module<T,Tstate>::copy() {
    // new module (with its own local parameter buffers)
    maxss_module<T,Tstate> *l2 =
      new maxss_module<T, Tstate>(stridei, stridej,
				  subi, subj, thickness);
    return l2;
  }
 
  template <typename T, class Tstate>
  std::string maxss_module<T, Tstate>::describe() {
    std::string desc;
    desc << "maxss module " << this->name() << " with kernel "
	 << subi << "x" << subj
	 << " and stride " << stridei << "x" << stridej;
    return desc;
  }
  
} // end namespace ebl
