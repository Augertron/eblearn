#include "GblTest.h"

using namespace std;
using namespace ebl;

void GblTest::setUp() {
}

void GblTest::tearDown() {
}

void GblTest::test_clayer_fprop() {
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
  stdsigmoid_module sqsh;
  parameter prm(10000);
  c_layer c(prm, ki, kj, 1, 1, &table, thick, si, sj, &sqsh);
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
  (c.kernel)->x.set(1 * fact, 0, 0, 0);
  (c.kernel)->x.set(2 * fact, 0, 0, 1);
  (c.kernel)->x.set(3 * fact, 0, 1, 0);
  (c.kernel)->x.set(4 * fact, 0, 1, 1);
  (c.bias)->x.set(-2.85, 0);

  c.fprop(&in, &out);

  /*
    cout << endl;
    (in.x).printElems(); cout << endl;
    (c.kernel)->x.printElems(); cout << endl;
    (c.bias)->x.printElems(); cout << endl;
    (out.x).printElems(); cout << endl;
    table.printElems(); cout << endl;
    cout << "thick: " << thick << endl;
  */

  CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.0, (out.x).get(0, 0, 0), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.551693, (out.x).get(0, 0, 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.551693, (out.x).get(0, 1, 0), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, (out.x).get(0, 1, 1), 0.000001);
}

void GblTest::test_full_table() {
  Idx<intg> m = full_table(2, 3);
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(0, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(0, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(1, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(1, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(2, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(2, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(3, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(3, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(4, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 2, m.get(4, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(5, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 2, m.get(5, 1));
}

void GblTest::test_softmax(){
  CPPUNIT_ASSERT_MESSAGE("TODO: Implement automatic test", false);

  state_idx *in = new state_idx(2,2,2,2,2,2);
  state_idx *out = new state_idx(1,1,1,1,1,1);
  double beta = 1;
  softmax *module = new softmax(beta);

  // init
  dseed(1);
  module->fprop(*in, *out);
  srand(time(NULL));
  idx_bloop2(i, in->x, double, o, out->x, double){
    idx_bloop2(ii, i, double, oo, o, double){
      idx_bloop2(iii, ii, double, ooo, oo, double){
	idx_bloop2(iiii, iii, double, oooo, ooo, double){
	  idx_bloop2(iiiii, iiii, double, ooooo, oooo, double){
	    idx_bloop2(iiiiii, iiiii, double, oooooo, ooooo, double){
	      iiiiii.set(drand((double)1));
	      oooooo.set(drand((double)1));
	    }
	  }
	}
      }
    }
  }

  // fprop, bprop, bbprop
  module->fprop(*in, *out);
  in->clear_dx();
  module->bprop(*in, *out);
  in->clear_ddx();
  module->bbprop(*in, *out);

  /*print
    printf(" Input\n");
    in->x.pretty(stdout);
    printf(" Output\n");
    out->x.pretty(stdout);
    printf(" Input dx\n");
    in->dx.pretty(stdout);
    printf(" Output dx\n");
    out->dx.pretty(stdout);
    printf(" Input ddx\n");
    in->ddx.pretty(stdout);
    printf(" Output ddx\n");
    out->ddx.pretty(stdout);
    printf("\n");
  */
  Idx<double> ib3 = in->x.select(0,0).select(0,0), calc_out = out->x.select(0,0).select(0,0);
  Idx<double> ib(new Srg<double>(), ib3.spec), des_out(new Srg<double>(), ib3.spec);
  idx_dotc(ib3, module->beta, ib);
  idx_exp(ib);
  double ib2 = 1/idx_sum(ib);
  idx_dotc(ib, ib2, des_out);
  //printf("Fprop error 1 : %3.3e \n", idx_sqrdist(calc_out, des_out));

  ib3 = in->x.select(0,1).select(0,0);
  calc_out = out->x.select(0,1).select(0,0);
  idx_dotc(ib3, module->beta, ib);
  idx_exp(ib);
  ib2 = 1/idx_sum(ib);
  idx_dotc(ib, ib2, des_out);
  //printf("Fprop error 2 :  %3.3e \n", idx_sqrdist(calc_out, des_out));

  ib3 = in->x.select(0,0).select(0,1);
  calc_out = out->x.select(0,0).select(0,1);
  idx_dotc(ib3, module->beta, ib);
  idx_exp(ib);
  ib2 = 1/idx_sum(ib);
  idx_dotc(ib, ib2, des_out);
  //printf("Fprop error 3 : %3.3e \n", idx_sqrdist(calc_out, des_out));

  ib3 = in->x.select(0,1).select(0,1);
  calc_out = out->x.select(0,1).select(0,1);
  idx_dotc(ib3, module->beta, ib);
  idx_exp(ib);
  ib2 = 1/idx_sum(ib);
  idx_dotc(ib, ib2, des_out);
  //printf("Fprop error 4 :  %3.3e \n", idx_sqrdist(calc_out, des_out));
  /*
    Bprop_tester *bproptest = new Bprop_tester();
    bproptest->test(module);

    Bbprop_tester *bbproptest = new Bbprop_tester();
    bbproptest->test(module);

    Jacobian_tester *test= new Jacobian_tester();
    test->test(module);
  */
  delete module;
  delete in;
  delete out;
}

void GblTest::test_state_copy() {
  state_idx a(4,4);

  dseed(32);
  idx_aloop3(xx,a.x,double,xd,a.dx,double,xdd,a.ddx,double){
    *xx = drand(2);
    *xd = drand(2);
    *xdd = drand(2);
  }

  state_idx b = a.make_copy();
  //	a.x.pretty(std::cout);
  //	a.x.printElems(std::cout);
  //	b.x.pretty(std::cout);
  //	b.x.printElems(std::cout);
  CPPUNIT_ASSERT(0 == idx_sqrdist(a.x,b.x));
  CPPUNIT_ASSERT(0 == idx_sqrdist(a.dx,b.dx));
  CPPUNIT_ASSERT(0 == idx_sqrdist(a.ddx,b.ddx));

  idx_addc(a.x,1.,a.x);
  idx_addc(a.dx,2.,a.dx);
  idx_addc(a.ddx,3.,a.ddx);
  CPPUNIT_ASSERT(a.x.footprint() == idx_sqrdist(a.x,b.x));
  CPPUNIT_ASSERT(4*a.dx.footprint() == idx_sqrdist(a.dx,b.dx));
  CPPUNIT_ASSERT(9*a.ddx.footprint() == idx_sqrdist(a.ddx,b.ddx));
}

