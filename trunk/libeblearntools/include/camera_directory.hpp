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

#include <sstream>

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <typename Tdata>
  camera_directory<Tdata>::camera_directory(const char *dir,
					    int height_, int width_,
					    bool randomize_, uint npasses_)
    : camera<Tdata>(height_, width_), indir(dir),
      randomize(randomize_), npasses(npasses_) {
    if (npasses == 0)
      eblerror("number of passes must be >= 1");
    cout << "Initializing directory camera from: " << dir << endl;
    read_directory(dir);
  }

  template <typename Tdata>
  camera_directory<Tdata>::camera_directory(int height_, int width_,
					    bool randomize_, uint npasses_)
    : camera<Tdata>(height_, width_),
      randomize(randomize_), npasses(npasses_) {
    if (npasses == 0)
      eblerror("number of passes must be >= 1");
  }

  template <typename Tdata>
  bool camera_directory<Tdata>::read_directory(const char *dir) {
    string directory = dir;
    indir = dir;
    // // first count number of images, to allocate list and speed up
    // uint n = count_files(directory, IMAGE_PATTERN);
    // if (fl) delete fl;
    // fl = new files_list(n);
    // get all file names
    fl = find_files(directory, IMAGE_PATTERN, NULL, false, true, randomize);
    if (!fl) {
      cerr << "invalid directory: " << dir << endl;
      eblerror("invalid directory");
      return false;
    }
    cout << "Found " << fl->size() << " images." << endl;
    if (randomize)
      cout << "Image list is randomized." << endl;
    if (npasses > 1)
      cout << "Image list will be used " << npasses << " times." << endl;
    flsize = fl->size() * npasses;
    fli = fl->begin(); // initialize iterator to beginning
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
    fdir = fli->first; // directory
    fname = fli->second; // file name
    ostringstream fn("");
    if (strcmp(fdir.c_str(), ""))
      fn << fdir << "/";
    fn << fname << "_" << frame_id;
    frame_name_ = fn.str();
    if (strcmp(fdir.c_str(), "")) {
      size_t npos = frame_name_.length() - indir.length();
      frame_name_ = frame_name_.substr(indir.length(), npos);
    }
    for (size_t i = 0; i < frame_name_.length(); ++i)
      if (frame_name_[i] == '/')
	frame_name_[i] = '_';
    fli++; // move to next element
    cout << frame_id << "/" << flsize << ": processing ";
    cout << fdir << "/" << fname << endl;
    oss.str(""); oss << fdir << "/" << fname;
    try {
      frame = load_image<Tdata>(oss.str());
    } catch (const string &err) {
      cerr << err << ". Trying next image..." << endl;
      return grab();
    }
    return this->postprocess();
  }
    
  template <typename Tdata>
  bool camera_directory<Tdata>::empty() {
    if (!fl)
      eblerror("directory not initialized");
    if (fli == fl->end()) {
      if (npasses > 0) {
	npasses--;
	fli = fl->begin(); // reset to begining.
      }
      if (npasses == 0) // we did all passes, stop.
	return true;
    }
    return false;
  }
    
  template <typename Tdata>
  string camera_directory<Tdata>::frame_name() {
    return frame_name_;
  }
  
} // end namespace ebl

#endif /* CAMERA_DIRECTORY_HPP_ */
