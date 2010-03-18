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

#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace std;
using namespace ebl;

////////////////////////////////////////////////////////////////
// definitions

// the images will be displayed between range_min and range_max, which
// are initialized with those values. (0,0) meaning automatic range.
#define DEFAULT_DISPLAY_RANGE_MIN 0 // (0,0) -> automatic range
#define DEFAULT_DISPLAY_RANGE_MAX 0 // (0,0) -> automatic range

////////////////////////////////////////////////////////////////
// interface

// print command line usage
void print_usage() {
  cout << "Usage: ./matdisplay <root/name>" << endl;
  cout << "  example: ./matdisplay /datasets/pascal" << endl;
}

// parse command line input
bool parse_args(int argc, char **argv, string &ds_name) {
  // Read arguments from shell input
  if (argc < 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  ds_name = argv[1];
  return true;
}

template <typename T> void display(const string &fname, idx<T> &mat) {
#ifdef __GUI__
  new_window(fname);
  draw_matrix(mat);
  gui << mat;
  sleep(1); // TODO: this lets time for window to open, fix this issue
#endif
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) { 
#else
int main(int argc, char **argv) {
#endif
  try {
    for (int i = 1; i < argc; ++i) {
      string fname = argv[i];
      idx<float> img = load_image<float>(fname);
      display(fname, img);
    }
  } catch(string &err) {
    cerr << err << endl;
  }
  return 0;
}
