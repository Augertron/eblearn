/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
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

#include "sort.h"

using namespace std;

namespace ebl {

#include <cctype>


  inline int safe_compare(int a, int b) {
    return a < b ? -1 : a > b;
  }

  int natural_compare(const char *a, const char *b) {
    int cmp = 0;
    const char *a0 = a, *b0 = b;
    const char *aprev, *bprev;
    bool decimal = false;

    while (cmp == 0 && *a != '\0' && *b != '\0') {      
      int_span lhs(a), rhs(b);
      aprev = max(a0, a - 1);
      bprev = max(b0, b - 1);
      if (*aprev == '.' || *bprev == '.')
	decimal = true;
      if (!lhs.is_int() || !rhs.is_int())
	decimal = false;
      if (lhs.is_int() && rhs.is_int() && !decimal) {
	if (lhs.digits() != rhs.digits()) {
	  // For differing widths (excluding leading characters),
	  // the value with fewer digits takes priority
	  cmp = safe_compare(lhs.digits(), rhs.digits());
	}
	else {
	  int digits = lhs.digits();

	  a = lhs.value();
	  b = rhs.value();

	  // For matching widths (excluding leading characters),
	  // search from MSD to LSD for the larger value
	  while (--digits >= 0 && cmp == 0)
	    cmp = safe_compare(*a++, *b++);
	}

	if (cmp == 0) {
	  // If the values are equal, we need a tie   
	  // breaker using leading whitespace and zeros
	  if (lhs.non_value() != rhs.non_value()) {
	    // For differing widths of combined whitespace and 
	    // leading zeros, the smaller width takes priority
	    cmp = safe_compare(lhs.non_value(), rhs.non_value());
	  }
	  else {
	    // For matching widths of combined whitespace 
	    // and leading zeros, more whitespace takes priority
	    cmp = safe_compare(rhs.whitespace(), lhs.whitespace());
	  }
	}
      }
      else {
	// No special logic unless both spans are integers
	cmp = safe_compare(*a++, *b++);
      }
    }

    // All else being equal so far, the shorter string takes priority
    return cmp == 0 ? safe_compare(*a, *b) : cmp;
  }

  int natural_compare(const std::string& a, const std::string& b) {
    return natural_compare(a.c_str(), b.c_str());
  }
  
  bool natural_less::operator()(const std::string& a, const std::string& b) {
    return natural_compare(a, b) < 0;
  }

} // end namespace ebl
