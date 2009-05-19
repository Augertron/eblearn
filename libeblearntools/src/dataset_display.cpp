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

string ds_fname;
string lab_fname;
string classes_fname;
string classpairs_fname;
string deformpairs_fname;

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 6) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  ds_fname = argv[1];
  lab_fname = argv[2];
  classes_fname = argv[3];
  classpairs_fname = argv[4];
  deformpairs_fname = argv[5];
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
  cout << "displaying " << ds_fname << endl;

  labeled_pair_datasource<float, int> train_ds(ds_fname.c_str(),
					       lab_fname.c_str(),
					       classes_fname.c_str(),
					       classpairs_fname.c_str(),
					       deformpairs_fname.c_str(),
					       0, 1,
					       "Pairs training dataset");
//   labeled_datasource<float, int> train_ds(ds_fname.c_str(),
// 					  lab_fname.c_str(),
// 					  classes_fname.c_str(), 0, 1,
// 					  "Training dataset");
  
  cout << "images: " << train_ds.data << endl;
  cout << "labels: " << train_ds.labels << endl;
  cout << "****************************************";
  cout << "***************************************" << endl;
  
#ifdef __GUI__
  labeled_pair_datasource_gui<float, int> dsgui(true);
  dsgui.display(train_ds, 4, 8, 0, 0, 1, -1, NULL, false, -1.0, 1.0);
#endif 
  return 0;
}
