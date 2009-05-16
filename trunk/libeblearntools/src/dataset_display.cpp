/***************************************************************************
 *   Copyright (C) 2008 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include <cstdlib>

#include "libidx.h"
#include "libeblearn.h"

#ifdef __GUI__
#include "libidxgui.h"
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl;

string dsfilename;
string labfilename;
string classesfilename;

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 4) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  dsfilename = argv[1];
  labfilename = argv[2];
  classesfilename = argv[3];
  return true;
}

// print command line usage
void print_usage() {
  cout << "Usage: ./dataset_display <dataset.mat>" << endl;
}

unsigned int draw_layer(idx<float> &layer, const char *s,
			unsigned int h, unsigned int w) {
#ifdef __GUI__
  draw_matrix(layer, s, h, w, 1.0, 1.0, (float)-1.0, (float)1.0);
#endif
  return layer.dim(1) + 3;
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) { 
#else
int main(int argc, char **argv) {
#endif
  //for (int i = 0 ; i < argc ; i++)cout<<"argv["<<i<<"]: "<<argv[i]<<endl;
  cout << "******************* Dataset display for libeblearn library ";
  cout << "*******************" << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  cout << "displaying " << dsfilename << endl;

  idx<float> images(1, 1, 1, 1);
  load_matrix(images, dsfilename.c_str());
  idx<int> labels(1);
  load_matrix(labels, labfilename.c_str());
  idx<ubyte> classes(1, 1);
  load_matrix(classes, classesfilename.c_str());
  cout << "images: " << images << endl;
  cout << "labels: " << labels << endl;
  cout << "classes: " << classes << endl;
  
  cout << "****************************************";
  cout << "***************************************" << endl;
  
#ifdef __GUI__
  new_window("Dataset display");

  // display all images
  idx_bloop2(image, images, float, label, labels, int) {
    unsigned int h = 0, w = 0;
    disable_window_updates();
    clear_window();
    gui << at(h, w) << classes[label.get()].idx_ptr();
    idx_bloop1(layer, image, float) {
      w += draw_layer(layer, "", h, w);
    }
    enable_window_updates();
    sleep(1);
  }

#endif 
  return 0;
}
