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
void display(string &fname) {
#ifdef __GUI__
  disable_window_updates();
  clear_window();
  idx<T> mat = load_image<T>(fname);
  T min, max;
  T matmin = idx_min(mat);
  if (matmin < 0) {
    min = -1; 
    max = -1;
  }
  draw_matrix(mat, 0, 0, 1.0, 1.0, min, max);
  gui << mat;
  enable_window_updates();
#endif
}

void display(string &fname) {
#ifdef __GUI__
  disable_window_updates();
  clear_window();
  idx<ubyte> mat = load_image<ubyte>(fname);
  draw_matrix(mat);
  gui << mat;
  enable_window_updates();
#endif
}

void load_display(string &fname) {
  switch (get_matrix_type(fname.c_str())) {
  case MAGIC_BYTE_MATRIX:
  case MAGIC_UBYTE_VINCENT:
    display<ubyte>(fname);
    break ;
  case MAGIC_INTEGER_MATRIX:
  case MAGIC_INT_VINCENT:
    display<int>(fname);
    break ;
  case MAGIC_FLOAT_MATRIX:
  case MAGIC_FLOAT_VINCENT:
    display<float>(fname);
    break ;
  case MAGIC_DOUBLE_MATRIX:
  case MAGIC_DOUBLE_VINCENT:
    display<double>(fname);
    break ;
  case MAGIC_LONG_MATRIX:
    display<long>(fname);
    break ;
  case MAGIC_UINT_MATRIX:
    display<uint>(fname);
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
    // show mat images
    for (int i = 1; i < argc; ++i) {
      string fname = argv[i];
      load_display(fname);
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
	if (gui.pop_key_pressed() == Qt::Key_Space) {
	  ++i;
	  if (i == mats->end())
	    i = mats->begin();
	  load_display(*i);
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
