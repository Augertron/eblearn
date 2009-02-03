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

#include "EblStates.h"
#include "IdxIO.h"

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
    x(s0, s1, s2, s3, s4, s5, s6, s7), dx(s0, s1, s2, s3, s4, s5, s6, s7), 
    ddx(s0, s1, s2, s3, s4, s5, s6, s7)
  {
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(state_idx *si) :
    x(si->x.getstorage(), 0, si->x.nelements()), 
    dx(si->dx.getstorage(), 0, si->dx.nelements()), 
    ddx(si->ddx.getstorage(), 0, si->ddx.nelements())
  {
  }

  state_idx::state_idx(parameter *st) :
    x(st->x.getstorage(), st->x.footprint()), 
    dx(st->dx.getstorage(), st->dx.footprint()), 
    ddx(st->ddx.getstorage(), st->ddx.footprint())
  {
    st->resize(st->footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(parameter *st, intg s0) :
    x(st->x.getstorage(), st->x.footprint(), s0), 
    dx(st->dx.getstorage(), st->dx.footprint(), s0), 
    ddx(st->ddx.getstorage(), st->ddx.footprint(), s0)
  {
    st->resize(st->footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(parameter *st, intg s0, intg s1) :
    x(st->x.getstorage(), st->x.footprint(), s0, s1), 
    dx(st->dx.getstorage(), st->dx.footprint(), s0, s1), 
    ddx(st->ddx.getstorage(), st->ddx.footprint(), s0, s1)
  {
    st->resize(st->footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(parameter *st, intg s0, intg s1, intg s2) :
    x(st->x.getstorage(), st->x.footprint(), s0, s1, s2), 
    dx(st->dx.getstorage(), st->dx.footprint(), s0, s1, s2), 
    ddx(st->ddx.getstorage(), st->ddx.footprint(), s0, s1, s2)
  {
    st->resize(st->footprint() + nelements());
    clear();
    clear_dx();
    clear_ddx();
  }

  state_idx::state_idx(parameter *st, intg s0, intg s1, intg s2, intg s3,
		       intg s4, intg s5, intg s6, intg s7) :
    x(st->x.getstorage(), st->x.footprint(), s0, s1, s2, s3, s4, s5, s6, s7),
    dx(st->dx.getstorage(), st->dx.footprint(), s0, s1, s2, s3, s4, s5, s6,s7), 
    ddx(st->ddx.getstorage(), st->ddx.footprint(), 
	s0, s1, s2, s3, s4, s5, s6, s7)
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
      ylerror("State_Idx::resizeAs accepts states with same number of \
dimensions");
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
    state_idx other(dims[0],dims[1],dims[2],dims[3],dims[4],dims[5],dims[6],
		    dims[7]);
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

} // end namespace ebl
