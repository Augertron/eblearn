/***************************************************************************
 *   Copyright (C) 2013 by Pierre Sermanet *
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

// classify ////////////////////////////////////////////////////////////////////

template <typename T, typename Tdata, typename Tlabel>
int classify(configuration &conf, string &conffname, idx<Tdata> *inputs) {
  try {
		bool silent = true;
    timer titer, gtimer;
    gtimer.start(); // total running time
    string shortname = ebl::basename(conffname.c_str());
    if (conf.exists("meta_conf_shortname"))
      shortname = conf.get_string("meta_conf_shortname");
    uint              ipp_cores     = 1;
    if (conf.exists("ipp_cores")) ipp_cores = conf.get_uint("ipp_cores");
    ipp_init(ipp_cores, silent); // limit IPP (if available) to 1 core

    // load datasets
    uint noutputs = 0;
    labeled_datasource<T,Tdata,Tlabel> *ds = NULL;
    string valdata, traindata;
    ds = create_validation_set<T,Tdata,Tlabel>(conf, noutputs, valdata,
																							 silent);
    if (conf.exists_true("add_features_dimension")) {
      ds->add_features_dimension();
    }
    // create the trainable network
    ddparameter<T> theparam(1, silent);
    module_1_1<T> *net = NULL;
    trainable_module<T,Tdata,Tlabel> *machine =
        create_trainable_module<T,Tdata,Tlabel>(theparam, conf, noutputs,
																								&net, silent);
    // find out if jitter module is present
    jitter_module<T> *jitt = NULL;
    jitt = arch_find(net, jitt);
    if (jitt) jitt->disable(); // disable jitter for testing

    // learning parameters
    gd_param gdp;
    load_gd_param(conf, gdp, silent);

		// inference
		state<T> energy, dummy(noutputs);
		if (inputs) { // take inputs from input matrix
			idx_bloop1(e, *inputs, Tdata) {
				state<T> s(e.get_idxdim());
				idx_copy(e, s.x[0]);
				machine->fprop(s, dummy, energy);
				const state<T> &a = machine->compute_answers();
				cout << a.x.at_const(0).csv(true) << endl;
			}
		} else { // take inputs from datasource defined in conf
			ds->seek_begin();
			do {
				machine->fprop(*ds, energy);
				const state<T> &a = machine->compute_answers();
				cout << a.x.at_const(0).csv(true) << endl;
			} while (ds->next());
		}

    // free variables
    if (net) delete net;
    if (machine) delete machine;
#ifdef __GUI__
    if (!conf.exists_true("show_wait_user")) // wait for user to close windows
      quit_gui(); // close all windows
#endif
  } eblcatcherror();
  return 0;
}

// types selection functions ///////////////////////////////////////////////////

template <typename T, typename Tdata>
int select_label_type(configuration &conf, string &conffname,
											idx<Tdata> *inputs) {
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
      return classify<T,Tdata,int>(conf, conffname, inputs);
    break ;
    case MAGIC_FLOAT_MATRIX:
    case MAGIC_FLOAT_VINCENT:
      return classify<T,Tdata,float>(conf, conffname, inputs);
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
      cout << "classify is not compiled for labels with type " << type
					 << ", found in " << labels_fname << ", using int instead." << endl;
      return classify<T,Tdata,int>(conf, conffname, inputs);
    }
  } eblcatcherror();
  return -1;
}

template <typename T>
int select_data_type(configuration &conf, string &conffname,
										 idx<float> *inputs) {
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
      return select_label_type<T,float>(conf, conffname, inputs);
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
      return select_label_type<T,float>(conf, conffname, inputs);
    }
  } eblcatcherror();
  return -1;
}

// main ////////////////////////////////////////////////////////////////////////

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
#ifdef __GPROF__
  ProfilerStart("eblearn_train_google_perftools_profiler_dump");
#endif
  if (argc != 2 && argc != 3) {
    cout << "Usage: ./classify <config file> [input matrix]" << endl;
    eblerror("config file not specified");
  }
#ifdef __LINUX__
  feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
  try {
    string conffname = argv[1];
    if (!file_exists(conffname.c_str()))
      eblerror("configuration file not found: " << conffname);
    configuration conf(conffname, true, true, false); // configuration file
		if (conf.exists_true("show_configuration")) conf.pretty();
		dynamic_init_drand(argc, argv);
    // set current directory
    string curdir;
    curdir << dirname(argv[1]) << "/";
    conf.set("current_dir", curdir.c_str());
    conf.set("run_type", "train"); // tell conf that we are in train mode
    conf.resolve();
    if (conf.exists_true("show_conf")) conf.pretty();
    const char *precision = "double";
    if (conf.exists("training_precision"))
      precision = conf.get_cstring("training_precision");
		// open passed matrix if present
		idx<float> mat;
		idx<float> *inputs = NULL;
		if (argc > 2) {
			if (is_matrix(argv[2]))
				mat = load_matrix<float>(argv[2]);
			else // try csv format
				mat = load_csv_matrix<float>(argv[2], false, false, true, true);
			inputs = &mat;
		}
    // classify
    if (!strcmp(precision, "float"))
      return select_data_type<float>(conf, conffname, inputs);
    else if (!strcmp(precision, "double"))
      return select_data_type<double>(conf, conffname, inputs);
    else eblerror("unknown inference precision " << precision);
  } eblcatcherror();
#ifdef __GPROF__
  ProfilerStop();
#endif
  return 0;
}
