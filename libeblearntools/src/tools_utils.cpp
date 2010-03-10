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

#include "tools_utils.h"
#include <algorithm>
#include <sstream>
#include <iostream>

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // directory utilities

  inline bool less_than(const stringpair& b1, const stringpair& b2) {
    if ((b1.first < b2.first) ||
	((b1.first == b2.first) && (b1.second < b2.second)))
      return true;
    return false;
  }

  files_list *find_files(const string &dir, const char *pattern,
			 files_list *fl_) {
    files_list *fl = fl_;
#ifndef __BOOST__
    eblerror("boost not installed, install and recompile");
#else
    cmatch what;
    regex r(pattern);
    path p(dir);
    if (!exists(p))
      return NULL; // return if invalid directory
    // allocate fl if null
    if (!fl)
      fl = new files_list();
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (is_directory(itr->status()))
	find_files(itr->path().string(), pattern, fl);
      else if (regex_match(itr->leaf().c_str(), what, r)) {
	// found an image, add it to the list
	fl->push_back(pair<string,string>(itr->path().branch_path().string(),
					  itr->leaf()));
      }
      // sort list
      fl->sort(less_than);
    }
#endif
    return fl;
  }

  list<string> *find_fullfiles(const string &dir, const char *pattern,
			       list<string> *fl_) {
    list<string> *fl = fl_;
#ifndef __BOOST__
    eblerror("boost not installed, install and recompile");
#else
    cmatch what;
    regex r(pattern);
    path p(dir);
    if (!exists(p))
      return NULL; // return if invalid directory
    // allocate fl if null
    if (!fl)
      fl = new list<string>();
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (is_directory(itr->status()))
	find_fullfiles(itr->path().string(), pattern, fl);
      else if (regex_match(itr->leaf().c_str(), what, r)) {
	// found an match, add it to the list
	fl->push_back(itr->path().string());
      }
      // sort list
      fl->sort();
    }
#endif
    return fl;
  }

  uint count_files(const string &dir, const char *pattern) {
    uint total = 0;
#ifndef __BOOST__
    eblerror("boost not installed, install and recompile");
#else
    cmatch what;
    regex r(pattern);
    path p(dir);
    if (!exists(p))
      return 0; // return if invalid directory
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (is_directory(itr->status()))
	total += count_files(itr->path().string(), pattern);
      else if (regex_match(itr->leaf().c_str(), what, r)) {
	// found file matching pattern, increment counter
	total++;
      }
    }
#endif
    return total;
  }

  uint string_to_uint(const string &s) {
    istringstream iss(s, istringstream::in);
    uint d;
    d = numeric_limits<uint>::max();
    iss >> d;
    if (d == numeric_limits<uint>::max()) {
      cerr << "\"" << s << "\" is not an unsigned int." << endl;
      eblerror("invalid conversion to uint");
      throw "invalid conversion to uint";
    }
    return d;
  }

  double string_to_double(const string &s) {
    istringstream iss(s, istringstream::in);
    double d;
    d = numeric_limits<double>::max();
    iss >> d;
    if (d == numeric_limits<double>::max()) {
      cerr << "\"" << s << "\" is not a double." << endl;
      throw "invalid conversion to double";
    }
    return d;
  }

  list<uint> string_to_uintlist(const string &s_) {
    list<uint> l;
    string s = s_;
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
      l.push_back(string_to_uint(s0));
      k++;
    }
    return l;
  }

  vector<double> string_to_doublevector(const string &s_) {
    vector<double> l;
    string s = s_;
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
      l.push_back(string_to_double(s0));
      k++;
    }
    return l;
  }

  void tar(const string &dir, const string &tgtdir) {
#ifdef __BOOST__
    ostringstream cmd;
    path p(dir);
    cmd << "tar cz -C " << dir << "/../ -f " << tgtdir << "/" << p.leaf() << ".tgz " 
	<< p.leaf();// << " 2> /dev/null";
    int ret = std::system(cmd.str().c_str());
    if (ret < 0)
      cerr << "tar failed." << endl;
#endif
  }

} // namespace ebl
