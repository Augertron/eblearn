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

#ifndef GUITHREAD_H_
#define GUITHREAD_H_

#include <QPixmap>
#include <QWidget>
#include <QtGui>
#include <QResizeEvent>
#include <math.h>
#include <vector>

#include "libidx.h"
#include "Window.h"
#include "RenderThread.h"

namespace ebl {

//! Window is a simple "whiteboard" on which you can display
//! Idxs with for example gray_draw_matrix and RGB_draw_matrix.
//! Warning: do not use electric fence with QT as it is unstable.
class GuiThread : public QWidget { 
  Q_OBJECT
  private:
    unsigned int		wcur;
    std::vector<Window*>	windows;
  public:
    RenderThread		thread;

  private slots:
    void window_destroyed(QObject *obj);
    void addText(const std::string *s);
    void updatePixmap(Idx<ubyte> *img, int h0, int w0);
    void appquit();
    void clear();
    void new_window(const char *wname = NULL, unsigned int *wid = NULL);
    void select_window(unsigned int wid);

  public:
    GuiThread(int argc, char **argv);
    virtual ~GuiThread();
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
    ebl::GuiThread gt(argc, argv);		\
    window = &(gt.thread);			\
    gt.thread.start();				\
    a.exec();					\
    return 0;					\
  }						\
  void RenderThread::run()

} // namespace ebl {

#endif /* GUITHREAD_H_ */
