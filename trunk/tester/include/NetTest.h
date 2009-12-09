#ifndef NETTEST_H_
#define NETTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "libeblearn.h"
#include "libidx.h"

//! Test class for Blas class
class NetTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(NetTest);
  CPPUNIT_TEST(test_full_table);
  CPPUNIT_TEST(test_lenet5_mnist);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_lenet5_mnist();
  void test_full_table();
};

 namespace ebl{

  template<class T>
    void draw_matrix42(idx<T> &im, unsigned int h0 = 0, unsigned int w0 = 0,
		     double zoomh = 1.0, double zoomw = 1.0,
		     T minv = 0, T maxv = 0);
}


#endif /* NETTEST_H_ */
