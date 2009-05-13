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

#ifdef __GUI__
#include "libidxgui.h"
#include "libeblearngui.h"
#endif

#include "libeblearntools.h"

using namespace std;
using namespace ebl;

string images_root = ".";
string image_pattern = ".*[.]ppm";
int channels = 0;
bool display = false;
bool training = false;
bool testing = false;
bool stereo = false;
string stereo_lpattern = "_L";
string stereo_rpattern = "_R";
string outdir = ".";
string dataset_name = "dataset";
int max_per_class = -1; // -1 means no limitation

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
    if (strcmp(argv[i], "-channels") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -channels." << endl;
	return false;
      }
      if (strcmp(argv[i], "RGB") == 0)
	channels = 0;
      else if (strcmp(argv[i], "YUV") == 0)
	channels = 1;
      else if (strcmp(argv[i], "HSV") == 0)
	channels = 2;
      else if (strcmp(argv[i], "Y") == 0)
	channels = 3;
      else if (strcmp(argv[i], "YH3") == 0)
	channels = 4;
      else {
	cerr << "input error: unknown channel mode: " << argv[i] << endl;
	return false;
      }
    } else if (strcmp(argv[i], "-image_pattern") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -image_pattern." << endl;
	return false;
      }
      image_pattern = argv[i];
    } else if (strcmp(argv[i], "-dset_display") == 0) {
      display = true;
    } else if (strcmp(argv[i], "-training") == 0) {
      training = true;
    } else if (strcmp(argv[i], "-testing") == 0) {
      testing = true;
    } else if (strcmp(argv[i], "-stereo") == 0) {
      stereo = true;
    } else if (strcmp(argv[i], "-stereo_lpattern") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -stereo_lpattern." << endl;
	return false;
      }
      stereo_lpattern = argv[i];
    } else if (strcmp(argv[i], "-stereo_rpattern") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -stereo_rpattern." << endl;
	return false;
      }
      stereo_rpattern = argv[i];
    } else if (strcmp(argv[i], "-output_dir") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -output_dir." << endl;
	return false;
      }
      outdir = argv[i];
    } else if (strcmp(argv[i], "-dset_name") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -dname." << endl;
	return false;
      }
      dataset_name = argv[i];
    } else if (strcmp(argv[i], "-max_per_class") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -max_per_class." << endl;
	return false;
      }
      max_per_class = atoi(argv[i]);
    } else if ((strcmp(argv[i], "-help") == 0) ||
	       (strcmp(argv[i], "-h") == 0)) {
      return false;
    } else {
      cerr << "input error: unknown parameter: " << argv[i] << endl;
      return false;
    }
  }
  return true;
}

// print command line usage
void print_usage() {
  cout << "Usage: ./dataset_compiler <images_root> [OPTIONS]" << endl;
  cout << "Options are:" << endl;
  cout << "  -image_pattern <pattern>" << endl;
  cout << "   e.g.: \".*[.]ppm\"" << endl;
  cout << "  -channels <channel>" << endl;
  cout << "    channels are:" << endl;
  cout << "      - RGB" << endl;
  cout << "      - YUV" << endl;
  cout << "      - HSV" << endl;
  cout << "      - Y (Y only in YUV)" << endl;
  cout << "      - YH3" << endl;
  cout << "  -dset_display" << endl;
  cout << "  -training" << endl;
  cout << "  -testing" << endl;
  cout << "  -stereo" << endl;
  cout << "  -stereo_lpattern <pattern>" << endl;
  cout << "  -stereo_rpattern <pattern>" << endl;
  cout << "  -output_dir <directory>" << endl;
  cout << "  -dset_name <dataset_name>" << endl;
  cout << "  -max_per_class <integer>" << endl;
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) { 
#else
int main(int argc, char **argv) {
#endif
  //for (int i = 0 ; i < argc ; i++)cout<<"argv["<<i<<"]: "<<argv[i]<<endl;
  cout << "******************* Dataset compiler for libeblearn library ";
  cout << "*******************" << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  cout << "input parameters:" << endl;
  cout << "  dataset name: " << dataset_name << endl;
  cout << "  images root directory: " << images_root << endl;
  cout << "  output directory: " << outdir << endl;
  cout << "  images pattern: " << image_pattern << endl;
  cout << "  channels mode: " << channels << " (";
  switch (channels) {
  case 0: cout << "RGB"; break;
  case 1: cout << "YUV"; break;
  case 2: cout << "HSV"; break;
  case 3: cout << "Y"; break;
  case 4: cout << "YH3"; break;
  default: cerr << "input error: unknown channel mode." << endl;
    print_usage(); return -1;
  } cout << ")" << endl;
  cout << "  display: " << (display ? "yes" : "no") << endl;
  cout << "  training: " << (training ? "yes" : "no") << endl;
  cout << "  testing: " << (testing ? "yes" : "no") << endl;
  cout << "  stereo: " << (stereo ? "yes" : "no") << endl;
  if (stereo) {
    cout << "    stereo left pattern: " << stereo_lpattern << endl;
    cout << "    stereo right pattern: " << stereo_rpattern << endl;
  }
  cout << "  max per class: ";
  if (max_per_class == -1) cout << "no limit" << endl;
  else cout<<max_per_class << endl;
  cout << "outputs:" << endl;
  cout << "  " << outdir << "/" << dataset_name << "_images.mat" << endl;
  cout << "  " << outdir << "/" << dataset_name << "_labels.mat" << endl;
  cout << "  " << outdir << "/" << dataset_name << "_classes.mat" << endl;
  cout << "****************************************";
  cout << "***************************************" << endl;

  imagedir_to_idx(images_root.c_str(), 143, channels, image_pattern.c_str(),
		  NULL,
		  outdir.c_str(), NULL, false, display,
		  training? "_train" : "_test",
		  dataset_name.c_str(), max_per_class);
  return 0;
}
