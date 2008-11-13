/*
 * spBlasTest.cpp
 *
 *      Author: cyril Poulet
 */

#include "spBlasTest.h"

void spBlasTest::test_copy(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<int> sptest2(0, 4, 4);
	idx_copy(sptest, sptest2);

	sptest.printElems();
	sptest.pretty(cout);
	sptest2.printElems();
	sptest2.pretty(cout);

	spIdx<ubyte> sptest3(0, 4, 4);
	idx_copy(sptest, sptest3);

	spIdx<double> sptest4(0, 4, 4);
	idx_copy(sptest, sptest4);

	spIdx<double> sptest5(0, 4, 4, 6);
	idx_copy(sptest, sptest5);

	spIdx<double> sptest6(0, 3, 4);
	idx_copy(sptest, sptest6);
}

void spBlasTest::test_copy2(){
	spIdx<intg> sptest(0, 4, 4, 4);
	sptest.set(1, 0, 1, 2);
	sptest.set(2, 1, 2, 3);
	sptest.set(3, 2, 3, 0);
	sptest.set(4, 3, 0, 1);

	Idx<int> test(4,4, 4);
	idx_copy(sptest, test);
	test.printElems();

	Idx<int> test2(4, 5, 4);
	idx_copy(sptest, test2);

	Idx<int> test3(4, 4);
	idx_copy(sptest, test3);
}

void spBlasTest::test_copy3(){
	Idx<double> test(2,2);
	test.set(1.5, 0, 0);
	test.set(2.25, 0, 1);
	test.set(-1.5, 1, 0);
	test.set(-1, 1, 1);

	spIdx<int> sptest(0,2,2);
	idx_copy(test, sptest);

	sptest.printElems();
	sptest.pretty();

	Idx<double> test2(2,2);
	test2.set(1.5, 0, 0);
	test2.set(-2.5, 1, 0);

	spIdx<ubyte> sptest2(0, 2, 2);
	idx_copy(test2, sptest2);

	sptest2.printElems();
	sptest2.pretty();
}

void spBlasTest::test_clear(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	idx_clear(sptest);
	sptest.printElems();
	sptest.pretty();
}

void spBlasTest::test_minus(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<intg> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 2, 3);
	sptest2.set(4, 3, 0);

	idx_minus(sptest, sptest2);
	sptest2.printElems();
	sptest2.pretty();

	spIdx<intg> sptest3(0, 4, 3);
	sptest3.set(2, 1, 2);
	sptest3.set(3, 2, 1);
	sptest3.set(4, 3, 0);

	idx_minus(sptest, sptest3);
	sptest3.printElems();
	sptest3.pretty();
}

void spBlasTest::test_inv(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 2, 3);
	sptest2.set(4, 3, 0);

	idx_inv(sptest, sptest2);
	sptest2.printElems();
	sptest2.pretty();
}
void spBlasTest::test_add(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	spIdx<double> sptest3(0,4,4);

	idx_add(sptest, sptest2, sptest3);
	sptest3.printElems();
	sptest3.pretty();
}

void spBlasTest::test_sub(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	spIdx<double> sptest3(0,4,4);

	idx_sub(sptest, sptest2, sptest3);
	sptest3.printElems();
	sptest3.pretty();
}

void spBlasTest::test_mul(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	spIdx<double> sptest3(0,4,4);

	idx_mul(sptest, sptest2, sptest3);
	sptest3.printElems();
	sptest3.pretty();
}

void spBlasTest::test_addc(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	idx_addc(sptest, (double)-3, sptest2);
	sptest2.printElems();
	sptest2.pretty();
}

void spBlasTest::test_addcacc(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	idx_addcacc(sptest, (double)-3, sptest2);
	sptest2.printElems();
	sptest2.pretty();
}

void spBlasTest::test_dotc(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	idx_dotc(sptest, (double)-3, sptest2);
	sptest2.printElems();
	sptest2.pretty();

	spIdx<double> sptest3(0, 4, 4);

	idx_dotc(sptest, (double)0, sptest2);
	sptest3.printElems();
	sptest3.pretty();
}

void spBlasTest::test_dotcacc(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	idx_dotcacc(sptest, (double)-3, sptest2);
	sptest2.printElems();
	sptest2.pretty();

}

void spBlasTest::test_subsquare(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	spIdx<double> sptest3(0, 4, 4);
	idx_subsquare(sptest, sptest2, sptest3);
	sptest3.printElems();
	sptest3.pretty();
}

void spBlasTest::test_lincomb(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	spIdx<double> sptest3(0, 4, 4);
	idx_lincomb(sptest, (double)-3, sptest2, (double)2, sptest3);
	sptest3.printElems();
	sptest3.pretty();

	spIdx<double> sptest4(0, 4, 4);
	idx_lincomb(sptest, (double)0, sptest2, (double)0, sptest4);
	sptest4.printElems();
	sptest4.pretty();
}

void spBlasTest::test_abs(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(-2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(-4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	idx_abs(sptest, sptest2);
	sptest2.printElems();
	sptest2.pretty();
}

void spBlasTest::test_indexmax(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(-2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(-4, 3, 0);

	cout << idx_indexmax(sptest);
}

void spBlasTest::test_sqrdist(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4);
	sptest2.set(2, 1, 2);
	sptest2.set(3, 1, 3);

	cout << "\n" << idx_sqrdist(sptest, sptest2) << "\n";

	Idx<double> res;
	idx_sqrdist(sptest, sptest2, res);
	cout << res.get();
}

void spBlasTest::test_m2dotm1(){
	spIdx<float> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	Idx<float> x(4), y(4);
	x.set(0, 0);
	x.set(1,1);
	x.set(2,2);
	x.set(3,3);

	idx_m2dotm1(sptest, x, y);
	y.printElems();
}


void spBlasTest::test_m2dotm1sp(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> x(0, 4), y(0, 4);
	x.set(0, 0);
	x.set(1,1);
	x.set(2,2);
	x.set(3,3);

	idx_m2dotm1(sptest, x, y);
	y.printElems();
}

void spBlasTest::test_m2dotm1acc(){
	spIdx<float> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	Idx<float> x(4), y(4);
	x.set(0, 0);
	x.set(1,1);
	x.set(2,2);
	x.set(3,3);
	y.set(-16, 0);
	y.set(1,1);
	y.set(2,2);
	y.set(3,3);

	idx_m2dotm1acc(sptest, x, y);
	y.printElems();
}

void spBlasTest::test_m2dotm1accsp(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> x(0, 4), y(0, 4);
	x.set(0, 0);
	x.set(1,1);
	x.set(2,2);
	x.set(3,3);
	y.set(-16, 0);
	y.set(1,1);
	y.set(2,2);
	y.set(3,3);

	idx_m2dotm1acc(sptest, x, y);
	y.printElems();
}

void spBlasTest::test_m2extm2(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4, 4, 4);
	idx_m2extm2(sptest, sptest, sptest2);

	sptest2.printElems();
	sptest2.pretty();

	spIdx<double> sptest3(0, 4, 4, 4, 3);
	idx_m2extm2(sptest, sptest, sptest3);
}

void spBlasTest::test_m2extm2acc(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4, 4, 4);
	sptest2.set(10, 0, 0, 0, 0);
	sptest2.set(10, 0, 1, 0, 1);
	idx_m2extm2acc(sptest, sptest, sptest2);

	sptest2.printElems();
	sptest2.pretty();
}

void spBlasTest::test_m2squextm2acc(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> sptest2(0, 4, 4, 4, 4);
	sptest2.set(10, 0, 0, 0, 0);
	sptest2.set(10, 0, 1, 0, 1);
	idx_m2squextm2acc(sptest, sptest, sptest2);

	sptest2.printElems();
	sptest2.pretty();
}


void spBlasTest::test_m2squdotm2(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	Idx<double> res;
	idx_m2squdotm2(sptest, sptest, res);

	res.printElems();

}

void spBlasTest::test_m2squdotm2acc(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	Idx<double> res;
	res.set(15);
	idx_m2squdotm2acc(sptest, sptest, res);

	res.printElems();
}

void spBlasTest::test_m1extm1(){
	spIdx<double> sptest(0, 5);
	sptest.set(1, 1);
	sptest.set(5, 3);
	sptest.set(2, 2);

	spIdx<double> sptest2(0, 6);
	sptest2.set(1, 1);
	sptest2.set(5, 5);
	sptest2.set(2, 2);

	spIdx<double> res(0,5,6);
	idx_m1extm1(res, sptest, sptest2);

	res.printElems();
	res.pretty();
}

void spBlasTest::test_m1extm1acc(){
	spIdx<double> sptest(0, 5);
	sptest.set(1, 1);
	sptest.set(5, 3);
	sptest.set(2, 2);

	spIdx<double> sptest2(0, 6);
	sptest2.set(1, 1);
	sptest2.set(5, 5);
	sptest2.set(2, 2);

	spIdx<double> res(0,5,6);
	res.set(15, 0, 0);
	res.set(10, 2, 2);
	idx_m1extm1acc(res, sptest, sptest2);

	res.printElems();
	res.pretty();
}

void spBlasTest::test_m2squdotm1(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	Idx<double> x(4), y(4);
	x.set(0, 0);
	x.set(1,1);
	x.set(2,2);
	x.set(3,3);

	idx_m2squdotm1(sptest, x, y);
	y.printElems();
}

void spBlasTest::test_m2squdotm1sp(){
	spIdx<float> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<float> x(0,4), y(0, 4);
	x.set(0, 0);
	x.set(1,1);
	x.set(2,2);
	x.set(3,3);

	idx_m2squdotm1(sptest, x, y);
	y.printElems();
}

void spBlasTest::test_m2squdotm1acc(){
	spIdx<float> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	Idx<float> x(4), y(4);
	x.set(0, 0);
	x.set(1,1);
	x.set(2,2);
	x.set(3,3);
	y.set(-16, 0);
	y.set(1,1);
	y.set(2,2);
	y.set(3,3);

	idx_m2squdotm1acc(sptest, x, y);
	y.printElems();
}

void spBlasTest::test_m2squdotm1accsp(){
	spIdx<double> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(5, 0, 3);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	spIdx<double> x(0, 4), y(0, 4);
	x.set(0, 0);
	x.set(1,1);
	x.set(2,2);
	x.set(3,3);
	y.set(-16, 0);
	y.set(1,1);
	y.set(2,2);
	y.set(3,3);

	idx_m2squdotm1acc(sptest, x, y);
	y.printElems();
}
void spBlasTest::test_m1squextm1(){
	spIdx<double> sptest(0, 5);
	sptest.set(1, 1);
	sptest.set(5, 3);
	sptest.set(2, 2);

	spIdx<double> sptest2(0, 6);
	sptest2.set(1, 1);
	sptest2.set(5, 5);
	sptest2.set(2, 2);

	spIdx<double> res(0,5,6);
	idx_m1squextm1(res, sptest, sptest2);

	res.printElems();
	res.pretty();
}

void spBlasTest::test_m1squextm1acc(){
	spIdx<double> sptest(0, 5);
	sptest.set(1, 1);
	sptest.set(5, 3);
	sptest.set(2, 2);

	spIdx<double> sptest2(0, 6);
	sptest2.set(1, 1);
	sptest2.set(5, 5);
	sptest2.set(2, 2);

	spIdx<double> res(0,5,6);
	res.set(15, 0, 0);
	res.set(10, 2, 2);
	idx_m1squextm1acc(res, sptest, sptest2);

	res.printElems();
	res.pretty();
}

void spBlasTest::test_normcolumn(){
	spIdx<double> sptest(0, 5);
	sptest.set(1, 1);
	sptest.set(5, 3);
	sptest.set(2, 2);

	spIdx<double> sptest2(0, 6);
	sptest2.set(1, 1);
	sptest2.set(5, 5);
	sptest2.set(2, 2);

	spIdx<double> res(0,5,6);
	idx_m1squextm1(res, sptest, sptest2);
	norm_columns(res);
	res.printElems();
	res.pretty();
}

void spBlasTest::test_2dconvol(){
	Idx<double> test(10,10);
	test.set(1, 0, 0);
	test.set(3, 0, 5);
	test.set(5, 3, 3);
	test.set(1, 3, 9);
	test.set(2, 6, 2);
	test.set(3, 6, 6);
	test.set(7, 7, 6);
	test.set(-4, 7, 7);
	test.set(6, 9, 4);

	Idx<double> kernel(3,3);
	kernel.set(-1, 0, 1);
	kernel.set(-1, 1, 0);
	kernel.set(-1, 1, 2);
	kernel.set(-1, 2, 1);
	kernel.set(4, 1, 1);

	Idx<double> res1(8, 8);

	idx_2dconvol(test, kernel, res1);
	res1.printElems();

	spIdx<double> test2(0,10,10);
	test2.set(1, 0, 0);
	test2.set(3, 0, 5);
	test2.set(5, 3, 3);
	test2.set(1, 3, 9);
	test2.set(2, 6, 2);
	test2.set(3, 6, 6);
	test2.set(7, 7, 6);
	test2.set(-4, 7, 7);
	test2.set(6, 9, 4);

	spIdx<double> res2(0, 8, 8);

	idx_2dconvol(test2, kernel, res2, true);
	res2.printElems();
}
