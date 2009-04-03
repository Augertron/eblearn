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

#ifndef WINDOW_H_
#define WINDOW_H_

#include <QPixmap>
#include <QWidget>
#include <QtGui>
#include <QResizeEvent>
#include <math.h>

#include "RenderThread.h"

namespace ebl {

//! Window is a simple "whiteboard" on which you can display
//! Idxs with for example gray_draw_matrix and RGB_draw_matrix.
//! Warning: do not use electric fence with QT as it is unstable.
class Window : public QWidget { 
  Q_OBJECT
  private:
    QPixmap	       *pixmap;
    QPoint		pixmapOffset;
    QPoint		lastDragPos;
    double		pixmapScale;
    double		curScale;
    float		scaleIncr;
  public:
    RenderThread	thread;

  protected:
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    
  private slots:
    void updatePixmap();
    void appquit();

  public:
    //! Be careful to create a whiteboard big enough for your pictures, since 
    //! you won't be able to make it bigger after (ie resizing the window will 
    //! scale the content, you won't have more space to draw on)
    Window(int argc, char **argv, int height = 600, int width = 800);
    virtual ~Window();
  };

  //! Global pointer to window, allows to call for example 
  //! window->gray_draw_matrix from anywhere in the code.
  extern ebl::RenderThread *window;

  //! This macro is intended to replace your int main(int argc, char **argv)
  //! declaration and hides the declaration of the application and thread.
  //! What happens is QT takes over the main thread and runs your code
  //! in a thread.
#define MAIN_QTHREAD()				\
  using namespace ebl;				\
  int main(int argc, char **argv) {		\
    QApplication a(argc, argv);			\
    ebl::Window w(argc, argv);			\
    window = &(w.thread);			\
    w.thread.start();				\
    a.exec();					\
    return 0;					\
  }						\
  void RenderThread::run()

} // namespace ebl {

#endif /* WINDOW_H_ */
