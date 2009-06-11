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

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

#include "libeblearntools.h"

using namespace std;
using namespace ebl;

typedef map<string, string, less<string> > string_map_t;
typedef map<string, vector<string>, less<string> > string_list_map_t;

void remove_trailing_whitespaces(string &s) {
  string::size_type pos;
  
  pos = s.find_first_not_of(' ');
  if (pos == string::npos)
    s = "";
  else {
    s = s.substr(pos);
    pos = s.find_last_not_of(' ');
    s = s.erase(pos + 1);
  }
}

// open file fname and put variables assignments in smap.
// e.g. " i = 42 # comment " will yield a entry in smap
// with "i" as first value and "42" as second value.
bool extract_variables(string &fname, string_map_t &smap) {
  string s0, s;
  char separator = '=';
  char comment1 = '#';
  char comment2 = ';';
  string::size_type pos;
  string name, value;

  ifstream in(fname.c_str());
  if (!in) {
    cerr << "error: failed to open " << fname << endl;
    return false;
  }
  while (!in.eof()) {
    name = "";
    value = "";
    getline(in, s0);
    // remove comments first
    s = s0;
    pos = s.find(comment1);
    if (pos != string::npos) // comment found, remove it
      s = s.erase(pos);
    pos = s.find(comment2);
    if (pos != string::npos) // comment found, remove it
      s = s.erase(pos);
    if (s.size() > 0) {
      // look for separator
      pos = s.find(separator);
      if (pos != string::npos) {
	if (pos >= s.size() - 1) {
	  cerr << "warning: variable value cannot be empty,";
	  cerr << " ignoring this line: " << s0 << endl;
	  continue ;
	}
	// separate in 2
	name = s.substr(0, pos);
	value = s.substr(pos + 1);
	// removing trailing whitespaces
	remove_trailing_whitespaces(name);
	remove_trailing_whitespaces(value);
	// forbid spaces in names
	pos = name.find(' ');
	if (pos != string::npos) {
	  cerr << "warning: variable name cannot contain an empty space,";
	  cerr << " ignoring this line: " << s0 << endl;
	  continue ;
	}
	// forbid empty values
	if (value.size() == 0) {
	  cerr << "warning: variable value cannot be empty,";
	  cerr << " ignoring this line: " << s0 << endl;
	  continue ;
	}
	// forbid duplicates
	if (smap.find(name) != smap.end()) {
	  cerr << "warning: duplicate variable name \"" << name;
	  cerr << "\", ignoring new assignment: " << s0 << endl;
	  continue ;
	}
	// add variable to map
	smap[name] = value;
      }
    }
  }
  return true;
}

void variables_to_variables_list(string_map_t &smap, string_list_map_t &lmap) {
  string::size_type pos;
  string s, stmp;
  string_map_t::iterator smi = smap.begin();
  for ( ; smi != smap.end(); ++smi) {
    s = smi->second;
    vector<string> &vs = lmap[smi->first];
    // loop over list of elements
    pos = s.find_first_of(' ');
    while (pos != string::npos) {
      stmp = s.substr(0, pos);
      vs.push_back(stmp);
      s = s.substr(pos);
      remove_trailing_whitespaces(s);
      pos = s.find(' ');
    }
    vs.push_back(s);
  }
}

int main(int argc, char **argv) {
  string fname = argv[1];
  string_map_t smap;
  string_list_map_t lmap;

  extract_variables(fname, smap);
  variables_to_variables_list(smap, lmap);
  
  cout << "__________________" << endl;
  string_list_map_t::iterator lmi = lmap.begin();
  for ( ; lmi != lmap.end(); ++lmi) {
    cout << lmi->first << " : ";
    vector<string>::iterator vi = lmi->second.begin();
    for ( ; vi != lmi->second.end(); ++vi) {
      cout << *vi << ", ";
    }
    cout << endl;
  }
  return 0;
}
