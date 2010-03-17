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
#include <ext/stdio_filebuf.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>

#include "idxIO.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // I/O: helper functions

  template<class T>
  idx<T> image_read(const char *fname, idx<T> *out_) {
    ostringstream err;
    /* TODO : add a line in the config to detect "convert" path
       char myconvert[100];
       FILE* bla = popen("which convert", "r");
       fgets(myconvert, 100, bla);
       pclose(bla);
       *(myconvert+16) = 0;
       */
    string myconvert = "convert";
    // if(strstr(myconvert.c_str(), "convert") == NULL){
    //   cerr << "failed to find \"convert\", please install ImageMagick" << endl;
    //   return false;
    // }
    ostringstream cmd;
    cmd << myconvert.c_str() << " \"" << fname << "\" PPM:-";
    FILE* fp = popen(cmd.str().c_str(), "r");
    if (!fp) {
      err << "conversion of image " << fname << " failed";
      throw err.str();
    }
    // convert FILE to stream
    __gnu_cxx::stdio_filebuf<char> fb(fp, ios::in) ;
    istream in(&fb) ;    
    // read pnm image
    idx<ubyte> tmp = pnm_read(in);
    pclose(fp);
    idxdim dims(tmp);
    idx<T> out;
    // allocate if not allocated
    if (!out_)
      out = idx<T>(dims);
    else
      out = *out_;
    // resize if necessary
    if (out.get_idxdim() != dims)
      out.resize(dims);
    // copy/cast
    idx_copy(tmp, out);
    return out;
  }

  ////////////////////////////////////////////////////////////////
  // I/O: loading

  template<class T>
  void load_image(const char *fname, idx<T> &out) {
    // first try if the image is a mat file
    try {
      load_matrix<T>(out, fname);
      return ;
    } catch(string &err) {
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
      return m;
    } catch(string &err) {
      ; // not a mat file, try regular image
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
    idx_bloop1(inn, in, T) {
      idx_bloop1(innn, inn, T) {
	fputc((ubyte) innn.get(0), fp);
	fputc((ubyte) innn.get(1), fp);
	fputc((ubyte) innn.get(2), fp);
      }
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
