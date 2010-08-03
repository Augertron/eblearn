/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun   *
 *   yann@cs.nyu.edu   *
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

// tell header that we are in the libidx scope
#define LIBIDX

#include "idx.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // private methods

  // set the order (number of dimensions).
  // This is used to allocate/deallocate the dim/mod arrays.
  // It is also used to deallocate them by passing a zero argument.
  // Hence, if ndim is reduced, the arrays are preserved.
  int idxspec::setndim(int n) {
    try {
      if ((n<0) || (n>MAXDIMS)) {
	throw("idx: cannot set ndim, ndim < 0 or ndim > MAXDIMS");
      } else {
	// if new ndim is zero or larger than before: deallocate arrays
	if ((n == 0) || (n > ndim)) {
	  if (dim) { delete []dim; }
	  if (mod) { delete []mod; }
	  dim = NULL; mod = NULL;
	}
	// now allocate new arrays if necessary
	if (n > 0) {
	  if (!dim) { dim = new intg[n]; }
	  if (!mod) { mod = new intg[n]; }
	}
	// if the arrays allocated and ndim was larger 
	// than new ndim, we don't do anything.
	ndim = n;
	return ndim;
      }
    }
    catch(const char *s) {
      eblerror(s);
      return -1;
    }
  }

  // set the order (number of dimensions).
  // using pre-allocated mod/dim arrays.
  // Probably not useful.
  int idxspec::setndim(int n, intg *ldim, intg *lmod) {
    try {
      if ((n<1) || (n>=MAXDIMS)) { 
	throw("idx: cannot set ndim, ndim < 0 or ndim > MAXDIMS");
      } else {
	if (dim) { delete []dim; }
	if (mod) { delete []mod; }
	dim = ldim;
	mod = lmod;
	ndim = n;
	return ndim;
      }
    }
    catch(const char *s) {
      eblerror(s);
      return -1;
    }
  }

  //intg idxspec::resize( intg* dimsBegin, intg* dimsEnd ){
  //	
  //	const int nArgDims = std::distance(dimsBegin, dimsEnd);
  //
  //	// Error-check the supplied number of dims.
  //	if( ndim == 0 ){
  //		eblerror("Cannot call resize on a 0-dimensional idxspec.");	
  //	}
  //	else if( ndim != nArgDims ){
  //		std::ostringstream oss;
  //		oss<<"Number of supplied dimension sizes ("<<nArgDims;
  //            oss<<") doesn't match idxspec's number of dims ("<<ndim<<")";
  //		eblerror(oss.str().c_str());
  //	}
  //	
  //	// copy dimensions to dim
  //	std::copy(dimsBegin, dimsEnd, dim);
  //	
  //	// set mod to be the partial sum of the dim sequence, in reverse order.
  //	typedef std::reverse_iterator<intg*> RIter;
  //	std::partial_sum(RIter(dimsEnd-1), RIter(dimsBegin-1), 
  //                     RIter(mod+(nArgDims-1)), std::multiplies<intg>());
  //
  //	// return the memory footprint
  //	return mod[0] * dim[0] + offset;
  //}

  // resizing: order is not allowed to change
  intg idxspec::resize(intg s0, intg s1, intg s2, intg s3, 
		       intg s4, intg s5, intg s6, intg s7) {
    intg md = 1;
    try {
      // resizeing non-contiguous is forbiden to prevent nasty bugs
      if (!contiguousp()) throw(42);
      if (ndim==0) { throw(0); }  // can't resize idx0
      if (s7>=0) { 
	if (ndim<8) throw(8); 
	dim[7] = s7; mod[7] = md; md *= s7; 
      } else { if (ndim>7) throw(-8); }
      if (s6>=0) { 
	if (ndim<7) throw(7); 
	dim[6] = s6; mod[6] = md; md *= s6; 
      } else { if (ndim>6) throw(-7); }
      if (s5>=0) { 
	if (ndim<6) throw(6); 
	dim[5] = s5; mod[5] = md; md *= s5; 
      } else { if (ndim>5) throw(-6); }
      if (s4>=0) { 
	if (ndim<5) throw(5); 
	dim[4] = s4; mod[4] = md; md *= s4; 
      } else { if (ndim>4) throw(-5); }
      if (s3>=0) { 
	if (ndim<4) throw(4); 
	dim[3] = s3; mod[3] = md; md *= s3; 
      } else { if (ndim>3) throw(-4); }
      if (s2>=0) { 
	if (ndim<3) throw(3); 
	dim[2] = s2; mod[2] = md; md *= s2; 
      } else { if (ndim>2) throw(-3); }
      if (s1>=0) { 
	if (ndim<2) throw(2); 
	dim[1] = s1; mod[1] = md; md *= s1; 
      } else { if (ndim>1) throw(-2); }
      if (s0>=0) { 
	if (ndim<1) throw(1); 
	dim[0] = s0; mod[0] = md; md *= s0; 
      } else { if (ndim>0) throw(-1); }
    }
    catch(int v) { 
      if (v == 42) {
	eblerror("Resizing non-contiguous idx is not allowed"); 
      }
      else {
	cerr << "error while trying to resize from (";
	if (ndim > 0)
	  cerr << dim[0];
	for (int i = 1; i < ndim; ++i)
	  cerr << " x " << dim[i];
	cerr << ") to (";
	cerr << s0 << " x " << s1 << " x " << s2 << " x " << s3 << " x " << s4;
	cerr << " x " << s5 << " x " << s6 << " x " << s7 << ")." << endl;
	eblerror("idxspec::resize: dimensions are incompatible"); 
      }
    }
    return md + offset; // return new footprint
  }

  intg idxspec::resize(const idxdim &d) {
    return resize(d.dim(0), d.dim(1), d.dim(2), d.dim(3), 
		  d.dim(4), d.dim(5), d.dim(6), d.dim(7)); 
  }

  // resize one dimension <dimn> with size <size>.
  // only already allocated dimensions can be resized 
  // (order is not allowed to change)
  intg idxspec::resize1(intg dimn, intg size) {
    // resizeing non-contiguous is forbiden to prevent nasty bugs
    if (!contiguousp()) eblerror("Resizing non-contiguous idx is not allowed"); 
    if ((dimn >= ndim) || (dimn < 0)) 
      eblerror("idxspec::resize1: cannot resize an unallocated dimension");
    if (size < 0)
      eblerror("idxspec::resize1: cannot resize with a negative size");
    // since we know the current spec is valid, no need for error checking,
    // simply assign new dimension and propagate new mods.
    dim[dimn] = size;
    for (int i = dimn - 1; i >= 0; --i) {
      mod[i] = dim[i + 1] * mod[i + 1];
    }
    return mod[0] * dim[0] + offset; // return new footprint
  }
  
  ////////////////////////////////////////////////////////////////
  // public methods

  // destructor: deletes dim/mod arrays
  idxspec::~idxspec() {
    DEBUG("idxspec::~idxspec: %ld\n",(intg)this);
    setndim(0);
  }

  // assignment operators: copies new dim/mod arrays
  const idxspec& idxspec::operator=(const idxspec &src) {
    if (this != &src) { copy(src); }
    return *this;
  }

  // copy method: this allocates new dim/mod arrays
  // and copies them from original
  void idxspec::copy( const idxspec &src) {
    DEBUG("idxspec::copy: %ld\n",(intg)this);
    offset = src.offset;
    // we do not initialize ndim before setndim here because it may already 
    // be initialized.
    setndim(src.ndim);
    if (ndim > 0) {
      memcpy(dim, src.dim, ndim * sizeof(intg));
      memcpy(mod, src.mod, ndim * sizeof(intg));
    }
  }

  // copy constructor
  idxspec::idxspec( const idxspec& src) : ndim(0), dim(NULL), mod(NULL)
  { copy(src); }

  // constructor for idx0 with offset 0.
  // Can be used to build an empty/blank idx.
  idxspec::idxspec() {
    ndim = 0;
    offset = 0;
    dim = NULL; mod = NULL;
  }

  //idxspec::idxspec( const idxspec& other )
  //	:ndim(other.ndim),
  //	 offset(other.offset),
  //	 dim(NULL),
  //	 mod(NULL)
  //{
  //	setndim(ndim)
  //	if(other.mod != NULL){
  //		std::copy(other.mod, other.mod+other.ndim, mod);
  //	}
  //	if( other.dim != NULL ){
  //		std::copy(other.dim, other.dim+other.ndim, dim);
  //	}
  //}

  // constructor for idx0 with offset
  idxspec::idxspec(intg o) {
    ndim = 0;
    offset = o;
    dim = NULL; mod = NULL;
  }

  // constructor for idx1
  idxspec::idxspec(intg o, intg size0) {
    if ( size0 < 0) { 
      eblerror("negative dimension"); }
    dim = NULL; mod = NULL;
    offset = o;
    ndim = 0; // required in constructors to avoid side effects in setndim
    setndim(1);
    dim[0] = size0;
    mod[0] = 1;
  }

  // constructor for idx2
  idxspec::idxspec(intg o, intg size0, intg size1) {
    if ( (size0<0)||(size1<0)) { 
      eblerror("negative dimension"); }
    dim = NULL; mod = NULL;
    offset = o;
    ndim = 0; // required in constructors to avoid side effects in setndim
    setndim(2);
    dim[0] = size0;
    mod[0] = size1;
    dim[1] = size1;
    mod[1] = 1;
  }

  // constructor for idx3
  idxspec::idxspec(intg o, intg size0, intg size1, intg size2) {
    if ( (size0<0)||(size1<0)||(size2<0)) { 
      eblerror("negative dimension"); }
    dim = NULL; mod = NULL;
    offset = o;
    ndim = 0; // required in constructors to avoid side effects in setndim
    setndim(3);
    dim[0] = size0;
    mod[0] = size1 * size2;
    dim[1] = size1;
    mod[1] = size2;
    dim[2] = size2;
    mod[2] = 1;
  }

  // generic constructor for any dimension.
  idxspec::idxspec(intg o,
		   intg s0, intg s1, intg s2, intg s3, 
		   intg s4, intg s5, intg s6, intg s7) {
    init_spec(o, s0, s1, s2, s3, s4, s5, s6, s7);
  }

  // generic constructor for any dimension.
  void idxspec::init_spec(intg o, intg s0, intg s1, intg s2, intg s3, 
			  intg s4, intg s5, intg s6, intg s7) {
    bool ndimset = false;
    intg md = 1;
    dim = NULL; mod = NULL;
    offset = o;
    ndim = 0; // required in constructors to avoid side effects in setndim
    try {
      if (s7>=0) { 
	if (!ndimset) { setndim(8); ndimset = true; } 
	dim[7] = s7; mod[7] = md; md *= s7; 
      } else { if (ndimset) { throw(-8); } }
      if (s6>=0) { 
	if (!ndimset) { setndim(7); ndimset = true; } 
	dim[6] = s6; mod[6] = md; md *= s6; 
      } else { if (ndimset) { throw(-7); } }
      if (s5>=0) { 
	if (!ndimset) { setndim(6); ndimset = true; } 
	dim[5] = s5; mod[5] = md; md *= s5; 
      } else { if (ndimset) { throw(-6); } }
      if (s4>=0) { 
	if (!ndimset) { setndim(5); ndimset = true; } 
	dim[4] = s4; mod[4] = md; md *= s4; 
      } else { if (ndimset) { throw(-5); } }
      if (s3>=0) { 
	if (!ndimset) { setndim(4); ndimset = true; } 
	dim[3] = s3; mod[3] = md; md *= s3; 
      } else { if (ndimset) { throw(-4); } }
      if (s2>=0) { 
	if (!ndimset) { setndim(3); ndimset = true; } 
	dim[2] = s2; mod[2] = md; md *= s2; 
      } else { if (ndimset) { throw(-3); } }
      if (s1>=0) { 
	if (!ndimset) { setndim(2); ndimset = true; } 
	dim[1] = s1; mod[1] = md; md *= s1; 
      } else { if (ndimset) { throw(-2); } }
      if (s0>=0) { 
	if (!ndimset) { setndim(1); ndimset = true; } 
	dim[0] = s0; mod[0] = md; md *= s0; 
      } else { if (ndimset) { throw(-1); } }
      if (!ndimset) { setndim(0); }
    }
    catch(int v) {
      cerr << "bad dimensions (error " << v << "): " 
	   << s0 << "x" << s1 << "x" << s2 << "x" << s3;
      cerr << "x" << s4 << "x" << s5 << "x" << s6 << "x" << s7 << endl;
      eblerror("idxspec: bad dimensions in constructor"); }
  }

  idxspec::idxspec(intg o, const idxdim &d) {
    init_spec(o, d.dim(0), d.dim(1), d.dim(2), d.dim(3), d.dim(4), d.dim(5), 
	      d.dim(6), d.dim(7));
  }

  // generic constructor for any dimension.
  // The dim and mod arrays past as argument are copied.
  idxspec::idxspec(intg o, int n, intg *ldim, intg *lmod) {
    DEBUG("idxspec::idxspec: %ld\n",(intg)this);
    dim = NULL; mod = NULL;
    offset = o;
    ndim = 0; // required in constructors to avoid side effects in setndim
    setndim(n);
    for (int i = 0; i < n; i++) { 
      if (ldim[i] < 0) eblerror("negative dimension");
      dim[i] = ldim[i]; mod[i] = lmod[i]; 
    }
  }

  intg idxspec::footprint()  {
    intg r = offset + 1;
    for(int i=0; i<ndim; i++){ r += mod[i]*(dim[i]-1); }
    return r;
  }

  //! total number of elements accessed by idxspec
  intg idxspec::nelements() const {
    intg r = 1;
    for(int i=0; i<ndim; i++){ r *= dim[i]; }
    return r;
  }

  bool idxspec::contiguousp() const {
    intg size = 1; bool r = true;
    for(int i=ndim-1; i>=0; i--){
      if (size != mod[i]) r = false;
      size *= dim[i];
    }
    return r;
  }
  
  ////////////////////////////////////////////////////////////////

  // pretty print 
  void idxspec::pretty(FILE *f) {
    int i;
    fprintf(f,"  idxspec %ld\n",(intg)this);
    fprintf(f,"    ndim=%d\n",ndim);
    fprintf(f,"    offset=%ld\n",offset);
    if (ndim>0) {
      fprintf(f,"    dim=[ "); 
      for (i=0; i<ndim-1; i++){ fprintf(f,"%ld, ",dim[i]); }
      fprintf(f,"%ld]\n",dim[ndim-1]); 
      fprintf(f,"    mod=[ "); 
      for (i=0; i<ndim-1; i++){ fprintf(f,"%ld, ",mod[i]); }
      fprintf(f,"%ld]\n",mod[ndim-1]);
    } else {
      fprintf(f,"    dim = %ld, mod = %ld\n",(intg)dim, (intg)mod);
    }
    fprintf(f,"    footprint= %ld\n",footprint());
    fprintf(f,"    contiguous= %s\n",(contiguousp())?"yes":"no");
  }

  void idxspec::pretty(std::ostream& out) {
    int i;
    out << "  idxspec " << (intg)this << "\n";
    out << "    ndim= " << ndim << "\n";
    out << "    offset= " << offset << "\n";
    if (ndim>0) {
      out << "    dim=[ ";
      for (i=0; i<ndim-1; i++){ out << dim[i] << ", "; }
      out << dim[ndim-1] << "]\n"; 
      out << "    mod=[ "; 
      for (i=0; i<ndim-1; i++){ out << mod[i] << ", "; }
      out << mod[ndim-1] << "]\n";
    } else {
      out << "    dim = " << (intg)dim << ", mod = " << (intg)mod <<"\n";
    }
    out << "    footprint= " << footprint() << "\n";
    out << "    contiguous= " << ((contiguousp())? "yes":"no") << "\n";
  }
  ////////////////////////////////////////////////////////////////
  // select, narrow, unfold, etc
  // Each function has 3 version: 
  // 1. XXX_into: which writes the result
  // into an existing idxspec apssed as argument.
  // 2. XXX_inplace: writes into the current idxspec
  // 3. XXX: creates a new idxspec and returns it.

  intg idxspec::select_into(idxspec *dst, int d, intg n) {
    if (ndim <= 0) eblerror("cannot select a scalar");
    if ((n < 0) || (n >= dim[d])) {
      cerr << "error: trying to select layer " << n;
      cerr << " of dimension " << d << " which is of size ";
      cerr << dim[d] << " in idx " << *this << "." << endl;
      eblerror("idx::select error");
    }
    // this preserves the dim/mod arrays if dst == this
    dst->setndim(ndim-1);
    dst->offset = offset + n * mod[d];
    if (ndim -1 > 0) { // dim and mod don't exist for idx0
      for (int j=0; j<d; j++) {
	dst->dim[j] = dim[j];
	dst->mod[j] = mod[j];
      }
      for (int j=d; j<ndim-1; j++) {
	dst->dim[j] = dim[j+1];
	dst->mod[j] = mod[j+1];
      }
    }
    return n;
  }

  intg idxspec::select_inplace(int d, intg n) {
    return select_into(this, d, n);
  }

  idxspec idxspec::select(int d, intg n) {
    // create new idxspec of order ndim-1
    idxspec r;
    select_into(&r, d, n);
    return r;
  }
  
  ////////////////////////////////////////////////////////////////

  intg idxspec::narrow_into(idxspec *dst, int d, intg s, intg o) {
    try {
      if (ndim <= 0) throw("cannot narrow a scalar");
      if ((d < 0) || (d>=ndim)) throw("narrow: illegal dimension index");
      if ((o < 0)||(s < 1)||(s+o > dim[d])) {
	cerr << "trying to narrow dimension " << d << " to size " << s;
	cerr << " starting at offset " << o << " (dimension " << d << " is ";
	cerr << dim[d] << " large)." << endl;
	throw("narrow: illegal size/offset");
      }
    }
    catch(const char *s) { eblerror(s); return -1;}
    // this preserves the dim/mod arrays if dst == this
    dst->setndim(ndim);
    dst->offset = offset + o * mod[d];
    for (int j=0; j<ndim; j++) {
      dst->dim[j] = dim[j];
      dst->mod[j] = mod[j];
    }
    dst->dim[d] = s;
    return s;
  }

  intg idxspec::narrow_inplace(int d, intg s, intg o) {
    return narrow_into(this, d, s, o);
  }

  idxspec idxspec::narrow(int d, intg s, intg o) {
    // create new idxspec of order ndim
    idxspec r;
    narrow_into(&r, d, s, o);
    return r;
  }

  ////////////////////////////////////////////////////////////////
  // transpose

  // tranpose two dimensions into pre-existing idxspec
  int idxspec::transpose_into(idxspec *dst, int d1, int d2) {
    if ((d1 < 0) || (d1 >= ndim) || (d2 < 0) || (d2 >= ndim)) {
      ostringstream err;
      err << "illegal transpose of dimension " << d1
	  << " to dimension " << d2;
      throw err.str();
    }
    // this preserves the dim/mod arrays if dst == this
    dst->setndim(ndim);
    dst->offset = offset;
    for (int j=0; j<ndim; j++) {
      dst->dim[j] = dim[j];
      dst->mod[j] = mod[j];
    }
    intg tmp;
    // we do this in case dst = this
    tmp=dim[d1]; dst->dim[d1]=dim[d2]; dst->dim[d2]=tmp;
    tmp=mod[d1]; dst->mod[d1]=mod[d2]; dst->mod[d2]=tmp;
    return ndim;
  }

  // tranpose all dims with a permutation vector
  int idxspec::transpose_into(idxspec *dst, int *p) {
    for (int i=0; i<ndim; i++) {
      if ((p[i] < 0) || (p[i] >= ndim)) {
	ostringstream err;
	err << "illegal transpose of dimensions";
	throw err.str();
      }
    }
    dst->setndim(ndim);
    dst->offset = offset;
    if (dst == this) {
      // we need temp storage if done in place
      intg tmpdim[MAXDIMS], tmpmod[MAXDIMS];
      for (int j=0; j<ndim; j++) {
	tmpdim[j] = dim[p[j]];
	tmpmod[j] = mod[p[j]];
      }
      for (int j=0; j<ndim; j++) {
	dst->dim[j] = tmpdim[j];
	dst->mod[j] = tmpdim[j];
      }
    } else {
      // not in place
      for (int j=0; j<ndim; j++) {
	dst->dim[j] = dim[p[j]];
	dst->mod[j] = mod[p[j]];
      }
    }
    return ndim;
  }

  int idxspec::transpose_inplace(int d1, int d2) {
    return transpose_into(this, d1, d2);
  }

  int idxspec::transpose_inplace(int *p) {
    return transpose_into(this, p);
  }

  idxspec idxspec::transpose(int d1, int d2) {
    idxspec r;
    transpose_into(&r, d1, d2);
    return r;
  }

  idxspec idxspec::transpose(int *p) {
    idxspec r;
    transpose_into(&r, p);
    return r;
  }

  ////////////////////////////////////////////////////////////////
  // unfold

  // d: dimension; k: kernel size; s: stride.
  intg idxspec::unfold_into(idxspec *dst, int d, intg k, intg s) {
    intg ns; // size of newly created dimension
    try {
      if (ndim <= 0) throw("cannot unfold an idx of maximum order");
      if ((d < 0) || (d>=ndim)) throw("unfold: illegal dimension index");
      if ((k < 1) || (s < 1)) throw("unfold: kernel and stride must be >= 1");
      ns = 1+ (dim[d]-k)/s;
      if ((ns <= 0) || ( dim[d] != s*(ns-1)+k )) 
	throw("unfold: kernel and stride incompatible with size");
    }
    catch(const char *err) {
      cerr << "error: unfolding dimension " << d << " to size " << k;
      cerr << " with step " << s << " from idx " << *this << " into idx ";
      cerr << *dst << endl;
      eblerror(err);
    }
    // this preserves the dim/mod arrays if dst == this
    dst->setndim(ndim+1);
    dst->offset = offset;
    for (int i=0; i<ndim; i++) {
      dst->dim[i] = dim[i];
      dst->mod[i] = mod[i];
    }
    dst->dim[ndim] = k;
    dst->mod[ndim] = mod[d];
    dst->dim[d] = ns;
    dst->mod[d] = mod[d]*s;
    return ns;
  }

  intg idxspec::unfold_inplace(int d, intg k, intg s) {
    return unfold_into(this, d, k, s);
  }

  idxspec idxspec::unfold(int d, intg k, intg s) {
    idxspec r;
    unfold_into(&r, d, k, s);
    return r;
  }

  ////////////////////////////////////////////////////////////////

  // return true if two idxspec have the same dimensions,
  // i.e. if all their dimensions are equal (regardless of strides).
  bool same_dim(idxspec &s1, idxspec &s2) {
    if ( s1.ndim != s2.ndim ) return false; 
    for (int i=0; i<s1.ndim; i++) { if (s1.dim[i] != s2.dim[i]) return false; }
    return true;
  }

  ////////////////////////////////////////////////////////////////
  // idxdim: constructors
    
  idxdim::~idxdim() {
  }

  idxdim::idxdim() {
    ndim = -1;
    memset(dims, -1, MAXDIMS * sizeof (intg));
  }
  
  idxdim::idxdim(const idxspec &s) {
    setdims(s);
  }

  idxdim::idxdim(const idxdim &s) {
    setdims(s);
  }

  idxdim::idxdim(intg s0, intg s1, intg s2, intg s3,
		 intg s4, intg s5, intg s6, intg s7) {
    dims[0] = s0; dims[1] = s1; dims[2] = s2; dims[3] = s3;
    dims[4] = s4; dims[5] = s5; dims[6] = s6; dims[7] = s7;
    ndim = 0;
    for (int i=0; i<8; i++)
      if (dims[i] >= 0) ndim++;
      else break;
  }

  intg idxdim::order() const {
    return ndim;
  }

  ////////////////////////////////////////////////////////////////
  // idxdim: set dimensions
     
  void idxdim::setdims(const idxspec &s) {
    ndim = s.ndim;
    memcpy(dims, s.dim, s.ndim * sizeof (intg)); // copy input dimensions
    // set remaining to -1
    memset(dims + s.ndim, -1, (MAXDIMS - s.ndim) * sizeof (intg));
  }
  
  void idxdim::setdims(const idxdim &s) {
    ndim = s.order();
    for (int i = 0; i < s.order(); ++i)
      dims[i] = s.dim(i);
    // set remaining to -1
    memset(dims + s.order(), -1, (MAXDIMS - s.order()) * sizeof (intg)); 
  }

  void idxdim::setdims(intg n) {
    for (int i = 0; i < ndim; ++i)
      dims[i] = n;
  }
  
  bool idxdim::insert_dim(intg dim_size, uint pos) {
    if (ndim + 1 > MAXDIMS) {
      cerr << "error: cannot add another dimension to dim.";
      cerr << " Maximum number of dimensions (" << MAXDIMS << ") reached.";
      cerr << endl;
      eblerror("maximum order reached.");
      return false;
    }
    // check that dim_size is valid
    if (dim_size <= 0)
      eblerror("cannot set negative or zero dimension");
    // check that all dimensions up to pos (excluded) are > 0.
    for (uint i = 0; i < pos; ++i)
      if (dims[i] <= 0) {
	cerr << "error: cannot insert dimension " << pos
	     << " after empty dimensions: " << *this << endl;
	eblerror("empty dimensions before new dimension to insert");
      }
    // add order of 1
    ndim++;
    if (ndim == 0) // one more if it was empty
      ndim++;
    // shift all dimensions until position pos
    for (uint i = ndim - 1; i > pos && i >= 1; i--)
      dims[i] = dims[i - 1];
    dims[pos] = dim_size;
    return true;
  }
 
  void idxdim::setdim(intg dimn, intg size) {
    if (dimn >= ndim) {
      cerr << "error: trying to set dimension " << dimn << " to size ";
      cerr << size << " but idxidm has only " << ndim;
      cerr << " dimension(s)." << endl;
      eblerror("cannot change the order of idxdim");
    }
    dims[dimn] = size; 
  }

  intg idxdim::dim(intg dimn) const {
    if (dimn >= MAXDIMS) {
      cerr << "trying to access size of dimension " << dimn;
      cerr << " but idxdim's maximum dimensions is " << MAXDIMS << "." << endl;
      eblerror("dimension error");
    }
    return dims[dimn]; 
  }

  bool idxdim::operator==(const idxdim& other) {
    if (other.ndim != ndim)
      return false;
    for (int i = 0; i < ndim; ++i)
      if (other.dim(i) != dim(i))
	return false;
    return true;
  }
  
  bool idxdim::operator!=(const idxdim& other) {
    return !(*this == other);
  }
  
  std::ostream& operator<<(std::ostream& out, const idxdim& d) {
    if (d.order() <= 0)
      out << "<empty>";
    else {
      out << d.dim(0);
      for (int i = 1; i < d.order(); ++i)
	out << "x" << d.dim(i);
    }
    return out;
  }

  std::ostream& operator<<(std::ostream& out, idxspec& d) {
    if (d.getndim() <= 0)
      out << "<empty>";
    else {
      out << d.dim[0];
      for (int i = 1; i < d.getndim(); ++i)
	out << "x" << d.dim[i];
    }
    return out;
  }

  intg idxdim::nelements() {
    intg total = 1;
    for (int i = 0; i < ndim; ++i)
      total *= dim(i);
    return total;
  }

  ////////////////////////////////////////////////////////////////
  // rect

  rect::rect(uint h0_, uint w0_, uint height_, uint width_)
    : h0(h0_), w0(w0_), height(height_), width(width_) {
  }
  
  rect::rect() {
  }
  
  rect::rect(const rect &r)
  : h0(r.h0), w0(r.w0), height(r.height), width(r.width) {
  }

  rect::~rect() {
  }

  bool rect::overlap(const rect &r) {
    if (((h0 <= r.h0 + r.height) && (h0 + height >= r.h0)) &&
	((w0 <= r.w0 + r.width) && (w0 + width >= r.w0)))
      return true;
    return false;
  }
  
  bool rect::max_overlap(const rect &r, float hmax, float wmax) {
    if (((h0 <= r.h0 + r.height) && (h0 + height >= r.h0)) &&
	((w0 <= r.w0 + r.width) && (w0 + width >= r.w0))) {
      // there is overlap, now check how much is authorized.
      uint hoverlap = std::min(h0 + height, r.h0 + r.height)
	- std::max(h0, r.h0);
      uint woverlap = std::min(w0 + width, r.w0 + r.width) - std::max(w0, r.w0);
      float hratio = hoverlap / (float) std::min(height, r.height);
      float wratio = woverlap / (float) std::min(width, r.width);
      if (hratio >= hmax && wratio >= wmax)
	return true;
    }
    return false;
  }
  
  bool rect::is_within(const rect &r) {
    if (((h0 >= r.h0) && (h0 + height <= r.h0 + r.height)) &&
	((w0 >= r.w0) && (w0 + width <= r.w0 + r.width)))
      return true;
    return false;
  }
  
  std::ostream& operator<<(std::ostream& out, const rect& r) {
    out << "rect:<(" << r.h0 << "," << r.w0 << ")," << r.height;
    out << "x" << r.width << ">";
    return out;
  }

  rect operator/(const rect& r, double d) {
    rect newr((uint) (r.h0 / d), (uint) (r.w0 / d),
	      (uint) (r.height / d), (uint) (r.width / d));
    return newr;
  }

  rect operator*(const rect& r, double d) {
    rect newr((uint) (r.h0 * d), (uint) (r.w0 * d),
	      (uint) (r.height * d), (uint) (r.width * d));
    return newr;
  }

} // end namespace ebl
