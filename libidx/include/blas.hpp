#include <algorithm>

using namespace std;

namespace ebl {

#if USING_STL_ITERS == 0

  template<class T> void idx_clear(idx<T> &inp) {
    idxiter<T> pinp;
    idx_aloop1_on(pinp,inp) { *pinp = 0; }
  }

  template<class T> void idx_fill(idx<T> &inp, T v) {
    idxiter<T> pinp;
    idx_aloop1_on(pinp,inp) { *pinp = v; }
  }

  template<class T> void idx_minus(idx<T> &inp, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = - *pinp; }
  }

  template<class T> void idx_inv(idx<T> &inp, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = 1 / *pinp; }
  }

  template<class T> void idx_sub(idx<T> &i1, idx<T> &i2, idx<T> &out) {
    idxiter<T> pi1, pi2; idxiter<T> pout;
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { *pout = *pi1 - *pi2; }
  }

  template<class T> void idx_add(idx<T> &i1, idx<T> &i2, idx<T> &out) {
    idxiter<T> pi1, pi2; idxiter<T> pout;
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { *pout = *pi1 + *pi2; }
  }

  template<class T> void idx_mul(idx<T> &i1, idx<T> &i2, idx<T> &out) {
    idxiter<T> pi1, pi2; idxiter<T> pout;
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { *pout = (*pi1) * (*pi2); }
  }

  template<class T> void idx_addc(idx<T> &inp, T c, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = *pinp + c; }
  }

  template<class T> void idx_addc_bounded(idx<T> &inp, T c, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout; double val;
    idx_aloop2_on(pinp,inp,pout,out) { 
      val = *pinp + c;
      // prevent under and overflow
      *pout = MAX(std::numeric_limits<T>::min(), 
		  MIN(std::numeric_limits<T>::max(), val));
    }
  }

  template<class T> void idx_subc_bounded(idx<T> &inp, T c, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout; double val;
    idx_aloop2_on(pinp,inp,pout,out) { 
      val = *pinp - c;
      // prevent under and overflow
      *pout = MAX(std::numeric_limits<T>::min(), 
		  MIN(std::numeric_limits<T>::max(), val));
    }
  }

  template<class T> void idx_addcacc(idx<T> &inp, T c, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout += *pinp + c; }
  }

  template<class T> void idx_dotc(idx<T> &inp, T c, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = *pinp * c; }
  }

  template<class T> void idx_dotc_bounded(idx<T> &inp, T c, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout; double val;
    idx_aloop2_on(pinp,inp,pout,out) { 
      val = *pinp * c;
      // prevent under and overflow
      *pout = MAX(std::numeric_limits<T>::min(), 
		  MIN(std::numeric_limits<T>::max(), val));
    }
  }

  template<class T> void idx_dotcacc(idx<T> &inp, T c, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout += *pinp * c; }
  }

  template<class T> void idx_signdotc(idx<T> &inp, T c, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (*pinp<0)?-c:c; }
  }

  template<class T> void idx_signdotcacc(idx<T> &inp, T c, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout += (*pinp<0)?-c:c; }
  }

  template<class T> void idx_subsquare(idx<T> &i1, idx<T> &i2, idx<T> &out) {
    idxiter<T> pi1; idxiter<T> pi2; idxiter<T> pout;
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { T d = *pi1 - *pi2; *pout += d*d; }
  }

  // not very efficient. There must be a more parallel way of doing this
  template<class T> void idx_lincomb(idx<T> &i1, T k1, idx<T> &i2, T k2, 
				     idx<T> &out) {
    idxiter<T> pi1; idxiter<T> pi2; idxiter<T> pout;
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { *pout =  k1*(*pi1) + k2*(*pi2); }
  }

  template<class T> void idx_tanh(idx<T> &inp, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(tanh((double)*pinp)); }
  }

  template<class T> void idx_dtanh(idx<T> &inp, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(dtanh((double)*pinp)); }
  }

  template<class T> void idx_stdsigmoid(idx<T> &inp, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(stdsigmoid((double)*pinp)); }
  }

  template<class T> void idx_dstdsigmoid(idx<T> &inp, idx<T> &out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(dstdsigmoid((double)*pinp)); }
  }

  template<class T> void idx_abs(idx<T>& inp, idx<T>& out) {
    idxiter<T> pinp; idxiter<T> pout;
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(fabs((double)*pinp)); }
  }

  // there is a much faster and parallel way
  // of doing this using a tree.
  template<class T> T idx_sum(idx<T> &inp, T *out) {
    T z = 0;
    idxiter<T> pinp;
    idx_aloop1_on(pinp,inp) { z += *pinp; }
    if (out != NULL) {
      *out += z;
      return *out;
    }
    return z;
  }

  // there is a much faster and parallel way
  // of doing this using a tree.
  template<class T> T idx_sumsqr(idx<T> &inp) {
    T z = 0;
    idxiter<T> pinp;
    idx_aloop1_on(pinp,inp) { z += (*pinp)*(*pinp); }
    return z;
  }

  ////////////////////////////////////////////////////////////////////////
#else

  template<class T> void idx_clear(idx<T> &inp) {
    ScalarIter<T> pinp(inp);
    idx_aloop1_on(pinp,inp) { *pinp = 0; }
  }

  template<class T> void idx_fill(idx<T> &inp, T v) {
    ScalarIter<T> pinp(inp);
    idx_aloop1_on(pinp,inp) { *pinp = v; }
  }

  template<class T> void idx_minus(idx<T> &inp, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout = - *pinp; }
  }

  template<class T> void idx_inv(idx<T> &inp, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout = 1 / *pinp; }
  }

  template<class T> void idx_sub(idx<T> &i1, idx<T> &i2, idx<T> &out) {
    ScalarIter<T> pi1(i1), pi2(i2); ScalarIter<T> pout(out);
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { *pout = *pi1 - *pi2; }
  }

  template<class T> void idx_add(idx<T> &i1, idx<T> &i2, idx<T> &out) {
    ScalarIter<T> pi1(i1), pi2(i2); ScalarIter<T> pout(out);
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { *pout = *pi1 + *pi2; }
  }

  template<class T> void idx_mul(idx<T> &i1, idx<T> &i2, idx<T> &out) {
    ScalarIter<T> pi1(i1), pi2(i2); ScalarIter<T> pout(out);
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { *pout = (*pi1) * (*pi2); }
  }

  template<class T> void idx_addc(idx<T> &inp, T c, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) {
      *pout = *pinp + c; }
  }

  template<class T> void idx_addcacc(idx<T> &inp, T c, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout += *pinp + c; }
  }

  template<class T> void idx_dotc(idx<T> &inp, T c, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout = *pinp * c; }
  }

  template<class T> void idx_dotcacc(idx<T> &inp, T c, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout += *pinp * c; }
  }

  template<class T> void idx_signdotc(idx<T> &inp, T c, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (*pinp<0)?-c:c; }
  }

  template<class T> void idx_signdotcacc(idx<T> &inp, T c, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout += (*pinp<0)?-c:c; }
  }

  template<class T> void idx_subsquare(idx<T> &i1, idx<T> &i2, idx<T> &out) {
    ScalarIter<T> pi1; ScalarIter<T> pi2; ScalarIter<T> pout(out);
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { T d = *pi1 - *pi2; *pout += d*d; }
  }

  // not very efficient. There must be a more parallel way of doing this
  template<class T> void idx_lincomb(idx<T> &i1, T k1, idx<T> &i2, T k2, idx<T> &out) {
    ScalarIter<T> pi1(i1); ScalarIter<T> pi2(i2); ScalarIter<T> pout(out);
    idx_aloop3_on(pi1,i1,pi2,i2,pout,out) { *pout =  k1*(*pi1) + k2*(*pi2); }
  }

  template<class T> void idx_tanh(idx<T> &inp, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(tanh((double)*pinp)); }
  }

  template<class T> void idx_dtanh(idx<T> &inp, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(dtanh((double)*pinp)); }
  }

  template<class T> void idx_stdsigmoid(idx<T> &inp, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(stdsigmoid((double)*pinp)); }
  }

  template<class T> void idx_dstdsigmoid(idx<T> &inp, idx<T> &out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(dstdsigmoid((double)*pinp)); }
  }

  template<class T> void idx_abs(idx<T>& inp, idx<T>& out) {
    ScalarIter<T> pinp(inp); ScalarIter<T> pout(out);
    idx_aloop2_on(pinp,inp,pout,out) { *pout = (T)(fabs((double)*pinp)); }
  }
  // there is a much faster and parallel way
  // of doing this using a tree.
  template<class T> T idx_sum(idx<T> &inp, T *out) {
    T z = 0;
    ScalarIter<T> pinp(inp);
    idx_aloop1_on(pinp,inp) { z += *pinp; }
    if (out != NULL) {
      *out += z;
      return *out;
    }
    return z;
  }

  // there is a much faster and parallel way
  // of doing this using a tree.
  template<class T> T idx_sumsqr(idx<T> &inp) {
    T z = 0;
    ScalarIter<T> pinp(inp);
    idx_aloop1_on(pinp,inp) { z += (*pinp)*(*pinp); }
    return z;
  }

#endif

  // generic copy for two different types.
  //template<class T1, class T2> void idx_copy(idx<T1> &src, idx<T2> &dst) {
  ////  idxiter<T1> isrc;
  ////  idxiter<T2> idst;
  ////  idx_aloop2_on(isrc, src, idst, dst) { *idst = (T2)(*isrc); }
  //  {idx_aloop2(isrc, src, T1, idst, dst, T2) { *idst = (T2)(*isrc); }}
  //}

  //// generic copy for two different types.
  //template<class T1, class T2> void idx_copy(idx<T1> &src, idx<T2> &dst) {
  ////  idxiter<T1> isrc;
  ////  idxiter<T2> idst;
  ////  idx_aloop2_on(isrc, src, idst, dst) { *idst = (T2)(*isrc); }
  //  {idx_aloop2(isrc, src, T1, idst, dst, T2) { *idst = (T2)(*isrc); }}
  //}

  template<class T1, class T2> void idx_copy(idx<T1> &src, idx<T2> &dst){
    idx_aloop2(isrc, src, T1, idst, dst, T2) { *idst = (T2)(*isrc); }
  }

  template<class T1, class T2> void idx_copy_clip(idx<T1> &src, idx<T2> &dst){
    idx_aloop2(isrc, src, T1, idst, dst, T2) { 
      *idst = (T2) MAX(std::numeric_limits<T2>::min(), 
		       MIN(std::numeric_limits<T2>::max(), *isrc));
    }
  }

  // generic copy for the same type.
  template<class T> void idx_copy(idx<T> &src, idx<T> &dst) {
    intg N1=src.nelements();
    intg N2 =dst.nelements();
    if (N1 != N2) { ylerror("idx_op: idxs have different number of elements\n"); }
    if ( (src.order() == 0) && (dst.order() == 0) ) {
      *(dst.idx_ptr()) = *(src.idx_ptr());
    } else if ( src.contiguousp() && dst.contiguousp() ) {
      /* they are both contiguous: call the stride 1 routine */
      memcpy(dst.idx_ptr(), src.idx_ptr(), N1 * sizeof(T));
    } else {
      // else, they don't have the same structure: do it "by hand". This is slower
      {idx_aloop2(isrc, src, T, idst, dst, T) { *idst = *isrc; }}
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Functions without iterators

  template<class T> T idx_sumacc(idx<T> &inp, idx<T> &acc) {
    // acc must be of order 0.
    if (acc.order() != 0)
      ylerror("expecting an idx0 as output");
    return idx_sum(inp, acc.ptr());
  }

  template<class T> void rev_idx2 (idx<T> &m) {
    if (m.order() != 2)
      ylerror("Expecting idx of order 2");
    T tmp, *p = m.ptr();
    intg size = m.dim(0) * m.dim(1);
    intg i;

    for (i = 0; i < size/2; i++) {
      tmp = p[i];
      p[i] = p[size-i-1];
      p[size-i-1] = tmp;
    }
  }

  template<class T> void rev_idx2_tr (idx<T> &m, idx<T> &n) {
    if ((m.order() != 2) || (n.order() != 2))
      ylerror("Expecting idx of order 2");
    T *p = m.ptr();
    T *q = n.ptr();
    intg size = m.dim(0) * m.dim(1);
    intg i;

    for (i=0; i < size; i++) {
      q[i] = p[size-i-1];
    }
  }

  // TODO-0 write specialized blas version in cpp
  template<class T> void idx_m4dotm2(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
    idx_checkorder3(i1, 4, i2, 2, o1, 2); // check for compatible orders
    if ((i1.dim(0) != o1.dim(0)) || (i1.dim(1) != o1.dim(1)) 
	|| (i1.dim(2) != i2.dim(0)) || (i1.dim(3) != i2.dim(1)))
      ylerror("incompatible dimensions");
    T *c1, *c1_2;
    T *c2, *c2_0;
    T *c1_0, *c1_1;
    T *ker;
    intg c1_m2 = (i1).mod(2), c2_m0 = (i2).mod(0);
    intg c1_m3 = (i1).mod(3), c2_m1 = (i2).mod(1);
    intg k,l, kmax = (i2).dim(0), lmax = (i2).dim(1);
    T *d1_0, *d1, f;
    intg c1_m0 = (i1).mod(0), d1_m0 = (o1).mod(0);
    intg c1_m1 = (i1).mod(1), d1_m1 = (o1).mod(1);
    intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
    c1_0 = i1.idx_ptr();
    ker = i2.idx_ptr();
    d1_0 = o1.idx_ptr();
    for (i=0; i<imax; i++) {
      c1_1 = c1_0;
      d1 = d1_0;
      for (j=0; j<jmax; j++) {
	f = 0;
	c1_2 = c1_1;
	c2_0 = ker;
	for (k=0; k<kmax; k++) {
	  c1 = c1_2;
	  c2 = c2_0;
	  for (l=0; l<lmax; l++) {
	    f += (*c1)*(*c2);
	    c1 += c1_m3;
	    c2 += c2_m1;
	  }
	  c1_2 += c1_m2;
	  c2_0 += c2_m0;
	}
	*d1 = f;
	d1 += d1_m1;
	c1_1 += c1_m1;
      }
      d1_0 += d1_m0;
      c1_0 += c1_m0;
    }
  }

  // TODO-0 write specialized blas version in cpp
  template<class T> void idx_m4dotm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
    idx_checkorder3(i1, 4, i2, 2, o1, 2); // check for compatible orders
    if ((i1.dim(0) != o1.dim(0)) || (i1.dim(1) != o1.dim(1)) 
	|| (i1.dim(2) != i2.dim(0)) || (i1.dim(3) != i2.dim(1)))
      ylerror("incompatible dimensions");
    T *c1, *c1_2;
    T *c2, *c2_0;
    T *c1_0, *c1_1;
    T *ker;
    intg c1_m2 = (i1).mod(2), c2_m0 = (i2).mod(0);
    intg c1_m3 = (i1).mod(3), c2_m1 = (i2).mod(1);
    intg k,l, kmax = (i2).dim(0), lmax = (i2).dim(1);
    T *d1_0, *d1, f;
    intg c1_m0 = (i1).mod(0), d1_m0 = (o1).mod(0);
    intg c1_m1 = (i1).mod(1), d1_m1 = (o1).mod(1);
    intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
    c1_0 = i1.idx_ptr();
    ker = i2.idx_ptr();
    d1_0 = o1.idx_ptr();
    for (i=0; i<imax; i++) {
      c1_1 = c1_0;
      d1 = d1_0;
      for (j=0; j<jmax; j++) {
	f = *d1;
	c1_2 = c1_1;
	c2_0 = ker;
	for (k=0; k<kmax; k++) {
	  c1 = c1_2;
	  c2 = c2_0;
	  for (l=0; l<lmax; l++) {
	    f += (*c1)*(*c2);
	    c1 += c1_m3;
	    c2 += c2_m1;
	  }
	  c1_2 += c1_m2;
	  c2_0 += c2_m0;
	}
	*d1 = f;
	d1 += d1_m1;
	c1_1 += c1_m1;
      }
      d1_0 += d1_m0;
      c1_0 += c1_m0;
    }
  }

  template<class T> void idx_m4squdotm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
    idx_checkorder3(i1, 4, i2, 2, o1, 2);
    T *c1, *c1_2;
    T *c2, *c2_0;
    T *c1_0, *c1_1;
    T *ker;
    intg c1_m2 = (i1).mod(2), c2_m0 = (i2).mod(0);
    intg c1_m3 = (i1).mod(3), c2_m1 = (i2).mod(1);
    intg k,l, kmax = (i2).dim(0), lmax = (i2).dim(1);
    T *d1_0, *d1, f;
    intg c1_m0 = (i1).mod(0), d1_m0 = (o1).mod(0);
    intg c1_m1 = (i1).mod(1), d1_m1 = (o1).mod(1);
    intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
    c1_0 = i1.idx_ptr();
    ker = i2.idx_ptr();
    d1_0 = o1.idx_ptr();
    for (i=0; i<imax; i++) {
      c1_1 = c1_0;
      d1 = d1_0;
      for (j=0; j<jmax; j++) {
	f = *d1;
	c1_2 = c1_1;
	c2_0 = ker;
	for (k=0; k<kmax; k++) {
	  c1 = c1_2;
	  c2 = c2_0;
	  for (l=0; l<lmax; l++) {
	    f += (*c1)*(*c1)*(*c2);
	    c1 += c1_m3;
	    c2 += c2_m1;
	  }
	  c1_2 += c1_m2;
	  c2_0 += c2_m0;
	}
	*d1 = f;
	d1 += d1_m1;
	c1_1 += c1_m1;
      }
      d1_0 += d1_m0;
      c1_0 += c1_m0;
    }
  }

  template<class T> void idx_m2extm2(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
    idx_checkorder3(i1, 2, i2, 2, o1, 4);
    T *c2_0, *c2_1;
    T *d1_2, *d1_3;
    T *d1_0, *d1_1;
    T *c1_0, *c1_1;
    T *ker;
    intg c2_m0 = (i2).mod(0), c2_m1 = (i2).mod(1);
    intg d1_m2 = (o1).mod(2), d1_m3 = (o1).mod(3);
    intg c1_m0 = (i1).mod(0), c1_m1 = (i1).mod(1);
    intg d1_m0 = (o1).mod(0), d1_m1 = (o1).mod(1);
    intg k,l, lmax = (o1).dim(3), kmax = (o1).dim(2);
    intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
    c1_0 = i1.idx_ptr();
    ker = i2.idx_ptr();
    d1_0 = o1.idx_ptr();
    for (i=0; i<imax; i++) {
      d1_1 = d1_0;
      c1_1 = c1_0;
      for (j=0; j<jmax; j++) {
	d1_2 = d1_1;
	c2_0 = ker;
	for (k=0; k<kmax; k++) {
	  d1_3 = d1_2;
	  c2_1 = c2_0;
	  for (l=0; l<lmax; l++) {
	    *d1_3 = (*c1_1)*(*c2_1);
	    d1_3 += d1_m3;
	    c2_1 += c2_m1;
	  }
	  d1_2 += d1_m2;
	  c2_0 += c2_m0;
	}
	d1_1 += d1_m1;
	c1_1 += c1_m1;
      }
      d1_0 += d1_m0;
      c1_0 += c1_m0;
    }
  }

  template<class T> void idx_m2extm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
    idx_checkorder3(i1, 2, i2, 2, o1, 4);
    T *c2_0, *c2_1;
    T *d1_2, *d1_3;
    T *d1_0, *d1_1;
    T *c1_0, *c1_1;
    T *ker;
    intg c2_m0 = (i2).mod(0), c2_m1 = (i2).mod(1);
    intg d1_m2 = (o1).mod(2), d1_m3 = (o1).mod(3);
    intg c1_m0 = (i1).mod(0), c1_m1 = (i1).mod(1);
    intg d1_m0 = (o1).mod(0), d1_m1 = (o1).mod(1);
    intg k,l, lmax = (o1).dim(3), kmax = (o1).dim(2);
    intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
    c1_0 = i1.idx_ptr();
    ker = i2.idx_ptr();
    d1_0 = o1.idx_ptr();
    for (i=0; i<imax; i++) {
      d1_1 = d1_0;
      c1_1 = c1_0;
      for (j=0; j<jmax; j++) {
	d1_2 = d1_1;
	c2_0 = ker;
	for (k=0; k<kmax; k++) {
	  d1_3 = d1_2;
	  c2_1 = c2_0;
	  for (l=0; l<lmax; l++) {
	    *d1_3 += (*c1_1)*(*c2_1);
	    d1_3 += d1_m3;
	    c2_1 += c2_m1;
	  }
	  d1_2 += d1_m2;
	  c2_0 += c2_m0;
	}
	d1_1 += d1_m1;
	c1_1 += c1_m1;
      }
      d1_0 += d1_m0;
      c1_0 += c1_m0;
    }
  }

  template<class T> void idx_m2squextm2acc(idx<T> &i1, idx<T> &i2, idx<T> &o1) {
    idx_checkorder3(i1, 2, i2, 2, o1, 4);
    T *c2_0, *c2_1;
    T *d1_2, *d1_3;
    T *d1_0, *d1_1;
    T *c1_0, *c1_1;
    T *ker;
    intg c2_m0 = (i2).mod(0), c2_m1 = (i2).mod(1);
    intg d1_m2 = (o1).mod(2), d1_m3 = (o1).mod(3);
    intg c1_m0 = (i1).mod(0), c1_m1 = (i1).mod(1);
    intg d1_m0 = (o1).mod(0), d1_m1 = (o1).mod(1);
    intg k,l, lmax = (o1).dim(3), kmax = (o1).dim(2);
    intg i,j, imax = (o1).dim(0), jmax = (o1).dim(1);
    c1_0 = i1.idx_ptr();
    ker = i2.idx_ptr();
    d1_0 = o1.idx_ptr();
    for (i=0; i<imax; i++) {
      d1_1 = d1_0;
      c1_1 = c1_0;
      for (j=0; j<jmax; j++) {
	d1_2 = d1_1;
	c2_0 = ker;
	for (k=0; k<kmax; k++) {
	  d1_3 = d1_2;
	  c2_1 = c2_0;
	  for (l=0; l<lmax; l++) {
	    *d1_3 += (*c1_1)*(*c2_1)*(*c2_1);
	    d1_3 += d1_m3;
	    c2_1 += c2_m1;
	  }
	  d1_2 += d1_m2;
	  c2_0 += c2_m0;
	}
	d1_1 += d1_m1;
	c1_1 += c1_m1;
      }
      d1_0 += d1_m0;
      c1_0 += c1_m0;
    }
  }

  // Fu Jie Huang, May 20, 2008
  template<typename T> void idx_m2squdotm2(idx<T>& i1, idx<T>& i2, idx<T>& o) {
    idx_checkorder3(i1, 2, i2, 2, o, 0);
    idx_checkdim2(i1, 0, i2.dim(0), i1, 1, i2.dim(1));
    intg imax = i1.dim(0), jmax = i2.dim(1);
    intg c1_m0 = i1.mod(0), c2_m0 = i2.mod(0);
    intg c1_m1 = i1.mod(1), c2_m1 = i2.mod(1);

    T *c1_0 = i1.idx_ptr();
    T *c2_0 = i2.idx_ptr();
    T *d1 = o.idx_ptr();
    T *c1, *c2;

    T f = 0;
    for(int i=0; i<imax; ++i) {
      c1 = c1_0;
      c2 = c2_0;
      for(int j=0; j<jmax; ++j) {
	f += (*c1) * (*c1) * (*c2);   // only difference
	c1 += c1_m1;
	c2 += c2_m1;
      }
      c1_0 += c1_m0;
      c2_0 += c2_m0;
    }
    *d1 = f;
  }

  // Fu Jie Huang, May 20, 2008
  template<typename T> void idx_m2squdotm2acc(idx<T>& i1, idx<T>& i2, idx<T>& o) {
    idx_checkorder3(i1, 2, i2, 2, o, 0);
    idx_checkdim2(i1, 0, i2.dim(0), i1, 1, i2.dim(1));
    intg imax = i1.dim(0), jmax = i2.dim(1);
    intg c1_m0 = i1.mod(0), c2_m0 = i2.mod(0);
    intg c1_m1 = i1.mod(1), c2_m1 = i2.mod(1);

    T *c1_0 = i1.idx_ptr();
    T *c2_0 = i2.idx_ptr();
    T *d1 = o.idx_ptr();
    T *c1, *c2;

    T f = *d1;          //   only difference: accumulate
    for(int i=0; i<imax; ++i) {
      c1 = c1_0;
      c2 = c2_0;
      for(int j=0; j<jmax; ++j) {
	f += (*c1) * (*c1) * (*c2);
	c1 += c1_m1;
	c2 += c2_m1;
      }
      c1_0 += c1_m0;
      c2_0 += c2_m0;
    }
    *d1 = f;
  }

  // Fu Jie Huang, May 20, 2008
  template<typename T>
  void idx_m2oversample(idx<T>& small, intg nlin, intg ncol, idx<T>& big)
  {
    idx<T> uin  = big.unfold(0, nlin, nlin);
    idx<T> uuin = uin.unfold(1, ncol, ncol);
    idx_eloop1(z1, uuin, T) {
      idx_eloop1( z2, z1, T) {
	idx_copy(small, z2);
      }
    }
  }

  template <class T> T idx_max(idx<T> &m) {
    T v = *(m.idx_ptr());
    { idx_aloop1(i, m, T) {
	if (*i > v) v = *i;
      }}
    return v;
  }

  template <class T> T idx_min(idx<T> &m) {
    T v = *(m.idx_ptr());
    { idx_aloop1(i, m, T) {
	if (*i < v) v = *i;
      }}
    return v;
  }

  template<class T> intg idx_indexmax(idx<T> &m) {
    intg i = 0, imax = 0;
    T v = *(m.idx_ptr());
    { idx_aloop1(me, m, T) {
	if (*me > v) {
	  v = *me;
	  imax = i;
	}
	i++;
      }}
    return imax;
  }

  template<class T> intg idx_indexmin(idx<T> &m) {
    intg i = 0, imin = 0;
    T v = *(m.idx_ptr());
    { idx_aloop1(me, m, T) {
	if (*me < v) {
	  v = *me;
	  imin = i;
	}
	i++;
      }}
    return imin;
  }

  template<class T1, class T2> void idx_sortdown(idx<T1> &m, idx<T2> &p) {
    idx_checkorder2(m, 1, p, 1);
    if (m.mod(0) != 1) ylerror("idx_sortdown: vector is not contiguous");
    if (p.mod(0) != 1) ylerror("idx_sortdown: vector is not contiguous");
    intg n = m.dim(0);
    intg z = p.dim(0);
    if (n != z) ylerror("idx_sortdown: vectors have different sizes");
    if (n > 1) {
      int l,j,ir,i;
      T1 *ra, rra;
      T2 *rb, rrb;

      ra = (T1*)m.idx_ptr() -1;
      rb = (T2*)p.idx_ptr() -1;

      l = (n >> 1) + 1;
      ir = n;
      for (;;) {
	if (l > 1) {
	  rra=ra[--l];
	  rrb=rb[l];
	} else {
	  rra=ra[ir];
	  rrb=rb[ir];
	  ra[ir]=ra[1];
	  rb[ir]=rb[1];
	  if (--ir == 1) {
	    ra[1]=rra;
	    rb[1]=rrb;
	    return ; } }
	i=l;
	j=l << 1;
	while (j <= ir)	{
	  if (j < ir && ra[j] > ra[j+1]) ++j;
	  if (rra > ra[j]) {
	    ra[i]=ra[j];
	    rb[i]=rb[j];
	    j += (i=j);
	  } else j=ir+1; }
	ra[i]=rra;
	rb[i]=rrb;
      }
    }
  }

  template<class T> T idx_sqrdist(idx<T> &i1, idx<T> &i2) {
    idx_checknelems2_all(i1, i2);
    T z = 0;
    T tmp;
    { idx_aloop2(pi1, i1, T, pi2, i2, T) {
  	tmp = *pi1 - *pi2;
  	z += tmp * tmp;
      }
    }
    return z;
  }

  template<class T> void idx_sqrdist(idx<T> &i1, idx<T> &i2, idx<T> &out) {
    idx_checknelems2_all(i1, i2);
    if (out.order() != 0) ylerror("idx_sqrdist: expecting an idx of order 0");
    out.set(idx_sqrdist(i1, i2));
  }

  template <class T> void idx_exp(idx<T> &m) {
    idx_aloop1(i, m, T) {
      *i = exp(*i);
    };
  }

  /* TODO: implement generic version of idx_dot
     template <class T> void idx_dot(idx<T> &i1, idx<T> &i2, idx<T> &o1){
     if (o1.order() != 0) ylerror("Not an idx0");
     return o1.set(idx_dot(i1, i2));
     }
  */

  template<class T> void idx_dotacc(idx<T> &i1, idx<T> &i2, idx<T> &o) {
    if (o.order() != 0) ylerror("Not an idx0");
    o.set(o.get() + idx_dot(i1, i2));
  }

  template<class T> void idx_clip(idx<T> &i1, T m, idx<T> &o1) {
    idx_checknelems2_all(i1, o1);
    { idx_aloop2(i, i1, T, o, o1, T) {
	*o = std::max(m, *i);
      }}
  }

  template<class T> void idx_2dconvol(idx<T> &in, idx<T> &kernel, idx<T> &out) {
    idx_checkorder3(in, 2, kernel, 2, out, 2);
    idx<T> uin(in.unfold(0, kernel.dim(0), 1));
    uin = uin.unfold(1, kernel.dim(1), 1);
    idx_m4dotm2(uin, kernel, out);
  }

} // end namespace ebl
