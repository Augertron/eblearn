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

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // RenderThread

  RenderThread::RenderThread() {
  }

  void RenderThread::init(int argc_, char **argv_) {
    argc = argc_;
    argv = argv_;
    nwid = 0;
    str("");
    set_cout_and_gui();
  }

  RenderThread::~RenderThread() {
    wait();
  }

  void RenderThread::quit() {
    emit appquit();
  }

  void RenderThread::clear() {
    emit gui_clear();
  }

  unsigned int RenderThread::new_window(const char *wname, unsigned int h, 
					unsigned int w) {
    // TODO: add mutex
    unsigned int wid = nwid;
    nwid++; // increment number of windows
    emit gui_new_window(wname, h, w);
    return wid;
  }

  void RenderThread::select_window(unsigned int wid) {
    emit gui_select_window(wid);
  }

  void RenderThread::set_silent() {
    emit gui_set_silent(NULL);
  }

  void RenderThread::set_silent(const std::string *filename) {
    emit gui_set_silent(filename);
  }

  void RenderThread::set_silent(const char *filename) {
    if (filename)
      emit gui_set_silent(new std::string(filename));
    else
      emit gui_set_silent(NULL);
  }

  void RenderThread::run() {
    ylerror("The run2 method of RenderThread must be overridden and contain \
your code");
  }

  void RenderThread::add_text(std::string *s) {
    emit gui_add_text(s);    
  }

  void RenderThread::set_text_origin(unsigned int h0, unsigned int w0) {
    emit gui_set_text_origin(h0, w0);    
  }

  RenderThread& att(RenderThread& r, unsigned int h0, unsigned int w0) {
    r.set_text_origin(h0, w0);
    return r;
  }

  ManipInfra<unsigned int, unsigned int> at(unsigned int h0, unsigned int w0) {
    return (ManipInfra<unsigned int, unsigned int>(att, h0, w0));
  }

  RenderThread& fcout_and_gui(RenderThread& r) {
    r.set_cout_and_gui();
    return r;
  }

  ManipInfra<int, int> cout_and_gui() {
    return (ManipInfra<int, int>(fcout_and_gui));
  }

  RenderThread& fgui_only(RenderThread& r) {
    r.set_gui_only();
    return r;
  }

  ManipInfra<int, int> gui_only() {
    return (ManipInfra<int, int>(fgui_only));
  }

  void RenderThread::set_cout_and_gui() {
    cout_output = true;
  }

  void RenderThread::set_gui_only() {
    cout_output = false;
  }

} // end namespace ebl
