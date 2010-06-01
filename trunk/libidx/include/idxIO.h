/***************************************************************************
 *   Copyright (C) 2008 by Pierre Sermanet *
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

#ifndef IDXIO_H_
#define IDXIO_H_

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include "idx.h"
#include "idx_iterators.h"

using namespace std;

// standard lush magic numbers
#define MAGIC_FLOAT_MATRIX	0x1e3d4c51
#define MAGIC_PACKED_MATRIX	0x1e3d4c52
#define MAGIC_DOUBLE_MATRIX	0x1e3d4c53
#define MAGIC_INTEGER_MATRIX	0x1e3d4c54
#define MAGIC_BYTE_MATRIX	0x1e3d4c55
#define MAGIC_SHORT_MATRIX	0x1e3d4c56
#define MAGIC_SHORT8_MATRIX	0x1e3d4c57
#define MAGIC_LONG_MATRIX	0x1e3d4c58
#define MAGIC_ASCII_MATRIX	0x2e4d4154

// non-standard magic numbers
#define MAGIC_UINT_MATRIX	0x1e3d4c59

// pascal vincent's magic numbers
#define MAGIC_UBYTE_VINCENT	0x0800
#define MAGIC_BYTE_VINCENT	0x0900
#define MAGIC_SHORT_VINCENT	0x0B00
#define MAGIC_INT_VINCENT	0x0C00
#define MAGIC_FLOAT_VINCENT	0x0D00
#define MAGIC_DOUBLE_VINCENT	0x0E00

namespace ebl {

  // TODO: implement all types.
  // TODO: is check for endianess required?

  ////////////////////////////////////////////////////////////////
  // loading
  
  //! Returns matrix from file filename. If original matrix type is different
  //! than requested type, it is casted (copied) into the new type.
  //! This throws string exceptions upon errors.
  template<typename T>
    idx<T> load_matrix(const char *filename);

  //! Returns matrix from file filename. If original matrix type is different
  //! than requested type, it is casted (copied) into the new type.
  //! This throws string exceptions upon errors.
  template<typename T>
    idx<T> load_matrix(const string &filename);
  
  //! Loads a matrix from file filename into given matrix m.
  //! m if resized if necessary. Data is cast into m's type if different.
  //! This throws string exceptions upon errors.
  template<typename T>
    void load_matrix(idx<T>& m, const char *filename);

  //! Loads a matrix from file filename into given matrix m.
  //! m if resized if necessary. Data is cast into m's type if different.
  //! This throws string exceptions upon errors.
  template<typename T>
    void load_matrix(idx<T>& m, const string &filename);
  
  //! Loads a matrix from stream 'stream' into given matrix out if given,
  //! allocates a new one otherwise. This returns either *out or the newly
  //! allocated idx.
  //! If out is not null, it is resized if necessary.
  //! In all cases, data is cast into T if different.
  //! This throws string exceptions upon errors.
  template<typename T>
    idx<T> load_matrix(istream &stream, idx<T> *out = NULL);

  ////////////////////////////////////////////////////////////////
  // saving
  
  //! Saves a matrix m in file filename.
  //! Returns true if successful, false otherwise.
  template<typename T> bool save_matrix(idx<T>& m, const string &filename);

  //! Saves a matrix m in file filename. One can force the saving type to
  //! a different type than the passed idx, e.g.
  //! by calling save_matrix<float>(m, ..);
  //! Returns true if successful, false otherwise.
  template<typename T2, typename T>
    bool save_matrix(idx<T>& m, const string &filename);
  
  //! Saves a matrix m in file filename.
  //! Returns true if successful, false otherwise.
  template<typename T> bool save_matrix(idx<T>& m, const char *filename);

  //! Saves a matrix m in file filename.
  //! Returns true if successful, false otherwise.
  template<typename T> bool save_matrix(idx<T>& m, ostream &stream);

  ////////////////////////////////////////////////////////////////
  // helper functions
  
  //! Set string type to a string describing the matrix type found in filename.
  //! Possible strings are: ubyte, int, float, double, long, uint,
  //! ubyte (pascal vincent), int (pascal vincent), float (pascal vincent),
  //! double (pascal vincent).
  bool get_matrix_type(const char *filename, string &type);

  //! Return the magic number associated with the matrix's type found in 
  //! 'filename'.
  int get_matrix_type(const char *filename);

  //! Return true if this magic number is a vincent magic number.
  bool is_magic_vincent(int magic);

  //! Return true if this magic number is a regular magic number.
  bool is_magic(int magic);

  //! Return the dimensions found in the header and set 'magic' to the magic
  //! number found (either vincent or regular type).
  idxdim read_matrix_header(istream &stream, int &magic);

} // end namespace ebl

#include "idxIO.hpp"

#endif /* IDXIO_H_ */

