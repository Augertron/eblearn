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

namespace ebl {

#ifdef USE_IPP

#ifndef Ipp_H
#define Ipp_H

#include "libidx.h"
#include <ipp.h>

  ////////////////////////////////////////////////////////////////
  // idx to IPP wrapper functions

  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  //! <ker> is actually in reverse order, so you might
  //! want to reverse it first.
  int ipp_convolution_float(idx<float> &in, idx<float> &ker, idx<float> &out);

  //! this calls ippiAdd_32f_C1IR
  //! This does component-wise addition of 2 matrices of floats. The result is 
  //! saved in the second idx
  int ipp_add_float(idx<float> &in1, idx<float> &in2);

  //! this calls ippiAddC_32f_C1R
  //! This adds a constant to each component of an idx of floats. NOT In place
  int ipp_addc_nip_float(idx<float> &in, float constant, idx<float> &out);

#endif

#endif

} // end namespace ebl
