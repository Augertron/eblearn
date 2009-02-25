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

#include "Gbl.h"
#include "IdxIO.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////

  f_layer::f_layer(parameter &p, intg tin, intg tout, intg si, intg sj,
		   module_1_1<state_idx,state_idx> *sq)
  {
    weight = new state_idx(p, tout, tin);
    bias = new state_idx(p, tout);
    sum = new state_idx(tout, si, sj);
    squash = sq;
  }

  f_layer::~f_layer()
  {
    delete weight;
    delete bias;
    delete sum;
  }

  void f_layer::forget(forget_param_linear &fp)
  {
    idx_clear(bias->x);
    double z = fp.value / pow(weight->x.dim(1), fp.exponent);
    check_drand_ini();
    idx_aloop1(w,weight->x,double)
      {	*w = drand(z);}
  }

  void f_layer::fprop(state_idx *in, state_idx *out)
  {
    intg inx_d1 = in->x.dim(1);
    intg inx_d2 = in->x.dim(2);
    intg ws = weight->x.dim(0);
    // resize sum and output
    sum->resize(ws, inx_d1, inx_d2);
    out->resize(ws, inx_d1, inx_d2);
    // main matrix multiplication
    {
      int tr[] = { 2, 1, 0 };
      Idx<double> inx(in->x.transpose(tr));
      Idx<double> outx(sum->x.transpose(tr));
      // loop over spatial dimensions
      idx_bloop2(linx,inx,double, loutx,outx,double)
	{
	  idx_bloop2(llinx,linx,double, lloutx,loutx,double)
	    {
	      // multiply weight matrix by input
	      idx_m2dotm1(weight->x, llinx, lloutx);
	    }
	}
    }
    { // add bias
      // fprintf(stdout,"f_layer::fprop adding bias\n");
      // bias->x.pretty(stdout);
      // bias->x.fdump(stdout);
      idx_bloop2(sumx,sum->x,double, biasx,bias->x,double)
	{
	  idx_addc(sumx, biasx.get(), sumx);
	}
      // sum->x.pretty(stdout);
      // sum->x.fdump(stdout);

    }
    // call squashing function
    squash->fprop(*sum, *out);
  }

  void f_layer::bprop(state_idx *in, state_idx *out)
  {
    // backprop through squasher
    squash->bprop(*sum, *out);
    // compute gradient of bias
    {
      idx_bloop2(lha,sum->dx,double, lb,bias->dx,double)
	{	*(lb.ptr()) += idx_sum(lha);}
    }
    // backprop through weight matrix
    int tr[] = { 2, 1, 0 };
    Idx<double> inx(in->x.transpose(tr));
    Idx<double> indx(in->dx.transpose(tr));
    Idx<double> outdx(sum->dx.transpose(tr));
    Idx<double> tkerx(weight->x.transpose(0, 1));
    { idx_bloop3(linx,inx,double, lindx,indx,double, loutdx,outdx,double) {
	{ idx_bloop3(llinx,linx,double, llindx,lindx,double, 
		     lloutdx,loutdx,double) {
	    idx_m1extm1acc(lloutdx,llinx,weight->dx);
	    idx_m2dotm1(tkerx,lloutdx,llindx);
	    // direct call because idxm2dotm1 didn't use to 
	    // work on tranposed matrices
	    // cblas_dgemv(CblasRowMajor, CblasTrans, weight->x.dim(0), 
	    //             weight->x.dim(1),
	    //             1.0, weight->x.idx_ptr(), weight->x.mod(0),
	    //             lloutdx.idx_ptr(), lloutdx.mod(0),
	    //             0.0, llindx.idx_ptr(), llindx.mod(0));
	  }}
      }}
  }

  void f_layer::bbprop(state_idx *in, state_idx *out)
  {
    // backprop through squasher
    squash->bbprop(*sum, *out);
    // compute gradient of bias
    {	idx_bloop2(lha, sum->ddx, double, lb, bias->ddx, double) {
	idx_sumacc(lha, lb); }}
    // backprop through weight matrix
    int tr[] = { 2, 1, 0 };
    Idx<double> inx(in->x.transpose(tr));
    Idx<double> indx(in->ddx.transpose(tr));
    Idx<double> outdx(sum->ddx.transpose(tr));
    Idx<double> tkerx(weight->x.transpose(1, 0));
    idx_bloop3(linx,inx,double, lindx,indx,double, loutdx,outdx,double) {
	idx_bloop3(llinx,linx,double, llindx,lindx,double, 
		   lloutdx,loutdx,double)
	  {
	    idx_m1squextm1acc(lloutdx,llinx,weight->ddx);
	    idx_m2squdotm1(tkerx,lloutdx,llindx);
	  }
      }
  }

  ////////////////////////////////////////////////////////////////

  c_layer::c_layer(parameter &p, intg ki, intg kj, intg ri, intg rj, 
		   Idx<intg> *tbl, intg thick, intg si, intg sj, 
		   module_1_1<state_idx,state_idx> *sqsh)
  {
    thickness = thick;
    stridei = ri;
    stridej = rj;
    kernel = new state_idx(p, tbl->dim(0), ki, kj);
    table = tbl;
    bias = new state_idx(p, thick);
    sum = new state_idx(thick, si, sj);
    squash = sqsh;
  }

  c_layer::~c_layer()
  {
    delete kernel;
    delete bias;
    delete sum;
  }

  void c_layer::set_stride(intg ri, intg rj)
  {
    stridei = ri;
    stridej = rj;
  }

  void c_layer::forget(forget_param_linear &fp)
  {
    idx_clear(bias->x);
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

  void c_layer::fprop(state_idx *in, state_idx *out)
  {
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
    sum->resize(thickness, uuin.dim(1), uuin.dim(2));
    out->resize(thickness, uuin.dim(1), uuin.dim(2));
    idx_clear(sum->x);
    // generic convolution
    {	idx_bloop2(lk, kernel->x, double, lt, *table, intg)	{
	Idx<double> suin(uuin.select(0, lt.get(0)));
	Idx<double> sout((sum->x).select(0, lt.get(1)));
	idx_m4dotm2acc(suin, lk, sout);
      }}
    // add bias
    {	idx_bloop3(sumx, sum->x, double, biasx, bias->x, double,
		   outx, out->x, double)	{
	idx_addc(sumx, biasx.get(), sumx);
      }}
    // call squashing function
    squash->fprop(*sum, *out);
  }

  void c_layer::bprop(state_idx *in, state_idx *out)
  {
    // backprop gradient through squasher
    squash->bprop(*sum, *out);
    // compute gradient of bias
    {	idx_bloop2(lha, sum->dx, double, lb, bias->dx, double) {
	idx_sumacc(lha, lb); }}
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
	Idx<double> sout((sum->dx).select(0, lt.get(1)));
	// backward convolution
	idx_m2extm2acc(sout, lkf, suin);
	// compute gradient for kernel
	idx_m4dotm2acc(sborp, sout, lk);
      }}
  }

  void c_layer::bbprop(state_idx *in, state_idx *out)
  {
    // backprop gradient through squasher
    squash->bbprop(*sum, *out);
    // compute gradient of bias
    { idx_bloop2(lha, sum->ddx, double, lb, bias->ddx, double) {
	idx_sumacc(lha, lb); }}
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
	Idx<double> sout((sum->ddx).select(0, lt.get(1)));
	// backward convolution
	idx_m2squextm2acc(sout, lkf, suin);
	// compute gradient for kernel
	idx_m4squdotm2acc(sborp, sout, lk);
      }}
  }

  ////////////////////////////////////////////////////////////////

#ifdef USE_IPP

  // TODO: Copy IPP in project
  // TODO: ipp 64 for doubles?

  void c_layer_ipp::fprop (state_idx *in, state_idx *out) {
    intg ki = kernel->x.dim(1);
    intg kj = kernel->x.dim(2);
    intg sini = in->x.dim(1);
    intg sinj = in->x.dim(2);

    if (((sini - (ki - stridei) % stridei) != 0) ||
	((sinj - (kj - stridej) % stridej) != 0))
      ylerror("inconsistent input size, kernel size, and subsampling ratio.");
    if ((stridei != 1) || (stridej != 1))
      ylerror("stride > 1 not implemented yet.");
    Idx<double> uuin = in->x.unfold(1, ki, stridei);
    uuin = uuin.spec.unfold_inplace(2, kj, stridej);
    Idx<double> lki = Idx<double>(kernel->x.dim(1), kernel->x.dim(2));
    // resize output if necessary
    sum->resize(thickness, uuin.dim(1), uuin.dim(2));
    out->resize(thickness, uuin.dim(1), uuin.dim(2));
    idx_clear(sum->x);
    // generic convolution
    Idx<double> tout = Idx<double>(sum->x.dim(1), sum->x.dim(2));
    { idx_bloop2(lk, kernel->x, double, lt, *table, intg) {
	rev_idx2_tr(*lk, lki);
	//      ipp_convolution_float(in->x.select(0, lt.get(0)), lki, tout);
	//      ipp_add_float(tout, sum->x.select(0, lt.get(1)));
      }
    }
    // add bias
    { idx_bloop3(sumx, sum->x, double, biasx, bias->x, double,
		 outx, out->x, double) {
	//      ipp_addc_nip_float(sumx, biasx.get(), outx);
      }
    }
    // call squashing function
    squash->fprop(*sum, *out);
  }

  void c_layer_ipp::bprop (state_idx *in, state_idx *out) {
    // backprop gradient through squasher
    squash->bprop(*sum, *out);
    // compute gradient of bias
    { idx_bloop2(lha, sum->dx, double, lb, bias->dx, double) {
	idx_sumacc(lha, lb);
      }
    }
    // backprop through convolution
    idx_clear(in->dx);
    /*
      (let* ((ki (idx-dim :kernel:dx 1))	 (kj (idx-dim :kernel:dx 2))
      (ini (idx-dim :in:dx 1))	 (inj (idx-dim :in:dx 2))
      (outi (idx-dim :out:dx 1))	 (outj (idx-dim :out:dx 2))
      (sumi (idx-dim :sum:dx 1))	 (sumj (idx-dim :sum:dx 2))
      (souti (gbtype-matrix sumi sumj))
      (tout (gbtype-matrix ki kj)))
      (idx-bloop ((lk :kernel:dx) (lkf :kernel:x) (lt table))
      (let* ((islice (lt 0))
      (sout  (select :sum:dx 0 (lt 1)))
      )
      ;; backward convolution
      (ipp-convolution-full-float sout lkf (select :in:dx 0 islice))
      ;; compute gradient for kernel
      (rev-idx2-tr-float sout souti)
      (ipp-convolution-float (select :in:x 0 islice) souti tout)
      (ipp-add-float tout lk)
      )))
    */
  }

#endif

  ////////////////////////////////////////////////////////////////////////

  s_layer::s_layer(parameter &p, intg ki, intg kj, intg thick, intg si, intg sj,
		   module_1_1<state_idx, state_idx> *sqsh) :
    stridei(ki), stridej(kj), squash(sqsh)
  {
    coeff = new state_idx(p, thick);
    bias = new state_idx(p, thick);
    sub = new state_idx(thick, si, sj);
    sum = new state_idx(thick, si, sj);
  }

  s_layer::~s_layer()
  {
    delete coeff;
    delete bias;
    delete sub;
    delete sum;
  }

  void s_layer::fprop(state_idx *in, state_idx *out)
  {
    intg sin_t = in->x.dim(0);
    intg sin_i = in->x.dim(1);
    intg sin_j = in->x.dim(2);
    intg si = sin_i / stridei;
    intg sj = sin_j / stridej;

    if( (sin_i % stridei) != 0 ||
	(sin_j % stridej) != 0)
      ylerror("inconsistent input size and subsampleing ratio");
    sub->resize(sin_t, si, sj);
    sum->resize(sin_t, si, sj);
    out->resize(sin_t, si, sj);
    // 1. subsampling ( coeff * average )
    idx_clear(sub->x);
    { idx_bloop4(lix, in->x, double, lsx, sub->x, double,
		 lcx, coeff->x, double, ltx, sum->x, double) {
	Idx<double> uuin(lix.unfold(1, stridej, stridej));
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
    // 2. add bias
    { idx_bloop3(sumx, sum->x, double, biasx, bias->x, double,
		 outx, out->x, double) {
	idx_addc(sumx, biasx.get(), sumx);
      }
    }
    // 3. call squashing function
    squash->fprop(*sum, *out);
  }

  void s_layer::bprop(state_idx *in, state_idx *out)
  {
    // 1.
    squash->bprop(*sum, *out);
    // 2.
    { idx_bloop2(lha, sum->dx, double, lb, bias->dx, double) {
	idx_sumacc(lha, lb);
      }}
    // 3.
    { idx_bloop3(lcdx, coeff->dx, double, ltdx, sum->dx, double,
		 lsx, sub->x, double) {
	idx_dotacc(lsx, ltdx, lcdx);
      }}
    // 4.
    { idx_bloop4(lidx, in->dx, double, lsdx, sub->dx, double,
		 lcx, coeff->x, double, ltdx2, sum->dx, double) {
	idx_dotc(ltdx2, lcx.get(), lsdx);
	idx_m2oversample(lsdx, stridei, stridej, lidx);
      }}
  }

  void s_layer::bbprop(state_idx *in, state_idx *out)
  {
    // 1.
    squash->bbprop(*sum, *out);
    // 2.
    { idx_bloop2(lha, sum->ddx, double, lb, bias->ddx, double) {
	idx_sumacc(lha, lb);
      }}
    // 3.
    { idx_bloop3(lcdx, coeff->ddx, double, ltdx, sum->ddx, double,
		 lsx, sub->x, double) {
	idx_m2squdotm2acc(lsx, ltdx, lcdx);
      }}
    // 4.
    { idx_bloop4(lidx, in->ddx, double, lsdx, sub->ddx, double,
		 lcx, coeff->x, double, ltdx2, sum->ddx, double) {
	double cf = lcx.get();
	idx_dotc(ltdx2, cf * cf, lsdx);
	idx_m2oversample(lsdx, stridei, stridej, lidx);
      }}
  }

  void s_layer::forget(forget_param_linear &fp) {
    double c = fp.value / pow(stridei * stridej, fp.exponent);
    idx_clear(bias->x);
    idx_fill(coeff->x, c);
  }
  ////////////////////////////////////////////////////////////////////////

  logadd_layer::logadd_layer(intg thick, intg si, intg sj) {
    expdist = Idx<double>(thick, si, sj);
    sumexp = Idx<double>(thick);		// scaled partition function
  }

  void logadd_layer::fprop(state_idx *in, state_idx *out) {
    intg thick = in->x.dim(0);
    intg si = in->x.dim(1);
    intg sj = in->x.dim(2);
    expdist.resize(thick, si, sj);
    out->x.resize(thick);
    if (1 == (si * sj)) {
      // save time and precision if no replication
      Idx<double> inx(in->x.select(2, 0));
      Idx<double> m(inx.select(1, 0));
      Idx<double> ed(expdist.select(2, 0));
      Idx<double> ed1(ed.select(1, 0));
      idx_fill(ed1, 1.0);
      idx_fill(sumexp, 1.0);
      idx_copy(m, out->x);
    }	else {
      // spatially replicated
      // loop over output units
      { idx_bloop4(m, in->x, double, outx, out->x, double,
		   ed, expdist, double, sx, sumexp, double) {
	  // first compute smallest element of m
	  double mini = m.get(0, 0);
	  { idx_bloop1(m1, m, double) {
	      { idx_bloop1(m0, m1, double) {
		  if (m0.get() < mini)
		    mini = m0.get();
		}
	      }
	    }
	  }
	  // now do log-add, and save exponentials
	  double r = 0.0;
	  double w = 1 / (si * sj);
	  { idx_bloop2(m1, m, double, ed1, ed, double) {
	      { idx_bloop2(m0, m1, double, ed0, ed1, double) {
		  ed0.set(w * exp(mini - m0.get()));
		  r += ed0.get();
		}
	      }
	    }
	  }
	  sx.set(r);
	  // put result in output
	  outx.set(mini - log(r));
	}
      }
    }
  }

  void logadd_layer::bprop(state_idx *in, state_idx *out) {
    intg si = in->dx.dim(1);
    intg sj = in->dx.dim(2);
    if ((si * sj) == 1) {
      // save time and precision if no replication
      Idx<double> indx(in->dx.select(2, 0));
      Idx<double> m(indx.select(1, 0));
      idx_copy(out->dx, m);
    } else {
      // spatially replicated
      // loop over output units
      { idx_bloop4(m, in->dx, double, o, out->dx, double,
		   ed, expdist, double, sx, sumexp, double) {
	  { idx_bloop2(m1, m, double, ed1, ed, double) {
	      { idx_bloop2(m0, m1, double, ed0, ed1, double) {
		  m0.set(ed0.get() * o.get() / sx.get());
		}
	      }
	    }
	  }
	}
      }
    }
  }

  void logadd_layer::bbprop(state_idx *in, state_idx *out) {
    { idx_bloop2(o, out->ddx, double, i, in->ddx, double) {
  	idx_fill(i, o.get());
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////

  edist_cost::edist_cost(Idx<ubyte> *classes, intg ini, intg inj, 
			 Idx<double> *p) {
    intg imax = idx_max(*classes);
    intg imin = idx_min(*classes);
    if (imin < 0) ylerror("labels must be positive");
    if (imax > 100000) 
      printf("warning: [edist-cost] largest label is huuuge\n");
    label2classindex = Idx<ubyte>(1 + imax);
    { idx_bloop1(v, label2classindex, ubyte) {
  	v.set(0);
      }
    }
    for (intg i = 0; i < classes->dim(0); ++i)
      label2classindex.set(i, classes->get(i));
    dist = new state_idx(1, ini, inj);
    logadder = new logadd_layer(1, ini, inj);
    logadded_dist = new state_idx(1);
    proto = p;
  }

  void edist_cost::fprop(state_idx *in, Idx<ubyte> *desired, 
			 state_idx *energy) {
    Idx<double> p(proto->select(0, label2classindex.get(desired->get())));
    intg ini = in->x.dim(1);
    intg inj = in->x.dim(2);
    dist->resize(1, ini, inj);
    int tr[] = {1, 2, 0};
    Idx<double> inx(in->x.transpose(tr));
    Idx<double> distx(dist->x.select(0, 0));
    // loop over spatial dimensions
    { idx_bloop2(inx1, inx, double, dx1, distx, double) {
	{ idx_bloop2(inx0, inx1, double, dx0, dx1, double) {
	    // distance between desired prototype and output
	    // at current location
	    idx_sqrdist(p, inx0, dx0);
  	  }
	}
      }
    }
    idx_dotc(distx, 0.5, distx);
    logadder->fprop(dist, logadded_dist);
    energy->x.set(logadded_dist->x.get(0));
  }

  void edist_cost::bprop(state_idx *in, Idx<ubyte> *desired, 
			 state_idx *energy) {
    Idx<double> p(proto->select(0, label2classindex.get(desired->get())));
    // backprop through logadder
    logadded_dist->dx.set(energy->dx.get(), 0);
    logadder->bprop(dist, logadded_dist);
    // backprop through Euclidean distance
    int tr1[] = {1, 2, 0};
    int tr2[] = {1, 2, 0};
    Idx<double> tinx(in->x.transpose(tr1));
    Idx<double> tindx(in->dx.transpose(tr2));
    Idx<double> distdx(dist->dx.select(0, 0));
    // loop over last two dimensions
    { idx_bloop3(linx, tinx, double, lindx, tindx, double, 
		 ldistdx, distdx, double) {
	{ idx_bloop3(llinx, linx, double, llindx, lindx, double, 
		     lldistdx, ldistdx, double) {
	    idx_sub(llinx, p, llindx);
	    idx_dotc(llindx, lldistdx.get(), llindx);
	  }
	}
      }
    }
  }

  // mse has this funny property that the bbprop method mixes up the
  // the first derivative after with the second derivative before, and
  // vice versa. Only the first combination is used here.
  void edist_cost::bbprop(state_idx *in, Idx<ubyte> *desired, 
			  state_idx *energy) {
    // don't bother bbproping through the logadder
    // we would ignore its output anyway
    idx_fill(in->ddx, energy->dx.get());
  }

  ////////////////////////////////////////////////////////////////////////

  classifier_meter::classifier_meter() {
    this->clear();
  }

  int classifier_meter::correctp(ubyte co, ubyte cd) {
    // TODO-0: can co be negative?
    //	if (co == -1)
    //		return 0;
    if (co == cd)
      return 1;
    return -1;
  }

  void classifier_meter::clear() {
    total_correct = 0;
    total_error = 0;
    total_punt = 0;
    total_energy = 0;
    age = 0;
    size = 0;
  }

  void classifier_meter::resize (intg sz) {
    ylerror("not implemented");
  }

  char classifier_meter::update(intg a, class_state *co, ubyte cd, 
				state_idx *en) {
    intg crrct = this->correctp(co->output_class, cd);
    age = a;
    energy = en->x.get();
    confidence = co->confidence;
    total_energy += energy;
    if (crrct == 1)
      total_correct++;
    else if (crrct == 0)
      total_punt++;
    else if (crrct == -1)
      total_error++;
    size++;
    return crrct;
  }

  void classifier_meter::test(class_state *co, ubyte cd, state_idx *en) {
    intg crrct = this->correctp(co->output_class, cd);
    age = 0;
    energy = en->x.get();
    confidence = co->confidence;
    total_energy = energy;
    total_correct = 0;
    total_punt = 0;
    total_error = 0;
    if (crrct == 1)
      total_correct = 1;
    else if (crrct == 0)
      total_punt = 1;
    else if (crrct == -1)
      total_error = 1;
    size = 1;
  }

  void classifier_meter::info() {
    /*
      (list
      age
      size
      (/ total-energy size)
      (/ (* 100 total-correct) size)
      (/ (* 100 total-error) size)
      (/ (* 100 total-punt) size)))
    */
    err_not_implemented();
  }

  void classifier_meter::info_sprint() {
    err_not_implemented();
  }

  void classifier_meter::info_print() {
    err_not_implemented();
  }

  void classifier_meter::display() {
    printf("[%5d]  size=%3d  energy=%g  correct=%3.2f%%  errors=%3.2f%%  \
rejects=%3.2f%%\n",
	   (int) age, (int) size,
	   total_energy / (double) size,
	   (total_correct * 100) / (double) size,
	   (total_error * 100) / (double) size,
	   (total_punt * 100) / (double) size);
  }

  bool classifier_meter::save() {
    err_not_implemented();
    return false;
  }

  bool classifier_meter::load() {
    err_not_implemented();
    return false;
  }

  ////////////////////////////////////////////////////////////////////////

  class_state::class_state(ubyte n) {
    sorted_classes = new Idx<ubyte>(n);
    sorted_scores = new Idx<float>(n);
  }

  class_state::~class_state() {
    delete sorted_classes;
    delete sorted_scores;
  }

  void class_state::resize(ubyte n) {
    sorted_classes->resize(n);
    sorted_scores->resize(n);
  }

  ////////////////////////////////////////////////////////////////////////

  max_classer::max_classer(Idx<ubyte> *classes) {
    classindex2label = classes;
  }

  void max_classer::fprop(state_idx *in, class_state *out) {
    intg n = in->x.dim(0);
    out->resize(n);
    { idx_bloop2(sc, *(out->sorted_scores), float, insc, in->x, double) {
	sc.set(idx_max(insc));
      }
    }
    idx_copy(*classindex2label, *(out->sorted_classes));
    idx_sortdown(*(out->sorted_scores), *(out->sorted_classes));
    out->output_class = out->sorted_classes->get(0);
    out->confidence = out->sorted_scores->get(0);
  }

  ////////////////////////////////////////////////////////////////////////

  idx3_supervised_module::idx3_supervised_module(nn_machine_cscscf *m, 
						 edist_cost *c,
						 max_classer *cl) {
    mout = new state_idx(1, 1, 1);
    machine = m;
    cost = c;
    classifier = cl;
  }

  idx3_supervised_module::~idx3_supervised_module() {
    delete mout;
  }

  void idx3_supervised_module::fprop(state_idx *input, class_state *output,
				     Idx<ubyte> *desired, state_idx *energy) {
    machine->fprop(*input, *mout);
    classifier->fprop(mout, output);
    cost->fprop(mout, desired, energy);
  }

  void idx3_supervised_module::use(state_idx *input, class_state *output) {
    machine->fprop(*input, *mout);
    classifier->fprop(mout, output);
  }

  void idx3_supervised_module::bprop(state_idx *input, class_state *output,
				     Idx<ubyte> *desired, state_idx *energy) {
    cost->bprop(mout, desired, energy);
    machine->bprop(*input, *mout);
  }

  void idx3_supervised_module::bbprop(state_idx *input, class_state *output,
				      Idx<ubyte> *desired, state_idx *energy) {
    cost->bbprop(mout, desired, energy);
    machine->bbprop(*input, *mout);
  }

} // end namespace ebl
