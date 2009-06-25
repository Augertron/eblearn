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

#ifndef GUI_THREAD_H_
#define GUI_THREAD_H_

#include <QtGui/QPixmap>
#include <QtGui/QWidget>
#include <QtGui/QtGui>
#include <QtGui/QResizeEvent>
#include <math.h>
#include <vector>

#include "libidx.h"
#include "window.h"
#include "idxgui.h"
#include "scroll_box0.h"

namespace ebl {

  //! Global pointer to gui, allows to call for example 
  //! gui.draw_matrix() from anywhere in the code.
  extern ebl::idxgui gui;

  //! Window is a simple "whiteboard" on which you can display
  //! idxs with for example gray_draw_matrix and RGB_draw_matrix.
  //! Warning: do not use electric fence with QT as it is unstable.
  class gui_thread : public QWidget { 
    Q_OBJECT
  private:
    int				 wcur;
    unsigned int		 nwindows;
    std::vector<Window*>	 windows;
    bool			 silent;
    std::string			 savefname;
  public:
    idxgui		        &thread;

  public:
    gui_thread(int argc, char **argv);
    virtual ~gui_thread();

  private slots:
    void window_destroyed(QObject *obj);
    void set_wupdate(bool update);
    void add_text(const std::string *s);
    void add_arrow(int x1, int y1, int x2, int y2);
    void add_box(int h0, int w0, int h, int w, unsigned char r, unsigned char g,
		 unsigned char b, string *s);
    void set_text_origin(unsigned int h0, unsigned int w0);
    void set_text_colors(unsigned char fg_r, unsigned char fg_g, 
			 unsigned char fg_b, unsigned char fg_a,
			 unsigned char bg_r, unsigned char bg_g, 
			 unsigned char bg_b, unsigned char bg_a);
    void updatePixmap(idx<ubyte> *img, unsigned int h0, unsigned int w0);
    void appquit();
    void clear();
    void new_window(const char *wname = NULL, unsigned int h = 0, 
		    unsigned int w = 0);
    void select_window(int wid);
    void set_silent(const std::string *filename = NULL);
    void add_scroll_box(scroll_box0 *sb);
  };

  //! This macro is intended to replace your int main(int argc, char **argv)
  //! declaration and hides the declaration of the application and thread.
  //! What happens is QT takes over the main thread and runs your code
  //! in a thread.
#define MAIN_QTHREAD(targc, argc, targv, argv)	\
  int run_main(targc argc, targv argv);		\
  using namespace ebl;				\
  int main(targc argc, targv argv) {		\
    gui.thread_init = true;			\
    gui.run_main = &run_main;			\
    gui.thread_init = true;			\
    QApplication a(argc, argv);			\
    a.setQuitOnLastWindowClosed(false);		\
    ebl::gui_thread gt(argc, argv);		\
    gt.thread.start();				\
    a.exec();					\
    return 0;					\
  }						\
  int run_main(targc argc, targv argv)
  
} // namespace ebl {

#endif /* GUI_THREAD_H_ */
