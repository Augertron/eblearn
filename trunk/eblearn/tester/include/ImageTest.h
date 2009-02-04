#ifndef IMAGETEST_
#define IMAGETEST_

#include <cppunit/extensions/HelperMacros.h>
#include "libidx.h"

//! Test class for Image class
class ImageTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(ImageTest);
  CPPUNIT_TEST(test_resize);
  CPPUNIT_TEST(test_pnm_P3);
  CPPUNIT_TEST(test_pnm_P6);
  CPPUNIT_TEST(test_yuvrgb);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_resize();
  void test_pnm_P3();
  void test_pnm_P6();
  void test_yuvrgb();
};
#endif /*IMAGETEST_*/
