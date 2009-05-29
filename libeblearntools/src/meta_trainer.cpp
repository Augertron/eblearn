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

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

#include "libeblearntools.h"

using namespace std;
using namespace ebl;

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
    if (strcmp(argv[i], "-channels") == 0) {
      ++i;
      if (i >= argc) {
	cerr << "input error: expecting string after -channels." << endl;
	return false;
      }
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
  cout << "      - YpUV" << endl;
  cout << "      - HSV" << endl;
  cout << "      - Yp (Yp only in YpUV)" << endl;
  cout << "      - YpH3" << endl;
  cout << "      - VpH2SV" << endl;
  cout << "  -dset_display" << endl;
  cout << "  -training" << endl;
  cout << "  -testing" << endl;
  cout << "  -stereo" << endl;
  cout << "  -stereo_lpattern <pattern>" << endl;
  cout << "  -stereo_rpattern <pattern>" << endl;
  cout << "  -output_dir <directory>" << endl;
  cout << "  -dset_name <dataset_name>" << endl;
  cout << "  -max_per_class <integer>" << endl;
  cout << "  -mexican_hat_size <integer>" << endl;
  cout << "  -deformations <integer>" << endl;
}

bool append_plotter(const string &name, const string &vname, int col) {
  string fname = vname;
  fname += ".p";
  ifstream in(fname.c_str());
  ofstream out(fname.c_str(), ios::app);
  cout << "append plotter " << name << " " << vname << endl;
  if (!out) { 
    cerr << "warning: failed to open " << fname << endl;
    return false;
  }
  if (!in) {
    in.close();
    out << "clear ; \
set terminal png small size 1024,768 ; \n	\
set output \"plot.png\" ;			\
set multiplot ;					\
xrmin = 0.0 ;					\
xrmax = 50.0 ;					\
set size 1, 0.33 ;				\
set origin 0.0,0.64 ;				\
plot ";
  } else
    out << ", ";
  out << "\"" << vname << ".plot\" using 1:" << col + 1 << " title \"";
  out << name << "\" with lines";
  out.close();
  return true;
}

int count_numbers(const string &sentence) {
  istringstream iss(sentence, istringstream::in);
  int c = 0;
  while (!iss.eof()) {
    float f = numeric_limits<float>::max();
    iss >> f;
    if (f != numeric_limits<float>::max()) {
    c++;
    }
  }
  return c;
}

bool append_plot(const string &vname, int line, float value, int &colpos) {
  string s, fname = vname;
  fname += ".plot";
  string fname2 = fname;
  fname2 += ".tmp";
  string separator = "\t";
  ifstream in(fname.c_str());
  ofstream out(fname2.c_str());
  int i;

  if (!out) {
    cerr << "warning: failed to open " << fname << endl;
    return false;
  }
  i = 0;
  if (!in) {
    out << i << separator << value << separator;
    colpos = 1;
  } else {
    i = 0;
    while (!in.eof()) {
      getline(in, s);
      if (colpos < 0)
	colpos = count_numbers(s);
      out << s;
      if (line == i) {
	for (int j = count_numbers(s); j < colpos; ++j) {
	  if (j == 0)
	    out << i << separator;
	  else 
	    out << 0 << separator;
	}
	out << value << separator;
      }
      if (s.size() > 0)
	out << endl;
      i++;
    }
    for ( ; i <= line; ++i) {
      out << i << separator;
      for (int j = 1; j < colpos; ++j)
	out << 0 << separator;
      if (i == line) out << value << separator;
      out << endl;
    }
  }
  out.close();
  rename(fname2.c_str(), fname.c_str());
  return true;
}

bool parse_output_log(const string &fname, const string &name) {
  cout << "parsing " << fname << endl;
  ifstream in(fname.c_str());
  string s, vname;
  char separator = '=';
  int line = 0;
  float f;
  int colpos = -1;
  string::size_type itok, stok;
  bool first = true;

  if (!in) {
    cerr << "warning: failed to open " << fname << endl;
    return false;
  }
  while (!in.eof()) {
    getline(in, s);
    istringstream iss(s, istringstream::in);
    try {
      iss >> line;
      itok = s.find(separator);
      while (itok != string::npos) {
	stok = s.find_last_of(' ', itok - 1); 
	vname = s.substr(stok + 1, itok - stok - 1);
	s = s.substr(itok + 1);
	iss.str(s);
	iss >> f;
	itok = s.find(separator);
	append_plot(vname, line, f, colpos);
	if (first)
	  append_plotter(name, vname, colpos);
	if (itok == string::npos)
	  first = false;
      }
      line++;
    } catch(std::istringstream::failure& e) {}
  }
  return true;
}

int main(int argc, char **argv) {
  cout << "________________________________Meta Trainer";
  cout << "________________________________" << endl;
  // parse arguments
  if (!parse_args(argc, argv)) {
    print_usage();
    return -1;
  }

  string root = "/home/pierre/logstmp";
  
#ifdef __BOOST__
    // check root directory
    path rd(root);
    if (!exists(rd)) {
      cerr << "cannot find path: " << rd.string() << endl;
      eblerror("path does not exist");
      return false;
    }
    // explore root
    cmatch what;
    regex hidden_dir(".svn");
    regex output_log("output.log");
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(rd); itr != end_itr; itr++) {
      if (is_directory(itr->status())
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	// look for output.log in current directory
	for (directory_iterator itr2(itr->path()); itr2 != end_itr; itr2++) {
	  if (!is_directory(itr2->status())
	      && regex_match(itr2->leaf().c_str(), what, output_log)) {
	    // parse output.log
	    parse_output_log(itr2->path().string(), itr->leaf());
	  }
	}
      }
    }
#endif 
  return 0;
}
