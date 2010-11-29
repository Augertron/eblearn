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
			 files_list *fl_, bool sort, bool recursive,
			 bool randomize) {
    files_list *fl = fl_;
#ifndef __BOOST__
    eblerror("boost not installed, install and recompile");
#else
    if (sort && randomize)
      eblerror("it makes no sense to sort and randomize at the same time");
    cmatch what;
    regex r(pattern);
    path p(dir);
    if (!exists(p))
      return NULL; // return if invalid directory
    // allocate fl if null
    if (!fl)
      fl = new files_list();
    directory_iterator end_itr; // default construction yields past-the-end
    // first process all elements of current directory
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (!is_directory(itr->status()) && 
	  regex_match(itr->leaf().c_str(), what, r)) {
	// found an match, add it to the list
	fl->push_back(pair<string,string>(itr->path().branch_path().string(),
					  itr->leaf()));
      }
    }
    // then explore subdirectories
    if (recursive) {
      for (directory_iterator itr(p); itr != end_itr; ++itr) {
	if (is_directory(itr->status()))
	  find_files(itr->path().string(), pattern, fl, sort,
		     recursive, false);
      }
    }
    // sort list
    if (sort)
      fl->sort(less_than);
    // randomize list
    if (randomize) {
      // list randomization is very inefficient, so first copy to vector,
      // randomize, then copy back (using vectors directly would be innefficient
      // too for big file lists because we constantly resize the list as we
      // find new files).
      vector<stringpair> v(fl->size());
      vector<stringpair>::iterator iv = v.begin();
      for (files_list::iterator i = fl->begin(); i != fl->end(); ++i, ++iv)
	*iv = *i;
      delete fl;
      fl = new files_list();
      random_shuffle(v.begin(), v.end());
      for (iv = v.begin(); iv != v.end(); ++iv)
	fl->push_back(*iv);
    }
#endif
    return fl;
  }

  list<string> *find_fullfiles(const string &dir, const char *pattern,
			       list<string> *fl_, bool sorted, bool recursive) {
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
    // first process all elements of current directory
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (!is_directory(itr->status()) && 
	  regex_match(itr->leaf().c_str(), what, r)) {
	// found an match, add it to the list
	fl->push_back(itr->path().string());
      }
    }
    // then explore subdirectories
    if (recursive) {
      for (directory_iterator itr(p); itr != end_itr; ++itr) {
	if (is_directory(itr->status()))
	  find_fullfiles(itr->path().string(), pattern, fl, sorted);
      }
    }
    // sort list
    if (sorted)
      fl->sort();
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
    return string_to_uint(s.c_str());
  }
  
  uint string_to_uint(const char *s) {
    istringstream iss(s);
    uint d;
    iss >> d;
    if (iss.fail()) {
      cerr << "\"" << s << "\" is not an unsigned int." << endl;
      eblerror("invalid conversion to uint");
      throw "invalid conversion to uint";
    }
    return d;
  }

  int string_to_int(const string &s) {
    return string_to_int(s.c_str());
  }
  
  int string_to_int(const char *s) {
    istringstream iss(s);
    int d;
    iss >> d;
    if (iss.fail()) {
      cerr << "\"" << s << "\" is not an unsigned int." << endl;
      eblerror("invalid conversion to int");
      throw "invalid conversion to int";
    }
    return d;
  }

  float string_to_float(const string &s) {
    return string_to_float(s.c_str());
  }

  float string_to_float(const char *s) {
    istringstream iss(s);
    float d;
    iss >> d;
    if (iss.fail()) {
      cerr << "\"" << s << "\" is not a float." << endl;
      throw "invalid conversion to float";
    }
    return d;
  }

  double string_to_double(const string &s) {
    return string_to_double(s.c_str());
  }

  double string_to_double(const char *s) {
    istringstream iss(s);
    double d;
    iss >> d;
    if (iss.fail()) {
      cerr << "\"" << s << "\" is not a double." << endl;
      throw "invalid conversion to double";
    }
    return d;
  }

  list<uint> string_to_uintlist(const string &s_) {
    return string_to_uintlist(s_.c_str());
  }
  
  list<uint> string_to_uintlist(const char *s_) {
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

  list<string> string_to_stringlist(const string &s_) {
    return string_to_stringlist(s_.c_str());
  }
  
  list<string> string_to_stringlist(const char *s_) {
    list<string> l;
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
      l.push_back(s0);
      k++;
    }
    return l;
  }

  vector<double> string_to_doublevector(const string &s_) {
    return string_to_doublevector(s_.c_str());
  }
  
  vector<double> string_to_doublevector(const char *s_) {
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

  bool tar(const string &dir, const string &tgtdir) {
#ifdef __BOOST__
    ostringstream cmd;
    path p(dir);
    cmd << "tar cz -C " << dir << "/../ -f " << tgtdir << "/" << p.leaf()
	<< ".tgz " << p.leaf();// << " 2> /dev/null";
    int ret = std::system(cmd.str().c_str());
    if (ret < 0) {
      cerr << "tar failed." << endl;
      return false;
    }
#endif
    return true;
  }

  bool tar_pattern(const string &dir, const string &tgtdir,
		   const string &tgtfilename, const char *pattern) {
#ifdef __BOOST__
    // find all files matching pattern
    list<string> *files = find_fullfiles(dir, pattern);
    if (!files) {
      cerr << "No files matching pattern \"" << pattern
	   << "\" were found." << endl;
      return false;
    }
    // tar them
    ostringstream cmd;
    path p(dir);
    cmd << "tar czf " << tgtdir << "/" << tgtfilename << " ";
    for (list<string>::iterator i = files->begin();
	 i != files->end(); ++i) {
      cmd << *i << " ";
    }
    int ret = std::system(cmd.str().c_str());
    if (ret < 0) {
      cerr << "tar failed." << endl;
      return false;
    }
#endif
    return true;
  }

  string stringlist_to_string(list<string> &l) {
    ostringstream s;
    list<string>::iterator j;
    for (j = l.begin(); j != l.end(); ++j)
      s << *j << " ";
    return s.str();
  }

  string dirname(const char *s_) {
#ifdef __LINUX__
    char c = '/';
#else /* __WINDOWS__ */
    char c = '\\';
#endif
    string s = s_;
    size_t pos = s.find_last_of(c);
    return s.substr(0, pos);
  }
  
  string filename(const char *s_) {
#ifdef __LINUX__
    char c = '/';
#else /* __WINDOWS__ */
    char c = '\\';
#endif
    string s = s_;
    size_t pos = s.find_last_of(c);
    return s.substr(pos + 1);
  }
  
} // namespace ebl