#include "ebl_basic_test.h"

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;
using namespace ebl;

void ebl_basic_test::setUp() {
}

void ebl_basic_test::tearDown() {
}

void ebl_basic_test::test_nn_layer_convolution_fprop() {
  intg ini = 3;
  intg inj = 3;
  intg ki = 2;
  intg kj = 2;
  intg si = 1 + ini - ki;
  intg sj = 1 + inj - kj;
  state_idx<double> in(1, ini, inj);
  state_idx<double> out(1, si, sj);
  idx<intg> table(1, 2);
  idx_clear(table);
  idx<intg> tableout = table.select(1, 1);
  parameter<double> prm(10000);
  nn_layer_convolution<double> c(prm, ki, kj, 1, 1, table);
  double fact = 0.05;

  in.x.set(1, 0, 0, 0);
  in.x.set(2, 0, 0, 1);
  in.x.set(3, 0, 0, 2);
  in.x.set(4, 0, 1, 0);
  in.x.set(5, 0, 1, 1);
  in.x.set(6, 0, 1, 2);
  in.x.set(7, 0, 2, 0);
  in.x.set(8, 0, 2, 1);
  in.x.set(9, 0, 2, 2);
  c.convol.kernel.x.set(1 * fact, 0, 0, 0);
  c.convol.kernel.x.set(2 * fact, 0, 0, 1);
  c.convol.kernel.x.set(3 * fact, 0, 1, 0);
  c.convol.kernel.x.set(4 * fact, 0, 1, 1);
  c.adder.bias.x.set(-2.85, 0);

  c.fprop(in, out);

  // different values than c_layer because here we use tanh activation
  // function and c_layer uses the stdsigmoid.
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.761594, (out.x).get(0, 0, 0), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.462117, (out.x).get(0, 0, 1), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.462117, (out.x).get(0, 1, 0), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.761594, (out.x).get(0, 1, 1), 0.000001);
}

void ebl_basic_test::test_jacobian_nn_layer_convolution() {
  intg ini = 3;
  intg inj = 3;
  intg ki = 2;
  intg kj = 2;
  intg si = 1 + ini - ki;
  intg sj = 1 + inj - kj;
  state_idx<double> in(1, ini, inj);
  state_idx<double> out(1, si, sj);
  idx<intg> table(1, 2);
  idx_clear(table);
  idx<intg> tableout = table.select(1, 1);
  parameter<double> prm(10000);
  nn_layer_convolution<double> c(prm, ki, kj, 1, 1, table);

  ModuleTester<double> mt;
  idx<double> errs = mt.test_jacobian(c, in, out);
//   cout << "err0: " << errs.get(0) << " err1: " << errs.get(1);
//   cout << " thres " << mt.get_acc_thres();
  CPPUNIT_ASSERT(errs.get(0) < mt.get_acc_thres());
  CPPUNIT_ASSERT(errs.get(1) < mt.get_acc_thres());
}

void ebl_basic_test::test_jacobian_nn_layer_subsampling() {
  parameter<double> p(10000);
  int ki = 4, kj = 4, thick = 1, si = 2, sj =2;
  nn_layer_subsampling<double> s(p, ki, kj, si, sj, thick);
  state_idx<double> in(1, 8, 8);
  state_idx<double> out(1, 1, 1);

  ModuleTester<double> mt;
  idx<double> errs = mt.test_jacobian(s, in, out);
//   cout << "err0: " << errs.get(0) << " err1: " << errs.get(1);
//   cout << " thres " << mt.get_acc_thres();
  CPPUNIT_ASSERT(errs.get(0) < mt.get_acc_thres());
  CPPUNIT_ASSERT(errs.get(1) < mt.get_acc_thres());
}

void ebl_basic_test::test_state_copy() {
  state_idx<double> a(4,4);

  dseed(32);
  idx_aloop3(xx,a.x,double,xd,a.dx,double,xdd,a.ddx,double){
    *xx = drand(2);
    *xd = drand(2);
    *xdd = drand(2);
  }

  state_idx<double> b = a.make_copy();
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


void ebl_basic_test::test_softmax(){
  CPPUNIT_ASSERT_MESSAGE("TODO: Implement automatic test", false);

  state_idx<double> *in = new state_idx<double>(2,2,2,2,2,2);
  state_idx<double> *out = new state_idx<double>(1,1,1,1,1,1);
  double beta = 1;
  softmax<double> *module = new softmax<double>(beta);

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
  idx<double> ib3 = in->x.select(0,0).select(0,0), calc_out = out->x.select(0,0).select(0,0);
  idx<double> ib(new Srg<double>(), ib3.spec), des_out(new Srg<double>(), ib3.spec);
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

void ebl_basic_test::test_power_module() {
  state_idx<double> in(1);
  state_idx<double> out(1);
  power_module<double> pw(.5);

  in.x.set(2, 0);
  out.dx.set(1, 0);
  out.ddx.set(1, 0);
  pw.fprop(in, out);
  pw.bprop(in, out);
  pw.bbprop(in, out);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.4142, out.x.get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3536, in.dx.get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.125, in.ddx.get(0), 0.0001);
}

void ebl_basic_test::test_convolution_timing() {
  layers_n<float> l(true);
  parameter<float> p(1);
  idx<intg> tbl = full_table(1, 8);
  idx<intg> tbl2 = full_table(8, 16);
  l.add_module(new convolution_module_2D<float>(p, 9, 9, 1, 1, tbl),
	       new state_idx<float>(1,1,1));  
  l.add_module(new tanh_module<float>(),
	       new state_idx<float>(1,1,1));
  l.add_module(new convolution_module_2D<float>(p, 9, 9, 1, 1, tbl2),
	       new state_idx<float>(1,1,1));
  l.add_last_module(new tanh_module<float>());
  state_idx<float> in(1, 512, 512), out(16, 496, 496);
  struct timeval start, end;
  long seconds, useconds;
  gettimeofday(&start, NULL);
  for (uint i = 0; i < 1; ++i) {
    l.fprop(in, out);
  }
  gettimeofday(&end, NULL);
  seconds = end.tv_sec - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  cout << " big convolution time: "
       << (uint)(((seconds) * 1000 + useconds/1000.0)+0.5) << "ms";
}
