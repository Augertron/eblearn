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

#ifndef TOOLS_UTILS_HPP_
#define TOOLS_UTILS_HPP_

#include <map>
#include <string>
#include <sstream>
#include "stl.h"

namespace ebl {

  template <typename T1, typename T2>
  std::string map_to_string(std::map<T1,T2> &m) {
    std::string s;
    typename std::map<T1,T2>::iterator j;
    for (j = m.begin(); j != m.end(); ++j)
      s << "(" << j->first << ", " << j->second << ") ";
    return s;
  }

  template <typename T1, typename T2>
  std::string map_to_string2(std::map<T1,T2> &m) {
    std::ostringstream s;
    typename std::map<T1,T2>::iterator j;
    for (j = m.begin(); j != m.end(); ++j)
      s << j->first << ": " << j->second << std::endl;
    return s.str();
  }

  template <typename T>
  void list_to_vector(std::list<T> &l, std::vector<T> &v) {
    v.resize(l.size());
    typename std::vector<T>::iterator iv = v.begin();
    typename std::list<T>::iterator il = l.begin();
    for ( ; il != l.end(); ++iv, ++il) {
      *iv = *il;
    }
  }

  template <typename T>
  idx<T> string_to_idx(const char *s_, char sep) {
    idx<T> d(1);
    std::string s = s_;
    int k = 0;
    bool found = false;
    while (s.size()) {
      uint j;
      for (j = 0; j < s.size(); ++j)
	if (s[j] == sep)
	  break ;
      std::string s0 = s.substr(0, j);
      if (j >= s.size())
	s = "";
      else
	s = s.substr(j + 1, s.size());
      if (!s0.empty()) {
	if (!found)
	  d.set(string_to_number<T>(s0.c_str()), 0); // 1st element
	else {
	  d.resize(d.dim(0) + 1);
	  d.set(string_to_number<T>(s0.c_str()), d.dim(0) - 1);
	}
	found = true;
	k++;
      }
    }
    if (!found)
      eblerror("expected at least 1 number in: " << s_);
    return d;
  }

  template <typename T>
  T string_to_number(const char *s_) {
    return (T) string_to_double(s_);
  }

////////////////////////////////////////////////////////////////////////////////

template <typename T>
idx<T> load_csv_matrix(const char *filename, bool ignore_first_line,
		       bool ignore_missing_value_lines) {
  char sep = ',';
  T default_value = 0;
  // open file
  FILE *fp = fopen(filename, "rb");
  if (!fp) eblthrow("load_csv_matrix failed to open " << filename);
  // read it
  idx<T> m;
  char *ret = NULL, buf[4096];
  intg line = 0;
  intg n = 1;
  intg i = 0, j = 0;
  // figure out geometry
  try {
    while ((ret = fgets(buf, 4096, fp))) {
      if (line == 0 && ignore_first_line) {
	line++;
	continue ;
      }
      if (line == 1) {
	std::string s = buf;
	size_t pos = 0;
	size_t len = s.size();
	while (pos < len && ((pos = s.find(sep, pos)) != std::string::npos)) {
	  n++;
	  pos++;
	}
      }
      line++;
      i++;
    }
    EDEBUG("found a " << i << "x" << n << " matrix");
    m = idx<T>(i, n);
    idx_clear(m);
    fseek(fp, 0, SEEK_SET); // reset fp to beginning
    i = 0;
    line = 0;
    while ((ret = fgets(buf, 4096, fp))) {
      if (line == 0 && ignore_first_line) {
	line++;
	continue ;
      }
      std::string s = buf, tmp;
      size_t pos0 = 0;
      size_t pos1 = 0;
      size_t len = s.size();
      bool missing = false;
      j = 0;
      while (pos0 < len && ((pos1 = s.find(sep, pos0)) != std::string::npos)) {
	if (pos1 - pos0 == 0) {
	  eblwarn("no value at position " << i << ", " << j << ", setting to "
		  << default_value);
	  m.set(default_value, i, j);
	  missing = true;
	} else {
	  tmp = s.substr(pos0, pos1-pos0);
	  EDEBUG("string to double from: " << tmp);
	  try {
	    m.set((T) string_to_double(tmp), i, j);
	  } eblcatchwarn_extra(missing = true;);
	}
	j++;
	pos0 = pos1 + 1;
      }
      if (len - pos0 - 1 == 0) {
	eblwarn("no value at position " << i << ", " << j << ", setting to "
		<< default_value);
	m.set(default_value, i, j);
	missing = true;
      } else {
	tmp = s.substr(pos0, len-pos0);
	EDEBUG("string to double from: " << tmp);
	try {
	  m.set((T) string_to_double(tmp), i, j);
	} eblcatchwarn_extra(missing = true;);
      }
      idx<T> mtmp = m.select(0, i);
      EDEBUG("row: " << mtmp.str());
      if (n != m.dim(1))
	eblerror("expected " << m.dim(1) << " values but found " << n);
      line++;
      if (missing && ignore_missing_value_lines) {
	eblwarn("ignoring line " << i
		<< " because it contains a missing value");
	m = m.narrow(0, m.dim(0) - 1, 0);
      } else
	i++;
    }    
    fclose(fp);
  } catch(eblexception &e) { eblthrow(" while loading " << filename) }
  return m;
}

} // end namespace ebl

#endif /* TOOLS_UTILS_HPP_ */
