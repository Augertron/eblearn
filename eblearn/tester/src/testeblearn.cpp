#include <stdio.h>

#include "libeblearn.h"
#include "test_IdxIter.h"
#include "test_Idx.h"


using namespace ebl;

int testSrg() {
	// testing Srg
	printf("\n****************************************************************\n");
	printf("*** testing Srg\n");
	Srg<double> *s = new Srg<double>(10);
	printf("size=%d\n", (int)s->size());
	s->set(3, 42);
	printf("s->get(3)=%g\n", (double)s->get(3));
	s->changesize(8);
	printf("size=%d (should be 8)\n", (int)s->size());
	s->growsize(20);
	printf("size=%d (should be 20)\n", (int)s->size());
	s->growsize(10);
	printf("size=%d  (should be 10)\n", (int)s->size());
	printf("s->get(3)=%g\n", (double)s->get(3));
	s->changesize(0);
	printf("size=%d  (should be 0)\n", (int)s->size());
	s->changesize(30);
	printf("size=%d  (should be 30)\n", (int)s->size());
	return 0;
}

int testIdxSpec() {
	// testing IdxSpec
	printf("\n****************************************************************\n");
	printf("*** testing IdxSpec ***\n");
	IdxSpec *sp = new IdxSpec(5, 4, 3);
	printf("=== new/delete IdxSpec(5, 4, 3) =====\n");
	sp->pretty(stdout);
	delete sp;

	printf("==== stack spec1(0, 5, 6) ======\n");
	IdxSpec spec1(0, 5, 6);
	spec1.pretty(stdout);

	printf("==== stack spec2(0, 10) ======\n");
	IdxSpec spec2(0, 10);
	spec2.pretty(stdout);

	printf("==== spec1.select_into(&spec2,0,2) ======\n");
	spec1.select_into(&spec2, 0, 2);
	spec2.pretty(stdout);

	printf("==== spec1.select_into(&spec2,1,4) ======\n");
	spec1.select_into(&spec2, 1, 4);
	spec2.pretty(stdout);

	printf("==== spec1.transpose_into(&spec2,0,1) ======\n");
	spec1.transpose_into(&spec2, 0, 1);
	spec2.pretty(stdout);

	printf("==== spec1.unfold_into(&spec2,1,3,1) ======\n");
	spec1.unfold_into(&spec2, 1, 3, 1);
	spec2.pretty(stdout);

	printf("==== spec1.transpose_into(&spec2,{1,0}) ======\n");
	int p[2] = { 1, 0 };
	spec1.transpose_into(&spec2, p);
	spec2.pretty(stdout);

	printf("==== specu(0, 2, 3, 2, 4, 5, 1) ======\n");
	IdxSpec specu(0, 2, 3, 2, 4, 5, 1);
	specu.pretty(stdout);

	printf("done with IdxSpec\n");
	return 0;
}

int testIdx01() {
	// testing Idx
	printf("\n****************************************************************\n");
	printf("*** testing Idx01\n");

	printf("=== Idx<double> m0()\n");
	Idx<double> m0;
	m0.pretty(stdout);

	printf("=== Idx<int> m1((intg)5);\n");
	Idx<int> m1((intg)5);
	m1.pretty(stdout);

	printf("=== Idx<float> m2(3,4);\n");
	Idx<float> m2(3, 4);
	m2.pretty(stdout);

	printf("=== Idx<float> m3(2,4,3);\n");
	Idx<float> m3(2, 4, 3);
	m3.pretty(stdout);

	printf("=== Idx<float> m6(2,4,3,2,5,4);\n");
	Idx<float> m6(2, 4, 3, 2, 5, 4);
	m6.pretty(stdout);

	printf("=== Idx<double> *nm3 = new Idx<double>(2,4,3);\n");
	Idx<double> *nm3 = new Idx<double>(2,4,3);
	printf("delete Idx3\n");
	delete nm3;

	printf("=== Idx<double> ma(3,10,20);\n");
	Idx<double> ma(3, 10, 20);
	ma.pretty(stdout);

	printf("=== Idx<double> ms = ma.select(0,1);\n");
	Idx<double> ms = ma.select(0, 1);
	ms.pretty(stdout);

	printf("=== Idx<double> mn = ma.narrow(2,8,4);\n");
	Idx<double> mn = ma.narrow(2, 8, 4);
	mn.pretty(stdout);

	printf("=== Idx<double> mt = ma.transpose(1,2);\n");
	Idx<double> mt = ma.transpose(1, 2);
	mt.pretty(stdout);

	printf("=== Idx<double> mu = ma.unfold(2,6,2);\n");
	Idx<double> mu = ma.unfold(2, 6, 2);
	mu.pretty(stdout);

	printf("done\n");
	return 0;

}

int testIdx02() {
	// testing Idx
	printf("\n****************************************************************\n");
	printf("*** testIdx02\n");

	printf("=== Testing appending into Srg\n");
	{
		Srg<float> *srgptr = new Srg<float>(0);
		srgptr->pretty(stdout);
		Idx<float> m1(srgptr, srgptr->size(), 3, 4);
		srgptr->pretty(stdout);
		m1.pretty(stdout);
		Idx<float> m2(srgptr, srgptr->size(), 2, 5);
		srgptr->pretty(stdout);
		m2.pretty(stdout);
		Idx<float> m3(srgptr, srgptr->size(), 6);
		srgptr->pretty(stdout);
		m3.pretty(stdout);
	}

	printf("=== Testing resizing\n");
	{
		Idx<double> m(8, 3, 4, 5);
		m.pretty(stdout);
		m.resize(9, 4, 5, 6);
		m.pretty(stdout);
		m.resize(7, 2, 3, 4);
		m.pretty(stdout);
	}

	printf("testIdx02 done\n");
	return 0;
}

int testIdx03() {
	// testing Idx
	printf("\n****************************************************************\n");
	printf("*** testIdx03: Testing IdxIter and aloop macros\n");
	Idx<double> m(3, 4);
	Idx<double> p(4, 3);
	m.pretty(stdout);
	// this is for test purpose: this is
	// a bad way to loop over an Idx.
	// Use aloop macro instead.
	for (intg i0 = 0; i0<3; i0++) {
		for (intg i1 = 0; i1<4; i1++) {
			m.set(10*i0+i1, i0, i1);
			printf("m(%ld,%ld)=%g, ", i0, i1, m.get(i0, i1));
		}
		printf("\n");
	}

	printf("=== testing IdxIter on m (contiguous)\n");
#if USING_STL_ITERS == 1
	for( ScalarIter<double> iter(m); iter.notdone(); ++iter) {
		printf("%g ", *iter);
	}
#else
	double *idxptr;
	IdxIter<double> idx;
	for (idxptr = idx.init(m); idx.notdone(); idxptr = idx.next() ) {
		printf("%g ", *idxptr);
	}
	printf("\n");
#endif

	{
		printf("=== testing IdxIter on m.narrow(1,2,1) (non-contiguous)\n");
		Idx<double> z = m.narrow(1, 2, 1);
		z.pretty(stdout);
		// loop over all elements of an Idx
#if USING_STL_ITERS == 1
		for( ScalarIter<double> ziter(z); ziter.notdone(); ++ziter ) {
			printf("%g ", *ziter);
		}
#else
		double *zptr;
		IdxIter<double> ziter;
		for (zptr = ziter.init(z); ziter.notdone(); zptr = ziter.next() ) {
			printf("%g ", *zptr);
		}
		printf("\n");
		delete zptr;
#endif
	}

	{
		printf("=== testing idx_aloop1() on qq\n");
		Idx<double> qq(4, 5);
		//{ idx_aloop1(p,qq,double) { *p = 2 * p.i; } }
		int ii = 0;
		{
			idx_aloop1(p, qq, double) {
				*p = 2 * ++ii;
			}
		}
		{
			idx_aloop1(p, qq, double) {
				printf("%g ", *p);
			}
		}
		printf("\n");
	}

	{
		printf("=== testing idx_aloop2() on m and p\n");
		{
			idx_aloop2(lm, m, double, lp, p, double) {
				*lp = *lm;
			}
		}
		{
			idx_aloop1(lp, p, double) {
				printf("%g ", *lp);
			}
		}
		printf("\n");
	}
	printf("testIdx03 done\n");
	return 0;
}

int testIdx04() {
	// testing Idx
	printf("\n****************************************************************\n");
	printf("*** testIdx04: Testing IdxLooper and bloop/eloop macros\n");
	Idx<double> m(3, 4);
	Idx<double> p(3, 5);
	m.pretty(stdout);
	printf("=== idx_bloop1 over m\n");
	{
		idx_bloop1(lm, m, double) {
			lm.pretty(stdout);
		}
	}
	printf("=== idx_bloop2 over m and p\n");
	{
		idx_bloop2(lm, m, double, lp, p, double) {
			lm.pretty(stdout);
			lp.pretty(stdout);
		}
	}

	{
		printf("=== double bloop: filling\n");
		intg i = 0;
		std::cout<<"woo"<<std::endl;
		idx_bloop1(lm, m, double) {
			std::cout<<"in first bloop"<<std::endl;
			idx_bloop1(llm, lm, double) {
				//*llm = i++;
				std::cout<<i<<" "<<std::flush;
				llm.set(i++);
			}
		}
		printf("\n");
	}

	{
		printf("=== double bloop: printing\n");
		idx_bloop1(lm, m, double) {
			idx_bloop1(llm, lm, double) {
				//printf("%g, ",*llm);
				printf("%g, ", llm.get());
			}
			printf("\n");
		}
	}

	{
		printf("=== double eloop: printing\n");
		idx_eloop1(lm, m, double) {
			idx_eloop1(llm, lm, double) {
				//printf("%g, ",*llm);
				printf("%g, ", llm.get());
			}
			printf("\n");
		}
	}
	return 0;
}

int testBlas01() {
	printf("\n****************************************************************\n");
	printf("*** testBlas01\n");

	Idx<double> m1(2, 5);
	Idx<double> m2(2, 5);
#if USING_STL_ITERS
	int ii = 0;
	idx_aloop2(lm1,m1,double,lm2,m2,double) {*lm1 = *lm2 = ++ii;}
#else
	idx_aloop2(lm1, m1, double, lm2, m2, double) {
		*lm1 = *lm2 = (double)lm1.i;
	}
#endif
	printf("idx_dot(m1,m2)=%g (should be 285)\n", idx_dot(m1, m2));

	// non contiguous
	Idx<double> nm1 = m1.narrow(1, 3, 1);
	Idx<double> nm2 = m2.narrow(1, 3, 1);
	printf("idx_dot(nm1,nm2)=%g (should be 163)\n", idx_dot(nm1, nm2));

	printf("==== norm_columns(m1)\n");
	norm_columns(m1);
	idx_bloop1(lm, m1, double) {
		idx_bloop1(llm, lm, double) {
			printf("%g, ", llm.get());
		}
		printf("\n");
	}

	return 0;
}

int testBlas02() {
	printf("\n****************************************************************\n");
	printf("*** testBlas02\n");

	{
		printf("==== idx_m2dotm1(m,x,y)\n");
		double mdata[] = { 1000, 100, 10, 1, -1000, -100, -10, -1, 1e7, 1e6,
				1e5, 1e4 };
		// Idx<double> z(3,6);
		// Idx<double> m = z.narrow(1,4,1);
		Idx<double> z(4, 3);
		Idx<double> m = z.transpose(0, 1);
		double xdata[] = { 2, 3, 4, 6 };
		Idx<double> x(4);
		Idx<double> y(3);
		int i;
		i = 0;
		idx_aloop1(lm, m, double) {
			*lm = mdata[i++];
		}
		i = 0;
		idx_aloop1(lx, x, double) {
			*lx = xdata[i++];
		}
		m.pretty(stdout);
		idx_m2dotm1(m, x, y);
		y.pretty(stdout);
		idx_aloop1(ly, y, double) {
			printf("%g, ", *ly);
		}
	}

	{
		printf("==== idx_m1extm1(x,y,m)\n");
		Idx<double> m(3, 4);
		Idx<double> x(3);
		Idx<double> y(4);
		double i;
		i = 1;
		idx_aloop1(lx, x, double) {
			*lx = i++;
		}
		i = 1;
		idx_aloop1(ly, y, double) {
			*ly = i++;
		}
		idx_m1extm1(x, y, m);
		idx_aloop1(lm, m, double) {
			printf("%g, ", *lm);
		}
		printf("\n");
	}
	return 0;
}

/*
 int testEbm01() {
 printf("\n****************************************************************\n");
 printf("*** testEbm01\n");
 parameter p(100);
 printf("x:\n");
 p.x.pretty(stdout);
 printf("dx:\n");
 p.dx.pretty(stdout);
 printf("ddx:\n");
 p.ddx.pretty(stdout);
 printf("gradient:\n");
 p.gradient.pretty(stdout);

 return 0;
 }

 int testEbm02() {
 printf("\n****************************************************************\n");
 printf("*** testEbm02\n");

 double i;
 forget_param_linear fgp(1, 0.5);
 parameter p(100);
 double wdata[] = {1000,100,10,1, -1000,-100,-10,-1, 1e7,1e6,1e5,1e4};
 double bdata[] = {0.6,-0.7,8000};
 state_idx in(4,1,1);
 state_idx out(3,1,1);
 stdsigmoid_module sigmod;
 f_layer fl(&p,4,3,1,1,&sigmod);
 fl.forget(fgp);
 // { int i=0; idx_aloop1(lw,fl.weight->x,double) { *lw = wdata[i++]; } }
 // { int i=0; idx_aloop1(lw,fl.bias->x,double) { *lw = bdata[i++]; } }
 { i=1; idx_aloop1(lin,in.x,double) { *lin = i++; } }
 { i=1; idx_aloop1(lout,out.dx,double) { *lout = i++; } }
 { i=1; idx_aloop1(lout,out.ddx,double) { *lout = i++; } }
 printf("weight=\n");
 fl.weight->x.fdump(stdout);
 { printf("in.x: ["); idx_aloop1(lin,in.x,double) { printf("%g ", *lin); } printf("]\n"); }

 printf("fprop\n");
 fl.fprop(&in,&out);
 { printf("fl.sum->x: ["); idx_aloop1(lin,fl.sum->x,double) { printf("%g ", *lin); } printf("]\n"); }
 { printf("out.x: ["); idx_aloop1(lin,out.x,double) { printf("%g ",*lin); } printf("]\n"); }

 printf("bprop\n");
 fl.bprop(&in,&out);
 { printf("fl.sum->dx: ["); idx_aloop1(lin,fl.sum->dx,double) { printf("%g ",*lin); } printf("]\n"); }
 { printf("in.dx: ["); idx_aloop1(lin,in.dx,double) { printf("%g ",*lin); } printf("]\n"); }

 printf("bbprop\n");
 fl.bbprop(&in,&out);
 { printf("in.ddx: ["); idx_aloop1(lin,in.ddx,double) { printf("%g ",*lin); } printf("]\n"); }
 return 0;
 }
 */

#include <fstream>
#include <iostream>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/BriefTestProgressListener.h>

#include "BlasTest.h"
#include "EbmTest.h"
#include "IdxIOTest.h"
#include "DataSourceTest.h"
#include "NetTest.h"
#include "IdxTest.h"
#include "IdxIteratorsTest.h"
#include "Classifier2DTest.h"
#include "ImageTest.h"

using namespace std;

//! global variables
string *gl_mnist_dir = NULL;
string *gl_data_dir = NULL;
string *gl_mnist_errmsg = NULL;
string *gl_data_errmsg = NULL;

int main(int argc, char **argv)
{
  // Read arguments from run.init file in working directory
  // (easiest way to configure the run within eclipse and without it).
  gl_mnist_dir = NULL;
  gl_data_dir = NULL;
  gl_mnist_errmsg =
    new string("MNIST directory is unknown, some tests will be ignored. Run configure.sh to fix it.");
  gl_data_errmsg =
    new string("Data directory is unknown, some tests will be ignored");
  string s;
  ifstream in("run.init");
  if (!in) {
    cout << "Warning: failed to open run.init, please run configure.sh";
    cout << endl;
  }
  else {
    while (!in.eof()) {
      in >> s;
      if (s == "-mnist") {
	in >> s;
	gl_mnist_dir = new string(s.c_str());
	cout << "Using mnist database (" << *gl_mnist_dir << ")" << endl;
      }
      if (s == "-data") {
	in >> s;
	gl_data_dir = new string(s.c_str());
	cout << "Data directory: " << *gl_data_dir << endl;
      }
    }
  }
  in.close();
  if (gl_mnist_dir == NULL) {
    cout << "Warning: " << *gl_mnist_errmsg << endl;
  }
  if (gl_data_dir == NULL) {
    cout << "Warning: " << *gl_data_errmsg << endl;
  }
  cout << endl;

  // cppunit tests
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(IdxTest::suite());
  runner.addTest(IdxIteratorsTest::suite());
  runner.addTest(BlasTest::suite());
  runner.addTest(EbmTest::suite());
  runner.addTest(IdxIOTest::suite());
  runner.addTest(ImageTest::suite());
  runner.addTest(DataSourceTest::suite());
  runner.addTest(Classifier2DTest::suite());
  runner.addTest(NetTest::suite());
  // Shows a message as each test starts
  CppUnit::BriefTestProgressListener listener;
  runner.eventManager().addListener(&listener);
  // Run all tests
  runner.run();

  if (gl_mnist_dir) delete gl_mnist_dir;
  if (gl_data_dir) delete gl_data_dir;
  if (gl_mnist_errmsg) delete gl_mnist_errmsg;
  if (gl_data_errmsg) delete gl_data_errmsg;
  return 0;
}


int main2(int argc, char** argv) {
	if (false) {

		testSrg();
		testIdxSpec();

		testIdx01();
		testIdx02();
		testIdx03();
		testIdx04();

		testBlas01();
	}
	/*
	 if (false){
	 testEbm01();
	 dseed(32);
	 for (int i=0; i<20; i++) { printf("%g, ",drand()); }; printf("\n");
	 testEbm02();
	 }
	 */
	if (false) {
		//est_ElemIter_shifting();
		//test_ElemIter_parallelism();
		test_DimensionIterator_shifting();
		//test_matrixMultiply();
		//test_printElems();
	}

	if (false) {
		// test_LabeledDataSource();
	}
  return 0;
}



