/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#include <algorithm>
#include <stdio.h>
#include <inttypes.h>
#include <ostream>

#include "imageIO.h"
#include "idxops.h"
#include "idx.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////

  //! skip comments lines starting with <start>
  void skip_comments(ubyte start, istream &stream) {
    char c;
    for (;;) {
      stream.get(c);
      while (c == ' ' || c == '\n' || c == '\t' || c == '\r')
	stream.get(c);
      if (c != start)
	break;
      while (c != '\n')
	stream.get(c);
    }
    stream.unget();
  }

  //! Fills type and vmax if valid PNM file, and return dimensions.
  //! type = 6 if P6, 5 if P5, 4 if P4, -1 otherwise.
  idxdim read_pnm_header(istream &stream, int &type, int &vmax) {
    ostringstream err;
    string s;
    int ncol = -1, nlin = -1;
    
    type = -1;
    // read type
    stream >> s;
    if (stream.fail()) {
      err << "failed to read from stream";
      throw err.str();
    }
    if ((strcmp("P1", s.c_str()) == 0) ||
	(strcmp("P2", s.c_str()) == 0) ||
	(strcmp("P3", s.c_str()) == 0) ||
	(strcmp("P4", s.c_str()) == 0) ||
	(strcmp("P5", s.c_str()) == 0) ||
	(strcmp("P6", s.c_str()) == 0))
      type = (int) s[1] - '0';
    else {
      err << "invalid binary PNM file type: " << s;
      throw err.str();
    }
    // read columns
    skip_comments('#', stream); // skip comments
    stream >> ncol;
    if (stream.fail() || ncol <= 0) {
      err << "invalid PNM file: 0 columns";
      throw err.str();
    }
    // read lines
    skip_comments('#', stream); // skip comments
    stream >> nlin;
    if (stream.fail() || nlin <= 0) {
      err << "invalid PNM file: 0 lines";
      throw err.str();
    }
    // read vals
    skip_comments('#', stream); // skip comments
    stream >> vmax;
    if (stream.fail() || vmax <= 0) {
      err << "invalid PNM file: can't read maximum value";
      throw err.str();
    }
    if (vmax < 255)
      cout << "Warning: PNM values range lower than 255: " << vmax << endl;
    skip_comments('#', stream); // skip comments
    // return dimensions
    idxdim dims(nlin, ncol, 3);
    return dims;
  }

  idx<ubyte> pnm_read(istream &in, idx<ubyte> *out_){
    ostringstream err;
    int type, vmax;
    uint expected_size, read_size;
    idxdim dims = read_pnm_header(in, type, vmax);
    idx<ubyte> out;
    if (!out_) // allocate buffer if out is empty
      out = idx<ubyte>(dims);
    else
      out = *out_;
    idx_checkorder1(out, 3); // allow only 3D buffers
    // resize out if necessary
    if (out.get_idxdim() != dims)
      out.resize(dims);
    // sizes
    size_t sz = (vmax == 65535) ? 2 : 1;
    expected_size = dims.nelements();
    
    switch (type) {
    case 3: // PPM ASCII
      uint val;
      { idx_aloop1(o, out, ubyte) {
	in >> val;
	// downcasting automatically scales values if vmax > 255
	*o = (unsigned char) val;
	}}
      break ;
    case 6: // PPM binary
      if (out.contiguousp()) {
	if (sz == 2) { // 16 bits per pixel
	  idx<short> out2(dims);
	  in.read((char *) out2.idx_ptr(), sz * expected_size);
	  idx_copy(out2, out);
	} else // 8 bits per pixel
	  in.read((char *) out.idx_ptr(), sz * expected_size);
	read_size = in.gcount() / sz;
	if (expected_size != read_size)
	  // TODO: fixme, adding temporarly +1 to fix reading failures bug
	  //	    && (expected_size != read_size + 1))
	  {
	    cerr << "WARNING: image read: not enough items read. expected ";
	    cerr << expected_size;
	    cerr << " but found " << read_size << endl;
	  }
      } else {
	{ idx_aloop1(o, out, ubyte) {
	    in.get((char&)*o);
	  }}
      }
      break ;
      // TODO: implement pnm formats
      /*  case 4: // PBM image
	  ((= head "P4")
	  (let* ((ncol (fscan-int f))
	  (nlin (fscan-int f))
	  (n 0) (c 0))
	  ((-int-) ncol nlin n c)
	  (when (or (<> ncol ncolo) (<> nlin nlino) (< ncmpo 3))
	  (idx-u3resize out nlin ncol (max ncmpo 3)))
	  (getc f)
	  (cinline-idx2loop out "unsigned char" "p" "i" "j"
	  #{{ unsigned char v;
	  if ((j == 0) || ($n == 0)) { $c = getc((FILE *)$f); $n=0; }
	  v = ($c & 128) ? 0 : 255 ;
	  p[0]= p[1]= p[2]= v ;
	  $n = ($n == 7) ? 0 : $n+1 ;
	  $c = $c << 1;
	  } #} )))
	  // PGM image
	  ((= head "P5")
	  (let* ((ncol (fscan-int f))
	  (nlin (fscan-int f)))
	  ((-int-) ncol nlin)
	  (when (or (<> ncol ncolo) (<> nlin nlino) (< ncmpo 3))
	  (idx-u3resize out nlin ncol (max ncmpo 3)))
	  (fscan-int f)
	  (getc f)
	  (cinline-idx2loop out "unsigned char" "p" "i" "j"
	  #{ p[0]=p[1]=p[2]=getc((FILE *)$f) #}
	  )))
      */
    default:
      cerr << "Format P" << type << " not implemented." << endl;
    }
    return out;
  }

} // end namespace ebl
