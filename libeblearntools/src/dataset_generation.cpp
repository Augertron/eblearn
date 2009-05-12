/***************************************************************************
 *   Copyright (C) 2008 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include "dataset_generation.h"
#include <algorithm>

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

#ifdef __GUI__
#include "libidxgui.h"
#include "libeblearngui.h"
#endif

using namespace std;

namespace ebl {

#ifdef __BOOST__

  ////////////////////////////////////////////////////////////////
  // Utility functions to prepare a dataset from raw images.

  void convert_image(idx<float> src, idx<float> dst, const int channels_mode) {
    switch (channels_mode) {
    case 0: // RGB
      idx_copy(src, dst);
      break ;
    case 1: // YUV
      rgb_to_yuv(src, dst);
      break ;
    case 4: // YH3
      rgb_to_yh3(src, dst);
      break ;
    default:
      cerr << "unknown channel mode: " << channels_mode << endl;
      eblerror("unknown channel mode");
    }
  }
  
  //! Recursively goes through dir, looking for files matching extension ext.
  void process_dir(const char *dir, const char *ext, const char* leftp, 
		   const char *rightp, unsigned int width, idx<float> &images,
		   idx<int> &labels, int label, bool display, bool *binocular, 
		   const int channels_mode, const int channels_size,
		   idx<ubyte> &classes, unsigned int &counter) {
    regex eExt(ext);
    string el(".*");
    idx<float> limg(1, 1, 1);
    idx<float> rimg(1, 1, 1);
    idx<float> tmp;
    idx<float> tmp2;
    if (leftp) {
      el += leftp;
      el += ".*";
    }
    regex eLeft(el);
    cmatch what;
    path p(dir);
    if (!exists(p))
      return ;
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (is_directory(itr->status())) {
	process_dir(itr->path().string().c_str(), ext, leftp, rightp, width, 
		    images, labels, label, display, binocular, channels_mode,
		    channels_size, classes, counter);
      } else if (regex_match(itr->leaf().c_str(), what, eExt)) {
	if (regex_match(itr->leaf().c_str(), what, eLeft)) {
	  // found left image
	  // increase example number
	  labels.set(label, counter);
	  // check for right image
	  if (rightp != NULL) {
	    regex reg(leftp);
	    string rp(rightp);
	    string s = regex_replace(itr->leaf(), reg, rp);
	    string sfull = itr->path().branch_path().string();
	    sfull += "/";
	    sfull += s;
	    path r(sfull);
	    if (exists(r)) {
	      // found right image
	      *binocular = true;
	      if (image_read_rgbx(r.string().c_str(), rimg)) {
		// resize stereo dimension to twice the channels_size
		if (images.dim(3) == channels_size)
		  images.resize(images.dim(0), images.dim(1), 
				images.dim(2), images.dim(3) * 2);
		// take the right most square of the image
		if (rimg.dim(0) <= rimg.dim(1))
		  rimg = rimg.narrow(1, rimg.dim(0), rimg.dim(1) - rimg.dim(0));
		else
		  rimg = rimg.narrow(0, rimg.dim(1), rimg.dim(0) - rimg.dim(1));
		// resize image to target width
		rimg = image_resize(rimg, width, width, 1);
		tmp = images.select(0, counter);
		tmp = tmp.narrow(2, channels_size, channels_size);
		// finally copy right images to idx
		convert_image(rimg, tmp, channels_mode);
	      }
	      if (display)
		cout << "Processing (right): " << sfull << endl;
	    }
	  }
	  if (display) {
	    cout << counter << "/" << images.dim(0) << ": ";
	    cout << itr->path().string().c_str() << endl;
	  }
	  // process left image
	  if (image_read_rgbx(itr->path().string().c_str(), limg)) {
	    // take the left most square of the image
	    if (limg.dim(0) <= limg.dim(1))
	      limg = limg.narrow(1, limg.dim(0), 0);
	    else
	      limg = limg.narrow(0, limg.dim(1), 0);
	    // resize image to target width
	    limg = image_resize(limg, width, width, 1);
	    tmp = images.select(0, counter);
	    tmp = tmp.narrow(2, channels_size, 0);
	    // finally copy right images to idx
	    convert_image(limg, tmp, channels_mode);
	  }
	  // display current image conversion
#ifdef __GUI__
	  if (display && (counter % 15 == 0)) {
	    disable_window_updates();
	    clear_window();
	    unsigned int h = 0, w = 0;
	    // input (RGB)
	    static string s;
	    s = "RGB";
	    s += " - ";
	    s += (char *) classes[label].idx_ptr();
	    draw_matrix(limg, s.c_str(), h, w);
	    w = 0;
	    h += tmp.dim(1) + 5;
	    // output
	    switch (channels_mode) {
	    case 1:
	      tmp2 = tmp.select(2, 0);
	      draw_matrix(tmp2, "Y", h, w);
	      w += tmp2.dim(1) + 5;
	      tmp2 = tmp.select(2, 1);
	      draw_matrix(tmp2, "U", h, w);
	      w += tmp2.dim(1) + 5;
	      tmp2 = tmp.select(2, 2);
	      draw_matrix(tmp2, "V", h, w);
	      break ;
	    case 4:
	      tmp2 = tmp.select(2, 0);
	      draw_matrix(tmp2, "Y", h, w);
	      w += tmp2.dim(1) + 5;
	      tmp2 = tmp.select(2, 1);
	      idx_dotc(tmp2, (float)420.0, tmp2);
	      draw_matrix(tmp2, "H3", h, w, 1.0, 1.0, (float)0, (float)420);
	      //	      cout << "h3 inf: " << idx_min(tmp2) << " max: ";
	      //cout << idx_max(tmp2) << endl;
	      w += tmp2.dim(1) + 5;
	      idxdim d(limg);
	      static idx<float> rgb(images.dim(1),
				    images.dim(2), 3);
	      h3_to_rgb(tmp2, rgb);
	      draw_matrix(rgb, "H3", h, w);
	      break ;
	    }
	    enable_window_updates();
	  }
#endif
	  counter++;
	}
      }
    }
  }

  // Recursively goes through dir, looking for files matching extension ext.
  void count_matches(const char *dir, const char *pattern,
		     unsigned int &nimages) {
    regex eExt(pattern);
    cmatch what;
    path p(dir);
    if (!exists(p))
      return ;
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (is_directory(itr->status()))
	count_matches(itr->path().string().c_str(), pattern, nimages);
      else if (regex_match(itr->leaf().c_str(), what, eExt))
	nimages++;
    }
  }

  //! Return an idx of dimensions Nx2 containing all possible N similar pairs.
  idx<int> make_pairs(idx<int> &labels) {
    idx<int> pairs(1, 2);
    pairs.resize(0, pairs.dim(1));
    int n = 1;
    for (int i = 0; i < labels.dim(0); ++i) {
      for (int j = i + 1; j < labels.dim(0); ++j) {
	if (labels.get(i) == labels.get(j)) {
	  pairs.resize(n, pairs.dim(1));
	  pairs.set(i, n - 1, 0);
	  pairs.set(j, n - 1, 1);
	  n++;
	}
      }
    }
    return pairs;
  }

#endif 

  //! Finds all images corresponding to the imgPatternLeft pattern in directory 
  //! imgDir, assigns for each a class name corresponding to the first directory
  //! level found, crops to square and resizes all images to width*width size.
  //! If imgPatternRight is not null, two images are stored for each example, 
  //! bringing the stereo dimension sdim from 1 to 2.
  //! Finally outputs all N examples found in a single matrix files as follow:
  //! outDir/dset_images.mat: 
  //!   (ubyte) N x 6 (G or GG, RGB or RGBRGB) x width x width
  //! outDir/dset_labels.mat: 	(int)   N
  //! outDir/dset_classes.mat:  (ubyte) Nclasses x 128
  bool imagedir_to_idx(const char *imgDir, unsigned int width,
		       const int channels_mode,
		       const char *imgExtension, const char *imgPatternLeft, 
		       const char *outDir, const char *imgPatternRight, 
		       bool silent, bool display, const char *prefix,
		       const char *name_) {
    // local variables
    unsigned int	nimages;
    unsigned int	counter;
    int			nclasses;
    int			channels_size;
    idx<ubyte>		classes;
    idx<float>		images;
    idx<int>		labels;
    idx<ubyte>		tmp;
    int                 i;
    string              name = (name_ == NULL) ? "dataset" : name_;
    if (silent) display = false;
#ifdef __GUI__
    if (display) new_window("Dataset Compiler");
#endif
#ifdef __BOOST__
    // check root directory
    path imgp(imgDir);
    bool binocular = false;
    if (!exists(imgp)) {
      cerr << "cannot find path: " << imgp.string() << endl;
      eblerror("imagedir_to_idx: path does not exist");
      return false;
    }

    // select channels size
    channels_size = 0;
    switch (channels_mode) {
    case 0: case 1: case 2: channels_size = 3; break;
    case 3: channels_size = 1; break;      
    case 4: channels_size = 2; break;
    default: cerr << "unknown channels mode " << channels_mode << endl;
      eblerror("unknown channels mode");
    }
    
    // find classes from first-level directories
    if (!silent) cout << "Scanning classes... ";
    idx_clear(classes);
    nclasses = 0;
    cmatch what;
    regex hidden_dir(".svn");
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(imgp); itr != end_itr; ++itr) {
      if (is_directory(itr->status()) 
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	nclasses++;
      }
    }
    if (nclasses == 0) {
      eblerror("ImageDirToidx: no classes found");
      return false;
    }
    // collect class names
    classes = idx<ubyte>(nclasses, 128); // Nclasses x 128
    i = 0;
    for (directory_iterator itr(imgp); itr != end_itr; ++itr) {
      if (is_directory(itr->status()) 
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	tmp = classes.select(0, i);
	// copy class name
	memcpy(tmp.idx_ptr(), itr->leaf().c_str(), 
	       min((size_t) 128, itr->leaf().length() + 1) * sizeof (ubyte));
	++i;
      }
    }
    if (!silent) {
      cout << nclasses << " classes found (";
      i = 0;
      idx_bloop1(classe, classes, ubyte) { 
	cout << classe.idx_ptr(); 
	if (i != classes.dim(0) - 1)
	  cout << ", "; 
	i++; 
      }
      cout << ")." << endl;
    }
    
    // find number of images
    if (!silent) cout << "Counting images... ";
    nimages = 0;
    for (directory_iterator itr(imgp); itr != end_itr; itr++) {
      if (is_directory(itr->status())
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	// recursively search each directory
	count_matches(itr->path().string().c_str(), imgExtension, nimages);
      }
    }
    if (!silent) cout << nimages << " images found." << endl;

    // allocate memory
    if (!silent) {
      cout << "Allocating memory for " << nimages << "x" << channels_size;
      cout << "x" << width << "x" << width << " image buffer (";
      cout << (nimages * width * width * channels_size * sizeof (float))
	/(1024*1024);
      cout << " Mb)..." << endl;
    }
      // N x w x w x channels_size
    images = idx<float>(nimages, width, width, channels_size);
    labels = idx<int>(nimages); // N

     // collect images
    if (!silent) cout << "Collecting images..." << endl;
    counter = 0;
    i = 0;
    for (directory_iterator itr(imgp); itr != end_itr; itr++) {
      if (is_directory(itr->status())
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	// process subdirs to extract images into the single image idx
	process_dir(itr->path().string().c_str(), imgExtension, imgPatternLeft,
		    imgPatternRight, width, images, labels, i, display, 
		    &binocular, channels_mode, channels_size, classes, counter);
	++i; // increment only for directories
      }
    }

    if (!silent) {
      cout << "Collected " << images.dim(0) << " images." << endl;
      cout << "Saving images, labels, classes and pairs in ";
      cout << (outDir != NULL ? outDir: imgDir) << ":" << endl;
    }

    // filenames
    string cular(binocular? "_bino" : "");
    string dsetimages(outDir != NULL ? outDir: imgDir);
    dsetimages += "/"; dsetimages += name;
    dsetimages += cular; if (prefix) dsetimages += prefix;
    dsetimages += "_images.mat";
    string dsetlabels(outDir != NULL ? outDir: imgDir);
    dsetlabels += "/"; dsetlabels += name;
    dsetlabels += cular; if (prefix) dsetlabels += prefix;
    dsetlabels += "_labels.mat";
    string dsetclasses(outDir != NULL ? outDir: imgDir);
    dsetclasses += "/"; dsetclasses += name;
    dsetclasses += cular; if (prefix) dsetclasses += prefix;
    dsetclasses += "_classes.mat";
    string dsetpairs(outDir != NULL ? outDir: imgDir);
    dsetpairs += "/"; dsetpairs += name;
    dsetpairs += cular; if (prefix) dsetpairs += prefix;
    dsetpairs += "_pairs.mat";
    idx<int> pairs = make_pairs(labels);

    // saving files
    if (!silent) cout << "Saving " << dsetpairs << endl;
    save_matrix(pairs, dsetpairs.c_str());
    int tr[4] = { 0, 3, 1, 2 };
    if (!silent) cout << "Saving " << dsetimages << endl;
    images = images.transpose(tr);
    save_matrix(images, dsetimages.c_str());
    if (!silent) cout << "Saving " << dsetlabels << endl;
    save_matrix(labels, dsetlabels.c_str());
    if (!silent) cout << "Saving " << dsetclasses << endl;
    save_matrix(classes, dsetclasses.c_str());
#endif /* __BOOST__ */
#ifdef __GUI__
    if (display) quit_gui();
#endif
    return true;
  }

} // end namespace ebl

