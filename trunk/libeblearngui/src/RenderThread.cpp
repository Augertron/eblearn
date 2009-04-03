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

#include "RenderThread.moc"

#include "libidx.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // RenderThread

  RenderThread::RenderThread(int argc_, char **argv_) 
    : colorTable(256) {
    mutex = new QMutex(QMutex::Recursive);
    if (mutex->tryLock(MUTEX_WAIT_MSEC)) {
      argc = argc_;
      argv = argv_;
      buffer = NULL;
      qimage = NULL;
      for (int i = 0; i < 256; i++){
	colorTable[i] = qRgb(i, i, i);
      }
      resize(600, 800);
      mutex->unlock();
    } else lock_failed_warning();
  }

  RenderThread::~RenderThread() {
    if (mutex->tryLock(MUTEX_WAIT_MSEC)) {
      if (buffer)
	delete buffer;
      if (qimage)
	delete qimage;
      mutex->unlock();
    } else lock_failed_warning();
    delete mutex;
    wait();
  }

  void RenderThread::quit() {
    emit appquit();
  }

  void RenderThread::resize(int h, int w) {
    if (mutex->tryLock(MUTEX_WAIT_MSEC)) {
      if (!buffer) {
	buffer = new Idx<ubyte>(h, w);
	idx_fill(*buffer, (ubyte) 255);
      }
      else {
	Idx<ubyte> *inew = new Idx<ubyte>(h, w);
	idx_fill(*inew, (ubyte) 255);
	Idx<ubyte> tmpnew = inew->narrow(0, MIN(h, buffer->dim(0)), 0);
	tmpnew = tmpnew.narrow(1, MIN(w, buffer->dim(1)), 0);
	Idx<ubyte> tmpbuf = buffer->narrow(0, MIN(h, buffer->dim(0)), 0);
	tmpbuf = tmpbuf.narrow(1, MIN(w, buffer->dim(1)), 0);
	idx_copy(tmpbuf, tmpnew);
	delete buffer;
	buffer = inew;
      }
      if (qimage)
	delete qimage;
      qimage = new QImage((unsigned char*) buffer->idx_ptr(), 
			  buffer->dim(1), buffer->dim(0), 
			  buffer->dim(1) * sizeof (unsigned char),
			  QImage::Format_Indexed8);
      qimage->setColorTable(colorTable);
      mutex->unlock();
    } else lock_failed_warning();
  }

  void RenderThread::copy(Idx<ubyte> &im, int x, int y) {
    if (mutex->tryLock(MUTEX_WAIT_MSEC)) {
      int h = MAX(buffer?buffer->dim(0):0, x + im.dim(0));
      int w = MAX(buffer?buffer->dim(1):0, y + im.dim(1));
      if (!buffer)
	resize(h, w);
      else if ((h > buffer->dim(0)) || (w > buffer->dim(1)))
	resize(h, w);
      Idx<ubyte> tmpbuf = buffer->narrow(0, im.dim(0), x);
      tmpbuf = tmpbuf.narrow(1, im.dim(1), y);
      idx_copy(im, tmpbuf);
      mutex->unlock();
    } else lock_failed_warning();
  }

  void RenderThread::clear() {
    if (mutex->tryLock(MUTEX_WAIT_MSEC)) {
      if (buffer) {
	idx_fill(*buffer, (ubyte) 255);
	emit renderedImage();
      }
      mutex->unlock();
    } else lock_failed_warning();
  }

  void RenderThread::run() {
    ylerror("The run2 method of RenderThread must be overridden and contain \
your code");
  }

} // end namespace ebl
