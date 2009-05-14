/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#include "image.h"
#include "blas.h"
#include "idx.h"

using namespace std;

namespace ebl {

  bool collide_rect(int x1, int y1, int w1, int h1,
		    int x2, int y2, int w2, int h2) {
    int x,y,w,h;
    x = max(x1,x2);
    y = max(y1,y2);
    w = min(x1+w1,x2+w2)-x;
    h = min(y1+h1,y2+h2)-y;
    if (w>0 && h>0)
      return true;
    else
      return false;
  }

  double common_area(int x1, int y1, int w1, int h1,
		     int x2, int y2, int w2, int h2) {
    int x,y,w,h;
    x = max(x1,x2);
    y = max(y1,y2);
    w = min(x1+w1,x2+w2)-x;
    h = min(y1+h1,y2+h2)-y;
    if((w <= 0)||(h <= 0)) return 0;
    else return (double)((w*h)/(w1*h1));

  }

  ////////////////////////////////////////////////////////////////

  //! skip comments lines starting with <start>
  FILE* skip_comments(ubyte start, FILE *fp) {
    if (!fp) {
      cerr << "file is not opened" << endl;
      return NULL;
    }
    int c;
    for (	;;) {
      c = getc(fp);
      while (c==' ' || c=='\n' || c=='\t' || c=='\r') c = getc(fp);
      if (c != start) break;
      while (c != '\n')  c = getc(fp);
    }
    ungetc(c, fp);
    return fp;
  }

  //! Fills type, ncol and nlin if valid PNM file.
  //! type = 6 if P6, 5 if P5, 4 if P4, -1 otherwise.
  //! This function returns false upon errors in the header.
  bool pnm_header(FILE * fp, int *type, int *ncol, int *nlin, int *vmax) {
    if (!fp) return false;
    rewind(fp);
    char buf[128];
    memset(buf, 0, sizeof (buf));
    if (fscanf(fp, "%s\n", buf) == 0) {
      cerr << "invalid PNM file." << endl; return false; }
    *type = -1;
    if ((strcmp("P1", buf) == 0) ||
	(strcmp("P2", buf) == 0) ||
	(strcmp("P3", buf) == 0) ||
	(strcmp("P4", buf) == 0) ||
	(strcmp("P5", buf) == 0) ||
	(strcmp("P6", buf) == 0))
      *type = (int) buf[1] - '0';
    else {
      cerr << "invalid binary PNM file." << endl;
      return false;
    }
    fp = skip_comments(35, fp);
    if (fscanf(fp, "%d", ncol) == 0) {
      cerr << "invalid PNM file." << endl; return false; }
    fp = skip_comments(35, fp);
    if (fscanf(fp, "%d", nlin) == 0) {
      cerr << "invalid PNM file." << endl; return false; }
    fp = skip_comments(35, fp);
    if (fscanf(fp, "%d\n", vmax) == 0) {
      cerr << "invalid PNM file." << endl; return false; }
    return true;
  }

  bool pnm_fread_into_rgbx(const char* fname, idx<ubyte> &out){
    FILE *fp = fopen(fname, "r");
    if (!fp) {
      cerr << "failed to open " << fname << endl;
      return false;
    }
    bool bla =  pnm_fread_into_rgbx(fp, out);
    fclose(fp);
    return bla;
  }

  bool pnm_fread_into_rgbx(FILE *fp, idx<ubyte> &out) {
    idx_checkorder1(out, 3);

    int type, ncol, nlin, ncolo, nlino, ncmpo, vmax;
    unsigned int expected_size, read_size;
    if (!pnm_header(fp, &type, &ncol, &nlin, &vmax))
      return false;
    // TODO: allow PNM > 255 when idx out is int
    if (vmax > 255) {
      cerr << "values range from 0 to " << vmax;
      cerr << ". Only ubyte are supported (0 .. 255)." << endl;
      return false;
    }
    if (vmax != 255)
      cout << "Warning: PNM values range lower than 255: " << vmax << endl;
    nlino = out.dim(0);
    ncolo = out.dim(1);
    ncmpo = out.dim(2);
    expected_size = ncol * nlin;
    if ((ncol != ncolo) || (nlin != nlino) || (ncmpo < 3))
      out.resize(nlin, ncol, max(ncmpo, 3));
    switch (type) {
    case 3: // PPM ASCII
      int res;
      unsigned int val;
      { idx_aloop1(o, out, ubyte) {
  	  res = fscanf(fp, "%u", &val);
	  *o = (unsigned char) val;
	}}
      break ;
    case 6: // PPM binary
      if ((3 == out.dim(2)) && out.contiguousp()) {
	read_size = fread(out.idx_ptr(), 3, expected_size, fp);
	if ((expected_size != read_size) &&
	    // TODO: fixme, adding temporarly +1 to fix reading failures bug
	    (expected_size != read_size + 1)) {
	  cerr << "image read: not enough items read. expected ";
	  cerr << expected_size;
	  cerr << " but found " << read_size << endl;
	  return false;
	}
      } else {
	{ idx_aloop1(o, out, ubyte) {
	    *o = getc(fp);
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
    return true;
  }


  bool image_read_rgbx(const char *fname, idx<ubyte> &out){

    /* TODO : add a line in the config to detect "convert" path
       char myconvert[100];
       FILE* bla = popen("which convert", "r");
       fgets(myconvert, 100, bla);
       pclose(bla);
       *(myconvert+16) = 0;
       */
    string myconvert = "/usr/bin/convert";
    if(strstr(myconvert.c_str(), "convert") == NULL){
      cerr << "failed to find \"convert\", please install ImageMagick" << endl;
      return false;
    }
    char *mycommand = new char[myconvert.size() + strlen(fname) + 50];
    sprintf(mycommand, "%s %s PPM:-", myconvert.c_str(), fname);
    FILE* fp = popen(mycommand, "r");
    delete mycommand;

    if (!fp) {
      cerr << "failed to open " << fname << endl;
      return false;
    }
    bool ret =  pnm_fread_into_rgbx(fp, out);
    pclose(fp);
    fp = (FILE*)NULL;
    return ret;

  }
  ////////////////////////////////////////////////////////////////

  void image_interpolate_bilin(ubyte* background, ubyte *pin,
			       int indimi, int indimj, int inmodi, int inmodj,
			       int ppi, int ppj,
			       ubyte* out, int outsize) {
    int li0, lj0;
    register int li1, lj1;
    int deltai, ndeltai;
    int deltaj, ndeltaj;
    register ubyte *pin00;
    register ubyte *v00, *v01, *v10, *v11;
    li0 = ppi >> 16;
    li1 = li0+1;
    deltai = ppi & 0x0000ffff;
    ndeltai = 0x00010000 - deltai;
    lj0 = ppj  >> 16;
    lj1 = lj0+1;
    deltaj = ppj & 0x0000ffff;
    ndeltaj = 0x00010000 - deltaj;
    pin00 = (ubyte*)(pin) + inmodi * li0 + inmodj * lj0;
    if ((li1>0)&&(li1<indimi)) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==0) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = (pin00+inmodi);
      } else if (lj1==0) {
	v00 = background;
	v01 = background;
	v11 = (pin00+inmodi+inmodj);
	v10 = background;
      } else if (lj1==indimj) {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = (pin00+inmodi);
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else if (li1==indimi) {
      if ((lj1>0)&&(lj1<indimj)) {
	v00 = (pin00);
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==0) {
	v00 = background;
	v01 = (pin00+inmodj);
	v11 = background;
	v10 = background;
      } else if (lj1==indimj) {
	v00 = (pin00);
	v01 = background;
	v11 = background;
	v10 = background;
      } else {
	v00 = background;
	v01 = background;
	v11 = background;
	v10 = background;
      }
    } else {
      v00 = background;
      v01 = background;
      v11 = background;
      v10 = background;
    }
    if (outsize == 1)
      *out = (ndeltaj * (( *v10*deltai + *v00*ndeltai )>>16) +
	      deltaj  * (( *v11*deltai + *v01*ndeltai )>>16))>>16;
    else {
      if (outsize >= 3) {
	*out = (ndeltaj * (( v10[0]*deltai + v00[0]*ndeltai )>>16) +
		deltaj  * (( v11[0]*deltai + v01[0]*ndeltai )>>16))>>16;
	*(out + 1) = (ndeltaj * (( v10[1]*deltai + v00[1]*ndeltai )>>16) +
		      deltaj  * (( v11[1]*deltai + v01[1]*ndeltai )>>16))>>16;
	*(out + 2) = (ndeltaj * (( v10[2]*deltai + v00[2]*ndeltai )>>16) +
		      deltaj  * (( v11[2]*deltai + v01[2]*ndeltai )>>16))>>16;
      }
      if (outsize >= 4) {
	*(out + 3) = (ndeltaj * (( v10[3]*deltai + v00[3]*ndeltai )>>16) +
		      deltaj  * (( v11[3]*deltai + v01[3]*ndeltai )>>16))>>16;
      }
    }
  }

  /////////////////////////////////////////////////////////////////

  void image_rotscale_rect(int w, int h, double cx, double cy,
			   double angle, double coeff, idx<intg> &wh,
			   idx<double> &cxcy){
    double sa = sin(0.017453292*angle);
    double ca = cos(0.017453292*angle);
    double x1 = 0;
    double y1 = 0;
    double cw = coeff * w;
    double ch = coeff * h;
    double x2 = cw * ca;
    double y2 = cw * sa;
    double x4 = -(ch * sa);
    double y4 = ch * ca;
    double x3 = x2 + x4;
    double y3 = y2 + y4;
    double dcx = coeff * ( cx*ca - cy*sa);
    double dcy = coeff * ( cx*sa + cy*ca);
    double lx = min(min(x1, x2), min(x3, x4));
    double ly = min(min(y1, y2), min(y3, y4));
    double rx = max(max(x1, x2), max(x3, x4));
    double ry = max(max(y1, y2), max(y3, y4));
    wh.set((intg)(1 + rx - lx), 0);
    wh.set((intg)(1 + ry - ly), 1);
    cxcy.set(dcx - lx, 0);
    cxcy.set(dcy - ly, 1);
  }

} // end namespace ebl
