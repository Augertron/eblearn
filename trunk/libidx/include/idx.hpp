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

#ifndef idx_HPP
#define idx_HPP

#include <sstream>
#include <vector>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  //! state_idx error checking macros

  //! check that 2 state_idxs (references) are different, if not, call eblerror
#define state_idx_check_different(s1, s2)			\
  if (&s1 == &s2)						\
    eblerror("in and out state_idx must be different");

  //! check that 3 state_idxs (references) are different, if not, call eblerror
#define state_idx_check_different3(s1, s2, s3)			\
  if ((&s1 == &s2) || (&s1 == &s3))				\
    eblerror("in1, in2 and out state_idx must be different");

  ////////////////////////////////////////////////////////////////
  //! idx error macros

  //! Print incompatible idx1 and idx2 and call eblerror with errmsg.
#define idx_compatibility_error2(idx1, idx2, errmsg) {			\
    cerr <<"error: "<<idx1 << " and " << idx2 << " are incompatible." << endl; \
    eblerror(errmsg); }
  
  //! Print incompatible idx1, idx2, idx3 and call eblerror with errmsg.
#define idx_compatibility_error3(idx1, idx2, idx3, errmsg) {		\
    cerr << "error: " << idx1 << ", " << idx2 << " and " << idx3	\
	 << " are incompatible." << endl; eblerror(errmsg); }
  
  //! Print incompatible idx1, idx2, idx3 and call eblerror with errmsg.
#define idx_compatibility_error4(idx1, idx2, idx3, idx4, errmsg) {	\
    cerr << "error: " << idx1 << ", " << idx2 << ", " << idx3 << " and " \
	 << idx4 << " are incompatible." << endl; eblerror(errmsg); }
  
  //! Print incompatible idx1, idx2, idx3 and call eblerror with errmsg.
#define idx_compatibility_error5(idx1, idx2, idx3, idx4, idx5, errmsg) { \
    cerr << "error: " << idx1 << ", " << idx2 << ", " << idx3 << ", " << idx4 \
	 << " and " << idx5 << " are incompatible." << endl; eblerror(errmsg); }
  
  //! Print incompatible idx1, idx2, idx3 and call eblerror with errmsg.
#define idx_compatibility_error6(idx1, idx2, idx3, idx4, idx5, idx6, errmsg) { \
    cerr << "error: " << idx1 << ", " << idx2 << ", " << idx3 << ", " << idx4 \
	 << ", " << idx5 << " and " << idx6				\
	 << " are incompatible." << endl; eblerror(errmsg); }
  
  ////////////////////////////////////////////////////////////////
  //! idx elements and dimensions error checking macros

  //! Calls eblerror if src0 is not contiguous.
#define idx_check_contiguous1(src0)				\
  if (!(src0).contiguousp())					\
    eblerror("idx must be contiguous\n");

  //! Calls eblerror if src0 and src1 are not contiguous.
#define idx_check_contiguous2(src0, src1)				\
  if (!(src0).contiguousp() || !(src1).contiguousp())			\
    eblerror("idx must be contiguous\n");

  //! Calls eblerror if src0 and src1 have different number of elements.
#define idx_checknelems2_all(src0, src1)				\
  if ((src0).nelements() != (src1).nelements()) {			\
    cerr << "error: " << src0 << " and " << src1;			\
    cerr << " should have the same number of elements." << endl;	\
    eblerror("incompatible idx sizes\n"); }

  //! Calls eblerror if src0 and src1 and src2 have different number of
  //! elements.
#define idx_checknelems3_all(src0, src1, src2)				\
  if (((src0).nelements() != (src1).nelements()) ||			\
      ((src0).nelements() != (src2).nelements())) {			\
    cerr << "error: " << src0 << ", " << src1 << " and " << src2;	\
    cerr << " should have the same number of elements." << endl;	\
    eblerror("incompatible idx sizes\n"); }
  
  //! Calls eblerror if src0 and o0 do not match.
#define idx_checkorder1(src0, o0)					\
  if ((src0).order() != o0) {						\
    cerr << "error: " << src0 << " does not have order " << o0 << endl; \
    eblerror("idx has wrong order"); }

  //! Calls eblerror if src0,src1 and o0,o1 do not match.
#define idx_checkorder2(src0, o0, src1, o1)				\
  if (((src0).order() != o0) || ((src1).order() != o1)) {		\
    cerr << "error: ";							\
    if ((src0).order() != o0) cerr << src0 <<" should have order "<< o0<<". "; \
    if ((src1).order() != o1) cerr << src1 <<" should have order "<< o1<<". "; \
    cerr << endl;							\
    eblerror("idx have incompatible orders"); }

  //! Calls eblerror if src0,src1,src2 and o0,o1,o2 do not match.
#define idx_checkorder3(src0, o0, src1, o1, src2, o2)			\
  if (((src0).order() != o0) || ((src1).order() != o1)			\
      || ((src2).order() != o2)) {					\
    cerr << "error: ";							\
    if ((src0).order() != o0) cerr << src0 <<" should have order "<< o0<<". "; \
    if ((src1).order() != o1) cerr << src1 <<" should have order "<< o1<<". "; \
    if ((src2).order() != o2) cerr << src2 <<" should have order "<< o2<<". "; \
    cerr << endl;							\
    eblerror("idx have incompatible orders"); }

  //! Calls eblerror if src0.order(), src1.order() and src2.order() differ
#define idx_checkorder3_all(src0, src1, src2)				\
  if (((src0).order() != (src1).order())				\
      || ((src0).order() != (src2).order()))				\
    idx_compatibility_error3(src0, src1, src2, "idx have incompatible orders");

  //! Calls eblerror if src0.dim(0) and src1.dim(d1) don't match e0,e1
#define idx_checkdim2(src0, d0, e0, src1, d1, e1)	\
  if (((src0).dim(d0) != e0) || ((src1).dim(d1) != e1))	\
    idx_compatibility_error2(src0, src1, "idx have incompatible dimensions");

  //! Calls eblerror if src0.dim(d) and src1.dim(d) don't match
#define idx_checkdim2_all(src0, src1, d)		\
  if ((src0).dim(d) != (src1).dim(d))			\
    idx_compatibility_error2(src0, src1, "idx have incompatible dimensions");

  //! Calls eblerror if src0.dim(d) and src1.dim(d) and src2.dim(d) don't match
#define idx_checkdim3_all(src0, src1, src2, d)		\
  if ((src0.order() != 0) && (((src0).dim(d) != (src1).dim(d)) ||	\
			      ((src0).dim(d) != (src2).dim(d))))	\
    idx_compatibility_error3(src0,src1,src2,"idx have incompatible dimensions");

  //! Calls eblerror if src0.dim(d) and src1.dim(d) and src2.dim(d)
  //! and src3.dim(d) don't match
#define idx_checkdim4_all(src0, src1, src2, src3, d)	\
  if (((src0).dim(d) != (src1).dim(d)) ||		\
      ((src0).dim(d) != (src2).dim(d)) ||		\
      ((src0).dim(d) != (src3).dim(d)))			\
    idx_compatibility_error4(src0,src1,src2,src3,	\
			     "idx have incompatible dimensions");

  //! Calls eblerror if src0.dim(d) and src1.dim(d) and src2.dim(d)
  //! and src3.dim(d) and src4.dim(d) don't match
#define idx_checkdim5_all(src0, src1, src2, src3, src4, d)	\
  if (((src0).dim(d) != (src1).dim(d)) ||			\
      ((src0).dim(d) != (src2).dim(d)) ||			\
      ((src0).dim(d) != (src3).dim(d)) ||			\
      ((src0).dim(d) != (src4).dim(d)))				\
    idx_compatibility_error5(src0,src1,src2,src3,src4,			\
			     "idx have incompatible dimensions");

  //! Calls eblerror if src0.dim(d) and src1.dim(d) and src2.dim(d)
  //! and src3.dim(d) and src4.dim(d) amd src5.dim(d) don't match
#define idx_checkdim6_all(src0, src1, src2, src3, src4, src5, d)	\
  if (((src0).dim(d) != (src1).dim(d)) ||			\
      ((src0).dim(d) != (src2).dim(d)) ||			\
      ((src0).dim(d) != (src3).dim(d)) ||			\
      ((src0).dim(d) != (src4).dim(d)) ||			\
      ((src0).dim(d) != (src5).dim(d)))				\
    idx_compatibility_error6(src0,src1,src2,src3,src4,src5,	\
			     "idx have incompatible dimensions");

  ////////////////////////////////////////////////////////////////
  // TODO: these macros are untested (YLC)

  //! cidxN_bloopX: macros to loop simultaneously over elements 
  //! of X idx'es of order at least N. Can be used as follows:
  //! { double *z0, *z1; 
  //!   intg i;
  //!   cidx1_bloop2(i, z0, myidx0, z1, myidx1) { *z0 = *z1 + 4; }
  //! }
  //! { float *z0;
  //!   intg i,j;
  //!   cidx2_bloop1(i, j, z0, myidx0) { *z0 *= 2; }
  //! }
  //! Variable i is a loop index, myidx0 is an idx of any type whose
  //! order must be at least 1, and and z0 is a pointer to the numerical 
  //! type of myidx0.
  //! It is best to enclose each cidx1_bloopX in its own brace scope
  //! because the macro creates temprary variables with a fixed name.
  //! These macros should be used over the idx_aloopX macros whenever
  //! possible, because they are considerably more efficient.
  //! unlike the aloop macros, these macros manipulate regular pointers 
  //! with simple incrementation, as opposed to iterators with complicated 
  //! logic.
#define cidx1_bloop1(i,p0,src0)					\
  if ((src0).order() < 1) eblerror("idx has wrong order");	\
  intg _n0 = (src0).dim(0), _m0 = (src0).mod(0);		\
  for (i=0, p0=(src0).idx_ptr(); i<_n0; i++, p0+=_m0) 

#define cidx1_bloop2(i,p0,src0,p1,src1)					\
  if (((src0).order() < 1)||((src1).order() < 1))			\
    eblerror("idx has wrong order");					\
  intg _n0 = (src0).dim(0), _m0 = (src0).mod(0); _m1 = (src1).mod(0);	\
  idx_checkdim2_all(src0,src1,0)					\
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr();			\
       i<_n0;								\
       i++, p0+=_m0, p1+=_m1) 

#define cidx1_bloop3(i,p0,src0,p1,src1,p2,src2)				\
  intg _n0 = (src0).dim(0), _m0 = (src0).mod(0);			\
  intg _m1 = (src1).mod(0), _m2 = (src2).mod(0);			\
  idx_checkdim3_all(src0,src1,src2,0)					\
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr(), p2=(src2).idx_ptr();\
       i<_n0;								\
       i++, p0+=_m0, p1+=_m1, p2+=_m2) 

#define cidx1_bloop4(i,p0,src0,p1,src1,p2,src2,p3,src3)			\
  intg _n0 = (src0).dim(0), _m0 = (src0).mod(0), _m1 = (src1).mod(0);	\
  intg _m2 = (src2).mod(0), _m3 = (src3).mod(0);			\
  idx_checkdim4_all(src0,src1,src2,src3,0)				\
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr(),			\
	 p2=(src2).idx_ptr(), p3=(src3).idx_ptr();			\
       i<_n0;								\
       i++, p0+=_m0, p1+=_m1, p2+=_m2, p3+=_m3) 

#define cidx2_bloop1(i,j,p0,src0)					\
  if ((src0).order() < 2) eblerror("idx has wrong order");		\
  intg _n00 = (src0).dim(0), _m00 = (src0).mod(0);			\
  intg _n01 = (src0).dim(1), _m01 = (src0).mod(1);			\
  for (i=0, p0=(src0).idx_ptr(); i<_n00; i++, p0+=_m00-_n01*_m01)	\
    for (j=0; i<_n01; j++, p0+=_m01) 

#define cidx2_bloop2(i,j,p0,src0,p1,src1)				\
  if ((src0).order() < 2) eblerror("idx has wrong order");		\
  intg _n00 = (src0).dim(0), _m00 = (src0).mod(0);			\
  intg _n01 = (src0).dim(1), _m01 = (src0).mod(1);			\
  intg _n10 = (src1).dim(0), _m10 = (src1).mod(0);			\
  intg _n11 = (src1).dim(1), _m11 = (src1).mod(1);			\
  idx_checkdim2_all(src0,src1,0)					\
  idx_checkdim2_all(src0,src1,1)					\
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr();			\
       i<_n00;								\
       i++, p0+=_m00-_n01*_m01, p1+=_m10-_n11*_m11)			\
    for (j=0; i<_n01; j++, p0+=_m01, p1+=_m11) 

#define cidx2_bloop3(i,j,p0,src0,p1,src1,p2,src2)			\
  if ((src0).order() < 2) eblerror("idx has wrong order");		\
  intg _n00 = (src0).dim(0), _m00 = (src0).mod(0);			\
  intg _n01 = (src0).dim(1), _m01 = (src0).mod(1);			\
  intg _n10 = (src1).dim(0), _m10 = (src1).mod(0);			\
  intg _n11 = (src1).dim(1), _m11 = (src1).mod(1);			\
  intg _n20 = (src2).dim(0), _m20 = (src2).mod(0);			\
  intg _n21 = (src2).dim(1), _m21 = (src2).mod(1);			\
  idx_checkdim3_all(src0,src1,src2,0)					\
  idx_checkdim3_all(src0,src1,src2,1)					\
  for (i=0, p0=(src0).idx_ptr(), p1=(src1).idx_ptr(), p2=(src2).idx_ptr(); \
       i<_n00;								\
       i++, p0+=_m00-_n01*_m01, p1+=_m10-_n11*_m11, p2+=_m20-_n21*_m21) \
    for (j=0; i<_n01; j++, p0+=_m01, p1+=_m11, p2+=_m21) 

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
  // for a looping idx. I thought about generating a mangled name
  // but couldn't find a way to make it useful.
  // If a macro could define its own scope that would be great.

#if USING_STL_ITERS == 1

#define idx_bloop1(dst0,src0,type0)				\
  for ( DimIter<type0> dst0(src0,0); dst0.notdone(); ++dst0)

#define idx_bloop2(dst0,src0,type0,dst1,src1,type1)	\
  idx_checkdim2_all(src0, src1, 0);			\
  DimIter<type0> dst0(src0,0);				\
  DimIter<type1> dst1(src1,0);				\
  for ( ; dst0.notdone(); ++dst0, ++dst1)

#define idx_bloop3(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2)	\
  idx_checkdim3_all(src0, src1, src2, 0);				\
  DimIter<type0> dst0(src0,0);						\
  DimIter<type1> dst1(src1,0);						\
  DimIter<type2> dst2(src2,0);						\
  for ( ; dst0.notdone(); ++dst0, ++dst1, ++dst2)

#define idx_bloop4(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,	\
		   dst3,src3,type3)					\
  idx_checkdim4_all(src0, src1, src2, src3, 0);				\
  DimIter<type0> dst0(src0,0);						\
  DimIter<type1> dst1(src1,0);						\
  DimIter<type2> dst2(src2,0);						\
  DimIter<type3> dst3(src3,0);						\
  for ( ; dst0.notdone(); ++dst0, ++dst1, ++dst2, ++dst3)

#define idx_bloop5(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,	\
		   dst3,src3,type3,dst4,src4,type4)			\
  idx_checkdim5_all(src0, src1, src2, src3, src4, 0);			\
  DimIter<type0> dst0(src0,0);						\
  DimIter<type1> dst1(src1,0);						\
  DimIter<type2> dst2(src2,0);						\
  DimIter<type3> dst3(src3,0);						\
  DimIter<type4> dst4(src4,0);						\
  for ( ; dst0.notdone(); ++dst0, ++dst1, ++dst2, ++dst3, ++dst4)

#define idx_bloop6(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,	\
		   dst3,src3,type3,dst4,src4,type4,dst5,src5,type5)	\
  idx_checkdim6_all(src0, src1, src2, src3, src4, src5, 0);		\
  DimIter<type0> dst0(src0,0);						\
  DimIter<type1> dst1(src1,0);						\
  DimIter<type2> dst2(src2,0);						\
  DimIter<type3> dst3(src3,0);						\
  DimIter<type4> dst4(src4,0);						\
  DimIter<type5> dst5(src5,0);						\
  for ( ; dst0.notdone(); ++dst0, ++dst1, ++dst2, ++dst3, ++dst4, ++dst5)

  // eloop macros

#define idx_eloop1(dst0,src0,type0)		\
  DimIter<type0> dst0(src0,src0.order()-1);	\
  for ( ; dst0.notdone(); ++dst0)

#define idx_eloop2(dst0,src0,type0,dst1,src1,type1)			\
  if ((src0).dim((src0).order() - 1) != (src1).dim((src1).order() - 1)) \
    eblerror("incompatible idxs for eloop\n");				\
  DimIter<type0> dst0(src0,(src0).order()-1);				\
  DimIter<type1> dst1(src1,(src1).order()-1);				\
  for ( ; dst0.notdone(); ++dst0, ++dst1)

#define idx_eloop3(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2)	\
  if (((src0).dim((src0).order() - 1) != (src1).dim((src1).order() - 1)) \
      || ((src0).dim((src0).order() - 1) != (src2).dim((src2).order() - 1))) \
    eblerror("incompatible idxs for eloop\n");				\
  DimIter<type0> dst0(src0,(src0).order()-1);				\
  DimIter<type1> dst1(src1,(src1).order()-1);				\
  DimIter<type2> dst2(src2,(src2).order()-1);				\
  for ( ; dst0.notdone(); ++dst0, ++dst1, ++dst2)

#define idx_eloop4(dst0,src0,type0,dst1,src1,type1,			\
		   dst2,src2,type2,dst3,src3,type3)			\
  if (((src0).dim((src0).order() - 1) != (src1).dim((src1).order() - 1)) \
      || ((src0).dim((src0).order() - 1) != (src2).dim((src2).order() - 1)) \
      || ((src0).dim((src0).order() - 1) != (src3).dim((src3).order() - 1))) \
    eblerror("incompatible idxs for eloop\n");				\
  DimIter<type0> dst0(src0,(src0).order()-1);				\
  DimIter<type1> dst1(src1,(src1).order()-1);				\
  DimIter<type2> dst2(src2,(src2).order()-1);				\
  DimIter<type3> dst3(src3,(src3).order()-1);				\
  for ( ; dst0.notdone(); ++dst0, ++dst1, ++dst2, ++dst3)

  ////////////////////////////////////////////////////////////////
  // aloop macros: loop over all elements of an idx
  // These macros are somewhat inefficient and should be used as little
  // as possible, or whenever simplicity is preferable to speed.

  // Loops over all elements of an idx. This takes a pointer to
  // the data type of idx elements, and a blank idxiter object:
  // idx_aloop1(data_pointer,idxiter,&idx) { do_stuff(data_pointer); }
  // Example of use: add 1 to all element of m:
  //  idx<double> m(3,4);
  //  ScalarIter<double> p;
  //  idx_aloop1(p,&m) { *p += 1; }
#define idx_aloop1_on(itr0,src0)		\
  for ( ; itr0.notdone(); ++itr0)

  // this loops simultaneously over all elements of 2 idxs.
  // The two idxs can have different structures as long as they have
  // the same total number of elements.
#define idx_aloop2_on(itr0,src0,itr1,src1)	\
  idx_checknelems2_all(src0, src1);		\
  for ( ; itr0.notdone(); ++itr0, ++itr1)

#define idx_aloop3_on(itr0,src0,itr1,src1,itr2,src2)	\
  idx_checknelems3_all(src0, src1, src2);		\
  for (; itr0.notdone(); ++itr0, ++itr1, ++itr2)

  // high level aloop macros.
  // These should be enclosed in braces, to avoid name clashes
#define idx_aloop1(itr0,src0,type0)		\
  ScalarIter<type0> itr0(src0);			\
  idx_aloop1_on(itr0,src0)

#define idx_aloop2(itr0,src0,type0,itr1,src1,type1)	\
  ScalarIter<type0> itr0(src0);				\
  ScalarIter<type1> itr1(src1);				\
  idx_checknelems2_all(src0, src1);			\
  for (; itr0.notdone(); ++itr0, ++itr1)

#define idx_aloop3(itr0,src0,type0,itr1,src1,type1,itr2,src2,type2)	\
  ScalarIter<type0> itr0(src0);						\
  ScalarIter<type1> itr1(src1);						\
  ScalarIter<type2> itr2(src2);						\
  idx_checknelems3_all(src0, src1, src2);				\
  for (; itr0.notdone(); ++itr0, ++itr1, ++itr2)

  ////////////////////////////////////////////////////////////////
#else
  ////////////////////////////////////////////////////////////////
  //NO STL

#define idx_bloop1(dst0,src0,type0)		\
  idxlooper<type0> dst0(src0,0);		\
  for ( ; dst0.notdone(); dst0.next())

#define idx_bloop2(dst0,src0,type0,dst1,src1,type1)	\
  idx_checkdim2_all(src0, src1, 0);			\
  idxlooper<type0> dst0(src0,0);			\
  idxlooper<type1> dst1(src1,0);			\
  for ( ; dst0.notdone(); dst0.next(), dst1.next())

#define idx_bloop3(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2)	\
  idx_checkdim3_all(src0, src1, src2, 0);				\
  idxlooper<type0> dst0(src0,0);					\
  idxlooper<type1> dst1(src1,0);					\
  idxlooper<type2> dst2(src2,0);					\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next())

#define idx_bloop4(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,	\
		   dst3,src3,type3)					\
  idx_checkdim4_all(src0, src1, src2, src3, 0);				\
  idxlooper<type0> dst0(src0,0);					\
  idxlooper<type1> dst1(src1,0);					\
  idxlooper<type2> dst2(src2,0);					\
  idxlooper<type3> dst3(src3,0);					\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next(), dst3.next())

#define idx_bloop5(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,	\
		   dst3,src3,type3,dst4,src4,type4)			\
  idx_checkdim5_all(src0, src1, src2, src3, src4, 0);			\
  idxlooper<type0> dst0(src0,0);					\
  idxlooper<type1> dst1(src1,0);					\
  idxlooper<type2> dst2(src2,0);					\
  idxlooper<type3> dst3(src3,0);					\
  idxlooper<type4> dst4(src4,0);					\
  for ( ; dst0.notdone();						\
	dst0.next(), dst1.next(), dst2.next(), dst3.next(), dst4.next())

#define idx_bloop6(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2,	\
		   dst3,src3,type3,dst4,src4,type4,dst5,src5,type5)	\
  idx_checkdim6_all(src0, src1, src2, src3, src4, src5, 0);		\
  idxlooper<type0> dst0(src0,0);					\
  idxlooper<type1> dst1(src1,0);					\
  idxlooper<type2> dst2(src2,0);					\
  idxlooper<type3> dst3(src3,0);					\
  idxlooper<type4> dst4(src4,0);					\
  idxlooper<type5> dst5(src5,0);					\
  for ( ; dst0.notdone();						\
	dst0.next(), dst1.next(), dst2.next(), dst3.next(), dst4.next(), \
	  dst5.next())
  
  // eloop macros

#define idx_eloop1(dst0,src0,type0)		\
  idxlooper<type0> dst0(src0,src0.order()-1);	\
  for ( ; dst0.notdone(); dst0.next())

#define idx_eloop2(dst0,src0,type0,dst1,src1,type1)			\
  if ((src0).dim((src0).order() - 1) != (src1).dim((src1).order() - 1)) \
    eblerror("incompatible idxs for eloop\n");				\
  idxlooper<type0> dst0(src0,(src0).order()-1);				\
  idxlooper<type1> dst1(src1,(src1).order()-1);				\
  for ( ; dst0.notdone(); dst0.next(), dst1.next())

#define idx_eloop3(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2)\
  if (((src0).dim((src0).order() - 1) != (src1).dim((src1).order() - 1)) \
      || ((src0).dim((src0).order() - 1) != (src2).dim((src2).order() - 1))) \
    eblerror("incompatible idxs for eloop\n");				\
  idxlooper<type0> dst0(src0,(src0).order()-1);				\
  idxlooper<type1> dst1(src1,(src1).order()-1);				\
  idxlooper<type2> dst2(src2,(src2).order()-1);				\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next())

#define idx_eloop4(dst0,src0,type0,dst1,src1,type1,			\
		   dst2,src2,type2,dst3,src3,type3)			\
  if (((src0).dim((src0).order() - 1) != (src1).dim((src1).order() - 1)) \
      || ((src0).dim((src0).order() - 1) != (src2).dim((src2).order() - 1)) \
      || ((src0).dim((src0).order() - 1) != (src3).dim((src3).order() - 1))) \
    eblerror("incompatible idxs for eloop\n");				\
  idxlooper<type0> dst0(src0,(src0).order()-1);				\
  idxlooper<type1> dst1(src1,(src1).order()-1);				\
  idxlooper<type2> dst2(src2,(src2).order()-1);				\
  idxlooper<type3> dst3(src3,(src3).order()-1);				\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next(), dst3.next())

  ////////////////////////////////////////////////////////////////
  // aloop macros: loop over all elements

  // Loops over all elements of an idx. This takes a pointer to
  // the data type of idx elements, and a blank idxiter object:
  // idx_aloop1(data_pointer,idxiter,&idx) { do_stuff(data_pointer); }
  // Example of use: add 1 to all element of m:
  //  idx<double> m(3,4);
  //  idxiter<double> p;
  //  idx_aloop1(p,&m) { *p += 1; }
#define idx_aloop1_on(itr0,src0)			\
  for ( itr0.init(src0); itr0.notdone(); itr0.next())

  // this loops simultaneously over all elements of 2 idxs.
  // The two idxs can have different structures as long as they have
  // the same total number of elements.
#define idx_aloop2_on(itr0,src0,itr1,src1)	\
  idx_checknelems2_all(src0, src1);		\
  for ( itr0.init(src0), itr1.init(src1);	\
	itr0.notdone();				\
	itr0.next(), itr1.next())

#define idx_aloop3_on(itr0,src0,itr1,src1,itr2,src2)		\
  idx_checknelems3_all(src0, src1, src2);			\
  for (itr0.init(src0), itr1.init(src1), itr2.init(src2);	\
       itr0.notdone();						\
       itr0.next(), itr1.next(), itr2.next())

  // high level aloop macros.
  // These should be enclosed in braces, to avoid name clashes
#define idx_aloop1(itr0,src0,type0)		\
  idxiter<type0> itr0;				\
  idx_aloop1_on(itr0,src0)

#define idx_aloop2(itr0,src0,type0,itr1,src1,type1)	\
  idxiter<type0> itr0;					\
  idxiter<type1> itr1;					\
  idx_checknelems2_all(src0, src1);			\
  for (itr0.init(src0), itr1.init(src1);		\
       itr0.notdone();					\
       itr0.next(), itr1.next())

#define idx_aloop3(itr0,src0,type0,itr1,src1,type1,itr2,src2,type2)	\
  idxiter<type0> itr0;							\
  idxiter<type1> itr1;							\
  idxiter<type2> itr2;							\
  idx_checknelems3_all(src0, src1, src2);				\
  for (itr0.init(src0), itr1.init(src1), itr2.init(src2);		\
       itr0.notdone();							\
       itr0.next(), itr1.next(), itr2.next())

#endif // if USING_STL_ITERS, else

  ////////////////////////////////////////////////////////////////
  // idx methods

  template <class T> void idx<T>::growstorage() {
    if ( storage->growsize(spec.footprint()) < 0) {
      eblerror("cannot grow storage (probably out of memory)");
    }
  }

  template <class T> void idx<T>::growstorage_chunk(intg s_chunk){
    if( storage->growsize_chunk(spec.footprint(), s_chunk) < 0) {
      eblerror("cannot grow storage (probably out of memory)");
    }
  }

  /* the constructor doesn't exist and I didn't find one to replace it
     template<typename T>
     void idx<T>::printElems(FILE* filePtr){
     this->printElems(std::ofstream(filePtr));
     }
  */

  template <class T> idx<T>::~idx() {
    DEBUG("idx::destructor %ld\n",long(this));
    storage->unlock();
    if (pidxdim)
      delete pidxdim;
  }

  // fake constructor called by idxlooper constructor
  template <class T> idx<T>::idx(dummyt *dummy) {
    spec.dim = NULL;
    spec.mod = NULL;
    storage = NULL;
    pidxdim = NULL;
  }

  //template <typename T>
  //idx<T>::idx( idx<T>& other )
  //	:storage(other.storage),
  //	 spec(other.spec)
  // {
  //	storage->lock();
  // }
  //
  //template <typename T>
  //idx<T>::idx( const idx<T>& other )
  //	:storage(other.storage),
  //	 spec(other.spec)
  // {
  //	storage->lock();
  // }

  ////////////////////////////////////////////////////////////////
  //! constructors initialized with an array
  
  template <class T> idx<T>::idx(const T *mat, intg s0, intg s1)
    : pidxdim(NULL), spec(0, s0, s1) {
    storage = new Srg<T>();
    growstorage();
    storage->lock();
    memcpy(idx_ptr(), mat, nelements() * sizeof (T));
  }

  ////////////////////////////////////////////////////////////////
  // specific constructors for each number of dimensions

  template <class T> idx<T>::idx() : pidxdim(NULL), spec(0) {
    storage = new Srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T> idx<T>::idx(intg size0) : pidxdim(NULL), spec(0,size0) {
    storage = new Srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T> idx<T>::idx(intg size0, intg size1)
    : pidxdim(NULL), spec(0,size0,size1) {
    storage = new Srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T> idx<T>::idx(intg size0, intg size1, intg size2)
    : pidxdim(NULL), spec(0,size0,size1,size2) {
    storage = new Srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T> 
  idx<T>::idx(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
	      intg s6, intg s7)
    : pidxdim(NULL), spec(0,s0,s1,s2,s3,s4,s5,s6,s7) {
    storage = new Srg<T>();
    growstorage();
    storage->lock();
  }

  template <class T> idx<T>::idx(const idxdim &d)
    : pidxdim(NULL), spec(0, d) {
    storage = new Srg<T>();
    growstorage();
    storage->lock();
  }

  ////////////////////////////////////////////////////////////////
  // constructors from existing Srg and offset

  template <class T> 
  idx<T>::idx(Srg<T> *srg, idxspec &s) : pidxdim(NULL) {
    spec = s;
    storage = srg;
    growstorage();
    storage->lock();
  }

  template <class T>
  idx<T>::idx(Srg<T> *srg, intg o, intg n, intg *dims, intg *mods) 
    : pidxdim(NULL), spec(o, n, dims, mods) {
    storage = srg;
    growstorage();
    storage->lock();
  }

  template <class T> 
  idx<T>::idx(Srg<T> *srg, intg o) : pidxdim(NULL), spec(o) {
    storage = srg;
    growstorage();
    storage->lock();
  }

  template <class T> 
  idx<T>::idx(Srg<T> *srg, intg o, intg size0)
    : pidxdim(NULL), spec(o,size0) {
    storage = srg;
    growstorage();
    storage->lock();
  }

  template <class T> 
  idx<T>::idx(Srg<T> *srg, intg o, intg size0, intg size1)
    : pidxdim(NULL), spec(o,size0,size1) {
    storage = srg;
    growstorage();
    storage->lock();
  }

  template <class T> 
  idx<T>::idx(Srg<T> *srg, intg o, intg size0, intg size1, intg size2)
    : pidxdim(NULL), spec(o,size0,size1,size2) {
    storage = srg;
    growstorage();
    storage->lock();
  }

  template <class T> 
  idx<T>::idx(Srg<T> *srg, intg o, intg s0, intg s1, intg s2, intg s3,
	      intg s4, intg s5, intg s6, intg s7)
    : pidxdim(NULL), spec(o,s0,s1,s2,s3,s4,s5,s6,s7) {
    storage = srg;
    growstorage();
    storage->lock();
  }

  template <class T> 
  idx<T>::idx(Srg<T> *srg, intg o, const idxdim &d) 
    : pidxdim(NULL), spec(o, d) {
    storage = srg;
    growstorage();
    storage->lock();
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
  void idx<T>::resize(intg s0, intg s1, intg s2, intg s3, 
					 intg s4, intg s5, intg s6, intg s7) {
    if (!same_dim(s0,s1,s2,s3,s4,s5,s6,s7)) { // save some time
      spec.resize(s0,s1,s2,s3,s4,s5,s6,s7);
      growstorage();
    }
  }

  template <class T> 
  void idx<T>::resize(const idxdim &d) {
    if (d.order() > spec.ndim) eblerror("cannot change order of idx in resize");
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
    if (!pidxdim)
      pidxdim = new idxdim();
    pidxdim->setdims(spec);
    return *pidxdim;
  }

  ////////////////////////////////////////////////////////////////
  // idx manipulation methods

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
    try {
      spec.transpose_into(&r.spec, d1, d2); 
    } catch(const string &err) {
      cerr << "error in idx " << *this << endl;
      eblerror(err.c_str());
    }
   return r;
  }

  template <class T> idx<T> idx<T>::transpose(int *p) {
    idx<T> r(storage,spec.getoffset());
    try {
      spec.transpose_into(&r.spec, p);
    } catch(const string &err) {
      cerr << "error in idx " << *this << endl;
      eblerror(err.c_str());
    }
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
	return *this; 
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
	if (ldim) delete ldim;
	if (lmod) delete lmod;
	return r;
      }
      else {
	eblerror("view_as_order is not defined when n < current order");
	return *this;
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
    
  ////////////////////////////////////////////////////////////////
  // pointer access methods

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

  // return a pointer to an element of an idx
  // generic function for order>3
  template <class T> T *idx<T>::ptr(intg i0, intg i1, intg i2, intg i3, 
				    intg i4, intg i5, intg i6, intg i7) {
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
      if (k==10) eblerror("idx::get: number of indices and order are different");
      if (k < 0) {
	eblerror("idx::get: wrong number of indices, or negative index");
      } else {
	eblerror("idx::get: index out of bound");
      }
      return NULL;
    }
  }

  ////////////////////////////////////////////////////////////////
  // get methods

  // get element of idx0
  template <class T> T idx<T>::get() {
    idx_checkorder1(*this, 0);
    return (storage->data)[spec.offset];
  }

  // get element of idx1
  template <class T> T idx<T>::get(intg i0) {
    idx_checkorder1(*this, 1);
    if ((i0 < 0) || (i0 >= spec.dim[0])) {
      cerr << "error accessing elt " << i0 << " in " << *this << endl;
      eblerror("index out of bound");
    }
    return (storage->data)[spec.offset + i0*spec.mod[0]];
  }

  // get element of idx2
  template <class T> T idx<T>::get(intg i0, intg i1) {
    idx_checkorder1(*this, 2);
    if (((i0 < 0) || (i0 >= spec.dim[0])) || 
	((i1 < 0) || (i1 >= spec.dim[1]))) {
      cerr << "error accessing elt " << i0 << "x" 
	   << i1 << " in " << *this << endl;
      eblerror("index out of bound");
    }
    return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1]];
  }

  // get element of idx3
  template <class T> T idx<T>::get(intg i0, intg i1, intg i2) {
    idx_checkorder1(*this, 3);
    if (((i0 < 0) || (i0 >= spec.dim[0])) || 
	((i1 < 0) || (i1 >= spec.dim[1])) ||
	((i2 < 0) || (i2 >= spec.dim[2]))) {
      cerr << "error accessing elt " << i0 << "x" 
	   << i1 << "x" << i2 << " in " << *this << endl;
      eblerror("index out of bound");
    }
    return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1] 
			   + i2*spec.mod[2]];
  }

  // get element of an idx of any order
  template <class T> T idx<T>::get(intg i0, intg i1, intg i2, intg i3, 
				   intg i4, intg i5, intg i6, intg i7) {
    return *ptr(i0,i1,i2,i3,i4,i5,i6,i7);
  }

  ////////////////////////////////////////////////////////////////
  // set methods

  // set the element of idx0
  template <class T> T idx<T>::set(T val) {
    idx_checkorder1(*this, 0);
    return (storage->data)[spec.offset] = val;
  }

  // set the element of idx1
  template <class T> T idx<T>::set(T val, intg i0) {
    idx_checkorder1(*this, 1);
    if ((i0 < 0) || (i0 >= spec.dim[0])) eblerror("index 0 out of bound");
    return (storage->data)[spec.offset + i0*spec.mod[0]] = val;
  }

  // set the element of idx2
  template <class T> T idx<T>::set(T val, intg i0, intg i1) {
    idx_checkorder1(*this, 2);
    if ((i0 < 0) || (i0 >= spec.dim[0])) eblerror("index 0 out of bound");
    if ((i1 < 0) || (i1 >= spec.dim[1])) eblerror("index 1 out of bound");
    return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1]] = val;
  }

  // set the element of idx3
  template <class T> T idx<T>::set(T val, intg i0, intg i1, intg i2) {
    idx_checkorder1(*this, 3);
    if ((i0 < 0) || (i0 >= spec.dim[0])) eblerror("index 0 out of bound");
    if ((i1 < 0) || (i1 >= spec.dim[1])) eblerror("index 1 out of bound");
    if ((i2 < 0) || (i2 >= spec.dim[2])) eblerror("index 2 out of bound");
    return (storage->data)[spec.offset + i0*spec.mod[0] + i1*spec.mod[1] 
			   + i2*spec.mod[2]] = val;
  }

  // set an element of an idx of any order.
  template <class T> T idx<T>::set(T val, intg i0, intg i1, intg i2, intg i3, 
				   intg i4, intg i5, intg i6, intg i7) {
    return *ptr(i0,i1,i2,i3,i4,i5,i6,i7) = val;
  }

  ////////////////////////////////////////////////////////////////
  // print methods

  template <typename T>
  void idx<T>::printElems( std::ostream& out ){
    printElems_impl(0, out);
  }

  template <typename T>
  void idx<T>::printElems(){
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
  void idx<T>::printElems_impl( int indent, std::ostream& out ) {
    static const std::string lbrace = "[";
    static const std::string rbrace = "]";
    static const std::string sep = " ";
    std::ostringstream oss;
    for( unsigned int ii = 0; ii < lbrace.length(); ++ii ) {
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
      idx<T> subtensor(storage, spec.offset);
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

  template <class T> void idx<T>::pretty(FILE *f) {
    fprintf(f,"idx: at address %ld\n",(intg)this);
    fprintf(f,"  storage=%ld (size=%ld)\n",(intg)storage,storage->size());
    spec.pretty(f);
  }

  template <class T> void idx<T>::pretty(){
    pretty(cout);
  }

  template <class T> void idx<T>::pretty(std::ostream& out){
    out << "idx: at address " << (intg)this << "\n";
    out << "  storage=" <<  (intg)storage << "(size=" << storage->size();
    out << "\n";
    spec.pretty(out);
  }

  template <class T> int idx<T>::fdump(FILE *f) {
    if (spec.ndim == 0) {
      std::ostringstream oss;
      // fprintf(f,"[@ %g]\n",this->get());
      oss << "[@ " << this->get() << "]\n";
      fputs(oss.str().data(), f);
    } else if (spec.ndim == 1) {
      std::ostringstream oss;
      // fprintf(f,"[");
      oss << "[";
      for (intg i=0; i<dim(0); i += mod(0)) {
	// fprintf(f,"%g ",(storage->data)[spec.offset + i]);
	oss << (storage->data)[spec.offset + i] << " ";
      }
      // fprintf(f,"]\n");
      oss << "]\n";
      fputs(oss.str().data(), f);
    } else {
      fprintf(f,"[");
      { idx_bloop1(p,*this,T) { p.fdump(f); } }
      fprintf(f,"]\n");
    }
    return 0;
  }

  template <class T> 
  std::ostream& operator<<(std::ostream& out, idx<T>& m) {
    out << m.spec;
    return out;
  }

  ////////////////////////////////////////////////////////////////
  // STL-style iterator creators

  template <typename T> 
  typename idx<T>::scalar_iterator idx<T>::scalars_begin(){
    return scalar_iterator(*this);
  }

  template <typename T> 
  typename idx<T>::scalar_iterator idx<T>::scalars_end(){
    return scalar_iterator(*this, false);
  }

  template <typename T> 
  typename idx<T>::reverse_scalar_iterator idx<T>::scalars_rbegin(){
    return reverse_scalar_iterator(*this);
  }

  template <typename T> 
  typename idx<T>::reverse_scalar_iterator idx<T>::scalars_rend(){
    return reverse_scalar_iterator(*this);
  }

  template <typename T>
  typename idx<T>::dimension_iterator idx<T>::dim_begin(int dd){
    return dimension_iterator(*this,dd);
  }

  template <typename T>
  typename idx<T>::dimension_iterator idx<T>::dim_end(int dd){
    return dimension_iterator(*this,dd,false);
  }

//   template <typename T>
//   typename idx<T>::reverse_dimension_iterator idx<T>::dim_rbegin(int dd){
//     return reverse_dimension_iterator(*this,dd);
//   }

//   template <typename T>
//   typename idx<T>::reverse_dimension_iterator idx<T>::dim_rend(int dd){
//     return reverse_dimension_iterator(*this,dd,false);
//   }

#if USING_STL_ITERS == 0
  ////////////////////////////////////////////////////////////////
  // an idxlooper is an iterator for idxs.
  // It is actually a subclass of idx.
  // These are not C++ iterators in the classical sense.

  template <class T> 
  idxlooper<T>::idxlooper(idx<T> &m, int ld) : idx<T>((dummyt*)0) {
    if (m.order() == 0) { // TODO: allow looping once on 0-order idx
      cerr << "cannot loop on idx with order 0. idx is: " << m << endl;
      eblerror("cannot loop on zero-order idx");
    }
    i = 0;
    dimd = m.spec.dim[ld];
    modd = m.spec.mod[ld];
    m.spec.select_into(&(this->spec), ld, i);
    this->storage = m.storage;
    this->storage->lock();
  }

  // like ++
  // CAUTION: this doesn't do array bound checking
  // because we coudn't use a for loop if it did.
  template <class T> T *idxlooper<T>::next() {
    i++;
    this->spec.offset += modd;
    return this->storage->data + this->spec.offset;
  }

  // return true when done.
  template <class T> bool idxlooper<T>::notdone() { return ( i < dimd ); }

  ////////////////////////////////////////////////////////////////
  // a pointer that loops over all elements
  // of an idx

  // empty constructor;
  template <class T> idxiter<T>::idxiter() { }

  template <class T> T *idxiter<T>::init(idx<T> &m) {
    iterand = &m;
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

  template <class T> T *idxiter<T>::next() {
    i++;
    if (d[0] < 0) {
      // contiguous idx
      data++;
    } else {
      // non-contiguous idx
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

  template <class T> bool idxiter<T>::notdone() { return ( i < n ); }

#endif // IF USING_STL_ITERS == 0

  ////////////////////////////////////////////////////////////////
  // idxdim
  
  template <class T>
  idxdim::idxdim(const idx<T> &i) {
    setdims(i.spec);
  }
  
  template <class T>
  void idxdim::setdims(const idx<T> &i) {
    setdims(i.spec);
  }
  
} // namespace ebl

#endif
