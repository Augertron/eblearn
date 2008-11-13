/*
 * spIdxTest.cpp
 *
 *      Author: cyril Poulet
 */

#include "spIdxTest.h"

void spIdxTest::test_set(){
	intg dims[2] = {4, 4};
	spIdx<intg> sptest(0, 2, dims);

	sptest.pretty(cout);
	sptest.printElems();

	sptest.set(36, 3, 2);
	sptest.pretty(cout);
	sptest.printElems();

	sptest.set(36, 2, 1);
	sptest.pretty(cout);
	sptest.printElems();

	sptest.set(27, 3, 2);
	sptest.pretty(cout);
	sptest.printElems();

	sptest.set(36, 5, 2);
	sptest.pretty(cout);
	sptest.printElems();

	sptest.set(0, 3, 2);
	sptest.pretty(cout);
	sptest.printElems();

	sptest.set(0, 2, 1);
	sptest.pretty(cout);
	sptest.printElems();

	sptest.set(0, 2, 1);
	sptest.pretty(cout);
	sptest.printElems();
}

void spIdxTest::test_get(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(36, 3, 2);
	sptest.set(24, 1, 2);

	cout << sptest.get(3,2) << "\n";
	cout << sptest.get(1,2) << "\n";
	cout << sptest.get(1,1) << "\n";
	cout << sptest.get(5,2) << "\n";
	cout << sptest.get(3,2) << "\n";
}

void spIdxTest::test_const(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(36, 3, 2);
	sptest.set(24, 1, 2);

	spIdx<intg> sptest2(sptest);
	sptest2.get(3,2);
	sptest2.set(3, 2, 2);
	sptest.get(2,2);

	sptest.pretty(cout);
	cout << "\n";
	sptest.printElems();
	cout << "\n";
	sptest2.pretty(cout);
	cout << "\n";
	sptest2.printElems();
	cout << "\n";
}

void spIdxTest::test_narrow(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 1, 1);
	sptest.set(2, 2, 2);
	sptest.set(3, 3, 3);
	sptest.set(4, 0, 0);

	sptest.printElems();

	spIdx<intg> sptest2 = sptest.narrow(0, 1, 1);
	sptest2.printElems();
	sptest2.pretty(cout);

	sptest2 = sptest.narrow(0, 1, 2);
	sptest2.printElems();
	sptest2.pretty(cout);

	sptest2 = sptest.narrow(0, 3, 1);
	sptest2.printElems();
	sptest2.pretty(cout);

	sptest2 = sptest.narrow(0, 5, 1);
	sptest2.printElems();
	sptest2.pretty(cout);

	sptest2 = sptest.narrow(1, 0, 0);
	sptest2.printElems();
	sptest2.pretty(cout);

	sptest2 = sptest.narrow(1, 2, 1);
	sptest2.printElems();
	sptest2.pretty(cout);
}

void spIdxTest::test_select(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 1, 1);
	sptest.set(2, 2, 2);
	sptest.set(3, 3, 3);
	sptest.set(4, 0, 0);
	sptest.set(10, 3, 2);

	sptest.printElems();

	spIdx<intg> sptest2 = sptest.select(0, 1);
	sptest2.printElems();
	sptest2.pretty(cout);

	sptest2 = sptest.select(0, 3);
	sptest2.printElems();
	sptest2.pretty(cout);

	sptest2 = sptest.select(1, 3);
	sptest2.printElems();
	sptest2.pretty(cout);

	sptest2 = sptest.select(1, 2);
	sptest2.printElems();
	sptest2.pretty(cout);

	spIdx<intg> sptest3 = sptest2.select(1, 2);
	sptest3.printElems();
	sptest3.pretty(cout);
}

void spIdxTest::test_transpose(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	sptest.printElems();

	spIdx<intg> sptest2 = sptest.transpose(0, 1);
	sptest2.printElems();
	sptest2.pretty(cout);

	spIdx<intg> sptest3 = sptest.transpose(0, 2);

	int p[2] = {1, 0};
	spIdx<intg> sptest4 = sptest.transpose(p);
	sptest4.printElems();
	sptest4.pretty(cout);
}

void spIdxTest::test_resize(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(2, 1, 2);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);

	sptest.printElems();

	sptest.resize(2, 3);
	sptest.printElems();
	sptest.pretty(cout);

	sptest.resize(0, 3);
	sptest.resize(2, 5, 6);
	sptest.resize( 4, 4);
	sptest.printElems();
	sptest.pretty(cout);
}

void spIdxTest::test_sort(){
	spIdx<intg> sptest(0, 4, 4);
	sptest.set(1, 0, 1);
	sptest.set(3, 2, 3);
	sptest.set(4, 3, 0);
	sptest.set(2, 1, 2);

	sptest.sort();
	sptest.printElems();
	sptest.pretty();
}
