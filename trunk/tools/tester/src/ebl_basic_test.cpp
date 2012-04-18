#include "ebl_basic_test.h"

using namespace std;
using namespace ebl;

void ebl_basic_test::setUp() {
}

void ebl_basic_test::tearDown() {
}

void ebl_basic_test::test_convolution_layer_fprop() {
  intg ini = 3;
  intg inj = 3;
  idxdim ker(2,2);
  idxdim stride(1,1);
  intg si = 1 + ini - ker.dim(0);
  intg sj = 1 + inj - ker.dim(1);
  fstate_idx<double> in(1, ini, inj);
  fstate_idx<double> out(1, si, sj);
  idx<intg> table(1, 2);
  idx_clear(table);
  idx<intg> tableout = table.select(1, 1);
  parameter<fs(double)> prm(10000);
  convolution_layer<fs(double)> c(&prm, ker, stride, table);
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

void ebl_basic_test::test_state_copy() {
  bbstate_idx<double> a(4,4);

  dseed(32);
  idx_aloop3(xx,a.x,double,xd,a.dx,double,xdd,a.ddx,double){
    *xx = drand(2);
    *xd = drand(2);
    *xdd = drand(2);
  }

  bbstate_idx<double> b = a.make_copy();
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

  fstate_idx<double> *in = new fstate_idx<double>(2,2,2,2,2,2);
  fstate_idx<double> *out = new fstate_idx<double>(1,1,1,1,1,1);
  double beta = 1;
  softmax<fs(double)> *module = new softmax<fs(double)>(beta);

  // init
  dseed(1);
  module->fprop(*in, *out);
  dynamic_init_drand();
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
  idx<double> ib3 = in->x.select(0,0).select(0,0),
    calc_out = out->x.select(0,0).select(0,0);
  idx<double> ib(new srg<double>(), ib3.spec),
    des_out(new srg<double>(), ib3.spec);
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

void ebl_basic_test::test_convolution_timing() {
  layers<fs(double)> l(true);
  idx<intg> tbl = full_table(1, 8);
  idx<intg> tbl2 = full_table(8, 16);
  idxdim ker(9,9);
  idxdim stride(1,1);
  l.add_module(new convolution_module<fs(double)>(NULL, ker, stride, tbl));
  l.add_module(new tanh_module<fs(double)>());
  l.add_module(new convolution_module<fs(double)>(NULL, ker, stride, tbl2));
  l.add_module(new tanh_module<fs(double)>());
  //convolution_module<fs(double)> l2(NULL, ker, stride, tbl);
  fstate_idx<double> in(1, 512, 512), out(16, 496, 496);
  timer t;
  t.start();
  for (uint i = 0; i < 10; ++i) {
    l.fprop(in, out);
  }
  long tim = t.elapsed_milliseconds();
  cout << " big convolution time: " << tim/10 << "ms";
}

#define FLOAT_THRESHOLD 1e-3
#define DOUBLE_THRESHOLD 1e-5

#define TEST_DERIVATIVES(module, in, out, T, thresh) {	\
    module_tester<T> mt(thresh);			\
    TEST_DERIVATIVES_MT(mt, module, in, out) }

#define TEST_DERIVATIVES_MT(mtester, module, in, out)			\
  idx<double> errs;							\
  errs = mtester.test_jacobian(module, in, out);			\
  EDEBUG("max err: " << errs.get(0) << " total err: " << errs.get(1));	\
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0, errs.get(0), mtester.get_acc_thres()); \
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0, errs.get(1), mtester.get_acc_thres()); \
  //   errs = mtester.test_hessian(s, in, out); 
//   CPPUNIT_ASSERT(errs.get(0) < mtester.get_acc_thres());
//   CPPUNIT_ASSERT(errs.get(1) < mtester.get_acc_thres());

void ebl_basic_test::test_convolution_module_float() {
  typedef float T;
  idxdim ker(7,7);
  idxdim stride(1,1);
  bbstate_idx<T> in(2, 10, 10), out;
  idx<intg> table = full_table(2, 3);
  parameter<T> prm(10000);
  convolution_module<T> c(&prm, ker, stride, table);
  TEST_DERIVATIVES(c, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_convolution_module_double() {
  typedef double T;
  idxdim ker(7,7);
  idxdim stride(1,1);
  bbstate_idx<T> in(2, 10, 10), out;
  idx<intg> table = full_table(2, 3);
  parameter<T> prm(10000);
  convolution_module<T> c(&prm, ker, stride, table);
  TEST_DERIVATIVES(c, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_subsampling_module_float() {
  typedef float T;
  parameter<T> p(10000);
  idxdim kd(4, 4), sd(2, 2);
  subsampling_module<T> s(&p, 2, kd, sd);
  bbstate_idx<T> in(2, 8, 8), out;
  TEST_DERIVATIVES(s, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_subsampling_module_double() {
  typedef double T;
  parameter<T> p(10000);
  idxdim kd(4, 4), sd(2, 2);
  subsampling_module<T> s(&p, 2, kd, sd);
  bbstate_idx<T> in(2, 8, 8), out;
  TEST_DERIVATIVES(s, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_wavg_pooling_module_float() {
  typedef double T;
  parameter<T> p(10000);
  idxdim kd(5, 5), sd(3, 3);
  wavg_pooling_module<T> s(2, kd, sd);
  bbstate_idx<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(s, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_wavg_pooling_module_double() {
  typedef double T;
  parameter<T> p(10000);
  idxdim kd(5, 5), sd(3, 3);
  wavg_pooling_module<T> s(2, kd, sd);
  bbstate_idx<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(s, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_l2pooling_module_float() {
  typedef double T;
  parameter<T> p(10000);
  idxdim kd(5, 5), sd(3, 3);
  lppooling_module<T> s(2, kd, sd);
  bbstate_idx<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(s, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_l2pooling_module_double() {
  typedef double T;
  parameter<T> p(10000);
  idxdim kd(5, 5), sd(3, 3);
  lppooling_module<T> s(2, kd, sd);
  bbstate_idx<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(s, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_sqrt_power_module_float() {
  typedef float T;
  // test by hand
  bbstate_idx<T> in(1);
  bbstate_idx<T> out(1);
  power_module<bs(T)> pw(.5);
  in.x.set(2, 0);
  out.dx.set(1, 0);
  out.ddx.set(1, 0);
  pw.fprop(in, out);
  pw.bprop(in, out);
  pw.bbprop(in, out);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.4142, out.x.get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3536, in.dx.get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.125, in.ddx.get(0), 0.0001);
  // sqrt
  { parameter<T> p(10000);
    power_module<T> m(.5);
    bbstate_idx<T> in(2, 11, 11), out;
    module_tester<T> mt(FLOAT_THRESHOLD, 1e-4, 2);
    TEST_DERIVATIVES_MT(mt, m, in, out)
  }
}

void ebl_basic_test::test_sqrt_power_module_double() {
  // test by hand
  bbstate_idx<double> in(1);
  bbstate_idx<double> out(1);
  power_module<fs(double)> pw(.5);
  in.x.set(2, 0);
  out.dx.set(1, 0);
  out.ddx.set(1, 0);
  pw.fprop(in, out);
  pw.bprop(in, out);
  pw.bbprop(in, out);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(1.4142, out.x.get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3536, in.dx.get(0), 0.0001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.125, in.ddx.get(0), 0.0001);
  // sqrt
  { typedef double T;
    parameter<T> p(10000);
    power_module<T> m(.5);
    bbstate_idx<T> in(2, 11, 11), out;
    module_tester<T> mt(DOUBLE_THRESHOLD, 1e-4, 2);
    TEST_DERIVATIVES_MT(mt, m, in, out)
  }
}
  
void ebl_basic_test::test_power2_module_float() {
  // square
  typedef float T;
  parameter<T> p(10000);
  power_module<T> m(2);
  bbstate_idx<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_power2_module_double() {
  // square
  typedef double T;
  parameter<T> p(10000);
  power_module<T> m(2);
  bbstate_idx<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_power_inv_module_float() {
  // inverse
  typedef float T;
  parameter<T> p(10000);
  power_module<T> m(-1);
  bbstate_idx<T> in(2, 11, 11), out;
  module_tester<T> mt(FLOAT_THRESHOLD, .1, 2);
  TEST_DERIVATIVES_MT(mt, m, in, out)
}

void ebl_basic_test::test_power_inv_module_double() {
  // inverse
  typedef double T;
  parameter<T> p(10000);
  power_module<T> m(-1);
  bbstate_idx<T> in(2, 11, 11), out;
  module_tester<T> mt(DOUBLE_THRESHOLD, .1, 2);
  TEST_DERIVATIVES_MT(mt, m, in, out)
}

void ebl_basic_test::test_convolution_layer() {
  intg ini = 3;
  intg inj = 3;
  idxdim ker(2,2);
  idxdim stride(1,1);
  intg si = 1 + ini - ker.dim(0);
  intg sj = 1 + inj - ker.dim(1);
  bbstate_idx<double> in(1, ini, inj);
  bbstate_idx<double> out(1, si, sj);
  idx<intg> table(1, 2);
  idx_clear(table);
  idx<intg> tableout = table.select(1, 1);
  parameter<double> prm(10000);
  convolution_layer<double> c(&prm, ker, stride, table);
  TEST_DERIVATIVES(c, in, out, double, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_subsampling_layer() {
  parameter<double> p(10000);
  idxdim kd(4, 4), sd(2, 2);
  subsampling_layer<double> s(&p, 1, kd, sd);
  bbstate_idx<double> in(1, 8, 8);
  bbstate_idx<double> out(1, 1, 1);
  TEST_DERIVATIVES(s, in, out, double, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_addc_module_float() {
  typedef float T;
  parameter<T> p(10000);
  addc_module<T> m(&p, 10);
  bbstate_idx<T> in(10, 11, 11), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_addc_module_double() {
  typedef double T;
  parameter<T> p(10000);
  addc_module<T> m(&p, 10);
  bbstate_idx<T> in(10, 11, 11), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_linear_module_float() {
  typedef float T;
  parameter<T> p(10000);
  linear_module<T> m(&p, 10, 100);
  bbstate_idx<T> in(10, 1, 1), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_linear_module_double() {
  typedef double T;
  parameter<T> p(10000);
  linear_module<T> m(&p, 10, 100);
  bbstate_idx<T> in(10, 1, 1), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_subtractive_norm_module_double() {
  typedef double T;
  parameter<T> p(10000);
  idxdim kd(7, 7);
  // subtractive_norm_module<T> m1(kd, 2);
  subtractive_norm_module<T> m2(kd, 2, false, false, NULL, "", false);
  // subtractive_norm_module<T> m2(kd, 2, false);
  // subtractive_norm_module<T> m3(kd, 2, true, false);
  bbstate_idx<T> in(2, 11, 11), out;
  // TEST_DERIVATIVES(m1, in, out, T, DOUBLE_THRESHOLD)
  TEST_DERIVATIVES(m2, in, out, T, DOUBLE_THRESHOLD)
  // TEST_DERIVATIVES(m3, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_divisive_norm_module_double() {
  typedef double T;
  parameter<T> p(10000);
  idxdim kd(7, 7);
  divisive_norm_module<T> m1(kd, 2, false, false, NULL, "", false);
  divisive_norm_module<T> m2(kd, 2, true, false, NULL, "", false);
  divisive_norm_module<T> m3(kd, 2, true, false, NULL, "", true);
  bbstate_idx<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(m1, in, out, T, DOUBLE_THRESHOLD)
  TEST_DERIVATIVES(m2, in, out, T, DOUBLE_THRESHOLD)
  TEST_DERIVATIVES(m3, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_contrast_norm_module_double() {
  typedef double T;
  parameter<T> p(10000);
  idxdim kd(7, 7);
  contrast_norm_module<T> m1(kd, 2, false, false, false, NULL, "", false);
  contrast_norm_module<T> m2(kd, 2, false, false, false, NULL, "", true);
  bbstate_idx<T> in(2, 11, 11), out;
  TEST_DERIVATIVES(m1, in, out, T, DOUBLE_THRESHOLD)
  TEST_DERIVATIVES(m2, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_fsum_module_float() {
  typedef float T;
  fsum_module<T> m;
  bbstate_idx<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_fsum_module_double() {
  typedef double T;
  fsum_module<T> m1(false);
  fsum_module<T> m2(true);
  fsum_module<T> m3(false, 3);
  fsum_module<T> m4(true, 3);
  bbstate_idx<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m1, in, out, T, DOUBLE_THRESHOLD)
  TEST_DERIVATIVES(m2, in, out, T, DOUBLE_THRESHOLD)
  TEST_DERIVATIVES(m3, in, out, T, DOUBLE_THRESHOLD)
  TEST_DERIVATIVES(m4, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_zpad_module_float() {
  typedef float T;
  idxdim d(7, 7);
  zpad_module<T> m(d);
  bbstate_idx<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_zpad_module_double() {
  typedef double T;
  idxdim d(7, 7);
  zpad_module<T> m(d);
  bbstate_idx<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_mirrorpad_module_float() {
  typedef float T;
  idxdim d(7, 7);
  mirrorpad_module<T> m(d);
  bbstate_idx<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_mirrorpad_module_double() {
  typedef double T;
  idxdim d(7, 7);
  mirrorpad_module<T> m(d);
  bbstate_idx<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_thres_module_float() {
  typedef float T;
  thres_module<T> m(.2, .5);
  bbstate_idx<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, FLOAT_THRESHOLD)
}

void ebl_basic_test::test_thres_module_double() {
  typedef double T;
  thres_module<T> m(.2, .5);
  bbstate_idx<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
}

void ebl_basic_test::test_tanh_shrink_module_float() {
  eblerror("not implemented");
}

void ebl_basic_test::test_tanh_shrink_module_double() {
  typedef double T;
  tanh_shrink_module<T> m(NULL, 5);
  bbstate_idx<T> in(5, 10, 10), out;
  TEST_DERIVATIVES(m, in, out, T, DOUBLE_THRESHOLD)
}

