#include "NetTest.h"

#ifdef __GUI__
#include "libidxgui.h"
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl;

extern string *gl_mnist_dir;
extern string *gl_mnist_errmsg;

void NetTest::setUp() {
}

void NetTest::tearDown() {
}

void NetTest::test_lenet5_mnist() {
#ifdef __GUI__  
  //  gui.new_window("Demo MNIST");
  //  RenderThread &cout = gui;
#endif
  cout << endl;
  // for testing purposes, we always initialize the randomization with 0 so 
  // that we know the exact results. 
  // in the real world, init_drand should be called with time(NULL) as argument.
  init_drand(0); // fixed randomization
  CPPUNIT_ASSERT_MESSAGE(*gl_mnist_errmsg, gl_mnist_dir != NULL);
  intg trsize = 2000; // maximum training set size: 60000
  intg tesize = 1000; // maximum testing set size:  10000
  
  // load MNIST dataset
  mnist_datasource<ubyte,ubyte> train_ds, test_ds;
  load_mnist_dataset(gl_mnist_dir->c_str(), train_ds, test_ds, trsize, tesize);

  // create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  idx<double> targets = create_target_matrix(1+idx_max(train_ds.labels), 1.0);

  // create the network weights, network and trainer
  idxdim dims(train_ds.sample_dims()); // get order and dimensions of sample
  parameter theparam(60000); // create trainable parameter
  lenet5 l5(theparam, 32, 32, 5, 5, 2, 2, 5, 5, 2, 2, 120, targets.dim(0));
  supervised_euclidean_machine thenet(l5, targets, dims);
  supervised_trainer<ubyte,ubyte> thetrainer(thenet, theparam, cout);
  supervised_trainer_gui stgui;

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
  cout << "training with " << train_ds.size() << " training samples and ";
  cout << test_ds.size() << " test samples" << endl;
	
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
	
  thetrainer.test(train_ds, trainmeter, infp);
  thetrainer.test(test_ds, testmeter, infp);
  stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
  stgui.display_internals(thetrainer, test_ds, infp, 2);
  // this goes at about 25 examples per second on a PIIIM 800MHz
  for (int i = 0; i < 5; ++i) {
    thetrainer.train(train_ds, trainmeter, gdp, 1);
    thetrainer.test(train_ds, trainmeter, infp);
    thetrainer.test(test_ds, testmeter, infp);
    stgui.display_datasource(thetrainer, test_ds, infp, 10, 10);
    stgui.display_internals(thetrainer, test_ds, infp, 2);
  }
  CPPUNIT_ASSERT_DOUBLES_EQUAL(97.15,
			       ((trainmeter.total_correct * 100) 
				/ (double) trainmeter.size), 0.01);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(95.80,
			       ((testmeter.total_correct * 100) 
				/ (double) testmeter.size), 0.01);
}


void NetTest::test_full_table() {
  idx<intg> m = full_table(2, 3);
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(0, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(0, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(1, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(1, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(2, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(2, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(3, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(3, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 0, m.get(4, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 2, m.get(4, 1));
  CPPUNIT_ASSERT_EQUAL((intg) 1, m.get(5, 0));
  CPPUNIT_ASSERT_EQUAL((intg) 2, m.get(5, 1));
}
