#ifndef IDXTEST_
#define IDXTEST_

#include <cppunit/extensions/HelperMacros.h>
#include "libeblearn.h"

//! Test class for IdxIO class
class IdxTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(IdxTest);
  CPPUNIT_TEST(test_get);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables
  
public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_get();
};
#endif /*IDXTEST_*/
