/*
 * spIdxIOTest.cpp
 *
 *      Author: cyril Poulet
 */

#include "spIdxIOTest.h"

#include <iostream>
#include <fstream>


void spIdxIOTest::test_saveandload_spidx(){
	spIdx<double> test(0,3,3);
	test.set(1, 0, 0);
	test.set(4, 1, 0);
	test.set(6, 1, 2);
	test.set(7, 2, 0);
	test.set(8, 2, 1);

	save_matrix(test, "/home/cyril/Bureau/test.txt");
	spIdx<double> test2(0,3,3);
	load_matrix(test2, "/home/cyril/Bureau/test.txt");
	test2.printElems();
	test2.pretty();

}
