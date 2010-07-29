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

#define NOCONSOLE

#define MBOX_ERROR_TITLE "MatShow error"
#ifdef __WINDOWS__
#ifdef   NOCONSOLE_	
#define  ERROR_MSG(err) MessageBox(NULL, err, MBOX_ERROR_TITLE, MB_OK)
#else
#define  ERROR_MSG(err) cerr << err << endl
#endif /* NOCONSOLE */
#else
#define ERROR_MSG(err) cerr << err << endl
#endif

#ifdef __GUI__
#include "libidxgui.h"
#include "defines_windows.h"
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
    ERROR_MSG("input error: expecting arguments.");
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
int display(list<string>::iterator &ifname,
	    bool signd, bool load, bool show_info,
	    bool show_help, uint nh, uint nw, list<string> *mats) {
  int loaded = 0;
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
      try {
	//      if (load)
	mat = load_image<T>(*fname);
	loaded++;
	maxh = (std::max)(maxh, (uint) (rowh + mat.dim(0)));
	T min = 0, max = 0;
	if (signd) {
	  T matmin = idx_min(mat);
	  if (matmin < 0) {
	    min = -1; 
	    max = -1;
	  }
	  draw_matrix(mat, rowh, w, 1.0, 1.0, min, max);
	} else
	  draw_matrix(mat, rowh, w);
	w += mat.dim(1) + 1;
      } catch(string &err) { 
	ERROR_MSG(err.c_str());
      }
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
  return loaded;
}

//! Retrieve type so that we know if we can look
//! for negative values when estimating range.
int load_display(list<string>::iterator &ifname,
		 bool load, bool show_info, bool show_help,
		 uint nh, uint nw, list<string> *mats) {
  try {
    switch (get_matrix_type((*ifname).c_str())) {
    case MAGIC_BYTE_MATRIX:
    case MAGIC_UBYTE_VINCENT:
      return display<ubyte>(ifname, false, load, show_info, show_help, nh, nw, mats);
      break ;
    case MAGIC_INTEGER_MATRIX:
    case MAGIC_INT_VINCENT:
      return display<int>(ifname, true, load, show_info, show_help, nh, nw, mats);
    break ;
    case MAGIC_FLOAT_MATRIX:
    case MAGIC_FLOAT_VINCENT:
      return display<float>(ifname, true, load, show_info, show_help, nh, nw, mats);
      break ;
    case MAGIC_DOUBLE_MATRIX:
    case MAGIC_DOUBLE_VINCENT:
      return display<double>(ifname, true, load, show_info, show_help, nh, nw, mats);
      break ;
    case MAGIC_LONG_MATRIX:
      return display<long>(ifname, true, load, show_info, show_help, nh, nw, mats);
    break ;
    case MAGIC_UINT_MATRIX:
      return display<uint>(ifname, false, load, show_info, show_help, nh, nw, mats);
      break ;
    default: // not a matrix, try as regular float image
      return display<float>(ifname, true, load, show_info, show_help, nh, nw, mats);
    }
  } catch(string &err) {
    ERROR_MSG(err.c_str());
  }
  return 0;
}

////////////////////////////////////////////////////////////////
// main

#ifdef __GUI__
#ifdef NOCONSOLE
NOCONSOLE_MAIN_QTHREAD(int, argc, char**, argv) { 
#else
MAIN_QTHREAD(int, argc, char**, argv) { 
#endif
#else
int main(int argc, char **argv) {
#endif
#ifndef __GUI__
  ERROR_MSG("QT not found, install and recompile.");
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
      cout << argv[i] << endl;
      argmats->push_front(argv[i]);
    }
    i = argmats->begin();
    if (!load_display(i, true, show_info, show_help, nh, nw, argmats)) {
      ERROR_MSG("failed to load image(s)");
      return -1;
    }

#ifdef __BOOST__
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
    list<string> *mats = find_fullfiles(dir, IMAGE_PATTERN_MAT,
					NULL, true, false);
    if ((mats) && (mats->size() > 1)) {
      // find current position in this list
      for (i = mats->begin(); i != mats->end(); ++i) {
	tmpname = i->substr(pos + 1, i->size() - pos + 1);
	if (!imgname.compare(tmpname)) {
	  break ;
	}
      }
      if (i == mats->end())
	i = mats->begin();
      // loop and wait for key pressed
      while (1) {
	millisleep(50);
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
	  nh = (std::max)((uint) 1, nh - 1);
	  load_display(i, false, show_info, show_help, nh, nw, mats);
	} else if (key == Qt::Key_X) {
	  // increase number of images shown on width axis
	  if (nh * nw < mats->size())
	    nw++;
	  load_display(i, false, show_info, show_help, nh, nw, mats);
	} else if (key == Qt::Key_Z) {
	  // decrease number of images shown on width axis
	  nw = (std::max)((uint) 1, nw - 1);
	  load_display(i, false, show_info, show_help, nh, nw, mats);
	}
      }
    }
    // free objects
    if (mats)
      delete mats;
#endif /* __BOOST__ */
    if (argmats)
      delete argmats;
  } catch(string &err) {
    ERROR_MSG(err.c_str());
  }
  millisleep(500); // TODO: this lets time for window to open, fix this issue
#endif
  return 0;
}
