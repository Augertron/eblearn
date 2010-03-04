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

#ifndef DETECTOR_GUI_H_
#define DETECTOR_GUI_H_

#include "libidxgui.h"
#include "libeblearn.h"
#include "detector.h"

#include <deque>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // detector_gui

  template <typename T>
    class detector_gui {
  public:
    //! Constructor.
    //! \param show_detqueue If true, show the queue of the latest detected
    //!        windows with step 'step'.
    //! \param step Step for show_detqueue.
    //! \param qheight Number of rows to show for show_detqueue.
    //! \param qwidth Number of cols to show for show_detqueue.
    //! \param show_detqueue2 If true, show another queue of the latest detected
    //!        windows with step 'step2'.
    //! \param step2 Step for show_detqueue2.
    //! \param qheight2 Number of rows to show for show_detqueue2.
    //! \param qwidth2 Number of cols to show for show_detqueue2.
    detector_gui(bool show_detqueue = false, uint step = 1, uint qheight = 5,
		 uint qwidth = 5, bool show_detqueue2 = false, uint step2 = 1,
		 uint qheight2 = 5, uint qwidth2 = 5);

    //! Destructor.
    virtual ~detector_gui();

    //! displays only the output of the classifier after a a call to 
    //! detector::fprop(img, zoom, threshold, objsize) at coordinates 
    //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified, 
    //! use that window, otherwise create a new window and reuse it.
    //! <wname> is an optional window title.
    template <typename Tdata>
      vector<bbox*>& display(detector<Tdata> &cl, idx<T> &img, double threshold,
			     unsigned int h0 = 0, unsigned int w0 = 0, 
			     double dzoom = 1.0, Tdata vmin = 0, Tdata vmax = 0,
			     int wid = -1, const char *wname = NULL);

    //! displays only the output of the classifier after a a call to 
    //! detector::fprop(img, zoom, threshold, objsize) at coordinates 
    //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified, 
    //! use that window, otherwise create a new window and reuse it.
    //! <wname> is an optional window title.
    template <typename Tdata>
      vector<bbox*>& display_input(detector<Tdata> &cl, idx<T> &img,
				   double threshold,
				   unsigned int h0 = 0, unsigned int w0 = 0, 
				   double dzoom = 1.0, Tdata vmin = 0,
				   Tdata vmax = 0,
				   int wid = -1, const char *wname = NULL);

    //! display the regular input/output display but also the inputs and outputs
    //! corresponding to each scale, corresponding to a call to 
    //! detector::fprop(img, zoom, threshold, objsize) at coordinates 
    //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified, 
    //! use that window, otherwise create a new window and reuse it.
    //! <wname> is an optional window title.
    template <typename Tdata>
      vector<bbox*>& display_inputs_outputs(detector<Tdata> &cl, 
					    idx<T> &img, double threshold,
					    unsigned int h0 = 0,
					    unsigned int w0 = 0, 
					    double dzoom = 1.0, Tdata vmin = 0,
					    Tdata vmax = 0, int wid = -1, 
					    const char *wname = NULL);
    
    //! display all, display_inputs_outputs and the internal states of the fprop
    //! on the first scale, corresponding to a call to 
    //! detector::fprop(img, zoom, threshold, objsize) at coordinates 
    //! (h0, w0), with zoom <dzoom>. If a window id <wid> is specified, 
    //! use that window, otherwise create a new window and reuse it.
    //! <wname> is an optional window title.
    template <typename Tdata>
      vector<bbox*>& display_all(detector<Tdata> &cl, idx<T> &img, 
				 double threshold,unsigned int h0 = 0,
				 unsigned int w0 = 0, double dzoom = 1.0,
				 Tdata vmin = 0, Tdata vmax = 0,
				 int wid = -1,const char *wname = NULL);

    //! displays all the current state of the classifier. 
    //! If a window id <wid> is specified, 
    //! use that window, otherwise create a new window and reuse it.
    //! <wname> is an optional window title.
    template <typename Tdata>
      void display_current(detector<Tdata> &cl, 
			   idx<T> &sample,
			   int wid = -1, 
			   const char *wname = NULL);

    ////////////////////////////////////////////////////////////////
    // private methods
  private:

    void update_and_display_queue(deque<idx<T> > &queue, uint step,
				  uint qheight, uint qwidth,
				  vector<idx<T> > &new_detections,
				  uint detcnt, uint &h0, uint &w0,
				  double dzoom);

    ////////////////////////////////////////////////////////////////
    // members
  private:
    int		display_wid;	//!< window id
    int		display_wid_fprop;	//!< window id for fprop
    bool	show_detqueue;	//!< show queue of last detections or not
    bool	show_detqueue2; //!< show queue of last detections or not
    uint	step;		//!< step for detqueue
    uint	step2;		//!< step for detqueue
    uint        qheight;        //!< height of queue display
    uint        qwidth;         //!< width of queue display
    uint        qheight2;	//!< height of queue2 display
    uint        qwidth2;        //!< width of queue2 display
    deque<idx<T> > detqueue;	//!< queue of last detections
    deque<idx<T> > detqueue2;	//!< queue of last detections
    uint        detcnt;         //!< counter of all detections
  };

} // end namespace ebl

#include "detector_gui.hpp"

#endif /* DETECTOR_GUI_H_ */
