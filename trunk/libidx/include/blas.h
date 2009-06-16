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

#include <cmath>
#include "idxblas.h"
#include "numerics.h"
#include "idx.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // simple operations

#define idxop_ii(i1,i2,op_idx0, op_idx1, op_contig, op_recursive, op_any) { \
    intg N1=(i1).nelements();						\
    intg N2 =(i2).nelements();						\
    if (N1 != N2)							\
      { eblerror("idx_op: idxs have different number of elements\n"); }	\
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
      { eblerror("idx_op: idxs have different number of elements\n"); }	\
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
  template<class T> void check_m2dotm1(idx<T> &m, idx<T> &x, idx<T> &y);
  template<class T> void check_m1extm1(idx<T> &x, idx<T> &y, idx<T> &m);

  //! generic copy
  // TODO-0: using slow idx_copy version, others are bugged, debug
  void idx_copy(idx<double> &src, idx<double> &dst);
  void idx_copy(idx<float> &src, idx<float> &dst);
  template<class T1, class T2> void idx_copy(idx<T1> &src, idx<T2> &dst);
  //template<class T> void idx_copy(idx<T> &src, idx<T> &dst);

  //! copy src into dst but prevent under and overflow if values in src
  //! are bigger than maximum and minimum values of type T1.
  //! Caution: note that this is slower than a reguler idx_copy.
  template<class T1, class T2> void idx_copy_clip(idx<T1> &src, idx<T2> &dst);

  ////////////////////////////////////////////////////////////////

  //! fill with zero
  template<class T> void idx_clear(idx<T> &inp);

  //! shuffle elements order of dimension d.
  //! if <out> is not null, then the shuffled version of <in> is put directly
  //! into <out> (faster), otherwise a temporary copy of <in> is used
  //! and copied back into in (slower).
  //! Warning: this function assumes that drand is already initialized
  //! (with init_drand(time(NULL)))
  template<class T> void idx_shuffle(idx<T> &in, intg d, idx<T> *out = NULL);

  //! shuffle elements order of dimension d simultaneously of <in1> and <in2>.
  //! the shuffled order will be the same for <in1> and <in2>, which means
  //! of course that they must have the same number of elements in dimension d.
  //! if <out> is not null, then the shuffled version of <in> is put directly
  //! into <out> (faster), otherwise a temporary copy of <in> is used
  //! and copied back into in (slower).
  //! Warning: this function assumes that drand is already initialized
  //! (with init_drand(time(NULL)))
  template<class T1, class T2>
    void idx_shuffle_together(idx<T1> &in1, idx<T2> &in2, intg d,
			      idx<T1> *out1 = NULL, idx<T2> *out2 = NULL);

  //! shuffle elements order of dimension d simultaneously of <in1>, <in2> and
  //! <in3>.
  //! the shuffled order will be the same for <in1>, <in2> and <in3> which means
  //! of course that they must have the same number of elements in dimension d.
  //! if <out> is not null, then the shuffled version of <in> is put directly
  //! into <out> (faster), otherwise a temporary copy of <in> is used
  //! and copied back into in (slower).
  //! Warning: this function assumes that drand is already initialized
  //! (with init_drand(time(NULL)))
  template<class T1, class T2, class T3>
    void idx_shuffle_together(idx<T1> &in1, idx<T2> &in2, idx<T3> &in3,
			      intg d,
			      idx<T1> *out1 = NULL, idx<T2> *out2 = NULL,
			      idx<T3> *out3 = NULL);

  //! fill with a value
  template<class T> void idx_fill(idx<T> &inp, T v);

  //! negate all elements
  template<class T> void idx_minus(idx<T> &inp, idx<T> &out);

  //! invert all elements
  template<class T> void idx_inv(idx<T> &inp, idx<T> &out);

  //! add two idx's
  template<class T> void idx_add(idx<T> &i1, idx<T> &i2, idx<T> &out);

  //! subtract two idx's
  template<class T> void idx_sub(idx<T> &i1, idx<T> &i2, idx<T> &out);

  //! multiply two idx's
  template<class T> void idx_mul(idx<T> &i1, idx<T> &i2, idx<T> &out);

  //! add a constant to each element:  o1 <- i1+c;
  template<class T> void idx_addc(idx<T> &inp, T c, idx<T> &out);

  //! add a constant to each element:  o1 <- i1+c;
  //! This version bounds the new values to minimum and maximum of type T,
  //! for example in the case of an underflow with type ubyte, the new value
  //! will be 0, for an overflow it will be 255. 
  //! Caution: This is slower than a regular idx_addc.
  //! Warning: bounding not working when T=double (TODO)
  template<class T> void idx_addc_bounded(idx<T> &inp, T c, idx<T> &out);

  //! subtract a constant to each element:  o1 <- i1-c;
  //! This version bounds the new values to minimum and maximum of type T,
  //! for example in the case of an underflow with type ubyte, the new value
  //! will be 0, for an overflow it will be 255. 
  //! Caution: This is slower than a regular idx_subc.
  //! Warning: bounding not working when T=double (TODO)
  template<class T> void idx_subc_bounded(idx<T> &inp, T c, idx<T> &out);

  //! add a constant to each element and accumulate
  //! result: o1 <- o1 + i1+c;
  template<class T> void idx_addcacc(idx<T> &inp, T c, idx<T> &out);

  //! multiply all elements by a constant:  o1 <- i1*c;
  template<class T> void idx_dotc(idx<T> &inp, T c, idx<T> &out);

  //! multiply all elements by a constant:  o1 <- i1*c;
  //! This version bounds the new values to minimum and maximum of type T,
  //! for example in the case of an underflow with type ubyte, the new value
  //! will be 0, for an overflow it will be 255. 
  //! Caution: This is slower than a regular idx_dotc.
  //! Warning: bounding not working when T=double (TODO)
  template<class T> void idx_dotc_bounded(idx<T> &inp, T c, idx<T> &out);

  //! multiply all elements by a constant and accumulate
  //! result: o1 <- o1 + i1*c;
  template<class T> void idx_dotcacc(idx<T> &inp, T c, idx<T> &out);

  //! set each element of out to +c if corresponding element of inp
  //! is positive, and to -c otherwise.
  template<class T> void idx_signdotc(idx<T> &inp, T c, idx<T> &out);

  //! accumulate into each element of out to +c if corresponding element
  //! of inp is positive, and to -c otherwise.
  template<class T> void idx_signdotcacc(idx<T> &inp, T c, idx<T> &out);

  //! square of difference of each term:  out <- (i1-i2)^2
  template<class T> void idx_subsquare(idx<T> &i1, idx<T> &i2, idx<T> &out);

  //! compute linear combination of two idx
  template<class T> 
    void idx_lincomb(idx<T> &i1, T k1, idx<T> &i2, T k2, idx<T> &out);

  //! hyperbolic tangent
  template<class T> void idx_tanh(idx<T> &inp, idx<T> &out);

  //! derivative of hyperbolic tangent
  template<class T> void idx_dtanh(idx<T> &inp, idx<T> &out);

  //! standard Lush sigmoid
  template<class T> void idx_stdsigmoid(idx<T> &inp, idx<T> &out);

  //! derivative of standard Lush sigmoid
  template<class T> void idx_dstdsigmoid(idx<T> &inp, idx<T> &out);

  //! absolute value
  template<class T> void idx_abs(idx<T>& inp, idx<T>& out);

  //! accumulates -c in <out> if <in> is less than <th>, c if more than <th>,
  //! 0 otherwise (this is used as bprop for sumabs).
  template<class T> void idx_thresdotc_acc(idx<T>& in, T c, T th, idx<T>& out);
  
  //! if input is less than th, assign th.
  template<class T> void idx_threshold(idx<T>& in, T th, idx<T>& out);
  
  //! if input is less than th, assign value.
  template<class T> void idx_threshold(idx<T>& in, T th, T value, idx<T>& out);
  
  //! takes the square root of in and puts it in out.
  template<class T> void idx_sqrt(idx<T>& in, idx<T>& out);
  
  ////////////////////////////////////////////////////////////////
  // sums

  //! returns the sum of all the terms, and optionally puts the result
  //! in out if passed.
  template<class T> T idx_sum(idx<T> &inp, T *out = NULL);
  template<> double idx_sum(idx<double> &inp, double *out);
  template<> float idx_sum(idx<float> &inp, float *out);

  //! sum of all absolute values of the terms, and optionally puts the result
  //! in out if passed.
  template<class T> T idx_sumabs(idx<T> &inp, T *out = NULL);
  
  //! sum of all the terms, accumulated in idx0 acc
  template<class T> T idx_sumacc(idx<T> &inp, idx<T> &acc);

  //! sum of all absolute values of the terms, accumulated in idx0 acc
  template<class T> T idx_sumabs(idx<T> &inp, idx<T> &acc);

  //! sum of square of all the terms
  template<class T> T idx_sumsqr(idx<T> &in);

  //! l2 norm of an idx, seen as a vector.
  //! equivalent to the square root of idx_sumsqr
  template<class T> T idx_l2norm(idx<T> &in);

  //! returns the mean of all the terms, and optionally puts the result
  //! in out if passed.
  template<class T> T idx_mean(idx<T> &inp, T *out = NULL);

  //! removes the mean of <in> and divide by the standard deviation.
  //! reuse <mean> if not null, otherwise recompute it.
  template<class T> void idx_std_normalize(idx<T> &in, T *mean = NULL);
  
  ////////////////////////////////////////////////////////////////
  // dot products

  //! dot product of two idx. Returns sum of product of all elements.
  double idx_dot(idx<double> &i1, idx<double> &i2);
  float idx_dot(idx<float> &i1, idx<float> &i2);
  template <class T> T idx_dot(idx<T> &i1, idx<T> &i2);

  //! dot product of two idx. Accumulate result into idx0.
  template<class T> void idx_dotacc(idx<T>& i1, idx<T>& i2, idx<T>& o);

  //! matrix-vector multiplication y <- a.x
  void idx_m2dotm1(idx<double> &a, idx<double> &x, idx<double> &y);
  void idx_m2dotm1(idx<float> &a, idx<float> &x, idx<float> &y);

  //! matrix-vector multiplication y <- y + a.x
  void idx_m2dotm1acc(idx<double> &a, idx<double> &x, idx<double> &y);
  void idx_m2dotm1acc(idx<float> &a, idx<float> &x, idx<float> &y);

  //! 4-tensor by 2-matrix multiplication R_ij = sum_kl M1_ijkl * M2_kl
  template<class T> void idx_m4dotm2(idx<T> &i1, idx<T> &i2, idx<T> &o1);

  //! 4-tensor by 2-matrix multiplication with accumulation 
  //! R_ij += sum_kl M1_ijkl * M2_kl
  template<class T> void idx_m4dotm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1);

  //! multiply vector <m2> by matrix <m1> using square of <m1> elements
  //! M3i += sum_j M1ij^2 * M2j
  template<class T> void idx_m4squdotm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1);

  //! outer product between matrices. Gives a 4-tensor: R_ijkl = M1_ij * M2_kl
  template<class T> void idx_m2extm2(idx<T> &i1, idx<T> &i2, idx<T> &o1);

  //! outer product between matrices with accumulation. 
  //! Gives a 4-tensor: R_ijkl += M1_ij * M2_kl
  template<class T> void idx_m2extm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1);

  //! square outer product of <m1> and <m2>. M3_ijkl += M1_ij * (M2_kl)^2
  template<class T> void idx_m2squextm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1);

  //! returns sum(M1_ij * (M2_ij)^2) in the output idx0
  template<typename T> void idx_m2squdotm2(idx<T>& i1, idx<T>& i2, idx<T>& o);

  //! accumulates sum(M1_ij * (M2_ij)^2) in the output idx0
  template<typename T> 
    void idx_m2squdotm2acc(idx<T>& i1, idx<T>& i2, idx<T>& o);

  //! vector-vector outer product a <- x.y'
  void idx_m1extm1(idx<double> &a, idx<double> &x, idx<double> &y);
  void idx_m1extm1(idx<float> &a, idx<float> &x, idx<float> &y);

  //! vector-vector outer product a <- a + x.y'
  void idx_m1extm1acc(idx<double> &a, idx<double> &x, idx<double> &y);
  void idx_m1extm1acc(idx<float> &a, idx<float> &x, idx<float> &y);

  //! square matrix vector multiplication : Yi = sum((Aij)^2 * Xj)
  void idx_m2squdotm1(idx<double> &a, idx<double> &x, idx<double> &y);
  void idx_m2squdotm1(idx<float> &a, idx<float> &x, idx<float> &y);

  //! square matrix vector multiplication : Yi += sum((Aij)^2 * Xj)
  void idx_m2squdotm1acc(idx<double> &a, idx<double> &x, idx<double> &y);
  void idx_m2squdotm1acc(idx<float> &a, idx<float> &x, idx<float> &y);

  //! Aij = Xi * Yj^2
  void idx_m1squextm1(idx<double> &a, idx<double> &x, idx<double> &y);
  void idx_m1squextm1(idx<float> &a, idx<float> &x, idx<float> &y);

  //! Aij += Xi * Yj^2
  void idx_m1squextm1acc(idx<double> &a, idx<double> &x, idx<double> &y);
  void idx_m1squextm1acc(idx<float> &a, idx<float> &x, idx<float> &y);

  //! normalize the colums of a matrix
  void norm_columns(idx<double> &m);
  void norm_columns(idx<float> &m);

  //! 2D convolution. all arguments are idx2.
  template<class T> void idx_2dconvol(idx<T> &in, idx<T> &kernel, idx<T> &out);

  ////////////////////////////////////////////////////////////////
  // Flip functions

  //! flip an idx2 on each dimension
  template<class T> void rev_idx2 (idx<T> &m);

  //! flip an idx2 m on each dimension and put the result into n
  template<class T> void rev_idx2_tr (idx<T> &m, idx<T> &n);

  ////////////////////////////////////////////////////////////////
  // min/max functions

  //! returns largest element in m
  template<class T> T idx_max(idx<T> &m);

  //! returns smallest element in m
  template<class T> T idx_min(idx<T> &m);

  //! returns index of largest element of m.
  template<class T> intg idx_indexmax(idx<T> &m);

  //! returns index of smallest element of m.
  template<class T> intg idx_indexmin(idx<T> &m);

  ////////////////////////////////////////////////////////////////
  // sort functions

  //! <m> is a vector, <p> is a vector (same dimension as <m>).
  //! on output, <m> is sorted in descending order, and <p>
  //! is sorted with the same permutation table.
  template<class T1, class T2> void idx_sortdown(idx<T1> &m, idx<T2> &p);

  ////////////////////////////////////////////////////////////////

  //! generalized Uclidean distance between <i1> and <i2>,
  //! i.e. the sum of squares of all the differences
  //! between corresponding terms of <i1> and <i2>.
  //! The result is returned by the function.
  template<class T> T idx_sqrdist(idx<T> &i1, idx<T> &i2);

  //! generalized Uclidean distance between <i1> and <i2>,
  //! i.e. the sum of squares of all the differences
  //! between corresponding terms of <i1> and <i2>.
  //! The result is assigned into the idx out (of order 0).
  template<class T> void idx_sqrdist(idx<T> &i1, idx<T> &i2, idx<T> &out);

  //! oversample (by repetition) a 2-d matrix
  template<typename T> 
    void idx_m2oversample(idx<T>& small, intg nlin, intg ncol, idx<T>& big);

  //! Copy the max of m and each element of i1 into o1
  template<class T> void idx_clip(idx<T> &i1, T m, idx<T> &o1);


} // end namespace ebl

#include "blas.hpp"

#endif
