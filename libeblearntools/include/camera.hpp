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
  camera<Tdata>::camera(int height_, int width_)
    : height(height_), width(width_), bresize(false), frame_id(0),
      grabbed(false), wid(0), fps_grab(0.0), audio_filename("") {
    // decide if we resize input or not
    if ((height != -1) && (width != -1))
      bresize = true;
  }

  template <typename Tdata>
  camera<Tdata>::~camera() {
  }
  
  ////////////////////////////////////////////////////////////////
  // grabbin
  
  template <typename Tdata>
  bool camera<Tdata>::empty() {
    return false; // never empty by default
  }
  
  ////////////////////////////////////////////////////////////////
  // video recording
    
  template <typename Tdata>
  bool camera<Tdata>::start_recording(uint window_id, const char *name) {
    wid = window_id;
    record_cnt = 0;
    if (!name)
      recording_name = "toto2";
    else
      recording_name = name;
    mkdir_full(recording_name);
    return true;
  }

  template <typename Tdata>
  bool camera<Tdata>::record_frame() {
    ostringstream oss;
    oss << recording_name << "/frame_";
    oss << setfill('0') << setw(6) << record_cnt << ".png";
    save_window(oss.str().c_str());
    cout << "saved " << oss.str() << endl;
    record_cnt++;
    return true;
  }
    
  template <typename Tdata>
  bool camera<Tdata>::stop_recording(float fps) {
    ostringstream oss;
    uint optimal_bitrate = 50 * 25 * frame.dim(0) * frame.dim(1) / 256;
    ostringstream options;
    string codec = "msmpeg4v2";
    options << "vbitrate=" << optimal_bitrate << ":mbd=2:keyint=132:";
    options << "vqblur=1.0:cmp=2:subcmp=2:dia=2:mv0:last_pred=3";
    // pass 1
    oss << "mencoder \"mf://" << recording_name;
    oss << "/*.png\" -mf type=png:fps=" << fps;
    oss << " -ovc lavc";
    oss << " -lavcopts vcodec=" << codec << ":vpass=1:" << options.str();
    oss << " -o /dev/null ";
    int ret = system(oss.str().c_str());
    if (ret < 0)
      return false;
    // pass 2
    oss.str("");
    oss << "mencoder \"mf://" << recording_name;
    oss << "/*.png\" -mf type=png:fps=" << fps;
    oss << " -ovc lavc";
    oss << " -lavcopts vcodec=" << codec << ":vpass=2:" << options.str();
    if (audio_filename != "") {
      oss << " -audiofile " << audio_filename;
      oss << " -oac mp3lame -lameopts cbr:br=32 ";
    }
    oss << " -o " << recording_name << ".avi ";
    ret = system(oss.str().c_str());
    if (ret < 0)
      return false;
    cout << "Saved " << recording_name << ".avi";
    cout << " at " << fps << " fps." << endl;
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
    return fps_grab;
  }
    
  ////////////////////////////////////////////////////////////////
  // internal methods
  
  template <typename Tdata>
  idx<Tdata> camera<Tdata>::postprocess() {
    frame_id++;
    if (!bresize)
      return frame; // return original frame
    else // or return a resized frame
      return image_mean_resize(frame, height, width, 0);
  }
  
} // end namespace ebl

#endif /* CAMERA_HPP_ */
