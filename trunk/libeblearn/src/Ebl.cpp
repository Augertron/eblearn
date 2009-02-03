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

#include "Ebl.h"
#include "IdxIO.h"

using namespace std;

namespace ebl {

  void err_not_implemented()
  {
    ylerror("member function not implemented for this class");
  }

  ////////////////////////////////////////////////////////////////

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
    state_idx in1(in); // TODO: temporary, find a cleaner solution
    out->resize(w->x.dim(0));
    idx_m2dotm1(w->x, in1.x, out->x);
  }

  void linear_module::bprop(state_idx *in, state_idx *out)
  {
    state_idx in1(in); // TODO: temporary, find a cleaner solution
    intg instride = 0;
    intg outstride = 0;
    if (in1.x.order() == 1)
      {
	instride = in1.x.mod(0);
      }
    else if (in1.x.contiguousp())
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
	    idx_m1extm1(out->dx, in1.x, w->dx);
	    idx_m2dotm1(twx, out->dx, in1.dx);
	  }
      }
  }

  void linear_module::bbprop(state_idx *in, state_idx *out)
  {
    state_idx in1(in); // TODO: temporary, find a cleaner solution
    intg instride = 0;
    intg outstride = 0;
    if (in1.x.order() == 1)
      {
	instride = in1.x.mod(0);
      }
    else if (in1.x.contiguousp())
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
	    idx_m1squextm1(out->ddx, in1.x, w->ddx);
	    idx_m2squdotm1(twx, out->ddx, in1.ddx);
	  }
      }
  }

  void linear_module::forget(forget_param_linear &fp)
  {
    double fanin = w->x.dim(1);
    double z = fp.value / pow(fanin, fp.exponent);
    if(!drand_ini) printf("You have not initialized random sequence. \
Please call init_drand() before using this function !\n");
    idx_aloop1(lx,w->x,double)
      {	*lx = drand(z);}
  }

  void linear_module::normalize()
  {
    norm_columns(w->x);
  }

  ////////////////////////////////////////////////////////////////

  stdsigmoid_module::stdsigmoid_module()
  {
  }
  stdsigmoid_module::~stdsigmoid_module()
  {
  }

  // standard sigmoid module
  void stdsigmoid_module::fprop(state_idx *in, state_idx *out)
  {
    idx_stdsigmoid(in->x, out->x);
  }

  void stdsigmoid_module::bprop(state_idx *in, state_idx *out)
  {
    idx_dstdsigmoid(in->x, in->dx);
    idx_mul(in->dx, out->dx, in->dx);
  }

  void stdsigmoid_module::bbprop(state_idx *in, state_idx *out)
  {
    idx_dstdsigmoid(in->x, in->ddx);
    idx_mul(in->ddx, in->ddx, in->ddx);
    idx_mul(in->ddx, out->ddx, in->ddx);
  }

  ////////////////////////////////////////////////////////////////

  // tanh module
  void tanh_module::fprop(state_idx *in, state_idx *out)
  {
    idx_tanh(in->x, out->x);
  }

  void tanh_module::bprop(state_idx *in, state_idx *out)
  {
    idx_dtanh(in->x, in->dx);
    idx_mul(in->dx, out->dx, in->dx);
  }

  void tanh_module::bbprop(state_idx *in, state_idx *out)
  {
    idx_dtanh(in->x, in->ddx);
    idx_mul(in->ddx, in->ddx, in->ddx);
    idx_mul(in->ddx, out->ddx, in->ddx);
  }

  void tanh_module::forget(forget_param_linear& fp)
  {
  }

  void tanh_module::normalize()
  {
  }

  ////////////////////////////////////////////////////////////////

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
    out->resize(bias->x.dim(0));
    idx_add(in->x,bias->x,out->x);
  }

  void addc_module::bprop(state_idx* in, state_idx* out)
  {
    idx_copy(out->dx,in->dx);
    idx_copy(out->dx,bias->dx);
  }

  void addc_module::bbprop(state_idx* in, state_idx* out)
  {
    idx_copy(out->ddx,in->ddx);
    idx_copy(out->ddx,bias->ddx);
  }

  void addc_module::forget(forget_param_linear& fp)
  {
    idx_clear(bias->x);
  }

  void addc_module::normalize()
  {
  }
  ////////////////////////////////////////////////////////////////

  nn_layer_full::nn_layer_full(parameter *p, intg ninputs, intg noutputs)
  {
    linear = new linear_module(p, ninputs, noutputs);
    bias = new state_idx(p, noutputs);
    sum = new state_idx(noutputs);
    sigmoid = new tanh_module();
  }

  nn_layer_full::~nn_layer_full()
  {
    delete sigmoid;
    delete sum;
    delete bias;
    delete linear;
  }

  void nn_layer_full::fprop(state_idx *in, state_idx *out)
  {
    out->resize(bias->x.dim(0), out->x.dim(1), out->x.dim(2));
    linear->fprop(in, sum);
    idx_add(sum->x, bias->x, sum->x);
    sigmoid->fprop(sum, out);
  }

  void nn_layer_full::bprop(state_idx *in, state_idx *out)
  {
    sigmoid->bprop(sum, out);
    idx_copy(sum->dx, bias->dx);
    linear->bprop(in, sum);
  }

  void nn_layer_full::bbprop(state_idx *in, state_idx *out)
  {
    sigmoid->bbprop(sum, out);
    idx_copy(sum->ddx, bias->ddx);
    linear->bbprop(in, sum);
  }

  void nn_layer_full::forget(forget_param_linear &fp)
  {
    linear->forget(fp);
    idx_clear(bias->x);
  }

  ////////////////////////////////////////////////////////////////////////

  softmax::softmax(double b){
    beta = b;
  }

  void softmax::resize_nsame(state_idx *in, state_idx *out, int n){
    int nmax = in->x.order();
    if(n==0||n>nmax) {ylerror("illegal type")}
    else{
      switch(n){
      case 1: out->resize(in->x.dim(0));
	break;
      case 2: out->resize(in->x.dim(0), in->x.dim(1));
	break;
      case 3: out->resize(in->x.dim(0), in->x.dim(1), in->x.dim(2));
	break;
      case 4: out->resize(in->x.dim(0), in->x.dim(1), in->x.dim(2), 
			  in->x.dim(3));
	break;
      case 5: out->resize(in->x.dim(0), in->x.dim(1), in->x.dim(2), 
			  in->x.dim(3), in->x.dim(4));
	break;
      case 6: out->resize(in->x.dim(0), in->x.dim(1), in->x.dim(2), 
			  in->x.dim(3), in->x.dim(4), in->x.dim(5));
	break;
      }
    }
  }

  void softmax::fprop( state_idx *in, state_idx *out){
    int n=in->x.order();
    if(n==0){
      Idx<double> ib;
      ib.set(1);
      idx_copy(ib, out->x);
    }
    else {
      resize_nsame(in, out, n);
      if( n > 6) {ylerror("illegal type")}
      else{
	Idx<double> pp(new Srg<double>(), in->x.spec);
	Idx<double> dot(new Srg<double>(), in->x.spec);
	double mm = idx_max(in->x);
	idx_addc(in->x, -mm, pp);
	idx_dotc(pp, beta, dot);
	double out_sum = 0.0;
	double d = idx_sum(dot, &out_sum);
	idx_dotc(dot, (double)(1/d), out->x);
      }
    }
  }

  void softmax::bprop( state_idx *in, state_idx *out){
    int n = in->x.order();
    if( n == 0) return;
    if( n > 6 ) { ylerror("illegal type")}
    else{
      Idx<double> pp(new Srg<double>(), out->dx.spec);
      Idx<double> mul(new Srg<double>(), out->dx.spec);
      double dot = idx_dot(out->dx, out->x);
      idx_addc(out->dx, -dot, pp);
      idx_mul(out->x, pp, mul);
      idx_dotcacc(mul, beta, in->x);
    }
  }

  void softmax::bbprop( state_idx *in, state_idx *out){
    int n = in->x.order();
    if( n == 0) return;
    if( n > 6 ) { ylerror("illegal type")}
    else{
      Idx<double> mul(new Srg<double>(), out->x.spec);
      Idx<double> dot(new Srg<double>(), out->x.spec);
      Idx<double> pp(new Srg<double>(), out->x.spec);
      Idx<double> mul2(new Srg<double>(), out->x.spec);
      Idx<double> pp2(new Srg<double>(), out->x.spec);
      Idx<double> mul3(new Srg<double>(), out->x.spec);
      idx_mul(out->x, out->x, mul);
      idx_dotc(out->x, (double)-2, dot);
      idx_addc(dot, (double)1, pp);
      idx_mul(pp, out->ddx, mul2);
      idx_addc(mul2, idx_dot(out->ddx, mul), pp2);
      idx_mul(mul, pp2, mul3);

      idx_dotcacc(mul3, beta*beta, in->ddx);
    }
  }

  ////////////////////////////////////////////////////////////////////////

  void Jacobian_tester::test(module_1_1<state_idx, state_idx> *module){

    int insize = 16;
    state_idx *in = new state_idx(insize, 1, 1);
    state_idx *out = new state_idx(insize, 1, 1);

    //init
    dseed(2);  // 2 is chosen randomly... feel free to change it
    module->fprop(in, out); // used to resize the outputs
    { idx_bloop1( i, in->x, double)
	{ idx_bloop1 (ii, i, double)
	    { idx_bloop1( iii, ii, double)
		{ iii.set(drand(2)); }
	    }
	}
    }
    { idx_bloop1( o, out->x, double)
	{ idx_bloop1 (oo, o, double)
	    { idx_bloop1( ooo, oo, double)
		{ ooo.set(drand(2)); }
	    }
	}
    }


    // check the Jacobian
    int ndim_in = in->x.nelements();
    int ndim_out = in->x.nelements();
    // used to store the jacobian calculated via bprop
    Idx<double> jac_fprop(ndim_in, ndim_out); 
    //  used to store the jacobian calculated via prturbations
    Idx<double> jac_bprop(ndim_in, ndim_out); 

    // creation of jac_fprop
    module->fprop(in, out);
    int cnt = 0;
    { idx_bloop1(o, out->x, double)
	{ idx_bloop1(oo, o, double)
	    { idx_bloop1(ooo, oo, double)
		{
		  out->clear_dx();
		  in->clear_dx();
		  ooo.set(1);
		  module->bprop(in, out);
		  Idx<double> bla = jac_bprop.select(1, cnt);
		  idx_copy(in->dx, bla);
		  cnt++;
		}
	    }
	}
    }

    // creation of jac_bprop
    cnt = 0;
    double small = pow(10.0, -6);
    state_idx *in1 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *in2 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *out1 = new state_idx( 1, 1, 1);
    state_idx *out2 = new state_idx( 1, 1, 1);
    for(int d1 = 0; d1 < in->x.dim(0); d1++){
      for(int d2 = 0; d2 < in->x.dim(1); d2++){
	for(int d3 = 0; d3 < in->x.dim(2); d3++){
	  idx_copy(in->x, in1->x);
	  idx_copy(in->x, in2->x);
	  in1->x.set(in1->x.get( d1, d2, d3) + small, d1, d2, d3);
	  in2->x.set(in2->x.get( d1, d2, d3) - small, d1, d2, d3);
	  module->fprop(in1, out1);
	  module->fprop(in2, out2);
	  Idx<double> sub(new Srg<double>(), out1->x.spec);
	  Idx<double> dot(new Srg<double>(), out1->x.spec);
	  idx_sub(out1->x, out2->x, sub);
	  idx_dotc(sub, 0.5/small, dot);
	  Idx<double> bla2 = jac_fprop.select(0, cnt);
	  idx_copy(dot, bla2);
	  cnt++;
	}
      }
    }

    // comparison
    printf("Jacobian error: %8.7e \n", idx_sqrdist(jac_fprop, jac_bprop));
  }

  ////////////////////////////////////////////////////////////////////////

  void Bbprop_tester::test(module_1_1<state_idx, state_idx> *module){

    int insize = 16;
    state_idx *in = new state_idx(insize, 1, 1);
    state_idx *out = new state_idx(insize, 1, 1);

    //init
    dseed(2);  // 2 is chosen randomly... feel free to change it
    module->fprop(in, out); // used to resize the outputs
    { idx_bloop1( i, in->x, double)
	{ idx_bloop1 (ii, i, double)
	    { idx_bloop1( iii, ii, double)
		{ iii.set(drand(2)); }
	    }
	}
    }
    { idx_bloop1( o, out->x, double)
	{ idx_bloop1 (oo, o, double)
	    { idx_bloop1( ooo, oo, double)
		{ ooo.set(drand(2)); }
	    }
	}
    }

    module->fprop(in, out);
    module->bprop(in, out);
    module->bbprop(in, out);

    // used to store the bbprop calculated via perturbation
    Idx<double> bbprop_p(in->x.dim(0), in->x.dim(1), in->x.dim(2)); 

    // creation of bbprop_p
    int cnt = 0;
    double small = pow(10.0, -6);
    state_idx *in1 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *in2 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *out1 = new state_idx( 1, 1, 1);
    state_idx *out2 = new state_idx( 1, 1, 1);
    for(int d1 = 0; d1 < in->x.dim(0); d1++){
      for(int d2 = 0; d2 < in->x.dim(1); d2++){
	for(int d3 = 0; d3 < in->x.dim(2); d3++){
	  idx_copy(in->x, in1->x);
	  idx_copy(in->x, in2->x);
	  in1->x.set(in1->x.get( d1, d2, d3) + small, d1, d2, d3);
	  in2->x.set(in2->x.get( d1, d2, d3) - small, d1, d2, d3);
	  module->fprop(in1, out1);
	  module->fprop(in2, out2);
	  // here we calculate a in aX²+bX+c as a model for the 3 points 
	  // calculated via
	  // fprop(...), fprop(...+small) and fprop(...-small). the second 
	  // derivative is then 2*a
	  Idx<double> ad(new Srg<double>(), out1->x.spec);
	  Idx<double> sub(new Srg<double>(), out1->x.spec);
	  Idx<double> dot(new Srg<double>(), out1->x.spec);
	  Idx<double> dot2(new Srg<double>(), out1->x.spec);
	  idx_add(out1->x, out2->x, ad);
	  idx_dotc(out->x, (double)2, dot);
	  idx_sub(ad, dot, sub);
	  idx_dotc(sub, 1/small, dot2);
	  bbprop_p.set(dot2.get( d1, d2, d3), d1, d2, d3);
	  cnt++;
	}
      }
    }

    // comparison
    printf("bbprop error: %8.7e \n", idx_sqrdist(in->ddx, bbprop_p));
  }

  ////////////////////////////////////////////////////////////////////////

  void Bprop_tester::test(module_1_1<state_idx, state_idx> *module){

    int insize = 16;
    state_idx *in = new state_idx(insize, 1, 1);
    state_idx *out = new state_idx(insize, 1, 1);

    //init
    dseed(2);  // 2 is chosen randomly... feel free to change it
    module->fprop(in, out); // used to resize the outputs
    { idx_bloop1( i, in->x, double)
	{ idx_bloop1 (ii, i, double)
	    { idx_bloop1( iii, ii, double)
		{ iii.set(drand(2)); }
	    }
	}
    }
    { idx_bloop1( o, out->x, double)
	{ idx_bloop1 (oo, o, double)
	    { idx_bloop1( ooo, oo, double)
		{ ooo.set(drand(2)); }
	    }
	}
    }

    // used to store the bbprop calculated via perturbation
    Idx<double> bprop_p(in->x.dim(0), in->x.dim(1), in->x.dim(2)); 

    // creation of bprop_p
    int cnt = 0;
    double small = pow(10.0, -6);
    state_idx *in1 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *in2 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *out1 = new state_idx( 1, 1, 1);
    state_idx *out2 = new state_idx( 1, 1, 1);
    for(int d1 = 0; d1 < in->x.dim(0); d1++){
      for(int d2 = 0; d2 < in->x.dim(1); d2++){
	for(int d3 = 0; d3 < in->x.dim(2); d3++){
	  idx_copy(in->x, in1->x);
	  idx_copy(in->x, in2->x);
	  in1->x.set(in1->x.get( d1, d2, d3) + small, d1, d2, d3);
	  in2->x.set(in2->x.get( d1, d2, d3) - small, d1, d2, d3);
	  module->fprop(in1, out1);
	  module->fprop(in2, out2);

	  Idx<double> sub(new Srg<double>(), out1->x.spec);
	  Idx<double> dot(new Srg<double>(), out1->x.spec);
	  idx_sub(out1->x, out2->x, sub);
	  idx_dotc(sub, 0.5/small, dot);
	  bprop_p.set(dot.get( d1, d2, d3), d1, d2, d3);
	  cnt++;
	}
      }
    }

    printf("Bprop error : %8.7e \n", idx_sqrdist(in->dx, bprop_p));
  }

} // end namespace ebl
