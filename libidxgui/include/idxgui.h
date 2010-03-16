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
#include "scroll_box0.h"
#include "gui_thread.h"

using namespace std;

namespace ebl {

  class gui_thread;
  template<class T1, class T2> class ManipInfra;

  class idxgui : public QThread, public ostringstream {
    Q_OBJECT
      
  private:
    int			  argc;
    char		**argv;
    int	                  nwid;
    const unsigned int   *nwindows; // owned by gui_thread
    gui_thread *gt;
  public:
    bool		  thread_init;
    bool		  cout_output;
    int                 (*run_main)(int, char**);
    bool		  main_done;
    

  public:
    idxgui();
    void init(int argc_, char **argv_, const unsigned int *nwindows,
	      gui_thread *gt_);
    virtual ~idxgui();

    //! creates a new window.
    int new_window(const char *wname = NULL, unsigned int h = 0,
		   unsigned int w = 0);

    //! selects window wid.
    void select_window(int wid);

    //! operator<< for text drawing on the gui.
    template<class T> friend
      idxgui& operator<<(idxgui& r, const T val);

    //! draws an arrow from (h1, w1) to (h2, w2).
    void draw_arrow(int h1, int w1, int h2, int w2);

    //! draws a bounding box with top left corner (h0, w0) and size (h, w).
    //! the (r,g,b) color of the box can optionally be specified as well as
    //! a caption string.
    void draw_box(int h0, int w0, int h, int w,
		  unsigned char r = 255, unsigned char g = 255,
		  unsigned char b = 255, string *s = NULL);

    //! do not show windows, instead save them in png files in current dir.
    void set_silent();
    //! do not show windows, instead save them in png files in current dir.
    void set_silent(const std::string *filename);
    //! do not show windows, instead save them in png files in current dir.
    void set_silent(const char *filename);

    //! do not update display until enable_updates is called again.
    //! this should make the display faster.
    void disable_updates();

    //! allow display updates and display if it was previously off after a 
    //! call to disable_updates().
    void enable_updates();

    //! closes all windows.
    void quit();
    
    //! clears the window.
    void clear();

    //! save current window into filename image.
    //! wid is optional, if given save window with id wid.
    void save_window(const char *filename, int wid = -1);

    //! draw_matrix displays your idx2 or the first layer of your idx3 in
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
		       double zoomh = 1.0, double zoomw = 1.0,
		       T minv = 0, T maxv = 0);
    
    //! same as draw_matrix but draws a frame of color (r,g,b) around it.
    template<class T>
      void draw_matrix_frame(idx<T> &im, ubyte r, ubyte g, ubyte b,
			     unsigned int h0 = 0, unsigned int w0 = 0, 
			     double zoomh = 1.0, double zoomw = 1.0,
			     T minv = 0, T maxv = 0);

    //! same a draw_matrix but overlays the string <str> in the top left corner.
    template<class T>
      void draw_matrix(idx<T> &im, const char *str, 
		       unsigned int h0 = 0, unsigned int w0 = 0, 
		       double zoomh = 1.0, double zoomw = 1.0,
		       T minv = 0, T maxv = 0);

    //! draws text on the current window.
    //! you can also use the << operator instead of this function to add text 
    //! to the gui. for example: gui << "text" << endl;
    void draw_text(std::string *s);

    //! draws text on the current window at origin (h0, w0).
    //! you can also use the << operator instead of this function to add text 
    //! to the gui. for example: gui << at(h0, w0) << "text" << endl;
    void draw_text(std::string *s, unsigned int h0, unsigned int w0);

    //! sets the origin of further calls to draw_text or gui << "text".
    //! you can also use the at() function instead of this one.
    //! for example: gui << at(42, 0) << "text";
    void set_text_origin(unsigned int h0, unsigned int w0);

    //! sets the text color for further calls to draw_text or gui << "text".
    //! you can also use the set_colors() function to set
    //! text and background colors and transparency.
    //! for example: gui << set_colors(255, 255, 255, 255, 0, 0, 0, 127);
    //! this sets the text color to fully opaque white on a semi-transparent
    //! black background.
    void set_text_colors(unsigned char fg_r, unsigned char fg_g, 
			 unsigned char fg_b, unsigned char fg_a,
			 unsigned char bg_r, unsigned char bg_g, 
			 unsigned char bg_b, unsigned char bg_a);
    //! see unsigned char version.
    void set_text_colors(int fg_r, int fg_g, 
			 int fg_b, int fg_a,
			 int bg_r, int bg_g, 
			 int bg_b, int bg_a);

    //! Set color of background.
    void set_bg_colors(unsigned char r, unsigned char g, unsigned char b);

    //! set the << operator to output text on both std::cout and the current
    //! window.
    //! you can also use the cout_and_gui() function.
    //! for example: gui << cout_and_gui() << "text";
    void set_cout_and_gui();
    
    //! set the << operator to output text only to the current window.
    //! you can also use the gui_only() function and not to std::cout.
    //! for example: gui << gui_only() << "text";
    void set_gui_only();

    void add_scroll_box(scroll_box0* sb);

    //! Freeze or unfreeze style, no modification of colors are allowed when
    //! frozen.
    void freeze_style(bool freeze);

    //! Return the first key pressed in the queue of key events and remove it
    //! from the queue.
    int pop_key_pressed();
    
  private:
    // check that user used MAIN_QTHREAD instead of regular main
    void check_init(); 

  signals:
    void gui_drawImage(idx<ubyte> *img, unsigned int h0, unsigned int w0);
    void appquit();
    void gui_clear();
    void gui_save_window(const string *filename, int wid);
    void gui_new_window(const char *wname, unsigned int h, unsigned int w);
    void gui_select_window(int wid);
    void gui_add_text(const string *s);
    void gui_add_arrow(int h1, int w1, int h2, int w2);
    void gui_add_box(int h0, int w0, int h, int w, unsigned char r,
		     unsigned char g, unsigned char b, string *s);
    void gui_set_text_origin(unsigned int h0, unsigned int w0);
    void gui_set_text_colors(unsigned char fg_r, unsigned char fg_g, 
			     unsigned char fg_b, unsigned char fg_a,
			     unsigned char bg_r, unsigned char bg_g, 
			     unsigned char bg_b, unsigned char bg_a);
    void gui_set_bg_colors(unsigned char r, unsigned char g, 
			   unsigned char b);
    void gui_set_silent(const std::string *filename);
    void gui_set_wupdate(bool update);
    void gui_freeze_style(bool freeze);
    void gui_add_scroll_box(scroll_box0 *sb);

  protected:
    virtual void run();
  };

  //! specifies the origin of the text to draw.
  //! calling 'gui << at(42, 0) << "text";' will draw "text" at height 42
  //! and with 0.
  ManipInfra<unsigned int, unsigned int> at(unsigned int h0, unsigned int w0);
  idxgui& att(idxgui& r, unsigned int h0, unsigned int w0);

  //! specifies to output text to both the gui and std::cout.
  //! usage: gui << cout_and_gui();
  ManipInfra<int, int> cout_and_gui();
  idxgui& fcout_and_gui(idxgui& r);

  //! specifies to output text to both the gui and std::cout.
  //! usage: gui << gui_only();
  ManipInfra<int, int> gui_only();
  idxgui& fgui_only(idxgui& r);

  //! set the text color to black on white background with optional 
  //! transparency factors fg_a and bg_a respectively for foreground and 
  //! background colors, ranging from 0 to 255.
  //! usage: gui << black_on_white();
  //! usage: gui << black_on_white(127);
  //! usage: gui << black_on_white(50, 255);
  ManipInfra<unsigned char, unsigned char> 
  black_on_white(unsigned char fg_a = 255,
		 unsigned char bg_a = 255);
  idxgui& fblack_on_white(idxgui& r, unsigned char fg_a, 
			  unsigned char bg_a);

  //! set the text color to white on a transparent background 
  //! (transparency = 127 with black background).
  idxgui& fwhite_on_transparent(idxgui& r);
  ManipInfra<int, int> white_on_transparent();

  //! set the text color to rgba on a rgba background where each value ranges
  //! from 0 to 255.
  //! usage: gui << set_colors(255, 255, 255, 255, 0, 0, 0, 127);
  //! this sets the text color to fully opaque white on a semi-transparent black
  //! background.
  ManipInfra<unsigned char, unsigned char> 
  set_colors(unsigned char fg_r, unsigned char fg_g, 
	     unsigned char fg_b, unsigned char fg_a,
	     unsigned char bg_r, unsigned char bg_g, 
	     unsigned char bg_b, unsigned char bg_a);
  idxgui& fset_colors(idxgui& r, 
		      unsigned char fg_r, unsigned char fg_g, 
		      unsigned char fg_b, unsigned char fg_a,
		      unsigned char bg_r, unsigned char bg_g, 
		      unsigned char bg_b, unsigned char bg_a);

} // namespace ebl {

#include "idxgui.hpp"

#endif /* IDXGUI_H_ */
