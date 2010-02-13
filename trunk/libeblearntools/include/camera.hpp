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

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera<Tdata>::camera(int id, int height_, int width_)
    : height(height_), width(width_), resize(false) {
#ifndef __OPENCV__
  eblerror("opencv not found, install and recompile");
#else
    cout << "Initializing camera..." << endl;
    capture = cvCaptureFromCAM(id);
    if (!capture) {
      fprintf( stderr, "ERROR: capture is NULL \n" );
      getchar();
      eblerror("failed to initialize camera");
    }
    // get 1st frame to initialize image buffer
    ipl_frame = cvQueryFrame(capture);
    if (!ipl_frame)
      eblerror("failed to grab first frame");
    frame = ipl2idx<Tdata>(ipl_frame);
    // decide if we resize input or not
    if ((height != -1) && (width != -1))
      resize = true;
#endif /* __OPENCV__ */
  }
  
  template <typename Tdata>
  camera<Tdata>::~camera() {
#ifdef __OPENCV__
    // release camera
    cvReleaseCapture(&capture);
#endif /* __OPENCV__ */
  }
  
  ////////////////////////////////////////////////////////////////
  // frame grabbing

  template <typename Tdata>
  idx<Tdata> camera<Tdata>::grab() {
#ifdef __OPENCV__
    ipl_frame = cvQueryFrame(capture);
    if (!ipl_frame)
      eblerror("failed to grab frame");
    // convert ipl to idx image
    ipl2idx(ipl_frame, frame);
#endif /* __OPENCV__ */
    if (!resize)
      return frame; // return original frame
    else // or return a resized frame
      return image_mean_resize(frame, height, width, 0);
  }

  ////////////////////////////////////////////////////////////////
  // video recording
    
  template <typename Tdata>
  bool camera<Tdata>::start_recording(uint window_id, const string &path,
				      const string &name) {
// //     // video
// //     //    ostringstream oss;
// //     //    oss << "video/frame_" << setfill('0') << setw(5) << iframe << ".png";
// //     //    cout << "saving " << oss.str() << endl;
// //     //    save_window(oss.str().c_str());
// //     //    usleep(200000);
    return true;
  }

  template <typename Tdata>
  bool camera<Tdata>::record_frame() {
    return true;
  }
    
  template <typename Tdata>
  bool camera<Tdata>::stop_recording(uint fps) {
    return true;
  }

  ////////////////////////////////////////////////////////////////
  // info
  
  template <typename Tdata>
  float camera<Tdata>::fps() {
    // counters
    // cnt++;
    // iframe++;
    // time(&t1);
    // diff = difftime(t1, t0);
    // if (diff >= 1) {
    //   fps = cnt;
    //   cnt = 0;
    //   time(&t0);
    //   cout << "fps: " << fps << endl;
    // }
    return 0;
  }
    
} // end namespace ebl

#endif /* CAMERA_HPP_ */
