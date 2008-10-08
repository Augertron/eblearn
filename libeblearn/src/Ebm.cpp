/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun   *
 *   yann@cs.nyu.edu   *
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

#include "Ebm.h"
#include "IdxIO.h"

using namespace std;

namespace ebl {

void err_not_implemented()
{
	ylerror("member function not implemented for this class");
}

gd_param::gd_param(double leta, double ln, double l1, double l2, int dtime,
		double iner, double a_v, double a_t, double g_t)
{
	eta = leta;
	n = ln;
	decay_time = dtime;
	decay_l2 = l2;
	decay_l1 = l1;
	inertia = iner;
	anneal_value = a_v;
	anneal_time = a_t;
	gradient_threshold = g_t;
	niter_done = 0;
}

////////////////////////////////////////////////////////////////

forget_param_linear::forget_param_linear(double v, double e)
{
	value = v;
	exponent = e;
}

////////////////////////////////////////////////////////////////

void state::clear()
{
}
void state::clear_dx()
{
}
void state::clear_ddx()
{
}
void state::update_gd(gd_param &arg)
{
}

state::state()
{
	clear();
	clear_dx();
	clear_ddx();
}

state::~state()
{
}

////////////////////////////////////////////////////////////////
// state_idx

void state_idx::clear()
{
	idx_clear(x);
}
void state_idx::clear_dx()
{
	idx_clear(dx);
}
void state_idx::clear_ddx()
{
	idx_clear(ddx);
}

state_idx::state_idx() :
	x(), dx(), ddx()
{
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::state_idx(intg s0) :
	x(s0), dx(s0), ddx(s0)
{
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::state_idx(intg s0, intg s1) :
	x(s0, s1), dx(s0, s1), ddx(s0, s1)
{
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::state_idx(intg s0, intg s1, intg s2) :
	x(s0, s1, s2), dx(s0, s1, s2), ddx(s0, s1, s2)
{
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::state_idx(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
		intg s6, intg s7) :
	x(s0, s1, s2, s3, s4, s5, s6, s7), dx(s0, s1, s2, s3, s4, s5, s6, s7), ddx(
			s0, s1, s2, s3, s4, s5, s6, s7)
{
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::state_idx(parameter *st) :
	x(st->x.getstorage(), st->x.footprint()), dx(st->dx.getstorage(),
			st->dx.footprint()), ddx(st->ddx.getstorage(), st->ddx.footprint())
{
	st->resize(st->footprint() + nelements());
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::state_idx(parameter *st, intg s0) :
	x(st->x.getstorage(), st->x.footprint(), s0), dx(st->dx.getstorage(),
			st->dx.footprint(), s0), ddx(st->ddx.getstorage(),
			st->ddx.footprint(), s0)
{
	st->resize(st->footprint() + nelements());
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::state_idx(parameter *st, intg s0, intg s1) :
	x(st->x.getstorage(), st->x.footprint(), s0, s1), dx(st->dx.getstorage(),
			st->dx.footprint(), s0, s1), ddx(st->ddx.getstorage(),
			st->ddx.footprint(), s0, s1)
{
	st->resize(st->footprint() + nelements());
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::state_idx(parameter *st, intg s0, intg s1, intg s2) :
	x(st->x.getstorage(), st->x.footprint(), s0, s1, s2), dx(
			st->dx.getstorage(), st->dx.footprint(), s0, s1, s2), ddx(
			st->ddx.getstorage(), st->ddx.footprint(), s0, s1, s2)
{
	st->resize(st->footprint() + nelements());
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::state_idx(parameter *st, intg s0, intg s1, intg s2, intg s3,
		intg s4, intg s5, intg s6, intg s7) :
	x(st->x.getstorage(), st->x.footprint(), s0, s1, s2, s3, s4, s5, s6, s7),
			dx(st->dx.getstorage(), st->dx.footprint(), s0, s1, s2, s3, s4, s5,
					s6, s7), ddx(st->ddx.getstorage(), st->ddx.footprint(), s0,
					s1, s2, s3, s4, s5, s6, s7)
{
	st->resize(st->footprint() + nelements());
	clear();
	clear_dx();
	clear_ddx();
}

state_idx::~state_idx()
{
}

intg state_idx::nelements()
{
	return x.nelements();
}
intg state_idx::footprint()
{
	return x.footprint();
}
intg state_idx::size()
{
	return x.footprint();
}

void state_idx::resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
		intg s6, intg s7)
{
	x.resize(s0, s1, s2, s3, s4, s5, s6, s7);
	dx.resize(s0, s1, s2, s3, s4, s5, s6, s7);
	ddx.resize(s0, s1, s2, s3, s4, s5, s6, s7);
}

void state_idx::resizeAs(state_idx& s)
{
	if (x.order() != s.x.order())
		ylerror("State_Idx::resizeAs accepts states with same number of dimensions");
	intg dims[8] ={-1,-1,-1,-1,-1,-1,-1,-1};
	for (int i=0; i<x.order(); i++){
		dims[i] = s.x.dim(i);
	}
	resize(dims[0],dims[1],dims[2],dims[3],dims[4],dims[5],dims[6],dims[7]);
}

void state_idx::resize(const intg* dimsBegin, const intg* dimsEnd)
{
	x.resize(dimsBegin, dimsEnd);
	dx.resize(dimsBegin, dimsEnd);
	ddx.resize(dimsBegin, dimsEnd);
}

void state_idx::update_gd(gd_param &arg)
{
	if (arg.decay_l2 > 0)
	{
		idx_dotcacc(x, arg.decay_l2, dx);
	}
	if (arg.decay_l1 > 0)
	{
		idx_signdotcacc(x, arg.decay_l1, dx);
	}
	idx_dotcacc(dx, -arg.eta, x);
}

state_idx state_idx::make_copy() {
	intg dims[8] ={-1,-1,-1,-1,-1,-1,-1,-1};
	for (int i=0; i<x.order(); i++){
		dims[i] = x.dim(i);
	}
	state_idx other(dims[0],dims[1],dims[2],dims[3],dims[4],dims[5],dims[6],dims[7]);
	idx_copy(x,other.x);
	idx_copy(dx,other.dx);
	idx_copy(ddx,other.ddx);
	return other;
}

////////////////////////////////////////////////////////////////
// parameter

// TODO-0: BUG: a parameter object casted in state_idx* and called
// with resize(n) calls state_idx::resize instead of parameter::resize
// a temporary unclean solution is to use the same parameters as
// in state_idx::resize in parameter::resize:
// resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
//		intg s6, intg s7);
void parameter::resize(intg s0)
{
	x.resize(s0);
	dx.resize(s0);
	ddx.resize(s0);
	gradient.resize(s0);
	deltax.resize(s0);
	epsilons.resize(s0);
	ddeltax.resize(s0);
}

parameter::parameter(intg initial_size) :
	state_idx(initial_size), gradient(initial_size), deltax(initial_size),
	epsilons(initial_size), ddeltax(initial_size)
{
	idx_clear(x);
	idx_clear(dx);
	idx_clear(ddx);
	idx_clear(gradient);
	idx_clear(deltax);
	idx_clear(epsilons);
	idx_clear(ddeltax);
	resize(0);
}

parameter::~parameter()
{
}

bool parameter::load(const char *s) {
/*	Idx<double> m(1, 1), tmp(1);
	if (!load_matrix(m, s))
		return false;
	this->resize(m.dim(1));
	tmp = m.select(0, 0); idx_copy(tmp, x);
	tmp = m.select(0, 1); idx_copy(tmp, dx);
	tmp = m.select(0, 2); idx_copy(tmp, ddx);
	tmp = m.select(0, 3); idx_copy(tmp, gradient);
	tmp = m.select(0, 4); idx_copy(tmp, deltax);
	tmp = m.select(0, 5); idx_copy(tmp, epsilons);
	tmp = m.select(0, 6); idx_copy(tmp, ddeltax);
	return true;
	*/
	Idx<double> m(1);
	if (!load_matrix(m, s))
	  return false;
	this->resize(m.dim(0));
	idx_copy(m, x);
	return true;
}

void parameter::update_gd(gd_param &arg)
{
	if (arg.decay_l2 > 0)
	{
		idx_dotcacc(x, arg.decay_l2, dx);
	}
	if (arg.decay_l1 > 0)
	{
		idx_signdotcacc(x, arg.decay_l1, dx);
	}
	if (arg.inertia == 0)
	{
		idx_mul(dx, epsilons, dx);
		idx_dotcacc(dx, -arg.eta, x);
	}
	else
	{
		update_deltax( 1-arg.inertia, arg.inertia);
		idx_mul(deltax, epsilons, deltax);
		idx_dotcacc(deltax, -arg.eta, x);
	}
}

void parameter::update(gd_param &arg)
{
	update_gd(arg);
}

void parameter::clear_deltax()
{
	idx_clear(deltax);
}

void parameter::update_deltax(double knew, double kold)
{
	idx_lincomb(dx, knew, deltax, kold, deltax);
}

void parameter::clear_ddeltax()
{
	idx_clear(ddeltax);
}

void parameter::update_ddeltax(double knew, double kold)
{
	idx_lincomb(ddx, knew, ddeltax, kold, ddeltax);
}

void parameter::set_epsilons(double m)
{
	idx_fill(epsilons, m);
}

void parameter::compute_epsilons(double mu)
{
	idx_addc(ddeltax, mu, epsilons);
	idx_inv(epsilons, epsilons);
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
	out->resize(w->x.dim(0));
	idx_m2dotm1(w->x, in->x, out->x);
}

void linear_module::bprop(state_idx *in, state_idx *out)
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
			idx_m1extm1(out->dx, in->x, w->dx);
			idx_m2dotm1(twx, out->dx, in->dx);
		}
	}
}

void linear_module::bbprop(state_idx *in, state_idx *out)
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
			idx_m1squextm1(out->ddx, in->x, w->ddx);
			idx_m2squdotm1(twx, out->ddx, in->ddx);
		}
	}
}

void linear_module::forget(forget_param_linear &fp)
{
	double fanin = w->x.dim(1);
	double z = fp.value / pow(fanin, fp.exponent);
	if(!drand_ini) printf("You have not initialized random sequence. Please call init_drand() before using this function !\n");
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
	linear = new linear_module(p,ninputs,noutputs);
	bias = new state_idx(p,noutputs);
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
	out->resize(bias->x.dim(0));
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

////////////////////////////////////////////////////////////////

f_layer::f_layer(parameter *p, intg tin, intg tout, intg si, intg sj,
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
	if(!drand_ini) printf("You have not initialized random sequence. Please call init_drand() before using this function !\n");
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
				// fprintf(stdout,"f_layer::fprop\n");
				// weight->x.pretty(stdout);
				// weight->x.fdump(stdout);
				// llinx.pretty(stdout);
				// llinx.fdump(stdout);
				// lloutx.pretty(stdout);
				// multiply weight matrix by input
				idx_m2dotm1(weight->x, llinx, lloutx);
				// lloutx.pretty(stdout);
				// lloutx.fdump(stdout);
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
	squash->fprop(sum, out);
}

void f_layer::bprop(state_idx *in, state_idx *out)
{
	// backprop through squasher
	squash->bprop(sum, out);
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
		{ idx_bloop3(llinx,linx,double, llindx,lindx,double, lloutdx,loutdx,double) {
			idx_m1extm1acc(lloutdx,llinx,weight->dx);
			idx_m2dotm1(tkerx,lloutdx,llindx);
			// direct call because idxm2dotm1 didn't use to work on tranposed matrices
			// cblas_dgemv(CblasRowMajor, CblasTrans, weight->x.dim(0), weight->x.dim(1),
			//             1.0, weight->x.idx_ptr(), weight->x.mod(0),
			//             lloutdx.idx_ptr(), lloutdx.mod(0),
			//             0.0, llindx.idx_ptr(), llindx.mod(0));
		}}
	}}
}

void f_layer::bbprop(state_idx *in, state_idx *out)
{
	// backprop through squasher
	squash->bbprop(sum, out);
	// compute gradient of bias
	{	idx_bloop2(lha, sum->ddx, double, lb, bias->ddx, double) {
		idx_sumacc(lha, lb); }}
	// backprop through weight matrix
	int tr[] = { 2, 1, 0 };
	Idx<double> inx(in->x.transpose(tr));
	Idx<double> indx(in->ddx.transpose(tr));
	Idx<double> outdx(sum->ddx.transpose(tr));
	Idx<double> tkerx(weight->x.transpose(1, 0));
	idx_bloop3(linx,inx,double, lindx,indx,double, loutdx,outdx,double)
	{
		idx_bloop3(llinx,linx,double, llindx,lindx,double, lloutdx,loutdx,double)
		{
			idx_m1squextm1acc(lloutdx,llinx,weight->ddx);
			idx_m2squdotm1(tkerx,lloutdx,llindx);
		}
	}
}

////////////////////////////////////////////////////////////////

c_layer::c_layer(parameter *p, intg ki, intg kj, intg ri, intg rj, Idx<intg> *tbl,
		intg thick, intg si, intg sj, module_1_1<state_idx,state_idx> *sqsh)
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
	if(!drand_ini) printf("You have not initialized random sequence. Please call init_drand() before using this function !\n");
  idx_clear(fanin);
	{ idx_bloop1(tab, *table, intg)	{
			fanin.set(1 + fanin.get(tab.get(1)), tab.get(1)); }}
	{ idx_bloop2(tab, *table, intg, x, kx, double) {
		double s = fp.value / pow((vsize * hsize * fanin.get(tab.get(1))), fp.exponent);
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

	if (((sini - (ki - stridei)) % stridei != 0) || ((sinj - (kj - stridej))
			% stridej != 0))ylerror("inconsistent input size, kernel size, and subsampling ratio.");
	if ((stridei != 1) || (stridej != 1))ylerror("stride > 1 not implemented yet.");
	Idx<double> uuin(in->x.unfold(1, ki, stridei));
	uuin = uuin.unfold(2, kj, stridej);
	Idx<double> lki(kernel->x.dim(1), kernel->x.dim(2));
	// resize output if necessary
	sum->resize(thickness, uuin.dim(1), uuin.dim(2));
	out->resize(thickness, uuin.dim(1), uuin.dim(2));
	idx_clear(sum->x);
	// generic convolution
	{	idx_bloop2(lk, kernel->x, double, lt, *table, intg)	{
		Idx<double> suin(uuin.select(0, lt->get(0)));
		Idx<double> sout((sum->x).select(0, lt->get(1)));
		idx_m4dotm2acc(suin, lk, sout);
	}}
	// add bias
	{	idx_bloop3(sumx, sum->x, double, biasx, bias->x, double,
			outx, out->x, double)	{
		idx_addc(sumx, biasx.get(), sumx);
	}}
	// call squashing function
	squash->fprop(sum, out);
}

void c_layer::bprop(state_idx *in, state_idx *out)
{
	// backprop gradient through squasher
	squash->bprop(sum, out);
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
	{ idx_bloop3 (lk, kernel->dx, double, lkf, kernel->x, double, lt, *table, intg) {
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
	squash->bbprop(sum, out);
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
	{	idx_bloop3 (lk, kernel->ddx, double, lkf, kernel->x, double, lt, *table, intg) {
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
  squash->fprop(sum, out);
}

void c_layer_ipp::bprop (state_idx *in, state_idx *out) {
  // backprop gradient through squasher
  squash->bprop(sum, out);
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

////////////////////////////////////////////////////////////////

Idx<intg> full_table(intg a, intg b) {
  Idx<intg> m(a * b, 2);
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

////////////////////////////////////////////////////////////////////////

s_layer::s_layer(parameter *p, intg ki, intg kj, intg thick, intg si, intg sj,
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
	// 2. add bias
	{ idx_bloop3(sumx, sum->x, double, biasx, bias->x, double,
			outx, out->x, double) {
		idx_addc(sumx, biasx.get(), sumx);
	 }
	}
	// 3. call squashing function
	squash->fprop(sum, out);
}

void s_layer::bprop(state_idx *in, state_idx *out)
{
	// 1.
	squash->bprop(sum, out);
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
	squash->bbprop(sum, out);
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
			      if (m0->get() < mini)
			        mini = m0->get();
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
			outx->set(mini - log(r));
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
			      *m0 = ed0.get() * o->get() / sx->get();
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
  	idx_fill(*i, o->get());
    }
  }
}

////////////////////////////////////////////////////////////////////////

edist_cost::edist_cost(Idx<ubyte> *classes, intg ini, intg inj, Idx<double> *p) {
  intg imax = idx_max(*classes);
	intg imin = idx_min(*classes);
  if (imin < 0) ylerror("labels must be positive");
  if (imax > 100000) printf("warning: [edist-cost] largest label is huuuge\n");
  label2classindex = Idx<ubyte>(1 + imax);
  { idx_bloop1(v, label2classindex, ubyte) {
  	v->set(0);
    }
  }
  for (intg i = 0; i < classes->dim(0); ++i)
  	label2classindex.set(i, classes->get(i));
  dist = new state_idx(1, ini, inj);
  logadder = new logadd_layer(1, ini, inj);
  logadded_dist = new state_idx(1);
  proto = p;
}

void edist_cost::fprop(state_idx *in, Idx<ubyte> *desired, state_idx *energy) {
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
	     idx_sqrdist(p, *inx0, *dx0);
  	  }
  	 }
    }
   }
   idx_dotc(distx, 0.5, distx);
   logadder->fprop(dist, logadded_dist);
   energy->x.set(logadded_dist->x.get(0));
}

void edist_cost::bprop(state_idx *in, Idx<ubyte> *desired, state_idx *energy) {
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
	 { idx_bloop3(linx, tinx, double, lindx, tindx, double, ldistdx, distdx, double) {
	     { idx_bloop3(llinx, linx, double, llindx, lindx, double, lldistdx, ldistdx, double) {
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
void edist_cost::bbprop(state_idx *in, Idx<ubyte> *desired, state_idx *energy) {
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

char classifier_meter::update(intg a, class_state *co, ubyte cd, state_idx *en) {
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
	printf("[%5d]  size=%3d  energy=%g  correct=%3.2f%%  errors=%3.2f%%  rejects=%3.2f%%\n",
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
			case 4: out->resize(in->x.dim(0), in->x.dim(1), in->x.dim(2), in->x.dim(3));
				break;
			case 5: out->resize(in->x.dim(0), in->x.dim(1), in->x.dim(2), in->x.dim(3), in->x.dim(4));
				break;
			case 6: out->resize(in->x.dim(0), in->x.dim(1), in->x.dim(2), in->x.dim(3), in->x.dim(4), in->x.dim(5));
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
	Idx<double> jac_fprop(ndim_in, ndim_out); // used to store the jacobian calculated via bprop
	Idx<double> jac_bprop(ndim_in, ndim_out); //  used to store the jacobian calculated via prturbations

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

	Idx<double> bbprop_p(in->x.dim(0), in->x.dim(1), in->x.dim(2)); // used to store the bbprop calculated via perturbation

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
				// here we calculate a in aX²+bX+c as a model for the 3 points calculated via
				// fprop(...), fprop(...+small) and fprop(...-small). the second derivative is
				// then 2*a
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

	Idx<double> bprop_p(in->x.dim(0), in->x.dim(1), in->x.dim(2)); // used to store the bbprop calculated via perturbation

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
