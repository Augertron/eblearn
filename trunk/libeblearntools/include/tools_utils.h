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

#include "defines.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // directory utilities

  typedef pair<string, string> stringpair;
  typedef list<stringpair> files_list;

  inline bool less_than(const stringpair& b1, const stringpair& b2);
  
  //! Returns a list of pairs of root directory and filename of all images
  //! found recursively in directory 'dir'. The images are found using
  //! the IMAGE_PATTERN regular expression by default.
  //! If the directory does not exists, it returns NULL.
  //! \param fl A file list where new found files will be apprended if not null.
  //!           If null, a new list is allocated. This is used by the recursion.
  files_list *find_images(const string &dir,
			  const char *pattern = IMAGE_PATTERN,
			  files_list *fl = NULL);
  
} // end namespace ebl

#endif /* TOOLS_UTILS_ */
