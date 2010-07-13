/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResultCollector.h>

#include "libidx.h"
#include "libeblearn.h"
#include "MyTextOutputter.h"

#include "idxops_test.h"
#include "ebl_basic_test.h"
#include "ebl_preprocessing_test.h"
#include "idxIO_test.h"
#include "DataSourceTest.h"
#include "IdxTest.h"
#include "IdxIteratorsTest.h"
#include "detector_test.h"
#include "ClusterTest.h"
#include "image_test.h"
#include "NetTest.h"

#ifdef __DEBUGMEM__
  INIT_DEBUGMEM();
#endif
  
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
#ifdef __WINDOWS__
bool    color_print = false;
#else
bool    color_print = true;
#endif

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
#ifdef __LINUX__
  // enable float exceptions to halt instead of propagating errors
  feenableexcept(FE_DIVBYZERO | FE_INVALID);
#endif
  ebl::ipp_init(1); // limit IPP (if available) to 1 core
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
  runner.addTest(idxops_test::suite());
  runner.addTest(idxIO_test::suite());
  runner.addTest(image_test::suite());
  runner.addTest(ebl_preprocessing_test::suite());
  runner.addTest(DataSourceTest::suite());
  runner.addTest(ebl_basic_test::suite());
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
