/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
 *   All rights reserved.
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

#ifndef DATASOURCE_GUI_HPP_
#define DATASOURCE_GUI_HPP_

#include <ostream>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // labeled_datasource_gui

  template<typename Tdata, typename Tlabel>
  void labeled_datasource_gui::display(labeled_datasource<Tdata, Tlabel> &ds,
				       unsigned int nh, unsigned int nw,
				       unsigned int h0, unsigned int w0,
				       double zoom, int wid, const char *wname){
    display_wid = (wid >= 0) ? wid : 
      gui.new_window((wname ? wname : ds.name), 
		     nh * (ds.height + 1) - 1, nw * (ds.width + 1) - 1);
    gui.select_window(display_wid);
    gui.disable_updates();
    gui << white_on_transparent() << gui_only();
    idxdim d = ds.sample_dims();
    state_idx s(d);
    idx<double> m = s.x.select(0, 0);
    idx<Tlabel> lbl;
    ds.seek_begin();
    unsigned int h = h0, w = w0;
    for (unsigned int ih = 0; ih < nh; ++ih) {
      for (unsigned int iw = 0; iw < nw; ++iw) {
	ds.fprop(s, lbl);
	ds.next();
	m = s.x.select(0, 0);
	gui.draw_matrix(m, h, w, zoom, zoom);
	if ((ds.lblstr) && (ds.lblstr->at((int)lbl.get())))
	  gui << at(h + 1, w + 1) << (ds.lblstr->at((int)lbl.get()))->c_str();
	w += m.dim(1) + 1;
      }
      w = w0;
      h += m.dim(0) + 1;
    }
    ds.seek_begin();
    gui.enable_updates();
  }

} // end namespace ebl

#endif /* DATASOURCE_GUI_HPP_ */
