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
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <map>
#include <string.h>

#include "metajobs.h"

using namespace std;
using namespace ebl;

#ifdef __DEBUGMEM__
#include "libidx.h"
  INIT_DEBUGMEM()
#endif

////////////////////////////////////////////////////////////////
// global variables

string copy_path = ""; // path to copy to target folder
string manual_tstamp = ""; // tstamp string prefix to the experiment name

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc < 2) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  // if requesting help, print usage
  if ((strcmp(argv[1], "-help") == 0) ||
      (strcmp(argv[1], "-h") == 0))
    return false;
  // loop over arguments
  for (int i = 2; i < argc; ++i) {
    if (strcmp(argv[i], "-copy") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -copy." << endl;
	return false;
      }
      copy_path = argv[i];
    } else if (strcmp(argv[i], "-tstamp") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -tstamp." << endl;
	return false;
      }
      manual_tstamp = argv[i];
    } else {
      cerr << "input error: unknown parameter: " << argv[i] << endl;
      return false;
    }
  }
  return true;
}

// print command line usage
void print_usage() {
  cout << "Usage: ./meta_trainer <logs_root> [OPTIONS]" << endl;
  cout << "Options are:" << endl;
  cout << "  -copy <path>" << endl;
  cout << "      Copy the specified path recursively to each job's directory."
       << endl;
  cout << "  -tstamp <timestamp>" << endl
       << "      Manually set the timestamp prefix for the experiment's name."
       << endl;
}

int main(int argc, char **argv) {
  cout << "________________________________Meta Run";
  cout << "________________________________" << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }

  string metaconf = argv[1];
  job_manager jm;
  jm.read_metaconf(metaconf.c_str(), &manual_tstamp);
  jm.set_copy(copy_path);
  jm.run();
  return 0;
}
