/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef RENDERTHREAD_HPP_
#define RENDERTHREAD_HPP_

using namespace std;

namespace ebl {
  
  template<class T>
  void RenderThread::grey_draw_matrix(Idx<T> &im, int x, int y, T minv, T maxv, 
				      double zoomw, double zoomh) {
    if (mutex->tryLock(MUTEX_WAIT_MSEC)) {
      Idx<ubyte> uim = grey_image_to_ubyte<T>(im, minv, maxv, zoomw, zoomh);
      copy(uim, x, y);
      // send image to main gui thread
      emit renderedImage();
      mutex->unlock();
    } else lock_failed_warning();
  }
  
  void grey_draw_matrix(Idx<double> &im, int x, int y, double minv, double maxv,
			double zoomw, double zoomh);
  void grey_draw_matrix(Idx<int> &im, int x, int y, int minv, int maxv,
			double zoomw, double zoomh);
  void grey_draw_matrix(Idx<ubyte> &im, int x, int y, ubyte minv, ubyte maxv,
			double zoomw, double zoomh);

} // end namespace ebl

#endif /* RENDERTHREAD_HPP_ */
