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

#ifndef CLASSIFIER2D_GUI_H_
#define CLASSIFIER2D_GUI_H_

#include "libidxgui.h"
#include "classifier2D.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // classifier2D_gui

  class classifier2D_gui {
  private:
    unsigned int	 display_wid;
    unsigned int	 display_wid_fprop;

  public:
    classifier2D_gui() {};
    virtual ~classifier2D_gui() {};

    //! displays only the output of the classifier after a a call to 
    //! classifier2D::fprop(img, zoom, threshold, objsize) at coordinates 
    //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified, 
    //! use that window, otherwise create a new window and reuse it.
    //! <wname> is an optional window title.
    template <class Tdata>
    void display(classifier2D<Tdata> &cl, idx<Tdata> &img, 
		 float zoom, double threshold,
		 int objsize,
		 unsigned int h0 = 0, unsigned int w0 = 0, 
		 double dzoom = 1.0, int wid = -1, 
		 const char *wname = NULL);

    //! display the regular input/output display but also the inputs and outputs
    //! corresponding to each scale, corresponding to a call to 
    //! classifier2D::fprop(img, zoom, threshold, objsize) at coordinates 
    //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified, 
    //! use that window, otherwise create a new window and reuse it.
    //! <wname> is an optional window title.
    template <class Tdata>
    void display_inputs_outputs(classifier2D<Tdata> &cl, 
				idx<Tdata> &img, float zoom, 
				double threshold, int objsize,
				unsigned int h0 = 0, unsigned int w0 = 0, 
				double dzoom = 1.0, int wid = -1, 
				const char *wname = NULL);
    
    //! display all, display_inputs_outputs and the internal states of the fprop
    //! on the first scale, corresponding to a call to 
    //! classifier2D::fprop(img, zoom, threshold, objsize) at coordinates 
    //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified, 
    //! use that window, otherwise create a new window and reuse it.
    //! <wname> is an optional window title.
    template <class Tdata>
    void display_all(classifier2D<Tdata> &cl, idx<Tdata> &img, 
		     float zoom, double threshold, int objsize,
		     unsigned int h0 = 0, unsigned int w0 = 0, 
		     double dzoom = 1.0, int wid = -1, 
		     const char *wname = NULL);

    //! displays all the current state of the classifier. 
    //! If a window id <wid> is specified, 
    //! use that window, otherwise create a new window and reuse it.
    //! <wname> is an optional window title.
    template <class Tdata>
    void display_current(classifier2D<Tdata> &cl, 
			 idx<Tdata> &sample,
			 int wid = -1, 
			 const char *wname = NULL);

  };

} // end namespace ebl

#include "classifier2D_gui.hpp"

#endif /* CLASSIFIER2D_GUI_H_ */
