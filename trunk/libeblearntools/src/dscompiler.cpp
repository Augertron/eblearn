/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
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

#ifdef __GUI__
#include "libidxgui.h"
#endif

#include "dataset.h"
#include "pascal_dataset.h"
#include "pascalbg_dataset.h"
#include "lush_dataset.h"

using namespace std;
using namespace ebl;

////////////////////////////////////////////////////////////////
// global variables

string		images_root	 = ".";
string		image_pattern	 = IMAGE_PATTERN;
string		channels_mode	 = "RGB";
bool            preprocessing    = false;
bool		display		 = false;
bool		stereo		 = false;
bool		ignore_difficult = false;
bool		shuffle		 = false;
bool		scale_mode	 = false;
vector<uint>    scales;
string		stereo_lpattern	 = "_L";
string		stereo_rpattern	 = "_R";
string		outdir		 = ".";
string		dataset_name	 = "ds";
intg		maxperclass	 = 0;	// 0 means no limitation
unsigned int	mexican_hat_size = 0;
int		deformations	 = -1;	// <= means no deformations
string		type		 = "regular";
string          resize           = "gaussian";
string		precision	 = "float";
uint		sleep_delay	 = 0;	// sleep between frames displayed in ms
idxdim          outdims;	// dimensions of output sample
bool		outdims_set	 = false;

////////////////////////////////////////////////////////////////
// command line

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  images_root = argv[1];
  outdir = images_root;
  // if requesting help, print usage
  if ((strcmp(argv[1], "-help") == 0) ||
      (strcmp(argv[1], "-h") == 0))
    return false;
  // loop over arguments
  for (int i = 2; i < argc; ++i) {
    try {
      if (strcmp(argv[i], "-channels") == 0) {
	++i; if (i >= argc) throw 0;
	if ((strcmp(argv[i], "RGB") == 0) ||
	    (strcmp(argv[i], "YpUV") == 0) ||
	    (strcmp(argv[i], "HSV") == 0) ||
	    (strcmp(argv[i], "Yp") == 0))
	  ;
	else throw 3;
	channels_mode = argv[i];
	preprocessing = true;
      } else if (strcmp(argv[i], "-image_pattern") == 0) {
	++i; if (i >= argc) throw 0;
	image_pattern = argv[i];
      } else if (strcmp(argv[i], "-disp") == 0) {
	display = true;
      } else if (strcmp(argv[i], "-ignore_difficult") == 0) {
	ignore_difficult = true;
      } else if (strcmp(argv[i], "-shuffle") == 0) {
	shuffle = true;
      } else if (strcmp(argv[i], "-stereo") == 0) {
	stereo = true;
      } else if (strcmp(argv[i], "-stereo_lpattern") == 0) {
	++i; if (i >= argc) throw 0;
	stereo_lpattern = argv[i];
      } else if (strcmp(argv[i], "-stereo_rpattern") == 0) {
	++i; if (i >= argc) throw 0;
	stereo_rpattern = argv[i];
      } else if (strcmp(argv[i], "-outdir") == 0) {
	++i; if (i >= argc) throw 0;
	outdir = argv[i];
      } else if (strcmp(argv[i], "-type") == 0) {
	++i; if (i >= argc) throw 0;
	type = argv[i];
      } else if (strcmp(argv[i], "-precision") == 0) {
	++i; if (i >= argc) throw 0;
	precision = argv[i];
      } else if (strcmp(argv[i], "-resize") == 0) {
	++i; if (i >= argc) throw 0;
	resize = argv[i];
      } else if (strcmp(argv[i], "-dname") == 0) {
	++i; if (i >= argc) throw 0;
	dataset_name = argv[i];
      } else if (strcmp(argv[i], "-maxperclass") == 0) {
	++i; if (i >= argc) throw 1;
	maxperclass = atoi(argv[i]);
      } else if (strcmp(argv[i], "-sleep") == 0) {
	++i; if (i >= argc) throw 1;
	sleep_delay = atoi(argv[i]);
      } else if (strcmp(argv[i], "-mexican_hat_size") == 0) {
	++i; if (i >= argc) throw 1;
	mexican_hat_size = atoi(argv[i]);
      } else if (strcmp(argv[i], "-deformations") == 0) {
	++i; if (i >= argc) throw 1;
	deformations = atoi(argv[i]);
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
	outdims = d;
	outdims_set = true;
	preprocessing = true;
      } else if (strcmp(argv[i], "-scales") == 0) {
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
	  scales.push_back(atoi(s0.c_str()));
	  k++;
	}
	scale_mode = true;
	preprocessing = true;
      } else if ((strcmp(argv[i], "-help") == 0) ||
		 (strcmp(argv[i], "-h") == 0)) {
	return false;
      } else {
	cerr << "input error: unknown parameter: " << argv[i] << endl;
	return false;
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

// print command line usage
void print_usage() {
  cout << "Usage: ./dscompiler <images_root> [OPTIONS]" << endl;
  cout << "Options are:" << endl;
  cout << "  -type <regular(default)|pascal|pascalbg|lush>" << endl;
  cout << "  -precision <float(default)|double>" << endl;
  cout << "  -image_pattern <pattern>" << endl;
  cout << "   default: " << IMAGE_PATTERN << endl;
  cout << "  -channels <channel>" << endl;
  cout << "     channels are: RGB (default), YpUV, HSV, Yp (Yp only in YpUV)";
  cout << endl;
  cout << "  -disp" << endl;
  cout << "  -sleep <delay in ms> (sleep between frame display)" << endl;
  cout << "  -shuffle" << endl;
  cout << "  -stereo" << endl;
  cout << "  -stereo_lpattern <pattern>" << endl;
  cout << "  -stereo_rpattern <pattern>" << endl;
  cout << "  -outdir <directory (default=images_root)>" << endl;
  cout << "  -dname <name>" << endl;
  cout << "  -maxperclass <integer>" << endl;
  cout << "  -mexican_hat_size <integer>" << endl;
  cout << "  -deformations <integer>" << endl;
  cout << "  -dims <dimensions (default: 96x96x3)>" << endl;
  cout << "  -scales <scales (e.g: 1,2,4)>" << endl;
  cout << "  -resize <gaussian(default)|bilinear" << endl;
}

////////////////////////////////////////////////////////////////
// compilation

template <class Tds>
void compile_ds(Tds &ds) {
  if (outdims_set)
    ds.set_outdims(outdims);
  ds.set_display(display);
  ds.set_sleepdisplay(sleep_delay);
  ds.set_resize(resize);
  if (preprocessing)
    ds.set_pp_conversion(channels_mode.c_str());
  ds.set_max_per_class(maxperclass);
  if (scale_mode)
    ds.set_scales(scales, outdir);
  else 
    ds.alloc();
  ds.extract();
  if (shuffle)
    ds.shuffle();
  ds.save(outdir);
}

template <class Tdata>
void compile() {
  if (!strcmp(type.c_str(), "pascal")) {
    pascal_dataset<Tdata> ds(dataset_name.c_str(),
			     images_root.c_str(), ignore_difficult);
    compile_ds(ds);
  }
  else if (!strcmp(type.c_str(), "pascalbg")) {
    pascalbg_dataset<Tdata> ds(dataset_name.c_str(), images_root.c_str(),
			       outdir.c_str(), maxperclass, ignore_difficult);
    if (outdims_set)
      ds.set_outdims(outdims);
    ds.set_display(display);
    ds.set_sleepdisplay(sleep_delay);
    if (preprocessing)
      ds.set_pp_conversion(channels_mode.c_str());
    ds.extract();
  }
  else if (!strcmp(type.c_str(), "lush")) {
    lush_dataset<Tdata> ds(dataset_name.c_str(), images_root.c_str());
    compile_ds(ds);
  }
  else if (!strcmp(type.c_str(), "regular")) {
    dataset<Tdata> ds(dataset_name.c_str(), images_root.c_str());
    compile_ds(ds);
  }
  else {
    cerr << "unknown dataset type: " << type << endl;
    eblerror("unknown dataset type");
  }
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) { 
#else
int main(int argc, char **argv) {
#endif
  cout << "___________________________________________________________________";
  cout << endl << endl;
  cout << "             Dataset compiler for libeblearn library " << endl;
  cout << "___________________________________________________________________";
  cout << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  cout << "input parameters:" << endl;
  cout << "  dataset name: " << dataset_name << endl;
  cout << "  dataset type: " << type << endl;
  cout << "  dataset precision: " << precision << endl;
  cout << "  images root directory: " << images_root << endl;
  cout << "  output directory: " << outdir << endl;
  cout << "  outputs: " << outdir << "/" << dataset_name << "_*.mat" << endl;
  cout << "  images pattern: " << image_pattern << endl;
  cout << "  channels mode: " << channels_mode.c_str() << endl;
  cout << "  display: " << (display ? "yes" : "no") << endl;
  cout << "  display sleep: " << sleep_delay << " ms." << endl;
  cout << "  shuffling: " << (shuffle ? "yes" : "no") << endl;
  cout << "  stereo: " << (stereo ? "yes" : "no") << endl;
  if (stereo) {
    cout << "    stereo left pattern: " << stereo_lpattern << endl;
    cout << "    stereo right pattern: " << stereo_rpattern << endl;
  }
  cout << "  max per class limitation: ";
  if (maxperclass > 0) cout << maxperclass; else cout << "none"; cout << endl;
  cout << "  mexican_hat_size: " << mexican_hat_size << endl;
  cout << "  deformations: " << deformations << endl;
  cout << "  resizing method: " << resize << endl;
  cout << "___________________________________________________________________";
  cout << endl;

  // compile with specificed precision
  if (!strcmp(precision.c_str(), "float"))
    compile<float>();
  else if (!strcmp(precision.c_str(), "double"))
    compile<double>();
  else {
    cerr << "error: trying to compile dataset with precision \"" << precision;
    cerr << "\"" << endl;
    eblerror("unsupported precision for dataset compilation");
  }
  return 0;
}
