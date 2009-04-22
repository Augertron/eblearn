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

#include "gui.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // gui

  unsigned int new_window(const char *wname, unsigned int h,
			  unsigned int w) {
    gui.new_window(wname, h, w);
  }

  void select_window(unsigned int wid) {
    gui.select_window(wid);
  }

  void disable_window_updates() {
    gui.disable_updates();
  }

  void enable_window_updates() {
    gui.enable_updates();
  }

  void quit_gui() {
    gui.quit();
  }
  
  void clear_window() {
    gui.clear();
  }

  void draw_arrow(int h1, int w1, int h2, int w2) {
    gui.draw_arrow(h1, w1, h2, w2);
  }

  void set_gui_silent() {
    gui.set_silent();
  }

  void set_gui_silent(const std::string *filename) {
    gui.set_silent(filename);
  }

  void set_gui_silent(const char *filename) {
    gui.set_silent(filename);
  }

  void draw_text(std::string *s) {
    gui.draw_text(s);
  }

  void draw_text(std::string *s, unsigned int h0, unsigned int w0) {
    gui.draw_text(s, h0, w0);
  }

  void set_window_text_origin(unsigned int h0, unsigned int w0) {
    gui.set_text_origin(h0, w0);
  }

  void set_window_text_colors(unsigned char fg_r, unsigned char fg_g,
			      unsigned char fg_b, unsigned char fg_a,
			      unsigned char bg_r, unsigned char bg_g,
			      unsigned char bg_b, unsigned char bg_a) {
    gui.set_text_colors(fg_r, fg_g, fg_b, fg_a, bg_r, bg_g, bg_b, bg_a);
  }

  void set_window_text_colors(int fg_r, int fg_g, int fg_b, int fg_a,
			      int bg_r, int bg_g, int bg_b, int bg_a) {
    gui.set_text_colors(fg_r, fg_g, fg_b, fg_a, bg_r, bg_g, bg_b, bg_a);
  }

  void set_window_cout_and_gui() {
    gui.set_cout_and_gui();
  }

  void set_window_gui_only() {
    gui.set_gui_only();
  }


} // end namespace ebl
