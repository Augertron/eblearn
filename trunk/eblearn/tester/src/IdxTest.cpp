#include "IdxTest.h"

using namespace std;
using namespace ebl;

void IdxTest::setUp() {
}

void IdxTest::tearDown() {
}

void IdxTest::test_get(){
	Idx<double> t0;
	Idx<double> t1(2);
	Idx<double> t2(2,2);
	Idx<double> t3(2,2,2);
	Idx<double> t4(2,2,2,2);
	Idx<double> t5(2,2,2,2,2);
	Idx<double> t6(2,2,2,2,2,2);
	Idx<double> t7(2,2,2,2,2,2,2);
	Idx<double> t8(2,2,2,2,2,2,2,2);
	t0.set(1);
	t1.set(1, 0);
	t2.set(1, 0, 1);
	t3.set(1, 0, 1, 0);
	t4.set(1, 0, 1, 0, 1);
	t5.set(1, 0, 1, 0, 1, 0);
	t6.set(1, 0, 1, 0, 1, 0, 1);
	t7.set(1, 0, 1, 0, 1, 0, 1, 0);
	t8.set(1, 0, 1, 0, 1, 0, 1, 0, 1);
	/*
	printf("the results should always be 1 :\n");
	printf("Idx dim = 0 : %f \n", t0.get());
	printf("Idx dim = 1 : %f \n", t1.get(0));
	printf("Idx dim = 2 : %f \n", t2.get(0, 1));
	printf("Idx dim = 3 : %f \n", t3.get(0, 1, 0));
	printf("Idx dim = 4 : %f \n", t4.get(0, 1, 0, 1));
	printf("Idx dim = 5 : %f \n", t5.get(0, 1, 0, 1, 0));
	printf("Idx dim = 6 : %f \n", t6.get(0, 1, 0, 1, 0, 1));
	printf("Idx dim = 7 : %f \n", t7.get(0, 1, 0, 1, 0, 1, 0));
	printf("Idx dim = 8 : %f \n", t8.get(0, 1, 0, 1, 0, 1, 0, 1));
	*/
}
