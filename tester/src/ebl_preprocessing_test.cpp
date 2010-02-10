#include "ebl_preprocessing_test.h"

#include <ostream>

#define __SHOW__

#ifdef __GUI__
#include "libidxgui.h"
#endif

extern string *gl_data_dir;
extern string *gl_data_errmsg;

using namespace std;
using namespace ebl;

void ebl_preprocessing_test::setUp() {
}

void ebl_preprocessing_test::tearDown() {
}

void ebl_preprocessing_test::test_rgb_to_yp() {
  ostringstream path, s;
  path << *gl_data_dir << "/barn.png";
  idx<float> im = load_image<float>(path.str());
  path.str(""); path << *gl_data_dir << "/2008_007714.jpg";
  idx<float> imbird = load_image<float>(path.str());
  rgb_to_yp_module<float> pp(9);
  rgb_to_ypuv_module<float> ppuv(9);
  resizepp_module<float> mean_rspp(100, 300, MEAN_RESIZE, &pp, 9);
  resizepp_module<float> bil_rspp(100, 300, BILINEAR_RESIZE, &pp, 9);
  resizepp_module<float> gaus_rspp(100, 300, GAUSSIAN_RESIZE, &pp, 9);
  // barn
  im = im.shift_dim(2, 0);
  state_idx<float> in(im.get_idxdim()), out(1, 1, 1);
  idx_copy(im, in.x);
  im = im.shift_dim(0, 2);
  pp.fprop(in, out);
  idx<float> im2 = out.x.shift_dim(0, 2);
  im2 = idx_copy(im2);
  ppuv.fprop(in, out);
  idx<float> im3 = out.x.shift_dim(0, 2);
  im3 = idx_copy(im3);
  bil_rspp.fprop(in, out);
  idx<float> im4 = out.x.shift_dim(0, 2);
  im4 = idx_copy(im4);
  gaus_rspp.fprop(in, out);
  idx<float> im5 = out.x.shift_dim(0, 2);
  im5 = idx_copy(im5);
  mean_rspp.fprop(in, out);
  idx<float> im6 = out.x.shift_dim(0, 2);
  im6 = idx_copy(im6);
  // bird
  imbird = imbird.shift_dim(2, 0);
  state_idx<float> inb(imbird.get_idxdim());
  idx_copy(imbird, inb.x);
  imbird = imbird.shift_dim(0, 2);
  pp.fprop(inb, out);
  idx<float> im7 = out.x.shift_dim(0, 2);
  im7 = idx_copy(im7);
  ppuv.fprop(inb, out);
  idx<float> im8 = out.x.shift_dim(0, 2);
  im8 = idx_copy(im8);
  bil_rspp.fprop(inb, out);
  idx<float> im9 = out.x.shift_dim(0, 2);
  im9 = idx_copy(im9);
  gaus_rspp.fprop(inb, out);
  idx<float> im10 = out.x.shift_dim(0, 2);
  im10 = idx_copy(im10);
  mean_rspp.fprop(inb, out);
  idx<float> im11 = out.x.shift_dim(0, 2);
  im11 = idx_copy(im11);
#ifdef __GUI__  
#ifdef __SHOW__
  uint h = 0, w = 0;
  new_window("ebl_preprocessing_test");
  // barn
  s.str(""); s << "RGB " << im;
  draw_matrix(im, s.str().c_str(), h, w);
  w += im.dim(1) + 2;
  s.str(""); s << "Yp " << im2;
  draw_matrix(im2, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  w += im2.dim(1) + 2;  
  s.str(""); s << "YpUV " << im3;
  draw_matrix(im3, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  h += im3.dim(0) + 2;
  w = 0;
  s.str(""); s << "bilinear " << im4;
  draw_matrix(im4, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  w += im4.dim(1) + 2;
  s.str(""); s << "gaussian " << im5;
  draw_matrix(im5, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  w += im5.dim(1) + 2;
  s.str(""); s << "mean " << im6;
  draw_matrix(im6, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  h += im6.dim(0) + 2;
  w = 0;
  // bird
  h += 15;
  gui << at(h - 15, w) << "RGB " << im;
  draw_matrix(imbird, h, w);
  w += imbird.dim(1) + 152;
  gui << at(h - 15, w) << "Yp " << im7;
  draw_matrix(im7, h, w, 1, 1, (float)-1, (float)1);
  w += im7.dim(1) + 152;  
  gui << at(h - 15, w) << "YpUV " << im8;
  draw_matrix(im8, h, w, 1, 1, (float)-1, (float)1);
  h += im8.dim(0) + 2;
  w = 0;
  s.str(""); s << "bilinear " << im9;
  draw_matrix(im9, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  w += im9.dim(1) + 2;
  s.str(""); s << "gaussian " << im10;
  draw_matrix(im10, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  w += im10.dim(1) + 2;
  s.str(""); s << "mean " << im11;
  draw_matrix(im11, s.str().c_str(), h, w, 1, 1, (float)-1, (float)1);
  h += im11.dim(0) + 2;
  w = 0;
  
#endif
#endif
}
