#ifndef IDXITERATORSTEST_H_
#define IDXITERATORSTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "libeblearn.h"

//! Test class for IdxIterators class
class IdxIteratorsTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(IdxIteratorsTest);
  CPPUNIT_TEST(test_scalariter);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_scalariter();
};

#endif /* IDXITERATORSTEST_H_ */
