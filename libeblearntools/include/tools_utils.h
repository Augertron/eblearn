/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef TOOLS_UTILS_H_
#define TOOLS_UTILS_H_

#include <list>
#include <string>
#include <vector>
#include <map>

#include "defines.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // directory utilities

  typedef pair<string, string> stringpair;
  typedef list<stringpair> files_list;

  inline bool less_than(const stringpair& b1, const stringpair& b2);
  
  //! Returns a list of pairs of root directory and filename of all files
  //! found recursively in directory 'dir'. The files are found using
  //! the IMAGE_PATTERN regular expression by default.
  //! If the directory does not exists, it returns NULL.
  //! The user is responsible for deleting the returned list.
  //! \param fl A file list where new found files will be apprended if not null.
  //!           If null, a new list is allocated. This is used by the recursion.
  //! \param pattern The regular expression describing the file name pattern.
  //!           The default pattern matches images extensions.
  files_list *find_files(const string &dir,
			 const char *pattern = IMAGE_PATTERN,
			 files_list *fl = NULL, bool sorted = true);
  
  //! Returns a list of string of full paths to files recursively found in
  //! direcotry dir and matching the pattern. The files are found using
  //! the IMAGE_PATTERN regular expression by default.
  //! If the directory does not exists, it returns NULL.
  //! The user is responsible for deleting the returned list.
  //! \param fl A file list where new found files will be apprended if not null.
  //!           If null, a new list is allocated. This is used by the recursion.
  //! \param pattern The regular expression describing the file name pattern.
  //!           The default pattern matches images extensions.
  list<string> *find_fullfiles(const string &dir,
			       const char *pattern = IMAGE_PATTERN,
			       list<string> *fl = NULL, bool sorted = true);
  
  //! Counts recursively the number of files matching the pattern (default is
  //! an image extension pattern) in directory 'dir'.
  //! \param pattern The regular expression describing the file name pattern.
  //!           The default pattern matches images extensions.
  uint count_files(const string &dir, const char *pattern = IMAGE_PATTERN);

  //! Convert a string to an int. Throws a const char * exception
  //! upon failure.
  int string_to_int(const string &s);

  //! Convert a string to an unsigned int. Throws a const char * exception
  //! upon failure.
  uint string_to_uint(const string &s);

  //! Convert a string to an float. Throws a const char * exception
  //! upon failure.
  float string_to_float(const string &s);

  //! Convert a string to an double. Throws a const char * exception
  //! upon failure.
  double string_to_double(const string &s);

  //! Convert a string containing a list of uint separated by commas, e.g.
  //! "1,2,3,4" into a list of uints.
  list<uint> string_to_uintlist(const string &s);

  //! Convert a string containing a list of strings separated by commas, e.g.
  //! "errors,2,toto,4" into a list of strings.
  list<string> string_to_stringlist(const string &s);

  //! Convert a string containing a list of double separated by commas, e.g.
  //! "1,2,3.0,4.0" into a vector of doubles.
  vector<double> string_to_doublevector(const string &s);

  //! Convert a map to a string representation.
  template <typename T1, typename T2> string map_to_string(map<T1,T2> &m);
  
  //! Convert a map to a string representation, printing a new line between
  //! each variables/value pair.
  template <typename T1, typename T2> string map_to_string2(map<T1,T2> &m);
  
  //! Convert a string list to a string representation.
  string stringlist_to_string(list<string> &l);
  
  //! Tar directory dir into a .tgz archive, using directory's rightmost name,
  //! into target directory tgtdir.
  //! Return false upon failure, true otherwise.
  bool tar(const string &dir, const string &tgtdir);

  //! Tar all files found recursively in directory dir into a compressed tar 
  //! archive "tgtdir/tgtfilename", using matching pattern 'pattern'.
  //! Return false upon failure, true otherwise.
  bool tar_pattern(const string &dir, const string &tgtdir,
		   const string &tgtfilename, const char *pattern);

  //! Resize and fill vector v with values in l.
  template <typename T>
    void list_to_vector(list<T> &l, vector<T> &v);
    
} // end namespace ebl

#include "tools_utils.hpp"

#endif /* TOOLS_UTILS_ */
