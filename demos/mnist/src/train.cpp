#include <fenv.h>
#include "libeblearn.h"
#include "libeblearntools.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

// argv[1] is expected to contain the directory of the mnist dataset
#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
  cout << "* MNIST demo: learning handwritten digits using the eblearn";
  cout << " C++ library *" << endl;
  if (argc != 2) {
    cout << "Usage: ./mnist <config file>" << endl;
    eblerror("config file not specified");
  }
  feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
  init_drand(0); // initialize random seed
  //  init_drand(time(NULL)); // initialize random seed
  configuration conf(argv[1]); // configuration file
  bool display = conf.get_bool("display"); // enable/disable display
  uint ninternals = conf.get_uint("ninternals"); // # examples' to display

  //! load MNIST datasets: trize for training set and tesize for testing set
  intg trsize = conf.get_uint("training_size"); // maximum training set size: 60000
  intg tesize = conf.get_uint("testing_size"); // maximum testing set size:  10000  
  mnist_datasource<ubyte,ubyte> train_ds, test_ds;
  load_mnist_dataset(conf.get_cstring("root"), train_ds,test_ds,trsize,tesize);

  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  idx<double> targets = create_target_matrix(1+idx_max(train_ds.labels), 1.0);

  //! create the network weights, network and trainer
  idxdim dims(train_ds.sample_dims()); // get order and dimensions of sample
  parameter theparam(60000); // create trainable parameter
  lenet5 l5(theparam, 32, 32, 5, 5, 2, 2, 5, 5, 2, 2, 120, targets.dim(0), conf.get_bool("absnorm"));
  supervised_euclidean_machine thenet(l5, targets, dims);
  supervised_trainer<ubyte,ubyte> thetrainer(thenet, theparam);

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
  thetrainer.compute_diaghessian(train_ds, 100, 0.02);

#ifdef __GUI__
  supervised_trainer_gui<ubyte, ubyte> stgui;
  if (display) {
    //stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
    stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
  }
#endif

  // first show classification results without training
  thetrainer.test(train_ds, trainmeter, infp);
  thetrainer.test(test_ds, testmeter, infp);

  // now do training iterations 
  cout << "Training network on MNIST with " << train_ds.size();
  cout << " training samples and " << test_ds.size() <<" test samples:" << endl;
  for (uint i = 0; i < conf.get_uint("iterations"); ++i) {
    thetrainer.train(train_ds, trainmeter, gdp, 1);	         // train
    thetrainer.test(train_ds, trainmeter, infp);	         // test
    thetrainer.test(test_ds, testmeter, infp);	                 // test
    theparam.save_x("mnist_trained_network.mat"); // save trained network
#ifdef __GUI__
    if (display) {
      //stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
      stgui.display_internals(thetrainer, test_ds, infp, gdp, ninternals);
    }
#endif
    thetrainer.compute_diaghessian(train_ds, 100, 0.02); // recompute 2nd der
  }
  return 0;
}
