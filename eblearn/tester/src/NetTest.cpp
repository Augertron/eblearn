#include "NetTest.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif

using namespace std;
using namespace ebl;

extern string *gl_mnist_dir;
extern string *gl_mnist_errmsg;

void NetTest::setUp() {
}

void NetTest::tearDown() {
}

void NetTest::test_lenet5_mnist_ebl() {
  cout << endl;
  // for testing purposes, we always initialize the randomization with 0 so 
  // that we know the exact results. 
  // in the real world, init_drand should be called with time(NULL) as argument.
  init_drand(0); // fixed randomization
  CPPUNIT_ASSERT_MESSAGE(*gl_mnist_errmsg, gl_mnist_dir != NULL);
  intg trsize = 2000; // maximum training set size: 60000
  intg tesize = 1000; // maximum testing set size:  10000
  
  // load MNIST dataset
  MnistDataSource<ubyte,ubyte> train_ds, test_ds;
  load_mnist_dataset(gl_mnist_dir->c_str(), train_ds, test_ds, trsize, tesize);

  // create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  Idx<double> targets = create_target_matrix(1+idx_max(train_ds.labels), 1.0);

  // create the network weights, network and trainer
  IdxDim dims(train_ds.data.spec); // get order and dimenions from data
  parameter theparam(60000); // create trainable parameter
  lenet5 l5(theparam, 32, 32, 5, 5, 2, 2, 5, 5, 2, 2, 120, targets.dim(0));
  supervised_euclidean_machine thenet(l5, targets, dims);
  supervised_trainer<ubyte,ubyte> thetrainer(thenet, theparam);

   // a classifier-meter measures classification errors
   classifier_meter trainmeter;
   classifier_meter testmeter;

  // initialize the network weights
  forget_param_linear fgp(1, 0.5);
  thenet.forget(fgp);

  // estimate second derivative on 100 iterations, using mu=0.02
  // and set individual espilons
  //printf("computing diagonal hessian and learning rates\n");
  thetrainer.compute_diaghessian(train_ds, 100, 0.02);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.985363, 
			       idx_min(thetrainer.param.epsilons), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(49.851524, 
			       idx_max(thetrainer.param.epsilons), 0.000001);

  // do training iterations 
  printf("training with %d training samples and %d test samples\n", 
	 train_ds.size(), test_ds.size());
#ifdef __GUI__  
  gui_new_window("Demo MNIST");
  ostringstream o;
  o << "training with " << train_ds.size() << " training samples and ";
  o << test_ds.size() << " test samples\n" << endl;
  *window << new std::string(o.str());
#endif
	
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
  infer_param infp;
	
  // this goes at about 25 examples per second on a PIIIM 800MHz
  for (int i = 0; i < 5; ++i) {
    thetrainer.train(train_ds, trainmeter, gdp, 1);
    cout << "training: " << flush;
    thetrainer.test(train_ds, trainmeter, infp);
    trainmeter.display();
    cout << " testing: " << flush;
    thetrainer.test(test_ds, testmeter, infp);
    testmeter.display();
  }
  CPPUNIT_ASSERT_DOUBLES_EQUAL(97.15,
			       ((trainmeter.total_correct * 100) 
				/ (double) trainmeter.size), 0.01);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(95.80,
			       ((testmeter.total_correct * 100) 
				/ (double) testmeter.size), 0.01);
}

void NetTest::test_lenet5_mnist() {
  // for testing purposes, we always initialize the randomization with 0 so 
  // that we know the exact results. 
  // in the real world, init_drand should be called with time(NULL) as argument.
  init_drand(0); // fixed randomization
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
  MnistDataSource<ubyte,ubyte> test_ds(test_data, test_labels, 
				       32, 32, 0.0, 0.01);
  MnistDataSource<ubyte,ubyte> train_ds(train_data, train_labels, 
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
  //  idx_dotc(targets, 1.5, targets);
  idx_fill(targets, -target);
  for (int i = 0; i < nclasses; ++i) { 
    targets.set(target, i, i);
    labels.set((ubyte) i, i);
  }

  // create trainable parameter
  parameter theparam(60000);

  // create the network
  lenet5 l5(theparam, 32, 32, 5, 5, 2, 2, 5, 5, 2, 2, 120, 10);
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
  CPPUNIT_ASSERT_DOUBLES_EQUAL(0.985363, 
			       idx_min(thetrainer.param->epsilons), 0.000001);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(49.851524, 
			       idx_max(thetrainer.param->epsilons), 0.000001);

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
  CPPUNIT_ASSERT_DOUBLES_EQUAL(97.15,
			       ((trainmeter.total_correct * 100) 
				/ (double) trainmeter.size), 0.01);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(95.80,
			       ((testmeter.total_correct * 100) 
				/ (double) testmeter.size), 0.01);
}
