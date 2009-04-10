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

#include "gui_thread.moc"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // GuiThread

  // global variable
  idxgui gui;

  gui_thread::gui_thread(int argc, char** argv) 
    : wcur(-1), nwindows(0), silent(false), thread(gui) {
    thread.init(argc, argv);
    connect(&thread, SIGNAL(gui_drawImage(idx<ubyte> *, 
					  unsigned int, unsigned int)),
	    this,   SLOT(updatePixmap(idx<ubyte> *, 
				      unsigned int, unsigned int)));
    connect(&thread, SIGNAL(appquit()), this, SLOT(appquit()));
    connect(&thread, SIGNAL(gui_clear()), this, SLOT(clear()));
    connect(&thread, SIGNAL(gui_new_window(const char*, unsigned int, 
					   unsigned int)), 
	    this, SLOT(new_window(const char*, unsigned int, unsigned int)));
    connect(&thread, SIGNAL(gui_select_window(unsigned int)), 
	    this, SLOT(select_window(unsigned int)));
    connect(&thread, SIGNAL(gui_add_text(const std::string*)), 
	    this, SLOT(add_text(const std::string*)));
    connect(&thread, SIGNAL(gui_add_arrow(int, int, int, int)), 
	    this, SLOT(add_arrow(int, int, int, int)));
    connect(&thread, SIGNAL(gui_set_text_origin(unsigned int, unsigned int)), 
	    this, SLOT(set_text_origin(unsigned int, unsigned int)));
    connect(&thread, SIGNAL(gui_set_text_colors(unsigned char, unsigned char, 
						unsigned char, unsigned char,
						unsigned char, unsigned char, 
						unsigned char, unsigned char)), 
	    this, SLOT(set_text_colors(unsigned char, unsigned char, 
				       unsigned char, unsigned char,
				       unsigned char, unsigned char, 
				       unsigned char, unsigned char)));
    connect(&thread, SIGNAL(gui_set_silent(const std::string *)), 
	    this, SLOT(set_silent(const std::string *)));
    connect(&thread, SIGNAL(gui_set_wupdate(bool)), 
	    this, SLOT(set_wupdate(bool)));
  }

  gui_thread::~gui_thread() {
    cout << "destructor ****" << endl;
    for (vector<Window*>::iterator i = windows.begin(); i != windows.end(); ++i)
      if (*i)
	delete *i;
  }

  void gui_thread::window_destroyed(QObject *obj) {
    for (vector<Window*>::iterator i = windows.begin(); i != windows.end(); ++i)
      {
	if (*i == obj) {
	  *i = NULL;
	  nwindows--;
	}
      }
  }

  void gui_thread::add_text(const std::string *s) {
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->add_text(s);
  }

  void gui_thread::add_arrow(int x1, int y1, int x2, int y2) {
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->add_arrow(x1, y1, x2, y2);
  }

  void gui_thread::set_text_origin(unsigned int h0, unsigned int w0) {
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->set_text_origin(h0, w0);
  }

  void gui_thread::set_text_colors(unsigned char fg_r, unsigned char fg_g, 
				   unsigned char fg_b, unsigned char fg_a,
				   unsigned char bg_r, unsigned char bg_g, 
				   unsigned char bg_b, unsigned char bg_a) { 
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->set_text_colors(fg_r, fg_g, fg_b, fg_a, 
				     bg_r, bg_g, bg_b, bg_a);
  }

  void gui_thread::set_wupdate(bool update) {
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->set_wupdate(update);
  }

  void gui_thread::set_silent(const std::string *filename) {
    silent = true;
    for (vector<Window*>::iterator i = windows.begin(); i != windows.end(); ++i)
      {
	if (*i)
	  (*i)->set_silent(filename);
      }
    if (filename) {
      savefname = *filename;
      delete filename;
    }
  }

  void gui_thread::appquit() {
    exit(0);
  }

  void gui_thread::updatePixmap(idx<ubyte> *img, unsigned int h0, 
			       unsigned int w0) {
    //    if (nwindows == 0)
    //  new_window();
    if ((wcur >= 0) && (windows[wcur])) {
      windows[wcur]->update_pixmap(img, h0, w0);
    }
  }

  void gui_thread::clear() {
    if ((wcur >= 0) && (windows[wcur]))
      windows[wcur]->clear();
  }

  void gui_thread::new_window(const char *wname, unsigned int h, 
			      unsigned int w){
    windows.push_back(new Window(windows.size(), wname, h, w));
    wcur = windows.size() - 1;
    nwindows++;
    if (silent)
      windows[wcur]->set_silent(&savefname);
    connect(windows[wcur], SIGNAL(destroyed(QObject*)), 
    	    this, SLOT(window_destroyed(QObject*)));
  }

  void gui_thread::select_window(unsigned int wid) {
    if (wid >= windows.size()) {
      cerr << "idxGui Warning: trying to select an unknown window (id = ";
      cerr << wid << ")." << endl;
    }
    else if (windows[wid] == NULL) {
      wcur = -1;
      //cerr << 
      //"idxGui Warning: trying to select an window that was destroyed (id = ";
      //cerr << wid << ")." << endl;
    }
    else {
      wcur = wid;
      if ((windows[wcur]) && (!silent)) {
	windows[wcur]->show();
      }
    }
  }

} // end namespace ebl
