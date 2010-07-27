/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef IMAGEIO_HPP_
#define IMAGEIO_HPP_

#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#ifndef __WINDOWS__
#include <unistd.h>
#endif

#ifdef __MAGICKPP__
#include <Magick++.h>
#endif

#include <string.h>
#include <errno.h>
#include "config.h"
#include "idxIO.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // I/O: helper functions

  template<class T>
  idx<T> image_read(const char *fname, idx<T> *out_) {
    ostringstream err;
    idx<ubyte> tmp;
#if (defined(__IMAGEMAGICK__) && !defined(__NOIMAGEMAGICK__))
    // we are under linux or mac and convert is available
    ostringstream cmd;
    cmd << IMAGEMAGICK_CONVERT << " -compress lossless -depth 8 \"" 
	<< fname << "\" PPM:-";
#ifdef __WINDOWS__
    FILE* fp = POPEN(cmd.str().c_str(), "rb");
#else
    FILE* fp = POPEN(cmd.str().c_str(), "r");
#endif
    if (!fp) {
      err << "conversion of image " << fname << " failed (errno: " 
	  << errno << ", " << strerror(errno) << ")";
      throw err.str();
    }
    try {
    // read pnm image
    tmp = pnm_read(fp);
    } catch (string &err) {
      if (PCLOSE(fp) != 0) {
	cerr << "Warning: pclose failed (errno: " << errno << ")" << endl;
      }
      throw err;
    }
    if (PCLOSE(fp) != 0) {
      cerr << "Warning: pclose failed (errno: " << errno << ")" << endl;
    }
#else
#ifdef __MAGICKPP__
    // we are under any platform, convert is not available but Magick++ is
    try {
      eblerror("not implemented");
      Magick::Image im(fname);
      tmp = idx<ubyte>(im.rows(), im.columns(), im.depth() / 8);
      im.write(0, 0, im.depth() / 8, im.rows(), "RGB", (Magick::StorageType)0, 
	       tmp.idx_ptr());
    }
    catch( Magick::WarningCoder &warning )
      {
	// Process coder warning while loading file (e.g. TIFF warning)
	// Maybe the user will be interested in these warnings (or not).
	// If a warning is produced while loading an image, the image
	// can normally still be used (but not if the warning was about
	// something important!)
	cerr << "Coder Warning: " << warning.what() << endl;
      }
    catch( Magick::Warning &warning )
      {
	// Handle any other Magick++ warning.
	cerr << "Warning: " << warning.what() << endl;
      }
    catch( Magick::ErrorBlob &error ) 
      { 
	// Process Magick++ file open error
	cerr << "Error: " << error.what() << endl;
      }
#else
    // nor Magick++ nor convert are available, error
    eblerror("Nor ImageMagick's convert nor Magick++ are available, \
please install");
#endif /* __MAGICK++__ */
#endif /* __IMAGEMAGICK__ */
    
    idxdim dims(tmp);
    idx<T> out;
    idx<T> *pout = &out;
    // allocate if not allocated
    if (!out_)
      out = idx<T>(dims);
    else
      pout = out_;
    // resize if necessary
    if (pout->get_idxdim() != dims)
      pout->resize(dims);
    // copy/cast
    idx_copy(tmp, *pout);
    return *pout;
  }

  ////////////////////////////////////////////////////////////////
  // I/O: loading

  template<class T>
  void load_image(const char *fname, idx<T> &out) {
    // first try if the image is a mat file
    try {
      if (out.order() > 3 || out.order() < 2)
	eblerror("image has to be 2D or 3D");
      load_matrix<T>(out, fname);
      // channels are likely in dim 0 if size 1 or 3
      if (((out.dim(0) == 1) || (out.dim(0) == 3)) && (out.order() == 3))
	out = out.shift_dim(0, 2);
      return ;
    } catch(string &err) {
      err = err;
      // not a mat file, try regular image
    }
    image_read(fname, &out);
  }

  template<class T>
  void load_image(const string &fname, idx<T> &out) {
    return load_image(fname.c_str(), out);
  }
  
  template<class T>
  idx<T> load_image(const char *fname) {
    // first try if the image is a mat file
    try {
      idx<T> m = load_matrix<T>(fname);
      if (m.order() > 3 || m.order() < 2)
	eblerror("image has to be 2D or 3D");
      // channels are likely in dim 0 if size 1 or 3
      if (((m.dim(0) == 1) || (m.dim(0) == 3)) && (m.order() == 3))
	m = m.shift_dim(0, 2);
      return m;
    } catch(string &err) {
      err = err; // not a mat file, try regular image
    }
    return image_read<T>(fname);
  }

  template<class T>
  idx<T> load_image(const string &fname) {
    return load_image<T>(fname.c_str());
  }

  ////////////////////////////////////////////////////////////////
  // I/O: saving

  template<class T>
  bool save_image_ppm(const string &fname, idx<T> &in) {
    // check order
    // TODO: support grayscale
    if (in.order() != 3) {
      cerr << "error: image order (" << in.order() << " not supported." << endl;
      return false;
    }
    // save as ppm
    FILE *fp = fopen(fname.c_str(), "wb");
    if (!fp) {
      cerr << "error: failed to open file " << fname << endl;
      return false;
    }
    fprintf(fp,"P6 %d %d 255\n", (int) in.dim(1), (int) in.dim(0));
    if (in.dim(2) == 3) {
      idx_bloop1(inn, in, T) {
	idx_bloop1(innn, inn, T) {
	  fputc((ubyte) innn.get(0), fp);
 	  fputc((ubyte) innn.get(1), fp);
	  fputc((ubyte) innn.get(2), fp);
	}
      }
    } else if (in.dim(2) == 1) {
      idx_bloop1(inn, in, T) {
	idx_bloop1(innn, inn, T) {
	  fputc((ubyte) innn.get(0), fp);
	}
      }
    } else {
      cerr << "Error saving image " << in << endl;
      eblerror("Pixel dimension not supported");
    }
    fclose(fp);
    return true;
  }

  template<class T>
  bool save_image_jpg(const string &fname, idx<T> &in) {
    return save_image(fname, in, "JPG");
  }

  template<class T>
  bool save_image(const string &fname, idx<T> &in, const char *format) {
    // save as ppm
    string fname2 = fname;
    fname2 += ".ppm";
    if (!save_image_ppm(fname2, in))
      return false;
    // convert ppm to jpg
    string cmd = "convert PPM:";
    cmd += fname2;
    cmd += " ";
    cmd += format;
    cmd += ":";
    cmd += fname;
    int n = ::system(cmd.c_str());
    if (n != 0) {
      cerr << "error (" << n << "): failed to save image " << fname;
      cerr << " to format " << format << endl;
      return false;
    }
    remove(fname2.c_str());
    return true;
  }

} // end namespace ebl

#endif /* IMAGE_HPP_ */
