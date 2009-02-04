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

#include "Idx.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // private methods

  // set the order (number of dimensions).
  // This is used to allocate/deallocate the dim/mod arrays.
  // It is also used to deallocate them by passing a zero argument.
  // Hence, if ndim is reduced, the arrays are preserved.
  int IdxSpec::setndim(int n) {
    try {
      if ((n<0) || (n>MAXDIMS)) {
	throw("Idx: cannot set ndim");
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
    catch(char *s) {
      ylerror(s);
      return -1;
    }
  }

  // set the order (number of dimensions).
  // using pre-allocated mod/dim arrays.
  // Probably not useful.
  int IdxSpec::setndim(int n, intg *ldim, intg *lmod) {
    try {
      if ((n<1) || (n>=MAXDIMS)) { 
	throw("Idx: cannot set ndim"); 
      } else {
	if (dim) { delete []dim; }
	if (mod) { delete []mod; }
	dim = ldim;
	mod = lmod;
	ndim = n;
	return ndim;
      }
    }
    catch(char *s) {
      ylerror(s);
      return -1;
    }
  }

  //intg IdxSpec::resize( intg* dimsBegin, intg* dimsEnd ){
  //	
  //	const int nArgDims = std::distance(dimsBegin, dimsEnd);
  //
  //	// Error-check the supplied number of dims.
  //	if( ndim == 0 ){
  //		ylerror("Cannot call resize on a 0-dimensional IdxSpec.");	
  //	}
  //	else if( ndim != nArgDims ){
  //		std::ostringstream oss;
  //		oss<<"Number of supplied dimension sizes ("<<nArgDims;
  //            oss<<") doesn't match IdxSpec's number of dims ("<<ndim<<")";
  //		ylerror(oss.str().c_str());
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
  intg IdxSpec::resize(intg s0, intg s1, intg s2, intg s3, 
		       intg s4, intg s5, intg s6, intg s7) {
    intg md = 1;
    try {
      if (ndim==0) { throw(0); }  // can't resize Idx0
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
      fprintf(stderr,"ndim=%d, sizes: %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld\n",
	      ndim,s0,s1,s2,s3,s4,s5,s6,s6);
      ylerror("IdxSpec::resize: dimensions are incompatible"); }
    return md + offset;
  }

  ////////////////////////////////////////////////////////////////
  // public methods

  // destructor: deletes dim/mod arrays
  IdxSpec::~IdxSpec() {
    DEBUG("IdxSpec::~IdxSpec: %ld\n",(intg)this);
    setndim(0);
  }

  // assignment operators: copies new dim/mod arrays
  const IdxSpec& IdxSpec::operator=(const IdxSpec &src) {
    if (this != &src) { copy(src); }
    return *this;
  }

  // copy method: this allocates new dim/mod arrays
  // and copies them from original
  void IdxSpec::copy( const IdxSpec &src) {
    DEBUG("IdxSpec::copy: %ld\n",(intg)this);
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
  IdxSpec::IdxSpec( const IdxSpec& src) : ndim(0), dim(NULL), mod(NULL)
  { copy(src); }

  // constructor for Idx0 with offset 0.
  // Can be used to build an empty/blank Idx.
  IdxSpec::IdxSpec() {
    ndim = 0;
    offset = 0;
    dim = NULL; mod = NULL;
  }

  //IdxSpec::IdxSpec( const IdxSpec& other )
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

  // constructor for Idx0 with offset
  IdxSpec::IdxSpec(intg o) {
    ndim = 0;
    offset = o;
    dim = NULL; mod = NULL;
  }

  // constructor for Idx1
  IdxSpec::IdxSpec(intg o, intg size0) {
    if ( size0 < 0) { 
      ylerror("negative dimension"); }
    dim = NULL; mod = NULL;
    offset = o;
    ndim = 0; // required in constructors to avoid side effects in setndim
    setndim(1);
    dim[0] = size0;
    mod[0] = 1;
  }

  // constructor for Idx2
  IdxSpec::IdxSpec(intg o, intg size0, intg size1) {
    if ( (size0<0)||(size1<0)) { 
      ylerror("negative dimension"); }
    dim = NULL; mod = NULL;
    offset = o;
    ndim = 0; // required in constructors to avoid side effects in setndim
    setndim(2);
    dim[0] = size0;
    mod[0] = size1;
    dim[1] = size1;
    mod[1] = 1;
  }

  // constructor for Idx3
  IdxSpec::IdxSpec(intg o, intg size0, intg size1, intg size2) {
    if ( (size0<0)||(size1<0)||(size2<0)) { 
      ylerror("negative dimension"); }
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
  IdxSpec::IdxSpec(intg o,
		   intg s0, intg s1, intg s2, intg s3, 
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
    catch(int v) { ylerror("IdxSpec: bad dimensions in constructor"); }
  }

  // generic constructor for any dimension.
  // The dim and mod arrays past as argument are copied.
  IdxSpec::IdxSpec(intg o, int n, intg *ldim, intg *lmod) {
    DEBUG("IdxSpec::IdxSpec: %ld\n",(intg)this);
    dim = NULL; mod = NULL;
    offset = o;
    ndim = 0; // required in constructors to avoid side effects in setndim
    setndim(n);
    for (int i=0; i<n; i++) { 
      if ( dim[i]<0 ) { ylerror("negative dimension"); }
      dim[i] = ldim[i]; mod[i] = lmod[i]; }
  }



  ////////////////////////////////////////////////////////////////

  // pretty print 
  void IdxSpec::pretty(FILE *f) {
    int i;
    fprintf(f,"  IdxSpec %ld\n",(intg)this);
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

  void IdxSpec::pretty(std::ostream& out) {
    int i;
    out << "  IdxSpec " << (intg)this << "\n";
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
  // into an existing IdxSpec apssed as argument.
  // 2. XXX_inplace: writes into the current IdxSpec
  // 3. XXX: creates a new IdxSpec and returns it.

  intg IdxSpec::select_into(IdxSpec *dst, int d, intg n) {
    try {
      if (ndim <= 0) throw("cannot select a scalar");
    }
    catch(const char *s) { ylerror(s); return -1;}
    // this preserves the dim/mod arrays if dst == this
    dst->setndim(ndim-1);
    dst->offset = offset + n * mod[d];
    if (ndim -1 > 0) { // dim and mod don't exist for Idx0
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

  intg IdxSpec::select_inplace(int d, intg n) {
    return select_into(this, d, n);
  }

  IdxSpec IdxSpec::select(int d, intg n) {
    // create new IdxSpec of order ndim-1
    IdxSpec r;
    select_into(&r, d, n);
    return r;
  }
  
  ////////////////////////////////////////////////////////////////

  intg IdxSpec::narrow_into(IdxSpec *dst, int d, intg s, intg o) {
    try {
      if (ndim <= 0) throw("cannot narrow a scalar");
      if ((d < 0) || (d>=ndim)) throw("narrow: illegal dimension index");
      if ((o < 0)||(s < 1)||(s+o > dim[d])) 
	throw("narrow: illegal size/offset");
    }
    catch(const char *s) { ylerror(s); return -1;}
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

  intg IdxSpec::narrow_inplace(int d, intg s, intg o) {
    return narrow_into(this, d, s, o);
  }

  IdxSpec IdxSpec::narrow(int d, intg s, intg o) {
    // create new IdxSpec of order ndim
    IdxSpec r;
    narrow_into(&r, d, s, o);
    return r;
  }

  ////////////////////////////////////////////////////////////////
  // transpose

  // tranpose two dimensions into pre-existing IdxSpec
  int IdxSpec::transpose_into(IdxSpec *dst, int d1, int d2) {
    try {
      if ((d1 < 0) || (d1 >= ndim) || 
	  (d2 < 0) || (d2 >= ndim)) 
	throw("tranpose: illegal dimension index");
    }
    catch(const char *s) { ylerror(s); return -1;}
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
  int IdxSpec::transpose_into(IdxSpec *dst, int *p) {
    try {
      for (int i=0; i<ndim; i++) {
	if ((p[i] < 0) || (p[i] >= ndim)) 
	  throw("tranpose: illegal dimension index");
      }
    }
    catch(const char *s) { ylerror(s); return -1;}
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

  int IdxSpec::transpose_inplace(int d1, int d2) {
    return transpose_into(this, d1, d2);
  }

  int IdxSpec::transpose_inplace(int *p) {
    return transpose_into(this, p);
  }

  IdxSpec IdxSpec::transpose(int d1, int d2) {
    IdxSpec r;
    transpose_into(&r, d1, d2);
    return r;
  }

  IdxSpec IdxSpec::transpose(int *p) {
    IdxSpec r;
    transpose_into(&r, p);
    return r;
  }

  ////////////////////////////////////////////////////////////////
  // unfold

  // d: dimension; k: kernel size; s: stride.
  intg IdxSpec::unfold_into(IdxSpec *dst, int d, intg k, intg s) {
    intg ns; // size of newly created dimension
    try {
      if (ndim <= 0) throw("cannot unfold an Idx of maximum order");
      if ((d < 0) || (d>=ndim)) throw("unfold: illegal dimension index");
      if ((k < 1) || (s < 1)) throw("unfold: kernel and stride must be >= 1");
      ns = 1+ (dim[d]-k)/s;
      if ((ns <= 0) || ( dim[d] != s*(ns-1)+k )) 
	throw("unfold: kernel and stride incompatible with size");
    }
    catch(const char *s) { ylerror(s); return -1;}
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

  intg IdxSpec::unfold_inplace(int d, intg k, intg s) {
    return unfold_into(this, d, k, s);
  }

  IdxSpec IdxSpec::unfold(int d, intg k, intg s) {
    IdxSpec r;
    unfold_into(&r, d, k, s);
    return r;
  }

  ////////////////////////////////////////////////////////////////

  // return true if two idxspec have the same dimensions,
  // i.e. if all their dimensions are equal (regardless of strides).
  bool same_dim(IdxSpec &s1, IdxSpec &s2) {
    if ( s1.ndim != s2.ndim ) return false; 
    for (int i=0; i<s1.ndim; i++) { if (s1.dim[i] != s2.dim[i]) return false; }
    return true;
  }

} // end namespace ebl
