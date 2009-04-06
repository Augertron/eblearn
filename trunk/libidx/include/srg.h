/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun   *
 *   yann@cs.nyu.edu   *
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

#ifndef _SRG_H
#define	_SRG_H

#include <stdio.h>

#include "defines.h"

namespace ebl {

////////////////////////////////////////////////////////////////
// Srg: storage area for idx data.

//! Srg is a container for arrays of data. It contains
//! a pointer to a dynamically allocated chunk of data,
//! a size, and a reference counter. Access structures
//! such as idx, point to an Srg that contains the data.
//! Several idx's can share an Srg allowing access to the
//! data in multiple ways. Whenever an idx is created that
//! that points an Srg, the reference counter of that Srg
//! is incremented through the lock() member function.
//! When the reference counter is reaches 0, the Srg
//! is deallocated. An Srg must always be created with new,
//! and never created as an automatic variable. For that
//! reason, the destructor is private.
//! So, if compiling your code produces an error like
//! "Srg<T>::~Srg() is private", you mistakenly allocated
//! an Srg as an automatic variable instead of new.
template <class T> class Srg{

 private:
  //! number of allocated items
  intg size_;
  //! reference counter: tells us how many idx point here
  int refcount;

  //! destructor: deallocates the data.
  //! This is automatically called when the
  //! reference counter reaches 0.
  //! The destructor is made private so that the compiler
  //! will complain if someone decide to create an
  //! Srg on the stack (as an automatic variable).
  //! An Srg must ALWAYS be created with new.
  ~Srg();

 public:
  //! pointer to data segment
  T *data;

  //! builds an empty Srg (no data is allocated)
  Srg();

  //! allocate an Srg of size s
  Srg(intg s);

  //! return the size (number of items)
  intg size();

  //! change size to s. This must be used with extreme caution,
  //! because reducing the size of an Srg ma cause some idx
  //! that point to it to access non-existent data.
  intg changesize(intg s);

  //! increase size of data chunk
  intg growsize(intg s);

  //! increase size of data chunk by a given step s_chunk
  intg growsize_chunk(intg s, intg s_chunk);

  //! decrement reference counter and deallocate Srg
  //! if it reaches zero.
  int unlock();

  //! lock: increment reference counter.
  //! This is called wheneve a new idx is created
  //! on the Srg.
  int lock();

  //! access method: return the i-th item.
  T get(intg i);

  //! sets i-th element to val.
  void set(intg i, T val);

  //! fill data with zeros.
  void clear();

  //! prints innards
  void pretty(FILE *f);

};

} // end namespace ebl

#include "srg.hpp"

#endif	/* _SRG_H */

