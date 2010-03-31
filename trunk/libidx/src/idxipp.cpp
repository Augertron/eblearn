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

#ifdef __IPP__

#include "idxipp.h"

using namespace std;

namespace ebl {

  // TODO: handle non contiguous?
  template <>
  void ipp_convolution(idx<float> &in, idx<float> &ker, idx<float> &out) {
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
    ippiConvValid_32f_C1R(in.idx_ptr(),instep,insize,
			  ker.idx_ptr(),kerstep,kersize,
			  out.idx_ptr(),outstep);
  }

  int ipp_add_float(idx<float> &in1, idx<float> &in2) {
    IppiSize	insize;
    int		instep;

    if ((in1.dim(0) > INT_MAX) || (in1.dim(1) > INT_MAX) ||
	(in1.mod(0) > INT_MAX))
      eblerror("TODO: Cannot use long with IPP.");
  
    insize.height = in1.dim(0);
    insize.width = in1.dim(1); 
    instep = sizeof (float) * in1.mod(0);
    return ippiAdd_32f_C1IR(in1.ptr(), instep, in2.ptr(), instep, insize);
  }

  int ipp_addc_nip_float(idx<float> &in, float constant, idx<float> &out) {
    IppiSize	insize;
    int		instep, outstep;

    if ((in.dim(0) > INT_MAX) || (in.dim(1) > INT_MAX) ||
	(in.mod(0) > INT_MAX) || (out.mod(0) > INT_MAX))
      eblerror("TODO: Cannot use long with IPP.");

    insize.height = in.dim(0);
    insize.width = in.dim(1); 
    instep = sizeof (float) * in.mod(0);
    outstep = sizeof (float) * out.mod(0);
    return ippiAddC_32f_C1R(in.ptr(), instep, constant, out.ptr(), outstep, 
			    insize);
  }

} // end namespace ebl

#endif

