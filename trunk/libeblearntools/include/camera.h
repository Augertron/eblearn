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

#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace std;

namespace ebl {

  //! The camera class is an abstract class that serves as an interface
  //! to different camera implementation, such as camera_opencv, etc.
  //! It allows to grab images from camera in the idx format, and also
  //! to save gui outputs into video files.
  template <typename Tdata> class camera {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors/allocation

    //! Initialize a camera.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    camera(int height = -1, int width = -1);

    //! Destructor.
    virtual ~camera();

    ////////////////////////////////////////////////////////////////
    // frame grabbing

    //! Return a new frame.
    virtual idx<Tdata> grab() = 0;

    //! Return true if no frames available, false otherwise.
    virtual bool empty();

    ////////////////////////////////////////////////////////////////
    // video recording
    
    //! Start recording of frames from window window_id into path.
    //! This creates a directory name in path.
    //! No frames are actually recorded,
    virtual bool start_recording(uint window_id = 0, const char *name = NULL);

    //! Dump all frames declared to be recorded by start_recording().
    virtual bool record_frame();
    
    //! Create all videos started with start_recording() using frames dumped
    //! with record_frame(), using fps frames per second.
    virtual bool stop_recording(float fps);

    ////////////////////////////////////////////////////////////////
    // info

    //! Return the number of frames per second obtained via grab().
    virtual float fps();

    //! Return a name for current frame.
    virtual string frame_name();

    ////////////////////////////////////////////////////////////////
    // internal methods
  protected:

    //! Return post processed frame after grabbing it, e.g. resize frame to
    //! target height and width (if specified).
    //! This also increments frame counter.
    virtual idx<Tdata> postprocess();
    
    // members ////////////////////////////////////////////////////////
  protected:
    idx<Tdata>	 frame;		//!< frame buffer 
    int          height;        //!< resize input
    int          width;         //!< resize input
    bool         bresize;       //!< resize or not
    uint         frame_id;      //!< frame counter
    string	 frame_name_;	//!< frame name
    bool         grabbed;       //!< false if no frame grabbed yet
    uint         wid;           //!< window to record from
    string       recording_name;//!< name of video
    uint         record_cnt;    //!< frame counter for recording
    float        fps_grab;      //!< frames per second of grabbing
    string       audio_filename;//!< filename of audio file
  };

} // end namespace ebl

#include "camera.hpp"

#endif /* CAMERA_H_ */
