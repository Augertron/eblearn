/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include "libidx.h"
#include "tools_utils.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace std;
using namespace ebl;

////////////////////////////////////////////////////////////////
// interface

// print command line usage
void print_usage() {
  cout << "Usage: ./matshow *.mat" << endl;
}

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  // if requesting help, print usage
  if ((strcmp(argv[1], "-help") == 0) ||
      (strcmp(argv[1], "-h") == 0))
    return false;
  return true;
}

////////////////////////////////////////////////////////////////
// gui

template <typename T>
void display(string &fname, bool signd, bool load, bool show_info) {
#ifdef __GUI__
  static idx<T> mat;
  disable_window_updates();
  clear_window();
  if (load)
    mat = load_image<T>(fname);
  if (signd) {
    T min, max;
    T matmin = idx_min(mat);
    if (matmin < 0) {
      min = -1; 
      max = -1;
    }
    draw_matrix(mat, 0, 0, 1.0, 1.0, min, max);
  } else
    draw_matrix(mat);
  if (show_info) {
    gui << mat;
    gui << at(15, 0) << fname;
  }
  enable_window_updates();
#endif
}

//! Retrieve type so that we know if we can look
//! for negative values when estimating range.
void load_display(string &fname, bool load, bool show_info) {
  switch (get_matrix_type(fname.c_str())) {
  case MAGIC_BYTE_MATRIX:
  case MAGIC_UBYTE_VINCENT:
    display<ubyte>(fname, false, load, show_info);
    break ;
  case MAGIC_INTEGER_MATRIX:
  case MAGIC_INT_VINCENT:
    display<int>(fname, true, load, show_info);
    break ;
  case MAGIC_FLOAT_MATRIX:
  case MAGIC_FLOAT_VINCENT:
    display<float>(fname, true, load, show_info);
    break ;
  case MAGIC_DOUBLE_MATRIX:
  case MAGIC_DOUBLE_VINCENT:
    display<double>(fname, true, load, show_info);
    break ;
  case MAGIC_LONG_MATRIX:
    display<long>(fname, true, load, show_info);
    break ;
  case MAGIC_UINT_MATRIX:
    display<uint>(fname, false, load, show_info);
    break ;
  default:
    eblerror("unknown magic number");
  }
}

////////////////////////////////////////////////////////////////
// main

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) { 
#else
int main(int argc, char **argv) {
#endif
#ifndef __GUI__
  eblerror("QT not found, install and recompile.");
#else
  try {
    if (!parse_args(argc, argv))
      return -1;
    new_window("matshow");
    bool show_info = false;
    // show mat images
    for (int i = 1; i < argc; ++i) {
      string fname = argv[i];
      load_display(fname, true, show_info);
    }
    // list all other mat files in image directory
    string dir = argv[1];
    string imgname, tmpname;
    size_t pos = dir.find_last_of('/');
    imgname = dir.substr(pos + 1, dir.size() - pos + 1);
    dir = dir.substr(0, pos);
    list<string> *mats = find_fullfiles(dir, MAT_PATTERN, NULL, true, false);
    if ((mats) && (mats->size() > 1)) {
      // find current position in this list
      list<string>::iterator i;
      for (i = mats->begin(); i != mats->end(); ++i) {
	tmpname = i->substr(pos + 1, i->size() - pos + 1);
	if (!imgname.compare(tmpname)) {
	  break ;
	}
      }
      // loop and wait for key pressed
      while (1) {
	usleep(50000);
	int key = gui.pop_key_pressed();
	if ((key == Qt::Key_Space) || (key == Qt::Key_Right)) {
	  // show next image
	  ++i;
	  if (i == mats->end())
	    i = mats->begin();
	  load_display(*i, true, show_info);
	} else if (key == Qt::Key_Left) {
	  // show previous image
	  i--;
	  if (i == mats->begin()) {
	    i = mats->end();
	    i--;
	  }
	  load_display(*i, true, show_info);
	} else if (key == Qt::Key_I) {
	  // show info
	  show_info = !show_info;
	  load_display(*i, false, show_info);
	}
      }
      // free list
      delete mats;
    }
  } catch(string &err) {
    cerr << err << endl;
  }
  usleep(500000); // TODO: this lets time for window to open, fix this issue
#endif
  return 0;
}
