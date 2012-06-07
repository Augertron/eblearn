/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
 *   All rights reserved.
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

#ifndef IDX_HPP_
#define IDX_HPP_

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // idx memory methods

#define growstorage() {						\
  if (storage->growsize(spec.footprint()) < 0)			\
    eblerror("cannot grow storage to " << spec.footprint()	\
	     << " bytes (probably out of memory)");		\
  }

  template <class T> void idx<T>::growstorage_chunk(intg s_chunk){
    if (storage->growsize_chunk(spec.footprint(), s_chunk) < 0)
      eblerror("cannot grow storage to " << spec.footprint()
	       << " bytes (probably out of memory)");
  }

  ////////////////////////////////////////////////////////////////
  // idx basic constructors/destructor

  template <class T> idx<T>::~idx() {
    DEBUG_LOW("idx::destructor " << long(this));
    storage->unlock();
    if (this->pidxdim)
      delete this->pidxdim;
  }

  // fake constructor called by idxlooper constructor
  template <class T> idx<T>::idx(dummyt *dummy) {
    spec.dim = NULL;
    spec.mod = NULL;
    storage = NULL;
    pidxdim = NULL;
  }

  template <class T> idx<T>::idx(const idx<T>& other)
    : spec(other.spec), storage(other.storage), pidxdim(NULL) {
    storage->lock();
  }

  ////////////////////////////////////////////////////////////////
  //! constructors initialized with an array

  template <class T> idx<T>::idx(const T *mat, intg s0, intg s1)
    : spec(0, s0, s1), pidxdim(NULL) {
    storage = new srg<T>();
    growstorage();
    storage->lock();
    memcpy(idx_ptr(), mat, nelements() * sizeof (T));
  }

  template <class T> idx<T>::idx(const T *mat, intg s0, intg s1, intg s2)
    : spec(0, s0, s1, s2), pidxdim(NULL) {
    storage = new srg<T>();
    growstorage();
    storage->lock();
    memcpy(idx_ptr(), mat, nelements() * sizeof (T));
  }

  ////////////////////////////////////////////////////////////////
  // specific constructors for each number of dimensions

  template <class T> idx<T>::idx() : spec(0), pidxdim(NULL) {
    storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T> idx<T>::idx(intg size0)
  : spec(0,size0), pidxdim(NULL) {
    storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T> idx<T>::idx(intg size0, intg size1)
    : spec(0,size0,size1), pidxdim(NULL) {
    storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T> idx<T>::idx(intg size0, intg size1, intg size2)
    : spec(0,size0,size1,size2), pidxdim(NULL) {
    storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T>
  idx<T>::idx(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
	      intg s6, intg s7)
    : spec(0,s0,s1,s2,s3,s4,s5,s6,s7), pidxdim(NULL) {
    storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T> idx<T>::idx(const idxdim &d)
    : spec(0, d), pidxdim(NULL) {
    storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from existing srg and offset

  template <class T>
  idx<T>::idx(srg<T> *sg, idxspec &s) : pidxdim(NULL) {
    spec = s;
    if (sg) // use passed srg if not null
      storage = sg;
    else // otherwise create new one
      storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T>
  idx<T>::idx(srg<T> *sg, intg o, intg n, intg *dims, intg *mods)
    : spec(sg ? o : 0, n, dims, mods), pidxdim(NULL) {
    if (sg) // use passed srg if not null
      storage = sg;
    else // otherwise create new one
      storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T>
  idx<T>::idx(srg<T> *sg, intg o)
    : spec(sg ? o : 0), pidxdim(NULL) {
    if (sg) // use passed srg if not null
      storage = sg;
    else // otherwise create new one
      storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T>
  idx<T>::idx(srg<T> *sg, intg o, intg size0)
    : spec(sg ? o : 0, size0), pidxdim(NULL) {
    if (sg) // use passed srg if not null
      storage = sg;
    else // otherwise create new one
      storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T>
  idx<T>::idx(srg<T> *sg, intg o, intg size0, intg size1)
    : spec(sg ? o : 0, size0, size1), pidxdim(NULL) {
    if (sg) // use passed srg if not null
      storage = sg;
    else // otherwise create new one
      storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T>
  idx<T>::idx(srg<T> *sg, intg o, intg size0, intg size1, intg size2)
    : spec(sg ? o : 0, size0, size1, size2), pidxdim(NULL) {
    if (sg) // use passed srg if not null
      storage = sg;
    else // otherwise create new one
      storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T>
  idx<T>::idx(srg<T> *sg, intg o, intg s0, intg s1, intg s2, intg s3,
	      intg s4, intg s5, intg s6, intg s7)
    : spec(sg ? o : 0, s0, s1, s2, s3, s4, s5, s6, s7), pidxdim(NULL) {
    if (sg) // use passed sg if not null
      storage = sg;
    else // otherwise create new one
      storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T>
  idx<T>::idx(srg<T> *sg, intg o, const idxdim &d)
    : spec(sg ? o : 0, d), pidxdim(NULL) {
    if (sg) // use passed srg if not null
      storage = sg;
    else // otherwise create new one
      storage = new srg<T>();
    growstorage();
    storage->lock();
  }

  ////////////////////////////////////////////////////////////////
  // operators

  template <class T>
  idx<T>& idx<T>::operator=(T other){
    eblerror("Forbidden idx assignment: it can only be assigned another idx");
    return *this;
  }

  template <class T>
  idx<T>& idx<T>::operator=(const idx<T>& other) {
    if (this != &other) { // protect against invalid self-assignment
      srg<T> *tmp = NULL;
      if (this->storage != NULL)
	tmp = this->storage;
      this->storage = other.storage;
      this->spec = other.spec;
      this->storage->lock();
      if (tmp) // unlock previous storage
	tmp->unlock();
      if (other.pidxdim) this->pidxdim = new idxdim(*other.pidxdim);
      else this->pidxdim = NULL;
    }
    return *this;
  }

  template <class T>
  idx<T> idx<T>::operator[](const intg i) {
    return this->select(0,i);
  }

  ////////////////////////////////////////////////////////////////
  // resize methods

  template <class T>
  intg idx<T>::setoffset(intg o) {
    if (o<0) { eblerror("idx::setoffset: offset must be positive"); }
    if (o > spec.offset) {
      spec.setoffset(o);
      growstorage();
      return o;
    } else {
      spec.setoffset(o);
      return o;
    }
  }

  template <class T>
  void idx<T>::add_offset(intg o) {
    spec.add_offset(o);
  }

  template <class T>
  void idx<T>::resize(intg s0, intg s1, intg s2, intg s3,
		      intg s4, intg s5, intg s6, intg s7) {
    if (!same_dim(s0,s1,s2,s3,s4,s5,s6,s7)) { // save some time
      spec.resize(s0,s1,s2,s3,s4,s5,s6,s7);
      growstorage();
    }
  }

  template <class T>
  void idx<T>::resize(const idxdim &d) {
    if (d.order() > spec.ndim)
      eblerror("cannot change order of idx in resize while trying to resize "
	       << " from " << (*this) << " to " << d);
    if (!same_dim(d)) { // save some time if dims are same
      spec.resize(d);
      growstorage();
    }
  }

  template <class T>
  void idx<T>::resize1(intg dimn, intg size) {
    if (dimn > spec.ndim) eblerror("cannot change order of idx in resize");
    if (spec.dim[dimn] != size) {
      spec.resize1(dimn, size);
      growstorage();
    }
  }

  template <class T>
  void idx<T>::resize_chunk(intg s_chunk, intg s0, intg s1, intg s2, intg s3,
			    intg s4, intg s5, intg s6, intg s7) {
    spec.resize(s0,s1,s2,s3,s4,s5,s6,s7);
    growstorage_chunk(s_chunk);
  }

  // template <class T>
  // void idx<T>::resize_copy(intg s0, intg s1, intg s2, intg s3,
  //       	      intg s4, intg s5, intg s6, intg s7) {
  //   idx<T> oldt(*this);
  //   idx_copy(*this, oldt);
  //   resize(s0,s1,s2,s3,s4,s5,s6,s7);
  //   idx<T> newt(*this);
  //   for (int i = 0; i < oldt.order(); ++i)
  //     newt = newt.narrow(i, 0, oldt.dim(i));
  //   idx_copy(oldt,newt);
  // }

  // return true if this idx has same order and dimensions as idxdim d.
  // i.e. if all their dimensions are equal (regardless of strides).
  template <class T>
  bool idx<T>::same_dim(const idxdim &d) {
    if (spec.ndim != d.order())
      return false;
    for (int i=0; i < spec.ndim; ++i)
      if (spec.dim[i] != d.dim(i))
	return false;
    return true;
  }

  // idx manipulation methods //////////////////////////////////////////////////

  template <class T> idx<T> idx<T>::select(int d, intg i) {
    idx<T> r(storage,spec.getoffset());
    spec.select_into(&r.spec, d, i);
    return r;
  }

  template <class T> idx<T> idx<T>::narrow(int d, intg s, intg o) {
    idx<T> r(storage,spec.getoffset());
    spec.narrow_into(&r.spec, d, s, o);
    return r;
  }

  template <class T> idx<T> idx<T>::transpose(int d1, int d2) {
    idx<T> r(storage,spec.getoffset());
    spec.transpose_into(&r.spec, d1, d2);
    return r;
  }

  template <class T> idx<T> idx<T>::transpose(int *p) {
    idx<T> r(storage,spec.getoffset());
    spec.transpose_into(&r.spec, p);
    return r;
  }

  template <class T> idx<T> idx<T>::unfold(int d, intg k, intg s) {
    idx<T> r(storage,spec.getoffset());
    spec.unfold_into(&r.spec, d, k, s);
    return r;
  }

  template <class T> idx<T> idx<T>::view_as_order(int n) {
    if (n < 0) {
      eblerror("view_as_order: input dimension must be positive");
      return *this;
    }
    if (n == spec.ndim)
      return *this;
    else {
      if ((n == 1) && (spec.ndim == 1)) {
	// the order is already 1, do nothing and return current idx.
	return idx<T>(*this);
      }
      else if (n == 1) {
	// the order is not 1, check that data is contiguous and return
	// a 1D idx.
	idx_check_contiguous1(*this);
	idx<T> r(getstorage(), 0, spec.nelements());
	return r;
      }
      else if (n > spec.ndim) {
	intg *ldim = new intg[n];
	intg *lmod = new intg[n];
	memcpy(ldim, spec.dim, spec.ndim * sizeof (intg));
	memcpy(lmod, spec.mod, spec.ndim * sizeof (intg));
	for (int i = spec.ndim; i < n; ++i) {
	  ldim[i] = 1;
	  lmod[i] = 1;
	}
	idx<T> r(getstorage(), spec.getoffset(), n, ldim, lmod);
	if (ldim) delete[] ldim;
	if (lmod) delete[] lmod;
	return r;
      }
      else {
	eblerror("view_as_order is not defined when n < current order");
	return idx<T>(*this);
      }
    }
  }

  template <class T> idx<T> idx<T>::shift_dim(int d, int pos) {
    int tr[MAXDIMS];
    for (int i = 0, j = 0; i < spec.ndim; ++i) {
      if (i == pos)
	tr[i] = d;
      else {
	if (j == d)
	  j++;
	tr[i] = j++;
      }
    }
    return transpose(tr);
  }

  // return true if this idx has same order and dimensions s0 .. s7
  // i.e. if all their dimensions are equal (regardless of strides).
  template <class T>
  bool idx<T>::same_dim(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
			 intg s6, intg s7) {
    if ((s7 >= 0) && (spec.ndim < 8)) return false;
    if ((spec.ndim == 8) && (s7 != spec.dim[7])) return false;
    if ((s6 >= 0) && (spec.ndim < 7)) return false;
    if ((spec.ndim >= 7) && (s6 != spec.dim[6])) return false;
    if ((s5 >= 0) && (spec.ndim < 6)) return false;
    if ((spec.ndim >= 6) && (s5 != spec.dim[5])) return false;
    if ((s4 >= 0) && (spec.ndim < 5)) return false;
    if ((spec.ndim >= 5) && (s4 != spec.dim[4])) return false;
    if ((s3 >= 0) && (spec.ndim < 4)) return false;
    if ((spec.ndim >= 4) && (s3 != spec.dim[3])) return false;
    if ((s2 >= 0) && (spec.ndim < 3)) return false;
    if ((spec.ndim >= 3) && (s2 != spec.dim[2])) return false;
    if ((s1 >= 0) && (spec.ndim < 2)) return false;
    if ((spec.ndim >= 2) && (s1 != spec.dim[1])) return false;
    if ((s0 >= 0) && (spec.ndim < 1)) return false;
    if ((spec.ndim >= 1) && (s0 != spec.dim[0])) return false;
    return true;
  }

  template <class T> idxdim& idx<T>::get_idxdim() {
    // allocating idxdim on the fly creates a memory leak somehow
    // we temporarly keep an idxdim around in all idxs (more expensive in mem)
    // TODO: change idxdim to be contained in spec, replacing the dims of spec
    // this way we don't duplicate this info twice and can keep it around
    // at the same cost of current spec.

    // if (!pidxdim)
    //   pidxdim = new idxdim();
    // pidxdim->setdims(spec);
    // return *pidxdim;

    idd.setdims(spec);
    return idd;
  }

  template <class T> idxdim idx<T>::get_idxdim() const {
    idxdim d;
    d.setdims(spec);
    return d;
  }

  // data access methods ///////////////////////////////////////////////////////

  template <class T> T* idx<T>::idx_ptr() {
    return storage->data + spec.offset;
  }
  
  template <class T> const T* idx<T>::idx_ptr() const {
    return storage->data + spec.offset;
  }
  
  template <class T> intg* idx<T>::mod_ptr() {
    return spec.mod;
  }

  // pointer access methods ////////////////////////////////////////////////////

  template <class T> T* idx<T>::ptr() {
    if (spec.ndim != 0) eblerror("not an idx0");
    return storage->data + spec.offset;
  }
  
  // get element of idx1
  template <class T> T *idx<T>::ptr(intg i0) {
    idx_checkorder1(*this, 1);
    if ((i0 < 0) || (i0 >= spec.dim[0])) eblerror("index 0 out of bound");
    return storage->data + spec.offset + i0*spec.mod[0];
  }

  // get element of idx2
  template <class T> T *idx<T>::ptr(intg i0, intg i1) {
    idx_checkorder1(*this, 2);
    if ((i0 < 0) || (i0 >= spec.dim[0])) eblerror("index 0 out of bound");
    if ((i1 < 0) || (i1 >= spec.dim[1])) eblerror("index 1 out of bound");
    return storage->data + spec.offset + i0*spec.mod[0] + i1*spec.mod[1];
  }

  // get element of idx3
  template <class T> T *idx<T>::ptr(intg i0, intg i1, intg i2) {
    idx_checkorder1(*this, 3);
    if ((i0 < 0) || (i0 >= spec.dim[0])) eblerror("index 0 out of bound");
    if ((i1 < 0) || (i1 >= spec.dim[1])) eblerror("index 1 out of bound");
    if ((i2 < 0) || (i2 >= spec.dim[2])) eblerror("index 2 out of bound");
    return storage->data + spec.offset + i0*spec.mod[0] + i1*spec.mod[1]
      + i2*spec.mod[2];
  }

  // replacing exception throwing by macro to handle environments without
  // macros (e.g. android). this makes the compiled code a bit bigger.
#define PTR_ERROR(v)							\
  eblerror("idx::get: (error " << v					\
	   << "wrong number of indices, negative or out of bound index");

  // return a pointer to an element of an idx
  // generic function for order>3
  template <class T> T *idx<T>::ptr(intg i0, intg i1, intg i2, intg i3,
				    intg i4, intg i5, intg i6, intg i7) {
    // check that we passed the right number of indices
    // and that they are all positive
    switch (spec.ndim) {
    case 8: if (i7 < 0) PTR_ERROR(-8);break;
    case 7: if ((i6 < 0) || (i7 != -1)) PTR_ERROR(-7);break;
    case 6: if ((i5 < 0) || (i6 != -1)) PTR_ERROR(-6);break;
    case 5: if ((i4 < 0) || (i5 != -1)) PTR_ERROR(-5);break;
    case 4: if ((i3<0)||(i2<0)||(i1<0)||(i0<0)||(i4 != -1)) PTR_ERROR(-4);break;
    default:
      eblerror("idx::get: number of indices and order are different");
    }
    // now compute offset, and check that all
    // indices are within bounds.
    intg k = 0;
    switch (spec.ndim) {
    case 8: k += spec.mod[7]*i7; if (i7 >= spec.dim[7])  PTR_ERROR(7);
    case 7: k += spec.mod[6]*i6; if (i6 >= spec.dim[6])  PTR_ERROR(6);
    case 6: k += spec.mod[5]*i5; if (i5 >= spec.dim[5])  PTR_ERROR(5);
    case 5: k += spec.mod[4]*i4; if (i4 >= spec.dim[4])  PTR_ERROR(4);
    case 4: k += spec.mod[3]*i3; if (i3 >= spec.dim[3])  PTR_ERROR(3);
    }
    k += spec.mod[2]*i2; if (i2 >= spec.dim[2])  PTR_ERROR(2);
    k += spec.mod[1]*i1; if (i1 >= spec.dim[1])  PTR_ERROR(1);
    k += spec.mod[0]*i0; if (i0 >= spec.dim[0])  PTR_ERROR(0);
    return storage->data + spec.offset + k;
  }

  // get methods ///////////////////////////////////////////////////////////////

  // get element of idx0
  template <class T> T idx<T>::get() const {
#ifdef __DEBUG__
    idx_checkorder1(*this, 0);
#endif
    return (storage->data)[spec.offset];
  }

  // get element of idx1
  template <class T> T& idx<T>::get(intg i0) const {
#ifdef __DEBUG__
    idx_checkorder1(*this, 1);
    if ((i0 < 0) || (i0 >= spec.dim[0])) {
      eblerror("error accessing elt " << i0 << " in " << *this
	       << ", index out of bound");
    }
#endif
    return (storage->data)[spec.offset + i0*spec.mod[0]];
  }

  // get element of idx2
  template <class T> T idx<T>::get(intg i0, intg i1) const {
#ifdef __DEBUG__
    idx_checkorder1(*this, 2);
    if (((i0 < 0) || (i0 >= spec.dim[0])) ||
	((i1 < 0) || (i1 >= spec.dim[1]))) {
      eblerror("error accessing elt " << i0 << "x"
		<< i1 << " in " << *this << ", index out of bound");
    }
#endif
    return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1]];
  }

  // get element of idx3
  template <class T> T idx<T>::get(intg i0, intg i1, intg i2) const {
#ifdef __DEBUG__
    idx_checkorder1(*this, 3);
    if (((i0 < 0) || (i0 >= spec.dim[0])) ||
	((i1 < 0) || (i1 >= spec.dim[1])) ||
	((i2 < 0) || (i2 >= spec.dim[2]))) {
      eblerror("error accessing elt " << i0 << "x"
	       << i1 << "x" << i2 << " in " << *this
	       << ", index out of bound");
    }
#endif
    return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1]
			   + i2*spec.mod[2]];
  }

  // get element of an idx of any order
  template <class T> T idx<T>::get(intg i0, intg i1, intg i2, intg i3,
				   intg i4, intg i5, intg i6, intg i7) {
    return *ptr(i0,i1,i2,i3,i4,i5,i6,i7);
  }

  // get element of an idx of any order
  template <class T> T idx<T>::gget(intg i0, intg i1, intg i2, intg i3,
				    intg i4, intg i5, intg i6, intg i7) {
    switch (spec.ndim) {
    case 7: i7 = -1; break ;
    case 6: i6 = -1; i7 = -1; break ;
    case 5: i5 = -1; i6 = -1; i7 = -1; break ;
    case 4: i4 = -1; i5 = -1; i6 = -1; i7 = -1; break ;
    case 3: return get(i0, i1, i2);
    case 2: return get(i0, i1);
    case 1: return get(i0);
    case 0: return get();
    default: break ;
    }
    return *ptr(i0,i1,i2,i3,i4,i5,i6,i7);
  }

  ////////////////////////////////////////////////////////////////
  // set methods

  // set the element of idx0
  template <class T> T idx<T>::set(T val) {
#ifdef __DEBUG__
    idx_checkorder1(*this, 0);
#endif
    return (storage->data)[spec.offset] = val;
  }

  // set the element of idx1
  template <class T> T idx<T>::set(T val, intg i0) {
#ifdef __DEBUG__
    idx_checkorder1(*this, 1);
    if ((i0 < 0) || (i0 >= spec.dim[0]))
      eblerror("index " << i0 << " in dim 0 out of bound in " << *this);
#endif
    return (storage->data)[spec.offset + i0*spec.mod[0]] = val;
  }

  // set the element of idx2
  template <class T> T idx<T>::set(T val, intg i0, intg i1) {
#ifdef __DEBUG__
    idx_checkorder1(*this, 2);
    if ((i0 < 0) || (i0 >= spec.dim[0]))
      eblerror("index " << i0 << " in dim 0 out of bound in " << *this);
    if ((i1 < 0) || (i1 >= spec.dim[1]))
      eblerror("index " << i1 << " in dim 1 out of bound in " << *this);
#endif
    return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1]] = val;
  }

  // set the element of idx3
  template <class T> T idx<T>::set(T val, intg i0, intg i1, intg i2) {
#ifdef __DEBUG__
    idx_checkorder1(*this, 3);
    if ((i0 < 0) || (i0 >= spec.dim[0]))
      eblerror("index " << i0 << " in dim 0 out of bound in " << *this);
    if ((i1 < 0) || (i1 >= spec.dim[1]))
      eblerror("index " << i1 << " in dim 1 out of bound in " << *this);
    if ((i2 < 0) || (i2 >= spec.dim[2]))
      eblerror("index " << i2 << " in dim 2 out of bound in " << *this);
#endif
    return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1]
			   + i2*spec.mod[2]] = val;
  }

  // set an element of an idx of any order.
  template <class T> T idx<T>::set(T val, intg i0, intg i1, intg i2, intg i3,
				   intg i4, intg i5, intg i6, intg i7) {
    return *ptr(i0,i1,i2,i3,i4,i5,i6,i7) = val;
  }

  // get element of an idx of any order
  template <class T> T idx<T>::sset(T val, intg i0, intg i1, intg i2, intg i3,
				    intg i4, intg i5, intg i6, intg i7) {
    switch (spec.ndim) {
    case 7: i7 = -1; break ;
    case 6: i6 = -1; i7 = -1; break ;
    case 5: i5 = -1; i6 = -1; i7 = -1; break ;
    case 4: i4 = -1; i5 = -1; i6 = -1; i7 = -1; break ;
    case 3: return set(val, i0, i1, i2);
    case 2: return set(val, i0, i1);
    case 1: return set(val, i0);
    case 0: return set(val);
    default: break ;
    }
    return *ptr(i0,i1,i2,i3,i4,i5,i6,i7) = val;
  }

  ////////////////////////////////////////////////////////////////
  // print methods

  template <typename T>
  void idx<T>::printElems(std::ostream& out, bool newline) const {
    printElems_impl(0, out, newline);
    out.flush();
  }

  template <typename T>
  void idx<T>::printElems(std::string& out, bool newline) const {
    printElems_impl(0, out, newline);
  }

  template <typename T>
  void idx<T>::printElems() const {
    this->printElems(std::cout);
  }

  template <typename T>
  void idx<T>::print() const {
    this->printElems(std::cout);
    std::cout << "\n";
  }

  template <typename T>
  std::string idx<T>::str() const {
    std::string s;
    this->printElems(s, false);
    return s;
  }

  template<class T> inline T printElems_impl_cast(T val) {
    return val;
  }

  // specialization for ubyte to print as unsigned integers.
  inline unsigned int printElems_impl_cast(ubyte val) {
    return (unsigned int) val;
  }

  template <typename T> template <class stream>
  void idx<T>::printElems_impl(int indent, stream& out, bool newline) const {
    static const std::string lbrace = "[";
    static const std::string rbrace = "]";
    static const std::string sep = " ";
    // prepare indentation
    std::string tab;
    for( unsigned int ii = 0; ii < lbrace.length(); ++ii )
      tab << " ";
    // printing a 0-dimensional tensor
    if (order() == 0)
      out << lbrace << "@" << sep << printElems_impl_cast(get()) <<sep<< rbrace;
    else if (order() == 1) { // printing a 1-D tensor
      out << lbrace << sep;
      for (int ii = 0; ii < dim(0); ++ii)
	out << printElems_impl_cast(get(ii)) <<sep;
      out << rbrace; //<<"\n";
      //if (newline) out << "\n";
    } else { // printing a multidimensional tensor
      out << lbrace; // opening brace
      // print subtensors.
      idx<T> subtensor(storage, spec.offset);
      for(int dimInd = 0; dimInd < dim(0); ++dimInd ){
	// only print indent if this isn't the first subtensor.
	if (dimInd > 0)
	  for(int ii = 0; ii < indent + 1; ++ii) out << tab;
	// print subtensor
	spec.select_into(&subtensor.spec, 0, dimInd);
	subtensor.printElems_impl(indent+1, out, newline);
	// only print the newline if this isn't the last subtensor.
	if (dimInd < dim(0) - 1 && newline) out << "\n";
      }
      out << rbrace; // closing brace
      //if (newline) out << "\n";
    }
  }

  template <class T> void idx<T>::pretty(FILE *f) const {
    fprintf(f,"idx: at address %ld\n",(intg)this);
    fprintf(f,"  storage=%ld (size=%ld)\n",(intg)storage,storage->size());
    spec.pretty(f);
  }

  template <class T> void idx<T>::pretty() const {
    pretty(std::cout);
  }

  template <class T> void idx<T>::pretty(std::ostream& out) const {
    out << "idx: at address " << (intg)this << "\n";
    out << "  storage=" <<  (intg)storage << "(size=" << storage->size();
    out << "\n";
    spec.pretty(out);
  }

  template <class T> int idx<T>::fdump(std::ostream &f) {
    if (spec.ndim == 0)
      f << "[@ " << this->get() << "]" << std::endl;
    else if (spec.ndim == 1) {
      f << "[";
      for (intg i=0; i<dim(0); i += mod(0))
	f << (storage->data)[spec.offset + i] << " ";
      f << "]\n";
    } else {
      f << "[";
      { idx_bloop1(p,*this,T) { p.fdump(f); } }
      f << "]\n";
    }
    return 0;
  }

  // stream printing ///////////////////////////////////////////////////////////

  template <class T>
  std::ostream& operator<<(std::ostream& out, const idx<T>& m) {
    out << m.spec;
    return out;
  }

  template <class T>
  std::string& operator<<(std::string& out, idx<T>& m) {
    out << m.spec;
    return out;
  }

  template <class T>
  std::string& operator<<(std::string& out, idx<T>* m) {
    if (!m)
      out << "null";
    else
      out << m->spec;
    return out;
  }

  template <typename T, class stream>
  stream& operator<<(stream& out, midx<T>& m) {
    idxdim dmin, dmax;
    bool dnull = false, bpos = false;
    if (m.order() == 0) {
      out << "[empty midx]";
      return out;
    }
    idx_aloopf1(e, ((idx<idx<T>*>&) m), idx<T>*, {
	idx<T> *pe = *e;
	if (pe) {
	  idxdim d(*pe);
	  if (d.nelements() < dmin.nelements() || dmin.empty()) dmin = d;
	  if (d.nelements() > dmax.nelements()) dmax = d;
	  bpos = true;
	} else
	  dnull = true;
      });
    out << "[" << (idx<idx<T>*>&) m << ", ";
    if (!bpos)
      out << "all empty";
    else {
      out << "from ";
      if (dnull) out << "null";
      else out << dmin;
      out << " to " << dmax;
    }
    out << "]";
    return out;
  }

//   template <typename T, class stream>
//   stream& operator<<(stream& o, svector<midx<T> >& m) {
//     o << "[ ";
//     for (uint i = 0; i < m.size(); ++i) {
//       midx<T> &e = m[i];
//       o << e << " ";
//     }
//     o << "]";
//     return o;
//   }

  //////////////////////////////////////////////////////////////////////////////

  template <typename T>
  std::ostream& operator<<(std::ostream& out, const idxd<T>& d) {
    std::string s;
    s << d;
    out << s;
    return out;
  }

  template <typename T>
  std::string& operator<<(std::string& out, const idxd<T>& d) {
    if (d.order() <= 0)
      out << "<empty>";
    else {
      if (d.offsets) {
	bool show = false;
	for (int i = 0; i < d.order(); ++i)
	  if (d.offset(i) != 0) {
	    show = true;
	    break;
	  }
	if (show) {
	  out << "(";
	  out << d.offset(0);
	  for (int i = 1; i < d.order(); ++i)
	    out << "," << d.offset(i);
	  out << ")";
	}
      }
      out << d.dim(0);
      for (int i = 1; i < d.order(); ++i)
	out << "x" << d.dim(i);
    }
    return out;
  }

  // midx //////////////////////////////////////////////////////////////////////

  template <typename T>
  midx<T>::midx(intg size, std::file *fp_, idx<int64> *off)
    : idx<idx<T>*>(size), fp(fp_) {
    idx_clear(*this);
    if (fp) {
      if (!off) eblerror("expected an offset matrix");
      offsets = *off;
      fp->incr_ref();
    }
  }

  template <typename T>
  midx<T>::midx(intg size0, intg size1, std::file *fp_, idx<int64> *off)
    : idx<idx<T>*>(size0, size1), fp(fp_) {
    idx_clear(*this);
    if (fp) {
      if (!off) eblerror("expected an offset matrix");
      offsets = *off;
      fp->incr_ref();
    }
  }

  template <typename T>
  midx<T>::midx(idxdim &d, std::file *fp_, idx<int64> *off)
    : idx<idx<T>*>(d), fp(fp_) {
    idx_clear(*this);
    if (fp) {
      if (!off) eblerror("expected an offset matrix");
      offsets = *off;
      fp->incr_ref();
    }
  }

  template <typename T>
  midx<T>::midx() : idx<idx<T>*>(), fp(NULL) {
    idx_clear(*this);
  }

  template <typename T>
  midx<T>::midx(const midx<T> &o)
    //    : idx<idx<T>*>((idx<idx<T>*>&)o), fp(o.fp), offsets(o.offsets) {
    : idx<idx<T>*>(o), fp(o.fp), offsets(o.offsets) {
    if (fp)
      fp->incr_ref(); // signify fp that we're using it
    lock_all();
  }

  template <typename T>
  midx<T>::~midx() {
    unlock_all();
  }

  template <typename T>
  midx<T>& midx<T>::operator=(const midx<T>& other) {
    if (this != &other) { // protect against invalid self-assignment
      unlock_all();
      *((idx<idx<T>*>*)this) = (idx<idx<T>*>&) other;
      fp = other.fp;
      offsets = other.offsets;
      if (fp) fp->incr_ref();
      lock_all();
    }
    return *this;
  }

  template <typename T>
  void midx<T>::remove(intg i0) {
    idx<T> *e = idx<idx<T>*>::get(i0);
    if (e) e->unlock();
    idx<idx<T>*>::set(NULL, i0);
  }

  template <typename T>
  void midx<T>::resize(intg i0) {
    if (this->dim(0) != i0) {
      this->clear();
      idx<idx<T>*>::resize(i0);
      idx_clear(*this);
    }
  }

  template <typename T>
  void midx<T>::resize(intg i0, intg i1) {
    if (this->dim(0) != i0 || this->dim(1) != i1) {
      this->clear();
      idx<idx<T>*>::resize(i0, i1);
      idx_clear(*this);
    }
  }

  template <typename T>
  void midx<T>::resize(midx<T> &other) {
    idx_aloopf2(e, ((idx<idx<T>*>&) *this), idx<T>*,
		o, ((idx<idx<T>*>&) other), idx<T>*, {
		  idx<T> *pe = *e; idx<T> *po = *o;
		  if (pe) pe->resize(po->get_idxdim());
		  else {
		    pe = new idx<T>(po->get_idxdim());
		    pe->lock();
		    *e = pe;
		  }
      });
  }

  template <typename T>
  void midx<T>::remove(intg i0, intg i1) {
    idx<T> *e = idx<idx<T>*>::get(i0, i1);
    if (e) e->unlock();
    idx<idx<T>*>::set(NULL, i0, i1);
  }

  template <typename T>
  void midx<T>::clear() {
    idx_aloopf1(e, ((idx<idx<T>*>&) *this), idx<T>*, {
	idx<T> *pe = *e;
	if (pe) pe->unlock();
	*e = NULL;
      });
    if (fp) {
      fp->decr_ref();
      if (fp->no_references())
	delete fp; // close file
      fp = NULL;
    }
  }

  template <typename T>
  void midx<T>::set(idx<T> &e, intg pos) {
    idx<T> *pe = idx<idx<T>*>::get(pos);
    if (pe) pe->unlock();
    pe = new idx<T>(e);
    pe->lock();
    idx<idx<T>*>::set(pe, pos);
  }

  template <typename T>
  void midx<T>::set(idx<T> &e, intg i0, intg i1) {
    idx<T> *pe = idx<idx<T>*>::get(i0, i1);
    if (pe) pe->unlock();
    pe = new idx<T>(e);
    pe->lock();
    idx<idx<T>*>::set(pe, i0, i1);
  }

  // early definition of load_matrix
  template <typename T> idx<T> load_matrix(FILE *fp, idx<T> *out = NULL);

  template <typename T>
  idx<T> midx<T>::get(intg i0) {
    if (fp) { // on-demand loading
      if (fseek(fp->get_fp(), offsets.get(i0), SEEK_SET)) {
	fseek(fp->get_fp(), 0, SEEK_END);
	fpos_t fppos;
	fgetpos(fp->get_fp(), &fppos);
#if defined(__WINDOWS__) || defined(__MAC__)
	eblerror("fseek to position " << offsets.get(i0) << " failed, "
		 << "file is " << (intg) fppos << " big");
#else
	eblerror("fseek to position " << offsets.get(i0) << " failed, "
		 << "file is " << (intg) fppos.__pos << " big");
#endif
      }
      return load_matrix<T>(fp->get_fp());
    } else { // all data is already loaded
      idx<T> *e = idx<idx<T>*>::get(i0);
      if (!e) eblerror("trying to access null element at position " << i0);
      idx<T> m(*e);
      return m;
    }
  }

  template <typename T>
  idx<T> midx<T>::get(intg i0, intg i1) {
    if (fp) { // on-demand loading
      if (fseek(fp->get_fp(), offsets.get(i0, i1), SEEK_SET)) {
	fseek(fp->get_fp(), 0, SEEK_END);
	fpos_t fppos;
	fgetpos(fp->get_fp(), &fppos);
#if defined(__WINDOWS__) || defined(__MAC__)
	eblerror("fseek to position " << offsets.get(i0, i1) << " failed, "
		 << "file is " << (intg) fppos << " big");
#else
	eblerror("fseek to position " << offsets.get(i0, i1) << " failed, "
		 << "file is " << (intg) fppos.__pos << " big");
#endif
      }
      return load_matrix<T>(fp->get_fp());
    } else { // all data is already loaded
      idx<T> *e = idx<idx<T>*>::get(i0, i1);
      if (!e)
	eblerror("trying to access null element at position ("
		 << i0 << ", " << i1 << ")");
      idx<T> m(*e);
      return m;
    }
  }

  template <typename T>
  midx<T> midx<T>::narrow(int d, intg s, intg o) {
    // if same, return a copy
    if (o == 0 && this->dim(d) == s) {
      midx<T> copy(*this);
      return copy;
    }
    // otherwise narrow it
    idx<idx<T>*> tmp = idx<idx<T>*>::narrow(d, s, o);
    midx<T> r = *this;
    idx<idx<T>*> &rr = ((idx<idx<T>*>&)r);// = (idx<idx<T>*>&) tmp;
    rr = tmp;
    return r;
  }

  template <typename T>
  midx<T> midx<T>::select(int d, intg s) {
    idx<idx<T>*> tmp = idx<idx<T>*>::select(d, s);
    midx<T> r((midx<T>&)tmp);
    return r;
  }

  template <typename T>
  bool midx<T>::exists(intg pos) const {
    if (fp) // on-demand loading
      return offsets.get(pos) != 0; // check that offset is defined
    else { // data already loaded, check if present
      idx<T> *e = idx<idx<T>*>::get(pos);
      return (e != NULL);
    }
  }

  template <typename T>
  bool midx<T>::exists(intg i0, intg i1) const {
    if (fp) // on-demand loading
      return offsets.get(i0, i1) != 0; // check that offset is defined
    else { // data already loaded, check if present
      idx<T> *e = idx<idx<T>*>::get(i0, i1);
      return (e != NULL);
    }
  }

  template <typename T>
  idxdim midx<T>::get_maxdim() {
    if (fp)
      eblerror("get_maxdim should be avoided when loading on-demand"
	       << " because it would required loading all data");
    idxdim dmax;
    for (intg i = 0; i < this->dim(0); ++i) {
      if (exists(i)) {
	idx<T> p = this->get(i);
	idxdim d(p);
	if (d.nelements() > dmax.nelements()) dmax = d;
      }
    }
    return dmax;
  }

  template <typename T>
  idx<int64> midx<T>::get_offsets() {
    return offsets;
  }

  template <typename T>
  std::file *midx<T>::get_file_pointer() {
    return fp;
  }

  template <typename T>
  bool midx<T>::same_dim(const midx<T> &other) const {
    if (this->get_idxdim() != other.get_idxdim()) return false;
    idx_aloopf2(e1, ((idx<idx<T>*>&) *this), idx<T>*,
		e2, ((idx<idx<T>*>&) other), idx<T>*, {
		  idx<T> *pe1 = *e1; idx<T> *pe2 = *e2;
		  if (pe1 != NULL && pe2 != NULL) {
		    if (pe1->get_idxdim() != pe2->get_idxdim())
		      return false;
		  } else return false;
		});
    return true;
  }

  template <typename T>
  intg midx<T>::nelements_all() const {
    intg n = 0;
    idx_aloopf1(e, ((idx<idx<T>*>&) *this), idx<T>*, {
	idx<T> *pe = *e;
	if (pe) n += pe->nelements();
      });
    return n;
  }

  template <typename T>
  idx<T> midx<T>::pack() {
    // determine size of sub-tensor
    idx<T> sub;
    intg o = this->order();
    if (o == 1) sub = this->get(0);
    else if (o == 2) sub = this->get(0, 0);
    else eblerror("not implemented");
    idxdim d = this->get_idxdim();
    idxdim dsub = sub;
    for (uint i = 0; i < dsub.order(); ++i)
      d.insert_dim(d.order(), dsub.dim(i));
    // allocate target tensor
    idx<T> all(d);
    // empty it
    idx_clear(all);
    // loop on all sub-tensors and copy them
    if (o == 1) {
      for (intg i = 0; i < this->dim(0); ++i) {
	idx<T> tmp = this->get(i);
	if (tmp.get_idxdim() != dsub)
	  eblerror("can't pack midx containing variable sub-tensor dimensions");
	idx<T> tgt = all.select(0, i);
	idx_copy(tmp, tgt);
      }
    } else if (o == 2) {
      for (intg i = 0; i < this->dim(0); ++i) {
	idx<T> suball = all.select(0, i);
	for (intg j = 0; j < this->dim(1); ++j) {
	  idx<T> tmp = this->get(i, j);
	  if (tmp.get_idxdim() != dsub)
	    eblerror("can't pack midx containing variable "
		     << "sub-tensor dimensions");
	  idx<T> tgt = suball.select(0, j);
	  idx_copy(tmp, tgt);
	}
      }
    } else eblerror("not implemented");
    return all;
  }

  template <typename T>
  void midx<T>::shift_dim_internal(int d, int pos) {
    // loop over all elements
    idx_aloopf1(e, ((idx<idx<T>*>&) *this), idx<T>*, {
      	idx<T> *pe = *e;
	if (pe) *pe = pe->shift_dim(d, pos);
      });
  }

  template <typename T>
  void midx<T>::pretty() const {
    this->pretty(std::cout);
  }

  template <typename T> template <class stream>
  void midx<T>::pretty(stream &out, bool newline) const {
    out << "[ ";
    if (this->order() == 0)
      out << "empty midx";
    else {
      idx_aloopf1(e, ((idx<idx<T>*>&) *this), idx<T>*, {
	  idx<T> *pe = *e;
	  if (pe) {
	    idxdim d(*pe);
	    out << d << " ";
	  } else
	    out << "empty ";
	});
    }
    out << "]";
    if (newline) out << "\n";
  }

  template <typename T>
  std::string midx<T>::str() const {
    std::string s;
    this->pretty(s, false);
    return s;
  }

  // internal methods //////////////////////////////////////////////////////////

  template <typename T>
  void midx<T>::lock_all() {
    idx_aloopf1(e, ((idx<idx<T>*>&) *this), idx<T>*, {
	idx<T> *pe = *e;
	if (pe) pe->lock();
      });
  }

  template <typename T>
  void midx<T>::unlock_all() {
    idx_aloopf1(e, ((idx<idx<T>*>&) *this), idx<T>*, {
	idx<T> *pe = *e;
	if (pe) pe->unlock();
      });
  }

} // namespace ebl

#endif /* IDX_HPP_ */
