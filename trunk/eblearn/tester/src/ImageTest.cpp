#include "ImageTest.h"

#ifdef __GUI__
#include "libeblearn_gui.h"
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
  Idx<ubyte> m(3, 4);
  Idx<ubyte> res;
	
  int i = 0;
  { idx_aloop1(m0, m, ubyte) {
      *m0 = i++;
    }}
  //	m.printElems();
  res = image_resize(m, 1, 2, 1);
  //	res.printElems();
  CPPUNIT_ASSERT(res.get(0, 0) == 2);
  CPPUNIT_ASSERT(res.get(1, 0) == 8);
	
  Idx<ubyte> im(1, 1, 1);
  string imgfile = *gl_data_dir;
  imgfile += "/pnm/hat_P6.ppm";

  pnm_fread_into_rgbx(imgfile.c_str(), im);
  im = image_resize(im, 50, 50, 1);

#ifdef __GUI__  
  new_window(w);
  w->gray_draw_matrix(&im, UBYTE, 0, 0, 0, 255);
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
#ifdef __GUI__
  //ebwindow *w = new ebwindow;
  // TODO: this does not work
  //  w->RGB_draw_matrix(&im, UBYTE, 0, 0, 100, 100);
#endif
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
#ifdef __GUI__
  // TODO: this does not work
  //  w->RGB_draw_matrix(&im, UBYTE);
#endif
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
