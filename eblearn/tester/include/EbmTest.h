#ifndef EBMTEST_H_
#define EBMTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "libeblearn.h"

//! Test class for Ebm class
class EbmTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(EbmTest);
  CPPUNIT_TEST(test_clayer_fprop);
  CPPUNIT_TEST(test_full_table);
  CPPUNIT_TEST(test_slayer);
  CPPUNIT_TEST(test_softmax);
  CPPUNIT_TEST(test_state_copy);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables

public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_clayer_fprop();
  void test_full_table();
  void test_slayer();
  void test_softmax();
  void test_state_copy();
};

#endif /* EBMTEST_H_ */
