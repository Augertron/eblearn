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

#ifndef IDXGUI_H_
#define IDXGUI_H_

#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtGui/QtGui>

#include <ostream>
#include "libidx.h"

using namespace std;

namespace ebl {

  class idxgui : public QThread, public ostringstream {
    Q_OBJECT
      
  private:
    int		  argc;
    char	**argv;
    unsigned int  nwid;
  public:
    bool         cout_output;

  public:
    idxgui();
    void init(int argc_, char **argv_);
    virtual ~idxgui();

    void quit();
    void clear();
    unsigned int new_window(const char *wname = NULL, unsigned int h = 0,
			    unsigned int w = 0);
    void select_window(unsigned int wid);

    template<class T> friend
      idxgui& operator<<(idxgui& r, const T val);

    void add_arrow(int h1, int w1, int h2, int w2);
    void set_text_colors(unsigned char fg_r, unsigned char fg_g, 
			 unsigned char fg_b, unsigned char fg_a,
			 unsigned char bg_r, unsigned char bg_g, 
			 unsigned char bg_b, unsigned char bg_a);
    void set_text_colors(int fg_r, int fg_g, 
			 int fg_b, int fg_a,
			 int bg_r, int bg_g, 
			 int bg_b, int bg_a);
    void set_cout_and_gui();
    void set_gui_only();
    void set_silent();
    void set_silent(const std::string *filename);
    void set_silent(const char *filename);
    void enable_updates();
    void disable_updates();

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
      void draw_matrix(idx<T> &im, unsigned int h0 = 0, unsigned int w0 = 0, 
		       T minv = 0, T maxv = 0, 
		       double zoomw = 1.0, double zoomh = 1.0);
    template<class T>
      void draw_matrix_frame(idx<T> &im, ubyte r, ubyte g, ubyte b,
			     unsigned int h0 = 0, unsigned int w0 = 0, 
			     T minv = 0, T maxv = 0, 
			     double zoomw = 1.0, double zoomh = 1.0);

    void add_text(std::string *s);
    void set_text_origin(unsigned int h0, unsigned int w0);
  private:

  signals:
    void gui_drawImage(idx<ubyte> *img, unsigned int h0, unsigned int w0);
    void appquit();
    void gui_clear();
    void gui_new_window(const char *wname, unsigned int h, unsigned int w);
    void gui_select_window(unsigned int wid);
    void gui_add_text(const std::string *s);
    void gui_add_arrow(int h1, int w1, int h2, int w2);
    void gui_set_text_origin(unsigned int h0, unsigned int w0);
    void gui_set_text_colors(unsigned char fg_r, unsigned char fg_g, 
			     unsigned char fg_b, unsigned char fg_a,
			     unsigned char bg_r, unsigned char bg_g, 
			     unsigned char bg_b, unsigned char bg_a);
    void gui_set_silent(const std::string *filename);
    void gui_set_wupdate(bool update);
    
  protected:
    virtual void run();
    virtual int run_main();
  };

} // namespace ebl {

#include "idxgui.hpp"

#endif /* GUI_H_ */
