#include "libeblearn.h"
#include "DataTools.h"
#include <cstring>
using namespace std;
using namespace ebl;

// argv[1] is expected to contain the directory of the dataset
int main(int argc, char **argv) {
  
  // Create an Idx file for the datasets, if 'gen-idx' is supplied
  if (strcmp(argv[1], "gen-idx") == 0) {
    cout << "Converting images to idx object" << endl;
    imageDirToIdx("data/train", 46, ".*[.]ppm", 
		  NULL, "data/idx/", NULL, true,
		  "_train", false);
    imageDirToIdx("data/test", 46, ".*[.]ppm", NULL, "data/idx/", NULL, true,
		  "_test", false);
    return 0;
  }
  // Train if 'train' is supplied
  else if (strcmp(argv[1], "train") != 0) {
    cout << "syntax: test gen OR test train" << endl;
    return 0;
  }

  cout << "Training the ConvNet" << endl;
  init_drand(time(NULL)); // initialize random seed

  //! load datasets: trsize for training set and tesize for testing set
  Idx<float> trainingSet(1,1,1,1), testingSet(1,1,1,1);
  Idx<int> trainingLabels(1), testingLabels(1);

  load_matrix(trainingSet, "data/idx/dset_mono_train_images.mat");
  load_matrix(trainingLabels, "data/idx/dset_mono_train_labels.mat");
  load_matrix(testingSet, "data/idx/dset_mono_test_images.mat");
  load_matrix(testingLabels, "data/idx/dset_mono_test_labels.mat");

  // Get rid of the RGB data, only keep one channel per image
  //trainingSet = trainingSet.select(3, 0);
  //testingSet = testingSet.select(3, 0);

  LabeledDataSource<float,int> train_ds(trainingSet, trainingLabels,
					0.0, 0.01, "Posture Training Set");
  LabeledDataSource<float,int> test_ds(testingSet, testingLabels,
				       0.0, 0.01, "Posture Testing Set");

  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  Idx<double> targets = create_target_matrix(1+idx_max(train_ds.labels), 1.0);

  //! create the network weights, network and trainer
  IdxDim dims = train_ds.sample_dims(); // get order and dimensions of samples
  parameter theparam(120000); // create trainable parameter
  lenet5 l5(/* Stores the weights */ theparam, 
	    /* Input size */         46, 46, 
	    /* C0 kernel size */     7, 7, 
	    /* S1 mask size */       2, 2, 
	    /* C2 kernel size */     7, 7, 
	    /* S3 mask size */       2, 2, 
	    /* F5 Size */            80, 
	    /* Output size */        targets.dim(0));
  supervised_euclidean_machine thenet(l5, targets, dims);
  supervised_trainer<float,int> thetrainer(thenet, theparam);

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
  cout << "Computing second derivatives on the dataset: ";
  thetrainer.compute_diaghessian(train_ds, 100, 0.02);

  // do training iterations
  cout << "Training network on posture images with " << train_ds.size();
  cout << " training samples and " << test_ds.size() << " test samples" << endl;
  for (int i = 0; i < 100; ++i) {
    thetrainer.train(train_ds, trainmeter, gdp, 1);
    thetrainer.test(train_ds, trainmeter, infp);
    thetrainer.test(test_ds, testmeter, infp);
  }

  // Store the trained conv-net to a file...
  //theparam.save_x("data/trained_conv_net");
  // to get is back: theparam.load_x("data/trained_conv_net.mat");

  return 0;
}

 

