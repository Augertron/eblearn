#include "idxIO_test.h"

using namespace std;
using namespace ebl;

void idxIO_test::setUp() {
}

void idxIO_test::tearDown() {
}

template<class T> void test_save_load_matrix() {
  idx<T> m(9, 9);
  string fname= "/tmp/libeblearn_tester_matrix.mat";

  // initialize values
  double v = 0.1;
  { idx_aloop1(i, m, T) {
      *i = (T) v;
      v++;
    }
  }
  //m.printElems(); cout << endl;
  try {
    save_matrix(m, fname);
    idx<T> l = load_matrix<T>(fname);
    // test values
    v = 0.1;
    { idx_aloop1(i, l, T) {
	CPPUNIT_ASSERT_EQUAL((T)v, *i);
	v++;
      }
    }
  } catch(string &err) {
    cerr << err << endl;
    CPPUNIT_ASSERT(false); // error
  }
}

void idxIO_test::test_save_load_matrix_ubyte() {
  test_save_load_matrix<ubyte>();
}

void idxIO_test::test_save_load_matrix_int() {
  test_save_load_matrix<int>();
}

void idxIO_test::test_save_load_matrix_float() {
  test_save_load_matrix<float>();
}

void idxIO_test::test_save_load_matrix_double() {
  test_save_load_matrix<double>();
}

void idxIO_test::test_save_load_matrix_long() {
  idx<intg> test(3,3);
  test.set(2147483647, 0, 0);
  test.set(2147483646, 0, 1);
  test.set(2147483645, 0, 2);
  test.set(4, 1, 0);
  test.set(5, 1, 1);
  test.set(6, 1, 2);
  test.set(-2147483647, 2, 0);
  test.set(-2147483646, 2, 1);
  test.set(-2147483645, 2, 2);
  try {
    save_matrix(test, "/tmp/libeblearn_tester_matrix.mat");
    idx<long> m = load_matrix<long>("/tmp/libeblearn_tester_matrix.mat");
    { idx_aloop2(i, test, long, j, m, long) {
	CPPUNIT_ASSERT_EQUAL(*j, *i);
      }
    }
  } catch(string &err) {
    cerr << err << endl;
    CPPUNIT_ASSERT(false); // err
  }
}
