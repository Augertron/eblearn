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

#include "dataset.h" // different than datasource, for data compilation only

using namespace std;
using namespace ebl;

// TODO: temporary, need cleaner solution
typedef float t_data;

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
  cout << "Usage: ./dataset_display <root/name>" << endl;
  cout << "  example: ./dataset_display /datasets/pascal" << endl;
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

  // names of dataset and files to load
  string	ds_name;
  string	data_fname;
  string	labels_fname;
  string	classes_fname;
  string	classpairs_fname;
  string	deformpairs_fname;
  // boolean successes of files loading
  bool		bclasses    = true;
  bool		bdefpairs   = true;
  bool		bclasspairs = true;
  // data files
  idx<t_data>	data(1, 1, 1, 1);
  idx<t_label>	labels(1);
  idx<ubyte>	classes(1, 1);
  idx<int>	classpairs(1, 1);
  idx<int>	defpairs(1, 1);
  // range of values to use for displaying images (automatic by default)
  t_data range_min = DEFAULT_DISPLAY_RANGE_MIN;
  t_data range_max = DEFAULT_DISPLAY_RANGE_MAX;

  //for (int i = 0 ; i < argc ; i++)cout<<"argv["<<i<<"]: "<<argv[i]<<endl;
  cout << "******************* Dataset display for libeblearn library ";
  cout << "*******************" << endl;
  // parse arguments
  if (!parse_args(argc, argv, ds_name)) {
    print_usage();
    return -1;
  }
  cout << "displaying " << ds_name << endl;

  // data
  build_fname(ds_name, DATA_NAME, data_fname);
  loading_error(load_matrix(data, data_fname), data_fname);
  // labels
  build_fname(ds_name, LABELS_NAME, labels_fname);
  loading_error(load_matrix(labels, labels_fname), labels_fname);
  // classes
  build_fname(ds_name, CLASSES_NAME, classes_fname);
  if (!loading_warning(load_matrix(classes, classes_fname), classes_fname))
    bclasses = false;
  // classpairs
  build_fname(ds_name, CLASSPAIRS_NAME, classpairs_fname);
  if (!loading_warning(load_matrix(classpairs, classpairs_fname),
		       classpairs_fname))
    bclasspairs = false;
  // defpairs
  build_fname(ds_name, DEFORMPAIRS_NAME, deformpairs_fname);
  if (!loading_warning(load_matrix(defpairs, deformpairs_fname),
		       deformpairs_fname))
    bdefpairs = false;
  
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
    labeled_pair_datasource<t_data, t_label> train_cp_ds(data, labels, classes,
						    classpairs,
						    0, 1,
						    "Class pairs (training)");
    labeled_pair_datasource_gui<t_data, t_label> dsgui_cp(true);
    dsgui_cp.display(train_cp_ds, 4, 8, 0, 0, 1, -1, NULL, false,
		     range_min, range_max);
    sleep(1);
  }

  if (bdefpairs) {
    labeled_pair_datasource<t_data, t_label> train_dp_ds(data, labels, classes,
						    defpairs,
						    0, 1,
					       "Deformation pairs (training)");
    labeled_pair_datasource_gui<t_data, t_label> dsgui_dp(true);
    dsgui_dp.display(train_dp_ds, 4, 8, 0, 0, 1, -1, NULL, false,
		     range_min, range_max);
    sleep(1);
  }
  
  labeled_datasource_gui<t_data, t_label> dsgui(true);
  dsgui.display(train_ds, 4, 8, 0, 0, 1, -1, NULL, false, range_min, range_max);
  sleep(1);
#endif 
  return 0;
}
