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
  // fstate_idx

  template<typename T>
  fstate_idx<T>::~fstate_idx() {
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a fparameter

  template<typename T>
  fstate_idx<T>::fstate_idx() {
    clear();
  }

  template<typename T>
  fstate_idx<T>::fstate_idx(intg s0) {
    x = idx<T>(s0);
    clear();
  }

  template<typename T>
  fstate_idx<T>::fstate_idx(intg s0, intg s1) {
    x = idx<T>(s0, s1);
    clear();
  }

  template<typename T>
  fstate_idx<T>::fstate_idx(intg s0, intg s1, intg s2) {
    x = idx<T>(s0, s1, s2);
    clear();
  }

  template<typename T>
  fstate_idx<T>::fstate_idx(intg s0, intg s1, intg s2, intg s3, intg s4,
			    intg s5, intg s6, intg s7) {
    x = idx<T>(s0, s1, s2, s3, s4, s5, s6, s7);
    clear();
  }

  template<typename T>
  fstate_idx<T>::fstate_idx(const idxdim &d) {
    x = idx<T>(d);
    clear();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a fparameter

  template<typename T>
  fstate_idx<T>::fstate_idx(parameter<T,fstate_idx<T> > *st) {
    x = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  fstate_idx<T>::fstate_idx(parameter<T,fstate_idx<T> > *st, intg s0) {
    x = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, s0);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  fstate_idx<T>::fstate_idx(parameter<T,fstate_idx<T> > *st, intg s0, intg s1) {
    x = idx<T>(st ? st->x.getstorage() : NULL,
	       st ? st->x.footprint() : 0, s0, s1);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  fstate_idx<T>::fstate_idx(parameter<T,fstate_idx<T> > *st, intg s0, intg s1,
			    intg s2) {
    x = idx<T>(st ? st->x.getstorage() : NULL,
	       st ? st->x.footprint() : 0, s0, s1, s2);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }
  
  template<typename T>
  fstate_idx<T>::fstate_idx(parameter<T,fstate_idx<T> > *st, intg s0, intg s1,
			    intg s2,
			    intg s3, intg s4, intg s5, intg s6, intg s7) {
    x = idx<T>(st ? st->x.getstorage() : NULL,
	       st ? st->x.footprint() : 0, s0, s1, s2, s3, s4, s5, s6, s7);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  fstate_idx<T>::fstate_idx(parameter<T,fstate_idx<T> > *st, const idxdim &d) {
    x = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, d);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from other fstate_idx's dimensions

  template<typename T>
  fstate_idx<T>::fstate_idx(const idx<T> &_x) {
    x = idx<T>(_x);
  }

  ////////////////////////////////////////////////////////////////
  // clear methods

  template <typename T> void fstate_idx<T>::clear() {
    //    cout << "clearall(x): " << this << endl;
    idx_clear(x);
  }

  template <typename T> void fstate_idx<T>::clear_x() {
    //    cout << "clear(x): " << this << endl;
    idx_clear(x);
  }

  ////////////////////////////////////////////////////////////////
  // information methods

  template <typename T> intg fstate_idx<T>::nelements() {
    return x.nelements();
  }

  template <typename T> intg fstate_idx<T>::footprint() {
    return x.footprint();
  }

  template <typename T> intg fstate_idx<T>::size() {
    return x.footprint();
  }

  ////////////////////////////////////////////////////////////////
  // resize methods

  template <typename T>
  void fstate_idx<T>::resize(intg s0, intg s1, intg s2,
			    intg s3, intg s4, intg s5,
			    intg s6, intg s7) {
    if (!x.same_dim(s0, s1, s2, s3, s4, s5, s6, s7)) { // save some time
      x.resize(s0, s1, s2, s3, s4, s5, s6, s7);
    }
  }

  template <typename T>
  void fstate_idx<T>::resize(const idxdim &d) {
    if (!x.same_dim(d)) { // save some time if dimensions are the same
      x.resize(d);
    }
  }

  template <typename T>
  void fstate_idx<T>::resize1(intg dimn, intg size) {
    if (x.dim(dimn) != size) { // save some time if size is already set.
      x.resize1(dimn, size);
    }
  }

  template <typename T>
  void fstate_idx<T>::resize_as(fstate_idx<T>& s) {
    idxdim d(s.x.spec); // use same dimensions as s
    resize(d);
  }

  template <typename T>
  void fstate_idx<T>::resize_as_but1(fstate_idx<T>& s, intg fixed_dim) {
    idxdim d(s.x.spec); // use same dimensions as s
    d.setdim(fixed_dim, x.dim(fixed_dim));
    resize(d);
  }

  template <typename T>
  fstate_idx<T> fstate_idx<T>::make_copy() {
    intg dims[8] ={-1,-1,-1,-1,-1,-1,-1,-1};
    for (int i=0; i<x.order(); i++){
      dims[i] = x.dim(i);
    }
    fstate_idx<T> other(dims[0],dims[1],dims[2],dims[3],dims[4],dims[5],dims[6],
		    dims[7]);
    idx_copy(x,other.x);
    return other;
  }

  ////////////////////////////////////////////////////////////////
  // bstate_idx

  template<typename T>
  bstate_idx<T>::~bstate_idx() {
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a bparameter

  template<typename T>
  bstate_idx<T>::bstate_idx() {
    clear();
  }

  template<typename T>
  bstate_idx<T>::bstate_idx(intg s0) {
    x = idx<T>(s0);
    dx = idx<T>(s0);
    clear();
  }

  template<typename T>
  bstate_idx<T>::bstate_idx(intg s0, intg s1) {
    x = idx<T>(s0, s1);
    dx = idx<T>(s0, s1);
    clear();
  }

  template<typename T>
  bstate_idx<T>::bstate_idx(intg s0, intg s1, intg s2) {
    x = idx<T>(s0, s1, s2);
    dx = idx<T>(s0, s1, s2);
    clear();
  }

  template<typename T>
  bstate_idx<T>::bstate_idx(intg s0, intg s1, intg s2, intg s3, intg s4,
			    intg s5, intg s6, intg s7) {
    x = idx<T>(s0, s1, s2, s3, s4, s5, s6, s7);
    dx = idx<T>(s0, s1, s2, s3, s4, s5, s6, s7);
    clear();
  }

  template<typename T>
  bstate_idx<T>::bstate_idx(const idxdim &d) {
    x = idx<T>(d);
    dx = idx<T>(d);
    clear();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a bparameter

  template<typename T>
  bstate_idx<T>::bstate_idx(parameter<T,bstate_idx<T> > *st) {
    x = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0);
    dx = idx<T>(st ? st->dx.getstorage() : NULL, st ? st->dx.footprint() : 0);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  bstate_idx<T>::bstate_idx(parameter<T,bstate_idx<T> > *st, intg s0) {
    x = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, s0);
    dx = idx<T>(st ? st->dx.getstorage() : NULL,
		st ? st->dx.footprint() : 0, s0);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  bstate_idx<T>::bstate_idx(parameter<T,bstate_idx<T> > *st, intg s0, intg s1) {
    x = idx<T>(st ? st->x.getstorage() : NULL,
	       st ? st->x.footprint() : 0, s0, s1);
    dx = idx<T>(st ? st->dx.getstorage() : NULL,
		st ? st->dx.footprint() : 0, s0, s1);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  bstate_idx<T>::bstate_idx(parameter<T,bstate_idx<T> > *st, intg s0, intg s1,
			    intg s2) {
    x = idx<T>(st ? st->x.getstorage() : NULL,
	       st ? st->x.footprint() : 0, s0, s1, s2);
    dx = idx<T>(st ? st->dx.getstorage() : NULL,
		st ? st->dx.footprint() : 0, s0, s1, s2);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }
  
  template<typename T>
  bstate_idx<T>::bstate_idx(parameter<T,bstate_idx<T> > *st, intg s0, intg s1,
			    intg s2, intg s3, intg s4, intg s5,
			    intg s6, intg s7) {
    x = idx<T>(st ? st->x.getstorage() : NULL,
	       st ? st->x.footprint() : 0, s0, s1, s2, s3, s4, s5, s6, s7);
    dx = idx<T>(st ? st->dx.getstorage() : NULL,
		st ? st->dx.footprint : 0, s0, s1, s2, s3, s4, s5, s6,s7);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  bstate_idx<T>::bstate_idx(parameter<T,bstate_idx<T> > *st, const idxdim &d) {
    x = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, d);
    dx = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, d);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from other bstate_idx's dimensions

  template<typename T>
  bstate_idx<T>::bstate_idx(const idx<T> &_x, const idx<T> &_dx) {
    x = idx<T>(_x);
    dx = idx<T>(_dx);
  }

  ////////////////////////////////////////////////////////////////
  // clear methods

  template <typename T> void bstate_idx<T>::clear() {
    //    cout << "clearall(dx): " << this << endl;
    idx_clear(x);
    idx_clear(dx);
    idx_clear(ddx);
  }

  template <typename T> void bstate_idx<T>::clear_dx() {
    //    cout << "clear(dx): " << this << endl;
    idx_clear(dx);
  }

  ////////////////////////////////////////////////////////////////
  // resize methods

  template <typename T>
  void bstate_idx<T>::resize(intg s0, intg s1, intg s2,
			    intg s3, intg s4, intg s5,
			    intg s6, intg s7) {
    if (!x.same_dim(s0, s1, s2, s3, s4, s5, s6, s7)) { // save some time
      x.resize(s0, s1, s2, s3, s4, s5, s6, s7);
      dx.resize(s0, s1, s2, s3, s4, s5, s6, s7);
    }
  }

  template <typename T>
  void bstate_idx<T>::resize(const idxdim &d) {
    if (!x.same_dim(d)) { // save some time if dimensions are the same
      x.resize(d);
      dx.resize(d);
    }
  }

  template <typename T>
  void bstate_idx<T>::resize1(intg dimn, intg size) {
    if (x.dim(dimn) != size) { // save some time if size is already set.
      x.resize1(dimn, size);
      dx.resize1(dimn, size);
    }
  }

  template <typename T>
  void bstate_idx<T>::resize_as(bstate_idx<T>& s) {
    idxdim d(s.x.spec); // use same dimensions as s
    resize(d);
  }

  template <typename T>
  void bstate_idx<T>::resize_as_but1(bstate_idx<T>& s, intg fixed_dim) {
    idxdim d(s.x.spec); // use same dimensions as s
    d.setdim(fixed_dim, x.dim(fixed_dim));
    resize(d);
  }

  // template <typename T>
  // void bstate_idx<T>::resize(const intg* dimsBegin, const intg* dimsEnd) {
  //   x.resize(dimsBegin, dimsEnd);
  //   dx.resize(dimsBegin, dimsEnd);
  //   ddx.resize(dimsBegin, dimsEnd);
  // }

  template <typename T>
  void bstate_idx<T>::update_gd(gd_param &arg) {
    if (arg.decay_l2 > 0) {
      idx_dotcacc(x, arg.decay_l2, dx);
    }
    if (arg.decay_l1 > 0) {
      idx_signdotcacc(x, (T) arg.decay_l1, dx);
    }
    idx_dotcacc(dx, -arg.eta, x);
  }

  template <typename T>
  bstate_idx<T> bstate_idx<T>::make_copy() {
    intg dims[8] ={-1,-1,-1,-1,-1,-1,-1,-1};
    for (int i=0; i<x.order(); i++){
      dims[i] = x.dim(i);
    }
    bstate_idx<T> other(dims[0],dims[1],dims[2],dims[3],dims[4],dims[5],dims[6],
			dims[7]);
    idx_copy(x,other.x);
    idx_copy(dx,other.dx);
    return other;
  }

  ////////////////////////////////////////////////////////////////
  // bbstate_idx

  template<typename T>
  bbstate_idx<T>::~bbstate_idx() {
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a bbparameter

  template<typename T>
  bbstate_idx<T>::bbstate_idx() {
    clear();
  }

  template<typename T>
  bbstate_idx<T>::bbstate_idx(intg s0) {
    x = idx<T>(s0);
    dx = idx<T>(s0);
    ddx = idx<T>(s0);
    clear();
  }

  template<typename T>
  bbstate_idx<T>::bbstate_idx(intg s0, intg s1) {
    x = idx<T>(s0, s1);
    dx = idx<T>(s0, s1);
    ddx = idx<T>(s0, s1);
    clear();
  }

  template<typename T>
  bbstate_idx<T>::bbstate_idx(intg s0, intg s1, intg s2) {
    x = idx<T>(s0, s1, s2);
    dx = idx<T>(s0, s1, s2);
    ddx = idx<T>(s0, s1, s2);
    clear();
  }

  template<typename T>
  bbstate_idx<T>::bbstate_idx(intg s0, intg s1, intg s2, intg s3, intg s4,
			      intg s5, intg s6, intg s7) {
    x = idx<T>(s0, s1, s2, s3, s4, s5, s6, s7);
    dx = idx<T>(s0, s1, s2, s3, s4, s5, s6, s7);
    ddx = idx<T>(s0, s1, s2, s3, s4, s5, s6, s7);
    clear();
  }

  template<typename T>
  bbstate_idx<T>::bbstate_idx(const idxdim &d) {
    x = idx<T>(d);
    dx = idx<T>(d);
    ddx = idx<T>(d);
    clear();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from specific dimensions using a bbparameter

  template<typename T>
  bbstate_idx<T>::bbstate_idx(parameter<T,bbstate_idx<T> > *st) {
    x = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0);
    dx = idx<T>(st ? st->dx.getstorage() : NULL, st ? st->dx.footprint() : 0);
    ddx = idx<T>(st ? st->ddx.getstorage() : NULL, st ? st->ddx.footprint() :0);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  bbstate_idx<T>::bbstate_idx(parameter<T,bbstate_idx<T> > *st, intg s0) {
    x = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, s0);
    dx = idx<T>(st ? st->dx.getstorage() : NULL,
		st ? st->dx.footprint() : 0, s0);
    ddx = idx<T>(st ? st->ddx.getstorage() : NULL,
		 st ? st->ddx.footprint() : 0, s0);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  bbstate_idx<T>::bbstate_idx(parameter<T,bbstate_idx<T> > *st, intg s0,
			      intg s1) {
    x = idx<T>(st ? st->x.getstorage() : NULL,
	       st ? st->x.footprint() : 0, s0, s1);
    dx = idx<T>(st ? st->dx.getstorage() : NULL,
		st ? st->dx.footprint() : 0, s0, s1);
    ddx = idx<T>(st ? st->ddx.getstorage() : NULL,
		 st ? st->ddx.footprint() : 0, s0, s1);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  bbstate_idx<T>::bbstate_idx(parameter<T,bbstate_idx<T> > *st, intg s0,
			      intg s1, intg s2) {
    x = idx<T>(st ? st->x.getstorage() : NULL,
	       st ? st->x.footprint() : 0, s0, s1, s2);
    dx = idx<T>(st ? st->dx.getstorage() : NULL,
		st ? st->dx.footprint() : 0, s0, s1, s2);
    ddx = idx<T>(st ? st->ddx.getstorage() : NULL,
		 st ? st->ddx.footprint() : 0, s0, s1, s2);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }
  
  template<typename T>
  bbstate_idx<T>::bbstate_idx(parameter<T,bbstate_idx<T> > *st, intg s0,
			      intg s1, intg s2,
			      intg s3, intg s4, intg s5, intg s6, intg s7) {
    x = idx<T>(st ? st->x.getstorage() : NULL,
	       st ? st->x.footprint() : 0, s0, s1, s2, s3, s4, s5, s6, s7);
    dx = idx<T>(st ? st->dx.getstorage() : NULL,
		st ? st->dx.footprint : 0, s0, s1, s2, s3, s4, s5, s6,s7);
    ddx = idx<T>(st ? st->ddx.getstorage() : NULL,
		 st ? st->ddx.footprint() : 0, s0, s1, s2, s3, s4, s5, s6, s7);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  template<typename T>
  bbstate_idx<T>::bbstate_idx(parameter<T,bbstate_idx<T> > *st,
			      const idxdim &d) {
    x = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, d);
    dx = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, d);
    ddx = idx<T>(st ? st->x.getstorage() : NULL, st ? st->x.footprint() : 0, d);
    if (st)
      st->resize(st->footprint() + nelements());
    clear();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from other bbstate_idx's dimensions

  template<typename T>
  bbstate_idx<T>::bbstate_idx(const idx<T> &_x, const idx<T> &_dx, 
			      const idx<T> &_ddx) {
    x = idx<T>(_x);
    dx = idx<T>(_dx);
    ddx = idx<T>(_ddx);
  }

  ////////////////////////////////////////////////////////////////
  // clear methods

  template <typename T> void bbstate_idx<T>::clear() {
    //    cout << "clearall(ddx): " << this << endl;
    idx_clear(ddx);
  }

  template <typename T> void bbstate_idx<T>::clear_ddx() {
    //    cout << "clear(x, dx, ddx): " << this << endl;
    idx_clear(ddx);
  }

  ////////////////////////////////////////////////////////////////
  // resize methods

  template <typename T>
  void bbstate_idx<T>::resize(intg s0, intg s1, intg s2,
			    intg s3, intg s4, intg s5,
			    intg s6, intg s7) {
    if (!x.same_dim(s0, s1, s2, s3, s4, s5, s6, s7)) { // save some time
      x.resize(s0, s1, s2, s3, s4, s5, s6, s7);
      dx.resize(s0, s1, s2, s3, s4, s5, s6, s7);
      ddx.resize(s0, s1, s2, s3, s4, s5, s6, s7);
    }
  }

  template <typename T>
  void bbstate_idx<T>::resize(const idxdim &d) {
    if (!x.same_dim(d)) { // save some time if dimensions are the same
      x.resize(d);
      dx.resize(d);
      ddx.resize(d);
    }
  }

  template <typename T>
  void bbstate_idx<T>::resize1(intg dimn, intg size) {
    if (x.dim(dimn) != size) { // save some time if size is already set.
      x.resize1(dimn, size);
      dx.resize1(dimn, size);
      ddx.resize1(dimn, size);
    }
  }

  template <typename T>
  void bbstate_idx<T>::resize_as(bbstate_idx<T>& s) {
    idxdim d(s.x.spec); // use same dimensions as s
    resize(d);
  }

  template <typename T>
  void bbstate_idx<T>::resize_as_but1(bbstate_idx<T>& s, intg fixed_dim) {
    idxdim d(s.x.spec); // use same dimensions as s
    d.setdim(fixed_dim, x.dim(fixed_dim));
    resize(d);
  }

  // template <typename T>
  // void bbstate_idx<T>::resize(const intg* dimsBegin, const intg* dimsEnd) {
  //   x.resize(dimsBegin, dimsEnd);
  //   dx.resize(dimsBegin, dimsEnd);
  //   ddx.resize(dimsBegin, dimsEnd);
  // }

  template <typename T>
  bbstate_idx<T> bbstate_idx<T>::make_copy() {
    intg dims[8] ={-1,-1,-1,-1,-1,-1,-1,-1};
    for (int i=0; i<x.order(); i++){
      dims[i] = x.dim(i);
    }
    bbstate_idx<T> other(dims[0],dims[1],dims[2],dims[3],dims[4],dims[5],
			 dims[6],dims[7]);
    idx_copy(x,other.x);
    idx_copy(dx,other.dx);
    idx_copy(ddx,other.ddx);
    return other;
  }

  ////////////////////////////////////////////////////////////////
  // parameter

  template <typename T>
  parameter<T,fstate_idx<T> >::parameter(intg initial_size) 
    : fstate_idx<T>(initial_size) {
    resize(0);
  }

  template <typename T>
  parameter<T,fstate_idx<T> >::parameter(const char *param_filename) 
    : fstate_idx<T>(1) {
    if (!load_x(param_filename)) {
      cerr << "failed to open " << param_filename << endl;
      eblerror("failed to load parameter file in parameter constructor");
    }
  }

  template <typename T>
  parameter<T,fstate_idx<T> >::~parameter() {
  }

  // TODO-0: BUG: a parameter object casted in state_idx* and called
  // with resize(n) calls state_idx::resize instead of parameter<T>::resize
  // a temporary unclean solution is to use the same parameters as
  // in state_idx::resize in parameter<T>::resize:
  // resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
  //		intg s6, intg s7);
  template <typename T>
  void parameter<T,fstate_idx<T> >::resize(intg s0) {
    x.resize(s0);
  }

  template <typename T>
  bool parameter<T,fstate_idx<T> >::save_x(const char *s) {
    if (!save_matrix(x, s))
      return false;
    return true;
  }

  template <typename T>
  bool parameter<T,fstate_idx<T> >::load_x(const char *s) {
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

  ////////////////////////////////////////////////////////////////
  // parameter<T,bstate_idx<T> >

  template <typename T>
  parameter<T,bstate_idx<T> >::parameter(intg initial_size) 
    : bstate_idx<T>(initial_size), gradient(initial_size),
      deltax(initial_size), epsilons(initial_size) {
    idx_clear(gradient);
    idx_clear(deltax);
    idx_clear(epsilons);
    resize(0);
  }

  template <typename T>
  parameter<T,bstate_idx<T> >::parameter(const char *param_filename) 
    : bstate_idx<T>(1), gradient(1), deltax(1), epsilons(1) {
    if (!load_x(param_filename)) {
      cerr << "failed to open " << param_filename << endl;
      eblerror("failed to load bparameter file in bparameter constructor");
    }
  }

  template <typename T>
  parameter<T,bstate_idx<T> >::~parameter() {
  }

  // TODO-0: BUG: a bparameter object casted in state_idx* and called
  // with resize(n) calls state_idx::resize instead of parameter<T,bstate_idx<T> >resize
  // a temporary unclean solution is to use the same bparameters as
  // in state_idx::resize in parameter<T,bstate_idx<T> >resize:
  // resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
  //		intg s6, intg s7);
  template <typename T>
  void parameter<T,bstate_idx<T> >::resize(intg s0) {
    x.resize(s0);
    dx.resize(s0);
    gradient.resize(s0);
    deltax.resize(s0);
    epsilons.resize(s0);
  }

  template <typename T>
  bool parameter<T,bstate_idx<T> >::save_x(const char *s) {
    if (!save_matrix(x, s))
      return false;
    return true;
  }

  template <typename T>
  bool parameter<T,bstate_idx<T> >::load_x(const char *s) {
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

  template <typename T>
  void parameter<T,bstate_idx<T> >::update_gd(gd_param &arg) {
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

  template <typename T>
  void parameter<T,bstate_idx<T> >::update(gd_param &arg) {
    update_gd(arg);
  }

  template <typename T>
  void parameter<T,bstate_idx<T> >::clear_deltax() {
    idx_clear(deltax);
  }

  template <typename T>
  void parameter<T,bstate_idx<T> >::update_deltax(T knew, T kold) {
    idx_lincomb(dx, knew, deltax, kold, deltax);
  }

  template <typename T>
  void parameter<T,bstate_idx<T> >::set_epsilons(T m) {
    idx_fill(epsilons, m);
  }

  ////////////////////////////////////////////////////////////////
  // parameter<T,bbstate_idx<T> >

  template <typename T>
  parameter<T,bbstate_idx<T> >::parameter(intg initial_size) 
    : bbstate_idx<T>(initial_size), gradient(initial_size),
      deltax(initial_size), epsilons(initial_size), ddeltax(initial_size) {
    idx_clear(gradient);
    idx_clear(deltax);
    idx_clear(epsilons);
    idx_clear(ddeltax);
    resize(0);
  }

  template <typename T>
  parameter<T,bbstate_idx<T> >::parameter(const char *param_filename) 
    : bbstate_idx<T>(1), gradient(1), deltax(1), epsilons(1), ddeltax(1) {
    if (!load_x(param_filename)) {
      cerr << "failed to open " << param_filename << endl;
      eblerror("failed to load bbparameter file in bbparameter constructor");
    }
  }

  template <typename T>
  parameter<T,bbstate_idx<T> >::~parameter() {
  }

  // TODO-0: BUG: a bbparameter object casted in state_idx* and called
  // with resize(n) calls state_idx::resize instead of
  // parameter<T,bbstate_idx<T> >::resize
  // a temporary unclean solution is to use the same bbparameters as
  // in state_idx::resize in parameter<T,bbstate_idx<T> >::resize:
  // resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
  //		intg s6, intg s7);
  template <typename T>
  void parameter<T,bbstate_idx<T> >::resize(intg s0) {
    this->x.resize(s0);
    this->dx.resize(s0);
    this->ddx.resize(s0);
    gradient.resize(s0);
    deltax.resize(s0);
    epsilons.resize(s0);
    ddeltax.resize(s0);
  }

  template <typename T>
  bool parameter<T,bbstate_idx<T> >::save_x(const char *s) {
    if (!save_matrix(this->x, s))
      return false;
    return true;
  }

  template <typename T>
  bool parameter<T,bbstate_idx<T> >::load_x(const char *s) {
    try {
      idx<T> m = load_matrix<T>(s);
      this->resize(m.dim(0));
      idx_copy(m, this->x);
      cout << "Loaded weights from " << s << ": " << this->x << endl;
      return true;
    } catch(string &err) {
      cout << err << endl;
      eblerror("failed to load weights");
    }
    return false;
  }

  template <typename T>
  void parameter<T,bbstate_idx<T> >::update_gd(gd_param &arg) {
    if (arg.decay_l2 > 0)
      idx_dotcacc(this->x, arg.decay_l2, this->dx);
    if (arg.decay_l1 > 0)
      idx_signdotcacc(this->x, (T) arg.decay_l1, this->dx);
    if (arg.inertia == 0) {
      idx_mul(this->dx, epsilons, this->dx);
      idx_dotcacc(this->dx, -arg.eta, this->x);
    } else {
      update_deltax((T) (1 - arg.inertia), (T) arg.inertia);
  	idx_mul(deltax, epsilons, deltax);
  	idx_dotcacc(deltax, -arg.eta, this->x);
    }
  }

  template <typename T>
  void parameter<T,bbstate_idx<T> >::update(gd_param &arg) {
    update_gd(arg);
  }

  template <typename T>
  void parameter<T,bbstate_idx<T> >::clear_deltax() {
    idx_clear(deltax);
  }

  template <typename T>
  void parameter<T,bbstate_idx<T> >::update_deltax(T knew, T kold) {
    idx_lincomb(this->dx, knew, deltax, kold, deltax);
  }

  template <typename T>
  void parameter<T,bbstate_idx<T> >::clear_ddeltax() {
    idx_clear(ddeltax);
  }

  template <typename T>
  void parameter<T,bbstate_idx<T> >::update_ddeltax(T knew, T kold) {
    idx_lincomb(this->ddx, knew, ddeltax, kold, ddeltax);
  }

  template <typename T>
  void parameter<T,bbstate_idx<T> >::set_epsilons(T m) {
    idx_fill(epsilons, m);
  }

  template <typename T>
  void parameter<T,bbstate_idx<T> >::compute_epsilons(T mu) {
    idx_addc(ddeltax, mu, epsilons);
    idx_inv(epsilons, epsilons);
  }

  ////////////////////////////////////////////////////////////////
  // fstate_idxlooper

  template <typename T>
  state_idxlooper<fstate_idx<T> >::state_idxlooper(fstate_idx<T> &s, int ld) 
    : fstate_idx<T>(s.x.select(ld, 0)), lx(s.x, ld) {
  }

  template <typename T>
  state_idxlooper<fstate_idx<T> >::~state_idxlooper() {
  }

  template <typename T>
  void state_idxlooper<fstate_idx<T> >::next() {
    lx.next();
    x = lx;
  }

  // return true when done.
  template <typename T>
  bool state_idxlooper<fstate_idx<T> >::notdone() { 
    return lx.notdone(); 
  }

  ////////////////////////////////////////////////////////////////
  // bstate_idxlooper

  template <typename T>
  state_idxlooper<bstate_idx<T> >::state_idxlooper(bstate_idx<T> &s, int ld) 
    : bstate_idx<T>(s.x.select(ld, 0), s.dx.select(ld, 0)), 
      lx(s.x, ld), ldx(s.dx, ld) {
  }

  template <typename T>
  state_idxlooper<bstate_idx<T> >::~state_idxlooper() {
  }

  template <typename T>
  void state_idxlooper<bstate_idx<T> >::next() {
    lx.next();
    ldx.next();
    x = lx;
    dx = ldx;
  }

  // return true when done.
  template <typename T>
  bool state_idxlooper<bstate_idx<T> >::notdone() { 
    return lx.notdone(); 
  }

  ////////////////////////////////////////////////////////////////
  // bbstate_idxlooper

  template <typename T>
  state_idxlooper<bbstate_idx<T> >::state_idxlooper(bbstate_idx<T> &s, int ld) 
    : bbstate_idx<T>(s.x.select(ld, 0), 
		     s.dx.select(ld, 0), 
		     s.ddx.select(ld, 0)),
      lx(s.x, ld), ldx(s.dx, ld), lddx(s.ddx, ld) {
  }

  template <typename T>
  state_idxlooper<bbstate_idx<T> >::~state_idxlooper() {
  }

  template <typename T>
  void state_idxlooper<bbstate_idx<T> >::next() {
    lx.next();
    ldx.next();
    lddx.next();
    x = lx;
    dx = ldx;
    ddx = lddx;
  }

  // return true when done.
  template <typename T>
  bool state_idxlooper<bbstate_idx<T> >::notdone() { 
    return lx.notdone(); 
  }

} // end namespace ebl
