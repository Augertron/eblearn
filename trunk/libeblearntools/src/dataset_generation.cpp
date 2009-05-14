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

  void convert_image(idx<float> src, idx<float> dst, const int channels_mode,
		     unsigned int fkernel_size) {
    idxdim d = idxdim(src), d2 = idxdim(src);
    d2.setdim(2, dst.dim(2));
    idx<float> in, out, src2(d), tmp(d2), tmp2;
    if (fkernel_size > 0) {
      src2 = src.narrow(0, src.dim(0) - fkernel_size + 1,
			floor(fkernel_size / 2));
      src2 = src2.narrow(1, src.dim(1) - fkernel_size + 1,
			 floor(fkernel_size / 2));
      tmp2 = tmp.narrow(0, tmp.dim(0) - fkernel_size + 1,
			floor(fkernel_size / 2));
      tmp2 = tmp2.narrow(1, tmp.dim(1) - fkernel_size + 1,
			 floor(fkernel_size / 2));
    }
    idx_clear(dst);
    // narrow dst to fit src if smaller
    if (src2.dim(0) < dst.dim(0))
      dst = dst.narrow(0, src2.dim(0), floor((dst.dim(0) - src2.dim(0)) / 2));
    if (src2.dim(1) < dst.dim(1))
      dst = dst.narrow(1, src2.dim(1), floor((dst.dim(1) - src2.dim(1)) / 2));
    // converting
    switch (channels_mode) {
    case 0: // RGB
      idx_copy(src2, dst);
      image_global_normalization(dst);
      break ;
    case 1: // YUV
      rgb_to_yuv(src, tmp);
      in = tmp.select(2, 0);
      d = idxdim(in);
      out = idx<float>(d);      
      image_mexican_filter(in, out, 6, fkernel_size);
      idx_copy(out, in);
      idx_copy(tmp2, dst);
      image_global_normalization(dst);
      break ;
    case 4: // YH3
      rgb_to_yh3(src, tmp);
      in = tmp.select(2, 0);
      d = idxdim(in);
      out = idx<float>(d);      
      image_mexican_filter(in, out, 6, fkernel_size);
      image_global_normalization(out);
      idx_copy(out, in);
      idx_copy(tmp2, dst);
      break ;
    case 5: // VpH2SV
      rgb_to_vph2sv(src, tmp, 6, fkernel_size);
      idx_copy(tmp2, dst);
      break ;
    default:
      cerr << "unknown channel mode: " << channels_mode << endl;
      eblerror("unknown channel mode");
    }
  }
  
  //! Recursively goes through dir, looking for files matching extension ext.
  void process_dir(const char *dir, const char *ext, const char* leftp, 
		   const char *rightp, unsigned int width,
		   unsigned int fwidth, idx<float> &images,
		   idx<int> &labels, int label, bool display, bool *binocular, 
		   const int channels_mode, const int channels_size,
		   idx<ubyte> &classes, unsigned int &counter,
		   idx<unsigned int> &counters_used, idx<int> &ds_assignment,
		   unsigned int fkernel_size) {
    regex eExt(ext);
    string el(".*");
    idx<float> limg(1, 1, 1);
    idx<float> rimg(1, 1, 1);
    idx<float> tmp;
    idx<float> tmp2;
    int current_ds;
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
		    fwidth,
		    images, labels, label, display, binocular, channels_mode,
		    channels_size, classes, counter, counters_used,
		    ds_assignment, fkernel_size);
      } else if (regex_match(itr->leaf().c_str(), what, eExt)) {
	if (regex_match(itr->leaf().c_str(), what, eLeft)) {
	  current_ds = ds_assignment.get(counter);
	  if (current_ds != -1) {
	    // found left image
	    // increase example number
	    labels.set(label, current_ds, counters_used.get(current_ds));
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
		    rimg = rimg.narrow(1, rimg.dim(0),
				       rimg.dim(1) - rimg.dim(0));
		  else
		    rimg = rimg.narrow(0, rimg.dim(1),
				       rimg.dim(0) - rimg.dim(1));
		  // resize image to target width
		  rimg = image_resize(rimg, width, width, 1);
		  tmp = images[current_ds];
		  tmp = tmp[counters_used.get(current_ds)];
		  tmp = tmp.narrow(2, channels_size, channels_size);
		  // finally copy right images to idx
		  convert_image(rimg, tmp, channels_mode, fkernel_size);
		}
		if (display)
		  cout << "Processing (right): " << sfull << endl;
	      }
	    }
	    if (display) {
	      cout << counter << "/" << ds_assignment.dim(0) << ": ";
	      cout << itr->path().string().c_str() << endl;
	    }
	    // process left image
	    if (image_read_rgbx(itr->path().string().c_str(), limg)) {
	      // take the left most square of the image
	      int h = limg.dim(0), w = limg.dim(1), newh, neww;
	      if (h > w) {
		newh = width;
		neww = (newh / (float) h) * w;
	      } else {
		neww = width;
		newh = (neww / (float) w) * h;
	      }
	      // resize image to target width
	      limg = image_resize(limg, neww, newh, 1);
	      tmp = images[current_ds];
	      tmp = tmp.select(0, counters_used.get(current_ds));
	      tmp = tmp.narrow(2, channels_size, 0);
	      // finally copy right images to idx
	      convert_image(limg, tmp, channels_mode, fkernel_size);

	      // display current image conversion
#ifdef __GUI__
	      if ((display) && (counter % 15 == 0)) {
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
		  draw_matrix(tmp2, "Y", h, w, 1.0, 1.0,
			      (float)-1.0, (float)1.0);
		  w += tmp2.dim(1) + 5;
		  tmp2 = tmp.select(2, 1);
		  draw_matrix(tmp2, "U", h, w, 1.0, 1.0,
			      (float)-1.0, (float)1.0);
		  w += tmp2.dim(1) + 5;
		  tmp2 = tmp.select(2, 2);
		  draw_matrix(tmp2, "V", h, w, 1.0, 1.0,
			      (float)-1.0, (float)1.0);
		  break ;
		case 4: {
		  tmp2 = tmp.select(2, 0);
		  draw_matrix(tmp2, "Y", h, w, 1.0, 1.0,
			      (float)-1.0, (float)1.0);
		  w += tmp2.dim(1) + 5;
		  tmp2 = tmp.select(2, 1);
		  draw_matrix(tmp2, "H3", h, w, 1.0, 1.0,
			      (float)-1.0,(float)1.0);
		  idx_addc(tmp2, (float)1.0, tmp2);
		  idx_dotc(tmp2, (float)210.0, tmp2);
		  w += tmp2.dim(1) + 5;
		  idxdim d(limg);
		  static idx<float> rgb(images.dim(2),
					images.dim(3), 3);
		  h3_to_rgb(tmp2, rgb);
		  draw_matrix(rgb, "H3", h, w);
		}
		  break ;
		case 5:
		  tmp2 = tmp.select(2, 0);
		  draw_matrix(tmp2, "Vp", h, w, 1.0, 1.0,
			      (float)-1.0, (float)1.0);
		  w += tmp.dim(1) + 5;
		  tmp2 = tmp.select(2, 1);
		  draw_matrix(tmp2, "H1", h, w, 1.0, 1.0,
			      (float)-1.0, (float)1.0);
		  w += tmp.dim(1) + 5;
		  tmp2 = tmp.select(2, 2);
		  draw_matrix(tmp2, "H2", h, w, 1.0, 1.0,
			      (float)-1.0, (float)1.0);
		  w += tmp.dim(1) + 5;
		  tmp2 = tmp.select(2, 3);
		  draw_matrix(tmp2, "S", h, w, 1.0, 1.0,
			      (float)-1.0, (float)1.0);
		  w += tmp.dim(1) + 5;
		  tmp2 = tmp.select(2, 4);
		  draw_matrix(tmp2, "V", h, w, 1.0, 1.0,
			      (float)-1.0, (float)1.0);
		  w += tmp.dim(1) + 5;
		  break ;
		}
		enable_window_updates();
		//		sleep(1);
	      }
#endif
	    // increment counter for dataset current_ds
	    counters_used.set(counters_used.get(current_ds) + 1, current_ds);
	    }
	  }
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

  ////////////////////////////////////////////////////////////////
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
		       const char *name_, int max_per_class,
		       idx<ubyte> *datasets_names_,
		       unsigned int fkernel_size) {
    // local variables
    idx<ubyte>          datasets_names = datasets_names_ ? *datasets_names_
      : idx<ubyte>(1, 1);      
    int                 ndatasets = datasets_names.dim(0);
    unsigned int	nimages;
    unsigned int        nimages_used;
    unsigned int	counter;
    idx<unsigned int>	counters_used(ndatasets);
    int			nclasses;
    int			channels_size;
    idx<ubyte>		classes;
    idx<float>		images;
    idx<int>		labels;
    idx<ubyte>		tmp;
    int                 i;
    string              name = (name_ == NULL) ? "dataset" : name_;
    idx<int>            class_ranges;
    idx<int>            ds_assignment;
    if (fkernel_size > 0) width += fkernel_size - 1;
    unsigned int        fwidth = fkernel_size == 0 ? width :
      width - fkernel_size + 1;
    cout << "width : " << width << endl;
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
    case 5: channels_size = 5; break;
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
    
    // find number of images
    if (!silent) cout << "Counting images... ";
    nimages = 0;
    class_ranges = idx<int>(nclasses, 2);
    i = 0;
    for (directory_iterator itr(imgp); itr != end_itr; itr++) {
      if (is_directory(itr->status())
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	class_ranges.set(nimages, i, 0);
	// recursively search each directory
	count_matches(itr->path().string().c_str(), imgExtension, nimages);
	class_ranges.set(nimages - 1, i, 1);
	if ((max_per_class > 0) &&
	    (class_ranges.get(i, 1) + 1 - class_ranges.get(i, 0) <
	     max_per_class)) {
	  cout << "warning in dataset_generation: class ";
	  cout << classes[i].idx_ptr() << " has ";
	  cout << class_ranges.get(i, 1) + 1 - class_ranges.get(i, 0);
	  cout << " elements but max_per_class = " << max_per_class << endl;
	}
	++i;
      }
    }
    if (!silent) {
      cout << nimages << " images found." << endl;
      cout << nclasses << " classes found:" << endl;
      i = 0;
      idx_bloop2(classe, classes, ubyte, range, class_ranges, int) { 
	cout << "  " << classe.idx_ptr() << " (";
	cout << range.get(1) - range.get(0) + 1 << ")" << endl;
	i++; 
      }
    }

    // allocate memory
    nimages_used = (max_per_class == -1) ? nimages : nclasses * max_per_class;
    if (!silent) {
      cout << "Allocating memory for " << ndatasets << "x";
      cout << nimages_used << "x" << channels_size;
      cout << "x" << fwidth << "x" << fwidth << " image buffer (";
      cout << (ndatasets * nimages_used * fwidth * fwidth
	       * channels_size * sizeof (float)) / (1024*1024);
      cout << " Mb)..." << endl;
    }
      // N x w x w x channels_size
    images = idx<float>(ndatasets, nimages_used, fwidth, fwidth, channels_size);
    labels = idx<int>(ndatasets, nimages_used); // N
    ds_assignment = idx<int>(nimages);

    if (!silent) { cout << "Shuffling image collection..." << endl; }
    init_drand(time(NULL)); // initialize random seed
    // for each class, assign images to training or testing set
    if (max_per_class == -1)
      idx_fill(ds_assignment, 0); // put all images in dataset 0
    else {
      idx_fill(ds_assignment, -1); // -1 means no assignment
      idx_bloop2(range, class_ranges, int, classe, classes, ubyte) {
	int j;
	int k;
	int pos;
	for (k = 0; k < max_per_class; ++k) {
	  pos = (int) drand(range.get(0), range.get(1));
	  // if conflict, assign next available slot
	  if (ds_assignment.get(pos) != -1) {
	    for (j = pos + 1; j != pos; j++) {
	      if (j == range.get(1) + 1) // reach end, go to beginning
		j = range.get(0);
	      if (ds_assignment.get(j) == -1) {
		pos = j;
		break ;
	      }
	    }
	    if (ds_assignment.get(pos) != -1) { // did not find an image
	      cout << "warning: only " << k << " available images for class ";
	      cout << classe.idx_ptr() << " in dataset 0." << endl;
	      break ;
	    }
	  }
	  ds_assignment.set(0, pos); // assign to dataset 0
	}
	for (k = 0; (k < max_per_class) && (j < range.get(1)); ++j, ++k) {
	  pos = (int) drand(range.get(0), range.get(1));
	  // if conflict, assign next available image
	  if (ds_assignment.get(pos) != -1) {
	    for (j = pos + 1; j != pos; j++) {
	      if (j == range.get(1) + 1) // reach end, go to beginning
		j = range.get(0);
	      if (ds_assignment.get(j) == -1) {
		pos = j;
		break ;
	      }
	    }
	    if (ds_assignment.get(pos) != -1) { // did not find an image
	      cout << "warning: only " << k << " available images for class ";
	      cout << classe.idx_ptr() << " in dataset 1." << endl;
	      break ;
	    }
	  }
	  ds_assignment.set(1, pos); // assign to dataset 1
	}
      }
    }

     // collect images
    if (!silent) cout << "Collecting images..." << endl;
    counter = 0;
    idx_bloop1(cused, counters_used, unsigned int) {
      cused.set(0);
    }
    i = 0;
    for (directory_iterator itr(imgp); itr != end_itr; itr++) {
      if (is_directory(itr->status())
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	// process subdirs to extract images into the single image idx
	process_dir(itr->path().string().c_str(), imgExtension, imgPatternLeft,
		    imgPatternRight, width, fwidth, images, labels, i, display, 
		    &binocular, channels_mode, channels_size, classes, counter,
		    counters_used, ds_assignment, fkernel_size);
	++i; // increment only for directories
      }
    }

    if (!silent) {
      cout << "Collected " << idx_sum(counters_used) << " images:" << endl;
      idx_bloop2(cused, counters_used, unsigned int,
		 dsname, datasets_names, ubyte) {
	cout << "  collected " << cused.get() << " images for ";
	cout << dsname.idx_ptr() << " dataset." << endl;
      }
      cout << "Saving images, labels, classes and pairs in ";
      cout << (outDir != NULL ? outDir: imgDir) << ":" << endl;
    }

    // put layers dimension first, easier for neural nets
    int tr[5] = { 0, 1, 4, 2, 3 };
    images = images.transpose(tr);

    // save each dataset
    idx_bloop4(dsname, datasets_names, ubyte,
	       dsimages, images, float,
	       dslabels, labels, int,
	       cntused, counters_used, unsigned int) {
      // filenames
      string cular(binocular? "_bino" : "");
      string dsetimages(outDir != NULL ? outDir: imgDir);
      dsetimages += "/"; dsetimages += name;
      dsetimages += cular;
      if (prefix) { dsetimages += "_"; dsetimages += prefix; }
      dsetimages += "_"; dsetimages += (const char *) dsname.idx_ptr();
      dsetimages += "_images.mat";
      string dsetlabels(outDir != NULL ? outDir: imgDir);
      dsetlabels += "/"; dsetlabels += name;
      dsetlabels += cular;
      if (prefix) { dsetlabels += "_"; dsetlabels += prefix; }
      dsetlabels += "_"; dsetlabels += (const char *) dsname.idx_ptr();
      dsetlabels += "_labels.mat";
      string dsetclasses(outDir != NULL ? outDir: imgDir);
      dsetclasses += "/"; dsetclasses += name;
      dsetclasses += cular;
      if (prefix) { dsetclasses += "_"; dsetclasses += prefix; }
      dsetclasses += "_"; dsetclasses += (const char *) dsname.idx_ptr();
      dsetclasses += "_classes.mat";
      string dsetpairs(outDir != NULL ? outDir: imgDir);
      dsetpairs += "/"; dsetpairs += name;
      dsetpairs += cular;
      if (prefix) { dsetpairs += "_"; dsetpairs += prefix; }
      dsetpairs += "_"; dsetpairs += (const char *) dsname.idx_ptr();
      dsetpairs += "_pairs.mat";
      idx<int> pairs = make_pairs(dslabels); // FIXME

      // saving files
      if (pairs.dim(0) > 0) pairs = pairs.narrow(0, cntused.get(), 0);
      idx<float> dsimages_used = dsimages.narrow(0, cntused.get(), 0);
      idx<int> dslabels_used = dslabels.narrow(0, cntused.get(), 0);
      if (!silent) cout << "Saving " << dsetpairs << endl;
      save_matrix(pairs, dsetpairs.c_str());
      if (!silent) cout << "Saving " << dsetimages << endl;
      save_matrix(dsimages_used, dsetimages.c_str());
      if (!silent) cout << "Saving " << dsetlabels << endl;
      save_matrix(dslabels_used, dsetlabels.c_str());
      if (!silent) cout << "Saving " << dsetclasses << endl;
      save_matrix(classes, dsetclasses.c_str());
    }
#endif /* __BOOST__ */
#ifdef __GUI__
    if (display) quit_gui();
#endif
    return true;
  }

} // end namespace ebl

