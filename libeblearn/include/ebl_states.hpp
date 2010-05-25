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

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // state_idx

  template<class T>
  state_idx<T>::~state_idx() {
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a parameter

  template<class T>
  state_idx<T>::state_idx() :
    x(), dx(), ddx() {
    clear();
  }

  template<class T>
  state_idx<T>::state_idx(intg s0) :
    x(s0), dx(s0), ddx(s0) {
    clear();
  }

  template<class T>
  state_idx<T>::state_idx(intg s0, intg s1) :
    x(s0, s1), dx(s0, s1), ddx(s0, s1)  {
    clear();
  }

  template<class T>
  state_idx<T>::state_idx(intg s0, intg s1, intg s2) :
    x(s0, s1, s2), dx(s0, s1, s2), ddx(s0, s1, s2) {
    clear();
  }

  template<class T>
  state_idx<T>::state_idx(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
		       intg s6, intg s7) :
    x(s0, s1, s2, s3, s4, s5, s6, s7), dx(s0, s1, s2, s3, s4, s5, s6, s7), 
    ddx(s0, s1, s2, s3, s4, s5, s6, s7) {
    clear();
  }

  template<class T>
  state_idx<T>::state_idx(const idxdim &d) : x(d), dx(d), ddx(d) {
    clear();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a parameter

  template<class T>
  state_idx<T>::state_idx(parameter<T> *st) :
    x(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0), 
    dx(st ? st->dx.getstorage() : NULL, st ? st->dx.footprint() : 0), 
    ddx(st ? st->ddx.getstorage() : NULL, st ? st->ddx.footprint() : 0) {
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<class T>
  state_idx<T>::state_idx(parameter<T> *st, intg s0) :
    x(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, s0), 
    dx(st ? st->dx.getstorage() : NULL, st ? st->dx.footprint() : 0, s0), 
    ddx(st ? st->ddx.getstorage() : NULL, st ? st->ddx.footprint() : 0, s0) {
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<class T>
  state_idx<T>::state_idx(parameter<T> *st, intg s0, intg s1) :
    x(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, s0, s1), 
    dx(st ? st->dx.getstorage() : NULL, st ? st->dx.footprint() : 0, s0, s1), 
    ddx(st ? st->ddx.getstorage() : NULL,
	st ? st->ddx.footprint() : 0, s0, s1) {
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<class T>
  state_idx<T>::state_idx(parameter<T> *st, intg s0, intg s1, intg s2) :
    x(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, s0, s1, s2), 
    dx(st ? st->dx.getstorage() : NULL,
       st ? st->dx.footprint() : 0, s0, s1, s2), 
    ddx(st ? st->ddx.getstorage() : NULL,
	st ? st->ddx.footprint() : 0, s0, s1, s2) {
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }
  
  template<class T>
  state_idx<T>::state_idx(parameter<T> *st, intg s0, intg s1, intg s2, intg s3,
		       intg s4, intg s5, intg s6, intg s7) :
    x(st ? st->x.getstorage() : NULL,
      st ? st->x.footprint() : 0, s0, s1, s2, s3, s4, s5, s6, s7),
    dx(st ? st->dx.getstorage() : NULL,
       st ? st->dx.footprint : 0, s0, s1, s2, s3, s4, s5, s6,s7), 
    ddx(st ? st->ddx.getstorage() : NULL, st ? st->ddx.footprint() : 0, 
	s0, s1, s2, s3, s4, s5, s6, s7) {
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<class T>
  state_idx<T>::state_idx(parameter<T> *st, const idxdim &d) 
    : x(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, d), 
      dx(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, d),
      ddx(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, d) {
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from other state_idx's dimensions

  template<class T>
  state_idx<T>::state_idx(const idx<T> &_x, const idx<T> &_dx, 
		       const idx<T> &_ddx) :
    x(_x), dx(_dx), ddx(_ddx) {
  }

  ////////////////////////////////////////////////////////////////
  // clear methods

  template <class T> void state_idx<T>::clear() {
    idx_clear(x);
    idx_clear(dx);
    idx_clear(ddx);
  }

  template <class T> void state_idx<T>::clear_x() {
    idx_clear(x);
  }

  template <class T> void state_idx<T>::clear_dx() {
    idx_clear(dx);
  }

  template <class T> void state_idx<T>::clear_ddx() {
    idx_clear(ddx);
  }

  ////////////////////////////////////////////////////////////////
  // information methods

  template <class T> intg state_idx<T>::nelements() {
    return x.nelements();
  }

  template <class T> intg state_idx<T>::footprint() {
    return x.footprint();
  }

  template <class T> intg state_idx<T>::size() {
    return x.footprint();
  }

  ////////////////////////////////////////////////////////////////
  // resize methods

  template <class T>
  void state_idx<T>::resize(intg s0, intg s1, intg s2,
			    intg s3, intg s4, intg s5,
			    intg s6, intg s7) {
    if (!x.same_dim(s0, s1, s2, s3, s4, s5, s6, s7)) { // save some time
      x.resize(s0, s1, s2, s3, s4, s5, s6, s7);
      dx.resize(s0, s1, s2, s3, s4, s5, s6, s7);
      ddx.resize(s0, s1, s2, s3, s4, s5, s6, s7);
    }
  }

  template <class T>
  void state_idx<T>::resize(const idxdim &d) {
    if (!x.same_dim(d)) { // save some time if dimensions are the same
      x.resize(d);
      dx.resize(d);
      ddx.resize(d);
    }
  }

  template <class T>
  void state_idx<T>::resize1(intg dimn, intg size) {
    if (x.dim(dimn) != size) { // save some time if size is already set.
      x.resize1(dimn, size);
      dx.resize1(dimn, size);
      ddx.resize1(dimn, size);
    }
  }

  template <class T>
  void state_idx<T>::resize_as(state_idx<T>& s) {
    idxdim d(s.x.spec); // use same dimensions as s
    resize(d);
  }

  template <class T>
  void state_idx<T>::resize_as_but1(state_idx<T>& s, intg fixed_dim) {
    idxdim d(s.x.spec); // use same dimensions as s
    d.setdim(fixed_dim, x.dim(fixed_dim));
    resize(d);
  }

  template <class T>
  void state_idx<T>::resize(const intg* dimsBegin, const intg* dimsEnd) {
    x.resize(dimsBegin, dimsEnd);
    dx.resize(dimsBegin, dimsEnd);
    ddx.resize(dimsBegin, dimsEnd);
  }

  template <class T>
  void state_idx<T>::update_gd(gd_param &arg) {
    if (arg.decay_l2 > 0) {
      idx_dotcacc(x, arg.decay_l2, dx);
    }
    if (arg.decay_l1 > 0) {
      idx_signdotcacc(x, (T) arg.decay_l1, dx);
    }
    idx_dotcacc(dx, -arg.eta, x);
  }

  template <class T>
  state_idx<T> state_idx<T>::make_copy() {
    intg dims[8] ={-1,-1,-1,-1,-1,-1,-1,-1};
    for (int i=0; i<x.order(); i++){
      dims[i] = x.dim(i);
    }
    state_idx<T> other(dims[0],dims[1],dims[2],dims[3],dims[4],dims[5],dims[6],
		    dims[7]);
    idx_copy(x,other.x);
    idx_copy(dx,other.dx);
    idx_copy(ddx,other.ddx);
    return other;
  }

  ////////////////////////////////////////////////////////////////
  // parameter

  template <class T>
  parameter<T>::parameter(intg initial_size) 
    : state_idx<T>(initial_size), gradient(initial_size), deltax(initial_size),
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

  template <class T>
  parameter<T>::parameter(const char *param_filename) 
    : state_idx<T>(1), gradient(1), deltax(1), epsilons(1), ddeltax(1) {
    if (!load_x(param_filename)) {
      cerr << "failed to open " << param_filename << endl;
      eblerror("failed to load parameter file in parameter constructor");
    }
  }

  template <class T>
  parameter<T>::~parameter() {
  }

  // TODO-0: BUG: a parameter object casted in state_idx* and called
  // with resize(n) calls state_idx::resize instead of parameter<T>::resize
  // a temporary unclean solution is to use the same parameters as
  // in state_idx::resize in parameter<T>::resize:
  // resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
  //		intg s6, intg s7);
  template <class T>
  void parameter<T>::resize(intg s0) {
    x.resize(s0);
    dx.resize(s0);
    ddx.resize(s0);
    gradient.resize(s0);
    deltax.resize(s0);
    epsilons.resize(s0);
    ddeltax.resize(s0);
  }

  template <class T>
  bool parameter<T>::save_x(const char *s) {
    if (!save_matrix(x, s))
      return false;
    return true;
  }

  template <class T>
  bool parameter<T>::load_x(const char *s) {
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
    try {
      idx<T> m = load_matrix<T>(s);
      this->resize(m.dim(0));
      idx_copy(m, x);
      cout << "Loaded weights from " << s << ": " << x << endl;
      return true;
    } catch(string &err) {
      cout << err << endl;
      eblerror("failed to load weights");
    }
    return false;
  }

  template <class T>
  void parameter<T>::update_gd(gd_param &arg) {
    if (arg.decay_l2 > 0)
      idx_dotcacc(x, arg.decay_l2, dx);
    if (arg.decay_l1 > 0)
      idx_signdotcacc(x, (T) arg.decay_l1, dx);
    if (arg.inertia == 0) {
      idx_mul(dx, epsilons, dx);
      idx_dotcacc(dx, -arg.eta, x);
    } else {
      update_deltax((T) (1 - arg.inertia), (T) arg.inertia);
	idx_mul(deltax, epsilons, deltax);
	idx_dotcacc(deltax, -arg.eta, x);
    }
  }

  template <class T>
  void parameter<T>::update(gd_param &arg) {
    update_gd(arg);
  }

  template <class T>
  void parameter<T>::clear_deltax() {
    idx_clear(deltax);
  }

  template <class T>
  void parameter<T>::update_deltax(T knew, T kold) {
    idx_lincomb(dx, knew, deltax, kold, deltax);
  }

  template <class T>
  void parameter<T>::clear_ddeltax() {
    idx_clear(ddeltax);
  }

  template <class T>
  void parameter<T>::update_ddeltax(T knew, T kold) {
    idx_lincomb(ddx, knew, ddeltax, kold, ddeltax);
  }

  template <class T>
  void parameter<T>::set_epsilons(T m) {
    idx_fill(epsilons, m);
  }

  template <class T>
  void parameter<T>::compute_epsilons(T mu) {
    idx_addc(ddeltax, mu, epsilons);
    idx_inv(epsilons, epsilons);
  }

  ////////////////////////////////////////////////////////////////
  // state_idxlooper

  template <class T>
  state_idxlooper<T>::state_idxlooper(state_idx<T> &s, int ld) 
    : state_idx<T>(s.x.select(ld, 0), 
		s.dx.select(ld, 0), 
		s.ddx.select(ld, 0)),
      lx(s.x, ld), ldx(s.dx, ld), lddx(s.ddx, ld) {
  }

  template <class T>
  state_idxlooper<T>::~state_idxlooper() {
  }

  template <class T>
  void state_idxlooper<T>::next() {
    lx.next();
    ldx.next();
    lddx.next();
    x = lx;
    dx = ldx;
    ddx = lddx;
  }

  // return true when done.
  template <class T>
  bool state_idxlooper<T>::notdone() { 
    return lx.notdone(); 
  }

} // end namespace ebl
