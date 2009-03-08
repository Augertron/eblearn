#include "ImageTest.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif

extern string *gl_data_dir;
extern string *gl_data_errmsg;

using namespace std;
using namespace ebl;

void ImageTest::setUp() {
}

void ImageTest::tearDown() {
}

void ImageTest::test_resize() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  Idx<ubyte> m(6, 8);
  Idx<ubyte> res;
	
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
	
  Idx<ubyte> im(1, 1, 1);
  string imgfile = *gl_data_dir;
  imgfile += "/pnm/hat_P6.ppm";

  pnm_fread_into_rgbx(imgfile.c_str(), im);
  im = im.select(2, 0);
#ifdef __GUI__  
  unsigned int wid = gui.new_window("ImageTest");
  //  im = image_resize(im, im.dim(0) + 10, im.dim(1) + 10);
  im = image_resize(im, 100, 100);
  gui.draw_matrix(im, 0, 0, (ubyte)0, (ubyte)0, 4.0, 4.0);
  RenderThread &cout = gui;
  cout << "Testing images operations..." << endl;
  int hy = im.dim(0) * 4;
#endif

  im = image_resize(im, 250, 250);

#ifdef __GUI__  
  int wx = 0;
//   gui.new_window("ImageTest2");
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
}

void ImageTest::test_pnm_P3() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  Idx<ubyte> im(1, 1, 3);
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

void ImageTest::test_pnm_P6() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  Idx<ubyte> im(1, 1, 3);
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

void ImageTest::test_yuvrgb() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  Idx<ubyte> im(1, 1, 3);
  Idx<ubyte> imb(1, 1, 3);
  string imgfile = *gl_data_dir;
  imgfile += "/pnm/rgb_P6.ppm";
  pnm_fread_into_rgbx(imgfile.c_str(), im);
  pnm_fread_into_rgbx(imgfile.c_str(), imb);
  idx_clear(imb);
  RGBtoYUV(im, imb);

  CPPUNIT_ASSERT(imb.get(0, 0, 0) == 81);
  CPPUNIT_ASSERT(imb.get(0, 0, 1) == 239);
  CPPUNIT_ASSERT(imb.get(0, 0, 2) == 90);
  CPPUNIT_ASSERT(imb.get(0, 1, 0) == 144);
  CPPUNIT_ASSERT(imb.get(0, 1, 1) == 34);
  CPPUNIT_ASSERT(imb.get(0, 1, 2) == 53);
  CPPUNIT_ASSERT(imb.get(0, 2, 0) == 40);
  CPPUNIT_ASSERT(imb.get(0, 2, 1) == 109);
  CPPUNIT_ASSERT(imb.get(0, 2, 2) == 239);

  Idx<ubyte> im0(1, 1, 1);
  Idx<ubyte> im1(1, 1, 1);
  Idx<ubyte> im2(1, 1, 1);
  Idx<ubyte> im3(1, 1, 1);
  pnm_fread_into_rgbx(imgfile.c_str(), im0);
  im1 = image_resize(im0, 200, 100, 0);
  im2 = image_resize(im0, 200, 100, 0);
  im3 = image_resize(im0, 200, 100, 0);
  idx_clear(im2);
  idx_clear(im3);
  RGBtoYUV(im1, im2);
  YUVtoRGB(im2, im3);
  /*
    ebwindow *w = new ebwindow(500, 500);
    w->RGB_draw_matrix(&im, UBYTE, 150, 0, 100, 100);
    w->RGB_draw_matrix(&imb, UBYTE, 150, 100, 100, 100);
    w->RGB_draw_matrix(&im1, UBYTE, 0, 0, 1, 1);
    w->RGB_draw_matrix(&im2, UBYTE, 0, im0.dim(1) * 1 + 30, 1, 1);
    w->RGB_draw_matrix(&im3, UBYTE, 0, im0.dim(1) * 2 + 30, 1, 1);
  */
}
