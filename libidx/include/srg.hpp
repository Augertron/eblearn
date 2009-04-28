/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com   *
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

#ifndef SRG_HPP_
#define SRG_HPP_

#include <stdlib.h>
#include <string.h>

namespace ebl {

// Templates for Srg.h
// This is placed in an hpp file so that implementation files can instanciate templates.

////////////////////////////////////////////////////////////////
// the creation of an idx should call lock() on the Srg,
// and its destruction should call unlock().

// simplest constructor builds an empty Srg
template <class T> Srg<T>::Srg() {
  refcount = 0;
  data = (T *)NULL;
  size_ = 0;
}

// return size on success, and -1 on error
template <class T> Srg<T>::Srg(intg s) {
  intg r;
  refcount = 0;
  data = (T *)NULL;
  if ( ( r=this->changesize(s) ) > 0 ) this->clear();
  if (r < 0) { eblerror("can't allocate Srg"); }
}

// destructor: can be called twice when the Srg
// is not dynamically allocated. Hence this must
// make sure the data is not deallocated twice.
template <class T> Srg<T>::~Srg() {
  DEBUG("Srg::destructor: refcount=%d\n",refcount);
  if (refcount != 0) { eblerror("can't delete an Srg with non zero refcount"); }
  if (data != NULL) {
    free((void *)data);
    data = NULL;
    size_ =0;
  }
}

// return size
template <class T> intg Srg<T>::size() { return size_; }

// low-level resize: can grow and shrink
// returns -1 on failure.
// Self should be used with care, because shrinking
// an Srg that has idx pointing to it is very dangerous.
// In most case, the grow() method should be used.
template <class T> intg Srg<T>::changesize(intg s) {
  if (s == 0) {
    free((void *)data);
    data = (T *)NULL;
    size_ = 0;
  } else {
    data = (T *)realloc((void *)data, s*sizeof(T));
    if (data != NULL) {
      size_ = s;
      return size_;
    } else {
      size_ = 0;
      return -1;
    }
  }
  return size_;
}

// this grows the size of the srg if necessary.
// This is called when a new idx is created on the Srg.
// returns -1 on failure.
template <class T> intg Srg<T>::growsize(intg s) {
  if (s > size_) { return this->changesize(s); } else { return size_; }
}

template<class T> intg Srg<T>::growsize_chunk(intg s, intg s_chunk){
  if(s > size_) { return this->changesize(s + s_chunk); } else {return size_;}
}

// decrement refcount and free if it reaches zero
template <class T> int Srg<T>::unlock() {
  refcount--;
  DEBUG("Srg::unlock: refcount=%d\n",refcount);
  if (refcount<0) { eblerror("Srg has negative refcount"); return refcount; }
  else {
    if (refcount == 0) { delete this; return 0; } else { return refcount; }
  }
}

// increment refcount
template <class T> int Srg<T>::lock() {
  DEBUG("Srg::lock: refcount=%d\n",refcount+1);
  return ++refcount;
}

// get i-th item
template <class T> T Srg<T>::get(intg i) { return data[i]; }

// set i-th item
template <class T> void Srg<T>::set(intg i, T val) { data[i] = val; }

// fill with a ubyte value
template <class T> void Srg<T>::clear() { memset(data, 0, size_ * sizeof(T)); }

// prints innards
template <class T> void Srg<T>::pretty(FILE *f) {
  fprintf(f,"Srg at address %ld; ",(long)this);
  fprintf(f,"size=%ld; ",size_);
  fprintf(f,"data=%ld; ",(long)data);
  fprintf(f,"refcount=%d\n",refcount);
}

} // end namespace ebl

#endif /* SRG_HPP_ */
