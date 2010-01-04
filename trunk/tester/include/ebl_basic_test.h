#ifndef EBL_BASIC_TEST_H_
#define EBL_BASIC_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "ebl_basic.h"
#include "ebl_layers.h"
#include "ebl_tester.h"

//! Test class for Ebm class
class ebl_basic_test : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE(ebl_basic_test);
  CPPUNIT_TEST(test_nn_layer_convolution_fprop);
/*   CPPUNIT_TEST(test_full_table); */
/*   CPPUNIT_TEST(test_softmax); */
/*   CPPUNIT_TEST(test_state_copy); */
/*   CPPUNIT_TEST(test_Ebm01); */
/*   CPPUNIT_TEST(test_Ebm02); */
  CPPUNIT_TEST(test_jacobian_nn_layer_convolution);
  CPPUNIT_TEST(test_jacobian_nn_layer_subsampling);
  //  CPPUNIT_TEST(test_softmax); // TODO: fix test
  CPPUNIT_TEST(test_state_copy);
  CPPUNIT_TEST(test_power_module);
  CPPUNIT_TEST_SUITE_END();

private:
  // member variables

public:
  //! This function is called before each test function is called.
  void setUp();
  //! This function is called after each test function is called.
  void tearDown();

  // Test functions
  void test_nn_layer_convolution_fprop();
/*   void test_full_table(); */
/*   void test_softmax(); */
/*   void test_state_copy(); */
/*   void test_Ebm01(); */
/*   void test_Ebm02(); */
  void test_jacobian_nn_layer_convolution();
  void test_jacobian_nn_layer_subsampling();
  void test_softmax();
  void test_state_copy();
  void test_power_module();
};

#endif /* EBL_BASIC_TEST_H_ */
