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

#include <QThread>
#include <QWaitCondition>
#include <QtGui>

#include "libidx.h"

#define MUTEX_WAIT_MSEC 1000 // milliseconds wait until giving up on gui draw

#define WINDOW_FUNCTION_DECLARATION(fn)					\
  if (!window) cerr << "Warning: GUI window object unavailable." << endl; \
  if (window) window->fn

#define gui_draw_matrix WINDOW_FUNCTION_DECLARATION(g_draw_matrix)
#define gui_clear WINDOW_FUNCTION_DECLARATION(g_clear)
#define gui_quit WINDOW_FUNCTION_DECLARATION(g_quit)
#define gui_new_window WINDOW_FUNCTION_DECLARATION(g_new_window)
#define gui_select_window WINDOW_FUNCTION_DECLARATION(g_select_window)

namespace ebl {

  class RenderThread : public QThread {
    Q_OBJECT
      
  private:
    int		  argc;
    char	**argv;

  public:
    RenderThread(int argc_, char **argv_);
    virtual ~RenderThread();

    void g_quit();
    void g_clear();
    void g_new_window(const char *wname = NULL, unsigned int *wid = NULL);
    void g_select_window(unsigned int wid);
    void operator<<(const std::string *s);

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
    //! @param zoomw and @param zoomh are the zoom factors in width and height
    template<class T>
      void g_draw_matrix(Idx<T> &im, int h0 = 0, int w0 = 0, 
		       T minv = 0, T maxv = 0, 
		       double zoomw = 1.0, double zoomh = 1.0);

  signals:
    void drawImage(Idx<ubyte> *img, int h0, int w0);
    void appquit();
    void clear();
    void new_window(const char *wname, unsigned int *wid);
    void select_window(unsigned int wid);
    void addText(const std::string *s);
    
  protected:
    virtual void run();
  };

} // namespace ebl {

#include "RenderThread.hpp"

#endif /* RENDERTHREAD_H_ */
