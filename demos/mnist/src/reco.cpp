#include "libeblearn.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

// NOTE: this is one way to execute a trained network on single input images.
//   In this simple program, one has to provide the correct input size based
//   on the convolution and subsampling kernels.
//   With a cscscf network with 5-convolutions and 2-subsamplings, the valid
//   input sizes are given by the following formula, given the output size (x,y)
//     in_x = (((out_x + 4) * 2) + 4) * 2 + 4
//     in_y = (((out_y + 4) * 2) + 4) * 2 + 4
//   A better way uses the classifier2D class which provides multi-scale
//   analysis, automatic input/output size computation,
//   a better answer inference and returns bounding boxes.
//   One should preferably use the Classifier2D for recognition.
//   Classifier2D is however currently under restructuration to work on generic
//   sizes.
//   TODO-0: finish classifier 2D generic version.


// argv[1] is expected to contain the directory of the mnist dataset
#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
  cout << "* MNIST recognition demo" << endl;
  if (argc > 3) {
    cout << "Usage: ./mnist_reco <trained parameter> <image file>" << endl;
    eblerror("expected trained parameter filename and image filename");
  }
  string paramfname = "mnist_trained_network.mat";
  string imagefname = "../data/mnist/2_34x34.pnm";
  if (argc > 1) paramfname = argv[1];
  if (argc > 2) imagefname = argv[2];
  cout << "Looking for digits in image " << imagefname;
  cout << " using trained network " << paramfname << endl;

  // load trained network
  parameter theparam;
  lenet5 l5(theparam, 32, 32, 5, 5, 2, 2, 5, 5, 2, 2, 120, 10);
  theparam.load_x(paramfname.c_str());

  // load image
  idx<ubyte> image(1,1,1);  
  if (!image_read_rgbx(imagefname.c_str(), image)) {
    eblerror("failed to read image");
    return -1;
  }
  // only keep 1 color channel since we trained with greyscale images
  image = image.select(2, 0);

  // copy 1 color layer of the image into input state_idx
  state_idx stin(1, image.dim(0), image.dim(1)), stout(1,1,1);
  idx<double> inx = stin.x.select(0, 0); // pointer to 2D input
  idx_copy(image, inx); // convert and copy ubyte 2D image to double 2D input

  // fprop input throught the network
  l5.fprop(stin, stout);
  int answer = idx_indexmax(stout.x); // infer answer from maximum score
  cout << "answer is " << answer << endl;

  // display internals of the fprop
  module_1_1_gui l5gui;
  l5gui.display_fprop(l5, stin, stout);
  sleep(1);
  return 0;
}
