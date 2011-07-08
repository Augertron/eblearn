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

#include "idxops.h"

// endianess test
static int endiantest = 1;
#define LITTLE_ENDIAN_P (*(char*)&endiantest)

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // helper functions

  // template functions returning the magic number associated with a 
  // particular type
  template <class T> inline int get_magic() {
    eblerror("matrix type not implemented."); return 0; }
  template <> inline int get_magic<ubyte>()       {return MAGIC_BYTE_MATRIX; }
  template <> inline int get_magic<int>()         {return MAGIC_INTEGER_MATRIX;}
  template <> inline int get_magic<float>()       {return MAGIC_FLOAT_MATRIX; }
  template <> inline int get_magic<double>()      {return MAGIC_DOUBLE_MATRIX; }
  template <> inline int get_magic<long>()        {return MAGIC_LONG_MATRIX; }
  template <> inline int get_magic<uint>()        {return MAGIC_UINT_MATRIX; }

  // Pascal Vincent type
  template <class T> inline int get_magic_vincent() {
    eblerror("matrix type not implemented"); return 0; }
  template <> inline int get_magic_vincent<ubyte>(){return MAGIC_UBYTE_VINCENT;}
  template <> inline int get_magic_vincent<int>()  {return MAGIC_INT_VINCENT;}
  template <> inline int get_magic_vincent<float>(){return MAGIC_FLOAT_VINCENT;}
  template <> inline int get_magic_vincent<double>() {
    return MAGIC_DOUBLE_VINCENT; }
  template <> inline int get_magic_vincent<long>() {return 0x0000; }

  // type to string function for debug message.
  inline std::string get_magic_str(int magic) {
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
  template <class T> inline void reverse_n(T *ptr, int n) {
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

  template <class T> inline T endian(T ptr) {
    T v = ptr;
    if (LITTLE_ENDIAN_P) reverse_n(&v, 1);
    return v;
  }

  template <typename T, typename T2>
  void read_cast_matrix(FILE *fp, idx<T2> &out) {
    idx<T> m(out.get_idxdim());
    read_matrix_body(fp, m);
    idx_copy(m, out);
  }

  template <typename T>
  void read_matrix_body(FILE *fp, idx<T> &m) {
    int res;
    idx_aloop1(i, m, T) {
      res = fread(&(*i), sizeof (T), 1, fp);
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // loading

  template <typename T> idx<T> load_matrix(const std::string &filename) {
    return load_matrix<T>(filename.c_str());
  }

  template <typename T> idx<T> load_matrix(const char *filename) {
    // open file
    FILE *fp = fopen(filename, "rb");
    if (!fp)
      eblthrow("load_matrix failed to open " << filename);
    // read it
    idx<T> m;
    try {
      m = load_matrix<T>(fp);
      fclose(fp);
    } catch(eblexception &e) {
      e << " while loading " << filename;
      throw e;
    }
    return m;
  }

  template <typename T>
  void load_matrix(idx<T>& m, const std::string &filename) {
    load_matrix(m, filename.c_str());
  }

  template <typename T> void load_matrix(idx<T>& m, const char *filename) {
    // open file
    FILE *fp = fopen(filename, "rb");
    if (!fp)
      eblthrow("load_matrix failed to open " << filename);
    // read it
    m = load_matrix<T>(fp, &m);
    fclose(fp);
  }

  template <typename T>
  idx<T> load_matrix(FILE *fp, idx<T> *out_) {
    int magic;
    idxdim dims = read_matrix_header(fp, magic);
    idx<T> out;
    idx<T> *pout = &out;
    if (!out_) // if no input matrix, allocate new one
      out = idx<T>(dims);
    else // otherwise use given one
      pout = out_;

    // resize out if necessary
    if (pout->get_idxdim() != dims) { // different order/dimensions
      // if order is different, it's from the input matrix, error
      if (pout->order() != dims.order())
	eblerror("error: different orders: " << *pout << " " << dims);
      // resize output idx
      pout->resize(dims);
    }
    //! if out matrix is same type as current, read directly
    if ((magic == get_magic<T>()) || (magic == get_magic_vincent<T>())) {
      // read
      read_matrix_body(fp, *pout);
    } else { // different type, read original type, then copy/cast into out
      switch (magic) {
      case MAGIC_BYTE_MATRIX:
      case MAGIC_UBYTE_VINCENT:
	read_cast_matrix<ubyte>(fp, *pout);
	break ;
      case MAGIC_INTEGER_MATRIX:
      case MAGIC_INT_VINCENT:
	read_cast_matrix<int>(fp, *pout);
	break ;
      case MAGIC_FLOAT_MATRIX:
      case MAGIC_FLOAT_VINCENT:
	read_cast_matrix<float>(fp, *pout);
	break ;
      case MAGIC_DOUBLE_MATRIX:
      case MAGIC_DOUBLE_VINCENT:
	read_cast_matrix<double>(fp, *pout);
	break ;
      case MAGIC_LONG_MATRIX:
	read_cast_matrix<long>(fp, *pout);
	break ;
      case MAGIC_UINT_MATRIX:
	read_cast_matrix<uint>(fp, *pout);
	break ;
      default:
	eblerror("unknown magic number");
      }
    }
    return *pout;
  }

  template <typename T>
  idxs<T> load_matrices(const std::string &filename){
    // open file
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp)
      eblthrow("load_matrix failed to open " << filename);
    // first read presence matrix
    idx<ubyte> p = load_matrix<ubyte>(fp);
    // read all present matrices
    idxs<T> all(p.dim(0));
    for (uint i = 0; i < p.dim(0); ++i) {
      if (p.get(i) == 1) { // matrix is present, get it
	idx<T> m = load_matrix<T>(fp);
	all.set(m, i);
      }
    }
    fclose(fp);
    return all;
  }
  
  ////////////////////////////////////////////////////////////////
  // saving
  
  template <typename T> bool save_matrix(idx<T>& m,
					 const std::string &filename) {
    return save_matrix(m, filename.c_str());
  }

  template <typename T2, typename T>
  bool save_matrix(idx<T>& m, const std::string &filename) {
    idx<T2> m2(m.get_idxdim());
    idx_copy(m, m2);
    return save_matrix(m2, filename.c_str());
  }

  // TODO: intg support
  template <typename T> bool save_matrix(idx<T>& m, const char *filename) {
    FILE *fp = fopen(filename, "wb");

    if (!fp) {
      cerr << "save_matrix failed (" << filename << ")." << endl;
      return false;
    }
    bool ret = save_matrix(m, fp);
    if (!ret)
      cerr << "failed to write matrix " << m << " to " << filename << "."
	   << endl;
    fclose(fp);
    return ret;
  }
  
  // TODO: intg support
  template <typename T> bool save_matrix(idx<T>& m, FILE *fp) {
    int v, i;
    // write header
    v = get_magic<T>();
    if (fwrite(&v, sizeof (int), 1, fp) != 1) return false;
    v = m.order();
    if (fwrite(&v, sizeof (int), 1, fp) != 1) return false;
    for (i = 0; (i < m.order()) || (i < 3); ++i) {
      if (i < m.order())
	v = m.dim(i);
      else
	v = 1;
      if (fwrite(&v, sizeof (int), 1, fp) != 1) return false;
    }
    // write body
    int res;
    idx_aloop1(k, m, T) 
      res = fwrite(&(*k), sizeof (T), 1, fp);
    return true;
  }

  template <typename T>
  bool save_matrices(idxs<T>& m, const std::string &filename) {
    FILE *fp = fopen(filename.c_str(), "wb");

    if (!fp) {
      cerr << "save_matrix failed (" << filename << ")." << endl;
      return false;
    }
    // convert pointers to presence/absence matrix
    idx<ubyte> p(m.dim(0));
    idx_clear(p);
    for (uint i = 0; i < m.dim(0); ++i) {
      if (m.exists(i))
	p.set((ubyte) 1, i);
    }
    // save presence matrix first    
    bool ret = save_matrix(p, fp);
    if (!ret) {
      cerr << "failed to write matrix " << p << " to " << filename << "."
	   << endl;
      fclose(fp);
      return false;
    }
    // then save all matrices contained in m
    for (uint i = 0; i < m.dim(0); ++i) {
      if (m.exists(i)) {
	idx<T> e = m.get(i);
	ret = save_matrix(e, fp);
	if (!ret) {
	  cerr << "failed to write matrix " << e << " to " << filename << "."
	       << endl;
	  fclose(fp);
	  return false;
	}
      }
    }
    fclose(fp);
    return ret;
  }
  
} // end namespace ebl

#endif /* IDXIO_HPP_ */
