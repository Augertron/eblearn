#include <fenv.h>
#include "libeblearn.h"
#include "libeblearntools.h"
#include <iomanip>

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

// argv[1] is expected to contain the directory of the pascal dataset
#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
  cout << "* PASCAL object recognition demo" << endl;
  if (argc != 2) {
    cout << "Usage: ./pascal <config file>" << endl;
    eblerror("config file not specified");
  }
  typedef float t_net;
  feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
  init_drand(time(NULL)); // initialize random seed
  configuration conf(argv[1]); // configuration file

  //! load PASCAL datasets
  labeled_datasource<t_net, float, int>
    train_ds(conf.get_cstring("root"),conf.get_cstring("train"),"pascal train"),
    test_ds(conf.get_cstring("root"), conf.get_cstring("test"), "pascal test");
  train_ds.set_balanced();

  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  idx<t_net> targets =
    create_target_matrix<t_net>(train_ds.get_nclasses(), 1.0);

  //! create the network weights, network and trainer
  idxdim dims(train_ds.sample_dims()); // get order and dimensions of sample
  parameter<t_net> theparam(60000); // create trainable parameter
  lenet<t_net> net(theparam,
		  conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
		  conf.get_uint("net_c1h"), conf.get_uint("net_c1w"),
		  conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
		  conf.get_uint("net_c2h"), conf.get_uint("net_c2w"),
		  conf.get_uint("net_s2h"), conf.get_uint("net_s2w"),
		  conf.get_uint("net_full"), targets.dim(0),
		  conf.get_bool("absnorm"), conf.get_bool("color"));
  supervised_euclidean_machine<t_net, int> thenet((module_1_1<t_net>&)net, targets, dims);
  supervised_trainer<t_net, float, int> thetrainer(thenet, theparam);

  //! a classifier-meter measures classification errors
  classifier_meter trainmeter, testmeter;

  //! initialize the network weights
  forget_param_linear fgp(1, 0.5);
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
	
  // estimate second derivative on 100 iterations, using mu=0.02
  //  thetrainer.compute_diaghessian(train_ds, 100, 0.02);

#ifdef __GUI__
  supervised_trainer_gui<t_net, float, int> stgui;
  bool display = conf.get_bool("display"); // enable/disable display
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
  cout << "Training network on PASCAL with " << train_ds.size();
  cout << " training samples and " << test_ds.size() <<" test samples:" << endl;
  ostringstream name, fname;
  for (uint i = 1; i <= conf.get_uint("iterations"); ++i) {
    // train and test
    thetrainer.train(train_ds, trainmeter, gdp, 1);	         // train
    thetrainer.test(train_ds, trainmeter, infp);	         // test
    thetrainer.test(test_ds, testmeter, infp);	                 // test
    
    // save weights and confusion matrix for test set
    name.str(""); name << conf.get_string("name") << "_net" << setfill('0');
    name << setw(3) << i;
    fname.str(""); fname << name.str() << ".mat";
    theparam.save_x(fname.str().c_str()); // save trained network
    cout << "saved " << fname.str() << endl;
    fname.str(""); fname << name.str() << "_confusion_test.mat";
    save_matrix(testmeter.get_confusion(), fname.str().c_str());
#ifdef __GUI__ // display
    if (display) {
      //stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
      stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
    }
#endif
    
    // recompute 2nd derivatives
    thetrainer.compute_diaghessian(train_ds, 100, 0.02);
  }
  return 0;
}
