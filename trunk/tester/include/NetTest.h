#ifndef NETTEST_H_
#define NETTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "libeblearn.h"

//! Test class for Blas class
class NetTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(NetTest);
  CPPUNIT_TEST(test_lenet5_mnist);
  CPPUNIT_TEST(test_full_table);
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

#endif /* NETTEST_H_ */
