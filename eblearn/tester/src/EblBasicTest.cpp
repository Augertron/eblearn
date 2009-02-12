#include "EblBasicTest.h"

using namespace std;
using namespace ebl;

void EblBasicTest::setUp() {
}

void EblBasicTest::tearDown() {
}

void EblBasicTest::test_nn_layer_convolution_fprop() {
  intg ini = 3;
  intg inj = 3;
  intg ki = 2;
  intg kj = 2;
  intg si = 1 + ini - ki;
  intg sj = 1 + inj - kj;
  state_idx in(1, ini, inj);
  state_idx out(1, si, sj);
  Idx<intg> table(1, 2);
  idx_clear(table);
  Idx<intg> tableout = table.select(1, 1);
  intg thick = 1 + idx_max(tableout);
  parameter prm(10000);
  nn_layer_convolution c(&prm, ki, kj, 1, 1, &table, thick);
  double fact = 0.05;

  (in.x).set(1, 0, 0, 0);
  (in.x).set(2, 0, 0, 1);
  (in.x).set(3, 0, 0, 2);
  (in.x).set(4, 0, 1, 0);
  (in.x).set(5, 0, 1, 1);
  (in.x).set(6, 0, 1, 2);
  (in.x).set(7, 0, 2, 0);
  (in.x).set(8, 0, 2, 1);
  (in.x).set(9, 0, 2, 2);
  (c.convol->kernel)->x.set(1 * fact, 0, 0, 0);
  (c.convol->kernel)->x.set(2 * fact, 0, 0, 1);
  (c.convol->kernel)->x.set(3 * fact, 0, 1, 0);
  (c.convol->kernel)->x.set(4 * fact, 0, 1, 1);
  (c.adder->bias)->x.set(-2.85, 0);

  c.fprop(&in, &out);

  // different values than c_layer because here we use tanh activation
  // function and c_layer uses the stdsigmoid.
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.761594, (out.x).get(0, 0, 0), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.462117, (out.x).get(0, 0, 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.462117, (out.x).get(0, 1, 0), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.761594, (out.x).get(0, 1, 1), 0.000001);
}

void EblBasicTest::test_jacobian_nn_layer_convolution() {
  intg ini = 3;
  intg inj = 3;
  intg ki = 2;
  intg kj = 2;
  intg si = 1 + ini - ki;
  intg sj = 1 + inj - kj;
  state_idx in(1, ini, inj);
  state_idx out(1, si, sj);
  Idx<intg> table(1, 2);
  idx_clear(table);
  Idx<intg> tableout = table.select(1, 1);
  intg thick = 1 + idx_max(tableout);
  parameter prm(10000);
  nn_layer_convolution c(&prm, ki, kj, 1, 1, &table, thick);

  ModuleTester mt;
  Idx<double> errs = mt.test_jacobian(&c, &in, &out);
//   cout << "err0: " << errs.get(0) << " err1: " << errs.get(1);
//   cout << " thres " << mt.get_acc_thres();
  CPPUNIT_ASSERT(errs.get(0) < mt.get_acc_thres());
  CPPUNIT_ASSERT(errs.get(1) < mt.get_acc_thres());
}

void EblBasicTest::test_jacobian_nn_layer_subsampling() {
  parameter p(10000);
  int ki = 4, kj = 4, thick = 1, si = 2, sj =2;
  nn_layer_subsampling s(&p, ki, kj, si, sj, thick);
  state_idx in(1, 8, 8);
  state_idx out(1, 1, 1);

  ModuleTester mt;
  Idx<double> errs = mt.test_jacobian(&s, &in, &out);
//   cout << "err0: " << errs.get(0) << " err1: " << errs.get(1);
//   cout << " thres " << mt.get_acc_thres();
  CPPUNIT_ASSERT(errs.get(0) < mt.get_acc_thres());
  CPPUNIT_ASSERT(errs.get(1) < mt.get_acc_thres());
}

