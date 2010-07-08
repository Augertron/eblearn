/***************************************************************************
 *   Copyright (C) 2008 by Pierre Sermanet and Yann LeCun   *
 *   pierre.sermanet@gmail.com, yann@cs.nyu.edu   *
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

#include "ippops.h"
#include <limits.h>

using namespace std;

namespace ebl {

  void ipp_init(int ncores) {
#ifdef __IPP__
    if (ncores > 0)
      ippSetNumThreads(ncores);
    ippGetNumThreads(&ncores);
    cout << "Using Intel IPP with " << ncores << " core(s)." << endl;
#else
    cout << "Not using Intel IPP." << endl;
#endif
  }
  
#ifdef __IPP__
  
  // TODO: handle non contiguous?
  template <>
  int ipp_convolution(idx<float> &in, idx<float> &ker, idx<float> &out) {
    if ((in.dim(0) > INT_MAX) || (in.dim(1) > INT_MAX) ||
	(ker.dim(0) > INT_MAX) || (ker.dim(1) > INT_MAX) ||
	(in.mod(0) > INT_MAX) || (ker.mod(0) > INT_MAX) ||
	(out.mod(0) > INT_MAX))
      eblerror("TODO: Cannot use long with IPP.");
  
    int instep	= sizeof (float) * (int) in.mod(0);
    int kerstep	= sizeof (float) * (int) ker.mod(0);
    int outstep	= sizeof (float) * (int) out.mod(0);
    IppiSize insize, kersize;
  
    insize.height = in.dim(0);
    insize.width = in.dim(1);
    kersize.height = ker.dim(0);
    kersize.width = ker.dim(1);
    return ippiConvValid_32f_C1R(in.idx_ptr(), instep, insize,
				 ker.idx_ptr(), kerstep, kersize,
				 out.idx_ptr(), outstep);
  }

  template <>
  int ipp_add(idx<float> &in1, idx<float> &in2) {
    idx_checknelems2_all(in1, in2);
    idx_check_contiguous2(in1, in2);
    IppiSize	insize;
    int		instep;
    if (in1.nelements() > INT_MAX)
      eblerror("TODO: Cannot use long with IPP.");
    insize.height = in1.nelements();
    insize.width = 1; 
    instep = sizeof (float);
    int ret = ippiAdd_32f_C1IR(in1.idx_ptr(), instep, in2.idx_ptr(),
			       instep, insize);
    if (ret != ippStsNoErr) {
      cerr << "IPP error: " << endl;
      eblerror("IPP error");
    }
    return ret;
  }

  template <>
  int ipp_addc(idx<float> &in, float c, idx<float> &out) {
    idx_checknelems2_all(in, out);
    idx_check_contiguous2(in, out);
    IppiSize	insize;
    int		instep, outstep;
    if (in.nelements() > INT_MAX)
      eblerror("TODO: Cannot use long with IPP.");
    insize.height = in.nelements();
    insize.width = 1; 
    return ippiAddC_32f_C1R(in.idx_ptr(), sizeof (float), c, out.idx_ptr(), 
			    sizeof (float), insize);
  }

  template <>
  int ipp_copy(idx<float> &in, idx<float> &out) {
    idx_checknelems2_all(in, out);
    idx_check_contiguous2(in, out);
    IppiSize	insize;
    int		instep, outstep;
    if (in.nelements() > INT_MAX)
      eblerror("TODO: Cannot use long with IPP.");
    insize.height = in.nelements();
    insize.width = 1; 
    return ippiCopy_32f_C1R(in.idx_ptr(), sizeof (float), out.idx_ptr(), 
			    sizeof (float), insize);
  }

  template <>
  int ipp_set(idx<float> &in, float v) {
    idx_check_contiguous1(in);
    IppiSize insize;
    insize.height = in.nelements();
    insize.width = 1;
    return ippiSet_32f_C1R(v, in.idx_ptr(), sizeof (float), insize);
  }

#endif /* __IPP__ */
  
} // end namespace ebl

