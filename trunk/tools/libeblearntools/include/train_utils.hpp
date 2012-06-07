/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef TRAIN_UTILS_HPP_
#define TRAIN_UTILS_HPP_

namespace ebl {

  //////////////////////////////////////////////////////////////////////////////
  // testing and saving

  template <typename Tnet, typename Tdata, typename Tlabel>
  void test_and_save(uint iter, configuration &conf, string &conffname,
		     parameter<Tnet> &theparam,
		     supervised_trainer<Tnet,Tdata,Tlabel> &thetrainer,
		     labeled_datasource<Tnet,Tdata,Tlabel> &train_ds,
		     labeled_datasource<Tnet,Tdata,Tlabel> &test_ds,
		     classifier_meter &trainmeter,
		     classifier_meter &testmeter,
		     infer_param &infp, gd_param &gdp, string &shortname,
		     long iteration_seconds) {
    ostringstream wname, wfname;
    // save samples picking statistics
    if (conf.exists_true("save_pickings")) {
      string fname; fname << "pickings_" << iter;
      train_ds.save_pickings(fname.c_str());
    }
    // save weights and confusion matrix for test set
    wname.str("");
    if (conf.exists("job_name"))
      wname << conf.get_string("job_name");
    wname << "_net" << setfill('0') << setw(5) << iter;
    wfname.str(""); wfname << wname.str() << ".mat";
    if (conf.exists_false("save_weights"))
      cout << "Not saving weights (save_weights set to 0)." << endl;
    else {
      cout << "saving net to " << wfname.str() << endl;
      theparam.save_x(wfname.str().c_str()); // save trained network
      cout << "saved=" << wfname.str() << endl;
    }
    // test
    test(iter, conf, conffname, theparam, thetrainer, train_ds, test_ds,
	 trainmeter, testmeter, infp, gdp, shortname);
    // set retrain to next iteration with current saved weights
    ostringstream progress;
    progress << "retrain_iteration = " << iter + 1 << endl
	     << "retrain_weights = " << wfname.str() << endl;
    if (iteration_seconds > 0)
      progress << "meta_timeout = " << iteration_seconds * 1.2 << endl;
    // save progress
    job::write_progress(iter + 1, conf.get_uint("iterations"),
			progress.str().c_str());
    // save confusion
    if (conf.exists_true("save_confusion")) {
      string fname; fname << wname.str() << "_confusion_test.mat";
      cout << "saving confusion to " << fname << endl;
      save_matrix(testmeter.get_confusion(), fname.c_str());
    }
  }

  template <typename Tnet, typename Tdata, typename Tlabel>
  void test(uint iter, configuration &conf, string &conffname,
	    parameter<Tnet> &theparam,
	    supervised_trainer<Tnet,Tdata,Tlabel> &thetrainer,
	    labeled_datasource<Tnet,Tdata,Tlabel> &train_ds,
	    labeled_datasource<Tnet,Tdata,Tlabel> &test_ds,
	    classifier_meter &trainmeter,
	    classifier_meter &testmeter,
	    infer_param &infp, gd_param &gdp, string &shortname) {
    timer ttest;
    ostringstream wname, wfname;

    //   // some code to average several random solutions
    //     cout << "Testing...";
    //     if (original_tests > 1) cout << " (" << original_tests << " times)";
    //     cout << endl;
    //     ttest.restart();
    //     for (uint i = 0; i < original_tests; ++i) {
    //       if (test_only && original_tests > 1) {
    // 	// we obviously wanna test several random solutions
    // 	cout << "Initializing weights from random." << endl;
    // 	thenet.forget(fgp);
    //       }
    //       if (!no_training_test)
    // 	thetrainer.test(train_ds, trainmeter, infp);
    //       thetrainer.test(test_ds, testmeter, infp);
    //       cout << "testing_time="; ttest.pretty_elapsed(); cout << endl;
    //     }
    //     if (test_only && original_tests > 1) {
    //       // display averages over all tests
    //       testmeter.display_average(test_ds.name(), test_ds.lblstr, 
    // 				test_ds.is_test());
    //       trainmeter.display_average(train_ds.name(), train_ds.lblstr, 
    // 				 train_ds.is_test());
    //     }
    cout << "Testing on " << test_ds.size() << " samples..." << endl;
    uint maxtest = conf.exists("max_testing") ? conf.get_uint("max_testing") :0;
    ttest.start();
    if (!conf.exists_true("no_training_test"))
      thetrainer.test(train_ds, trainmeter, infp, maxtest);	// test
    if (!conf.exists_true("no_testing_test"))
      thetrainer.test(test_ds, testmeter, infp, maxtest);	// test
    cout << "testing_time="; ttest.pretty_elapsed(); cout << endl;
    // detection test
    if (conf.exists_true("detection_test")) {
      uint dt_nthreads = 1;
      if (conf.exists("detection_test_nthreads"))
	dt_nthreads = conf.get_uint("detection_test_nthreads");
      timer dtest;
      dtest.start();
      // copy config file and augment it and detect it
      string cmd, params;
      if (conf.exists("detection_params")) {
	params = conf.get_string("detection_params");
	params = string_replaceall(params, "\\n", "\n");
      }
      cmd << "cp " << conffname << " tmp.conf && echo \"silent=1\n"
	  << "nthreads=" << dt_nthreads << "\nevaluate=1\nweights_file=" 
	  << wfname.str() << "\n" << params
	  << "\" >> tmp.conf && detect tmp.conf";
      if (std::system(cmd.c_str()))
	cerr << "warning: failed to execute: " << cmd << endl;
      cout << "detection_test_time="; dtest.pretty_elapsed(); cout << endl;
    }
#ifdef __GUI__ // display
    static supervised_trainer_gui<Tnet,Tdata,Tlabel> stgui(shortname.c_str());
    static supervised_trainer_gui<Tnet,Tdata,Tlabel> stgui2(shortname.c_str());
    bool display = conf.exists_true("show_train"); // enable/disable display
    uint ninternals = conf.exists("show_train_ninternals") ? 
      conf.get_uint("show_train_ninternals") : 1; // # examples' to display
    bool show_train_errors = conf.exists_true("show_train_errors");
    bool show_train_correct = conf.exists_true("show_train_correct");
    bool show_val_errors = conf.exists_true("show_val_errors");
    bool show_val_correct = conf.exists_true("show_val_correct");
    bool show_only_images = conf.exists_true("show_only_images");
    bool show_raw_outputs = conf.exists_true("show_raw_outputs");
    bool show_all_jitter = conf.exists_true("show_all_jitter");
    uint hsample = conf.exists("show_hsample") ?conf.get_uint("show_hsample"):5;
    uint wsample = conf.exists("show_wsample") ?conf.get_uint("show_wsample"):5;
    if (display) {
      cout << "Displaying training..." << endl;
      if (show_train_errors) {
	stgui2.display_correctness(true, true, thetrainer, train_ds, infp,
				   hsample, wsample, show_raw_outputs,
				   show_all_jitter, show_only_images);
	stgui2.display_correctness(true, false, thetrainer, train_ds, infp,
				   hsample, wsample, show_raw_outputs,
				   show_all_jitter, show_only_images);
      }
      if (show_train_correct) {
	stgui2.display_correctness(false, true, thetrainer, train_ds, infp,
				   hsample, wsample, show_raw_outputs,
				   show_all_jitter, show_only_images);
	stgui2.display_correctness(false, false, thetrainer, train_ds, infp,
				   hsample, wsample, show_raw_outputs,
				   show_all_jitter, show_only_images);
      }
      if (show_val_errors) {
	stgui.display_correctness(true, true, thetrainer, test_ds, infp,
				  hsample, wsample, show_raw_outputs,
				  show_all_jitter, show_only_images);
	stgui.display_correctness(true, false, thetrainer, test_ds, infp,
				  hsample, wsample, show_raw_outputs,
				  show_all_jitter, show_only_images);
      }
      if (show_val_correct) {
	stgui.display_correctness(false, true, thetrainer, test_ds, infp,
				  hsample, wsample, show_raw_outputs,
				  show_all_jitter, show_only_images);
	stgui.display_correctness(false, false, thetrainer, test_ds, infp,
				  hsample, wsample, show_raw_outputs,
				  show_all_jitter, show_only_images);
      }
      stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
    }
#endif
  }

  template <typename Tnet, typename Tdata, typename Tlabel>
  labeled_datasource<Tnet,Tdata,Tlabel>* 
  create_validation_set(configuration &conf, uint &noutputs, string &valdata) {
    bool classification = conf.exists_true("classification");
    valdata = conf.get_string("val");
    string vallabels, valclasses, valjitters, valscales;
    vallabels = conf.try_get_string("val_labels");
    valclasses = conf.try_get_string("val_classes");
    valjitters = conf.try_get_string("val_jitters");
    valscales = conf.try_get_string("val_scales");
    uint maxval = 0;
    if (conf.exists("val_size")) maxval = conf.get_uint("val_size");
    labeled_datasource<Tnet,Tdata,Tlabel> *val_ds = NULL;
    if (classification) { // classification task
      class_datasource<Tnet,Tdata,Tlabel> *ds =
	new class_datasource<Tnet,Tdata,Tlabel>;
      ds->init(valdata.c_str(), vallabels.c_str(), valjitters.c_str(),
	       valscales.c_str(), valclasses.c_str(), "val", maxval);
      if (conf.exists("limit_classes"))
	ds->limit_classes(conf.get_int("limit_classes"), 0, 
			  conf.exists_true("limit_classes_random"));
      noutputs = ds->get_nclasses();
      val_ds = ds;
    } else { // regression task
      val_ds = new labeled_datasource<Tnet,Tdata,Tlabel>;
      val_ds->init(valdata.c_str(), vallabels.c_str(), valjitters.c_str(),
		   valscales.c_str(), "val", maxval);
      idxdim d = val_ds->label_dims();
      noutputs = d.nelements();
    }
    val_ds->set_test(); // test is the test set, used for reporting
    val_ds->pretty();
    if (conf.exists("data_bias"))
      val_ds->set_data_bias((Tnet)conf.get_double("data_bias"));
    if (conf.exists("data_coeff"))
      val_ds->set_data_coeff((Tnet)conf.get_double("data_coeff"));
    if (conf.exists("label_bias"))
      val_ds->set_label_bias((Tnet)conf.get_double("label_bias"));
    if (conf.exists("label_coeff"))
      val_ds->set_label_coeff((Tnet)conf.get_double("label_coeff"));
    if (conf.exists("epoch_show_modulo"))
      val_ds->set_epoch_show(conf.get_uint("epoch_show_modulo"));
    val_ds->keep_outputs(conf.exists_true("keep_outputs"));
    return val_ds;    
  }

  template <typename Tnet, typename Tdata, typename Tlabel>
  labeled_datasource<Tnet,Tdata,Tlabel>* 
  create_training_set(configuration &conf, uint &noutputs, string &traindata) {
    bool classification = conf.exists_true("classification");
    traindata = conf.get_string("train");
    string trainlabels, trainclasses, trainjitters, trainscales;
    trainlabels = conf.try_get_string("train_labels");
    trainclasses = conf.try_get_string("train_classes");
    trainjitters = conf.try_get_string("train_jitters");
    trainscales = conf.try_get_string("train_scales");
    uint maxtrain = 0;
    if (conf.exists("train_size")) maxtrain = conf.get_uint("train_size");
    labeled_datasource<Tnet,Tdata,Tlabel> *train_ds = NULL;
    if (classification) { // classification task
      class_datasource<Tnet,Tdata,Tlabel> *ds =
	new class_datasource<Tnet,Tdata,Tlabel>;
      ds->init(traindata.c_str(), trainlabels.c_str(),
	       trainjitters.c_str(), trainscales.c_str(), 
	       trainclasses.c_str(), "train", maxtrain);
      if (conf.exists("balanced_training"))
	ds->set_balanced(conf.get_bool("balanced_training"));
      if (conf.exists("random_class_order"))
	ds->set_random_class_order(conf.get_bool("random_class_order"));
      if (conf.exists("limit_classes"))
	ds->limit_classes(conf.get_int("limit_classes"), 0, 
			  conf.exists_true("limit_classes_random"));
      noutputs = ds->get_nclasses();
      train_ds = ds;
    } else { // regression task
      train_ds = new labeled_datasource<Tnet,Tdata,Tlabel>;
      train_ds->init(traindata.c_str(), trainlabels.c_str(),
		     trainjitters.c_str(), trainscales.c_str(), 
		     "train", maxtrain);
      idxdim d = train_ds->label_dims();
      noutputs = d.nelements();
    }
    train_ds->ignore_correct(conf.exists_true("ignore_correct"));
    train_ds->set_weigh_samples(conf.exists_true("sample_probabilities"),
				conf.exists_true("hardest_focus"),
				conf.exists_true("per_class_norm"),
				conf.exists("min_sample_weight") ?
				conf.get_double("min_sample_weight") : 0.0);
    train_ds->set_shuffle_passes(conf.exists_bool("shuffle_passes"));
    if (conf.exists("epoch_size"))
      train_ds->set_epoch_size(conf.get_int("epoch_size"));
    if (conf.exists("epoch_mode"))
      train_ds->set_epoch_mode(conf.get_uint("epoch_mode"));
    if (conf.exists("epoch_show_modulo"))
      train_ds->set_epoch_show(conf.get_uint("epoch_show_modulo"));
    train_ds->pretty();
    if (conf.exists("data_bias"))
      train_ds->set_data_bias((Tnet)conf.get_double("data_bias"));
    if (conf.exists("data_coeff"))
      train_ds->set_data_coeff((Tnet)conf.get_double("data_coeff"));
    if (conf.exists("label_bias"))
      train_ds->set_label_bias((Tnet)conf.get_double("label_bias"));
    if (conf.exists("label_coeff"))
      train_ds->set_label_coeff((Tnet)conf.get_double("label_coeff"));
    train_ds->keep_outputs(conf.exists_true("keep_outputs"));
    return train_ds;
  }
  
} // end namespace ebl

#endif /* TRAIN_UTILS_HPP_ */

