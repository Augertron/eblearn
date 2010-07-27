/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
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

// tell header that we are in the libidx scope
#define LIBIDX

#include <stdio.h>
#include <iomanip>
#include "idxIO.h"

using namespace std;

namespace ebl {
  
  ////////////////////////////////////////////////////////////////
  // helper functions
  
  // TODO: use c++ IO to catch IO exceptions more easily
  // TODO: if types differ, print warning and cast to expected type
  // TODO: allow not knowing order in advance (just assign new idx to m)
  bool get_matrix_type(const char *filename, string &type) {
    // open file
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
      cerr << "get_matrix_type failed to open " << filename << "." << endl;
      return false;
    }

    int magic;
    // header: read magic number
    if (fread(&magic, sizeof (int), 1, fp) != 1) {
      cerr << "failed to read " << filename << "." << endl;
      fclose(fp);
      return false;
    }
    type = get_magic_str(magic);
    return true;
  }

  int get_matrix_type(const char *filename) {
    // open file
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
      ostringstream err;
      err << "get_matrix_type failed to open " << filename;
      throw err.str();
    }

    int magic;
    // header: read magic number
    if (fread(&magic, sizeof (int), 1, fp) != 1) {
      ostringstream err;
      err << "failed to read magic number in " << filename;
      fclose(fp);
      throw err.str();
    }
    return magic;
  }

  bool is_magic_vincent(int magic) {
    if (magic == MAGIC_UBYTE_VINCENT
	|| magic == MAGIC_BYTE_VINCENT
	|| magic == MAGIC_SHORT_VINCENT
	|| magic == MAGIC_INT_VINCENT
	|| magic == MAGIC_FLOAT_VINCENT
	|| magic == MAGIC_DOUBLE_VINCENT)
      return true;
    return false;
  }

  bool is_magic(int magic) {
    if (magic == MAGIC_FLOAT_MATRIX
	|| magic == MAGIC_PACKED_MATRIX
	|| magic == MAGIC_DOUBLE_MATRIX
	|| magic == MAGIC_INTEGER_MATRIX
	|| magic == MAGIC_BYTE_MATRIX
	|| magic == MAGIC_SHORT_MATRIX
	|| magic == MAGIC_SHORT8_MATRIX
	|| magic == MAGIC_LONG_MATRIX
	|| magic == MAGIC_ASCII_MATRIX
	|| magic == MAGIC_UINT_MATRIX)
      return true;
    return false;
  }

  idxdim read_matrix_header(FILE *fp, int &magic) {
    ostringstream err;
    int ndim, v, magic_vincent;
    int ndim_min = 3; // std header requires at least 3 dims even empty ones.
    idxdim dims;

    // read magic number
    if (fread(&magic, sizeof (int), 1, fp) != 1) {
      fclose(fp);
      err << "cannot read magic number";
      throw err.str();
    }
    magic_vincent = endian(magic);
    magic_vincent &= ~0xF; // magic contained in higher bits
    
    // read number of dimensions
    if (is_magic(magic)) { // regular magic number, read next number
      if (fread(&ndim, sizeof (int), 1, fp) != 1) {
	fclose(fp);
	err << "cannot read number of dimensions";
	throw err.str();
      }
      // check number is valid
      if (ndim > MAXDIMS) {
	err << "too many dimensions: " << ndim << " (MAXDIMS = "
	    << MAXDIMS << ").";
	fclose(fp);
	throw err.str();
      }
    } else if (is_magic_vincent(magic_vincent)) { // vincent magic number
      // ndim is contained in lower bits of the magic number
      ndim = endian(magic) & 0xF;
      ndim_min = ndim;
      magic = magic_vincent;
    } else { // unkown magic
      err << "unknown magic number: 0x" << std::hex << magic
	  << " or " << magic << " vincent: " << magic_vincent;
      fclose(fp);
      throw err.str();
    }
    // read each dimension
    for (int i = 0; (i < ndim) || (i < ndim_min); ++i) {
      if (fread(&v, sizeof (int), 1, fp) != 1) {
	fclose(fp);
	ostringstream oss;
	oss << "failed to read matrix dimensions";
	throw oss.str();
      }
      // if vincent, convert to endian first
      if (is_magic_vincent(magic_vincent))
	v = endian(v);
      if (i < ndim) { // ndim may be less than ndim_min
	if (v <= 0) { // check that dimension is valid
	  err << "dimension is negative or zero";
	  fclose(fp);
	  throw err.str();
	}
	dims.insert_dim(v, i); // insert dimension
      }
    }
    return dims;
  }
  
} // end namespace ebl
