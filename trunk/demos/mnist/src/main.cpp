#include "libeblearn.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

// argv[1] is expected to contain the directory of the mnist dataset
#ifdef __GUI__
MAIN_QTHREAD() { // this is the macro replacing main to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
  cout << "* MNIST demo: learning handwritten digits using the eblearn";
  cout << " C++ library *" << endl;
  if (argc != 2) {
    cout << "Usage: ./mnist <my mnist directory>" << endl;
    ylerror("MNIST path not specified");
  }
  init_drand(time(NULL)); // initialize random seed

  intg trsize = 60000; // maximum training set size: 60000
  intg tesize = 10000; // maximum testing set size:  10000
  
  //! load MNIST datasets: trize for training set and tesize for testing set
  mnist_datasource<ubyte,ubyte> train_ds, test_ds;
  load_mnist_dataset(argv[1], train_ds, test_ds, trsize, tesize);

  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  idx<double> targets = create_target_matrix(1+idx_max(train_ds.labels), 1.0);

  //! create the network weights, network and trainer
  idxdim dims(train_ds.sample_dims()); // get order and dimensions of sample
  parameter theparam(60000); // create trainable parameter
  lenet5 l5(theparam, 32, 32, 5, 5, 2, 2, 5, 5, 2, 2, 120, targets.dim(0));
  supervised_euclidean_machine thenet(l5, targets, dims);
  supervised_trainer<ubyte,ubyte> thetrainer(thenet, theparam);

  //! a classifier-meter measures classification errors
  classifier_meter trainmeter, testmeter;

  //! initialize the network weights
  forget_param_linear fgp(1, 0.5);
  thenet.forget(fgp);

  // learning parameters
  gd_param gdp(/* double leta*/ 0.0001,
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
  cout << "Computing second derivatives on MNIST dataset: ";
  thetrainer.compute_diaghessian(train_ds, 100, 0.02);

  // first show classification results without training
  thetrainer.test(train_ds, trainmeter, infp);
  thetrainer.test(test_ds, testmeter, infp);

#ifdef __GUI__
  supervised_trainer_gui stgui; // the gui to display supervised_trainer
  stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
  stgui.display_internals(thetrainer, test_ds, infp, 2);
#endif

  // now do training iterations 
  cout << "Training network on MNIST with " << train_ds.size();
  cout << " training samples and " << test_ds.size() << " test samples:" << endl;
  for (int i = 0; i < 100; ++i) {
    thetrainer.train(train_ds, trainmeter, gdp, 1);	         // train
    thetrainer.test(train_ds, trainmeter, infp);	         // test
    thetrainer.test(test_ds, testmeter, infp);	                 // test
#ifdef __GUI__
    stgui.display_datasource(thetrainer, test_ds, infp, 10, 10); // display
    stgui.display_internals(thetrainer, test_ds, infp, 2);       // display
#endif
    thetrainer.compute_diaghessian(train_ds, 100, 0.02); // recompute 2nd der
  }
  return 0;
}
