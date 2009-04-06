#include "Classifier2DTest.h"

// #define __SHOW__

#ifdef __GUI__
#include "libidxgui.h"
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
  idx<const char*> lbl(5);
  for (int i = 0; i < lbl.nelements(); ++i)
    lbl.set(labels[i], i);

  int sizes[] = { 9, 6, 4, 2 };	
  idx<ubyte> left(1, 1);
  CPPUNIT_ASSERT(load_matrix<ubyte>(left, imgfile.c_str()) == true);
  left = image_resize(left, 320, 240, 1);
  idx<int> sz(sizeof (sizes) / sizeof (int));
  memcpy(sz.idx_ptr(), sizes, sizeof (sizes));

  // parameter, network and classifier
  // load the previously saved weights of a trained network
  parameter theparam(1);
  // input to the network will be 96x96 and there are 5 outputs
  lenet7_binocular thenet(theparam, 96, 96, 5);
  theparam.load_x(mono_net.c_str());
  classifier2D cb(thenet, sz, lbl, 0.0, 0.01, 240, 320);

  // find category of image
  idx<double> res = cb.fprop(left.idx_ptr(), 1, 1.8, 60);
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
  idx<const char*> lbl(5);
  int sizes[] = { 9, 6, 4, 2 };	
  idx<ubyte> left(1, 1), right(1, 1);
  CPPUNIT_ASSERT(load_matrix<ubyte>(left, imgfilel.c_str()) == true);
  CPPUNIT_ASSERT(load_matrix<ubyte>(right, imgfiler.c_str()) == true);

  idx<int> sz(sizeof (sizes) / sizeof (int));
  memcpy(sz.idx_ptr(), sizes, sizeof (sizes));
  for (int i = 0; i < lbl.nelements(); ++i)
    lbl.set(labels[i], i);

  // parameter, network and classifier
  // load the previously saved weights of a trained network
  parameter theparam(1);
  // input to the network will be 96x96 and there are 5 outputs
  lenet7_binocular thenet(theparam, 96, 96, 5);
  theparam.load_x(binoc_net.c_str());
  classifier2D_binocular cb(thenet, sz, lbl, 0.0, 0.01, 240, 320);

  // find category of image
  left = image_resize(left, 320, 240, 1);
  right = image_resize(right, 320, 240, 1);
  //idx_copy(left, right);
  idx<double> res = cb.fprop(left.idx_ptr(), right.idx_ptr(), 
			     1, 0, 40, 1.8, 60);
  CPPUNIT_ASSERT(res.dim(0) == 1); // only 1 object
  CPPUNIT_ASSERT(res.get(0, 0) == 2); // plane
}

