
/* Posture estimation ConvNet.
 * In this example we build a ConvNet based on a generic CSCSCF architecture.
 * For that we create a class named ConvNetPosture
 * Everything can be redefined for this ConvNet:
 *  - the nb of feature maps we want per layer,
 *  - the connections between them,
 *  - the size of the kernels and subsample masks...
 * 
 * In the main function, we load a database of images (pnm images) and first 
 * package them in an Idx file (format used by EBLearn to handle matrices).
 * These images contiain 6 different human postures: bending, standing...
 * and are separated in two sets: one for training, the other one for testing.
 *
 * Then we load these Idx files and train our ConvNet to separate the output
 * space in these 6 classes.
 *
 * By using the raw dataset, the ConvNet converges to 100% good results on the 
 * training dataset, and 86% on the testing set... in 6 iterations !!
 *
 * The script expand-dataset.py (in $EBLEARN/eblearn/data/posture/) can be run
 * (requires Python) to create other images from the raw dataset 
 * (rotated/rescaled). Doing this helps reaching better results on the testing 
 * set.
 *
 * Feel free to change/tweak the paramaters of the ConvNet, in the class
 * ConvNetPosture. 
 *
 * Syntax:
 *   posture             -> creates the Idx files and train the ConvNet
 *   posture train-only  -> expect the Idx to be present, and train the ConvNet
 *
 * Clement Farabet (cfarabet@nyu.edu) || April 2009.
 */

#include "libeblearn.h"
#include "DataTools.h"
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;
using namespace ebl;

void generateIdxDataSet(string pathToData);
string getPathToData();

/* Here we define the ConvNet to be used for posture estimation:
 * This ConvNet inherits from the CSCSCF architecture, a classical stackup of
 * 3 convolutional layers. In this class, we simply redefine:
 *  - the nb of feature maps we want,
 *  - the connections between them,
 *  - the size of the kernels and subsample masks.
 */ 
class ConvNetPosture : public nn_machine_cscscf {
public:
  Idx<intg> table0;	
  Idx<intg> table1;	
  Idx<intg> table2;	
  
  ConvNetPosture(parameter &trainableParam, 
		 intg image_height, 
		 intg image_width, 
		 intg output_size) {
    // Define the number of feature maps per layer (C0, C1, C2)
    intg featureMaps0 = 6;
    intg featureMaps1 = 12;
    intg featureMaps2 = 40;

    // Define tables of connections between layers.
    // These two are fully connected layer, i.e. each feature map in a layer
    // is connected to every feature map in the previous layer
    table0 = full_table(1, featureMaps0); // from input to C0
    table2 = full_table(featureMaps1, featureMaps2); // from S1 to C2

    // ... whereas the connections there are sparse (S0 to C1):
    table1 = Idx<intg>(44, 2); // from S0 to C1
    intg tbl[44][2] =
      {{0, 0},  {1, 0},  {2, 0}, // 0,1,2 in S0 connected to 0 in C1
       {1, 1},  {2, 1},  {3, 1}, // and so on...
       {2, 2},  {3, 2},  {4, 2},
       {3, 3},  {4, 3},  {5, 3},
       {4, 4},  {5, 4},  {0, 4},
       {5, 5},  {0, 5},  {1, 5},

       {0, 6},  {1, 6},  {2, 6},  {3, 6},
       {1, 7},  {2, 7},  {3, 7},  {4, 7},
       {2, 8},  {3, 8},  {4, 8},  {5, 8},
       {3, 9},  {4, 9},  {5, 9},  {0, 9},
       {4, 10}, {5, 10}, {0, 10}, {1, 10},

       {0, 15}, {1, 15}, {2, 15}, {3, 15}, {4, 15}, {5, 15}};
    memcpy(table1.idx_ptr(), tbl, table1.nelements() * sizeof (intg));
    
    // Finally we initialize the architecture of the ConvNet.
    // Make sure that the output dim is 1x1 for the images in your dataset:
    // ((((image_height - ki0 + 1) / si0) - ki1 + 1) / si1) - ki2 + 1 = 1
    // ((((image_width  - kj0 + 1) / sj0) - kj1 + 1) / sj1) - kj2 + 1 = 1
    //
    this->init(trainableParam, // Param that holds the weights of the ConvNet
	       image_height, // Dim of input image
	       image_width, 
	       7, 7, // Dim of kernel in C0
	       table0, // Table of connections btwn input layer and C0 
	       2, 2, // Subsample mask size in S0
	       7, 7, // Dim of kernel in C1
	       table1, // Table of connections btwn S0 and C1 
	       2, 2, // Subsample mask size ni S1
	       7, 7, // Dim of kernel in C1
	       table2, // Table of connections btwn S1 and C2
	       output_size // Nb of classes
	       );
  }
  // Destructor not used
  virtual ~ConvNetPosture() {}
};

int main(int argc, char **argv) {
  
  // Useful paths to datasets
  string pathToData = getPathToData() + "/posture";
  string pathToIdxTrain = pathToData+"/idx/dset_mono_train_images.mat";
  string pathToIdxTrLabels = pathToData+"/idx/dset_mono_train_labels.mat";
  string pathToIdxTest = pathToData+"/idx/dset_mono_test_images.mat";
  string pathToIdxTeLabels = pathToData+"/idx/dset_mono_test_labels.mat";
  
  //! create an Idx file for the datasets, if 'train-only' is not supplied
  if (argc > 1) {
    if (strcmp(argv[1], "train-only") != 0) generateIdxDataSet(pathToData);
  } else if (argc == 1) generateIdxDataSet(pathToData);

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
					"Posture Training Set");
  LabeledDataSource<float,int> test_ds(testingSet, testingLabels,
				       0.0, 0.01, "Posture Testing Set");

  //! shuffle the training datasource
  train_ds.shuffle();

  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  Idx<double> targets = create_target_matrix(1+idx_max(train_ds.labels), 1.0);

  //! create the network weights, network and trainer
  IdxDim dims = train_ds.sample_dims(); // get order and dimensions of samples
  parameter theparam(120000); // create trainable parameter

  // instantiate the ConvNet
  ConvNetPosture myConvNet(theparam, // Trainable parameter
			   trainingSet.dim(3), // Input height
			   trainingSet.dim(2), // Input width
			   targets.dim(0)); // Nb of classes

  //! combine the conv net with targets -> gives a supervised system
  supervised_euclidean_machine thenet(myConvNet, targets, dims);
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

  // training, and testing...
  cout << "Training network on posture images with " << train_ds.size();
  cout << " training samples and " << test_ds.size() << " test samples" << endl;
  for (int i = 0; i < 10; ++i) {
    cout << endl;
    // Training on the whole dataset:
    thetrainer.train(train_ds, trainmeter, gdp, 1);

    // then test the current ConvNet on both training set and test set:
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
