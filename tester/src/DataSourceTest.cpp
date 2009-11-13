#include "DataSourceTest.h"
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;
using namespace ebl;

extern string *gl_mnist_dir;
extern string *gl_data_dir;
extern string *gl_mnist_errmsg;
extern string *gl_data_errmsg;

void DataSourceTest::setUp() {
}

void DataSourceTest::tearDown() {
}

// test function for mnist data source (requires special matrix header reading).
void DataSourceTest::test_mnist_LabeledDataSource() {
  CPPUNIT_ASSERT_MESSAGE("TODO: fixme", false);
  CPPUNIT_ASSERT_MESSAGE(*gl_mnist_errmsg, gl_mnist_dir != NULL);
  string datafile = *gl_mnist_dir;
  string labelfile = *gl_mnist_dir;
  datafile += "/t10k-images-idx3-ubyte";
  labelfile += "/t10k-labels-idx1-ubyte";
  idx<ubyte> data(1, 1, 1), labels(1);
  CPPUNIT_ASSERT(load_matrix<ubyte>(data, datafile.c_str()) == true);
  CPPUNIT_ASSERT(load_matrix<ubyte>(labels, labelfile.c_str()) == true);
  labeled_datasource<ubyte,ubyte> ds(data, labels, 0.0, 1.0);
  state_idx datum(28, 28);
  idx<ubyte> label;
  for (int i = 0; i < 5; i++) {
    ds.fprop(datum, label);
    /* cout<<"Datum:"<<endl;
       datum.x.printElems();
       cout<<"Label: ";
       label.printElems();
       cout<<endl; */
    ds.next();
  }
  // briefly test some values of the 5th element of mnist
  CPPUNIT_ASSERT_EQUAL((unsigned int) 4, (unsigned int) label.get());
  CPPUNIT_ASSERT_EQUAL((unsigned int) 236, (unsigned int) datum.x.get(9, 9));
}

void DataSourceTest::test_imageDirToIdx() {
  // TODO: add test for dataset class
//   CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
//   string dir = *gl_data_dir;
//   dir += "/pnm/";
// #ifdef __BOOST__
//   CPPUNIT_ASSERT(imagedir_to_idx(dir.c_str(), 48, 0, ".*[.]ppm", 
// 				 NULL, "/tmp", NULL, true) == true);
// #else
//   CPPUNIT_ASSERT_MESSAGE("Not tested because of missing Boost libraries", 
// 			 false);
// #endif
}
