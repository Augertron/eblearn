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

// endianess test
static int endiantest = 1;
#define LITTLE_ENDIAN_P (*(char*)&endiantest)

using namespace std;

namespace ebl {

// template functions returning the magic number associated with a particular type
template<class T>	inline int get_magic() { ylerror("matrix type not implemented."); return 0; }
template<> 				inline int get_magic<ubyte>() 	{ return MAGIC_BYTE_MATRIX; }
template<>   			inline int get_magic<int>() 		{ return MAGIC_INTEGER_MATRIX; }
template<>   			inline int get_magic<float>() 	{ return MAGIC_FLOAT_MATRIX; }
template<> 				inline int get_magic<double>() 	{ return MAGIC_DOUBLE_MATRIX; }
template<class T>	inline int get_magic_vincent() { ylerror("matrix type not implemented."); return 0; }
template<> 				inline int get_magic_vincent<ubyte>() 	{ return MAGIC_UBYTE_VINCENT; }
template<>   			inline int get_magic_vincent<int>() 		{ return MAGIC_INT_VINCENT; }
template<>   			inline int get_magic_vincent<float>() 	{ return MAGIC_FLOAT_VINCENT; }
template<> 				inline int get_magic_vincent<double>() 	{ return MAGIC_DOUBLE_VINCENT; }

// type to string function for debug message.
inline string get_magic_str(int magic) {
	switch (magic) {
	// standard format
	case MAGIC_BYTE_MATRIX: 		return "ubyte";
	case MAGIC_INTEGER_MATRIX: 	return "int";
	case MAGIC_FLOAT_MATRIX: 		return "float";
	case MAGIC_DOUBLE_MATRIX: 	return "double";
	// pascal vincent format
	case MAGIC_UBYTE_VINCENT: 	return "ubyte (pascal vincent)";
	case MAGIC_INT_VINCENT: 		return "int (pascal vincent)";
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

// TODO: intg support
// TODO: use c++ IO to catch IO exceptions more easily
// TODO: if types differ, print warning and cast to expected type
template<typename T> bool load_matrix(Idx<T>& m, const char *filename) {
	// open file
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		cerr << "load_matrix failed to open " << filename << "." << endl;
		return false;
	}
		
	int magic, ndim, v; 
	int ndim_min = 3; // the standard header requires at least 3 dimensions even empty ones.
	intg *dims = NULL;
	
	// header: read magic number
	fread(&magic, sizeof (int), 1, fp);
	int magic_vincent = endian(magic);
	ndim = endian(magic) & 0xF;
	magic_vincent &= ~0xF;
	if ((magic != get_magic<T>()) && (magic_vincent != get_magic_vincent<T>())) {
		cerr << "load_matrix failed (" << filename << "): ";
		cerr << get_magic_str(get_magic<T>()) << " expected, "; 
		cerr << get_magic_str(magic) << " found." << endl;
		return false;
	}
	// standard header
	if (magic == get_magic<T>()) {
		// read number of dimensions
		fread(&ndim, sizeof (int), 1, fp);
		if (ndim > MAXDIMS) {
			cerr << "load_matrix failed (" << filename << "): ";
			cerr << " too many dimensions: " << ndim << " (MAXDIMS = " << MAXDIMS << ")." << endl;
			return false;
		}
	}
	else if (magic_vincent == get_magic_vincent<T>()) {
		ndim_min = ndim;
	}
	if (ndim != m.order()) {
		cerr << "load_matrix failed (" << filename << "): ";
		cerr << "expected order of " << m.order() << " but found ";
		cerr << ndim << " in file." << endl;
		return false;
	}

	dims = (intg *) malloc(ndim * sizeof (intg));
	// header: read each dimension
	for (int i = 0; (i < ndim) || (i < ndim_min); ++i) {
		fread(&v, sizeof (int), 1, fp);
		if (magic_vincent == get_magic_vincent<T>())
			v = endian(v);
		if (i < ndim) {
			dims[i] = v;
			if (v <= 0) {
				cerr << "load_matrix failed (" << filename << "): ";
				cerr << " dimension is negative or 0." << endl;
				free(dims);
				return false;
			}
		}
	}
	// TODO: implement Idx constructor accepting array of dimensions and modify code below.
	m.resize(0 < ndim ? dims[0] : -1,
			  1 < ndim ? dims[1] : -1,
				2 < ndim ? dims[2] : -1,
				3 < ndim ? dims[3] : -1,
				4 < ndim ? dims[4] : -1,
				5 < ndim ? dims[5] : -1,
				6 < ndim ? dims[6] : -1,
				7 < ndim ? dims[7] : -1);
	// body
	{ idx_aloop1(i, m, T) fread(&(*i), sizeof (T), 1, fp); }		
	fclose(fp);
	free(dims);
	return true;
}

// TODO: intg support
// TODO: use c++ IO to catch IO exceptions more easily
template<typename T> bool save_matrix(Idx<T>& m, const char *filename) {
	int v, i;
	FILE *fp = fopen(filename, "wb");
	
	if (!fp) {
		cerr << "save_matrix failed (" << filename << ")." << endl;
		return false;
	}
	// header
	v = get_magic<T>();
	fwrite(&v, sizeof (int), 1, fp);
	v = m.order();
	fwrite(&v, sizeof (int), 1, fp);
	for (i = 0; (i < m.order()) || (i < 3); ++i) {
		if (i < m.order())
			v = m.dim(i);
		else
			v = 1;
		fwrite(&v, sizeof (int), 1, fp);
	}
	// body
	{ idx_aloop1(i, m, T) fwrite(&(*i), sizeof (T), 1, fp); }		
	fclose(fp);
	return true;
}

} // end namespace ebl

#endif /* IDXIO_HPP_ */
