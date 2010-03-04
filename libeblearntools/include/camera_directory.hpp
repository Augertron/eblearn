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

#ifndef CAMERA_DIRECTORY_HPP_
#define CAMERA_DIRECTORY_HPP_

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera_directory<Tdata>::camera_directory(const char *dir,
					    int height_, int width_)
    : camera<Tdata>(height_, width_) {
    cout << "Initializing directory camera from: " << dir << endl;
    read_directory(dir);
  }

  template <typename Tdata>
  camera_directory<Tdata>::camera_directory(int height_, int width_)
    : camera<Tdata>(height_, width_) {
  }

  template <typename Tdata>
  bool camera_directory<Tdata>::read_directory(const char *dir) {
    string directory = dir;
    // first count number of images, to allocate list and speed up
    uint n = count_files(directory, IMAGE_PATTERN);
    if (fl) delete fl;
    fl = new files_list(n);
    // get all file names
    fl = find_files(directory, IMAGE_PATTERN, fl);
    if (!fl) {
      cerr << "invalid directory: " << dir << endl;
      eblerror("invalid directory");
      return false;
    }
    cout << "Found " << fl->size() << " images." << endl;
    flsize = fl->size();
    return true;
  }
  
  template <typename Tdata>
  camera_directory<Tdata>::~camera_directory() {
    if (fl)
      delete fl;
  }
  
  ////////////////////////////////////////////////////////////////
  // frame grabbing

  template <typename Tdata>
  idx<Tdata> camera_directory<Tdata>::grab() {
    if (empty())
      eblerror("cannot grab images on empty list");
    fdir = fl->front().first; // directory
    fname = fl->front().second; // file name
    fl->pop_front(); // remove first element
    cout << frame_id << "/" << flsize << ": processing ";
    cout << fdir << "/" << fname << endl;
    oss.str(""); oss << fdir << "/" << fname;
    try {
      frame = load_image<Tdata>(oss.str());
    } catch (const char *err) {
      cerr << "failed to load image " << oss.str();
      cerr << ". Trying next image..." << endl;
      return grab();
    }
    return this->postprocess();
  }
    
  template <typename Tdata>
  bool camera_directory<Tdata>::empty() {
    if (!fl)
      eblerror("directory not initialized");
    return (fl->size() == 0);
  }
    
} // end namespace ebl

#endif /* CAMERA_DIRECTORY_HPP_ */
