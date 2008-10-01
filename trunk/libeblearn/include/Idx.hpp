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

#ifndef Idx_HPP
#define Idx_HPP

#include <sstream>
#include <vector>

namespace ebl {

////////////////////////////////////////////////////////////////

//! Idx elements and dimensions error checking macros

//! Calls ylerror if src0 and src1 have different number of elements.
#define idx_checknelems2_all(src0, src1) \
  if ((src0).nelements() != (src1).nelements()) ylerror("incompatible Idx sizes\n");

//! Calls ylerror if src0 and src1 and src2 have different number of elements.
#define idx_checknelems3_all(src0, src1, src2) \
  if (((src0).nelements() != (src1).nelements()) || \
  		((src0).nelements() != (src2).nelements())) \
  		ylerror("incompatible Idx sizes\n");

//! Calls ylerror if src0 and o0 do not match.
#define idx_checkorder1(src0, o0) \
  if ((src0).order() != o0) \
    ylerror("Idx has wrong order");

//! Calls ylerror if src0,src1 and o0,o1 do not match.
#define idx_checkorder2(src0, o0, src1, o1) \
  if (((src0).order() != o0) || ((src1).order() != o1)) \
    ylerror("Idx have incompatible orders");

//! Calls ylerror if src0,src1,src2 and o0,o1,o2 do not match.
#define idx_checkorder3(src0, o0, src1, o1, src2, o2) \
  if (((src0).order() != o0) || ((src1).order() != o1) || ((src2).order() != o2)) \
    ylerror("Idx have incompatible orders");

//! Calls ylerror if src0.order(), src1.order() and src2.order() differ
#define idx_checkorder3_all(src0, src1, src2) \
  if (((src0).order() != (src1).order()) || ((src0).order() != (src2).order())) \
    ylerror("Idx have incompatible orders");

//! Calls ylerror if src0.dim(0) and src1.dim(d1) don't match e0,e1
#define idx_checkdim2(src0, d0, e0, src1, d1, e1) \
	if (((src0).dim(d0) != e0) || ((src1).dim(d1) != e1))\
    ylerror("Idx have incompatible dimensions");

//! Calls ylerror if src0.dim(d) and src1.dim(d) don't match
#define idx_checkdim2_all(src0, src1, d) \
	if ((src0).dim(d) != (src1).dim(d)) \
    ylerror("Idx have incompatible dimensions");

//! Calls ylerror if src0.dim(d) and src1.dim(d) and src2.dim(d) don't match
#define idx_checkdim3_all(src0, src1, src2, d) \
	if (((src0).dim(d) != (src1).dim(d)) || \
			((src0).dim(d) != (src2).dim(d))) \
    ylerror("Idx have incompatible dimensions");

//! Calls ylerror if src0.dim(d) and src1.dim(d) and src2.dim(d)
//! and src3.dim(d) don't match
#define idx_checkdim4_all(src0, src1, src2, src3, d) \
	if (((src0).dim(d) != (src1).dim(d)) || \
			((src0).dim(d) != (src2).dim(d)) || \
			((src0).dim(d) != (src3).dim(d))) \
    ylerror("Idx have incompatible dimensions");

////////////////////////////////////////////////////////////////

//! call these macros like this:
//! { idx_bloop1(la, a) { r += la.get(); } }
//! exmaple: matrix-vector product  a x b -> c
//! { idx_bloop3(la, a, lb, b, lc, c) { dot_product(la,lb,lc); } }
//! It's advisable to encase every bloop in its own scope to
//! prevent name clashes if the same loop variable is used
//! multiple times in a scope.

// bloop/eloop macros

// Okay, either C++ really suxx0rz or I'm a really st00pid Lisp-head.
// Why can't we define syntax-changing macros like in Lisp? Basically,
// I can't encapsulate the allocation
// of temporary variable for a loop inside the macro, unless
// I define a "begin" macro and an "end" macro. I would like
// to define idx_bloop so I can do:
// idx_bloop2(lm, m, lv, v) { idx_dot(lm,lm,v); }
// but I can't do that because I have to allocate lm and lv
// inside the macro, hence I need to know the type.
// Now the call would be:
// idx_bloop2(lm, m, double, lv, v, double) { idx_dot(lm,lm,v); }
// But that still doesn't quite work because if I declare lm and lv
// then I can't reuse the same symbols for another loop in the same
// scope. The only way out is to force the user to encase every
// bloop call inside braces, or to not reuse the same synbol twice
// for a looping Idx. I thought about generating a mangled name
// but couldn't find a way to make it useful.
// If a macro could define its own scope that would be great.


#if USING_STL_ITERS == 1

#define idx_bloop1(dst0,src0,type0)	 \
  for ( DimIter<type0> dst0(src0,0); dst0.notdone(); ++dst0)

#define idx_bloop2(dst0,src0,type0,dst1,src1,type1)				\
  idx_checkdim2_all(src0, src1, 0); \
  DimIter<type0> dst0(src0,0); \
  DimIter<type1> dst1(src1,0); \
  for ( ; dst0.notdone(); ++dst0, ++dst1)

#define idx_bloop3(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2)	\
  idx_checkdim3_all(src0, src1, src2, 0); \
  DimIter<type0> dst0(src0,0);					\
  DimIter<type1> dst1(src1,0);					\
  DimIter<type2> dst2(src2,0);					\
  for ( ; dst0.notdone(); ++dst0, ++dst1, ++dst2)

#define idx_bloop4(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,dst3,src3,type3) \
  idx_checkdim4_all(src0, src1, src2, src3, 0); \
  DimIter<type0> dst0(src0,0);					\
  DimIter<type1> dst1(src1,0);					\
  DimIter<type2> dst2(src2,0);					\
  DimIter<type3> dst3(src3,0);					\
  for ( ; dst0.notdone(); ++dst0, ++dst1, ++dst2, ++dst3)

// eloop macros

#define idx_eloop1(dst0,src0,type0) \
  DimIter<type0> dst0(src0,src0.order()-1);	\
  for ( ; dst0.notdone(); ++dst0)

#define idx_eloop2(dst0,src0,type0,dst1,src1,type1)				\
  if ( (src0).dim((src0).order()) != (src1).dim((src1).order()) ) ylerror("incompatible Idxs for bloop\n"); \
  DimIter<type0> dst0(src0,(src0).order()-1); \
  DimIter<type1> dst1(src1,(src1).order()-1); \
  for ( ; dst0.notdone(); ++dst0, ++dst1)


////////////////////////////////////////////////////////////////
// aloop macros: loop over all elements

// Loops over all elements of an idx. This takes a pointer to
// the data type of idx elements, and a blank IdxIter object:
// idx_aloop1(data_pointer,idxiter,&idx) { do_stuff(data_pointer); }
// Example of use: add 1 to all element of m:
//  Idx<double> m(3,4);
//  ScalarIter<double> p;
//  idx_aloop1(p,&m) { *p += 1; }
#define idx_aloop1_on(itr0,src0)					\
  for ( ; itr0.notdone(); ++itr0)


// this loops simultaneously over all elements of 2 Idxs.
// The two Idxs can have different structures as long as they have
// the same total number of elements.
#define idx_aloop2_on(itr0,src0,itr1,src1)			\
	idx_checknelems2_all(src0, src1); \
  for ( ; itr0.notdone(); ++itr0, ++itr1)


#define idx_aloop3_on(itr0,src0,itr1,src1,itr2,src2)	\
	idx_checknelems3_all(src0, src1, src2); \
  for (; itr0.notdone(); ++itr0, ++itr1, ++itr2)


// high level aloop macros.
// These should be enclosed in braces, to avoid name clashes
#define idx_aloop1(itr0,src0,type0) \
  ScalarIter<type0> itr0(src0); \
  idx_aloop1_on(itr0,src0)


#define idx_aloop2(itr0,src0,type0,itr1,src1,type1)		\
  ScalarIter<type0> itr0(src0); \
  ScalarIter<type1> itr1(src1); \
  idx_checknelems2_all(src0, src1); \
  for (; itr0.notdone(); ++itr0, ++itr1)

#define idx_aloop3(itr0,src0,type0,itr1,src1,type1,itr2,src2,type2) \
  ScalarIter<type0> itr0(src0); \
  ScalarIter<type1> itr1(src1); \
  ScalarIter<type2> itr2(src2); \
  idx_checknelems3_all(src0, src1, src2); \
  for (; itr0.notdone(); ++itr0, ++itr1, ++itr2)

#else

#define idx_bloop1(dst0,src0,type0)	 \
  IdxLooper<type0> dst0(src0,0); \
  for ( ; dst0.notdone(); dst0.next())

#define idx_bloop2(dst0,src0,type0,dst1,src1,type1)				\
  idx_checkdim2_all(src0, src1, 0); \
  IdxLooper<type0> dst0(src0,0); \
  IdxLooper<type1> dst1(src1,0); \
  for ( ; dst0.notdone(); dst0.next(), dst1.next())

#define idx_bloop3(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2)	\
  idx_checkdim3_all(src0, src1, src2, 0); \
  IdxLooper<type0> dst0(src0,0);					\
  IdxLooper<type1> dst1(src1,0);					\
  IdxLooper<type2> dst2(src2,0);					\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next())

#define idx_bloop4(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,dst3,src3,type3) \
  idx_checkdim4_all(src0, src1, src2, src3, 0); \
  IdxLooper<type0> dst0(src0,0);					\
  IdxLooper<type1> dst1(src1,0);					\
  IdxLooper<type2> dst2(src2,0);					\
  IdxLooper<type3> dst3(src3,0);					\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next(), dst3.next())

// eloop macros

#define idx_eloop1(dst0,src0,type0) \
  IdxLooper<type0> dst0(src0,src0.order()-1);	\
  for ( ; dst0.notdone(); dst0.next())

#define idx_eloop2(dst0,src0,type0,dst1,src1,type1)				\
  if ( (src0).dim((src0).order()) != (src1).dim((src1).order()) ) ylerror("incompatible Idxs for bloop\n"); \
  IdxLooper<type0> dst0(src0,(src0).order()-1); \
  IdxLooper<type1> dst1(src1,(src1).order()-1); \
  for ( ; dst0.notdone(); dst0.next(), dst1.next())


////////////////////////////////////////////////////////////////
// aloop macros: loop over all elements

// Loops over all elements of an idx. This takes a pointer to
// the data type of idx elements, and a blank IdxIter object:
// idx_aloop1(data_pointer,idxiter,&idx) { do_stuff(data_pointer); }
// Example of use: add 1 to all element of m:
//  Idx<double> m(3,4);
//  IdxIter<double> p;
//  idx_aloop1(p,&m) { *p += 1; }
#define idx_aloop1_on(itr0,src0)					\
  for ( itr0.init(src0); itr0.notdone(); itr0.next())


// this loops simultaneously over all elements of 2 Idxs.
// The two Idxs can have different structures as long as they have
// the same total number of elements.
#define idx_aloop2_on(itr0,src0,itr1,src1)			\
	idx_checknelems2_all(src0, src1); \
  for ( itr0.init(src0), itr1.init(src1); itr0.notdone(); itr0.next(), itr1.next())

#define idx_aloop3_on(itr0,src0,itr1,src1,itr2,src2)	\
	idx_checknelems3_all(src0, src1, src2); \
  for (itr0.init(src0), itr1.init(src1), itr2.init(src2); itr0.notdone(); itr0.next(), itr1.next(), itr2.next())

// high level aloop macros.
// These should be enclosed in braces, to avoid name clashes
#define idx_aloop1(itr0,src0,type0) \
  IdxIter<type0> itr0; \
  idx_aloop1_on(itr0,src0)

#define idx_aloop2(itr0,src0,type0,itr1,src1,type1)		\
  IdxIter<type0> itr0; \
  IdxIter<type0> itr1; \
	idx_checknelems2_all(src0, src1); \
  for (itr0.init(src0), itr1.init(src1); itr0.notdone(); itr0.next(), itr1.next())

#define idx_aloop3(itr0,src0,type0,itr1,src1,type1,itr2,src2,type2) \
  IdxIter<type0> itr0; \
  IdxIter<type0> itr1; \
  IdxIter<type0> itr2; \
	idx_checknelems3_all(src0, src1, src2); \
  for (itr0.init(src0), itr1.init(src1), itr2.init(src2); itr0.notdone(); itr0.next(), itr1.next(), itr2.next())

#endif // if USING_STL_ITERS, else

////////////////////////////////////////////////////////////////
// Idx methods

template <class T> void Idx<T>::growstorage() {
  if ( storage->growsize(spec.footprint()) < 0) {
    ylerror("cannot grow storage");
  }
}

template <class T> void Idx<T>::growstorage_chunk(intg s_chunk){
	if( storage->growsize_chunk(spec.footprint(), s_chunk) < 0) {
		ylerror("cannot grow storage");
	}
}

/* the constructor doesn't exist and I didn't find one to replace it
template<typename T>
void Idx<T>::printElems(FILE* filePtr){
	this->printElems(std::ofstream(filePtr));
}
*/

template <typename T>
void Idx<T>::printElems( std::ostream& out ){
	printElems_impl(0, out);
}

template <typename T>
void Idx<T>::printElems(){
	this->printElems( std::cout );
}

template<class T> inline T printElems_impl_cast(T val) {
	return val;
}

// specialization for ubyte to print as unsigned integers.
inline unsigned int printElems_impl_cast(ubyte val) {
	return (unsigned int) val;
}

template <typename T>
void Idx<T>::printElems_impl( int indent, std::ostream& out ){
	static const std::string lbrace = "[";
	static const std::string rbrace = "]";
	static const std::string sep = " ";
	std::ostringstream oss;
	for( unsigned int ii = 0; ii < lbrace.length(); ++ii ){
		oss<<" ";
	}
	const std::string tab(oss.str());

	// printing a 0-dimensional tensor
	if( order() == 0 ){
		out<<lbrace<<"@"<<sep<< printElems_impl_cast(get()) <<sep<<rbrace<<"\n";
	}
	// printing a 1-D tensor
	else if( order() == 1 ){
		out<<lbrace<<sep;
		for( int ii = 0; ii < dim(0); ++ii ){
			out<< printElems_impl_cast(get(ii)) <<sep;
		}
		out<<rbrace<<"\n";
	}
	// printing a multidimensional tensor
	else{

		// opening brace
		out<<lbrace;

		// print subtensors.
		Idx<T> subtensor(storage, spec.offset);
		for( int dimInd = 0; dimInd < dim(0); ++dimInd ){

			// only print indent if this isn't the first subtensor.
			if( dimInd > 0 ){
				for( int ii = 0; ii < indent+1; ++ii ){
					out<<(tab);
				}
			}

			// print subtensor
			spec.select_into(&subtensor.spec, 0, dimInd);
			subtensor.printElems_impl( indent+1, out );

			// only print the newline if this isn't the last subtensor.
			if( dimInd < (dim(0)-1)){
				out<<"\n";
			}

		}

		// closing brace
		out<<rbrace<<"\n";
	}
}

template <class T> void Idx<T>::pretty(FILE *f) {
  fprintf(f,"Idx: at address %ld\n",(intg)this);
  fprintf(f,"  storage=%ld (size=%ld)\n",(intg)storage,storage->size());
  spec.pretty(f);
}

template <class T> void Idx<T>::pretty(std::ostream& out){
	out << "Idx: at address " << (intg)this << "\n";
	out << "  storage=" <<  (intg)storage << "(size=" << storage->size() << "\n";
	spec.pretty(out);
}

template <class T> Idx<T>::~Idx() {
  DEBUG("Idx::destructor %ld\n",long(this));
  storage->unlock();
}

// fake constructor called by IdxLooper constructor
template <class T> Idx<T>::Idx(dummyt *dummy) {
  spec.dim = NULL;
  spec.mod = NULL;
  storage = NULL;
}

//template <typename T>
//Idx<T>::Idx( Idx<T>& other )
//	:storage(other.storage),
//	 spec(other.spec)
// {
//	storage->lock();
// }
//
//template <typename T>
//Idx<T>::Idx( const Idx<T>& other )
//	:storage(other.storage),
//	 spec(other.spec)
// {
//	storage->lock();
// }

template <class T> Idx<T>::Idx(Srg<T> *srg, IdxSpec &s) {
  spec = s;
  storage = srg;
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx(Srg<T> *srg, intg o) : spec(o) {
  storage = srg;
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx() : spec(0) {
  storage = new Srg<T>();
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx(Srg<T> *srg, intg o, intg size0) : spec(o,size0) {
  storage = srg;
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx(intg size0) : spec(0,size0) {
  storage = new Srg<T>();
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx(Srg<T> *srg, intg o, intg size0, intg size1) : spec(o,size0,size1) {
  storage = srg;
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx(intg size0, intg size1) : spec(0,size0,size1) {
  storage = new Srg<T>();
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx(Srg<T> *srg, intg o, intg size0, intg size1, intg size2) : spec(o,size0,size1,size2) {
  storage = srg;
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx(intg size0, intg size1, intg size2) : spec(0,size0,size1,size2) {
  storage = new Srg<T>();
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx(Srg<T> *srg, intg o, intg s0, intg s1, intg s2, intg s3, intg s4, intg s5, intg s6, intg s7) : spec(o,s0,s1,s2,s3,s4,s5,s6,s7) {
  storage = srg;
  growstorage();
  storage->lock();
}

template <class T> Idx<T>::Idx(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5, intg s6, intg s7) : spec(0,s0,s1,s2,s3,s4,s5,s6,s7) {
  storage = new Srg<T>();
  growstorage();
  storage->lock();
}


template <class T> intg Idx<T>::setoffset(intg o) {
  if (o<0) { ylerror("Idx::setoffset: offset must be positive"); }
  if (o > spec.offset) {
    spec.setoffset(o);
    growstorage();
    return o;
  } else {
    spec.setoffset(o);
    return o;
  }
}

template <class T> void Idx<T>::resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5, intg s6, intg s7) {
  spec.resize(s0,s1,s2,s3,s4,s5,s6,s7);
  growstorage();
}

template <class T> void Idx<T>::resize_chunk(intg s_chunk, intg s0, intg s1, intg s2, intg s3, intg s4, intg s5, intg s6, intg s7) {
  spec.resize(s0,s1,s2,s3,s4,s5,s6,s7);
  growstorage_chunk(s_chunk);
}

//template<typename T, typename SizeIter>
//void Idx<T>::resize( SizeIter& sizesBegin, SizeIter& sizesEnd ){
//	const int ndims = std::distance( sizesBegin, sizesEnd );
//	if ( ndims > MAXDIMS ){
//		std::ostringstream oss;
//		oss<<"Number of dimensions ("<<ndims<<") exceeds MAX_DIMS ("<<MAXDIMS<<")";
//		ylerror(oss.str());
//	}
//
//	std::vector<T> sizes(ndims+1);
//	std::copy(sizesBegin, sizesEnd, sizes.begin());
//	sizes.back() = -1;
//	spec.resize( sizesBegin, sizesEnd );
//	growstorage();
//}


template <class T> Idx<T> Idx<T>::select(int d, intg i) {
  Idx<T> r(storage,spec.getoffset());
  spec.select_into(&r.spec, d, i);
  return r;
}

template <class T> Idx<T> Idx<T>::narrow(int d, intg s, intg o) {
  Idx<T> r(storage,spec.getoffset());
  spec.narrow_into(&r.spec, d, s, o);
  return r;
}

template <class T> Idx<T> Idx<T>::transpose(int d1, int d2) {
  Idx<T> r(storage,spec.getoffset());
  spec.transpose_into(&r.spec, d1, d2);
  return r;
}

template <class T> Idx<T> Idx<T>::transpose(int *p) {
  Idx<T> r(storage,spec.getoffset());
  spec.transpose_into(&r.spec, p);
  return r;
}

template <class T> Idx<T> Idx<T>::unfold(int d, intg k, intg s) {
  Idx<T> r(storage,spec.getoffset());
  spec.unfold_into(&r.spec, d, k, s);
  return r;
}


////////////////////////////////////////////////////////////////
// access methods

// get element of Idx1
template <class T> T *Idx<T>::ptr(intg i0) {
  if (spec.ndim != 1) ylerror("not an Idx1");
  if ((i0 < 0) || (i0 >= spec.dim[0])) ylerror("index 0 out of bound");
  return storage->data + spec.offset + i0*spec.mod[0];
}

// get element of Idx2
template <class T> T *Idx<T>::ptr(intg i0, intg i1) {
  if (spec.ndim != 2) ylerror("not an Idx2");
  if ((i0 < 0) || (i0 >= spec.dim[0])) ylerror("index 0 out of bound");
  if ((i1 < 0) || (i1 >= spec.dim[1])) ylerror("index 1 out of bound");
  return storage->data + spec.offset + i0*spec.mod[0] + i1*spec.mod[1];
}

// get element of Idx3
template <class T> T *Idx<T>::ptr(intg i0, intg i1, intg i2) {
  if (spec.ndim != 3) ylerror("not an Idx3");
  if ((i0 < 0) || (i0 >= spec.dim[0])) ylerror("index 0 out of bound");
  if ((i1 < 0) || (i1 >= spec.dim[1])) ylerror("index 1 out of bound");
  if ((i2 < 0) || (i2 >= spec.dim[2])) ylerror("index 2 out of bound");
  return storage->data + spec.offset + i0*spec.mod[0] + i1*spec.mod[1] + i2*spec.mod[2];
}


// return a pointer to an element of an Idx
// generic function for order>3
template <class T> T *Idx<T>::ptr(intg i0, intg i1, intg i2, intg i3, intg i4, intg i5, intg i6, intg i7) {
  try {
    // check that we passed the right number of indices
    // and that they are all positive
    switch (spec.ndim) {
    case 8: if (i7 < 0) throw(-8);break;
    case 7: if ((i6 < 0) || (i7 != -1)) throw(-7);break;
    case 6: if ((i5 < 0) || (i6 != -1)) throw(-6);break;
    case 5: if ((i4 < 0) || (i5 != -1)) throw(-5);break;
    case 4: if ((i3<0)||(i2<0)||(i1<0)||(i0<0)||(i4 != -1)) throw(-4);break;
    default: throw(10);
    }
    // now compute offset, and check that all
    // indices are within bounds.
    intg k = 0;
    switch (spec.ndim) {
    case 8: k += spec.mod[7]*i7; if (i7 >= spec.dim[7])  throw(7);
    case 7: k += spec.mod[6]*i6; if (i6 >= spec.dim[6])  throw(6);
    case 6: k += spec.mod[5]*i5; if (i5 >= spec.dim[5])  throw(5);
    case 5: k += spec.mod[4]*i4; if (i4 >= spec.dim[4])  throw(4);
    case 4: k += spec.mod[3]*i3; if (i3 >= spec.dim[3])  throw(3);
    }
    k += spec.mod[2]*i2; if (i2 >= spec.dim[2])  throw(2);
    k += spec.mod[1]*i1; if (i1 >= spec.dim[1])  throw(1);
    k += spec.mod[0]*i0; if (i0 >= spec.dim[0])  throw(0);
    return storage->data + spec.offset + k;
  }
  catch(int k) {
    if (k==10) ylerror("Idx::get: number of indices and order are different");
    if (k < 0) {
      ylerror("Idx::get: wrong number of indices, or negative index");
    } else {
      ylerror("Idx::get: index out of bound");
    }
    return NULL;
  }
}

////////////////////////////////////////////////////////////////
// get

// get element of Idx0
template <class T> T Idx<T>::get() {
  if (spec.ndim != 0) ylerror("not an Idx0");
  return (storage->data)[spec.offset];
}

// get element of Idx1
template <class T> T Idx<T>::get(intg i0) {
  if (spec.ndim != 1) ylerror("not an Idx1");
  if ((i0 < 0) || (i0 >= spec.dim[0])) ylerror("index 0 out of bound");
  return (storage->data)[spec.offset + i0*spec.mod[0]];
}

// get element of Idx2
template <class T> T Idx<T>::get(intg i0, intg i1) {
  if (spec.ndim != 2) ylerror("not an Idx2");
  if ((i0 < 0) || (i0 >= spec.dim[0])) ylerror("index 0 out of bound");
  if ((i1 < 0) || (i1 >= spec.dim[1])) ylerror("index 1 out of bound");
  return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1]];
}

// get element of Idx3
template <class T> T Idx<T>::get(intg i0, intg i1, intg i2) {
  if (spec.ndim != 3) ylerror("not an Idx3");
  if ((i0 < 0) || (i0 >= spec.dim[0])) ylerror("index 0 out of bound");
  if ((i1 < 0) || (i1 >= spec.dim[1])) ylerror("index 1 out of bound");
  if ((i2 < 0) || (i2 >= spec.dim[2])) ylerror("index 2 out of bound");
  return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1] + i2*spec.mod[2]];
}

// get element of an Idx of any order
template <class T> T Idx<T>::get(intg i0, intg i1, intg i2, intg i3, intg i4, intg i5, intg i6, intg i7) {
  return *ptr(i0,i1,i2,i3,i4,i5,i6,i7);
}

////////////////////////////////////////////////////////////////
// set

// set the element of Idx0
template <class T> T Idx<T>::set(T val) {
  if (spec.ndim != 0) ylerror("not an Idx0");
  return (storage->data)[spec.offset] = val;
}

// set the element of Idx1
template <class T> T Idx<T>::set(T val, intg i0) {
  if (spec.ndim != 1) ylerror("not an Idx1");
  if ((i0 < 0) || (i0 >= spec.dim[0])) ylerror("index 0 out of bound");
  return (storage->data)[spec.offset + i0*spec.mod[0]] = val;
}

// set the element of Idx2
template <class T> T Idx<T>::set(T val, intg i0, intg i1) {
  if (spec.ndim != 2) ylerror("not an Idx2");
  if ((i0 < 0) || (i0 >= spec.dim[0])) ylerror("index 0 out of bound");
  if ((i1 < 0) || (i1 >= spec.dim[1])) ylerror("index 1 out of bound");
  return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1]] = val;
}

// set the element of Idx3
template <class T> T Idx<T>::set(T val, intg i0, intg i1, intg i2) {
  if (spec.ndim != 3) ylerror("not an Idx3");
  if ((i0 < 0) || (i0 >= spec.dim[0])) ylerror("index 0 out of bound");
  if ((i1 < 0) || (i1 >= spec.dim[1])) ylerror("index 1 out of bound");
  if ((i2 < 0) || (i2 >= spec.dim[2])) ylerror("index 2 out of bound");
  return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1] + i2*spec.mod[2]] = val;
}


// set an element of an Idx of any order.
template <class T> T Idx<T>::set(T val, intg i0, intg i1, intg i2, intg i3, intg i4, intg i5, intg i6, intg i7) {
    return *ptr(i0,i1,i2,i3,i4,i5,i6,i7) = val;
}

template <class T> int Idx<T>::fdump(FILE *f) {
  if (spec.ndim == 0) {
    fprintf(f,"[@ %g]\n",this->get());
  } else if (spec.ndim == 1) {
    fprintf(f,"[");
    for (intg i=0; i<dim(0); i += mod(0)) {
      fprintf(f,"%g ",(storage->data)[spec.offset + i]);
    }
    fprintf(f,"]\n");
  } else {
    fprintf(f,"[");
    { idx_bloop1(p,*this,T) { p.fdump(f); } }
    fprintf(f,"]\n");
  }
  return 0;
}

////////////////////////////////////////////////////////////////
// STL-style iterator creators
template <typename T> typename Idx<T>::scalar_iterator Idx<T>::scalars_begin(){
	return scalar_iterator(*this);
}

template <typename T> typename Idx<T>::scalar_iterator Idx<T>::scalars_end(){
	return scalar_iterator(*this, false);
}

template <typename T> typename Idx<T>::reverse_scalar_iterator Idx<T>::scalars_rbegin(){
	return reverse_scalar_iterator(*this);
}

template <typename T> typename Idx<T>::reverse_scalar_iterator Idx<T>::scalars_rend(){
	return reverse_scalar_iterator(*this);
}

template <typename T>
typename Idx<T>::dimension_iterator
Idx<T>::dim_begin(int dd){
	return dimension_iterator(*this,dd);
}

template <typename T>
typename Idx<T>::dimension_iterator
Idx<T>::dim_end(int dd){
	return dimension_iterator(*this,dd,false);
}

template <typename T>
typename Idx<T>::reverse_dimension_iterator
Idx<T>::dim_rbegin(int dd){
	return reverse_dimension_iterator(*this,dd);
}

template <typename T>
typename Idx<T>::reverse_dimension_iterator
Idx<T>::dim_rend(int dd){
	return reverse_dimension_iterator(*this,dd,false);
}


#if USING_STL_ITERS == 0
////////////////////////////////////////////////////////////////
// an IdxLooper is an iterator for Idxs.
// It is actually a subclass of Idx.
// These are not C++ iterators in the classical sense.

template <class T> IdxLooper<T>::IdxLooper(Idx<T> &idx, int ld) : Idx<T>((dummyt*)0) {
  i = 0;
  dimd = idx.spec.dim[ld];
  modd = idx.spec.mod[ld];
  idx.spec.select_into(&(this->spec), ld, i);
  this->storage = idx.storage;
  this->storage->lock();
}

// like ++
// CAUTION: this doesn't do array bound checking
// because we coudn't use a for loop if it did.
template <class T> T *IdxLooper<T>::next() {
  i++;
  this->spec.offset += modd;
  return this->storage->data + this->spec.offset;
}

// return true when done.
template <class T> bool IdxLooper<T>::notdone() { return ( i < dimd ); }


////////////////////////////////////////////////////////////////
// a pointer that loops over all elements
// of an Idx

// empty constructor;
template <class T> IdxIter<T>::IdxIter() { };

template <class T> T *IdxIter<T>::init(Idx<T> &idx) {
  iterand = &idx;
  i = 0;
  j = iterand->spec.ndim;
  data = iterand->storage->data + iterand->spec.offset;
  if (iterand->spec.contiguousp()) {
    d[0] = -1;
  } else {
    for(int i=0; i < iterand->spec.ndim; i++) { d[i] = 0; }
  }
  n = iterand->spec.nelements();
  return data;
}

template <class T> T *IdxIter<T>::next() {
  i++;
  if (d[0] < 0) {
    // contiguous Idx
    data++;
  } else {
    // non-contiguous Idx
    j--;
    do {
      if (j<0) break;
      if (++d[j] < iterand->spec.dim[j]) {
	data += iterand->spec.mod[j];
	j++;
      } else {
	data -= iterand->spec.dim[j] * iterand->spec.mod[j];
	d[j--] = -1;
      }
    } while (j < iterand->spec.ndim);
  }
  return data;
}

template <class T> bool IdxIter<T>::notdone() { return ( i < n ); }

#endif // IF USING_STL_ITERS == 1

} // namespace ebl

#endif
