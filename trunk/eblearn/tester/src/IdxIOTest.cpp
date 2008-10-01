#include "IdxIOTest.h"

using namespace std;
using namespace ebl;

void IdxIOTest::setUp() {
}

void IdxIOTest::tearDown() {
}
	
template<class T> void test_save_load_matrix() {
	Idx<T> m(9, 9);
	Idx<T> l(1, 1);
	string fname= "/tmp/libeblearn_tester_matrix.mat";
	
	// initialize values
	double v = 0.1;
	{ idx_aloop1(i, m, T) { 
		*i = (T) v;
		v++;
	  }
	}
	//m.printElems(); cout << endl;
	CPPUNIT_ASSERT(save_matrix(m, fname.c_str()) == true);
	CPPUNIT_ASSERT(load_matrix<T>(l, fname.c_str()) == true);
	//l.printElems(); cout << endl;
	// test values
	v = 0.1;
	{ idx_aloop1(i, l, T) { 
		CPPUNIT_ASSERT_EQUAL((T)v, *i);
		v++;
	  }
	}
}

void IdxIOTest::test_save_load_matrix_ubyte() {
	test_save_load_matrix<ubyte>();
}

void IdxIOTest::test_save_load_matrix_int() {
	test_save_load_matrix<int>();
}

void IdxIOTest::test_save_load_matrix_float() {
	test_save_load_matrix<float>();
}

void IdxIOTest::test_save_load_matrix_double() {
	test_save_load_matrix<double>();
}
