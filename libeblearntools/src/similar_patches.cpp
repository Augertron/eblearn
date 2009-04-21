/***************************************************************************
 *   Copyright (C) 2008 by Pierre Sermanet *
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

#include "similar_patches.h"

using namespace std;

namespace ebl {

  similar_patches::similar_patches(unsigned int maxcurrent, 
				   unsigned int maxsimilar,
				   unsigned int ph, unsigned int pw,
				   unsigned int ih, unsigned int iw) 
    : max_similar_patches(maxsimilar), 
      pheight(ph), pwidth(pw), iheight(ih), iwidth(iw),
      dataset(),
      current_patches(maxcurrent, NULL), 
      max_current_patches(maxcurrent), 
      current_patches_xy(maxcurrent) {
#ifdef __GUI__
    wdisplay = gui.new_window("similar_patches");
#endif
  }

  similar_patches::~similar_patches() {
  }

  bool similar_patches::add_similar_patch(idx<ubyte> &im, int h, 
					  int w, unsigned int index) {
      int h0 = h - pheight / 2;
      int w0 = w - pwidth / 2;
      // don't add patches outsides of the image
      if ((h0 < 0) || (w0 < 0) 
	  || (w0 + pheight >= iheight) || (w0 + pwidth >= iwidth))
	return false;
      if (index >= max_current_patches)
	return false; // the index is wrong
      // make a copy of the patch
      idx<ubyte>* patch = new idx<ubyte>(pheight, pwidth);
      idx<ubyte> im2 = im.narrow(0, pheight, h0);
      im2 = im2.narrow(1, pwidth, w0);
      idx_copy(im2, *patch);
      // if current spot is NULL, create a new vector
      if (current_patches[index] == NULL)
	current_patches[index] = new vector<idx<ubyte>*>;
      current_patches[index]->push_back(patch);
      current_patches_xy[index].first = w;
      current_patches_xy[index].second = h;
      // we reached the maximum number of patches, add group in
      // database and free spot for a new patch
      if (current_patches[index]->size() >= max_similar_patches) {
	// add group of similar patches to dataset
	dataset.push_back(current_patches[index]);
	// free up patch spot
	current_patches[index] = NULL;
      }
      return true;
    }

  bool similar_patches::current_patch_empty(unsigned int index) {
    if (index >= current_patches.size())
      return false;
    return (current_patches[index] == NULL) ? true : false;
  }

  void similar_patches::display_dataset(unsigned int maxh, unsigned int maxw) {
#ifdef __GUI__
    gui.select_window(wdisplay);
    gui.disable_updates();
    unsigned int h = 0, w = 0;
    vector<vector<idx<ubyte>*>*>::iterator i = dataset.begin();
    for ( ; (i != dataset.end()) && (h < maxh); ++i) {
      if ((*i) != NULL) {
	vector<idx<ubyte>*>::iterator k = (*i)->begin();
	for ( ; (k != (*i)->end()) && (h < maxh); ++k) {
	  if (w + (*k)->dim(1) > maxw) {
	    w = 0;
	    h += (*k)->dim(0);
	  }
	  gui.draw_matrix(**k, h, w);
	  w += (*k)->dim(1);
	}
      }
    }
#endif
  }

  } // end namespace ebl

