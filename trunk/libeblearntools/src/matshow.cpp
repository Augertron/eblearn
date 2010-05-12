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
void display(list<string>::iterator &ifname,
	     bool signd, bool load, bool show_info,
	     bool show_help, uint nh, uint nw, list<string> *mats) {
#ifdef __GUI__
  static idx<T> mat;
  uint h = 0, w = 0, rowh = 0, maxh = 0;
  list<string>::iterator fname = ifname;
  disable_window_updates();
  clear_window();
  for (uint i = 0; i < nh; ++i) {
    rowh = maxh;
    for (uint j = 0; j < nw; ++j) {
      if (fname == mats->end())
	fname = mats->begin();
      //      if (load)
      mat = load_image<T>(*fname);
      maxh = MAX(maxh, rowh + mat.dim(0));
      if (signd) {
	T min, max;
	T matmin = idx_min(mat);
	if (matmin < 0) {
	  min = -1; 
	  max = -1;
	}
	draw_matrix(mat, rowh, w, 1.0, 1.0, min, max);
      } else
	draw_matrix(mat, rowh, w);
      w += mat.dim(1) + 1;
      fname++;
      if (fname == ifname)
	break ;
    }
    if (fname == ifname)
      break ;
    maxh++;
    w = 0;
  }
  // info
  if (show_info) {
    set_text_colors(0, 0, 0, 255, 255, 255, 255, 200);
    gui << mat;
    gui << at(15, 0) << *fname;
  }
  // help
  if (show_help) {
    h = 0;
    w = 0;
    uint hstep = 14;
    set_text_colors(0, 0, 255, 255, 255, 255, 255, 200);
    gui << at(h, w) << "Controls:"; h += hstep;
    set_text_colors(0, 0, 0, 255, 255, 255, 255, 200);
    gui << at(h, w) << "Spacebar/Right: next image"; h += hstep;
    gui << at(h, w) << "Left: previous image"; h += hstep;
    gui << at(h, w) << "i: image info"; h += hstep;
    gui << at(h, w) << "x/z: show more/less images on width axis"; h += hstep;
    gui << at(h, w) << "y/t: show more/less images on height axis"; h += hstep;
    gui << at(h, w) << "h: help"; h += hstep;
  }
  enable_window_updates();
#endif
}

//! Retrieve type so that we know if we can look
//! for negative values when estimating range.
void load_display(list<string>::iterator &ifname,
		  bool load, bool show_info, bool show_help,
		  uint nh, uint nw, list<string> *mats) {
  try {
    switch (get_matrix_type((*ifname).c_str())) {
    case MAGIC_BYTE_MATRIX:
    case MAGIC_UBYTE_VINCENT:
      display<ubyte>(ifname, false, load, show_info, show_help, nh, nw, mats);
      break ;
    case MAGIC_INTEGER_MATRIX:
    case MAGIC_INT_VINCENT:
      display<int>(ifname, true, load, show_info, show_help, nh, nw, mats);
    break ;
    case MAGIC_FLOAT_MATRIX:
    case MAGIC_FLOAT_VINCENT:
      display<float>(ifname, true, load, show_info, show_help, nh, nw, mats);
      break ;
    case MAGIC_DOUBLE_MATRIX:
    case MAGIC_DOUBLE_VINCENT:
      display<double>(ifname, true, load, show_info, show_help, nh, nw, mats);
      break ;
    case MAGIC_LONG_MATRIX:
    display<long>(ifname, true, load, show_info, show_help, nh, nw, mats);
    break ;
    case MAGIC_UINT_MATRIX:
      display<uint>(ifname, false, load, show_info, show_help, nh, nw, mats);
      break ;
    default:
      eblerror("unknown magic number");
    }
  } catch(string &err) {
    cerr << err << endl;
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
    // variables
    bool show_info = false;
    bool show_help = false;
    uint nh = 1, nw = 1;
    // show mat images
    list<string> *argmats = new list<string>();
    list<string>::iterator i;
    for (int i = 1; i < argc; ++i) {
      argmats->push_front(argv[i]);
    }
    i = argmats->begin();
    load_display(i, true, show_info, show_help, nh, nw, argmats);
    // list all other mat files in image directory
    string dir = argv[1];
    string imgname, tmpname;
    size_t pos = dir.find_last_of('/');
    if (pos == string::npos) {
      imgname = dir;
      dir = "./";
    } else { // it contains a directory
      imgname = dir.substr(pos + 1, dir.size() - pos + 1);
      dir = dir.substr(0, pos);
    }
    list<string> *mats = find_fullfiles(dir, MAT_PATTERN, NULL, true, false);
    if ((mats) && (mats->size() > 1)) {
      // find current position in this list
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
	// next/previous images only if not everuything is already displayed
	if (mats->size() > nh * nw) {
	  if ((key == Qt::Key_Space) || (key == Qt::Key_Right)) {
	    // show next image
	    for (uint k = 0; k < nw * nh; ++k) {
	      i++;
	      if (i == mats->end()) {
		i = mats->begin();
	      }
	    }
	    load_display(i, true, show_info, show_help, nh, nw, mats);
	  } else if (key == Qt::Key_Left) {
	    // show previous image
	    for (uint k = 0; k < nw * nh; ++k) {
	      i--;
	      if (i == mats->begin()) {
		i = mats->end();
		i--;
	      }
	    }
	    load_display(i, true, show_info, show_help, nh, nw, mats);
	  }
	}
	if (key == Qt::Key_I) {
	  // show info
	  show_info = !show_info;
	  if (show_info)
	    show_help = false;
	  load_display(i, false, show_info, show_help, nh, nw, mats);
	} else if (key == Qt::Key_H) {
	  // show help
	  show_help = !show_help;
	  if (show_help)
	    show_info = false;
	  load_display(i, false, show_info, show_help, nh, nw, mats);
	} else if (key == Qt::Key_Y) {
	  // increase number of images shown on height axis
	  if (nh * nw < mats->size())
	    nh++;
	  load_display(i, false, show_info, show_help, nh, nw, mats);
	} else if (key == Qt::Key_T) {
	  // decrease number of images shown on height axis
	  nh = MAX(1, nh - 1);
	  load_display(i, false, show_info, show_help, nh, nw, mats);
	} else if (key == Qt::Key_X) {
	  // increase number of images shown on width axis
	  if (nh * nw < mats->size())
	    nw++;
	  load_display(i, false, show_info, show_help, nh, nw, mats);
	} else if (key == Qt::Key_Z) {
	  // decrease number of images shown on width axis
	  nw = MAX(1, nw - 1);
	  load_display(i, false, show_info, show_help, nh, nw, mats);
	}
      }
    }
    // free objects
    if (mats)
      delete mats;
    if (argmats)
      delete argmats;
  } catch(string &err) {
    cerr << err << endl;
  }
  usleep(500000); // TODO: this lets time for window to open, fix this issue
#endif
  return 0;
}
