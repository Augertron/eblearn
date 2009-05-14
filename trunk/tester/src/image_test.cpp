#include "image_test.h"

//#define __SHOW__

#ifdef __GUI__
#include "libidxgui.h"
#endif

extern string *gl_data_dir;
extern string *gl_data_errmsg;

using namespace std;
using namespace ebl;

void image_test::setUp() {
}

void image_test::tearDown() {
}

void image_test::test_resize() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  idx<ubyte> m(6, 8);
  idx<ubyte> res;
	
  int i = 0;
  { idx_aloop1(m0, m, ubyte) {
      *m0 = i++;
    }}
  //m.printElems();
  res = image_resize(m, 4, 3, 1);
  //res.printElems();
  CPPUNIT_ASSERT(res.get(0, 0) == 4);
  CPPUNIT_ASSERT(res.get(0, 1) == 6);
  CPPUNIT_ASSERT(res.get(0, 2) == 8);
  CPPUNIT_ASSERT(res.get(0, 3) == 10);
  CPPUNIT_ASSERT(res.get(1, 0) == 20);
  CPPUNIT_ASSERT(res.get(1, 1) == 22);
  CPPUNIT_ASSERT(res.get(1, 2) == 24);
  CPPUNIT_ASSERT(res.get(1, 3) == 26);
  CPPUNIT_ASSERT(res.get(2, 0) == 36);
  CPPUNIT_ASSERT(res.get(2, 1) == 38);
  CPPUNIT_ASSERT(res.get(2, 2) == 40);
  CPPUNIT_ASSERT(res.get(2, 3) == 42);
	
  idx<ubyte> im(1, 1, 1);
  string imgfile = *gl_data_dir;
  imgfile += "/pnm/hat_P6.ppm";

  pnm_fread_into_rgbx(imgfile.c_str(), im);
  im = im.select(2, 0);
#ifdef __GUI__  
#ifdef __SHOW__
  unsigned int wid = gui.new_window("image_test");
  //  im = image_resize(im, im.dim(0) + 10, im.dim(1) + 10);
  im = image_resize(im, 100, 100);
  gui.draw_matrix(im, 0, 0, 4.0, 4.0);
  RenderThread &cout = gui;
  cout << "Testing images operations..." << endl;
  int hy = im.dim(0) * 4;
#endif
#endif

  im = image_resize(im, 250, 250);

#ifdef __GUI__  
#ifdef __SHOW__
  int wx = 0;
//   gui.new_window("image_test2");
//   for (int i = 0; i < 10; ++i) {
//     gui.draw_matrix(im, 0, wx);
//     wx += im.dim(1) + 2;
//   }
  im = image_resize(im, 100, 100);
  gui.select_window(wid);
  wx = 0;
  for (int i = 0; i < 10; ++i) {
    gui.draw_matrix(im, hy + 2, wx);
    wx += im.dim(1) + 2;
  }
#endif
#endif
}

void image_test::test_pnm_P3() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  idx<ubyte> im(1, 1, 3);
  string imgfile = *gl_data_dir;
  imgfile += "/pnm/rgb_P3.ppm";
  pnm_fread_into_rgbx(imgfile.c_str(), im);

  CPPUNIT_ASSERT(im.get(0, 0, 0) == 255);
  CPPUNIT_ASSERT(im.get(0, 0, 1) == 0);
  CPPUNIT_ASSERT(im.get(0, 0, 2) == 0);
  CPPUNIT_ASSERT(im.get(0, 1, 0) == 0);
  CPPUNIT_ASSERT(im.get(0, 1, 1) == 255);
  CPPUNIT_ASSERT(im.get(0, 1, 2) == 0);
  CPPUNIT_ASSERT(im.get(0, 2, 0) == 0);
  CPPUNIT_ASSERT(im.get(0, 2, 1) == 0);
  CPPUNIT_ASSERT(im.get(0, 2, 2) == 255);
}

void image_test::test_pnm_P6() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  idx<ubyte> im(1, 1, 3);
  string imgfile = *gl_data_dir;
  imgfile += "/pnm/rgb_P6.ppm";
  pnm_fread_into_rgbx(imgfile.c_str(), im);
  CPPUNIT_ASSERT(im.get(0, 0, 0) == 255);
  CPPUNIT_ASSERT(im.get(0, 0, 1) == 0);
  CPPUNIT_ASSERT(im.get(0, 0, 2) == 0);
  CPPUNIT_ASSERT(im.get(0, 1, 0) == 0);
  CPPUNIT_ASSERT(im.get(0, 1, 1) == 255);
  CPPUNIT_ASSERT(im.get(0, 1, 2) == 0);
  CPPUNIT_ASSERT(im.get(0, 2, 0) == 0);
  CPPUNIT_ASSERT(im.get(0, 2, 1) == 0);
  CPPUNIT_ASSERT(im.get(0, 2, 2) == 255);

  pnm_fread_into_rgbx(imgfile.c_str(), im);
}

void image_test::test_colorspaces() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  idx<ubyte> im_rgb(1, 1, 3);
  string imgfile = *gl_data_dir;
  imgfile += "/barn.png";
  image_read_rgbx(imgfile.c_str(), im_rgb);
  im_rgb = image_resize(im_rgb, 252, 189);
  idxdim d(im_rgb);
  idx<float> fim_rgb(d);
  idx_copy(im_rgb, fim_rgb);
  idx<float> fim_yuv(d);
  rgb_to_yuv(fim_rgb, fim_yuv);
  idx<float> im_hsv(d);
  rgb_to_hsv(fim_rgb, im_hsv);
  idx<float> im_hsv3(d);
  rgb_to_hsv3(fim_rgb, im_hsv3, .10, .15);
  
  idx<float> ftmp(d);
  idx<float> ftmp1(d);
  idx<float> ftmp3(1, 1);
  idx<float> ftmp5(1, 1);
  idx<float> ftmp6(d.dim(0), d.dim(1));

  // TODO: wrong values, fixme
//   CPPUNIT_ASSERT(imb.get(0, 0, 0) == 81);
//   CPPUNIT_ASSERT(imb.get(0, 0, 1) == 239);
//   CPPUNIT_ASSERT(imb.get(0, 0, 2) == 90);
//   CPPUNIT_ASSERT(imb.get(0, 1, 0) == 144);
//   CPPUNIT_ASSERT(imb.get(0, 1, 1) == 34);
//   CPPUNIT_ASSERT(imb.get(0, 1, 2) == 53);
//   CPPUNIT_ASSERT(imb.get(0, 2, 0) == 40);
//   CPPUNIT_ASSERT(imb.get(0, 2, 1) == 109);
//   CPPUNIT_ASSERT(imb.get(0, 2, 2) == 239);

//   idx<ubyte> im0(1, 1, 1);
//   idx<ubyte> im1(1, 1, 1);
//   idx<ubyte> im2(1, 1, 1);
//   idx<ubyte> im3(1, 1, 1);
//   pnm_fread_into_rgbx(imgfile.c_str(), im0);
//   im1 = image_resize(im0, 200, 100, 0);
//   im2 = image_resize(im0, 200, 100, 0);
//   im3 = image_resize(im0, 200, 100, 0);
//   idx_clear(im2);
//   idx_clear(im3);
//   rgb_to_yuv(im1, im2);
//   yuv_to_rgb(im2, im3);
  // TODO: complete test

#ifdef __GUI__
  new_window("Testing color spaces");
  idx<ubyte> tmp2;
  idx<float> ftmp2;
  unsigned int h = 0, w = 0;
  // input (RGB)
  draw_matrix(im_rgb, "RGB", h, w);
  w += im_rgb.dim(1) + 5;
  tmp2 = im_rgb.select(2, 0);
  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
  draw_matrix(tmp2, "R", h, w);
  w += tmp2.dim(1) + 5;
  tmp2 = im_rgb.select(2, 1);
  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
  draw_matrix(tmp2, "G", h, w);
  w += tmp2.dim(1) + 5;
  tmp2 = im_rgb.select(2, 2);
  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
  draw_matrix(tmp2, "B", h, w);
  w = 0;
  h += tmp2.dim(0) + 5;
  
  // yuv
//   draw_matrix(fim_yuv, "YUV", h, w);
//   w += fim_yuv.dim(1) + 5;
  ftmp2 = fim_yuv.select(2, 0);
//   idx<float> hat = create_mexican_hat<float>(2, 5);
//   image_mexican_filter(ftmp2, ftmp5, 0, 0, 7, &hat, &ftmp3);
  image_mexican_filter(ftmp2, ftmp5, 5, 9);
  image_global_normalization(ftmp5);
//   cout << "Y inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
//   cout << "Y hat inf: " << (int) idx_min(ftmp5) << " sup:" << (int) idx_max(ftmp5) << endl;
  draw_matrix(ftmp5, "Y (mexican hat = 5, 9x9)", h, w, 1, 1, (float)-1.0, (float)1.0);
  w += tmp2.dim(1) + 5;

  idx_copy(ftmp2, ftmp5);
  image_global_normalization(ftmp5);
  image_local_normalization(ftmp5, ftmp6, 9);
  draw_matrix(ftmp6, "Y (global + local norm 9x9)", h, w, 1, 1, (float)-1.0, (float)1.0);
  w += tmp2.dim(1) + 5;

  image_mexican_filter(ftmp2, ftmp5, 5, 9);
  image_global_normalization(ftmp5);
  image_local_normalization(ftmp5, ftmp6, 9);
  draw_matrix(ftmp6, "Y (mex glob loc 5,9x9)", h, w, 1, 1, (float)-1.0, (float)1.0);
  w += tmp2.dim(1) + 5;

  image_mexican_filter(ftmp2, ftmp5, 8, 9);
  image_global_normalization(ftmp5);
  draw_matrix(ftmp5, "Y (mexican hat = 9)", h, w, 1, 1, (float)-1.0, (float)1.0);
  w += tmp2.dim(1) + 5;

//   ftmp2 = fim_yuv.select(2, 1);
//   cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
//   draw_matrix(ftmp2, "U", h, w);
//   w += ftmp2.dim(1) + 5;
//   ftmp2 = fim_yuv.select(2, 2);
//   cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
//   draw_matrix(ftmp2, "V", h, w);

  w = 0;
  h += ftmp2.dim(0) + 5;
  
  // hsv
  draw_matrix(im_hsv, "HSV", h, w);
  w += im_hsv.dim(1) + 5;
  idx_copy(im_hsv, ftmp);
  ftmp2 = ftmp.select(2, 1); // s
  idx_fill(ftmp2, (float).5);
  ftmp2 = ftmp.select(2, 2); // v
  idx_fill(ftmp2, (float).5);
  hsv_to_rgb(ftmp, ftmp1);
  ftmp2 = im_hsv.select(2, 0);
  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
  cout << "inf tmp1: " << (int) idx_min(ftmp1) << " sup:" << (int) idx_max(ftmp1) << endl;
  draw_matrix(ftmp1, "H", h, w, 1.0, 1.0, (float)63.0, (float)127.0);
  w += ftmp2.dim(1) + 5;
  ftmp2 = im_hsv.select(2, 1);
  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
  draw_matrix(ftmp2, "S", h, w);
  w += ftmp2.dim(1) + 5;
  ftmp2 = im_hsv.select(2, 2);
  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
  draw_matrix(ftmp2, "V", h, w);
  w = 0;
  h += ftmp2.dim(0) + 5;

    // hsv3
  draw_matrix(im_hsv3, "HSV3", h, w);
  w += im_hsv3.dim(1) + 5;
  idx_copy(im_hsv3, ftmp);
  ftmp2 = ftmp.select(2, 1); // s
  idx_fill(ftmp2, (float).5);
  ftmp2 = ftmp.select(2, 2); // v
  idx_fill(ftmp2, (float).5);
  hsv3_to_rgb(ftmp, ftmp1);
  ftmp2 = im_hsv3.select(2, 0);
  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
  cout << "inf tmp1: " << (int) idx_min(ftmp1) << " sup:" << (int) idx_max(ftmp1) << endl;
  draw_matrix(ftmp1, "H3", h, w, 1.0, 1.0, (float)63.0, (float)127.0);
  w += ftmp2.dim(1) + 5;
  ftmp2 = im_hsv3.select(2, 1);
  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
  draw_matrix(ftmp2, "S", h, w);
  w += ftmp2.dim(1) + 5;
  ftmp2 = im_hsv3.select(2, 2);
  cout << "inf: " << (int) idx_min(ftmp2) << " sup:" << (int) idx_max(ftmp2) << endl;
  draw_matrix(ftmp2, "V", h, w);

  w = 0;
  h += ftmp2.dim(0) + 5;
  idx<float> spectrum(50, 420, 3);
  idx<float> ftmp4(50, 420, 3);
  for (int i = 0; i < 420; ++i) {
    ftmp2 = spectrum.select(1, i);
    idx_fill(ftmp2, (float) i);
  }
  ftmp2 = spectrum.select(2, 1);
  idx_fill(ftmp2, (float).5);
  ftmp2 = spectrum.select(2, 2);
  idx_fill(ftmp2, (float).5);
  hsv3_to_rgb(spectrum, ftmp4);
  draw_matrix(ftmp4, "H3 0 - 420", h, w, 1.0, 1.0, (float)63.0, (float)127.0);
  
  sleep(5000);
#endif
}
