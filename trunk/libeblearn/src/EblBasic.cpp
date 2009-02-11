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
#include "IdxIO.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // linear_module

  linear_module::linear_module(parameter *p, intg in, intg out)
  {
    w = new state_idx(p, out, in);
  }

  linear_module::~linear_module()
  {
    delete w;
  }

  void linear_module::fprop(state_idx *in, state_idx *out)
  {
    //	intg instride = 0;
    //	intg outstride = 0;
    //	if (in->x.order() == 1)
    //	{
    //		instride = in->x.mod(0);
    //	}
    //	else if (in->x.contiguousp())
    //	{
    //		instride = 1;
    //	}
    //	if (out->x.order() == 1)
    //	{
    //		outstride = out->x.mod(0);
    //		out->resize(w->x.dim(0));
    //	}
    //	else if (out->x.contiguousp())
    //	{
    //		outstride = 1;
    //	}
    //	if ( (instride == 0)||(outstride==0))
    //	{
    //		ylerror("linear_module::fprop: state must 1D or contiguous");
    //	}
    //	else
    //	{
    //		if (out->x.nelements() != w->x.dim(0))
    //		{
    //			ylerror("linear_module::fprop: output has wrong size");
    //		}
    //		else
    //		{
    //			idx_m2dotm1(w->x, in->x, out->x);
    //		}
    //	}
    Idx<double> inx = in->x.view_as_order(1);
    out->resize(w->x.dim(0));
    idx_m2dotm1(w->x, inx, out->x);
  }

  void linear_module::bprop(state_idx *in, state_idx *out)
  {
    Idx<double> inx = in->x.view_as_order(1);
    Idx<double> indx = in->dx.view_as_order(1);
    intg instride = 0;
    intg outstride = 0;
    if (inx.order() == 1)
      {
	instride = inx.mod(0);
      }
    else if (inx.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("linear_module::fprop: state must 1D or contiguous");
      }
    else
      {
	if (out->x.nelements() != w->x.dim(0))
	  {
	    ylerror("linear_module::fprop: output has wrong size");
	  }
	else
	  {
	    Idx<double> twx(w->x.transpose(0, 1));
	    idx_m1extm1(out->dx, inx, w->dx);
	    idx_m2dotm1(twx, out->dx, indx);
	  }
      }
  }

  void linear_module::bbprop(state_idx *in, state_idx *out)
  {
    Idx<double> inx = in->x.view_as_order(1);
    Idx<double> inddx = in->ddx.view_as_order(1);
    intg instride = 0;
    intg outstride = 0;
    if (inx.order() == 1)
      {
	instride = inx.mod(0);
      }
    else if (inx.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("linear_module::fprop: state must 1D or contiguous");
      }
    else
      {
	if (out->x.nelements() != w->x.dim(0))
	  {
	    ylerror("linear_module::fprop: output has wrong size");
	  }
	else
	  {
	    Idx<double> twx = w->x.transpose(0, 1);
	    idx_m1squextm1(out->ddx, inx, w->ddx);
	    idx_m2squdotm1(twx, out->ddx, inddx);
	  }
      }
  }

  void linear_module::forget(forget_param_linear &fp)
  {
    double fanin = w->x.dim(1);
    double z = fp.value / pow(fanin, fp.exponent);
    check_drand_ini();
    idx_aloop1(lx,w->x,double)
      {	*lx = drand(z);}
  }

  void linear_module::normalize()
  {
    norm_columns(w->x);
  }

  ////////////////////////////////////////////////////////////////
  // linear_module 

  linear_module_dim0::linear_module_dim0(parameter *p, intg in, intg out)
    : linear_module(p, in, out) {
  }

  void linear_module_dim0::fprop(state_idx *in, state_idx *out)
  {
    //	intg instride = 0;
    //	intg outstride = 0;
    //	if (in->x.order() == 1)
    //	{
    //		instride = in->x.mod(0);
    //	}
    //	else if (in->x.contiguousp())
    //	{
    //		instride = 1;
    //	}
    //	if (out->x.order() == 1)
    //	{
    //		outstride = out->x.mod(0);
    //		out->resize(w->x.dim(0));
    //	}
    //	else if (out->x.contiguousp())
    //	{
    //		outstride = 1;
    //	}
    //	if ( (instride == 0)||(outstride==0))
    //	{
    //		ylerror("linear_module_dim0::fprop: state must 1D or contiguous");
    //	}
    //	else
    //	{
    //		if (out->x.nelements() != w->x.dim(0))
    //		{
    //			ylerror("linear_module_dim0::fprop: output has wrong size");
    //		}
    //		else
    //		{
    //			idx_m2dotm1(w->x, in->x, out->x);
    //		}
    //	}

    // TODO: resize out?

    // check that input and output have at most 4 dimensions
    if ((in->x.order() > 4) || (out->x.order() > 4))
      ylerror("linear_module_dim0: currently only 4-order idx are supported");
    // see input and output as idx of order 4
    Idx<double> inx = in->x.view_as_order(4);
    Idx<double> outx = out->x.view_as_order(4);
    // loop over last 3 dimensions and call linear combination on first dim
    { idx_eloop2(linx,inx,double, loutx,outx,double) {
	idx_eloop2(llinx,linx,double, lloutx,loutx,double) {
	  idx_eloop2(lllinx,llinx,double, llloutx,lloutx,double) {
	    // multiply weight matrix by input
	    idx_m2dotm1(w->x, lllinx, llloutx);
	  }
	}
      }}
  }

  void linear_module_dim0::bprop(state_idx *in, state_idx *out)
  {
    intg instride = 0;
    intg outstride = 0;
    if (in->x.order() == 1)
      {
	instride = in->x.mod(0);
      }
    else if (in->x.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("linear_module_dim0::fprop: state must 1D or contiguous");
      }
    else
      {
	if (out->x.nelements() != w->x.dim(0))
	  {
	    ylerror("linear_module_dim0::fprop: output has wrong size");
	  }
	else
	  {
	    // see input and output as idx of order 4
	    Idx<double> inx = in->x.view_as_order(4);
	    Idx<double> outx = out->x.view_as_order(4);
	    Idx<double> indx = in->dx.view_as_order(4);
	    Idx<double> outdx = out->dx.view_as_order(4);
	    Idx<double> twx(w->x.transpose(0, 1));
	    // loop over last 3 dimensions and bprop on first dim
	    { idx_eloop4(linx,inx,double, lindx,indx,double, 
			 loutx,outx,double, loutdx,outdx,double) {
		idx_eloop4(llinx,linx,double, llindx,lindx,double,
			   lloutx,loutx,double, lloutdx,loutdx,double) {
		  idx_eloop4(lllinx,llinx,double, lllindx,llindx,double,
			     llloutx,lloutx,double, llloutdx,lloutdx,double) {
		    idx_m1extm1(llloutdx, lllinx, w->dx);
		    idx_m2dotm1(twx, llloutdx, lllindx);
		  }
		}
	      }}
	  }
      }
  }

  void linear_module_dim0::bbprop(state_idx *in, state_idx *out)
  {
    intg instride = 0;
    intg outstride = 0;
    if (in->x.order() == 1)
      {
	instride = in->x.mod(0);
      }
    else if (in->x.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("linear_module_dim0::fprop: state must 1D or contiguous");
      }
    else
      {
	if (out->x.nelements() != w->x.dim(0))
	  {
	    ylerror("linear_module_dim0::fprop: output has wrong size");
	  }
	else
	  {
	    // see input and output as idx of order 4
	    Idx<double> inx = in->x.view_as_order(4);
	    Idx<double> inddx = in->ddx.view_as_order(4);
	    Idx<double> outddx = out->ddx.view_as_order(4);
	    Idx<double> twx(w->x.transpose(0, 1));
	    // loop over last 3 dimensions and bprop on first dim
	    { idx_eloop3(linx,inx,double, linddx,inddx,double, 
			 loutddx,outddx,double) {
		idx_eloop3(llinx,linx,double, llinddx,linddx,double,
			   lloutddx,loutddx,double) {
		  idx_eloop3(lllinx,llinx,double, lllinddx,llinddx,double,
			     llloutddx,lloutddx,double) {
		    idx_m1squextm1(llloutddx, lllinx, w->ddx);
		    idx_m2squdotm1(twx, llloutddx, lllinddx);
		  }
		}
	      }}
	  }
      }
  }

  ////////////////////////////////////////////////////////////////
  // convolution_module_2D

  convolution_module_2D::convolution_module_2D(parameter *p, 
					       intg kerneli, intg kernelj, 
					       intg ri, intg rj, 
					       Idx<intg> *tbl, intg thick)
  {
    table = tbl;
    kernel = new state_idx(p, tbl->dim(0), kerneli, kernelj);
    thickness = thick;
    stridei = ri;
    stridej = rj;
  }

  convolution_module_2D::~convolution_module_2D()
  {
    delete kernel;
  }

  void convolution_module_2D::fprop(state_idx *in, state_idx *out)
  {
    //	intg instride = 0;
    //	intg outstride = 0;
    //	if (in->x.order() == 1)
    //	{
    //		instride = in->x.mod(0);
    //	}
    //	else if (in->x.contiguousp())
    //	{
    //		instride = 1;
    //	}
    //	if (out->x.order() == 1)
    //	{
    //		outstride = out->x.mod(0);
    //		out->resize(w->x.dim(0));
    //	}
    //	else if (out->x.contiguousp())
    //	{
    //		outstride = 1;
    //	}
    //	if ( (instride == 0)||(outstride==0))
    //	{
    //		ylerror("convolution_module_2D::fprop: state must 1D or contiguous");
    //	}
    //	else
    //	{
    //		if (out->x.nelements() != w->x.dim(0))
    //		{
    //			ylerror("convolution_module_2D::fprop: output has wrong size");
    //		}
    //		else
    //		{
    //			idx_m2dotm1(w->x, in->x, out->x);
    //		}
    //	}

     // check that input and output have at most 4 dimensions
//     if ((in->x.order() > 4) || (out->x.order() > 4))
//       ylerror("convolution_module_2D: currently only 4-order idx are supported");

    // see input and output as idx of order 4
//     Idx<double> inx = in->x.view_as_order(4);
//     Idx<double> outx = out->x.view_as_order(4);

    intg ki = kernel->x.dim(1);
    intg kj = kernel->x.dim(2);
    intg sini = in->x.dim(1);
    intg sinj = in->x.dim(2);

    if (((sini - (ki - stridei)) % stridei != 0) || 
	((sinj - (kj - stridej)) % stridej != 0))
      ylerror("inconsistent input size, kernel size, and subsampling ratio.");
    if ((stridei != 1) || (stridej != 1))
      ylerror("stride > 1 not implemented yet.");

    Idx<double> uuin(in->x.unfold(1, ki, stridei));
    uuin = uuin.unfold(2, kj, stridej);
    Idx<double> lki(kernel->x.dim(1), kernel->x.dim(2));

    // resize output if necessary
    IdxDim d(in->x.spec);
    d.setdim(0, thickness);
    d.setdim(1, uuin.dim(1));
    d.setdim(2, uuin.dim(2));
    out->resize(d);

    Idx<double> inx = in->x;
    Idx<double> outx = out->x;
    idx_clear(outx);
    // generic convolution
    //    {	idx_eloop2(uuuin, uuin, double, outx, out->x, double) {
    {	idx_bloop2(lk, kernel->x, double, lt, *table, intg)	{
	  Idx<double> suin(uuin.select(0, lt->get(0)));
	  Idx<double> sout(outx.select(0, lt->get(1)));
	  idx_m4dotm2acc(suin, lk, sout);
	  //	}
      }}
  }

  void convolution_module_2D::bprop(state_idx *in, state_idx *out)
  {
    intg instride = 0;
    intg outstride = 0;
    if (in->x.order() == 1)
      {
	instride = in->x.mod(0);
      }
    else if (in->x.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("convolution_module_2D::fprop: state must 1D or contiguous");
      }
    else
      {
// 	if (out->x.nelements() != w->x.dim(0))
// 	  {
// 	    ylerror("convolution_module_2D::fprop: output has wrong size");
// 	  }
// 	else
// 	  {
	    // backprop through convolution
	    idx_clear(in->dx);
	    Idx<double> uuin(in->dx.unfold(1, (kernel->dx).dim(1), stridei));
	    uuin = uuin.unfold(2, (kernel->dx).dim(2), stridej);
	    Idx<double> uuinf(in->x.unfold(1, (kernel->dx).dim(1), stridei));
	    uuinf = uuinf.unfold(2, (kernel->dx).dim(2), stridej);
	    int transp[5] = { 0, 3, 4, 1, 2 };
	    Idx<double> borp(uuinf.transpose(transp));
	    { idx_bloop3 (lk, kernel->dx, double, lkf, kernel->x, double, 
			  lt, *table, intg) {
		intg islice = lt.get(0);
		Idx<double> suin(uuin.select(0, islice));
		Idx<double> sborp(borp.select(0, islice));
		Idx<double> sout((out->dx).select(0, lt.get(1)));
		// backward convolution
		idx_m2extm2acc(sout, lkf, suin);
		// compute gradient for kernel
		idx_m4dotm2acc(sborp, sout, lk);
	      }}
	    //	  }
      }
  }

  void convolution_module_2D::bbprop(state_idx *in, state_idx *out)
  {
    intg instride = 0;
    intg outstride = 0;
    if (in->x.order() == 1)
      {
	instride = in->x.mod(0);
      }
    else if (in->x.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("convolution_module_2D::fprop: state must 1D or contiguous");
      }
    else
      {
// 	if (out->x.nelements() != w->x.dim(0))
// 	  {
// 	    ylerror("convolution_module_2D::fprop: output has wrong size");
// 	  }
// 	else
// 	  {
	    // backprop through convolution
	    idx_clear(in->ddx);
	    Idx<double> uuin(in->ddx.unfold(1, (kernel->ddx).dim(1), stridei));
	    uuin = uuin.unfold(2, (kernel->ddx).dim(2), stridej);
	    Idx<double> uuinf(in->x.unfold(1, (kernel->ddx).dim(1), stridei));
	    uuinf = uuinf.unfold(2, (kernel->ddx).dim(2), stridej);
	    int transp[5] = { 0, 3, 4, 1, 2 };
	    Idx<double> borp(uuinf.transpose(transp));
	    {	idx_bloop3 (lk, kernel->ddx, double, lkf, kernel->x, double, 
			    lt, *table, intg) {
		intg islice = lt.get(0);
		Idx<double> suin(uuin.select(0, islice));
		Idx<double> sborp(borp.select(0, islice));
		Idx<double> sout((out->ddx).select(0, lt.get(1)));
		// backward convolution
		idx_m2squextm2acc(sout, lkf, suin);
		// compute gradient for kernel
		idx_m4squdotm2acc(sborp, sout, lk);
	      }}
	    //	  }
      }
  }

  void convolution_module_2D::forget(forget_param_linear &fp)
  {
    Idx<double> kx(kernel->x);
    intg vsize = kx.dim(1);
    intg hsize = kx.dim(2);
    Idx<intg> ts(table->select(1, 1));
    Idx<int> fanin(1 + idx_max(ts));
    check_drand_ini();
    idx_clear(fanin);
    { idx_bloop1(tab, *table, intg)	{
	fanin.set(1 + fanin.get(tab.get(1)), tab.get(1)); }}
    { idx_bloop2(tab, *table, intg, x, kx, double) {
	double s = fp.value / pow((vsize * hsize * fanin.get(tab.get(1))), 
				  fp.exponent);
	{	idx_bloop1(lx, x, double)	{
	    {	idx_bloop1(llx, lx, double) {
		double n = drand(-s, s);
		llx.set(n);
	      }}
	  }}
      }}
  }

  ////////////////////////////////////////////////////////////////
  // subsampling_module_2D

  subsampling_module_2D::subsampling_module_2D(parameter *p, 
					       intg stridei_, intg stridej_,
					       intg subi, intg subj, 
					       intg thick)
  {
    coeff = new state_idx(p, thick);
    sub = new state_idx(thick, subi, subj);
    thickness = thick;
    stridei = stridei_;
    stridej = stridej_;
  }

  subsampling_module_2D::~subsampling_module_2D()
  {
    delete coeff;
    delete sub;
  }

  void subsampling_module_2D::fprop(state_idx *in, state_idx *out)
  {
    //	intg instride = 0;
    //	intg outstride = 0;
    //	if (in->x.order() == 1)
    //	{
    //		instride = in->x.mod(0);
    //	}
    //	else if (in->x.contiguousp())
    //	{
    //		instride = 1;
    //	}
    //	if (out->x.order() == 1)
    //	{
    //		outstride = out->x.mod(0);
    //		out->resize(w->x.dim(0));
    //	}
    //	else if (out->x.contiguousp())
    //	{
    //		outstride = 1;
    //	}
    //	if ( (instride == 0)||(outstride==0))
    //	{
    //		ylerror("subsampling_module_2D::fprop: state must 1D or contiguous");
    //	}
    //	else
    //	{
    //		if (out->x.nelements() != w->x.dim(0))
    //		{
    //			ylerror("subsampling_module_2D::fprop: output has wrong size");
    //		}
    //		else
    //		{
    //			idx_m2dotm1(w->x, in->x, out->x);
    //		}
    //	}

     // check that input and output have at most 4 dimensions
//     if ((in->x.order() > 4) || (out->x.order() > 4))
//       ylerror("subsampling_module_2D: currently only 4-order idx are supported");

    // see input and output as idx of order 4
//     Idx<double> inx = in->x.view_as_order(4);
//     Idx<double> outx = out->x.view_as_order(4);

    intg sin_t = in->x.dim(0);
    intg sin_i = in->x.dim(1);
    intg sin_j = in->x.dim(2);
    intg si = sin_i / stridei;
    intg sj = sin_j / stridej;

    if( (sin_i % stridei) != 0 ||
	(sin_j % stridej) != 0)
      ylerror("inconsistent input size and subsampleing ratio");
    sub->resize(sin_t, si, sj);
    out->resize(sin_t, si, sj);
    // 1. subsampling ( coeff * average )
    idx_clear(sub->x);
    { idx_bloop4(lix, in->x, double, lsx, sub->x, double,
		 lcx, coeff->x, double, ltx, out->x, double) {
	Idx<double> uuin(lix->unfold(1, stridej, stridej));
	uuin = uuin.unfold(0, stridei, stridei);
	{ idx_eloop1(z1, uuin, double) {
	    { idx_eloop1(z2, z1, double) {
		idx_add(z2, lsx, lsx);
	      }
	    }
	  }
	}
	idx_dotc(lsx, lcx.get(), ltx);
      }
    }
  }

  void subsampling_module_2D::bprop(state_idx *in, state_idx *out)
  {
    intg instride = 0;
    intg outstride = 0;
    if (in->x.order() == 1)
      {
	instride = in->x.mod(0);
      }
    else if (in->x.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("subsampling_module_2D::fprop: state must 1D or contiguous");
      }
    else
      {
// 	if (out->x.nelements() != w->x.dim(0))
// 	  {
// 	    ylerror("subsampling_module_2D::fprop: output has wrong size");
// 	  }
// 	else
// 	  {
	{ idx_bloop3(lcdx, coeff->dx, double, ltdx, out->dx, double,
		     lsx, sub->x, double) {
	    idx_dotacc(lsx, ltdx, lcdx);
	  }}
	// 4.
	{ idx_bloop4(lidx, in->dx, double, lsdx, sub->dx, double,
		     lcx, coeff->x, double, ltdx2, out->dx, double) {
	    idx_dotc(ltdx2, lcx.get(), lsdx);
	    idx_m2oversample(lsdx, stridei, stridej, lidx);
	  }}
	    //	  }
      }
  }

  void subsampling_module_2D::bbprop(state_idx *in, state_idx *out)
  {
    intg instride = 0;
    intg outstride = 0;
    if (in->x.order() == 1)
      {
	instride = in->x.mod(0);
      }
    else if (in->x.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("subsampling_module_2D::fprop: state must 1D or contiguous");
      }
    else
      {
// 	if (out->x.nelements() != w->x.dim(0))
// 	  {
// 	    ylerror("subsampling_module_2D::fprop: output has wrong size");
// 	  }
// 	else
// 	  {

	{ idx_bloop3(lcdx, coeff->ddx, double, ltdx, out->ddx, double,
		     lsx, sub->x, double) {
	    idx_m2squdotm2acc(lsx, ltdx, lcdx);
	  }}
	// 4.
	{ idx_bloop4(lidx, in->ddx, double, lsdx, sub->ddx, double,
		     lcx, coeff->x, double, ltdx2, out->ddx, double) {
	    double cf = lcx.get();
	    idx_dotc(ltdx2, cf * cf, lsdx);
	    idx_m2oversample(lsdx, stridei, stridej, lidx);
	  }}

      }
  }

  void subsampling_module_2D::forget(forget_param_linear &fp)
  {
    double c = fp.value / pow(stridei * stridej, fp.exponent);
    idx_fill(coeff->x, c);
  }

  ////////////////////////////////////////////////////////////////
  // addc_module

  addc_module::addc_module(parameter *p, intg size)
  {
    bias = new state_idx(p,size);
  }

  addc_module::~addc_module()
  {
    delete bias;
  }

  void addc_module::fprop(state_idx* in, state_idx* out)
  {
    out->resize1(0, bias->x.dim(0));
    idx_bloop3(inx, in->x, double, biasx, bias->x, double, outx, out->x, double)
      {
	idx_addc(inx, biasx.get(), outx);
      }
  }

  void addc_module::bprop(state_idx* in, state_idx* out)
  {
    idx_bloop3(indx, in->dx, double, biasdx, bias->dx, double, 
	       outdx, out->dx, double)
      {
	if (in != out)
	  idx_copy(outdx, indx); // only pass on info if necessary
	idx_sumacc(outdx, biasdx);
      }
  }

  void addc_module::bbprop(state_idx* in, state_idx* out)
  {
    idx_bloop3(inddx, in->ddx, double, biasddx, bias->ddx, double, 
	       outddx, out->ddx, double)
      {
	if (in != out)
	  idx_copy(outddx, inddx); // only pass on info if necessary
	idx_sumacc(outddx, biasddx);
      }
  }

  void addc_module::forget(forget_param_linear& fp)
  {
    idx_clear(bias->x);
  }

  void addc_module::normalize()
  {
  }

} // end namespace ebl
