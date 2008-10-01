#include "DataSourceTest.h"
#include <iostream>
#include <algorithm>
#include <string>
#include "DataSource.h"
#include "DataTools.h"

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

void DataSourceTest::test_LabeledDataSource() {
  const int ndata = 5;

  // Fill data with decreasing negative numbers
  Idx<double> data(ndata, 2, 3);
  std::generate(data.scalars_begin(), data.scalars_end(), Counter<double>(0,
									  -1));

  // Fill labels with increasing positive numbers
  Idx<int> labels(ndata);
  std::generate(labels.scalars_begin(), labels.scalars_end(), Counter<int>());

  LabeledDataSource<double, int> ds(&data, &labels);

  // Print out two epochs
  {
    state_idx datum(1, 2, 3);
    Idx<int> label;
    for (int age = 0; age < ndata * 2; ++age) {
      ds.fprop(&datum, &label);
      /*			cout<<"Datum:"<<endl;
				datum.x.printElems();
				cout<<"Label: ";
				label.printElems();
				cout<<endl;
      */			 
      ds.next();
    }
    CPPUNIT_ASSERT_EQUAL(-29.0, datum.x.get(0, 1, 2));
    CPPUNIT_ASSERT_EQUAL(4, label.get());
  }
}

// test function for mnist data source (requires special matrix header reading).
void DataSourceTest::test_mnist_LabeledDataSource() {
  CPPUNIT_ASSERT_MESSAGE(*gl_mnist_errmsg, gl_mnist_dir != NULL);
  string datafile = *gl_mnist_dir;
  string labelfile = *gl_mnist_dir;
  datafile += "/t10k-images-idx3-ubyte";
  labelfile += "/t10k-labels-idx1-ubyte";
  Idx<ubyte> data(1, 1, 1), labels(1);
  CPPUNIT_ASSERT(load_matrix<ubyte>(data, datafile.c_str()) == true);
  CPPUNIT_ASSERT(load_matrix<ubyte>(labels, labelfile.c_str()) == true);
  LabeledDataSource<ubyte,ubyte> ds(&data, &labels);
  state_idx datum(1, 28, 28);
  Idx<ubyte> label;
  for (int i = 0; i < 5; i++) {
    ds.fprop(&datum, &label);
    /* cout<<"Datum:"<<endl;
       datum.x.printElems();
       cout<<"Label: ";
       label.printElems();
       cout<<endl; */
    ds.next();
  }
  // briefly test some values of the 5th element of mnist
  CPPUNIT_ASSERT_EQUAL((unsigned int) 4, (unsigned int) label.get());
  CPPUNIT_ASSERT_EQUAL((unsigned int) 236, (unsigned int) datum.x.get(0, 9, 9));
}

void DataSourceTest::test_imageDirToIdx() {
  CPPUNIT_ASSERT_MESSAGE(*gl_data_errmsg, gl_data_dir != NULL);
#ifdef __BOOST__
  CPPUNIT_ASSERT(imageDirToIdx(gl_data_dir->c_str(), 48, ".*[.]ppm", 
			       NULL, "/tmp") == true);
#else
  CPPUNIT_ASSERT_MESSAGE("Not tested because of missing Boost libraries", 
			 false);
#endif
}
