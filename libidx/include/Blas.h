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

#ifndef Blas_H
#define Blas_H

#include <math.h>
#include "IdxBlas.h"

#include "Numerics.h"
#include "Idx.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // simple operations

#define idxop_ii(i1,i2,op_idx0, op_idx1, op_contig, op_recursive, op_any) { \
    intg N1=(i1).nelements();						\
    intg N2 =(i2).nelements();						\
    if (N1 != N2)							\
      { ylerror("idx_op: Idxs have different number of elements\n"); }	\
    if ( ((i1).order() == 0) && ((i2).order() == 0) ) {			\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx0;								\
    } else if ( (i1).contiguousp() && (i2).contiguousp() ) {		\
      /* they are both contiguous: call the stride 1 routine */		\
      op_contig;							\
    } else if ( ((i1).order() == 1) && ((i2).order() == 1) ) {		\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx1;								\
    } else if ( same_dim((i1).spec,(i2).spec) ) {			\
      op_recursive;							\
    } else {								\
      /* else, they don't have the same structure: do it "by hand".	\
	 This is slower */						\
      op_any;								\
    }									\
  }

#define idxop_simple_ii(i1,i2,op_idx0, op_idx1, op_contig,		\
			op_recursive, op_any) {				\
    intg N1=(i1).nelements();						\
    intg N2 =(i2).nelements();						\
    if (N1 != N2)							\
      { ylerror("idx_op: Idxs have different number of elements\n"); }	\
    if ( ((i1).order() == 0) && ((i2).order() == 0) ) {			\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx0;								\
    } else if ( (i1).contiguousp() && (i2).contiguousp() ) {		\
      /* they are both contiguous: call the stride 1 routine */		\
      op_contig;							\
    } else if ( ((i1).order() == 1) && ((i2).order() == 1) ) {		\
      /* they are 1D vectors of the same size, use the stride version */ \
      op_idx1;								\
    } else if ( same_dim((i1).spec,(i2).spec) ) {			\
      op_recursive;							\
    } else {								\
      /* else, they don't have the same structure: do it "by hand".	\
	 This is slower */						\
      op_any;								\
    }									\
  }

  ////////////////////////////////////////////////////////////////

  //! check that matrix a and vector x,y are compatible
  //! for a matrix-vector multiplication y <- a.x.
  template<class T> void check_m2dotm1(Idx<T> &m, Idx<T> &x, Idx<T> &y);
  template<class T> void check_m1extm1(Idx<T> &x, Idx<T> &y, Idx<T> &m);

  //! generic copy
  // TODO-0: using slow idx_copy version, others are bugged, debug
  void idx_copy(Idx<double> &src, Idx<double> &dst);
  void idx_copy(Idx<float> &src, Idx<float> &dst);
  template<class T1, class T2> void idx_copy(Idx<T1> &src, Idx<T2> &dst);
  //template<class T> void idx_copy(Idx<T> &src, Idx<T> &dst);

  ////////////////////////////////////////////////////////////////
  //! fill with zero
  template<class T> void idx_clear(Idx<T> &inp);

  //! fill with a value
  template<class T> void idx_fill(Idx<T> &inp, T v);

  //! negate all elements
  template<class T> void idx_minus(Idx<T> &inp, Idx<T> &out);

  //! invert all elements
  template<class T> void idx_inv(Idx<T> &inp, Idx<T> &out);

  //! add two Idx's
  template<class T> void idx_add(Idx<T> &i1, Idx<T> &i2, Idx<T> &out);

  //! subtract two Idx's
  template<class T> void idx_sub(Idx<T> &i1, Idx<T> &i2, Idx<T> &out);

  //! multiply two Idx's
  template<class T> void idx_mul(Idx<T> &i1, Idx<T> &i2, Idx<T> &out);

  //! add a constant to each element:  o1 <- i1+c;
  template<class T> void idx_addc(Idx<T> &inp, T c, Idx<T> &out);

  //! add a constant to each element and accumulate
  //! result: o1 <- o1 + i1+c;
  template<class T> void idx_addcacc(Idx<T> &inp, T c, Idx<T> &out);

  //! multiply all elements by a constant:  o1 <- i1*c;
  template<class T> void idx_dotc(Idx<T> &inp, T c, Idx<T> &out);

  //! multiply all elements by a constant and accumulate
  //! result: o1 <- o1 + i1*c;
  template<class T> void idx_dotcacc(Idx<T> &inp, T c, Idx<T> &out);

  //! set each element of out to +c if corresponding element of inp
  //! is positive, and to -c otherwise.
  template<class T> void idx_signdotc(Idx<T> &inp, T c, Idx<T> &out);

  //! accumulate into each element of out to +c if corresponding element
  //! of inp is positive, and to -c otherwise.
  template<class T> void idx_signdotcacc(Idx<T> &inp, T c, Idx<T> &out);

  //! square of difference of each term:  out <- (i1-i2)^2
  template<class T> void idx_subsquare(Idx<T> &i1, Idx<T> &i2, Idx<T> &out);

  //! compute linear combination of two Idx
  template<class T> 
    void idx_lincomb(Idx<T> &i1, T k1, Idx<T> &i2, T k2, Idx<T> &out);

  //! hyperbolic tangent
  template<class T> void idx_tanh(Idx<T> &inp, Idx<T> &out);

  //! derivative of hyperbolic tangent
  template<class T> void idx_dtanh(Idx<T> &inp, Idx<T> &out);

  //! standard Lush sigmoid
  template<class T> void idx_stdsigmoid(Idx<T> &inp, Idx<T> &out);

  //! derivative of standard Lush sigmoid
  template<class T> void idx_dstdsigmoid(Idx<T> &inp, Idx<T> &out);

  //! absolute value
  template<class T> void idx_abs(Idx<T>& inp, Idx<T>& out);

  ////////////////////////////////////////////////////////////////
  //! sum of all the terms
  template<class T> T idx_sum(Idx<T> &inp, T *out = NULL);
  template<> double idx_sum(Idx<double> &inp, double *out);
  template<> float idx_sum(Idx<float> &inp, float *out);

  //! sum of all the terms, accumulated in Idx0 acc
  template<class T> T idx_sumacc(Idx<T> &inp, Idx<T> &acc);

  ////////////////////////////////////////////////////////////////
  //! sum of square of all the terms
  template<class T> T idx_sumsqr(Idx<T> &inp);

  ////////////////////////////////////////////////////////////////
  // dot products

  //! dot product of two Idx. Returns sum of product of all elements.
  double idx_dot(Idx<double> &i1, Idx<double> &i2);
  float idx_dot(Idx<float> &i1, Idx<float> &i2);
  template <class T> T idx_dot(Idx<T> &i1, Idx<T> &i2);

  //! dot product of two Idx. Accumulate result into Idx0.
  template<class T> void idx_dotacc(Idx<T>& i1, Idx<T>& i2, Idx<T>& o);

  //! matrix-vector multiplication y <- a.x
  void idx_m2dotm1(Idx<double> &a, Idx<double> &x, Idx<double> &y);
  void idx_m2dotm1(Idx<float> &a, Idx<float> &x, Idx<float> &y);

  //! matrix-vector multiplication y <- y + a.x
  void idx_m2dotm1acc(Idx<double> &a, Idx<double> &x, Idx<double> &y);
  void idx_m2dotm1acc(Idx<float> &a, Idx<float> &x, Idx<float> &y);

  //! 4-tensor by 2-matrix multiplication R_ij = sum_kl M1_ijkl * M2_kl
  template<class T> void idx_m4dotm2(Idx<T> &i1, Idx<T> &i2, Idx<T> &o1);

  //! 4-tensor by 2-matrix multiplication with accumulation 
  //! R_ij += sum_kl M1_ijkl * M2_kl
  template<class T> void idx_m4dotm2acc(Idx<T> &i1, Idx<T> &i2, Idx<T> &o1);

  //! multiply vector <m2> by matrix <m1> using square of <m1> elements
  //! M3i += sum_j M1ij^2 * M2j
  template<class T> void idx_m4squdotm2acc(Idx<T> &i1, Idx<T> &i2, Idx<T> &o1);

  //! outer product between matrices. Gives a 4-tensor: R_ijkl = M1_ij * M2_kl
  template<class T> void idx_m2extm2(Idx<T> &i1, Idx<T> &i2, Idx<T> &o1);

  //! outer product between matrices with accumulation. 
  //! Gives a 4-tensor: R_ijkl += M1_ij * M2_kl
  template<class T> void idx_m2extm2acc(Idx<T> &i1, Idx<T> &i2, Idx<T> &o1);

  //! square outer product of <m1> and <m2>. M3_ijkl += M1_ij * (M2_kl)^2
  template<class T> void idx_m2squextm2acc(Idx<T> &i1, Idx<T> &i2, Idx<T> &o1);

  //! returns sum(M1_ij * (M2_ij)^2) in the output Idx0
  template<typename T> void idx_m2squdotm2(Idx<T>& i1, Idx<T>& i2, Idx<T>& o);

  //! accumulates sum(M1_ij * (M2_ij)^2) in the output Idx0
  template<typename T> 
    void idx_m2squdotm2acc(Idx<T>& i1, Idx<T>& i2, Idx<T>& o);

  //! vector-vector outer product a <- x.y'
  void idx_m1extm1(Idx<double> &a, Idx<double> &x, Idx<double> &y);
  void idx_m1extm1(Idx<float> &a, Idx<float> &x, Idx<float> &y);

  //! vector-vector outer product a <- a + x.y'
  void idx_m1extm1acc(Idx<double> &a, Idx<double> &x, Idx<double> &y);
  void idx_m1extm1acc(Idx<float> &a, Idx<float> &x, Idx<float> &y);

  //! square matrix vector multiplication : Yi = sum((Aij)^2 * Xj)
  void idx_m2squdotm1(Idx<double> &a, Idx<double> &x, Idx<double> &y);
  void idx_m2squdotm1(Idx<float> &a, Idx<float> &x, Idx<float> &y);

  //! square matrix vector multiplication : Yi += sum((Aij)^2 * Xj)
  void idx_m2squdotm1acc(Idx<double> &a, Idx<double> &x, Idx<double> &y);
  void idx_m2squdotm1acc(Idx<float> &a, Idx<float> &x, Idx<float> &y);

  //! Aij = Xi * Yj^2
  void idx_m1squextm1(Idx<double> &a, Idx<double> &x, Idx<double> &y);
  void idx_m1squextm1(Idx<float> &a, Idx<float> &x, Idx<float> &y);

  //! Aij += Xi * Yj^2
  void idx_m1squextm1acc(Idx<double> &a, Idx<double> &x, Idx<double> &y);
  void idx_m1squextm1acc(Idx<float> &a, Idx<float> &x, Idx<float> &y);

  //! normalize the colums of a matrix
  void norm_columns(Idx<double> &m);
  void norm_columns(Idx<float> &m);

  //! 2D convolution. all arguments are idx2.
  template<class T> void idx_2dconvol(Idx<T> &in, Idx<T> &kernel, Idx<T> &out);

  ////////////////////////////////////////////////////////////////
  // Flip functions

  //! flip an Idx2 on each dimension
  template<class T> void rev_idx2 (Idx<T> &m);

  //! flip an Idx2 m on each dimension and put the result into n
  template<class T> void rev_idx2_tr (Idx<T> &m, Idx<T> &n);

  ////////////////////////////////////////////////////////////////
  // min/max functions

  //! returns largest element in m
  template<class T> T idx_max(Idx<T> &m);

  //! returns smallest element in m
  template<class T> T idx_min(Idx<T> &m);

  //! returns index of largest element of m.
  template<class T> intg idx_indexmax(Idx<T> &m);

  //! returns index of smallest element of m.
  template<class T> intg idx_indexmin(Idx<T> &m);

  ////////////////////////////////////////////////////////////////
  // sort functions

  //! <m> is a vector, <p> is a vector (same dimension as <m>).
  //! on output, <m> is sorted in descending order, and <p>
  //! is sorted with the same permutation table.
  template<class T1, class T2> void idx_sortdown(Idx<T1> &m, Idx<T2> &p);

  ////////////////////////////////////////////////////////////////

  //! generalized Uclidean distance between <i1> and <i2>,
  //! i.e. the sum of squares of all the differences
  //! between corresponding terms of <i1> and <i2>.
  //! The result is returned by the function.
  template<class T> T idx_sqrdist(Idx<T> &i1, Idx<T> &i2);

  //! generalized Uclidean distance between <i1> and <i2>,
  //! i.e. the sum of squares of all the differences
  //! between corresponding terms of <i1> and <i2>.
  //! The result is assigned into the idx out (of order 0).
  template<class T> void idx_sqrdist(Idx<T> &i1, Idx<T> &i2, Idx<T> &out);

  //! oversample (by repetition) a 2-d matrix
  template<typename T> 
    void idx_m2oversample(Idx<T>& small, intg nlin, intg ncol, Idx<T>& big);

  //! Copy the max of m and each element of i1 into o1
  template<class T> void idx_clip(Idx<T> &i1, T m, Idx<T> &o1);


} // end namespace ebl

#include "Blas.hpp"

#endif
