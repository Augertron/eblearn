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

  linear_module::linear_module(parameter *p, intg in, intg out) {
    w = new state_idx(p, out, in);
  }

  linear_module::~linear_module() {
    delete w;
  }

  void linear_module::fprop(state_idx *in, state_idx *out) {
    out->resize(w->x.dim(0)); // resize output based weight matrix
    Idx<double> inx = in->x.view_as_order(1); // view as 1D Idx
    Idx<double> outx = out->x.view_as_order(1); // view as 1D Idx

    idx_m2dotm1(w->x, inx, outx); // linear combination
  }

  void linear_module::bprop(state_idx *in, state_idx *out) {
    Idx<double> inx = in->x.view_as_order(1); // view as 1D Idx
    Idx<double> indx = in->dx.view_as_order(1); // view as 1D Idx
    Idx<double> outdx = out->dx.view_as_order(1); // view as 1D Idx
    Idx<double> twx(w->x.transpose(0, 1)); // switch dimensions 0 and 1
    if (outdx.nelements() != w->dx.dim(0)) ylerror("output has wrong size");

    idx_m1extm1(outdx, inx, w->dx); // backprop to weights
    idx_m2dotm1(twx, outdx, indx); // backprop to input
  }

  void linear_module::bbprop(state_idx *in, state_idx *out) {
    Idx<double> inx = in->x.view_as_order(1); // view as 1D Idx
    Idx<double> inddx = in->ddx.view_as_order(1); // view as 1D Idx
    Idx<double> outddx = out->ddx.view_as_order(1); // view as 1D Idx
    Idx<double> twx = w->x.transpose(0, 1); // switch dimensions 0 and 1
    if (outddx.nelements() != w->ddx.dim(0)) ylerror("output has wrong size");

    idx_m1squextm1(outddx, inx, w->ddx); // backprop to weights
    idx_m2squdotm1(twx, outddx, inddx); // backprop to input
  }

  void linear_module::forget(forget_param_linear &fp) {
    double fanin = w->x.dim(1);
    double z = fp.value / pow(fanin, fp.exponent);
    check_drand_ini(); // check that the random seed was initialized
    idx_aloop1(lx,w->x,double)
      {	*lx = drand(z);}
  }

  void linear_module::normalize() {
    norm_columns(w->x);
  }

  void linear_module::resize_output(state_idx *in, state_idx *out) {
    // resize output based on input dimensions
    IdxDim d(in->x.spec); // use same dimensions as in
    d.setdim(0, w->x.dim(0)); // except for the first one
    out->resize(d);
  }

  ////////////////////////////////////////////////////////////////
  // module_1_1_replicable

  // recursively loop over the last dimensions of input in and out until
  // reaching the operating order.
  // then execute the appropriate function based on the prop code:
  // 1: fprop
  // 2: bprop
  // 3: bbprop
  void module_eloop2(module_1_1_replicable *m,
		     int prop, state_idx *in, state_idx *out) {
    module_1_1<state_idx, state_idx> *m2 = 
      dynamic_cast<module_1_1<state_idx,state_idx>*>(m);
    if (m2->order() == in->x.order()) {
      switch (prop) {
      case 1: m->fprop2(in, out); break ;
      case 2: m->bprop2(in, out); break ;
      case 3: m->bbprop2(in, out); break ;
      default: ylerror("no such code"); break ;
      }
    } else if (m2->order() > in->x.order()) {
      ylerror("the order of the input should be greater or equal to module's\
 operating order");
    } else {
      state_idx_eloop2(iin, *in, oout, *out) {
	module_eloop2(m, prop, &iin, &oout);
      }
    }
  }
  
  // check that orders of input and module are compatible
  void check_orders(module_1_1<state_idx, state_idx> *m, state_idx* in) {
    if (in->x.order() < 0)
      ylerror("module_1_1_replicable cannot replicate this module (order -1)");
    if (in->x.order() < m->order())
      ylerror("input order must be greater or equal to module's operating \
order");
    if (in->x.order() > MAXDIMS)
      ylerror("cannot replicate using more dimensions than MAXDIMS");
  }

  module_1_1_replicable::module_1_1_replicable() {}
  module_1_1_replicable::~module_1_1_replicable() {}

  void module_1_1_replicable::fprop(state_idx *in, state_idx *out) {
    module_1_1<state_idx, state_idx> *m = 
      dynamic_cast<module_1_1<state_idx,state_idx>*>(this);
    check_orders(m, in); // check for orders compatibility
    m->resize_output(in, out); // resize output
    module_eloop2(dynamic_cast<module_1_1_replicable*>(this), 1, in, out);
    }

  void module_1_1_replicable::bprop(state_idx *in, state_idx *out) {
    check_orders(dynamic_cast<module_1_1<state_idx,state_idx>*>(this), in);
    module_eloop2(dynamic_cast<module_1_1_replicable*>(this), 2, in, out);
  }

  void module_1_1_replicable::bbprop(state_idx *in, state_idx *out) {
    check_orders(dynamic_cast<module_1_1<state_idx,state_idx>*>(this), in);
    module_eloop2(dynamic_cast<module_1_1_replicable*>(this), 3, in, out);
  }

  ////////////////////////////////////////////////////////////////
  // linear_module 

  linear_module_dim0::linear_module_dim0(parameter *p, intg in, intg out)
    : linear_module(p, in, out) {
  }

  void linear_module_dim0::fprop(state_idx *in, state_idx *out) {
    // resize output based in input dimensions
    IdxDim d(in->x.spec); // use same dimensions as in
    d.setdim(0, w->x.dim(0)); // except for the first one
    out->resize(d);
    // check that input and output have at most 4 dimensions
    if ((in->x.order() > 4) || (out->x.order() > 4))
      ylerror("linear_module_dim0: supporting up to order 4 Idx only");
    // see input and output as Idx of order 4, so that this module is replicable
    // in up to 3 dimensions (dim 0 for the linear combination operations and
    // remaining dimensions for 1D, 2D or 3D replication)
    Idx<double> inx = in->x.view_as_order(4); // add extra dims if necessary
    Idx<double> outx = out->x.view_as_order(4); // add extra dims if necessary

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

  void linear_module_dim0::bprop(state_idx *in, state_idx *out) {
    // see input and output as Idx of order 4, so that this module is replicable
    // in up to 3 dimensions (dim 0 for the linear combination operations and
    // remaining dimensions for 1D, 2D or 3D replication)
    Idx<double> inx = in->x.view_as_order(4); // add extra dims if necessary
    Idx<double> outx = out->x.view_as_order(4); // add extra dims if necessary
    Idx<double> indx = in->dx.view_as_order(4); // add extra dims if necessary
    Idx<double> outdx = out->dx.view_as_order(4); // add extra dims if necessary
    Idx<double> twx(w->x.transpose(0, 1));
    if (outdx.dim(0) != w->dx.dim(0)) ylerror("output has wrong size");
	    
    // loop over last 3 dimensions and bprop on first dim
    { idx_eloop3(linx,inx,double, lindx,indx,double, loutdx,outdx,double) {
	idx_eloop3(llinx,linx,double, llindx,lindx,double, 
		   lloutdx,loutdx,double) {
	  idx_eloop3(lllinx,llinx,double, lllindx,llindx,double, 
		     llloutdx,lloutdx,double) {
	    idx_m1extm1(llloutdx, lllinx, w->dx);
	    idx_m2dotm1(twx, llloutdx, lllindx);
	  }
	}
      }}
  }

  void linear_module_dim0::bbprop(state_idx *in, state_idx *out) {
    // see input and output as Idx of order 4, so that this module is replicable
    // in up to 3 dimensions (dim 0 for the linear combination operations and
    // remaining dimensions for 1D, 2D or 3D replication)
    Idx<double> inx = in->x.view_as_order(4); // add extra dims if necessary
    Idx<double> inddx = in->ddx.view_as_order(4); // add extra dims if necessary
    Idx<double> outddx = out->ddx.view_as_order(4);
    Idx<double> twx(w->x.transpose(0, 1));
    if (outddx.dim(0) != w->ddx.dim(0)) ylerror("output has wrong size");
    
    // loop over last 3 dimensions and bbprop on first dim
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

  ////////////////////////////////////////////////////////////////
  // convolution_module_2D

  convolution_module_2D::convolution_module_2D(parameter *p, 
					       intg kerneli, intg kernelj, 
					       intg ri, intg rj, 
					       Idx<intg> *tbl, intg thick) {
    table     = tbl;
    kernel    = new state_idx(p, tbl->dim(0), kerneli, kernelj);
    thickness = thick;
    stridei   = ri;
    stridej   = rj;
  }

  convolution_module_2D::~convolution_module_2D() {
    delete kernel;
  }

  void convolution_module_2D::fprop(state_idx *in, state_idx *out) {
    intg ki = kernel->x.dim(1);
    intg kj = kernel->x.dim(2);
    intg sini = in->x.dim(1);
    intg sinj = in->x.dim(2);

    // check sizes
    if (((sini - (ki - stridei)) % stridei != 0) || 
	((sinj - (kj - stridej)) % stridej != 0))
      ylerror("inconsistent input size, kernel size, and subsampling ratio.");
    if ((stridei != 1) || (stridej != 1))
      ylerror("stride > 1 not implemented yet.");
    // check that input and output have at most 4 dimensions
    if ((in->x.order() > 4) || (out->x.order() > 4))
      ylerror("convolution_module_2D: supporting up to order 4 Idx only");
    // unfolding input for a faster convolution operation
    Idx<double> uuin(in->x.unfold(1, ki, stridei));
    uuin = uuin.unfold(2, kj, stridej);
    // resize output based in input dimensions
    IdxDim d(in->x.spec); // use same dimensions as in
    d.setdim(0, thickness); // except for the first one
    d.setdim(1, uuin.dim(1)); // convolution trims dimensions a bit
    d.setdim(2, uuin.dim(2)); // convolution trims dimensions a bit
    out->resize(d);
    // see input and output as Idx of order 4, so that this module is replicable
    // in the 4th dimension if present (dim 0 is the input layers, dims 1 and 2
    // are the 2D dimensions to convolve and dim3 is the optional extra 
    // dimension if we want to work with 3D data by 2D replication).
    Idx<double> inx = in->x.view_as_order(4); // add extra dims if necessary
    Idx<double> outx = out->x.view_as_order(4);

    idx_clear(outx);
    // generic convolution
    // loop on extra 4th dimension
    { idx_eloop2(iinx, inx, double, ooutx, outx, double) {
	// unfold 2D slice for the convolution
	uuin = iinx.unfold(1, ki, stridei);
	uuin = uuin.unfold(2, kj, stridej);
	// loop on 2D slice for each convolution kernel
    	idx_bloop2(lk, kernel->x, double, lt, *table, intg) {
	  Idx<double> suin(uuin.select(0, lt.get(0)));
	  Idx<double> sout(ooutx.select(0, lt.get(1)));
	  idx_m4dotm2acc(suin, lk, sout);
	}
      }}
  }
  
  void convolution_module_2D::bprop(state_idx *in, state_idx *out) {
    // see input and output as Idx of order 4, so that this module is replicable
    // in the 4th dimension if present (dim 0 is the input layers, dims 1 and 2
    // are the 2D dimensions to convolve and dim3 is the optional extra 
    // dimension if we want to work with 3D data by 2D replication).
    Idx<double> inx = in->x.view_as_order(4); // add extra dims if necessary
    Idx<double> indx = in->dx.view_as_order(4);
    Idx<double> outdx = out->dx.view_as_order(4);
    // loop on extra 4th dimension
    { idx_eloop3(linx,inx,double, lindx,indx,double, loutdx,outdx,double) {
	
	// backprop through convolution
	idx_clear(lindx);
	Idx<double> uuin(lindx.unfold(1, (kernel->dx).dim(1), stridei));
	uuin = uuin.unfold(2, (kernel->dx).dim(2), stridej);
	Idx<double> uuinf(linx.unfold(1, (kernel->dx).dim(1), stridei));
	uuinf = uuinf.unfold(2, (kernel->dx).dim(2), stridej);
	int transp[5] = { 0, 3, 4, 1, 2 };
	Idx<double> borp(uuinf.transpose(transp));
	{ idx_bloop3 (lk, kernel->dx, double, lkf, kernel->x, double, 
		      lt, *table, intg) {
	    intg islice = lt.get(0);
	    Idx<double> suin(uuin.select(0, islice));
	    Idx<double> sborp(borp.select(0, islice));
	    Idx<double> sout((loutdx).select(0, lt.get(1)));
	    // backward convolution
	    idx_m2extm2acc(sout, lkf, suin);
	    // compute gradient for kernel
	    idx_m4dotm2acc(sborp, sout, lk);
	  }}
      }}
  }

  void convolution_module_2D::bbprop(state_idx *in, state_idx *out) {
    // see input and output as Idx of order 4, so that this module is replicable
    // in the 4th dimension if present (dim 0 is the input layers, dims 1 and 2
    // are the 2D dimensions to convolve and dim3 is the optional extra 
    // dimension if we want to work with 3D data by 2D replication).
    Idx<double> inx = in->x.view_as_order(4); // add extra dims if necessary
    Idx<double> inddx = in->ddx.view_as_order(4);
    Idx<double> outddx = out->ddx.view_as_order(4);
    // loop on extra 4th dimension
    { idx_eloop3(linx,inx,double, linddx,inddx,double, loutddx,outddx,double) {
	
	// backprop through convolution
	idx_clear(linddx);
	Idx<double> uuin(linddx.unfold(1, (kernel->ddx).dim(1), stridei));
	uuin = uuin.unfold(2, (kernel->ddx).dim(2), stridej);
	Idx<double> uuinf(linx.unfold(1, (kernel->ddx).dim(1), stridei));
	uuinf = uuinf.unfold(2, (kernel->ddx).dim(2), stridej);
	int transp[5] = { 0, 3, 4, 1, 2 };
	Idx<double> borp(uuinf.transpose(transp));
	{ idx_bloop3 (lk, kernel->ddx, double, lkf, kernel->x, double, 
		      lt, *table, intg) {
	    intg islice = lt.get(0);
	    Idx<double> suin(uuin.select(0, islice));
	    Idx<double> sborp(borp.select(0, islice));
	    Idx<double> sout((loutddx).select(0, lt.get(1)));
	    // backward convolution
	    idx_m2squextm2acc(sout, lkf, suin);
	    // compute gradient for kernel
	    idx_m4squdotm2acc(sborp, sout, lk);
	  }}
      }}
  }

  void convolution_module_2D::forget(forget_param_linear &fp) {
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
	{ idx_bloop1(lx, x, double) {
	    { idx_bloop1(llx, lx, double) {
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
					       intg thick) {
    coeff     = new state_idx(p, thick);
    sub	      = new state_idx(thick, subi, subj);
    thickness = thick;
    stridei   = stridei_;
    stridej   = stridej_;
  }

  subsampling_module_2D::~subsampling_module_2D() {
    delete coeff;
    delete sub;
  }

  void subsampling_module_2D::fprop(state_idx *in, state_idx *out) {
    intg sin_i = in->x.dim(1);
    intg sin_j = in->x.dim(2);
    intg si = sin_i / stridei;
    intg sj = sin_j / stridej;
    // check sizes
    if ((sin_i % stridei) != 0 || (sin_j % stridej) != 0)
      ylerror("inconsistent input size and subsampleing ratio");
    // check that input and output have at most 4 dimensions
    if ((in->x.order() > 4) || (out->x.order() > 4))
      ylerror("subsampling_module_2D: supporting up to order 4 Idx only");
    // resize output and sub based in input dimensions
    IdxDim d(in->x.spec); // use same dimensions as in
    d.setdim(1, si); // new size after subsampling
    d.setdim(2, sj); // new size after subsampling
    out->resize(d);
    sub->resize(d);
    // see input and output as Idx of order 4, so that this module is replicable
    // in the 4th dimension if present (dim 0 is the input layers, dims 1 and 2
    // are the 2D dimensions to subsample and dim3 is the optional extra 
    // dimension if we want to work with 3D data by 2D replication).
    Idx<double> inx = in->x.view_as_order(4); // add extra dims if necessary
    Idx<double> subx = sub->x.view_as_order(4);
    Idx<double> outx = out->x.view_as_order(4);
    // loop on extra 4th dimension
    { idx_eloop3(linx,inx,double, lsubx,subx,double, loutx,outx,double) {

	// subsampling ( coeff * average )
	idx_clear(lsubx);
	{ idx_bloop4(lix, linx, double, lsx, lsubx, double,
		     lcx, coeff->x, double, ltx, loutx, double) {
	    Idx<double> uuin(lix.unfold(1, stridej, stridej));
	    uuin = uuin.unfold(0, stridei, stridei);
	    idx_eloop1(z1, uuin, double) {
	      idx_eloop1(z2, z1, double) {
		idx_add(z2, lsx, lsx);
	      }
	    }
	    idx_dotc(lsx, lcx.get(), ltx);
	  }}
      }}
  }

  void subsampling_module_2D::bprop(state_idx *in, state_idx *out) {
    // see input and output as Idx of order 4, so that this module is replicable
    // in the 4th dimension if present (dim 0 is the input layers, dims 1 and 2
    // are the 2D dimensions to oversample and dim3 is the optional extra 
    // dimension if we want to work with 3D data by 2D replication).
    Idx<double> indx = in->dx.view_as_order(4); // add extra dims if necessary
    Idx<double> subx = sub->x.view_as_order(4);
    Idx<double> subdx = sub->dx.view_as_order(4);
    Idx<double> outdx = out->dx.view_as_order(4);
    // loop on extra 4th dimension
    { idx_eloop4(lindx,indx,double, lsubx,subx,double, lsubdx,subdx,double, 
		 loutdx,outdx,double) {
	
	// oversampling
	idx_bloop3(lcdx, coeff->dx, double, ltdx, loutdx, double,
		   lsx, lsubx, double) {
	  idx_dotacc(lsx, ltdx, lcdx);
	}
	idx_bloop4(lidx, lindx, double, lsdx, lsubdx, double,
		   lcx, coeff->x, double, ltdx2, loutdx, double) {
	  idx_dotc(ltdx2, lcx.get(), lsdx);
	  idx_m2oversample(lsdx, stridei, stridej, lidx);
	}
      }}
  }

  void subsampling_module_2D::bbprop(state_idx *in, state_idx *out) {
    // see input and output as Idx of order 4, so that this module is replicable
    // in the 4th dimension if present (dim 0 is the input layers, dims 1 and 2
    // are the 2D dimensions to oversample and dim3 is the optional extra 
    // dimension if we want to work with 3D data by 2D replication).
    Idx<double> inddx = in->ddx.view_as_order(4); // add extra dims if necessary
    Idx<double> subx = sub->x.view_as_order(4);
    Idx<double> subddx = sub->ddx.view_as_order(4);
    Idx<double> outddx = out->ddx.view_as_order(4);
    // loop on extra 4th dimension
    { idx_eloop4(linddx,inddx,double, lsubx,subx,double, lsubddx,subddx,double, 
		 loutddx,outddx,double) {
	
	// oversampling
	idx_bloop3(lcdx, coeff->ddx, double, ltdx, loutddx, double,
		   lsx, lsubx, double) {
	  idx_m2squdotm2acc(lsx, ltdx, lcdx);
	}
	idx_bloop4(lidx, linddx, double, lsdx, lsubddx, double,
		   lcx, coeff->x, double, ltdx2, loutddx, double) {
	  double cf = lcx.get();
	  idx_dotc(ltdx2, cf * cf, lsdx);
	  idx_m2oversample(lsdx, stridei, stridej, lidx);
	}
      }}
  }

  void subsampling_module_2D::forget(forget_param_linear &fp) {
    double c = fp.value / pow(stridei * stridej, fp.exponent);
    idx_fill(coeff->x, c);
  }

  ////////////////////////////////////////////////////////////////
  // addc_module

  addc_module::addc_module(parameter *p, intg size) {
    bias = new state_idx(p,size);
  }

  addc_module::~addc_module() {
    delete bias;
  }

  void addc_module::fprop(state_idx* in, state_idx* out) {
    if (in != out) { // resize only when input and output are different
      IdxDim d(in->x.spec); // use same dimensions as in
      d.setdim(0, bias->x.dim(0)); // except for the first one
      out->resize(d);
    }

    // add each bias to entire slices cut from the first dimension
    idx_bloop3(inx, in->x, double, biasx, bias->x, double, outx, out->x, double)
      {
	idx_addc(inx, biasx.get(), outx);
      }
  }

  void addc_module::bprop(state_idx* in, state_idx* out) {
    if ((in != out) && (in->dx.nelements() != out->dx.nelements()))
      ylerror("output has wrong size");
    
    idx_bloop3(indx, in->dx, double, biasdx, bias->dx, double, 
	       outdx, out->dx, double) {
      if (in != out)
	idx_copy(outdx, indx); // only pass on info if necessary
      idx_sumacc(outdx, biasdx);
    }
  }

  void addc_module::bbprop(state_idx* in, state_idx* out) {
    if ((in != out) && (in->ddx.nelements() != out->ddx.nelements()))
      ylerror("output has wrong size");
    
    idx_bloop3(inddx, in->ddx, double, biasddx, bias->ddx, double, 
	       outddx, out->ddx, double) {
      if (in != out)
	idx_copy(outddx, inddx); // only pass on info if necessary
      idx_sumacc(outddx, biasddx);
    }
  }
  
  void addc_module::forget(forget_param_linear& fp) {
    idx_clear(bias->x);
  }

  void addc_module::normalize() {
  }

} // end namespace ebl
