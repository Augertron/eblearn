#include "detector_test.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace ebl;

extern string *gl_data_dir;
extern string *gl_data_errmsg;


void detector_test::setUp() {
}

void detector_test::tearDown() {
}

void detector_test::test_norb() {
  try {
    typedef double t_net;
    CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
    string mono_net = *gl_data_dir;
    mono_net += 
      "/norb/20040618-1628-031617-lenet7-jitt-bg-mono-demo-4082400.prm";
    string imgfile = *gl_data_dir;
    //  imgfile += "/norb/dino.jpg";
    //    imgfile += "/norb/planes.png";
    //      imgfile += "/einstein.jpg";
    //      imgfile += "/einstein_bicycle.jpg";
    imgfile += "/car_bull_cropped.jpg";
    //    imgfile += "/norb/car_left.png";

    const char labels[5][10] = {"animal", "human", "plane", "truck", "car"};
    idx<const char*> lbl(5);
    for (int i = 0; i < lbl.nelements(); ++i)
      lbl.set(labels[i], i);
    idx<ubyte> labs = strings_to_idx(lbl);

    idx<ubyte> left = load_image<ubyte>(imgfile.c_str());

    // parameter, network and classifier
    // load the previously saved weights of a trained network
    parameter<fstate_idx<t_net> > theparam(1);
    // input to the network will be 96x96 and there are 5 outputs
    lenet7_binocular<fs(t_net)> thenet(theparam, 96, 96, 5, false, false, true);
    theparam.load_x(mono_net.c_str());
    //left = left.narrow(2, 2, 0);
    //  int tr[3] = { 2, 1, 0 };
    //left = left.transpose(tr);
    //left = left.select(2, 0);
    double scales [] = { 2, 1.5, 1};
    detector<fs(t_net)> cb((module_1_1<fs(t_net)>&)thenet, labs, NULL,
			   0, NULL, 0, (float)0.01);
    cb.set_resolutions(3, scales);

#ifdef __GUI__
    detector_gui<fs(t_net)> cgui;
    vector<bbox*> &bb = cgui.display_all(cb, left, .97);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, bb.size()); // only 1 object
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double) 0.973895462819159,
				 bb[0]->confidence, .0000001);
    CPPUNIT_ASSERT_EQUAL((int) 0, bb[0]->class_id);
#else
    vector<bbox*> &bb = cb.fprop(left, .97);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, bb.size()); // only 1 object
    CPPUNIT_ASSERT_DOUBLES_EQUAL((double) 0.973895462819159,
				 bb[0]->confidence, .0000001);
    CPPUNIT_ASSERT_EQUAL((int) 0, bb[0]->class_id);
#endif
    secsleep(5);
  } catch(string &err) {
    cerr << err << endl;
  }
}

void detector_test::test_norb_binoc() {
  try {
    CPPUNIT_ASSERT(false);
    //   CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
    //   string binoc_net = *gl_data_dir;
    //   binoc_net += "/norb/20040619-1112-001772-lenet7-jitt-bg-demo.prm";
    //   string imgfilel = *gl_data_dir;
    //   imgfilel += "/norb/plane_left.mat";
    //   string imgfiler = *gl_data_dir;
    //   imgfiler += "/norb/plane_right.mat";
    //   const char labels[5][10] = {"animal","human", "plane", "truck", "car"};
    //   idx<const char*> lbl(5);
    //   int sizes[] = { 9, 6, 4, 2 };	
    //   idx<ubyte> left(1, 1), right(1, 1);
    //   CPPUNIT_ASSERT(load_matrix<ubyte>(left, imgfilel.c_str()) == true);
    //   CPPUNIT_ASSERT(load_matrix<ubyte>(right, imgfiler.c_str()) == true);

    //   idx<int> sz(sizeof (sizes) / sizeof (int));
    //   memcpy(sz.idx_ptr(), sizes, sizeof (sizes));
    //   for (int i = 0; i < lbl.nelements(); ++i)
    //     lbl.set(labels[i], i);

    //   // parameter, network and classifier
    //   // load the previously saved weights of a trained network
    //   parameter theparam(1);
    //   // input to the network will be 96x96 and there are 5 outputs
    //   lenet7_binocular thenet(theparam, 96, 96, 5);
    //   theparam.load_x<double>(binoc_net.c_str());
    //   detector_binocular<ubyte> cb(thenet, sz, lbl, 0.0, 0.01, 240, 320);

    //   // find category of image
    //   left = image_resize(left, 320, 240, 1);
    //   right = image_resize(right, 320, 240, 1);
    //   //idx_copy(left, right);
    //   idx<double> res = cb.fprop(left.idx_ptr(), right.idx_ptr(), 
    // 			     1, 0, 40, 1.8, 60);
    //   CPPUNIT_ASSERT(res.dim(0) == 1); // only 1 object
    //   CPPUNIT_ASSERT(res.get(0, 0) == 2); // plane
  } catch(string &err) {
    cerr << err << endl;
  }
}

