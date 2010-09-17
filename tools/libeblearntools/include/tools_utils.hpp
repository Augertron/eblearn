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

using namespace std;

namespace ebl {

  template <typename T1, typename T2>
  string map_to_string(map<T1,T2> &m) {
    ostringstream s;
    typename map<T1,T2>::iterator j;
    for (j = m.begin(); j != m.end(); ++j)
      s << "(" << j->first << ", " << j->second << ") ";
    return s.str();
  }

  template <typename T1, typename T2>
  string map_to_string2(map<T1,T2> &m) {
    ostringstream s;
    typename map<T1,T2>::iterator j;
    for (j = m.begin(); j != m.end(); ++j)
      s << j->first << ": " << j->second << endl;
    return s.str();
  }

  template <typename T>
  void list_to_vector(list<T> &l, vector<T> &v) {
    v.resize(l.size());
    typename vector<T>::iterator iv = v.begin();
    typename list<T>::iterator il = l.begin();
    for ( ; il != l.end(); ++iv, ++il) {
      *iv = *il;
    }
  }

} // end namespace ebl

#endif /* TOOLS_UTILS_HPP_ */

