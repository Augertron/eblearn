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

#ifndef IDXIO_HPP_
#define IDXIO_HPP_

#include <stdio.h>
#include <sstream>

// endianess test
static int endiantest = 1;
#define LITTLE_ENDIAN_P (*(char*)&endiantest)

using namespace std;

namespace ebl {
  
  // template functions returning the magic number associated with a 
  // particular type
  template<class T> inline int get_magic() 
  { eblerror("matrix type not implemented."); return 0; }
  template<> inline int get_magic<ubyte>()        {return MAGIC_BYTE_MATRIX; }
  template<> inline int get_magic<int>()          {return MAGIC_INTEGER_MATRIX;}
  template<> inline int get_magic<float>()        {return MAGIC_FLOAT_MATRIX; }
  template<> inline int get_magic<double>()       {return MAGIC_DOUBLE_MATRIX; }
  template<> inline int get_magic<long>()         {return MAGIC_LONG_MATRIX; }
  template<> inline int get_magic<uint>()         {return MAGIC_UINT_MATRIX; }

  // Pascal Vincent type
  template<class T> inline int get_magic_vincent() 
  { eblerror("matrix type not implemented"); return 0; }
  template<> inline int get_magic_vincent<ubyte>(){return MAGIC_UBYTE_VINCENT;}
  template<> inline int get_magic_vincent<int>()  {return MAGIC_INT_VINCENT;}
  template<> inline int get_magic_vincent<float>(){return MAGIC_FLOAT_VINCENT;}
  template<> inline int get_magic_vincent<double>()
  {return MAGIC_DOUBLE_VINCENT; }
  template<> inline int get_magic_vincent<long>() {return 0x0000; }

  // type to string function for debug message.
  inline string get_magic_str(int magic) {
    switch (magic) {
      // standard format
    case MAGIC_BYTE_MATRIX: 	return "ubyte";
    case MAGIC_INTEGER_MATRIX: 	return "int";
    case MAGIC_FLOAT_MATRIX: 	return "float";
    case MAGIC_DOUBLE_MATRIX: 	return "double";
    case MAGIC_LONG_MATRIX:	return "long";
      // non standard
    case MAGIC_UINT_MATRIX: 	return "uint";
      // pascal vincent format
    case MAGIC_UBYTE_VINCENT: 	return "ubyte (pascal vincent)";
    case MAGIC_INT_VINCENT: 	return "int (pascal vincent)";
    case MAGIC_FLOAT_VINCENT: 	return "float (pascal vincent)";
    case MAGIC_DOUBLE_VINCENT: 	return "double (pascal vincent)";
    default: return "unknown type";
    }
  }

  /*! Reverses order of bytes in <n> items of size <sz>
    starting at memory location <ptr>
    This is a tool for writing/reading file formats that are portable
    across systems with processors that represent long-words
    differently in memory (Sparc vs Intel-Pentium for ex.)
    It can be called from inline-C as C_reverse8(ptr,n);

    ptr: pointer to the block of memory that must be reversed.
    n: number of elements to reverse. */
  template<class T> inline void reverse_n(T *ptr, int n) {
    char *mptr = (char *) ptr;
    while(n--)
      {
	char tmp;
	char *uptr = mptr + sizeof (T);
	if (sizeof (T) >= 2)
	  { tmp = mptr[0]; mptr[0]=uptr[-1]; uptr[-1]=tmp; }
	if (sizeof (T) >= 4)
	  { tmp = mptr[1]; mptr[1]=uptr[-2]; uptr[-2]=tmp; }
	if (sizeof (T) >= 6)
	  { tmp = mptr[2]; mptr[2]=uptr[-3]; uptr[-3]=tmp; }
	if (sizeof (T) >= 8)
	  { tmp = mptr[3]; mptr[3]=uptr[-4]; uptr[-4]=tmp; }
	mptr = uptr;
      }
  }

  template<class T> inline T endian(T ptr) {
    T v = ptr;
    if (LITTLE_ENDIAN_P) reverse_n(&v, 1);
    return v;
  }

  template<typename T> bool load_matrix(idx<T>& m, const string &filename) {
    return load_matrix(m, filename.c_str());
  }

  // TODO: use c++ IO to catch IO exceptions more easily
  // TODO: if types differ, print warning and cast to expected type
  // TODO: allow not knowing order in advance (just assign new idx to m)
  template<typename T> bool load_matrix(idx<T>& m, const char *filename) {
    // open file
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
      ostringstream oss;
      oss << "load_matrix failed to open " << filename;
      cerr << oss.str() << endl;
      throw oss.str();
      return false;
    }

    int magic, ndim, v;
    int ndim_min = 3; // std header requires at least 3 dims even empty ones.
    intg *dims = NULL;

    // header: read magic number
    if (fread(&magic, sizeof (int), 1, fp) != 1) {
      fclose(fp);
      ostringstream oss;
      oss << "failed to read magic number in " << filename;
      throw oss.str();
      return false;
    }
    int magic_vincent = endian(magic);
    ndim = endian(magic) & 0xF;
    magic_vincent &= ~0xF;
    if ((magic != get_magic<T>()) && 
	(magic_vincent != get_magic_vincent<T>())) {
      fclose(fp);
      ostringstream oss;
      oss << "failed to read matrix (" << filename << "): ";
      oss << get_magic_str(get_magic<T>()) << " expected, ";
      oss << get_magic_str(magic) << " found.";
      throw oss.str();
      return false;
    }
    // standard header
    if (magic == get_magic<T>()) {
      // read number of dimensions
      if (fread(&ndim, sizeof (int), 1, fp) != 1) {
	fclose(fp);
	ostringstream oss;
	oss << "failed to read data in " << filename;
	throw oss.str();
	return false;
      }
      if (ndim > MAXDIMS) {
	fclose(fp);
	ostringstream oss;
	oss << "failed to load matrix " << filename << ": ";
	oss << " too many dimensions: " << ndim << " (MAXDIMS = ";
	oss << MAXDIMS << ")." << endl;
	throw oss.str();
	return false;
      }
    }
    else if (magic_vincent == get_magic_vincent<T>()) {
      ndim_min = ndim;
    }
    if (ndim != m.order()) {
      fclose(fp);
      ostringstream oss;
      oss << "failed to load matrix " << filename << ": ";
      oss << "expected order of " << m.order() << " but found ";
      oss << ndim << " in file." << endl;
      throw oss.str();
      return false;
    }

    dims = (intg *) malloc(ndim * sizeof (intg));
    // header: read each dimension
    for (int i = 0; (i < ndim) || (i < ndim_min); ++i) {
      if (fread(&v, sizeof (int), 1, fp) != 1) {
	fclose(fp);
	ostringstream oss;
	oss << "failed to read matrix dimensions in " << filename;
	throw oss.str();
	return false;
      }
      if (magic_vincent == get_magic_vincent<T>())
	v = endian(v);
      if (i < ndim) {
	dims[i] = v;
	if (v <= 0) {
	  free(dims);
	  fclose(fp);
	  ostringstream oss;
	  oss << "failed to read matrix dimensions in " << filename << ": ";
	  oss << " dimension is negative or 0." << endl;
	  throw oss.str();
	  return false;
	}
      }
    }
    // TODO: implement idx constructor accepting array of dimensions 
    // and modify code below.
    m.resize(0 < ndim ? dims[0] : -1,
	     1 < ndim ? dims[1] : -1,
	     2 < ndim ? dims[2] : -1,
	     3 < ndim ? dims[3] : -1,
	     4 < ndim ? dims[4] : -1,
	     5 < ndim ? dims[5] : -1,
	     6 < ndim ? dims[6] : -1,
	     7 < ndim ? dims[7] : -1);
    // body
    int res;
    { idx_aloop1(i, m, T) 
	res = fread(&(*i), sizeof (T), 1, fp); }
    fclose(fp);
    free(dims);
    return true;
  }

  template<typename T> bool load_matrix(idx<T>& m, istream &stream) {
    int magic, ndim, v;
    int ndim_min = 3; // std header requires at least 3 dims even empty ones.
    intg *dims = NULL;

    // header: read magic number
    stream.read((char*)&magic, sizeof (int));
    int magic_vincent = endian(magic);
    ndim = endian(magic) & 0xF;
    magic_vincent &= ~0xF;
    if ((magic != get_magic<T>()) 
	&& (magic_vincent != get_magic_vincent<T>())) {
      cerr << "load_matrix failed : ";
      cerr << get_magic_str(get_magic<T>()) << " expected, ";
      cerr << get_magic_str(magic) << " found." << endl;
      return false;
    }
    // standard header
    if (magic == get_magic<T>()) {
      // read number of dimensions
      stream.read((char*)&ndim, sizeof (int));
      if (ndim > MAXDIMS) {
	cerr << "load_matrix failed : ";
	cerr << " too many dimensions: " << ndim << " (MAXDIMS = ";
	cerr << MAXDIMS << ")." << endl;
	return false;
      }
    }
    else if (magic_vincent == get_magic_vincent<T>()) {
      ndim_min = ndim;
    }
    if (ndim != m.order()) {
      cerr << "load_matrix failed : ";
      cerr << "expected order of " << m.order() << " but found ";
      cerr << ndim << " in file." << endl;
      return false;
    }

    dims = (intg *) malloc(ndim * sizeof (intg));
    // header: read each dimension
    for (int i = 0; (i < ndim) || (i < ndim_min); ++i) {
      stream.read((char*)&v, sizeof (int));
      if (magic_vincent == get_magic_vincent<T>())
	v = endian(v);
      if (i < ndim) {
	dims[i] = v;
	if (v <= 0) {
	  cerr << "load_matrix failed : ";
	  cerr << " dimension is negative or 0." << endl;
	  free(dims);
	  return false;
	}
      }
    }
    // TODO: implement idx constructor accepting array of dimensions 
    // and modify code below.
    m.resize(0 < ndim ? dims[0] : -1,
	     1 < ndim ? dims[1] : -1,
	     2 < ndim ? dims[2] : -1,
	     3 < ndim ? dims[3] : -1,
	     4 < ndim ? dims[4] : -1,
	     5 < ndim ? dims[5] : -1,
	     6 < ndim ? dims[6] : -1,
	     7 < ndim ? dims[7] : -1);
    // body
    { idx_aloop1(i, m, T) stream.read((char*)&(*i), sizeof (T)); }
    free(dims);
    return true;
  }

  template<typename T> bool save_matrix(idx<T>& m, const string &filename) {
    return save_matrix(m, filename.c_str());
  }

  // TODO: intg support
  // TODO: use c++ IO to catch IO exceptions more easily
  template<typename T> bool save_matrix(idx<T>& m, const char *filename) {
    int v, i;
    FILE *fp = fopen(filename, "wb");

    if (!fp) {
      cerr << "save_matrix failed (" << filename << ")." << endl;
      return false;
    }
    // header
    v = get_magic<T>();
    if (fwrite(&v, sizeof (int), 1, fp) != 1) {
      cerr << "failed to write to " << filename << "." << endl;
      fclose(fp);
      return false;
    }
    v = m.order();
    if (fwrite(&v, sizeof (int), 1, fp) != 1) {
      cerr << "failed to write to " << filename << "." << endl;
      fclose(fp);
      return false;
    }
    for (i = 0; (i < m.order()) || (i < 3); ++i) {
      if (i < m.order())
	v = m.dim(i);
      else
	v = 1;
      if (fwrite(&v, sizeof (int), 1, fp) != 1) {
	cerr << "failed to write to " << filename << "." << endl;
	fclose(fp);
	return false;
      }
    }
    // body
    int res;
    { idx_aloop1(i, m, T) 
	res = fwrite(&(*i), sizeof (T), 1, fp); }
    fclose(fp);
    return true;
  }

  template<typename T> bool save_matrix(idx<T>& m, ostream &stream) {
    int v, i;

    // header
    v = get_magic<T>();
    stream.write((char*)&v, sizeof (int));
    v = m.order();
    stream.write((char*)&v, sizeof (int));
    for (i = 0; (i < m.order()) || (i < 3); ++i) {
      if (i < m.order())
	v = m.dim(i);
      else
	v = 1;
      stream.write((char*)&v, sizeof (int));
    }
    // body
    { idx_aloop1(i, m, T) stream.write((char*)&(*i), sizeof (T)); }
    return true;
  }

} // end namespace ebl

#endif /* IDXIO_HPP_ */
