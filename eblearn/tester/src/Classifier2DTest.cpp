#include "Classifier2DTest.h"

#ifdef __GUI__
#include "libeblearn_gui.h"
#endif

using namespace ebl;

extern string *gl_data_dir;
extern string *gl_data_errmsg;


void Classifier2DTest::setUp() {
}

void Classifier2DTest::tearDown() {
}

void Classifier2DTest::test_norb() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  string mono_net = *gl_data_dir;
  mono_net += "/norb/20040618-1628-031617-lenet7-jitt-bg-mono-demo-4082400.prm";
  string imgfile = *gl_data_dir;
  imgfile += "/norb/plane_left.mat";
  const char labels[5][10] = {"animal", "human", "plane", "truck", "car"};
  Idx<const char*> lbl(5);
  int sizes[] = { 9, 6, 4, 2 };	
  Idx<ubyte> left(1, 1);
  CPPUNIT_ASSERT(load_matrix<ubyte>(left, imgfile.c_str()) == true);
  left = image_resize(left, 320, 240, 1);
  Idx<int> sz(sizeof (sizes) / sizeof (int));
  memcpy(sz.idx_ptr(), sizes, sizeof (sizes));
  for (int i = 0; i < lbl.nelements(); ++i)
    lbl.set(labels[i], i);
  Classifier2D cb(mono_net.c_str(), sz, lbl, 0.0, 0.01, 240, 320);
  Idx<double> res = cb.fprop(left.idx_ptr(), 1, 1.8, 60);
  CPPUNIT_ASSERT(res.dim(0) == 1); // only 1 object
  CPPUNIT_ASSERT(res.get(0, 0) == 2); // plane
}

void Classifier2DTest::test_norb_binoc() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
  string binoc_net = *gl_data_dir;
  binoc_net += "/norb/20040619-1112-001772-lenet7-jitt-bg-demo.prm";
  string imgfilel = *gl_data_dir;
  imgfilel += "/norb/plane_left.mat";
  string imgfiler = *gl_data_dir;
  imgfiler += "/norb/plane_right.mat";
  const char labels[5][10] = {"animal", "human", "plane", "truck", "car"};
  Idx<const char*> lbl(5);
  int sizes[] = { 9, 6, 4, 2 };	
  Idx<ubyte> left(1, 1), right(1, 1);
  CPPUNIT_ASSERT(load_matrix<ubyte>(left, imgfilel.c_str()) == true);
  CPPUNIT_ASSERT(load_matrix<ubyte>(right, imgfiler.c_str()) == true);

  Idx<int> sz(sizeof (sizes) / sizeof (int));
  memcpy(sz.idx_ptr(), sizes, sizeof (sizes));
  for (int i = 0; i < lbl.nelements(); ++i)
    lbl.set(labels[i], i);
  Classifier2DBinoc cb(binoc_net.c_str(), sz, lbl, 0.0, 0.01, 240, 320);

  left = image_resize(left, 320, 240, 1);
  right = image_resize(right, 320, 240, 1);
  //idx_copy(left, right);
  Idx<double> res = cb.fprop(left.idx_ptr(), right.idx_ptr(), 
			     1, 0, 40, 1.8, 60);
  // display
  /*	Idx_Gui *ig = new Idx_Gui(&left, UBYTE);
	ig->setvmin(0);
	ig->setvmax(255);
	ig->show();
	Idx_Gui *ig2 = new Idx_Gui(&right, UBYTE);
	ig2->setvmin(0);
	ig2->setvmax(255);
	ig2->show();
  */	// tests
  CPPUNIT_ASSERT(res.dim(0) == 1); // only 1 object
  CPPUNIT_ASSERT(res.get(0, 0) == 2); // plane
}

