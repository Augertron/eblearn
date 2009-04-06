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

#include "ebl_states.h"
#include "libidx.h"

using namespace std;

namespace ebl {

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
    if (e == 0)
      ylerror("Cannot use an exponent of 0 in forget param");
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

  state_idx::~state_idx()
  {
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a parameter

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
    x(s0, s1, s2, s3, s4, s5, s6, s7), dx(s0, s1, s2, s3, s4, s5, s6, s7), 
    ddx(s0, s1, s2, s3, s4, s5, s6, s7)
  {
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(const idxdim &d) : x(d), dx(d), ddx(d) {
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(idx<double> m) :
    x(m.getidxdim(dims)), dx(m.getidxdim(dims)), ddx(m.getidxdim(dims)) {
    clear();
    clear_dx();
    clear_ddx();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a parameter

  state_idx::state_idx(parameter &st) :
    x(st.x.getstorage(), st.x.footprint()), 
    dx(st.dx.getstorage(), st.dx.footprint()), 
    ddx(st.ddx.getstorage(), st.ddx.footprint())
  {
    st.resize(st.footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(parameter &st, intg s0) :
    x(st.x.getstorage(), st.x.footprint(), s0), 
    dx(st.dx.getstorage(), st.dx.footprint(), s0), 
    ddx(st.ddx.getstorage(), st.ddx.footprint(), s0)
  {
    st.resize(st.footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(parameter &st, intg s0, intg s1) :
    x(st.x.getstorage(), st.x.footprint(), s0, s1), 
    dx(st.dx.getstorage(), st.dx.footprint(), s0, s1), 
    ddx(st.ddx.getstorage(), st.ddx.footprint(), s0, s1)
  {
    st.resize(st.footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(parameter &st, intg s0, intg s1, intg s2) :
    x(st.x.getstorage(), st.x.footprint(), s0, s1, s2), 
    dx(st.dx.getstorage(), st.dx.footprint(), s0, s1, s2), 
    ddx(st.ddx.getstorage(), st.ddx.footprint(), s0, s1, s2)
  {
    st.resize(st.footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(parameter &st, intg s0, intg s1, intg s2, intg s3,
		       intg s4, intg s5, intg s6, intg s7) :
    x(st.x.getstorage(), st.x.footprint(), s0, s1, s2, s3, s4, s5, s6, s7),
    dx(st.dx.getstorage(), st.dx.footprint(), s0, s1, s2, s3, s4, s5, s6,s7), 
    ddx(st.ddx.getstorage(), st.ddx.footprint(), 
	s0, s1, s2, s3, s4, s5, s6, s7) {
    st.resize(st.footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(parameter &st, const idxdim &d) 
    : x(st.x.getstorage(), st.x.footprint(), d), 
      dx(st.x.getstorage(), st.x.footprint(), d),
      ddx(st.x.getstorage(), st.x.footprint(), d) {
    st.resize(st.footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from other state_idx's dimensions

  state_idx::state_idx(const idx<double> &_x, const idx<double> &_dx, 
		       const idx<double> &_ddx) :
    x(_x), dx(_dx), ddx(_ddx) {
  }

  ////////////////////////////////////////////////////////////////
  // clear methods

  void state_idx::clear() {
    idx_clear(x);
  }

  void state_idx::clear_dx() {
    idx_clear(dx);
  }

  void state_idx::clear_ddx() {
    idx_clear(ddx);
  }

  ////////////////////////////////////////////////////////////////
  // information methods

  intg state_idx::nelements() {
    return x.nelements();
  }

  intg state_idx::footprint() {
    return x.footprint();
  }

  intg state_idx::size() {
    return x.footprint();
  }

  ////////////////////////////////////////////////////////////////
  // resize methods

  void state_idx::resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
			 intg s6, intg s7) {
    if (!x.same_dim(s0, s1, s2, s3, s4, s5, s6, s7)) { // save some time
      x.resize(s0, s1, s2, s3, s4, s5, s6, s7);
      dx.resize(s0, s1, s2, s3, s4, s5, s6, s7);
      ddx.resize(s0, s1, s2, s3, s4, s5, s6, s7);
    }
  }

  void state_idx::resize(const idxdim &d) {
    if (!x.same_dim(d)) { // save some time if dimensions are the same
      x.resize(d);
      dx.resize(d);
      ddx.resize(d);
    }
  }

  void state_idx::resize1(intg dimn, intg size) {
    if (x.dim(dimn) != size) { // save some time if size is already set.
      x.resize1(dimn, size);
      dx.resize1(dimn, size);
      ddx.resize1(dimn, size);
    }
  }

  void state_idx::resize_as(state_idx& s) {
    idxdim d(s.x.spec); // use same dimensions as s
    resize(d);
  }

  void state_idx::resize_as_but1(state_idx& s, intg fixed_dim) {
    idxdim d(s.x.spec); // use same dimensions as s
    d.setdim(fixed_dim, x.dim(fixed_dim));
    resize(d);
  }

  void state_idx::resize(const intg* dimsBegin, const intg* dimsEnd) {
    x.resize(dimsBegin, dimsEnd);
    dx.resize(dimsBegin, dimsEnd);
    ddx.resize(dimsBegin, dimsEnd);
  }

  void state_idx::update_gd(gd_param &arg) {
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
    state_idx other(dims[0],dims[1],dims[2],dims[3],dims[4],dims[5],dims[6],
		    dims[7]);
    idx_copy(x,other.x);
    idx_copy(dx,other.dx);
    idx_copy(ddx,other.ddx);
    return other;
  }

  ////////////////////////////////////////////////////////////////
  // parameter

  parameter::parameter(intg initial_size) 
    : state_idx(initial_size), gradient(initial_size), deltax(initial_size),
      epsilons(initial_size), ddeltax(initial_size) {
    idx_clear(x);
    idx_clear(dx);
    idx_clear(ddx);
    idx_clear(gradient);
    idx_clear(deltax);
    idx_clear(epsilons);
    idx_clear(ddeltax);
    resize(0);
  }

  parameter::parameter(const char *param_filename) 
    : state_idx(1), gradient(1), deltax(1), epsilons(1), ddeltax(1) {
    if (!load_x(param_filename)) {
      cerr << "failed to open " << param_filename << endl;
      ylerror("failed to load parameter file in parameter constructor");
    }
  }

  parameter::~parameter() {
  }

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

  bool parameter::load_x(const char *s) {
    /*	idx<double> m(1, 1), tmp(1);
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
    idx<double> m(1);
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
  // state_idxlooper

  state_idxlooper::state_idxlooper(state_idx &s, int ld) 
    : state_idx(s.x.select(ld, 0), 
		s.dx.select(ld, 0), 
		s.ddx.select(ld, 0)),
      lx(s.x, ld), ldx(s.dx, ld), lddx(s.ddx, ld) {
  }
  
  state_idxlooper::~state_idxlooper() {
  }
  
  void state_idxlooper::next() {
    lx.next();
    ldx.next();
    lddx.next();
    x = lx;
    dx = ldx;
    ddx = lddx;
  }

  // return true when done.
  bool state_idxlooper::notdone() { 
    return lx.notdone(); 
  }

} // end namespace ebl
