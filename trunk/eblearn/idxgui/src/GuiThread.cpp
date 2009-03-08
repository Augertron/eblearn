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

#include "GuiThread.moc"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // GuiThread

  RenderThread *window = NULL;

  GuiThread::GuiThread(int argc, char** argv) 
    : wcur(0), thread(argc, argv) {
    connect(&thread, SIGNAL(drawImage(Idx<ubyte> *, int, int)),
	    this,   SLOT(updatePixmap(Idx<ubyte> *, int, int)));
    connect(&thread, SIGNAL(appquit()), this, SLOT(appquit()));
    connect(&thread, SIGNAL(clear()), this, SLOT(clear()));
    connect(&thread, SIGNAL(new_window(const char*, unsigned int*)), 
	    this, SLOT(new_window(const char*, unsigned int*)));
    connect(&thread, SIGNAL(select_window(unsigned int)), 
	    this, SLOT(select_window(unsigned int)));
    connect(&thread, SIGNAL(addText(const std::string*)), 
	    this, SLOT(addText(const std::string*)));
  }

  GuiThread::~GuiThread() {
    for (vector<Window*>::iterator i = windows.begin(); i != windows.end(); ++i)
      if (*i)
	delete *i;
  }

  void GuiThread::window_destroyed(QObject *obj) {
    for (vector<Window*>::iterator i = windows.begin(); i != windows.end(); ++i)
      if (*i == obj) {
	*i = NULL;
      }
  }

  void GuiThread::addText(const std::string *s) {
    if (windows[wcur])
      windows[wcur]->addText(s);
  }

  void GuiThread::appquit() {
    exit(0);
  }

  void GuiThread::updatePixmap(Idx<ubyte> *img, int h0, int w0) {
    if (windows.size() == 0)
      new_window(NULL, &wcur);
    if (windows[wcur]) {
      windows[wcur]->updatePixmap(img, h0, w0);
      windows[wcur]->activateWindow();
    }
  }

  void GuiThread::clear() {
    if (windows[wcur])
      windows[wcur]->clear();
  }

  void GuiThread::new_window(const char *wname, unsigned int *wid) {
    windows.push_back(new Window(wname));
    wcur = windows.size() - 1;
    if (wid)
      *wid = wcur;
    connect(windows[wcur], SIGNAL(destroyed(QObject*)), 
	    this, SLOT(window_destroyed(QObject*)));
  }

  void GuiThread::select_window(unsigned int wid) {
    if (wid >= windows.size()) {
      cerr << "IdxGui Warning: trying to draw in unknown window id (";
      cerr << wid << ")." << endl;
    }
    else {
      wcur = wid;
      if (windows[wcur]) {
	windows[wcur]->show();
      }
    }
  }

} // end namespace ebl
