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
  im_rgb = image_resize(im_rgb, 320, 240);
  idxdim d(im_rgb);
  idx<ubyte> im_yuv(d);
  rgb_to_yuv(im_rgb, im_yuv);
  idx<ubyte> im_hsv(d);
  rgb_to_hsv(im_rgb, im_hsv);

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
  unsigned int h = 0, w = 0;
  // input (RGB)
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
  tmp2 = im_yuv.select(2, 0);
  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
  draw_matrix(tmp2, "Y", h, w);
  w += tmp2.dim(1) + 5;
  tmp2 = im_yuv.select(2, 1);
  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
  draw_matrix(tmp2, "U", h, w);
  w += tmp2.dim(1) + 5;
  tmp2 = im_yuv.select(2, 2);
  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
  draw_matrix(tmp2, "V", h, w);
  w = 0;
  h += tmp2.dim(0) + 5;
  // hsv
  tmp2 = im_hsv.select(2, 0);
  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
  draw_matrix(tmp2, "H", h, w);
  w += tmp2.dim(1) + 5;
  tmp2 = im_hsv.select(2, 1);
  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
  draw_matrix(tmp2, "S", h, w);
  w += tmp2.dim(1) + 5;
  tmp2 = im_hsv.select(2, 2);
  cout << "inf: " << (int) idx_min(tmp2) << " sup:" << (int) idx_max(tmp2) << endl;
  draw_matrix(tmp2, "V", h, w);
  sleep(500);
#endif
}
