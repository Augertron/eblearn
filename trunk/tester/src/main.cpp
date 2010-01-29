#include <fenv.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResultCollector.h>

#include "libeblearn.h"
#include "MyTextOutputter.h"

#include "BlasTest.h"
#include "ebl_basic_test.h"
#include "ebl_preprocessing_test.h"
#include "IdxIOTest.h"
#include "DataSourceTest.h"
#include "IdxTest.h"
#include "IdxIteratorsTest.h"
#include "detector_test.h"
#include "ClusterTest.h"
#include "image_test.h"
#include "NetTest.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace std;
using namespace CppUnit;

//! global variables
string *gl_mnist_dir = NULL;
string *gl_data_dir = NULL;
string *gl_mnist_errmsg = NULL;
string *gl_data_errmsg = NULL;
bool    color_print = true;

// Read arguments from ../run.init file in working directory
// (easiest way to configure the run within eclipse and without it).
void parse_args(int argc, char **argv) {
  gl_mnist_dir = NULL;
  gl_data_dir = NULL;
  gl_mnist_errmsg =
    new string("MNIST directory is unknown, some tests will be ignored.\
 Run configure.sh to fix it.");
  gl_data_errmsg =
    new string("Data directory is unknown, some tests will be ignored");
  string s;
  ifstream in("../run.init");
  if (!in)
    in.open("run.init");
  if (!in) {
    cerr << "Warning: failed to open ../run.init, please run configure.sh";
    cerr << endl;
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
  
  // Read arguments from shell input
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-nocolor") == 0) {
      color_print = false;
      cout << "Not using colors in shell." << endl;
    }
  }
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char**, argv) { 
#else
int main(int argc, char **argv) {
#endif
  // enable float exceptions to halt instead of propagating errors
  feenableexcept(FE_DIVBYZERO | FE_INVALID);
  //  gui.set_silent();
  cout << "***** Unit tester for libeblearn and libidx libraries *****" << endl;
  // parse arguments
  parse_args(argc, argv);
  cout << "***********************************************************" << endl;

  // cppunit tests
  CppUnit::BriefTestProgressListener listener;
  CppUnit::TestResultCollector collector;
  CppUnit::TextUi::TestRunner runner;
  runner.eventManager().addListener(&listener); // show each test
  runner.eventManager().addListener(&collector);
  // note: the custom outputter will be freed by the runner
  MyTextOutputter *outputter = new MyTextOutputter(&collector, cout);
  runner.setOutputter(outputter);

  // adding test suites
  runner.addTest(ClusterTest::suite());
  runner.addTest(IdxTest::suite());
  runner.addTest(IdxIteratorsTest::suite());
  runner.addTest(BlasTest::suite());
  runner.addTest(ebl_basic_test::suite());
  runner.addTest(ebl_preprocessing_test::suite());
  runner.addTest(IdxIOTest::suite());
  runner.addTest(image_test::suite());
  runner.addTest(DataSourceTest::suite());
  runner.addTest(detector_test::suite());
  runner.addTest(NetTest::suite());

  // run all tests
  runner.run();
  
  // print summary
  if (!collector.wasSuccessful()) {
    outputter->printHeader(); 
    cout << endl;
  }

  if (gl_mnist_dir) delete gl_mnist_dir;
  if (gl_data_dir) delete gl_data_dir;
  if (gl_mnist_errmsg) delete gl_mnist_errmsg;
  if (gl_data_errmsg) delete gl_data_errmsg;

  return 0;
}
