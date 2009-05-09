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

#ifdef __GUI__
#include "libidxgui.h"
#include "libeblearngui.h"
#endif

#include "libeblearntools.h"

using namespace std;
using namespace ebl;

string images_root;
int channels = 0;
bool stereo = false;
string stereo_lpattern = "_L";
string stereo_rpattern = "_R";

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  images_root = argv[1];
  for (int i = 2; i < argc; ++i) {
    if (strcmp(argv[i], "-channels") == 0) {
      i++;
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
    } else if (strcmp(argv[i], "-stereo") == 0) {
      stereo = true;
    } else if (strcmp(argv[i], "-stereo_lpattern") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -stereo_lpattern." << endl;
	return false;
      }
      stereo_lpattern = argv[++i];
    } else if (strcmp(argv[i], "-stereo_rpattern") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -stereo_rpattern." << endl;
	return false;
      }
      stereo_rpattern = argv[++i];
    }
  }
  return true;
}

// print command line usage
void print_usage() {
  cout << "Usage: ./dataset_compiler <images_root> [OPTIONS]" << endl;
  cout << "Options are:" << endl;
  cout << "  -channels <channel>" << endl;
  cout << "    channels are:" << endl;
  cout << "      - RGB" << endl;
  cout << "      - YUV" << endl;
  cout << "      - HSV" << endl;
  cout << "      - Y" << endl;
  cout << "  -stereo" << endl;
  cout << "  -stereo_lpattern <pattern>" << endl;
  cout << "  -stereo_rpattern <pattern>" << endl;
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) { 
#else
int main(int argc, char **argv) {
#endif
  cout << "********* Dataset compiler for libeblearn library *********" << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  cout << "images root: " << images_root << endl;
  cout << "channels mode: " << channels << " (";
  switch (channels) {
  case 0: cout << "RGB"; break;
  case 1: cout << "YUV"; break;
  case 2: cout << "HSV"; break;
  case 3: cout << "Y"; break;
  } cout << ")" << endl;
  cout << "stereo: " << (stereo ? "yes" : "no") << endl;
  if (stereo) {
    cout << "stereo left pattern: " << stereo_lpattern << endl;
    cout << "stereo right pattern: " << stereo_rpattern << endl;
  }
  cout << "***********************************************************" << endl;

    
  return 0;
}
