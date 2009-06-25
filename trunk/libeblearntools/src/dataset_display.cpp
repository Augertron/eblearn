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

// TODO: temporary, need cleaner solution
typedef ubyte t_data;


string ds_fname;
string lab_fname;
string classes_fname;
string classpairs_fname;
string deformpairs_fname;

bool bclasses = true;
bool bdefpairs = true;
bool bclasspairs = true;

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  ds_fname = argv[1];
  ds_fname += "images.mat";
  lab_fname = argv[1];
  lab_fname += "labels.mat";
  classes_fname = argv[1];
  classes_fname += "classes.mat";
  classpairs_fname = argv[1];
  classpairs_fname += "classpairs.mat";
  deformpairs_fname = argv[1];
  deformpairs_fname += "defpairs.mat";
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

  idx<t_data> data(1, 1, 1, 1);
  idx<int> labels(1);
  idx<ubyte> classes(1, 1);
  idx<int> classpairs(1, 1);
  idx<int> defpairs(1, 1);
  
  if (!load_matrix<t_data>(data, ds_fname.c_str())) {
    std::cerr << "Failed to load dataset file " << ds_fname << endl;
    eblerror("Failed to load dataset file");
  }
  if (!load_matrix<int>(labels, lab_fname.c_str())) {
    std::cerr << "Failed to load dataset file " << lab_fname << endl;
    eblerror("Failed to load dataset file");
  }
  if (!load_matrix<ubyte>(classes, classes_fname.c_str())) {
    std::cerr << "Failed to load dataset file " << classes_fname << endl;
    bclasses = false;
  }
  if (!load_matrix<int>(classpairs, classpairs_fname.c_str())) {
    std::cerr << "Failed to load dataset file " << classpairs_fname << endl;
    bclasspairs = false;
  }
  if (!load_matrix<int>(defpairs, deformpairs_fname.c_str())) {
    std::cerr << "Failed to load dataset file " << deformpairs_fname << endl;
    bdefpairs = false;
  }
  // TODO: move this inside datasource class
  if (!bclasses) {
    ubyte nclass = idx_max(labels) + 1;
    classes.resize(nclass, 128);
    idx_fill(classes, (ubyte) '\0');
    int i = 0;
    idx_bloop1(classe, classes, ubyte) {
      ostringstream oss;
      oss << i;
      const char *s = oss.str().c_str();
      cout << "len: " << strlen(s) << " : " << s << endl;
      memcpy(classe.idx_ptr(), s, MIN(127, strlen(s) * sizeof(ubyte)));
      ++i;
    }
  }
  cout << classes.dim(0) << " classes found:" << endl;
  idx_bloop1(classe, classes, ubyte) {
    cout << "  " << (const char *) classe.idx_ptr() << endl;
  }
  
  labeled_datasource<t_data, int> train_ds(data, labels, classes, 0, 1,
 					  "Training dataset");
  
  cout << "images: " << train_ds.data << endl;
  cout << "labels: " << train_ds.labels << endl;
  cout << "****************************************";
  cout << "***************************************" << endl;
  
#ifdef __GUI__
  if (bclasspairs) {
    labeled_pair_datasource<t_data, int> train_cp_ds(data, labels, classes,
						    classpairs,
						    0, 1,
						    "Class pairs (training)");
    labeled_pair_datasource_gui<t_data, int> dsgui_cp(true);
    dsgui_cp.display(train_cp_ds, 4, 8, 0, 0, 1, -1, NULL, false, -1.0, 1.0);
    sleep(1);
  }

  if (bdefpairs) {
    labeled_pair_datasource<t_data, int> train_dp_ds(data, labels, classes,
						    defpairs,
						    0, 1,
					       "Deformation pairs (training)");
    labeled_pair_datasource_gui<t_data, int> dsgui_dp(true);
    dsgui_dp.display(train_dp_ds, 4, 8, 0, 0, 1, -1, NULL, false, -1.0, 1.0);
    sleep(1);
  }
  
  labeled_datasource_gui<t_data, int> dsgui(true);
  dsgui.display(train_ds, 4, 8, 0, 0, 1, -1, NULL, false, -1.0, 1.0);
  sleep(1);
#endif 
  return 0;
}
