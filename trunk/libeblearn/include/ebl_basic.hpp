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
    : w(p, out, in) {
    this->name = name_;
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
    ostringstream fname;
    fname << "dump_" << this->name << "_linear_module_weights_" << w.x
	  << ".mat";
    save_matrix(w.x, fname.str());
    fname.str("");
    fname << "dump_" << this->name << "_linear_module_in.x_" << in.x << ".mat";
    save_matrix(in.x, fname.str());
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
    idxdim d(in.x.spec); // use same dimensions as in
    d.setdim(0, w.x.dim(0)); // except for the first one
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

  ////////////////////////////////////////////////////////////////
  // convolution_module

  template <typename T, class Tstate>
  convolution_module<T, Tstate>::convolution_module(parameter<T,Tstate> *p, 
					    intg kerneli, intg kernelj, 
					    intg stridei_, intg stridej_, 
					    idx<intg> &tbl, const char *name_)
    : kernel(p, tbl.dim(0), kerneli, kernelj), 
      stridei(stridei_), stridej(stridej_), table(tbl), warnings_shown(false),
      float_precision(false) {
    this->name = name_;
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
  void convolution_module<T, Tstate>::fprop(Tstate &in,
					    Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    // unfolding input for a faster convolution operation
    idx<T> uuin(in.x.unfold(1, kernel.x.dim(1), stridei));
    uuin = uuin.unfold(2, kernel.x.dim(2), stridej);
    idx_clear(out.x);
    // convolve 2D slice for each convolution kernel
    { idx_bloop2(lk, kernel.x, T, lt, table, intg) {
	idx<T> suin(uuin.select(0, lt.get(0)));
	idx<T> sout((out.x).select(0, lt.get(1)));
#ifdef __IPP__
	if (float_precision) {
	  rev_idx2_tr(lk, revkernel);
	  //	  idx_clear(outtmp);
	  ipp_convolution(suin, revkernel, outtmp);
	  ipp_add(outtmp, sout);
	}
	else // not using IPP
	  idx_m4dotm2acc(suin, lk, sout); // 2D convolution
#else
	idx_m4dotm2acc(suin, lk, sout); // 2D convolution
#endif
      }}
#ifdef __DUMP_STATES__ // used to debug
    ostringstream fname;
    fname << "dump_" << this->name << "_convolution_module_kernels_"
	  << kernel.x << ".mat";
    save_matrix(kernel.x, fname.str());
    fname.str("");
    fname << "dump_" << this->name << "_convolution_module_in.x_" << in.x
	  << ".mat";
    save_matrix(in.x, fname.str());
    fname.str("");
    fname << "dump_" << this->name << "_convolution_module_ker.x_" << kernel.x
	  << ".mat";
    save_matrix(kernel.x, fname.str());
    fname.str("");
    fname << "dump_" << this->name << "_convolution_module_table_" << table
	  << ".mat";
    save_matrix(table, fname.str());
    fname.str("");
    fname << "dump_" << this->name << "_convolution_module_out.x_" << out.x
	  << ".mat";
    save_matrix(out.x, fname.str());
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

  ////////////////////////////////////////////////////////////////
  // subsampling_module

  template <typename T, class Tstate>
  subsampling_module<T,Tstate>::subsampling_module(parameter<T,Tstate> *p, 
					    intg stridei_, intg stridej_,
					    intg subi, intg subj, 
					    intg thick, const char *name_)
    : coeff(p, thick), sub(thick, subi, subj), thickness(thick), 
      stridei(stridei_), stridej(stridej_) {
    this->name = name_;
  }

  template <typename T, class Tstate>
  subsampling_module<T,Tstate>::~subsampling_module() {
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::fprop(Tstate &in,
					   Tstate &out) {
    if (this->bResize) resize_output(in, out); // resize (iff necessary)
    // subsampling ( coeff * average )
    idx_clear(sub.x);
    { idx_bloop4(lix, in.x, T, lsx, sub.x, T, lcx, coeff.x, T, ltx, out.x, T) {
	idx<T> uuin(lix.unfold(1, stridej, stridej));
	uuin = uuin.unfold(0, stridei, stridei);
	idx_eloop1(z1, uuin, T) {
	  idx_eloop1(z2, z1, T) {
	    idx_add(z2, lsx); // average
	  }
	}
	idx_dotc(lsx, lcx.get(), ltx); // coeff
      }}

#ifdef __DUMP_STATES__ // used to debug
    ostringstream fname;
    fname << "dump_" << this->name << "_subsampling_module_coeff_"
	  << coeff.x << ".mat";
    save_matrix(coeff.x, fname.str());
#endif
  }

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
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

  template <typename T, class Tstate>
  void subsampling_module<T,Tstate>::bbprop(Tstate &in,
					    Tstate &out) {	
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

  ////////////////////////////////////////////////////////////////
  // addc_module

  template <typename T, class Tstate>
  addc_module<T,Tstate>::addc_module(parameter<T,Tstate> *p, intg size,
				     const char *name_)
    : bias(p, size) {
    this->name = name_;
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
    ostringstream fname;
    fname << "dump_" << this->name << "_addc_module_weights_"
	  << bias.x << ".mat";
    save_matrix(bias.x, fname.str());
#endif
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::bprop(Tstate& in, Tstate& out) {
    if ((&in != &out) && (in.dx.nelements() != out.dx.nelements()))
      eblerror("output has wrong size");
    
    idx_bloop3(indx, in.dx, T, biasdx, bias.dx, T, 
	       outdx, out.dx, T) {
      if (&in != &out)
	idx_copy(outdx, indx); // only pass on info if necessary
      idx_sumacc(outdx, biasdx);
    }
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::bbprop(Tstate& in, Tstate& out) {
    if ((&in != &out) && (in.ddx.nelements() != out.ddx.nelements()))
      eblerror("output has wrong size");
    
    idx_bloop3(inddx, in.ddx, T, biasddx, bias.ddx, T, 
	       outddx, out.ddx, T) {
      if (&in != &out)
	idx_copy(outddx, inddx); // only pass on info if necessary
      idx_sumacc(outddx, biasddx);
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

  ////////////////////////////////////////////////////////////////
  // power_module

  template <typename T, class Tstate>
  power_module<T,Tstate>::power_module(T p_) : p(p_) {
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
  diff_module<T,Tstate>::diff_module() {
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
  mul_module<T,Tstate>::mul_module() {
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
    : thres(thres_), val(val_) {
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
    : nrow(nr_), ncol(nc_) {
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
    : nrow(nr), ncol(nc), nrow2(nr), ncol2(nc) {
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
    ostringstream fname;
    fname << "dump_" << "" << "_zpad_module_out_" << out.x
	  << ".mat";
    save_matrix(out.x, fname.str());
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
    : nrow(nr), ncol(nc) {
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
  fsum_module<T,Tstate>::fsum_module() {
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

} // end namespace ebl
