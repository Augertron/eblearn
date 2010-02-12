/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
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

#ifndef CAMERA_H_
#define CAMERA_H_

#include "libidx.h"
#include "opencv.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace std;

namespace ebl {

#ifdef __OPENCV__

  //! The camera class interfaces with the opencv camera and images.
  //! It allows to grab images from camera in the idx format, and also
  //! to save gui outputs into video files.
  template <typename Tdata> class camera {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors/allocation

    //! Initialize opencv camera using id to choose which camera to use.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param id The ID of the camera. -1 chooses the first available camera.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    camera(int id, int height = -1, int width = -1);

    //! Destructor.
    virtual ~camera();

    ////////////////////////////////////////////////////////////////
    // frame grabbing

    //! Return a new frame.
    idx<Tdata> grab();

    ////////////////////////////////////////////////////////////////
    // video recording
    
    //! Start recording of frames from window window_id into path.
    //! This creates a directory name in path.
    //! No frames are actually recorded,
    bool start_recording(uint window_id, const string &path,
			 const string &name);

    //! Dump all frames declared to be recorded by start_recording().
    bool record_frame();
    
    //! Create all videos started with start_recording() using frames dumped
    //! with record_frame(), using fps frames per second.
    bool stop_recording(uint fps);

    ////////////////////////////////////////////////////////////////
    // info

    //! return the number of frames per second obtained via grab().
    float fps();
    
    // members ////////////////////////////////////////////////////////
  protected:
    idx<Tdata>	 frame;		//!< frame buffer
    CvCapture	*capture;	//!< opencv capture object
    IplImage	*ipl_frame;	//!< opencv image
    int          height;        //!< resize input
    int          width;         //!< resize input
    bool         resize;        //!< resize or not
  };

} // end namespace ebl

#include "camera.hpp"

#endif /* __OPENCV__ */

#endif /* CAMERA_H_ */
