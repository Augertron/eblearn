#ifndef DATASOURCETEST_H_
#define DATASOURCETEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "libeblearn.h"

//! Test class for IdxIO class
class DataSourceTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(DataSourceTest);
  CPPUNIT_TEST(test_mnist_LabeledDataSource);
  CPPUNIT_TEST(test_imageDirToIdx);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_mnist_LabeledDataSource();
  void test_imageDirToIdx();
};

#endif /* DATASOURCETEST_H_ */
