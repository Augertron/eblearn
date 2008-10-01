#include "NetTest.h"

using namespace std;
using namespace ebl;

extern string *gl_mnist_dir;
extern string *gl_mnist_errmsg;

void NetTest::setUp() {
}

void NetTest::tearDown() {
}

void NetTest::test_lenet5_mnist() {
  CPPUNIT_ASSERT_MESSAGE(*gl_mnist_errmsg, gl_mnist_dir != NULL);
  intg trsize, tesize;
  trsize = 2000; tesize = 1000; // small demo  
  // trsize = 60000; tesize = 10000; // full demo  	
  cout << endl;
  // train
  string train_datafile = *gl_mnist_dir;
  string train_labelfile = *gl_mnist_dir;
  train_datafile += "/train-images-idx3-ubyte";
  train_labelfile += "/train-labels-idx1-ubyte";
  Idx<ubyte> train_data(1, 1, 1), train_labels(1);
  CPPUNIT_ASSERT(load_matrix<ubyte>(train_data, train_datafile.c_str()) 
		 == true);
  CPPUNIT_ASSERT(load_matrix<ubyte>(train_labels, train_labelfile.c_str()) 
		 == true);
  // test
  string test_datafile = *gl_mnist_dir;
  string test_labelfile = *gl_mnist_dir;
  test_datafile += "/t10k-images-idx3-ubyte";
  test_labelfile += "/t10k-labels-idx1-ubyte";
  Idx<ubyte> test_data(1, 1, 1), test_labels(1);
  CPPUNIT_ASSERT(load_matrix<ubyte>(test_data, test_datafile.c_str()) == true);
  CPPUNIT_ASSERT(load_matrix<ubyte>(test_labels, test_labelfile.c_str()) 
		 == true);

  // TODO: implement DataSourceNarrow instead of manually narrowing here.
  train_data = train_data.narrow(0, trsize, 0); 
  train_labels = train_labels.narrow(0, trsize, 0);
  test_data = test_data.narrow(0, tesize, 5000 - (0.5 * tesize)); 
  test_labels = test_labels.narrow(0, tesize, 5000 - (0.5 * tesize));
  MnistDataSource<ubyte,ubyte> test_ds(&test_data, &test_labels, 
				       32, 32, 0.0, 0.01);
  MnistDataSource<ubyte,ubyte> train_ds(&train_data, &train_labels, 
					32, 32, 0.0, 0.01);
	
  // number of classes
  intg nclasses = 10;
  // the target values for mean-squared error training
  // are +target for the output corresponding to the class 
  // being shown and -target for the other outputs.
  double target = 1.0;

  // fill matrix with 1-of-n code
  Idx<ubyte> labels(nclasses);
  Idx<double> targets(nclasses, nclasses);
  idx_dotc(targets, 1.5, targets);
  idx_fill(targets, -target);
  for (int i = 0; i < nclasses; ++i) { 
    targets.set(target, i, i);
    labels.set((ubyte) i, i);
  }

  // create trainable parameter
  parameter theparam(60000);

  // create the network
  lenet5 l5(&theparam, 32, 32, 5, 5, 2, 2, 5, 5, 2, 2, 120, 10);
  edist_cost cost(&labels, 1, 1, &targets);
  max_classer c(&labels);
  idx3_supervised_module thenet(&l5, &cost, &c); 

  // create the trainer
  supervised_gradient thetrainer(&thenet, &theparam);

  // a classifier-meter measures classification errors
  classifier_meter trainmeter;
  classifier_meter testmeter;

  // initialize the network weights
  //printf("initializing network weights.\n");
  forget_param_linear fgp(1, 0.5);
  thenet.machine->forget(fgp);
  //theparam.load("/home/pierre/gblearn2/demos/param.mat");

  // estimate second derivative on 100 iterations, using mu=0.02
  // and set individual espilons
  //printf("computing diagonal hessian and learning rates\n");
  thetrainer.compute_diaghessian(&train_ds, 100, 0.02);

  // do training iterations 
  printf("training with %d training samples and %d test samples\n", 
	 train_ds.size(), test_ds.size());
	
  // gradient parameters
  gd_param gdp(/* double leta*/ 0.0001,
	       /* double ln */ 	0.0,
	       /* double l1 */ 	0.0,
	       /* double l2 */ 	0.0,
	       /* int dtime */ 	0,
	       /* double iner */0.0, 
	       /* double a_v */ 0.0,
	       /* double a_t */ 0.0,
	       /* double g_t*/ 	0.0);
	
  // this goes at about 25 examples per second on a PIIIM 800MHz
  for (int i = 0; i < 5; ++i) {
    thetrainer.train(&train_ds, &trainmeter, &gdp);
    cout << "training: " << flush;
    thetrainer.test(&train_ds, &trainmeter);
    trainmeter.display();
    cout << " testing: " << flush;
    thetrainer.test(&test_ds, &testmeter);
    testmeter.display();
  }
}
