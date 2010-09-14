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

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

typedef double t_net; // precision at which network is trained (ideally double)

// temporary code
void koray_temp(configuration &conf, layers<t_net> &net) {
  ostringstream m;
  idx<t_net> mat;
  m.str(""); m << conf.get_string("koray_dir")
	       << conf.get_string("wkoray") << "_conv0_kernel.x.mat";
  mat = load_matrix<t_net>(m.str());
  idx_copy(mat, ((convolution_module<t_net>*)((*net.modules)[0]))->kernel.x);

  m.str(""); m << conf.get_string("koray_dir")
	       << conf.get_string("wkoray") << "_bias0_bias.x.mat";
  mat = load_matrix<t_net>(m.str());
  idx_copy(mat, ((addc_module<t_net>*)((*net.modules)[1]))->bias.x);
  if (conf.exists_bool("use_shrink")) {
    m.str(""); m << conf.get_string("koray_dir")
		 << conf.get_string("wkoray") << "_shrink0_bias.x.mat";
    mat = load_matrix<t_net>(m.str());
    idx_copy(mat, ((smooth_shrink_module<t_net>*)((*net.modules)[2]))->bias.x);
    
    m.str(""); m << conf.get_string("koray_dir")
		 << conf.get_string("wkoray") << "_shrink0_beta.x.mat";
    mat = load_matrix<t_net>(m.str());
    idx_copy(mat, ((smooth_shrink_module<t_net>*)((*net.modules)[2]))->beta.x);
  }
}

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
  cout << "* Generic object recognition training demo" << endl;
  if (argc != 2) {
    cout << "Usage: ./objrec_train <config file>" << endl;
    eblerror("config file not specified");
  }
#ifdef __LINUX__
  feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
  try {
    ipp_init(1); // limit IPP (if available) to 1 core
    init_drand(time(NULL)); // initialize random seed
    configuration conf(argv[1]); // configuration file

    //! load datasets
    labeled_datasource<t_net, float, int>
      train_ds(conf.get_cstring("root"),conf.get_cstring("train"),"train"),
      test_ds(conf.get_cstring("root"), conf.get_cstring("val"), "val");
    test_ds.set_test(); // test is the test set, used for reporting
    train_ds.set_weigh_samples(conf.exists_bool("sample_probabilities"));
    train_ds.set_weigh_normalization(conf.exists_true("per_class_norm"));
    if (conf.exists("min_sample_weight"))
      train_ds.set_min_proba(conf.get_double("min_sample_weight"));
    train_ds.set_shuffle_passes(conf.exists_bool("shuffle_passes"));
    if (conf.exists("balanced_training"))
      train_ds.set_balanced(conf.get_bool("balanced_training"));
    if (conf.exists("epoch_size"))
      train_ds.set_epoch_size(conf.get_int("epoch_size"));
    if (conf.exists("epoch_mode"))
      train_ds.set_epoch_mode(conf.get_uint("epoch_mode"));
    if (conf.exists("epoch_show_modulo"))
      train_ds.set_epoch_show(conf.get_uint("epoch_show_modulo"));

    //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
    idx<t_net> targets =
      create_target_matrix<t_net>(train_ds.get_nclasses(), 1.0);
    if (conf.exists_true("binary_target")) {
      if (train_ds.get_nclasses() != 2)
	eblerror("expecting 2 classes only when binary_target is on");
      targets = idx<t_net>(2, 1);
      int neg_id = train_ds.get_class_id("bg"); // negative class
      if (neg_id == 0) {
	targets.set(-1.0, 0, 0); // negative: -1.0
	targets.set( 1.0, 1, 0); // positive:  1.0
      } else {
	targets.set( 1.0, 0, 0); // positive:  1.0
	targets.set(-1.0, 1, 0); // negative: -1.0
      }
    }
    if (conf.exists("target_factor"))
      idx_dotc(targets, conf.get_double("target_factor"), targets);
    cout << "Targets:" << endl; targets.printElems();

    //! create the network weights, network and trainer
    idxdim dims(train_ds.sample_dims()); // get order and dimensions of sample
    parameter<t_net> theparam;// create trainable parameter
    module_1_1<t_net> *net =
      create_network<bbs(t_net)>(theparam, conf, targets.dim(0));
    supervised_euclidean_machine<t_net, int> thenet(*net, targets, dims);
    supervised_trainer<t_net, float, int> thetrainer(thenet, theparam);
    //! initialize the network weights
    forget_param_linear fgp(1, 0.5);
    if (conf.exists_bool("retrain")) {
      theparam.load_x(conf.get_cstring("retrain_weights"));
    } else {
      cout << "Initializing weights from random." << endl;
      thenet.forget(fgp);
    }
    if ((!conf.exists_bool("retrain")) && (conf.exists_bool("koray"))) {
      cout << "Initializing some weights from koray." << endl;
      koray_temp(conf, *((layers<t_net>*)net));
    }

    //! a classifier-meter measures classification errors
    classifier_meter trainmeter, testmeter;

    // learning parameters
    gd_param gdp(/* double leta*/ conf.get_double("eta"),
		 /* double ln */ 	0.0,
		 /* double l1 */ 	0.0,
		 /* double l2 */ 	0.0,
		 /* int dtime */ 	0,
		 /* double iner */0.0, 
		 /* double a_v */ 0.0,
		 /* double a_t */ 0.0,
		 /* double g_t*/ 	0.0);
    infer_param infp;
	
#ifdef __GUI__
    supervised_trainer_gui<t_net, float, int> stgui;
    bool display = conf.exists_bool("train_display"); // enable/disable display
    uint ninternals = conf.get_uint("ninternals"); // # examples' to display
    if (display) {
      //stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
      stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
    }
#endif

    timer titer, ttest;
    // first show classification results without training
    ttest.restart();
    if (!conf.exists_bool("no_training_test"))
      thetrainer.test(train_ds, trainmeter, infp);
    thetrainer.test(test_ds, testmeter, infp);
    cout << "testing_time="; ttest.pretty_elapsed(); cout << endl;

    // now do training iterations 
    cout << "Training network with " << train_ds.size();
    cout << " training samples and " << test_ds.size() <<" val samples for " 
	 << conf.get_uint("iterations") << " iterations:" << endl;
    ostringstream name, fname;
    intg dh = conf.exists("ndiaghessian") ? conf.get_int("ndiaghessian") : 100;
    for (uint i = 1; i <= conf.get_uint("iterations"); ++i) {
      titer.restart();
      // estimate second derivative on 100 iterations, using mu=0.02
      thetrainer.compute_diaghessian(train_ds, dh, 0.02);
      // train and test
      thetrainer.train(train_ds, trainmeter, gdp, 1, infp); // train
      ttest.restart();
      if (!conf.exists_bool("no_training_test"))
	thetrainer.test(train_ds, trainmeter, infp);	// test
      thetrainer.test(test_ds, testmeter, infp);	// test
      cout << "test_minutes=" << ttest.elapsed_minutes() << endl;
      cout << "testing_time="; ttest.pretty_elapsed(); cout << endl;
      
      // save samples picking statistics
      if (conf.exists_true("save_pickings")) {
	  name.str(""); name << "pickings_" << i;
	  train_ds.save_pickings(name.str().c_str());
      }
      // save weights and confusion matrix for test set
      name.str("");
      if (conf.exists("job_name"))
	name << conf.get_string("job_name");
      name << "_net" << setfill('0') << setw(3) << i;
      fname.str(""); fname << name.str() << ".mat";
      cout << "saving net to " << fname.str() << endl;
      theparam.save_x(fname.str().c_str()); // save trained network
      cout << "saved=" << fname.str() << endl;
      fname.str(""); fname << name.str() << "_confusion_test.mat";
      cout << "saving confusion to " << fname.str() << endl;
      save_matrix(testmeter.get_confusion(), fname.str().c_str());
#ifdef __GUI__ // display
      if (display) {
	//stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
	stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
      }
#endif
      cout << "iteration_minutes=" << titer.elapsed_minutes() << endl;
      cout << "iteration_time="; titer.pretty_elapsed(); cout << endl;
      cout << "timestamp=" << tstamp() << endl;
    }
    // free variables
    if (net) delete net;
#ifdef __GUI__
    quit_gui(); // close all windows
#endif
  } catch(string &err) {
    cerr << err << endl;
  }
  return 0;
}
