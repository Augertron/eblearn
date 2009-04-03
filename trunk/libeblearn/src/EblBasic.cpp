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

#include "EblBasic.h"

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
    Idx<double> inx = in.x.view_as_order(1); // view as 1D Idx
    Idx<double> outx = out.x.view_as_order(1); // view as 1D Idx

    idx_m2dotm1(w.x, inx, outx); // linear combination
  }

  void linear_module::bprop(state_idx &in, state_idx &out) {
    Idx<double> inx = in.x.view_as_order(1); // view as 1D Idx
    Idx<double> indx = in.dx.view_as_order(1); // view as 1D Idx
    Idx<double> outdx = out.dx.view_as_order(1); // view as 1D Idx
    Idx<double> twx(w.x.transpose(0, 1)); // switch dimensions 0 and 1
    if (outdx.nelements() != w.dx.dim(0)) ylerror("output has wrong size");

    idx_m1extm1acc(outdx, inx, w.dx); // backprop to weights
    idx_m2dotm1acc(twx, outdx, indx); // backprop to input
  }

  void linear_module::bbprop(state_idx &in, state_idx &out) {
    Idx<double> inx = in.x.view_as_order(1); // view as 1D Idx
    Idx<double> inddx = in.ddx.view_as_order(1); // view as 1D Idx
    Idx<double> outddx = out.ddx.view_as_order(1); // view as 1D Idx
    Idx<double> twx = w.x.transpose(0, 1); // switch dimensions 0 and 1
    if (outddx.nelements() != w.ddx.dim(0)) ylerror("output has wrong size");

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
    IdxDim d(in.x.spec); // use same dimensions as in
    d.setdim(0, w.x.dim(0)); // except for the first one
    out.resize(d);
  }

  ////////////////////////////////////////////////////////////////
  // convolution_module_2D

  convolution_module_2D::convolution_module_2D(parameter &p, 
					       intg kerneli, intg kernelj, 
					       intg ri, intg rj, 
					       Idx<intg> &tbl, intg thick) 
    : kernel(p, tbl.dim(0), kerneli, kernelj), thickness(thick),
      stridei(ri), stridej(rj), table(tbl) {
  }

  convolution_module_2D::~convolution_module_2D() {
  }

  void convolution_module_2D::fprop(state_idx &in, state_idx &out){
    if (bResize) resize_output(in, out); // resize (iff necessary)
    // unfolding input for a faster convolution operation
    Idx<double> uuin(in.x.unfold(1, kernel.x.dim(1), stridei));
    uuin = uuin.unfold(2, kernel.x.dim(2), stridej);
    idx_clear(out.x);
    // convolve 2D slice for each convolution kernel
    { idx_bloop2(lk, kernel.x, double, lt, table, intg) {
      Idx<double> suin(uuin.select(0, lt.get(0)));
      Idx<double> sout((out.x).select(0, lt.get(1)));

      idx_m4dotm2acc(suin, lk, sout); // 2D convolution
      }}
  }
  
  void convolution_module_2D::bprop(state_idx &in, state_idx &out) {
    // backprop through convolution
    idx_clear(in.dx);
    Idx<double> uuin(in.dx.unfold(1, (kernel.dx).dim(1), stridei));
    uuin = uuin.unfold(2, (kernel.dx).dim(2), stridej);
    Idx<double> uuinf(in.x.unfold(1, (kernel.dx).dim(1), stridei));
    uuinf = uuinf.unfold(2, (kernel.dx).dim(2), stridej);
    int transp[5] = { 0, 3, 4, 1, 2 };
    Idx<double> borp(uuinf.transpose(transp));
    { idx_bloop3 (lk, kernel.dx, double, lkf, kernel.x, double, 
		  lt, table, intg) {
	intg islice = lt.get(0);
	Idx<double> suin(uuin.select(0, islice));
	Idx<double> sborp(borp.select(0, islice));
	Idx<double> sout(out.dx.select(0, lt.get(1)));

	idx_m2extm2acc(sout, lkf, suin); // backward convolution
	idx_m4dotm2acc(sborp, sout, lk); // compute gradient for kernel
      }}
  }

  void convolution_module_2D::bbprop(state_idx &in, state_idx &out) {
    // backprop through convolution
    idx_clear(in.ddx);
    Idx<double> uuin(in.ddx.unfold(1, (kernel.ddx).dim(1), stridei));
    uuin = uuin.unfold(2, (kernel.ddx).dim(2), stridej);
    Idx<double> uuinf(in.x.unfold(1, (kernel.ddx).dim(1), stridei));
    uuinf = uuinf.unfold(2, (kernel.ddx).dim(2), stridej);
    int transp[5] = { 0, 3, 4, 1, 2 };
    Idx<double> borp(uuinf.transpose(transp));
    { idx_bloop3 (lk, kernel.ddx, double, lkf, kernel.x, double, 
		  lt, table, intg) {
	intg islice = lt.get(0);
	Idx<double> suin(uuin.select(0, islice));
	Idx<double> sborp(borp.select(0, islice));
	Idx<double> sout((out.ddx).select(0, lt.get(1)));
	    
	idx_m2squextm2acc(sout, lkf, suin); // backward convolution
	idx_m4squdotm2acc(sborp, sout, lk); // compute gradient for kernel
      }}
  }

  void convolution_module_2D::forget(forget_param_linear &fp) {
    Idx<double> kx(kernel.x);
    intg vsize = kx.dim(1);
    intg hsize = kx.dim(2);
    Idx<intg> ts(table.select(1, 1));
    Idx<int> fanin(1 + idx_max(ts));
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

    // check sizes
    if (((sini - (ki - stridei)) % stridei != 0) || 
	((sinj - (kj - stridej)) % stridej != 0))
      ylerror("inconsistent input size, kernel size, and subsampling ratio.");
    if ((stridei != 1) || (stridej != 1))
      ylerror("stride > 1 not implemented yet.");
    // unfolding input for a faster convolution operation
    Idx<double> uuin(in.x.unfold(1, ki, stridei));
    uuin = uuin.unfold(2, kj, stridej);
    // resize output based in input dimensions
    IdxDim d(in.x.spec); // use same dimensions as in
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
	Idx<double> uuin(lix.unfold(1, stridej, stridej));
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
    if ((sin_i % stridei) != 0 || (sin_j % stridej) != 0)
      ylerror("inconsistent input size and subsampleing ratio");
    // resize output and sub based in input dimensions
    IdxDim d(in.x.spec); // use same dimensions as in
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
      IdxDim d(in.x.spec); // use same dimensions as in
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
      ylerror("output has wrong size");
    
    idx_bloop3(indx, in.dx, double, biasdx, bias.dx, double, 
	       outdx, out.dx, double) {
      if (&in != &out)
	idx_copy(outdx, indx); // only pass on info if necessary
      idx_sumacc(outdx, biasdx);
    }
  }

  void addc_module::bbprop(state_idx& in, state_idx& out) {
    if ((&in != &out) && (in.ddx.nelements() != out.ddx.nelements()))
      ylerror("output has wrong size");
    
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

  void addc_module::normalize() {
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
//       ylerror("inconsistent input size, kernel size, and subsampling ratio.");
//     if ((stridei != 1) || (stridej != 1))
//       ylerror("stride > 1 not implemented yet.");
//     Idx<double> uuin = in->x.unfold(1, ki, stridei);
//     uuin = uuin.spec.unfold_inplace(2, kj, stridej);
//     Idx<double> lki = Idx<double>(kernel->x.dim(1), kernel->x.dim(2));
//     // resize output if necessary
//     sum->resize(thickness, uuin.dim(1), uuin.dim(2));
//     out->resize(thickness, uuin.dim(1), uuin.dim(2));
//     idx_clear(sum->x);
//     // generic convolution
//     Idx<double> tout = Idx<double>(sum->x.dim(1), sum->x.dim(2));
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
