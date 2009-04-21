/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

using namespace std;
using namespace boost::filesystem;
using namespace boost;

#endif

namespace ebl {

#ifdef __BOOST__

  ////////////////////////////////////////////////////////////////
  // Utility functions to prepare a dataset from raw images.

  //! Recursively goes through dir, looking for files matching extension ext.
  void process_dir(const char *dir, const char *ext, const char* leftp, 
		  const char *rightp, unsigned int width, idx<float> &images,
		  idx<int> &labels, int label, bool verbose, bool *binocular, 
		  bool toYUV) {
    regex eExt(ext);
    string el(".*");
    idx<float> limg(1, 1, 1);
    idx<float> rimg(1, 1, 1);
    idx<float> tmp;
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
		   images, labels, label, verbose, binocular, toYUV);
      } else if (regex_match(itr->leaf().c_str(), what, eExt)) {
	if (regex_match(itr->leaf().c_str(), what, eLeft)) {
	  // found left image
	  // increase example number
	  images.resize(images.dim(0) + 1, images.dim(1), images.dim(2), 
			images.dim(3));
	  labels.resize(labels.dim(0) + 1);
	  labels.set(label, labels.dim(0) - 1);
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
	      if (pnm_fread_into_rgbx(r.string().c_str(), rimg)) {
		// resize stereo dimension to 2
		if ((limg.dim(1) == 1))
		  images.resize(images.dim(0), images.dim(1), 
				images.dim(2), rimg.dim(2) * 2);
		// take the right most square of the image
		rimg = rimg.narrow(1, rimg.dim(0), rimg.dim(1) - rimg.dim(0));
		// resize image to target width
		rimg = image_resize(rimg, width, width, 1);
		tmp = images.select(0, images.dim(0) -1);
		tmp = tmp.narrow(2, rimg.dim(2), rimg.dim(2));
		// finally copy right images to idx
		if (toYUV) {
		  RGBtoYUV(rimg, tmp);
		  YUVGlobalNormalization(tmp);
		}
		else
		  idx_copy(rimg, tmp);
	      }
	      if (verbose)
		cout << "Processing right img: " << sfull << endl;
	    }
	  }
	  if (verbose)
	    cout << "Processing left img: " << itr->path().string().c_str() 
		 << endl;
	  // process left image
	  if (pnm_fread_into_rgbx(itr->path().string().c_str(), limg)) {
	    // resize images to rgb if 3 channels found
	    if ((limg.dim(2) == 3) && (images.dim(3) < 3))
	      images.resize(images.dim(0), images.dim(1), images.dim(2), 3); 
	    // take the left most square of the image
	    limg = limg.narrow(1, limg.dim(0), 0);
	    // resize image to target width
	    limg = image_resize(limg, width, width, 1);
	    tmp = images.select(0, images.dim(0) -1);
	    tmp = tmp.narrow(2, limg.dim(2), 0);
	    // finally copy right images to idx
	    if (toYUV) {
	      RGBtoYUV(limg, tmp);
	      YUVGlobalNormalization(tmp);
	    }
	    else
	      idx_copy(limg, tmp);
	  }  	    
	}
      }
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
		       const char *imgExtension, const char *imgPatternLeft, 
		       const char *outDir, const char *imgPatternRight, 
		       bool verbose, const char *prefix, bool toYUV) {
#ifdef __BOOST__
    path imgp(imgDir);
    bool binocular = false;
    if (!exists(imgp)) {
      cerr << "cannot find path: " << imgp.string() << endl;
      ylerror("imagedir_to_idx: path does not exist");
      return false;
    }
    // build class list
    int nclasses = 0;
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(imgp); itr != end_itr; ++itr) {
      if (is_directory(itr->status())) {
	nclasses++;
      }
    }
    if (nclasses == 0) {
      ylerror("ImageDirToidx: no classes found");
      return false;
    }
    if (verbose) {
      cout << nclasses << " classes found. Now collecting images..." << endl;
    }

    idx<ubyte> 	classes(nclasses, 128); // Nclasses x 128
    idx<float> 	images(1, width, width, 1); // N x w x w x rgbrgb
    idx<int>	labels(1); // N
    idx<ubyte>  tmp;

    idx_clear(classes);
    images.resize(0, images.dim(1), images.dim(2), images.dim(3)); // empty idx
    labels.resize(0); // empty idx
    int i = 0;
    for (directory_iterator itr(imgp); itr != end_itr; itr++) {
      if (is_directory(itr->status())) {
	tmp = classes.select(0, i);
	// copy class name
	memcpy(tmp.idx_ptr(), itr->leaf().c_str(), 
	       min((size_t) 128, itr->leaf().length() + 1) * sizeof (ubyte));
	// process subdirs to extract images into the single image idx
	process_dir(itr->path().string().c_str(), imgExtension, imgPatternLeft,
		   imgPatternRight, width, images, labels, i, verbose, 
		   &binocular, toYUV);
	++i; // increment only for directories
      }
    }
    if (verbose) {
      cout << "Collected " << images.dim(0) << " examples of classes (";
      int i = 0;
      idx_bloop1(classe, classes, ubyte) { 
	cout << classe.idx_ptr(); 
	if (i != classes.dim(0) - 1)
	  cout << ", "; 
	i++; 
      }
      cout << ")." << endl;
      cout << "Saving images, labels, classes and pairs in ";
      cout << (outDir != NULL ? outDir: imgDir) << endl;
    }
    string cular(binocular? "bino" : "mono");
    string dsetimages(outDir != NULL ? outDir: imgDir);
    dsetimages += "/dset_";
    dsetimages += cular;
    if (prefix) dsetimages += prefix;
    dsetimages += "_images.mat";
    string dsetlabels(outDir != NULL ? outDir: imgDir);
    dsetlabels += "/dset_";
    dsetlabels += cular;
    if (prefix) dsetlabels += prefix;
    dsetlabels += "_labels.mat";
    string dsetclasses(outDir != NULL ? outDir: imgDir);
    dsetclasses += "/dset_";
    dsetclasses += cular;
    if (prefix) dsetclasses += prefix;
    dsetclasses += "_classes.mat";
    string dsetpairs(outDir != NULL ? outDir: imgDir);
    dsetpairs += "/dset_";
    dsetpairs += cular;
    if (prefix) dsetpairs += prefix;
    dsetpairs += "_pairs.mat";
    idx<int> pairs = make_pairs(labels);
    save_matrix(pairs, dsetpairs.c_str());
    int tr[4] = { 0, 3, 1, 2 };
    images = images.transpose(tr);
    save_matrix(images, dsetimages.c_str());
    save_matrix(labels, dsetlabels.c_str());
    save_matrix(classes, dsetclasses.c_str());
#endif /* __BOOST__ */
    return true;
  }

} // end namespace ebl

