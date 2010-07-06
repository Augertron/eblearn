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


#ifndef IPPOPS_H_
#define IPPOPS_H_

#ifdef __IPP__
#include <ipp.h>
#endif

#include "idx.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // idx to IPP wrapper functions

  //! Initialize ipp to ncores, or without core limit by default,
  //! then print how many cores are enabled. If not compiled with IPP,
  //! this will print a message.
  EXPORT void ipp_init(int ncores = -1);
  
  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  //! <ker> is actually in reverse order, so you might
  //! want to reverse it first.
  template <typename T>
    int ipp_convolution(idx<T> &in, idx<T> &ker, idx<T> &out);

  //! compute a 2D convolution of <in> with kernel <ker>
  //! and write result into <out>
  //! <ker> is actually in reverse order, so you might
  //! want to reverse it first.
  template <>
    int ipp_convolution(idx<float> &in, idx<float> &ker, idx<float> &out);

  //! this calls ippiAdd_32f_C1IR
  //! This does component-wise addition of 2 matrices of floats. The result is 
  //! saved in the second idx
  template <typename T>
  int ipp_add(idx<T> &in1, idx<T> &in2);

  //! this calls ippiAdd_32f_C1IR
  //! This does component-wise addition of 2 matrices of floats. The result is 
  //! saved in the second idx
  template <>
  int ipp_add(idx<float> &in1, idx<float> &in2);

  //! this calls ippiAddC_32f_C1R
  //! This adds a constant to each component of an idx of floats. NOT In place
  template <typename T>
  int ipp_addc(idx<T> &in, T c, idx<T> &out);

  //! this calls ippiAddC_32f_C1R
  //! This adds a constant to each component of an idx of floats. NOT In place
  template <>
  int ipp_addc(idx<float> &in, float c, idx<float> &out);

  //! Copy in to out.
  template <typename T>
  int ipp_copy(idx<T> &in, idx<T> &out);

  //! Copy in to out.
  template <>
  int ipp_copy(idx<float> &in, idx<float> &out);

  //! Set all idx elements to v.
  template <typename T>
    int ipp_set(idx<T> &in, T v);

  //! Set all idx elements to v.
  template <>
    int ipp_set(idx<float> &in, float v);

} // end namespace ebl

#include "ippops.hpp"

#endif /* IPPOPS_H_ */

