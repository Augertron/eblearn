
/* Posture estimation ConvNet.
 * In this example we build a ConvNet based on a generic CSCSCF architecture.
 * For that we create a class named ConvNetPosture
 * Everything can be redefined for this ConvNet:
 *  - the nb of feature maps we want per layer,
 *  - the connections between them,
 *  - the size of the kernels and subsample masks...
 * 
 * In the main function, we load a database of images (pnm images) and first 
 * package them in an idx file (format used by EBLearn to handle matrices).
 * These images contiain 6 different human postures: bending, standing...
 * and are separated in two sets: one for training, the other one for testing.
 *
 * Then we load these idx files and train our ConvNet to separate the output
 * space in these 6 classes.
 *
 * By using the raw dataset, the ConvNet converges to 100% good results on the 
 * training dataset, and 86% on the testing set... in 6 iterations !
 *
 * The script expand-dataset.py (in $EBLEARN/data/posture/) can be run
 * to create other images from the raw dataset (requires Python, and the image
 * library PIL - http://www.pythonware.com/products/pil/).
 * Doing this helps reaching better results on the testing 
 * set (unknown results). The training takes much more time, obviously,
 * but should yield better results.
 *
 * Feel free to change/tweak the paramaters of the ConvNet, in the class
 * ConvNetPosture. 
 *
 * Syntax:
 *   posture             -> creates the idx files and train the ConvNet
 *   posture train-only  -> expect the idx to be present, and train the ConvNet
 *   posture test-only   -> expect the network to be trained, and runs a test
 *
 * Clement Farabet (clement.farabet@gmail.com) || April 2009.
 */

#include "libeblearn.h"
#include "libeblearntools.h"
#include "classifier_gen.h"
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;
using namespace ebl;

void generate_idx_data_set(string pathToData);
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
  idx<intg> table0;	
  idx<intg> table1;	
  idx<intg> table2;	
  
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
    table1 = idx<intg>(44, 2); // from S0 to C1
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

       {0, 11}, {1, 11}, {2, 11}, {3, 11}, {4, 11}, {5, 11}};
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
  string pathToTrainedWeights = pathToData+"/trained-convnet/weights.mat";

  //! create an idx file for the datasets, if 'train-only' is not supplied
  if (argc > 1 
      && strcmp(argv[1], "train-only") != 0
      && strcmp(argv[1], "test-only") != 0) {
    generate_idx_data_set(pathToData);
  } else if (argc == 1)
    generate_idx_data_set(pathToData);  

  init_drand(time(NULL)); // initialize random seed

  //! load dataset from idx files
  idx<float> trainingSet(1,1,1,1), testingSet(1,1,1,1);
  idx<int> trainingLabels(1), testingLabels(1);
  load_matrix(trainingSet, pathToIdxTrain.c_str());
  load_matrix(trainingLabels, pathToIdxTrLabels.c_str());
  load_matrix(testingSet, pathToIdxTest.c_str());
  load_matrix(testingLabels, pathToIdxTeLabels.c_str());

  //! create two labeled data sources, for training and testing
  labeled_datasource<float,int> train_ds(trainingSet, // Data source
					 trainingLabels, // Labels
					 0.0, // Bias to be added to images
					 0.01, // Coef to scale images
					 "Posture Training Set");
  labeled_datasource<float,int> test_ds(testingSet, testingLabels,
					0.0, 0.01, "Posture Testing Set");

  //! shuffle the training datasource
  train_ds.shuffle();

  //! create 1-of-n targets with target 1.0 for shown class, -1.0 for the rest
  idx<double> targets = create_target_matrix(1+idx_max(train_ds.labels), 1.0);

  //! create the network weights, network and trainer
  idxdim dims = train_ds.sample_dims(); // get order and dimensions of samples
  parameter myConvNetWeights(1); // create trainable parameter

  // instantiate the ConvNet
  ConvNetPosture myConvNet(myConvNetWeights, // Trainable parameter
			   trainingSet.dim(2), // Input height
			   trainingSet.dim(3), // Input width
			   targets.dim(0)); // Nb of classes

  //! combine the conv net with targets -> gives a supervised system
  supervised_euclidean_machine mySupervisedNet(myConvNet, targets, dims);
  supervised_trainer<float,int> myTrainer(mySupervisedNet, myConvNetWeights);

  //! a classifier-meter measures classification errors
  classifier_meter trainmeter, testmeter;

  //! initialize the network weights
  forget_param_linear fgp(1, 0.5);
  mySupervisedNet.forget(fgp);

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

  if (argc > 1 && strcmp(argv[1], "test-only") == 0)
    goto test;

  // estimate second derivative on 100 iterations, using mu=0.02
  cout << endl << "Training the ConvNet. Type ./posture test-only to skip" 
       << endl << endl;
  cout << "Computing second derivatives on the dataset." << endl;
  cout << "See 'Efficient Backprop', LeCun et al., 1998" << endl;
  cout << "This is essential to set up initial learning rates "
       << "for each weight in the network." << endl << endl;
  myTrainer.compute_diaghessian(train_ds, 100, 0.02);

  // training, and testing...
  cout << "Training network on posture images with " << train_ds.size();
  cout << " training samples and " << test_ds.size() << " test samples" << endl;
  for (int i = 0; i < 50; ++i) {
    cout << endl;
    // Training on the whole dataset:
    myTrainer.train(train_ds, trainmeter, gdp, 1);

    // then test the current ConvNet on both training set and test set:
    myTrainer.test(train_ds, trainmeter, infp);
    myTrainer.test(test_ds, testmeter, infp);

    double errorTrain = 1-(trainmeter.total_correct / (double)trainmeter.size);
    // Error -> 0 when training is done
    if (errorTrain == 0) 
      break;
  }

  // Store the trained conv-net to a file...
  cout << endl << "Saving weights to " << pathToTrainedWeights << endl;
  myConvNetWeights.save_x(pathToTrainedWeights.c_str());

  if (argc > 1 && strcmp(argv[1], "train-only") == 0)
    return 0;

 test:
  // Select an image to be classified. train_ds is NBx3x46x46. we keep 1x46x46
  idx<float> testSample = test_ds.data[22]; // Select a random sample
  idx<float> testSampleGrayscale = testSample[1]; // discard color
  cout << "Loading a random example in class 'hand1up'" << endl << endl;

  // instantiate the ConvNet
  ConvNetPosture myTrainedConvNet(myConvNetWeights, // Trained weights
				  testSampleGrayscale.dim(0), // Input height
				  testSampleGrayscale.dim(1), // Input width
				  targets.dim(0)); // Nb of classes

  // Load the trained conv-net, if existing
  if (myConvNetWeights.load_x(pathToTrainedWeights.c_str()) == false) {
    cout << "Couldn't load weights" << endl;
    return 0;
  } else 
    cout << "Loading weights from " << pathToTrainedWeights << endl;

  // different sizes to be recognized during classification
  // the first it the size of objects during training
  // square objects is assumed
  idx<int> objectSizes(1);
  objectSizes.set(46, 0);

  // these are the labels of the different classes 
  // each object will be classified in one of those classes
  idx<const char*> objectLabels(6);
  objectLabels.set("bend", 0);
  objectLabels.set("hand1up", 1);
  objectLabels.set("hand2up", 2);
  objectLabels.set("squart", 3);
  objectLabels.set("stand", 4);
  objectLabels.set("swinghand", 5);

  //! a classifier needs a trained module, a list of scales, and the classes
  classifier_gen<float> myClassifier(myConvNet, // The trained ConvNet
				     objectSizes,  // sizes of objects
				     objectLabels, // Labels of classes
				     testSampleGrayscale, // Sample to classify
				     0.0, // Bias to be added to inputs
				     0.01 // Coef to scale inputs
				     );

  // do a pass, classify
  myClassifier.classify(0);

  return 0;
}



// generate Idx files from a given set of images.
void generate_idx_data_set(string pathToData) {

  string pathTrainingSet = pathToData+"/train";
  string pathTestingSet = pathToData+"/test";
  string pathToIdx = pathToData+"/idx";
  
  cout << "Converting images to idx object" << endl;
  imagedir_to_idx(pathTrainingSet.c_str(), 46, ".*[.]ppm", 
		NULL, pathToIdx.c_str(), NULL, true,
		"_train", false);
  imagedir_to_idx(pathTestingSet.c_str(), 46, ".*[.]ppm", 
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
