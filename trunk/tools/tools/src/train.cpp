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

#include <iomanip>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include "libidx.h"
#include "libeblearn.h"
#include "libeblearntools.h"
#include "eblapp.h"

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#ifdef __GUI__
#include "libeblearngui.h"
#endif

#ifdef __GPROF__
#include <google/profiler.h>
#endif

#define bbs2 Tnet,bbstate_idx<Tnet>
#define bbsds Tnet,Tdata,Tlabel,bbstate_idx<Tnet>
#define bbms3 Tnet,bbstate_idx<Tnet>,mstate<bbstate_idx<Tnet> >

// train ///////////////////////////////////////////////////////////////////////

template <typename Tnet, typename Tdata, typename Tlabel>
int train(configuration &conf, string &conffname) {
  try {
    timer titer, gtimer;
    gtimer.start(); // total running time
    string shortname = ebl::basename(conffname.c_str());
    cout << "Varied variables:" << endl; conf.pretty_match(conffname);
    if (conf.exists("meta_conf_shortname"))
      shortname = conf.get_string("meta_conf_shortname");
    bool test_only = conf.exists_true("test_only");
    uint original_tests = 1; // number of tests to run initially
    if (test_only) // in testing mode only, allow several tests for averaging
      original_tests = conf.get_uint("test_only");
    uint              ipp_cores     = 1;
    if (conf.exists("ipp_cores")) ipp_cores = conf.get_uint("ipp_cores");
    ipp_init(ipp_cores); // limit IPP (if available) to 1 core
    intg nhessian = conf.exists("ndiaghessian") ? 
      conf.get_int("ndiaghessian") : 100;
    intg hessian_period = conf.exists("hessian_period") ?
      conf.get_int("hessian_period") : 4000;

    //! load datasets
    uint noutputs = 0;
    labeled_datasource<Tnet,Tdata,Tlabel> *train_ds = NULL;
    labeled_datasource<Tnet,Tdata,Tlabel> *test_ds = NULL;
    string valdata, traindata;
    test_ds = create_validation_set<Tnet,Tdata,Tlabel>(conf, noutputs, valdata);
    if (!test_only) 
      train_ds = create_training_set<Tnet,Tdata,Tlabel>(conf, noutputs,
							traindata);

    answer_module<bbsds> *answer = create_answer<bbsds>(conf, noutputs);
    if (!answer) eblerror("no answer module found");
    cout << "Answering module: " << answer->describe() << endl;
    // update number of outputs given the answer module
    noutputs = answer->get_nfeatures();
    intg inthick = conf.exists("input_thickness") ?
      conf.get_int("input_thickness") : -1;
    //! create the network weights, network and trainer
    idxdim dims(test_ds->sample_dims()); // get order and dimensions of sample
    parameter<Tnet> theparam;// create trainable parameter
    module_1_1<Tnet> *net =
      create_network<bbs2 >(theparam, conf, inthick, noutputs, "arch");
    if (!net) eblerror("failed to create network");
    if (((layers<Tnet>*)net)->size() == 0) eblerror("0 modules in network");
    trainable_module<bbsds> *train =
      create_trainer<bbsds>(conf, *train_ds, *net, *answer);
    supervised_trainer<Tnet,Tdata,Tlabel> thetrainer(*train, theparam);
    thetrainer.set_progress_file(job::get_progress_filename());
    // initialize the network weights
    forget_param_linear fgp(1, 0.5);
    uint iter = 0;
    if (conf.exists_true("retrain")) {
      if (!conf.exists("retrain_weights")) 
	eblerror("retrain_weights variable not defined");
      // concatenate weights if multiple ones
      vector<string> w =
	string_to_stringvector(conf.get_string("retrain_weights"));
      theparam.load_x(w);
      if (conf.exists("retrain_iteration")) {
	iter = std::max(0, conf.get_int("retrain_iteration") - 1);
	thetrainer.set_iteration(iter - 1);
      }
    } else { 
	cout << "Initializing weights from random." << endl;
	train->forget(fgp);
    }
    if (!conf.exists_true("retrain") && conf.exists_true("manual_load"))
      manually_load_network(*((layers<bbs2 >*)net), conf);

    // a classifier-meter measures classification errors
    classifier_meter trainmeter, testmeter;
    trainmeter.init(noutputs);
    testmeter.init(noutputs);
    // find out if jitter module is present
    jitter_module<bbs2 > *jitt = NULL;
    jitt = arch_find(net, jitt);
    if (jitt) jitt->disable(); // disable jitter for testing

    // learning parameters
    gd_param gdp;
    load_gd_param(conf, gdp);
    infer_param infp;
	
    if (test_only) { // testing mode
      cout << "Test only mode..." << endl;
      test(iter++, conf, conffname, theparam, thetrainer, *train_ds, 
	   *test_ds, trainmeter, testmeter, infp, gdp, shortname);
      cout << "Testing only mode, stopping." << endl;
    } else { // training mode
      // first show classification results without training
      test_and_save(iter++, conf, conffname, theparam, thetrainer, *train_ds, 
		    *test_ds, trainmeter, testmeter, infp, gdp, shortname);

      // now do training iterations 
      cout << "Training network with " << train_ds->size()
	   << " training samples and " << test_ds->size() <<" val samples for " 
	   << conf.get_uint("iterations") << " iterations:" << endl;
      ostringstream name, fname;
      for ( ; iter <= conf.get_uint("iterations"); ++iter) {
	cout << "__ epoch " << iter << " ______________"
	     <<"________________________________________________________"<<endl;
	titer.restart();
	// train
	if (jitt) jitt->enable(); // enable jitter for testing
	thetrainer.train(*train_ds, trainmeter, gdp, 1, infp,
			 hessian_period, nhessian, .02); // train
	// test and save
	if (jitt) jitt->disable(); // disable jitter for testing
	test_and_save(iter, conf, conffname, theparam, thetrainer, *train_ds, 
		      *test_ds, trainmeter, testmeter, infp, gdp, shortname,
		      titer.elapsed_seconds());
	cout << "iteration_minutes=" << titer.elapsed_minutes() << endl;
	cout << "iteration_time="; titer.pretty_elapsed(); cout << endl;
	cout << "timestamp=" << tstamp() << endl;
      }
    }
    if (!test_only && iter > conf.get_uint("iterations"))
      job::write_finished(); // declare job finished
    else if (conf.exists_true("save_confusion")) {
      string fname; fname << shortname << "_confusion_test.mat";
      cout << "saving confusion to " << fname << endl;
      save_matrix(testmeter.get_confusion(), fname.c_str());
    } 
    // free variables
    if (net) delete net;
#ifdef __GUI__
    if (!conf.exists_true("show_wait_user")) // wait for user to close windows
      quit_gui(); // close all windows
#endif
    cout << "Training done. Running time: " << gtimer.elapsed() << endl;
  } eblcatcherror();
  return 0;
}

// types selection functions ///////////////////////////////////////////////////

template <typename Tnet, typename Tdata>
int select_label_type(configuration &conf, string &conffname) {
  string labels_fname = conf.get_string("val_labels");
  string type;
  try {
    switch (get_matrix_type(labels_fname.c_str(), type)) {
    // case MAGIC_BYTE_MATRIX:
    // case MAGIC_UBYTE_VINCENT:
    //   return train<Tdata, ubyte>(conf, conffname);
    //   break ;
    case MAGIC_INTEGER_MATRIX:
    case MAGIC_INT_VINCENT:
      return train<Tnet,Tdata,int>(conf, conffname);
    break ;
    case MAGIC_FLOAT_MATRIX:
    case MAGIC_FLOAT_VINCENT:
      return train<Tnet,Tdata,float>(conf, conffname);
      break ;
    // case MAGIC_DOUBLE_MATRIX:
    // case MAGIC_DOUBLE_VINCENT:
    //   return train<Tdata, double>(conf, conffname);
    //   break ;
    // case MAGIC_LONG_MATRIX:
    //   return train<Tdata, long>(conf, conffname);
    // break ;
    // case MAGIC_UINT_MATRIX:
    //   return train<Tdata, uint>(conf, conffname);
    //   break ;
    default:
      cout << "train is not compiled for labels with type " << type 
	   << ", found in " << labels_fname << ", using int instead." << endl;
      return train<Tnet,Tdata,int>(conf, conffname);
    }
  } eblcatcherror();
  return -1;
}

template <typename Tnet>
int select_data_type(configuration &conf, string &conffname) {
  string data_fname = conf.get_string("val");
  string type;
  try {
    switch (get_matrix_type(data_fname.c_str(), type)) {
    // case MAGIC_BYTE_MATRIX:
    // case MAGIC_UBYTE_VINCENT:
    //   return select_label_type<ubyte>(conf, conffname);
    //   break ;
    // case MAGIC_INTEGER_MATRIX:
    // case MAGIC_INT_VINCENT:
    //   return select_label_type<int>(conf, conffname);
    // break ;
    case MAGIC_FLOAT_MATRIX:
    case MAGIC_FLOAT_VINCENT:
      return select_label_type<Tnet,float>(conf, conffname);
      break ;
    // case MAGIC_DOUBLE_MATRIX:
    // case MAGIC_DOUBLE_VINCENT:
    //   return select_label_type<double>(conf, conffname);
    //   break ;
    // case MAGIC_LONG_MATRIX:
    //   return select_label_type<long>(conf, conffname);
    // break ;
    // case MAGIC_UINT_MATRIX:
    //   return select_label_type<uint>(conf, conffname);
    //   break ;
    default:
      cout << "train is not compiled for data with type " << type 
	   << ", found in " << data_fname << ", using float instead." << endl;
      return select_label_type<Tnet,float>(conf, conffname);
    }
  } eblcatcherror();
  return -1;
}

///////////////////////////////////////////////////////////////////////////////
// main

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
#ifdef __GPROF__
  ProfilerStart("eblearn_train_google_perftools_profiler_dump");
#endif
  cout << "* Generic trainer" << endl;
  if (argc != 2) {
    cout << "Usage: ./train <config file>" << endl;
    eblerror("config file not specified");
  }
#ifdef __LINUX__
  feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
  string conffname = argv[1];
  configuration conf(conffname, true, true, false); // configuration file
  if (conf.exists_true("fixed_randomization"))
    cout << "Using fixed seed: " << fixed_init_drand() << endl;
  else
    cout << "Using random seed: " << dynamic_init_drand(argc, argv) << endl;
  // set current directory
  string curdir;
  curdir << dirname(argv[1]) << "/";
  cout << "Setting conf directory to: " << curdir << endl;
  conf.set("current_dir", curdir.c_str());
  conf.resolve();
  if (conf.exists_true("show_conf")) conf.pretty();
  const char *precision = "double";
  if (conf.exists("training_precision"))
    precision = conf.get_cstring("training_precision");
  cout << "Training precision: " << precision << endl;
  // train
  if (!strcmp(precision, "float"))
    return select_data_type<float>(conf, conffname);
  else if (!strcmp(precision, "double"))
    return select_data_type<double>(conf, conffname);
  else eblerror("unknown training precision " << precision);
#ifdef __GPROF__
  ProfilerStop();
#endif
  return -1;
}
