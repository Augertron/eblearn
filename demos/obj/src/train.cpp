#include <iomanip>
#include "objrec.h"

typedef double t_net; // precision at which network is trained (ideally double)

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
#ifndef __MAC__
  feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
  ipp_init(1); // limit IPP (if available) to 1 core
  init_drand(time(NULL)); // initialize random seed
  configuration conf(argv[1]); // configuration file

  //! load datasets
  labeled_datasource<t_net, float, int>
    train_ds(conf.get_cstring("root"),conf.get_cstring("train"),"train"),
    test_ds(conf.get_cstring("root"), conf.get_cstring("val"), "val");
  test_ds.set_test(); // test is the test set, used for reporting
  train_ds.set_weigh_samples(conf.exists_bool("wsamples"));
  train_ds.set_weigh_normalization(conf.exists_bool("wnorm"));
  train_ds.set_shuffle_passes(conf.exists_bool("shuffle_passes"));

  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  idx<t_net> targets =
    create_target_matrix<t_net>(train_ds.get_nclasses(), 1.0);

  //! create the network weights, network and trainer
  idxdim dims(train_ds.sample_dims()); // get order and dimensions of sample
  parameter<t_net> theparam(60000); // create trainable parameter
  module_1_1<t_net> *net = init_network(theparam, conf, targets.dim(0));
  cout << "Initializing weights from ";
  if (conf.exists_bool("retrain")) {
    cout << conf.get_cstring("retrain_weights") << endl;
    theparam.load_x<t_net>(conf.get_cstring("retrain_weights"));
  } else cout << "random" << endl;
  supervised_euclidean_machine<t_net, int> thenet(*net, targets, dims);
  supervised_trainer<t_net, float, int> thetrainer(thenet, theparam);

  //! a classifier-meter measures classification errors
  classifier_meter trainmeter, testmeter;

  //! initialize the network weights
  forget_param_linear fgp(1, 0.5);
  if (!conf.exists_bool("retrain"))
    thenet.forget(fgp);

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

  // first show classification results without training
  thetrainer.test(train_ds, trainmeter, infp);
  thetrainer.test(test_ds, testmeter, infp);

  // now do training iterations 
  cout << "Training network with " << train_ds.size();
  cout << " training samples and " << test_ds.size() <<" val samples for " 
       << conf.get_uint("iterations") << " iterations:" << endl;
  ostringstream name, fname;

  // estimate second derivative on 100 iterations, using mu=0.02
  thetrainer.compute_diaghessian(train_ds, 100, 0.02);

  for (uint i = 1; i <= conf.get_uint("iterations"); ++i) {
    // train and test
    thetrainer.train(train_ds, trainmeter, gdp, 1);	// train
    thetrainer.test(train_ds, trainmeter, infp);	// test
    thetrainer.test(test_ds, testmeter, infp);	// test
    
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
    
    // recompute 2nd derivatives on 100 iterations with mu=0.02
    thetrainer.compute_diaghessian(train_ds, 100, 0.02);
  }
  // free variables
  if (net) delete net;
  return 0;
}
