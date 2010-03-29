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
#include <vector>

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

// global variables: parameters
idxdim		dims = idxdim(4, 8);	//!< number of samples to display (hxw)
bool		info = false;	//!< only print information and quit
bool		size = false;	//!< only print size info and quit
vector<double>	range;		//!< display range of values
int             channel = -1;   //!< display only this channel if >= 0

////////////////////////////////////////////////////////////////
// interface

// print command line usage
void print_usage() {
  cout << "Usage: ./dataset_display <root/name> [OPTIONS]" << endl;
  cout << "  example: ./dataset_display /datasets/pascal" << endl;
  cout << "Options are:" << endl;
  cout << "  -info, do not display dataset, just statistics on stdout."<< endl;
  cout << "  -size, do not display dataset, just print the size and exit."
       << endl;
  cout << "  -dims <dimensions, default: 4x8>, the number of columns and ";
  cout << " rows to display." << endl;
  cout << "  -range <range, e.g.: -1,1>: range to map to 0..255 for display.";
  cout << "  -channel <channel id, e.g.: 0>: only display a specific channel.";
  cout << endl;
}

// parse command line input
bool parse_args(int argc, char **argv, string &ds_name) {
  // Read arguments from shell input
  if (argc < 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  ds_name = argv[1];
  // if requesting help, print usage
  if ((strcmp(argv[1], "-help") == 0) ||
      (strcmp(argv[1], "-h") == 0))
    return false;
  // default arguments
  range.push_back(-1.0);
  range.push_back(1.0);
  // loop over arguments
  for (int i = 2; i < argc; ++i) {
    try {
      if (strcmp(argv[i], "-info") == 0) {
	info = true;
      } else if (strcmp(argv[i], "-size") == 0) {
	size = true;
      } else if (strcmp(argv[i], "-channel") == 0) {
	++i; if (i >= argc) throw 1;
	channel = atoi(argv[i]);
      } else if (strcmp(argv[i], "-dims") == 0) {
	++i; if (i >= argc) throw 0;
	idxdim d;
	string s = argv[i];
	int k = 0;
	while (s.size()) {
	  uint j;
	  for (j = 0; j < s.size(); ++j)
	    if (s[j] == 'x')
	      break ;
	  string s0 = s.substr(0, j);
	  if (j >= s.size())
	    s = "";
	  else
	    s = s.substr(j + 1, s.size());
	  d.insert_dim(atoi(s0.c_str()), k++);
	}
	dims = d;
      } else if (strcmp(argv[i], "-range") == 0) {
	range.clear();
	++i; if (i >= argc) throw 0;
	string s = argv[i];
	int k = 0;
	while (s.size()) {
	  uint j;
	  for (j = 0; j < s.size(); ++j)
	    if (s[j] == ',')
	      break ;
	  string s0 = s.substr(0, j);
	  if (j >= s.size())
	    s = "";
	  else
	    s = s.substr(j + 1, s.size());
	  range.push_back(atof(s0.c_str()));
	  k++;
	}
      }
    } catch (int err) {
      cerr << "input error: ";
      switch (err) {
      case 0: cerr << "expecting string after " << argv[i-1]; break;
      case 1: cerr << "expecting integer after " << argv[i-1]; break;
      case 2: cerr << "unknown parameter " << argv[i-1]; break;
      case 3: cerr << "unknown channel mode " << argv[i-1]; break;
      default: cerr << "undefined error";
      }
      cerr << endl << endl;
      return false;
    }
  }
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
 
  // parse arguments
  if (!parse_args(argc, argv, ds_name)) {
    print_usage();
    return -1;
  }
  
  if (!size) {
    cout <<
      "___________________________________________________________________";
    cout << endl << endl;
    cout << "             Dataset display for libeblearn library " << endl;
    cout <<
      "___________________________________________________________________";
    cout << endl;
    // print info
    cout << "input parameters:" << endl;
    cout << "  dataset name: " << ds_name << endl;
    cout << "  info only: " << (info ? "yes" : "no") << endl;
    cout << "  size only: " << (size ? "yes" : "no") << endl;
    cout << "  values range: ";
    for (vector<double>::iterator i = range.begin(); i != range.end(); ++i)
      cout << *i << " ";
    cout << endl;
    cout << "  display dimensions: " << dims << endl;  
    cout <<
      "___________________________________________________________________";
    cout << endl;
  }

  // data
  build_fname(ds_name, DATA_NAME, data_fname);

  if (size) {
    load_matrix(data, data_fname);
    cout << data.dim(0) << endl;
    return 0;
  }
  
  loading_error(data, data_fname);
  // labels
  build_fname(ds_name, LABELS_NAME, labels_fname);
  loading_error(labels, labels_fname);
  // classes
  build_fname(ds_name, CLASSES_NAME, classes_fname);
  bclasses = loading_warning(classes, classes_fname);
  // classpairs
  build_fname(ds_name, CLASSPAIRS_NAME, classpairs_fname);
  bclasspairs = loading_warning(classpairs, classpairs_fname);
  // defpairs
  build_fname(ds_name, DEFORMPAIRS_NAME, deformpairs_fname);
  bdefpairs = loading_warning(defpairs, deformpairs_fname);

  if (channel >= 0) {
    if (channel >= data.dim(1)) {
      cerr << "trying to select channel " << channel
	   << " but channel dimension is of size " << data.dim(0)
	   << " (in " << data << ")." << endl;
      eblerror("trying to select unknown channel dimension");
    }
    data = data.select(1, channel);
  }

  labeled_datasource<t_data, t_data, int>
    train_ds(data, labels, classes, "Dataset");

#ifdef __GUI__
  if (!info) { // only display if info is false
    if (bclasspairs) {
      labeled_pair_datasource<t_data, t_data, t_label>
	train_cp_ds(data, labels, classes, classpairs,
		    "Class pairs (training)");
      labeled_pair_datasource_gui<t_data, t_data, t_label> dsgui_cp(true);
      dsgui_cp.display(train_cp_ds, dims.dim(0), dims.dim(1),
		       0, 0, 1, -1, NULL, false,
		       (t_data) range[0], (t_data) range[1]);
      sleep(1);
    }
    
    if (bdefpairs) {
      labeled_pair_datasource<t_data, t_data, t_label>
	train_dp_ds(data, labels, classes, defpairs,
		    "Deformation pairs (training)");
      labeled_pair_datasource_gui<t_data, t_data, t_label> dsgui_dp(true);
      dsgui_dp.display(train_dp_ds, dims.dim(0), dims.dim(1),
		       0, 0, 1, -1, NULL, false,
		       (t_data) range[0], (t_data) range[1]);
      sleep(1);
    }
  
    labeled_datasource_gui<t_data, t_data, t_label> dsgui(true);
    dsgui.display(train_ds, dims.dim(0), dims.dim(1), 0, 0, 1, -1, NULL, false,
		  (t_data) range[0], (t_data) range[1]);
    sleep(1);
  }
#else
  cerr << "warning: QT gui libraries not available, install them and recompile"
       << endl;
#endif
  return 0;
}
