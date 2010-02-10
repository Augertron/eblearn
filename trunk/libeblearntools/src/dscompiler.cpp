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
#include "pascalfull_dataset.h"
#include "lush_dataset.h"

using namespace std;
using namespace ebl;

////////////////////////////////////////////////////////////////
// global variables

string		images_root	 = ".";
string		image_pattern	 = IMAGE_PATTERN;
string		channels_mode	 = "RGB";
bool            preprocessing    = true;
bool		display		 = false;
bool		stereo		 = false;
bool		ignore_difficult = false;
bool		ignore_truncated = false;
bool		ignore_occluded = false;
bool		shuffle		 = false;
bool		scale_mode	 = false;
vector<double>  scales;
string		stereo_lpattern	 = "_L";
string		stereo_rpattern	 = "_R";
string		outdir		 = ".";
string		dataset_name	 = "ds";
intg		maxperclass	 = 0;	// 0 means no limitation
intg            maxdata          = 0;	// 0 means no limitation
unsigned int	mexican_hat_size = 0;
uint		kernelsz	 = 9; // kernel size for preprocessing
int		deformations	 = -1;	// <= means no deformations
string		type		 = "regular";
string          resize           = "mean";
string		precision	 = "float";
uint		sleep_delay	 = 0;	// sleep between frames displayed in ms
idxdim          outdims;	        // dimensions of output sample
bool		outdims_set	 = false;
idxdim          mindims;	        // minimum dimensions in input
bool		mindims_set	 = false;
vector<string>  exclude;
vector<string>  include;
bool            usepose          = false; // use pose if given
bool            useparts         = false; // use parts if given
bool            partsonly        = false; // use parts only if given
string          save             = DATASET_SAVE;// save into lush dataset format
bool            save_set         = false;
float           bboxfact         = 1.0;
bool            bboxfact_set     = false;

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
      } else if (strcmp(argv[i], "-nopp") == 0) {
	preprocessing = true;
      } else if (strcmp(argv[i], "-ignore_difficult") == 0) {
	ignore_difficult = true;
      } else if (strcmp(argv[i], "-ignore_truncated") == 0) {
	ignore_truncated = true;
      } else if (strcmp(argv[i], "-ignore_occluded") == 0) {
	ignore_occluded = true;
      } else if (strcmp(argv[i], "-shuffle") == 0) {
	shuffle = true;
      } else if (strcmp(argv[i], "-usepose") == 0) {
	usepose = true;
      } else if (strcmp(argv[i], "-useparts") == 0) {
	useparts = true;
      } else if (strcmp(argv[i], "-partsonly") == 0) {
	partsonly = true;
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
      } else if (strcmp(argv[i], "-save") == 0) {
	++i; if (i >= argc) throw 0;
	save = argv[i];
	save_set = true;
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
      } else if (strcmp(argv[i], "-exclude") == 0) {
	++i; if (i >= argc) throw 0;
	exclude.push_back(argv[i]);
      } else if (strcmp(argv[i], "-include") == 0) {
	++i; if (i >= argc) throw 0;
	include.push_back(argv[i]);
      } else if (strcmp(argv[i], "-maxperclass") == 0) {
	++i; if (i >= argc) throw 1;
	maxperclass = atoi(argv[i]);
      } else if (strcmp(argv[i], "-maxdata") == 0) {
	++i; if (i >= argc) throw 1;
	maxdata = atoi(argv[i]);
      } else if (strcmp(argv[i], "-kernelsz") == 0) {
	++i; if (i >= argc) throw 1;
	kernelsz = atoi(argv[i]);
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
      } else if (strcmp(argv[i], "-mindims") == 0) {
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
	mindims = d;
	mindims_set = true;
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
	  scales.push_back(atof(s0.c_str()));
	  k++;
	}
	scale_mode = true;
	preprocessing = true;
      } else if (strcmp(argv[i], "-bboxfact") == 0) {
	++i; if (i >= argc) throw 0;
	bboxfact = (float) atof(argv[i]);
	bboxfact_set = true;
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
  cout << "  -type <regular(default)|pascal|pascalbg|pascalfull|lush>" << endl;
  cout << "     regular: compile images labeled by their top folder name"<<endl;
  cout << "     pascal: compile images labeled by xml files (PASCAL challenge)";
  cout << endl;
  cout << "     pascalbg: compile background images of PASCAL challenge"<< endl;
  cout << "     pascalfull: copy full original PASCAL images into outdir"<<endl;
  cout << "       (allows to exclude some classes, then call regular compiler)";
  cout << endl;
  cout << "     lush: regular compilation using .mat images" << endl;
  cout << "  -precision <float(default)|double>" << endl;
  cout << "  -image_pattern <pattern>" << endl;
  cout << "     default: " << IMAGE_PATTERN << endl;
  cout << "  -channels <channel>" << endl;
  cout << "     channels are: RGB (default), YpUV, HSV, Yp (Yp only in YpUV)";
  cout << endl;
  cout << "  -disp" << endl;
  cout << "  -nopp (no preprocessing, i.e. no resizing or conversion)" << endl;
  cout << "  -sleep <delay in ms> (sleep between frame display)" << endl;
  cout << "  -shuffle" << endl;
  cout << "  -usepose (separate classes with pose if available)" << endl;
  cout << "  -stereo" << endl;
  cout << "  -stereo_lpattern <pattern>" << endl;
  cout << "  -stereo_rpattern <pattern>" << endl;
  cout << "  -outdir <directory (default=images_root)>" << endl;
  cout << "  -save <dataset(default)|mat|ppm|png|jpg|...>" << endl;
  cout << "  -dname <name>" << endl;
  cout << "  -maxperclass <integer>" << endl;
  cout << "  -maxdata <integer>" << endl;
  cout << "  -kernelsz <integer>" << endl;
  cout << "  -mexican_hat_size <integer>" << endl;
  cout << "  -deformations <integer>" << endl;
  cout << "  -dims <dimensions (default: 96x96x3)>" << endl;
  cout << "  -mindims <dimensions (default: 1x1)>" << endl;
  cout << "     (exclude inputs for which one dimension is less than specified";
  cout << endl;
  cout << "  -scales <scales (e.g: 1.5,2,4)>" << endl;
  cout << "  -bboxfact <float factor> (multiply bounding boxes by a factor)";
  cout << endl;
  cout << "  -resize <mean(default)|gaussian|bilinear" << endl; 
  cout << "  -exclude <class name> (include all but excluded classes," << endl;
  cout << "                         exclude can be called multiple times)";
  cout << endl;
  cout << "  -include <class name> (exclude all but included classes," << endl;
  cout << "                         include can be called multiple times)";
  cout << endl;
  cout << "  -useparts  (also extract object parts, ";
  cout << "e.g. person->(head,hand,foot)" << endl;
  cout << "  -partsonly  (only extract object parts, ";
  cout << "e.g. person->(head,hand,foot)" << endl;
  cout << "  -ignore_difficult (ignore sample if \"difficult\" flag is on)";
  cout << endl;
  cout << "  -ignore_truncated (ignore sample if \"truncated\" flag is on)";
  cout << endl;
  cout << "  -ignore_occluded (ignore sample if \"occluded\" flag is on)";
  cout << endl;
}

////////////////////////////////////////////////////////////////
// compilation

template <class Tds>
void compile_ds(Tds &ds, bool imgpat = true) {
  if (bboxfact_set) ds.set_bboxfact(bboxfact);
  if (usepose) ds.use_pose();
  if (useparts) ds.use_parts();
  if (partsonly) ds.use_parts_only();
  ds.set_exclude(exclude);
  ds.set_include(include);
  if (outdims_set) ds.set_outdims(outdims);
  if (mindims_set) ds.set_mindims(mindims);
  ds.set_display(display);
  ds.set_sleepdisplay(sleep_delay);
  ds.set_resize(resize);
  if (save_set) ds.set_save(save);
  if (preprocessing) ds.set_pp_conversion(channels_mode.c_str(), kernelsz);
  if (maxperclass > 0) ds.set_max_per_class(maxperclass);
  if (maxdata > 0) ds.set_max_data(maxdata);
  if (imgpat) ds.set_image_pattern(image_pattern);
  if (scale_mode) ds.set_scales(scales, outdir);
  else ds.alloc();
  ds.extract();
  if (shuffle) ds.shuffle();
  ds.save(outdir);
}

template <class Tdata>
void compile() {
  if (!strcmp(type.c_str(), "pascal")) {
    pascal_dataset<Tdata> ds(dataset_name.c_str(), images_root.c_str(),
			     ignore_difficult, ignore_truncated,
			     ignore_occluded);
    compile_ds(ds);
  }
  else if (!strcmp(type.c_str(), "pascalbg")) {
    pascalbg_dataset<Tdata> ds(dataset_name.c_str(), images_root.c_str(),
			       outdir.c_str(), maxperclass, ignore_difficult,
			       ignore_truncated, ignore_occluded); 
    if (useparts) ds.use_parts();
    if (partsonly) ds.use_parts_only();
    ds.set_exclude(exclude);
    ds.set_include(include);
    ds.set_resize(resize);
    if (outdims_set) ds.set_outdims(outdims);
    if (mindims_set) ds.set_mindims(mindims);
    ds.set_display(display);
    ds.set_sleepdisplay(sleep_delay);
    ds.set_image_pattern(image_pattern);
    if (maxdata > 0) ds.set_max_data(maxdata);
    if (scale_mode) ds.set_scales(scales, outdir);
    if (preprocessing) ds.set_pp_conversion(channels_mode.c_str(), kernelsz);
    if (save_set) ds.set_save(save);
    ds.extract();
  }
  else if (!strcmp(type.c_str(), "pascalfull")) {
    pascalfull_dataset<Tdata> ds(dataset_name.c_str(), images_root.c_str(),
				 outdir.c_str());
    ds.set_exclude(exclude);
    ds.set_include(include);
    ds.set_display(display);
    ds.set_resize(resize);
    ds.set_sleepdisplay(sleep_delay);
    if (maxdata > 0)
      ds.set_max_data(maxdata);
    ds.extract();
  }
  else if (!strcmp(type.c_str(), "lush")) {
    lush_dataset<Tdata> ds(dataset_name.c_str(), images_root.c_str());
    compile_ds(ds, false);
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
  // print info
  cout << "input parameters:" << endl;
  cout << "  dataset name: " << dataset_name << endl;
  cout << "  dataset type: " << type << endl;
  cout << "  dataset precision: " << precision << endl;
  cout << "  images root directory: " << images_root << endl;
  cout << "  output directory: " << outdir << endl;
  cout << "  outputs: " << outdir << "/" << dataset_name << "_*.mat" << endl;
  cout << "  images pattern: " << image_pattern << endl;
  cout << "  channels mode: " << channels_mode.c_str() << endl;
  cout << "  oreprocessing: " << (preprocessing ? "yes" : "no") << endl;
  cout << "  display: " << (display ? "yes" : "no") << endl;
  cout << "  display sleep: " << sleep_delay << " ms." << endl;
  cout << "  shuffling: " << (shuffle ? "yes" : "no") << endl;
  cout << "  usepose: " << (usepose ? "yes" : "no") << endl;
  cout << "  useparts: " << (useparts ? "yes" : "no") << endl;
  cout << "  partsonly: " << (partsonly ? "yes" : "no") << endl;
  cout << "  stereo: " << (stereo ? "yes" : "no") << endl;
  if (stereo) {
    cout << "    stereo left pattern: " << stereo_lpattern << endl;
    cout << "    stereo right pattern: " << stereo_rpattern << endl;
  }
  cout << "  max per class limitation: ";
  if (maxperclass > 0) cout << maxperclass; else cout << "none"; cout << endl;
  cout << "  max data limitation: ";
  if (maxdata > 0) cout << maxdata; else cout << "none"; cout << endl;
  cout << "  mexican_hat_size: " << mexican_hat_size << endl;
  cout << "  preprocessing kernel size: " << kernelsz << endl;
  cout << "  deformations: " << deformations << endl;
  cout << "  resizing method: " << resize << endl;
  cout << "  output dimensions: " << outdims << endl;
  cout << "  minimum input dimensions: " << mindims << endl;
  cout << "  scales: ";
  if (!scale_mode) cout << "none";
  else for (vector<double>::iterator i = scales.begin(); i != scales.end(); ++i)
      cout << *i << " ";
  cout << endl;
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
