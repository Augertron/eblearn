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

#include "blas.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // size compatibility checking functions

  template<class T> void check_m2dotm1(idx<T> &m, idx<T> &x, idx<T> &y) {
    idx_checkorder3(m, 2, x, 1, y, 1);
    idx_checkdim2(y, 0, m.dim(0), x, 0, m.dim(1));
  }

  template<class T> void check_m1extm1(idx<T> &x, idx<T> &y, idx<T> &m) {
    idx_checkorder3(m, 2, x, 1, y, 1);
    idx_checkdim2(y, 0, m.dim(1), x, 0, m.dim(0));
  }

  ////////////////////////////////////////////////////////////////

  // specialization for doubles: can use blas versions.
  void idx_copy(idx<double> &src, idx<double> &dst) {
    idxop_ii(src, dst,
	     // idx0 version
	     { *(dst.idx_ptr()) = *(src.idx_ptr()); },
	     // idx1 version
	     { cblas_dcopy(N1, src.idx_ptr(), src.mod(0), dst.idx_ptr(), 
			   dst.mod(0)); },
	     // contiguous version
	     { cblas_dcopy(N1, src.idx_ptr(), 1, dst.idx_ptr(), 1); },
	     // recursive version
	     { idx_bloop2(lsrc, src, double, ldst, dst, double) { 
		 idx_copy(lsrc, ldst); } },
	     // any version
	     { idx_aloop2(isrc, src, double, idst, dst, double) { 
		 *idst = *isrc; }
	     }
	     );
  }

  // specialization for floats: can use blas versions.
  void idx_copy(idx<float> &src, idx<float> &dst) {
    idxop_ii(src, dst,
	     // idx0 version
	     { *(dst.idx_ptr()) = *(src.idx_ptr()); },
	     // idx1 version
	     { cblas_scopy(N1, src.idx_ptr(), src.mod(0), dst.idx_ptr(), 
			   dst.mod(0)); },
	     // contiguous version
	     { cblas_scopy(N1, src.idx_ptr(), 1, dst.idx_ptr(), 1); },
	     // recursive version
	     { idx_bloop2(lsrc, src, float, ldst, dst, float) { 
		 idx_copy(lsrc, ldst); } },
	     // any version
	     { idx_aloop2(isrc, src, float, idst, dst, float) { 
		 *idst = *isrc; }
	     }
	     );
  }

  ////////////////////////////////////////////////////////////////

  template<> double idx_sum(idx<double> &inp, double *out) {
    double z = 0;
    if (inp.order() == 0) {
      z = inp.get();
      /* TODO-0: bug: test cblas sum
      //  } else if (inp.order() == 1) {
      //    z = cblas_dasum(inp.dim(0), inp.idx_ptr(), inp.mod(0));
      //  } else if (inp.contiguousp()) {
      //    z = cblas_dasum(inp.nelements(), inp.idx_ptr(), 1);
      */
    } else {
      idx_aloop1(pinp,inp,double) {
    	z += *pinp; }
    }
    if (out != NULL) {
      *out += z;
      return *out;
    }
    return z;
  }

  template<> float idx_sum(idx<float> &inp, float *out) {
    float z = 0;
    if (inp.order() == 0) {
      z = inp.get();
      /* TODO-0: bug: test cblas sum
	 } else if (inp.order() == 1) {
	 z = cblas_sasum(inp.dim(0), inp.idx_ptr(), inp.mod(0));
	 } else if (inp.contiguousp()) {
	 z = cblas_sasum(inp.nelements(), inp.idx_ptr(), 1);
      */  } else {
      //      idxiter<float> pinp;
      //      idx_aloop1_on(pinp,inp) { z += *pinp; }
      idx_aloop1(pinp,inp,float) { z += *pinp; }
    }
    if (out != NULL) {
      *out += z;
      return *out;
    }
    return z;
  }

  ////////////////////////////////////////////////////////////////

  double idx_dot(idx<double> &i1, idx<double> &i2) {
    idxop_ii(i1, i2,
	     // idx0 version
	     { return *(i1.idx_ptr()) * *(i2.idx_ptr()); },
	     // idx1 version
	     { return cblas_ddot(N1, i1.idx_ptr(), i1.mod(0), i2.idx_ptr(), 
				 i2.mod(0)); },
	     // contiguous version
	     { return cblas_ddot(N1, i1.idx_ptr(), 1, i2.idx_ptr(), 1); },
	     // recursive version
	     { double z = 0; idx_bloop2(li1, i1, double, li2, i2, double) { 
		 z += idx_dot(li1, li2); } return z; },
	     // any version
	     { double z = 0;
	       //     idxiter<double> ii1; idxiter<double> ii2;
	       //     idx_aloop2_on(ii1, i1, ii2, i2) { z += (*ii1)*(*ii2); }
	       idx_aloop2(ii1, i1, double, ii2, i2, double) { 
		 z += (*ii1)*(*ii2); }
	       return z; }
	     );
  }

  float idx_dot(idx<float> &i1, idx<float> &i2) {
    idxop_ii(i1, i2,
	     // idx0 version
	     { return *(i1.idx_ptr()) * *(i2.idx_ptr()); },
	     // idx1 version
	     { return cblas_sdot(N1, i1.idx_ptr(), i1.mod(0), i2.idx_ptr(), 
				 i2.mod(0)); },
	     // contiguous version
	     { return cblas_sdot(N1, i1.idx_ptr(), 1, i2.idx_ptr(), 1); },
	     // recursive version
	     { float z = 0; idx_bloop2(li1, i1, float, li2, i2, float) { 
		 z += idx_dot(li1, li2); } return z; },
	     // any version
	     { float z = 0;
	       //     idxiter<float> ii1; idxiter<float> ii2;
	       //     idx_aloop2_on(ii1, i1, ii2, i2) { z += (*ii1)*(*ii2); }
	       idx_aloop2(ii1, i1, float, ii2, i2, float) { 
		 z += (*ii1)*(*ii2); }
	       return z; }
	     );
  }

  ////////////////////////////////////////////////////////////////

  // matrix-vector multiplication: y <- a.x
  void idx_m2dotm1(idx<double> &a, idx<double> &x, idx<double> &y) {
    check_m2dotm1(a,x,y);
    if (a.mod(0) > a.mod(1)) {
      cblas_dgemv(CblasRowMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(0), x.idx_ptr(), x.mod(0),
		  0.0, y.idx_ptr(), y.mod(0));
    } else {
      cblas_dgemv(CblasColMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(1), x.idx_ptr(), x.mod(0),
		  0.0, y.idx_ptr(), y.mod(0));
    }
  }

  // matrix-vector multiplication: y <- a.x
  void idx_m2dotm1(idx<float> &a, idx<float> &x, idx<float> &y) {
    check_m2dotm1(a,x,y);
    if (a.mod(0) > a.mod(1)) {
      cblas_sgemv(CblasRowMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(0), x.idx_ptr(), x.mod(0),
		  0.0, y.idx_ptr(), y.mod(0));
    } else {
      cblas_sgemv(CblasColMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(1), x.idx_ptr(), x.mod(0),
		  0.0, y.idx_ptr(), y.mod(0));
    }
  }

  // matrix-vector multiplication: y <- y + a.x
  void idx_m2dotm1acc(idx<double> &a, idx<double> &x, idx<double> &y) {
    check_m2dotm1(a,x,y);
    if (a.mod(0) > a.mod(1)) {
      cblas_dgemv(CblasRowMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(0), x.idx_ptr(), x.mod(0),
		  1.0, y.idx_ptr(), y.mod(0));
    } else {
      cblas_dgemv(CblasColMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(1), x.idx_ptr(), x.mod(0),
		  1.0, y.idx_ptr(), y.mod(0));
    }
  }


  // matrix-vector multiplication: y <- y + a.x
  void idx_m2dotm1acc(idx<float> &a, idx<float> &x, idx<float> &y) {
    check_m2dotm1(a,x,y);
    if (a.mod(0) > a.mod(1)) {
      cblas_sgemv(CblasRowMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(0), x.idx_ptr(), x.mod(0),
		  1.0, y.idx_ptr(), y.mod(0));
    } else {
      cblas_sgemv(CblasColMajor, CblasNoTrans, a.dim(0), a.dim(1),
		  1.0, a.idx_ptr(), a.mod(1), x.idx_ptr(), x.mod(0),
		  1.0, y.idx_ptr(), y.mod(0));
    }
  }

  ////////////////////////////////////////////////////////////////

  // square matrix-vector multiplication: Yi = sum((Aij)^2 * Xj)
  void idx_m2squdotm1(idx<double> &a, idx<double> &x, idx<double> &y) {
    check_m2dotm1(a,x,y);
    idx_bloop2(la,a,double, ly,y,double) {
      double *pa = la.idx_ptr(); intg amod = la.mod(0);
      double *px =  x.idx_ptr(); intg xmod = x.mod(0);
      double *py = ly.idx_ptr();
      // we don't use bloop for efficiency
      *py = 0;
      for(intg i=0; i<la.dim(0); i++) {
	*py += (*pa)*(*pa)*(*px);
	pa += amod; px += xmod;
      }
    }
  }

  // square matrix-vector multiplication: Yi = sum((Aij)^2 * Xj)
  void idx_m2squdotm1(idx<float> &a, idx<float> &x, idx<float> &y) {
    check_m2dotm1(a,x,y);
    idx_bloop2(la,a,float, ly,y,float) {
      float *pa = la.idx_ptr(); intg amod = la.mod(0);
      float *px =  x.idx_ptr(); intg xmod = x.mod(0);
      float *py = ly.idx_ptr();
      // we don't use bloop for efficiency
      *py = 0;
      for(intg i=0; i<la.dim(0); i++) {
	*py += (*pa)*(*pa)*(*px);
	pa += amod; px += xmod;
      }
    }
  }

  // square matrix-vector multiplication: Yi += sum((Aij)^2 * Xj)
  void idx_m2squdotm1acc(idx<double> &a, idx<double> &x, idx<double> &y) {
    check_m2dotm1(a,x,y);
    idx_bloop2(la,a,double, ly,y,double) {
      double *pa = la.idx_ptr(); intg amod = la.mod(0);
      double *px =  x.idx_ptr(); intg xmod = x.mod(0);
      double *py = ly.idx_ptr();
      // we don't use bloop for efficiency
      for(intg i=0; i<la.dim(0); i++) {
	*py += (*pa)*(*pa)*(*px);
	pa += amod; px += xmod;
      }
    }
  }

  // square matrix-vector multiplication: Yi += sum((Aij)^2 * Xj)
  void idx_m2squdotm1acc(idx<float> &a, idx<float> &x, idx<float> &y) {
    check_m2dotm1(a,x,y);
    idx_bloop2(la,a,float, ly,y,float) {
      float *pa = la.idx_ptr(); intg amod = la.mod(0);
      float *px =  x.idx_ptr(); intg xmod = x.mod(0);
      float *py = ly.idx_ptr();
      // we don't use bloop for efficiency
      for(intg i=0; i<la.dim(0); i++) {
	*py += (*pa)*(*pa)*(*px);
	pa += amod; px += xmod;
      }
    }
  }

  ////////////////////////////////////////////////////////////////

  // vector-vector outer product: a <- x.y'
  void idx_m1extm1(idx<double> &x, idx<double> &y, idx<double> &a) {
    check_m1extm1(x,y,a);
    idx_clear(a);
    cblas_dger(CblasRowMajor, a.dim(0), a.dim(1),
	       1.0, x.idx_ptr(), x.mod(0), y.idx_ptr(), y.mod(0),
	       a.idx_ptr(), a.mod(0));
  }

  // vector-vector outer product: a <- x.y'
  void idx_m1extm1(idx<float> &x, idx<float> &y, idx<float> &a) {
    check_m1extm1(x,y,a);
    idx_clear(a);
    cblas_sger(CblasRowMajor, a.dim(0), a.dim(1),
	       1.0, x.idx_ptr(), x.mod(0), y.idx_ptr(), y.mod(0),
	       a.idx_ptr(), a.mod(0));
  }

  // vector-vector outer product: a <- a + x.y'
  void idx_m1extm1acc(idx<double> &x, idx<double> &y, idx<double> &a) {
    check_m1extm1(x,y,a);
    cblas_dger(CblasRowMajor, a.dim(0), a.dim(1),
	       1.0, x.idx_ptr(), x.mod(0), y.idx_ptr(), y.mod(0),
	       a.idx_ptr(), a.mod(0));
  }

  // vector-vector outer product: a <- a + x.y'
  void idx_m1extm1acc(idx<float> &x, idx<float> &y, idx<float> &a) {
    check_m1extm1(x,y,a);
    cblas_sger(CblasRowMajor, a.dim(0), a.dim(1),
	       1.0, x.idx_ptr(), x.mod(0), y.idx_ptr(), y.mod(0),
	       a.idx_ptr(), a.mod(0));
  }

  ////////////////////////////////////////////////////////////////
  // squext operations

  // Aij = Xi * Yj^2
  void idx_m1squextm1(idx<double> &x, idx<double> &y, idx<double> &a) {
    check_m1extm1(x,y,a);
    idx_bloop2(lx,x,double, la,a,double) {
      // TODO: change to aloop
      idx_bloop2(ly,y,double, lla,la,double) {
	//        *lla = (*lx)*(*ly)*(*ly);
        lla.set(lx.get() * ly.get() * ly.get());
      }
    }
  }

  //Aij = Xi * Yj^2
  void idx_m1squextm1(idx<float> &x, idx<float> &y, idx<float> &a) {
    check_m1extm1(x,y,a);
    idx_bloop2(lx,x,float, la,a,float) {
      idx_bloop2(ly,y,float, lla,la,float) {
	//       *lla = (*lx)*(*ly)*(*ly);
        lla.set(lx.get() * ly.get() * ly.get());
      }
    }
  }

  // Aij += Xi * Yj^2
  void idx_m1squextm1acc(idx<double> &x, idx<double> &y, idx<double> &a) {
    check_m1extm1(x,y,a);
    idx_bloop2(lx,x,double, la,a,double) {
      idx_bloop2(ly,y,double, lla,la,double) {
	//      *lla += (*lx)*(*ly)*(*ly);
        lla.set(lla.get() + lx.get() * ly.get() * ly.get());
      }
    }
  }

  // Aij += Xi * Yj^2
  void idx_m1squextm1acc(idx<float> &x, idx<float> &y, idx<float> &a) {
    check_m1extm1(x,y,a);
    idx_bloop2(lx,x,float, la,a,float) {
      idx_bloop2(ly,y,float, lla,la,float) {
	//        *lla += (*lx)*(*ly)*(*ly);
        lla.set(lla.get() + lx.get() * ly.get() * ly.get());
	// TODO-0: BUG: this doesn't seem to work: 
	// *(lla.ptr()) += lx.get() * ly.get() * ly.get();
      }
    }
  }

  ////////////////////////////////////////////////////////////////

  void norm_columns(idx<double> &m) {
    if ( m.order() != 2) { eblerror("norm_columns: must be an idx2"); }
    idx_eloop1(lm,m,double) {
      double *p = lm.idx_ptr();
      double z = cblas_dnrm2(m.dim(0),p,m.mod(0));
      cblas_dscal(m.dim(0),1/z,p,m.mod(0));
    }
  }

  void norm_columns(idx<float> &m) {
    if ( m.order() != 2) { eblerror("norm_columns: must be an idx2"); }
    idx_eloop1(lm,m,float) {
      float *p = lm.idx_ptr();
      float z = cblas_snrm2(m.dim(0),p,m.mod(0));
      cblas_sscal(m.dim(0),1/z,p,m.mod(0));
    }
  }

  ////////////////////////////////////////////////////////////////

} // end namespace ebl
