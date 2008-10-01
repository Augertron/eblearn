#include "IdxIteratorsTest.h"

using namespace ebl;

void IdxIteratorsTest::setUp() {
}

void IdxIteratorsTest::tearDown() {
}

void IdxIteratorsTest::test_scalariter() {
  Idx<double> m1(2, 2);
  Idx<double> m2(2, 2);
  double c = 42;
  idx_clear(m1);
  idx_clear(m2);
  m2.set(0.0, 0, 0);
  m2.set(42.0, 0, 1);
  m2.set(1.0, 1, 0);
  m2.set(-42.0, 1, 1);
  ScalarIter<double> pinp(m2); ScalarIter<double> pout(m1);
  idx_aloop2_on(pinp,m2,pout,m1) { *pout = *pinp + c; }
  //m1.printElems(); cout << endl;
  //m2.printElems(); cout << endl;
  CPPUNIT_ASSERT_EQUAL(42.0, m1.get(0, 0));
  CPPUNIT_ASSERT_EQUAL(84.0, m1.get(0, 1));
  CPPUNIT_ASSERT_EQUAL(43.0, m1.get(1, 0));
  CPPUNIT_ASSERT_EQUAL(0.0,  m1.get(1, 1));
}
