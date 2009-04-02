

#include "libeblearn.h"
#include "DataTools.h"
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;
using namespace ebl;

void generateIdxDataSet(string pathToData);
string getPathToData();

int main(int argc, char **argv) {
  
  // Useful paths to datasets
  string pathToData = getPathToData() + "/posture";
  string pathToIdxTrain = pathToData+"/idx/dset_mono_train_images.mat";
  string pathToIdxTrLabels = pathToData+"/idx/dset_mono_train_labels.mat";
  string pathToIdxTest = pathToData+"/idx/dset_mono_test_images.mat";
  string pathToIdxTeLabels = pathToData+"/idx/dset_mono_test_labels.mat";
  
  //! create an Idx file for the datasets, if 'gen-idx' is supplied
  if (argc > 1)
    if (strcmp(argv[1], "gen-idx") == 0) generateIdxDataSet(pathToData);

  cout << endl << "Training the ConvNet" << endl;
  init_drand(time(NULL)); // initialize random seed

  //! load dataset from Idx files
  Idx<float> trainingSet(1,1,1,1), testingSet(1,1,1,1);
  Idx<int> trainingLabels(1), testingLabels(1);
  load_matrix(trainingSet, pathToIdxTrain.c_str());
  load_matrix(trainingLabels, pathToIdxTrLabels.c_str());
  load_matrix(testingSet, pathToIdxTest.c_str());
  load_matrix(testingLabels, pathToIdxTeLabels.c_str());

  //! create two labeled data sources, for training and testing
  LabeledDataSource<float,int> train_ds(trainingSet, // Data source
					trainingLabels, // Labels
					0.0, // Bias to be added to images
					0.01, // Coef to scale images
					NULL, "Posture Training Set");
  LabeledDataSource<float,int> test_ds(testingSet, testingLabels,
				       0.0, 0.01, "Posture Testing Set");

  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  Idx<double> targets = create_target_matrix(1+idx_max(train_ds.labels), 1.0);

  //! create the network weights, network and trainer
  IdxDim dims = train_ds.sample_dims(); // get order and dimensions of samples
  parameter theparam(120000); // create trainable parameter

  // create the architecture of the conv net. lenet5 is a generic cscscf network
  lenet5 l5(/* Stores the weights */ theparam, 
	    /* Input size */         46, 46, 
	    /* C0 kernel size */     7, 7, 
	    /* S1 mask size */       2, 2, 
	    /* C2 kernel size */     7, 7, 
	    /* S3 mask size */       2, 2, 
	    /* F5 Size */            80, 
	    /* Output size */        targets.dim(0));

  //! combine the conv net with targets -> gives a supervised system
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



// generate Idx files from a given set of images.
void generateIdxDataSet(string pathToData) {
  string pathTrainingSet = pathToData+"/train";
  string pathTestingSet = pathToData+"/test";
  string pathToIdx = pathToData+"/idx";
  
  cout << "Converting images to idx object" << endl;
  imageDirToIdx(pathTrainingSet.c_str(), 46, ".*[.]ppm", 
		NULL, pathToIdx.c_str(), NULL, true,
		"_train", false);
  imageDirToIdx(pathTestingSet.c_str(), 46, ".*[.]ppm", 
		NULL, pathToIdx.c_str(), NULL, true,
		"_test", false);
}

// return the path of the database, based on run.init
string getPathToData() {
  string s;
  ifstream in("../run.init");
  if (!in) {
    cout << "Warning: failed to open ../run.init, please run configure.sh";
    cout << endl;
  }
  else {
    while (!in.eof()) {
      in >> s;
      if (s == "-data") {
	in >> s;
      }
    }
  }
  in.close();
  return s;
}
