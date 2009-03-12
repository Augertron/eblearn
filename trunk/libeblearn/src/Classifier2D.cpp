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

#include "Classifier2D.h"
#include "Image.h"
#include <algorithm>

using namespace std;

unsigned int xw = 0; // global drawing coordinates
unsigned int yh = 0; // global drawing coordinates

namespace ebl {

  Classifier2D::Classifier2D(const char *paramfile, Idx<int> &sz, 
			     Idx<const char*> &lbls,
			     double b, double c, int h, int w,
			     int nn_h_, int nn_w_) 
    : nn_h(nn_h_), nn_w(nn_w_) {
    theparam = new parameter(60000);
    thenet = new lenet7(*theparam, nn_h, nn_w);
    theparam->load(paramfile);
    height = h;
    width = w;
    grabbed = Idx<ubyte>(height, width);
    bias = b;
    coeff = c;
    contrast = 1;
    brightness = 0;
    sizes = sz;
    labels = lbls;
    // initialize input and output states and result matrices for each size
    inputs = Idx<void*>(sizes.nelements());
    outputs = Idx<void*>(sizes.nelements());
    results = Idx<void*>(sizes.nelements());
    { idx_bloop4(size, sizes, int, in, inputs, void*, out, outputs, void*,
		 r, results, void*) {
	in.set((void*) new state_idx(2, 84 + (size.get() * 12), 
				     84 + (size.get() * 12)));
	out.set((void*) new state_idx(lbls.nelements(), size.get(), 
				      size.get()));
	r.set((void*) new Idx<double>(size.get(), 
				      size.get(), 2)); // (class,score)
      }}
    double sker[3][3] = {{0.3, 0.5, 0.3}, {0.5, 1, 0.5}, {0.3, 0.5, 0.3}};
    smoothing_kernel = Idx<double>(3, 3);
    memcpy(smoothing_kernel.idx_ptr(), sker, sizeof (sker));
  }

  Classifier2D::~Classifier2D() {
    delete theparam;
    delete thenet;
    { idx_bloop3(in, inputs, void*, out, outputs, void*, r, results, void*) {
	delete((state_idx*) in.get());
	delete((state_idx*) out.get());
	delete((Idx<double>*) r.get());
      }}
  }

  void Classifier2D::mark_maxima(Idx<double> &in, Idx<double> &inc, 
				 Idx<double> &res, double threshold) {
    idx_clear(res);
    int tr[] = { 1, 2, 0 };
    Idx<double> s(inc.transpose(tr));
    Idx<double> z(in.transpose(tr));
    z = z.unfold(0, 3, 1);
    z = z.unfold(1, 3, 1);
    { idx_bloop3(se, s, double, ze, z, double, re, res, double) {
	{ idx_bloop3(see, se, double, zee, ze, double, ree, re, double)  {
	    // find winning class
	    intg w = idx_indexmax(see);
	    double c = see.get(w);
	    // look if above threshold and local maximum
	    ree.set(-1.0, 0),
	      ree.set(-100.0, 1);
	    if ((c > threshold) &&
		(c > zee.get(w, 0, 0)) && (c > zee.get(w, 0, 1)) 
		&& (c > zee.get(w, 0, 2)) &&
		(c > zee.get(w, 1, 0)) && (c > zee.get(w, 1, 2)) &&
		(c > zee.get(w, 2, 0)) && (c > zee.get(w, 2, 1)) 
		&& (c > zee.get(w, 2, 2))) {
	      ree.set(w, 0);
	      ree.set(c, 1);
	    }
	  }}
      }}
  }

  // produce a score between 0 and 1 for each class at each location
  Idx<double> Classifier2D::postprocess_output(double threshold, int objsize) {
    // find candidates at each scale
    { idx_bloop2(out, outputs, void*, resu, results, void*) {
	Idx<double> outx = ((state_idx*) out.get())->x;
	int c = outx.dim(0);
	int h = outx.dim(1);
	int w = outx.dim(2);
	Idx<double> in(c, 2 + w, 2 + h);
	Idx<double> inc(in);
	inc = inc.narrow(1, w, 1);
	inc = inc.narrow(2, h, 1);
	Idx<double> m(c, h, w);
	idx_fill(in, 0.0);
	idx_clip(outx, 0.0, inc);
	// smooth
	{ idx_bloop2(ie, in, double, me, m, double) {
	    idx_2dconvol(ie, smoothing_kernel, me);
	  }}
	idx_copy(m, inc);
	// find points that are local maxima spatial and class-wise
	// write result in m. rescale result to [0 1]
	mark_maxima(in, inc, *((Idx<double>*) resu.get()), threshold);
      }}
    // now prune between scale and make a list
    Idx<double> rlist(1, 6);
    rlist.resize(0, rlist.dim(1));
    Idx<double> in0x(((state_idx*) inputs.get(0))->x);
    intg s0i = in0x.dim(1);
    intg s0j = in0x.dim(2);
    { idx_bloop3(input, inputs, void*, output, outputs, void*, 
		 r, results, void*) {    	 
	Idx<double> inx(((state_idx*) input.get())->x);
	intg sj = inx.dim(2);
	int i = 0, j = 0;
	double scale = s0j / (double) sj;
	{ idx_bloop1(re, *((Idx<double>*) r.get()), double) {
	    j = 0;
	    double scalewidth = s0i / (double) (scale * (nn_h + re.dim(0) - 1));
	    { idx_bloop1(ree, re, double) {
		if (ree.get(1) > threshold) {
		  intg ri = rlist.dim(0);
		  rlist.resize(ri + 1, rlist.dim(1));
		  rlist.set(ree.get(0), ri, 0);
		  rlist.set(ree.get(1), ri, 1);
		  rlist.set((int) (i * scalewidth) + nn_h/2, ri, 2);
		  rlist.set((int) (j * scalewidth) + nn_w/2, ri, 3);
		  rlist.set(nn_h * scale, ri, 4);
		  rlist.set(nn_w * scale, ri, 5);
		}
		j++;
	      }}
	    i++;
	  }}
      }}
    return rlist;
  }

  Idx<ubyte> Classifier2D::multi_res_prep(ubyte *img, float zoom) {
    // copy input images locally
    memcpy(grabbed.idx_ptr(), img, height * width * sizeof (ubyte));
#ifdef __GUI__
    gui.new_window("Classifier2D");
    gui.draw_matrix(grabbed);
    xw = grabbed.dim(1) + 2;
    yh = 0;
#endif
    // prepare multi resolutions input
    Idx<double> inx;
    int ni = ((state_idx*) inputs.get(0))->x.dim(1);
    int nj = ((state_idx*) inputs.get(0))->x.dim(2);
    int zi = max(ni, (int) (zoom * grabbed.dim(0)));
    int zj = max(nj, (int) (zoom * grabbed.dim(1)));
    int oi = (zi - ni) / 2;
    int oj = (zj - nj) / 2;
    Idx<ubyte> im = image_resize(grabbed, zj, zi, 1);
    im = im.narrow(0, ni, oi);
    im = im.narrow(1, nj, oj);
    // for display
    idx_clear(grabbed);
    Idx<ubyte> display(grabbed.narrow(0, im.dim(0), 0));
    display = display.narrow(1, im.dim(1), 0);
    idx_copy(im, display);
    { idx_bloop1(in, inputs, void*) {
  	inx = ((state_idx*) in.get())->x;
  	ni = inx.dim(1);
  	nj = inx.dim(2);
  	Idx<ubyte> imres = image_resize(im, nj, ni, 1);
  	Idx<double> inx0 = inx.select(0, 0);
  	Idx<double> inx1 = inx.select(0, 1);
  	idx_copy(imres, inx0);
  	idx_copy(imres, inx1);
  	idx_addc(inx, bias, inx);
  	idx_dotc(inx, coeff, inx);
#ifdef __GUI__
	gui.draw_matrix(imres, yh, xw);
	xw += imres.dim(1) + 2;
#endif
      }}
    xw = 0;
    yh += grabbed.dim(0) + 2;
    return display;
  }

  Idx<double> Classifier2D::multi_res_fprop(double threshold, int objsize) {
    // fprop network on different resolutions
    int hmax = ((state_idx*) outputs.get(0))->x.dim(1);
    int show = 0, k = 0;
    { idx_bloop2(in, inputs, void*, out, outputs, void*) {
	state_idx *ii = ((state_idx*) in.get());
	state_idx *oo = ((state_idx*) out.get());
	thenet->fprop(*ii, *oo); 
#ifdef __GUI__
	unsigned yhh = yh + grabbed.dim(0) + 2;
	if (k == show)
	  thenet->display_fprop(*ii, *oo, yhh, xw, 1.0, true);
 	double vmin = idx_min(oo->x);
	double vmax = idx_max(oo->x);
	int hcat = 0;
	double zoom = 5.0;
	{ idx_bloop1(category, oo->x, double) {
	    gui.draw_matrix(category, yh + hcat, xw, vmin, vmax, 
				     zoom, zoom);
	    hcat += hmax * zoom + 2;
	  }}
	xw += oo->x.dim(2) * zoom + 2;
#endif	
	k++;
      }}
    xw += 10;
    // post process outputs
    Idx<double> res = postprocess_output(threshold, objsize);
    res = prune(res);
    cout << " results: ";
    { idx_bloop1(re, res, double) {
	re.printElems();
	cout << " " << labels.get((int)re.get(0));
      }}
    if (res.dim(0) == 0) cout << "no object found.";
    return res;
  }

  Idx<double> Classifier2D::prune(Idx<double> &res) {
    // prune a list of detections.
    // only keep the largest scoring within an area
    Idx<double> rlist(1, res.dim(1));
    rlist.resize(0, rlist.dim(1));
    { idx_bloop1(re, res, double) {
	double score = re.get(1);
	int px = (int) re.get(2);
	int py = (int) re.get(3);
	int bx = (int) re.get(4);
	int by = (int) re.get(5);
	// if its center is inside the box of a higher-scoring obj, kill it
	bool ok = true;
	{ idx_bloop1(o, res, double) {
	    double scoreo = o.get(1);
	    int pxo = (int) o.get(2);
	    int pyo = (int) o.get(3);
	    int bxo = (int) o.get(4);
	    int byo = (int) o.get(5);
	    if ((score < scoreo) && 
		collide_rect((int) (px - 0.5 * bx), (int) (py - 0.5 * by), 
			     bx, by, (int) (pxo - (0.5 * bxo)),	
			     (int) (pyo - (0.5 * byo)), bxo, byo))
	      ok = false;
	  }}
	if (ok) {
	  intg ri = rlist.dim(0);
	  rlist.resize(ri + 1, rlist.dim(1));
	  Idx<double> out(rlist.select(0, ri));
	  idx_copy(re, out);
	}
      }}
    return rlist;
  }

  Idx<double> Classifier2D::fprop(ubyte *img, float zoom, double threshold, 
				  int objsize) {
    memcpy(grabbed.idx_ptr(), img, height * width * sizeof (ubyte));
    Idx<ubyte> display = this->multi_res_prep(img, 0.5);
    Idx<double> res = this->multi_res_fprop(threshold, objsize);
    { idx_bloop1(re, res, double) {
	unsigned int h = zoom * (re.get(2) - (0.5 * re.get(4)));
	unsigned int w = zoom * (display.dim(1) - re.get(3) 
				 - (0.5 * re.get(5)));
	unsigned int height = zoom * re.get(4);
	unsigned int width = zoom * re.get(5);
#ifdef __GUI__
	gui << at(yh + h + 1, xw + w + 1) << labels.get((re.get(0)));
#endif
	image_draw_box(display, (ubyte)255, h, w, height, width);
      }}
    memcpy(img, grabbed.idx_ptr(), height * width * sizeof (ubyte));
#ifdef __GUI__
    gui.draw_matrix(grabbed, yh, xw);
    xw += grabbed.dim(1) + 2;
#endif
    return res;
  }

  ////////////////////////////////////////////////////////////////

  Classifier2DBinoc::Classifier2DBinoc(const char *paramfile, 
				       Idx<int> &sz, Idx<const char*> &lbls,
				       double b, double c, int h, int w)
    : Classifier2D(paramfile, sz, lbls, b, c, h, w) {
    grabbed2 = Idx<ubyte>(height, width);
  }

  Classifier2DBinoc::~Classifier2DBinoc() {
  }

  void Classifier2DBinoc::multi_res_prep(ubyte *left, ubyte *right, 
					 int dx, int dy, float zoom) {
    // display
    Idx<ubyte> display(height, width), display2(height, width);
    idx_clear(display);
    idx_clear(display2);
    // copy input images locally
    memcpy(grabbed.idx_ptr(), left, height * width * sizeof (ubyte));
    memcpy(grabbed2.idx_ptr(), right, height * width * sizeof (ubyte));
    // prepare multi resolutions input
    Idx<double> inx;
    int ni = ((state_idx*) inputs.get(0))->x.dim(1);
    int nj = ((state_idx*) inputs.get(0))->x.dim(2);
    int zi = max(ni, (int) (zoom * grabbed.dim(0)));
    int zj = max(nj, (int) (zoom * grabbed.dim(1)));
    int oi = (zi - ni) / 2;
    int oj = (zj - nj) / 2;
    Idx<ubyte> imleft = image_resize(grabbed, zj, zi, 1);
    imleft = imleft.narrow(0, ni, oi);
    imleft = imleft.narrow(1, nj, oj);
    Idx<ubyte> tmp = image_resize(grabbed2, zj, zi, 1);
    Idx<ubyte> imright(tmp.dim(0), tmp.dim(1));
    idx_clear(imright);
    Idx<ubyte> tmpright(imright);
    tmp = tmp.narrow(0, imright.dim(0) - dx, 0);
    tmp = tmp.narrow(1, imright.dim(1) - dy, 0);
    tmpright = tmpright.narrow(0, imright.dim(0) - dx, dx);
    tmpright = tmpright.narrow(1, imright.dim(1) - dy, dy);
    idx_copy(tmp, tmpright);
    imright = imright.narrow(0, ni, oi);
    imright = imright.narrow(1, nj, oj);
    // display
    Idx<ubyte> disp1(display.narrow(0, imright.dim(0), 0));
    disp1 = disp1.narrow(1, imright.dim(1), 0);
    Idx<ubyte> disp2(display2.narrow(0, imright.dim(0), 0));
    disp2 = disp2.narrow(1, imright.dim(1), 0);
    idx_copy(imright, disp2);
    idx_copy(imleft, disp1);
    { idx_bloop1(in, inputs, void*) {
  	inx = ((state_idx*) in.get())->x;
  	ni = inx.dim(1);
  	nj = inx.dim(2);
  	Idx<ubyte> iml = image_resize(imleft, nj, ni, 1);
  	Idx<double> inx0 = inx.select(0, 0);
  	Idx<ubyte> imr = image_resize(imright, nj, ni, 1);
  	Idx<double> inx1 = inx.select(0, 1);
  	idx_copy(iml, inx0);
  	idx_copy(imr, inx1);
  	idx_addc(inx, bias, inx);
  	idx_dotc(inx, coeff, inx);
      }}
    // display
    memcpy(left, display.idx_ptr(), height * width * sizeof (ubyte));
    memcpy(right, display2.idx_ptr(), height * width * sizeof (ubyte));
  }

  Idx<double> Classifier2DBinoc::fprop(ubyte *left, ubyte *right, 
				       float zoom, int dx, int dy, 
				       double threshold, int objsize) {
    this->multi_res_prep(left, right, dx, dy, 0.5);
    return this->multi_res_fprop(threshold, objsize);
  }

  ////////////////////////////////////////////////////////////////

} // end namespace ebl

