/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
 *   All rights reserved.
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
#endif

#include <sstream>
#include <iomanip>
#include "ebl_utils.h"

using namespace std;
using namespace ebl;

uint insize = 0; // number of inputs
uint outsize = 0; // number of outputs
uint fanin = 0; // fanin
bool full = false;
bool brandom = false; // random table
bool one2one = false; // 1 to 1 table

// parse command line input
bool parse_args(int argc, char **argv) {
  // Read arguments from shell input
  if (argc <= 1) {
    cerr << "input error: expecting arguments." << endl;
    return false;
  }
  // loop over arguments
  for (int i = 1; i < argc; ++i) {
    try {
      if (strcmp(argv[i], "-full") == 0) {
	++i; if (i >= argc) throw 1;
	insize = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	outsize = atoi(argv[i]);
	full = true;
      } else if (strcmp(argv[i], "-random") == 0) {
	++i; if (i >= argc) throw 1;
	insize = atoi(argv[i]);
	++i; if (i >= argc) throw 1;
	outsize = atoi(argv[i]);
	brandom = true;
      } else if (strcmp(argv[i], "-fanin") == 0) {
	++i; if (i >= argc) throw 1;
	fanin = atoi(argv[i]);
      } else if (strcmp(argv[i], "-one2one") == 0) {
	one2one = true;
	++i; if (i >= argc) throw 1;
	insize = atoi(argv[i]);
      } else if ((strcmp(argv[i], "-help") == 0) ||
		 (strcmp(argv[i], "-h") == 0)) {
	return false;
      } else throw 2;
    } catch (int err) {
      cerr << "input error: ";
      switch (err) {
      case 0: cerr << "expecting string after " << argv[i-1]; break;
      case 1: cerr << "expecting integer after " << argv[i-1]; break;
      case 2: cerr << "unknown parameter " << argv[i-1]; break;
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
  cout << "Usage: ./maketable [OPTIONS]"
       << endl << "Options are:" << endl;
  cout << "  -full <in size> <out size>" << endl
       << "   Connect fully inputs to outputs." << endl;
  cout << "  -random <in size> <out size>" << endl
       << "   Connect randomly inputs to outputs." << endl;
  cout << "  -fanin <n>" 
       << "   Number of inputs connected to each output." << endl;
  cout << "  -one2one <size>" << endl
       << "   Connect [0 .. size] to [0 .. size] in a 1 to 1 mapping." << endl;
}

int main(int argc, char **argv) {
  cout << "___________________________________________________________________";
  cout << endl << endl;
  cout << "             maketable for eblearn" << endl;
  cout << "___________________________________________________________________";
  cout << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }
  // make table
  idx<intg> table;
  ostringstream name;
  string type = "";
  if (full) {
    cout << "Making a full table from " << insize << " to "
	 << outsize << endl;
    table = full_table(insize, outsize);
    type = "full";
    fanin = insize; // full fanin
  } else if (brandom) {
    if (fanin == 0)
      eblerror("you must set a fanin > 0");
    cout << "Making a random table from " << insize << " to "
	 << outsize << " with fanin " << fanin << endl;
    dynamic_init_drand(); // init random seed
    table = random_table(insize, outsize, fanin);
    type = "random";
  } else if (one2one) {
    cout << "Making a one to one table from " << insize << " to "
	 << insize << endl;
    table = one2one_table(insize);
    type = "one2one";
    fanin = 1;
    outsize = insize;
  } else
    eblerror("unknown type");
  table.printElems();
  float density = table.dim(0) / (float) (insize * outsize);
  cout << "Table dimensions: " << table << endl;
  name << "table_" << insize << "_" << outsize << "_connect_" << table.dim(0)
       << "_fanin_" << fanin
       << "_density_" << setw(1) << setfill('0') << setprecision(2)
       << density << "_" << type << ".mat";
  save_matrix<int>(table, name.str());
  cout << "Saved table in " << name.str() << endl;
  return 0;
}

