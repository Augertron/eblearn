/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include <algorithm>
#include <stdio.h>
#include <inttypes.h>

#include "color_spaces.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // YUV

  const double rgb_yuv_mat[3][3] = {{  0.299,  0.587,    0.114 }, 
				    { -0.147, -0.289,    0.437 },
				    {  0.615, -0.515,   -0.100 }};
  idx<double> rgb_yuv((const double*)rgb_yuv_mat, 3, 3);

  const double yuv_rgb_mat[3][3] = {{  1,      0,        1.13983 }, 
				    {  1,     -0.39465, -0.58060 },
				    {  1,      2.03211,  0 }};
  idx<double> yuv_rgb((const double*)yuv_rgb_mat, 3, 3);

  void YUVGlobalNormalization(idx<float> &yuv) {
    idx_checkorder1(yuv, 3);
    idx<float> tmp = yuv.select(2, 0);
    idx_addc(tmp, (float) -131.35, tmp);
    idx_dotc(tmp, (float)    0.01, tmp);
    tmp = yuv.select(2, 1);
    idx_addc(tmp, (float) -116.16, tmp);
    idx_dotc(tmp, (float)    0.01, tmp);
    tmp = yuv.select(2, 2);
    idx_addc(tmp, (float) -109.11, tmp);
    idx_dotc(tmp, (float)    0.01, tmp);
  }

} // end namespace ebl
