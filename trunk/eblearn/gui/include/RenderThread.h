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

#ifndef RENDERTHREAD_H_
#define RENDERTHREAD_H_

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include <QtGui>

  class QImage;

namespace ebl {

  /* *
   * these are the different types of idxs supported.
   * it has been created to "templatize" the display of the idxs,
   * because Qt does not allow templates of Q_OBJECTs
   * */
  enum idx_type { DOUBLE, FLOAT, INTG, UBYTE };

  class RenderThread : public QThread {
    Q_OBJECT

  private:
    int argc;
    char **argv;

  public:
    RenderThread(int argc_, char **argv_) : argc(argc_), argv(argv_) {};
    
  //! gray_draw_matrix displays your idx2 or the first layer of your idx3 in
  //! grayscale on the whiteboard. This function does a copy of your idx and
  //! won't change in in any way !
  //! @param idx and @param type are, like before, used to templatize the
  //! function
  //! @param x and @param y are the coordinates of the top-left corner of
  //! your picture on the whiteboard
  //! @param minv and @param maxv are the min and max values to set colors.
  //! If left to zero, the min of your idx will be set to 0 and the max will
  //! be 255
  //! @param zoomx and @param zoomy are the zoom factors in width and height
  //! @param mutex is used if you want to protect your idx (multi-thread)
  void gray_draw_matrix(void* idx, idx_type type, int x = 0, int y = 0,
			int minv = 0, int maxv = 0, int zoomx = 1,
			int zoomy = 1, QMutex* mutex = NULL);

  //! rgb_draw_matrix displays the 3 firsts layers of your idx3 as a
  //! RGB picture on the whiteboard.
  //! Attention : it won't change the values in your idx, so if you want a
  //! good display, you have to make it an idx3 with values between 0 and 255
  //! This function does a copy of your idx and won't change in in any way !
  //! @param idx and @param type are, like before, used to templatize the
  //! function
  //! @param x and @param y are the coordinates of the top-left corner of
  //! your picture on the whiteboard
  //! @param zoomx and @param zoomy are the zoom factors in width and height
  //! @param mutex is used if you want to protect your idx (multi-thread)
  void rgb_draw_matrix(void* idx, idx_type type, int x = 0, int y = 0,
		       int zoomx = 1, int zoomy = 1, QMutex* mutex = NULL);

  signals:
    void renderedImage(const QImage &im, double scaleFactor);

  protected:
    virtual void run();
  };

} // namespace ebl {

#endif /* RENDERTHREAD_H_ */
