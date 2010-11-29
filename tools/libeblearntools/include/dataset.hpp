/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
 *   All rights reserved.
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

#ifndef DATASET_HPP_
#define DATASET_HPP_

#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits>
#include <typeinfo>

#ifdef __GUI__
#include "libidxgui.h"
#endif

#include "defines_tools.h"

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <class Tdata>
  dataset<Tdata>::dataset(const char *name_, const char *inroot_) {
    // initialize members
    allocated = false;
    set_name(name_);
    if (inroot_) {
      inroot = inroot_;
      inroot += "/";
    } else inroot = "";
    no_outdims = true;
    outdims = idxdim(96, 96, 1);
    height = outdims.dim(0);
    width = outdims.dim(1);
    mindims = idxdim(1, 1);
    max_data = 0;
    max_data_set = false;
    total_samples = 0;
    display_extraction = false;
    display_result = false;
    // preprocessing
    ppconv_type = "";
    ppconv_set = false;
    ppmodule = NULL;
    resizepp = NULL;
    extension = IMAGE_PATTERN_MAT;
    sleep_display = false;
    sleep_delay = 0;
    // assuming already processed data, but the user can still require
    // processing via the set_pp_conversion method.
    do_preprocessing = false;
    scale_mode = false;
    scales.push_back(1); // initialize with scale 1
    data_cnt = 0;
    resize_mode = "bilinear";
    interleaved_input = true;
    max_per_class_set = false;
    mpc = (numeric_limits<intg>::max)();
    dynamic_init_drand(); // initialize random seed
    usepose = false;
    useparts = false;
    usepartsonly = false;
    save_mode = DATASET_SAVE;
    bboxhfact = 1.0;
    bboxwfact = 1.0;
    bbox_woverh = 0.0; // 0.0 means not set
    force_label = "";
    nclasses = 0;
    add_errors = 0;
    nopadded = false;
    wmirror = false;
    // jitter
    hjitter = 0;
    wjitter = 0;
    nsjitter = 0;
    sjitter = 0;
    njitter = 0;
#ifndef __BOOST__
    eblerror(BOOST_LIB_ERROR);
#endif
  }

  template <class Tdata>
  dataset<Tdata>::~dataset() {
    if (ppmodule)
      delete ppmodule;
    if (resizepp)
      delete resizepp;
  }

  template <class Tdata>
  bool dataset<Tdata>::alloc(intg max) {
    // save maximum number of samples if specified
    if (max > 0) {
      max_data = max;
      max_data_set = true;
    cout << "Limiting dataset to " << max << " samples." << endl;
    }
    // count samples
    cout << "Counting number of samples in " << inroot << " ..." << endl;
    if (!this->count_samples())
      eblerror("failed to count samples to be compiled");
    cout << "Found: " << total_samples << " total samples." << endl;
    if (njitter > 0) {
      total_samples *= njitter;
      cout << "Jitter is on with " << njitter << " jitters per sample, "
	   << "bringing total samples to " << total_samples << endl;
    }
    if (wmirror) {
      total_samples *= 2;
      cout << "Vertical-axis mirroring is on, "
	   << "bringing total samples to " << total_samples << endl;
    }
    if (total_samples == 0)
      return false;
    if (classes.size() == 0)
      eblerror("found 0 class");
    cout << "Found: "; print_classes(); cout << "." << endl;
    // (re)init max per class, knowing number of classes
    intg m = (numeric_limits<intg>::max)();
    if (max_per_class_set) { // mpc has already been set
      m = mpc;
      set_max_per_class(m);
    }
    // allocate 
    if (!allocate(total_samples, outdims))
      eblerror("allocation failed");
    // alloc tallies
    add_tally = idx<intg>(classes.size());
    idx_clear(add_tally);
    return true;
  }
  
  ////////////////////////////////////////////////////////////////
  // data extraction

  template <class Tdata>
  bool dataset<Tdata>::extract() {
    init_preprocessing();
    // extract
#ifdef __BOOST__
    if (!allocated && !scale_mode)
      return false;
    cmatch what;
    regex hidden_dir(".svn");    
    bool found = false;
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(inroot); itr != end_itr; itr++) {
      if (is_directory(itr->status())
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	found = true;
	// process subdirs to extract images into the single image idx
	process_dir(itr->path().string(), extension, itr->leaf());
      }}
    if (found) {
      cerr << "Samples adding failures: " << add_errors << endl;
    } else {
      cerr << "No class found in " << inroot << endl;
      return false;
    }
#endif /* __BOOST__ */
    return true;
  }
  
  ////////////////////////////////////////////////////////////////
  // data

  template <class Tdata>
  bool dataset<Tdata>::split_max_and_save(const char *name1, const char *name2,
					  intg max, const string &outroot) {
    dataset<Tdata> ds1(name1);
    dataset<Tdata> ds2(name2);
    ds1.set_outdims(outdims);
    ds2.set_outdims(outdims);
    split_max(ds1, ds2, max);
    bool ret1 = ds1.save(outroot);
    bool ret2 = ds2.save(outroot);
    return ret1 || ret2;
  }
  
  template <class Tdata>
  void dataset<Tdata>::split_max(dataset<Tdata> &ds1, dataset<Tdata> &ds2,
				 intg max) {
    cout << "Splitting \"" << name << "\" into datasets \"";
    cout << ds1.name << "\" and \"" << ds2.name << "\", limiting dataset \"";
    cout << ds1.name << "\" to " << max << " samples per class, the rest ";
    cout << "going to \"" << ds2.name << "\"." << endl;

    // copy classes strings
    if (classes.size() > 0) {
      idx<ubyte> classidx = build_classes_idx();
      ds1.set_classes(classidx);
      ds2.set_classes(classidx);
    }
    ds1.nclasses = nclasses;
    ds2.nclasses = nclasses;
    // set max samples per class for dataset 1 (ds2 takes whatever is left)
    ds1.set_max_per_class(max);
    // split
    split(ds1, ds2);
  }

  template <class Tdata>
  void dataset<Tdata>::shuffle() {
    cout << "Shuffling dataset \"" << name << "\"." << endl;
    dynamic_init_drand(); // initialize random seed
    idx_shuffle_together(data, labels, 0);
  }
  
  ////////////////////////////////////////////////////////////////
  // data preprocessing

  template <class Tdata>
  void dataset<Tdata>::set_pp_conversion(const char *conv_type,
					 uint ppkernel_size_) {
    if (strcmp(conv_type, "")) {
      ppconv_type = conv_type;
      ppkernel_size = ppkernel_size_;
      ppconv_set = true;
      do_preprocessing = true;
      cout << "Setting preprocessing image conversion to ";
      cout << conv_type << " format." << endl;
    }
  }

  ////////////////////////////////////////////////////////////////
  // accessors
    
  template <class Tdata>
  const idxdim& dataset<Tdata>::get_sample_outdim() {
    return outdims;
  }

  template <class Tdata>
  intg dataset<Tdata>::size() {
    return data_cnt;
  }

  template <class Tdata>
  t_label dataset<Tdata>::get_label_from_class(const string &class_name) {
    t_label label = 0;
    vector<string>::iterator res;
    res = find(classes.begin(), classes.end(), class_name);
    if (res == classes.end()) { // not found
      return -42; // excluded class
    }
    // found
    label = res - classes.begin();
    return label;
  }
  
  template <class Tdata>
  void dataset<Tdata>::set_display(bool display_) {
#ifdef __GUI__
    if (display_) {
      cout << "Enabling display." << endl;
      new_window("Dataset compiler");
      display_extraction = display_;
    }
#endif /* __GUI__ */
  }
    
  template <class Tdata>
  void dataset<Tdata>::set_sleepdisplay(uint delay) {
    if (delay > 0) {
      cout << "Enabling sleeping display for " << delay << "ms." << endl;
      sleep_display = true;
      sleep_delay = delay;
    }
  }
    
  template <class Tdata>
  void dataset<Tdata>::set_resize(const string &resize_mode_) {
    cout << "Setting resize mode: " << resize_mode_ << endl;
    resize_mode = resize_mode_;
  }
    
  template <class Tdata>
  bool dataset<Tdata>::full(t_label label) {
    if ((max_data_set && (data_cnt >= max_data)) ||
	((data.order() > 0) && (data_cnt >= data.dim(0))))
      return true;
    if (!scale_mode) {
      if (label == -42) // excluded class
	return true;
      if (max_per_class_set && (label >= 0) &&
	  (add_tally.get(label) >= max_per_class.get(label)))
	return true;
    }
    return false;
  }

  ////////////////////////////////////////////////////////////////
  // print methods
  
  template <class Tdata>
  void dataset<Tdata>::print_classes() {
    cout << nclasses << " classe";
    if (nclasses > 1) cout << "s";
    if (classes.size() > 0) {
      cout << " (";
      uint i;
      for (i = 0; i < classes.size() - 1; ++i)
	cout << classes[i] << ", ";
      cout << classes[i] << ")";
    }
  }

  template <class Tdata>
  void dataset<Tdata>::print_stats() {
    compute_stats();
    // print stats
    cout << "Dataset \"" << name << "\" contains " << data_cnt;
    cout << " samples (of dimensions " << outdims << " and ";
    cout << typeid(Tdata).name() << " precision)";
    cout << ", distributed in " << nclasses << " classes";
    if (classes.size() > 0) {
      uint i;
      cout << ": ";
      for (i = 0; i < classes.size() - 1; ++i) {
	cout << class_tally.get(i) << " " << classes[i];
	class_tally.get(i) > 1 ? cout << "s, " : cout << ", ";
      }
      cout << class_tally.get(i) << " " << classes[i];
      class_tally.get(i) > 1 ? cout << "s." : cout << ".";
    }
    cout << endl;
  }
  
  ////////////////////////////////////////////////////////////////
  // I/O
    
  template <class Tdata>
  bool dataset<Tdata>::load(const string &root) {
    string fname;
    string root1 = root;
    root1 += "/";
    cout << "Loading dataset " << name << " from " << root1;
    cout << name << "_*" << MATRIX_EXTENSION << endl;
    // load data
    data = idx<Tdata>(1,1,1,1); // TODO: implement generic load_matrix
    fname = root1; fname += data_fname;
    loading_error(data, fname);
    // load labels
    labels = idx<t_label>(1); // TODO: implement generic load_matrix
    fname = root1; fname += labels_fname;
    loading_error(labels, fname);
    // load classes
    idx<ubyte> classidx;
    classidx = idx<ubyte>(1,1); // TODO: implement generic load_matrix
    fname = root1; fname += classes_fname;
    if (loading_warning(classidx, fname))
      set_classes(classidx);
    else
      nclasses = idx_max(labels) + 1;
    // load classpairs
    classpairs = idx<t_label>(1,1); // TODO: implement generic load_matrix
    fname = root1; fname += classpairs_fname;
    loading_warning(classpairs, fname);
    // load deformation pairs
    deformpairs = idx<t_label>(1,1); // TODO: implement generic load_matrix
    fname = root1; fname += deformpairs_fname;
    loading_warning(deformpairs, fname);
    // initialize some members
    data_cnt = data.dim(0);
    allocated = true;
    idx<Tdata> sample = data.select(0, 0);
    outdims = sample.get_idxdim();
    print_stats();
    return true;
  }
  
  template <class Tdata>
  bool dataset<Tdata>::save(const string &root) {
    if (!allocated)
      return false;
    string root1 = root;
    root1 += "/";
    cout << "Saving " << name << " in " << save_mode << " mode." << endl;
    // return false if no samples
    if (data_cnt <= 0) {
      cerr << "Warning: No samples were added to the dataset, nothing to save.";
      cerr << endl;
      return false;
    }
    // creating directory
    mkdir_full(root1);
    // remove for empty slots
    idx<Tdata> dat = data;
    idx<t_label> labs = labels;
    if (data_cnt < data.dim(0)) {
      cerr << "Warning: not all samples were added to dataset (";
      cerr << data.dim(0) - data_cnt << " missing on " << data.dim(0);
      cerr << ")" << endl;
      cout << "Downsizing dataset to igore missing samples." << endl;
      dat = dat.narrow(0, data_cnt, 0);
      labs = labs.narrow(0, data_cnt, 0);
    }
    // switch between saving modes
    if (!strcmp(save_mode.c_str(), DATASET_SAVE)) { // dataset mode
      cout << "Saving dataset " << name << " in " << root << "/";
      cout << name << "_*" << MATRIX_EXTENSION << endl;
      // save data
      string fname = root1;
      fname += data_fname;
      cout << "Saving " << fname << " (" << dat << ")" << endl;
      if (!save_matrix(dat, fname)) {
	cerr << "error: failed to save " << fname << endl;
	return false;
      } else cout << "Saved " << fname << endl;
      // save labels
      fname = root1;
      fname += labels_fname;
      cout << "Saving " << fname << " (" << labs << ")"  << endl;
      if (!save_matrix(labs, fname)) {
	cerr << "error: failed to save labels into " << fname << endl;
	return false;
      } else cout << "Saved " << fname << endl;
      // save classes
      if (classes.size() > 0) {
	fname = root1;
	fname += classes_fname;
	idx<ubyte> classes_idx = build_classes_idx();
	cout << "Saving " << fname << " (" << classes_idx << ")"  << endl;
	if (!save_matrix(classes_idx, fname)) {
	  cerr << "error: failed to save classes into " << fname << endl;
	  return false;
	} else cout << "Saved " << fname << endl;
      }
    } else { // single file mode, use save as image extensions
//       root1 += name; root1 += "/";
//       mkdir_full(root1);
//       // save all images
//       ostringstream fname;
//       intg id = 0;
//       idx<Tdata> tmp;
//       idx_bloop2(dat, data, Tdata, lab, labs, t_label) {
// 	// make class directory if necessary
// 	fname.str("");
// 	fname << root1 << "/" << get_class_string(lab.get()) << "/";
// 	mkdir_full(fname.str().c_str());
// 	// save image
// 	fname << get_class_string(lab.get()) << "_" << id++ << "." << save_mode;
// 	tmp = dat.shift_dim(0, 2); // shift from planar to interleaved
// 	// scale image to 0 255 if preprocessed
// 	if (strcmp(ppconv_type.c_str(), "RGB")) {
// 	  idx_addc(tmp, (Tdata) 1.0, tmp);
// 	  idx_dotc(tmp, (Tdata) 127.5, tmp);
// 	}
// 	if (save_image(fname.str(), tmp, save_mode.c_str()))
// 	  cout << id << ": saved " << fname.str() << endl;
//      }
    }
    return true;
  }

  ////////////////////////////////////////////////////////////////
  // allocation

  template <class Tdata>
  bool dataset<Tdata>::allocate(intg n, idxdim &d) {
    // allocate only once
    //if (allocated)
    //  return false;
    // initialize members
    outdims = d;
    data_cnt = 0;
    // if max_data has been set, max n with max_data
    if (max_data_set)
      n = MIN(n, max_data);
    // max with max_per_class
    if (max_per_class_set)
      n = (std::max)((intg) 0, MIN(n, idx_sum(max_per_class)));
    cout << "Allocating dataset \"" << name << "\" with " << n;
    cout << " samples of size " << d << " (" 
	 << (n * d.nelements() * sizeof (Tdata)) / (1024 * 1024)
	 << " Mb) ..." << endl;
    if (n <= 0) {
      cerr << "Cannot allocate " << n << " samples." << endl;
      return false;
    }
    // allocate data buffer
    uint c = 0, h = 1, w = 2;
    if (interleaved_input) {
      c = 2; h = 0; w = 1;
    }
    idxdim datadims(outdims.dim(c), outdims.dim(h), outdims.dim(w));
    datadims.insert_dim(n, 0);
    data = idx<Tdata>(datadims);
    // allocate labels buffer
    labels = idx<t_label>(n);
    allocated = true;
    // alloc tally
    if (nclasses > 0) {
      add_tally = idx<intg>(nclasses);
      idx_clear(add_tally);
    }
    return true;
  }
  
  template <class Tdata>
  void dataset<Tdata>::init_preprocessing() {
    // initialize preprocessing module
    if (ppmodule) delete ppmodule;
    if (!strcmp(ppconv_type.c_str(), "YpUV")) {
      ppmodule = new rgb_to_ypuv_module<fs(Tdata)>(ppkernel_size);
      // set min/max val for display
      minval = (Tdata) -1;
      maxval = (Tdata) 1;
    } else if (!strcmp(ppconv_type.c_str(), "Yp")) {
      ppmodule = new rgb_to_yp_module<fs(Tdata)>(ppkernel_size);
      // set min/max val for display
      minval = (Tdata) -1;
      maxval = (Tdata) 1;
    } else if (!strcmp(ppconv_type.c_str(), "YUV")) {
      eblerror("YUV pp module not implemented");
    } else if (!strcmp(ppconv_type.c_str(), "HSV")) {
      eblerror("HSV pp module not implemented");
    } else if (!strcmp(ppconv_type.c_str(), "RGB")) {
      // no preprocessing module, just set min/max val for display
      minval = (Tdata) 0;
      maxval = (Tdata) 255;
    } else eblerror("undefined preprocessing method");
    // initialize resizing module
    if (resizepp) delete resizepp;
    if (!strcmp(resize_mode.c_str(), "bilinear"))
      resizepp = new resizepp_module<fs(Tdata)>
	(height, width, BILINEAR_RESIZE, ppmodule, ppkernel_size);
    else if (!strcmp(resize_mode.c_str(), "gaussian"))
      resizepp = new resizepp_module<fs(Tdata)>
	(height, width, GAUSSIAN_RESIZE, ppmodule, ppkernel_size);
    else if (!strcmp(resize_mode.c_str(), "mean"))
      resizepp = new resizepp_module<fs(Tdata)>
	(height, width, MEAN_RESIZE, ppmodule, ppkernel_size);
    else eblerror("undefined resizing method");
  }    
  
  ////////////////////////////////////////////////////////////////
  // data
    
  template <class Tdata>
  bool dataset<Tdata>::add_data(idx<Tdata> &dat, const t_label label,
				const string *class_name,
				const char *filename, const rect<int> *r,
				pair<uint,uint> *center) {
#ifdef __DEBUG__
    cout << "Adding image " << dat << " with label " << label;
    if (class_name) cout << ", class name " << *class_name;
    if (r) cout << ", ROI " << *r;
    if (center) cout << ", center " << center->first << "," << center->second;
    cout << " from " << (filename?filename:"") << endl;
#endif
    try {
      // check for errors
      if (!allocated)
	eblthrow("dataset has not been allocated, cannot add data.");
      // check that input is bigger than minimum dimensions allowed
      if ((dat.dim(0) < mindims.dim(0))
	  || (r && (r->height < (uint) mindims.dim(0)))
	  || (dat.dim(1) < mindims.dim(1))
	  || (r && (r->width < (uint) mindims.dim(1))))
	eblthrow("not adding " << *class_name << " from " 
		 << (filename?filename:"")
		 << ": smaller than minimum dimensions (" << dat << " < " 
		 << mindims << ")");
      // check that class exists (may not exist if excluded)
      if (classes.size() > 0 && 
	  find(classes.begin(), classes.end(), *class_name) == classes.end())
	eblthrow("not adding " << *class_name << " from " 
		 << (filename?filename:"") << ", this class is excluded.");
      // draw random jitter
      vector<jitter> jitt;
      if (njitter > 0) {
	// draw last n random pairs
	for (uint i = 0; i < njitter; ++i) {
	  if (random_jitter.size() == 0) // refill vector of random jitters
	    compute_random_jitter();
	  jitt.push_back(random_jitter.back());
	  random_jitter.pop_back();
	}
      }
      if (jitt.size() == 0)
	jitt.push_back(jitter(0,0,1.0)); // no jitter
      // add all jittered samples
      vector<jitter>::iterator ijit;
      for (ijit = jitt.begin(); ijit != jitt.end(); ++ijit) {
	// check for capacity
	if (!strcmp(save_mode.c_str(), DATASET_SAVE) && full(label))
	  // reached full capacity
	  eblthrow("not adding " << *class_name << " from " 
		   << (filename?filename:"")
		   << ", reached full capacity for this class.");
	// copy data into target type
	idxdim d(dat);
	idx<Tdata> sample(d);
	idx_copy(dat, sample);
	// do preprocessing
	if (do_preprocessing)
	  sample = preprocess_data(sample, class_name, true, filename, r, 0,
				   true, NULL, center, &(*ijit));
	// add mirrors
	if (wmirror) {
	  // flip using vertical axis
	  idx<Tdata> flipped = idx_flip(sample, 1);
	  cout << "(vertical-axis mirror) ";
	  add_data2(flipped, label, class_name, filename, &(*ijit));	  
	}
	// add/save sample
	add_data2(sample, label, class_name, filename, &(*ijit));
      }
      return true;
    } catch(eblexception &e) {
      cerr << "warning: " << e << endl;
      return false;
    }
  }

  template <class Tdata>
  void dataset<Tdata>::add_data2(idx<Tdata> &sample, t_label label,
				 const string *class_name,
				 const char *filename, jitter *jitt) {
    // check for capacity
    if (!strcmp(save_mode.c_str(), DATASET_SAVE) && full(label))
      // reached full capacity
      eblthrow("not adding " << (class_name?*class_name:"sample")
	       << " from " << (filename?filename:"")
	       << ", reached full capacity for this class.");
    // check for dimensions
    if (!sample.same_dim(outdims) && !no_outdims) {
      idxdim d2(sample);
      d2.setdim(2, outdims.dim(2)); // try with same # of channels
      if (d2 == outdims) {
	// same size except for the channel dimension, replicate it
	cout << "duplicating image channel (" << sample;
	cout << ") to fit target (" << outdims << ")." << endl;
	idx<Tdata> sample2(d2);
	idx_bloop2(samp2, sample2, Tdata, samp, sample, Tdata) {
	  idx_bloop2(s2, samp2, Tdata, s, samp, Tdata) {
	    for (intg i = 0, j = 0; i < sample2.dim(2); ++i, ++j) {
	      if (j >= sample.dim(2))
		j = 0;
	      s2.set(s.get(j), i);
	    }
	  }
	}
	sample = sample2;
      } else
	eblthrow("expected data with dimensions " << outdims
		 << " but found " << sample.get_idxdim());
    }
    // increase counter for that class
    add_tally.set(add_tally.get(label) + 1, label);
    // print info
    cout << data_cnt+1 << ": add ";
    cout << (filename ? filename : "sample" );
    if (class_name)
      cout << " as " << *class_name;
    cout << " (" << label << ")";
    if (jitt)
      cout << " (jitter " << jitt->h << "," << jitt->w << "," << jitt->s << ")";
    cout << endl;
    // if save_mode is dataset, cpy to dataset, otherwise save individ file
    if (!strcmp(save_mode.c_str(), DATASET_SAVE)) { // dataset mode
      // put sample's channels dimensions first, if interleaved.
      if (interleaved_input)
	sample = sample.shift_dim(2, 0);
      // copy sample
      idx<Tdata> tgt = data.select(0, data_cnt);
      idx_copy(sample, tgt);
      // copy label
      labels.set(label, data_cnt);
    } else {
      ostringstream fname;
      fname.str("");
      fname << outdir << "/" << get_class_string(label) << "/";
      mkdir_full(fname.str().c_str());
      // save image
      fname << get_class_string(label) << "_" << data_cnt << "."<<save_mode;
      //       // scale image to 0 255 if preprocessed
      //       if (strcmp(ppconv_type.c_str(), "RGB")) {
      // 	idx_addc(tmp, (Tdata) 1.0, tmp);
      // 	idx_dotc(tmp, (Tdata) 127.5, tmp);
      //       }
      if (save_image(fname.str(), sample, save_mode.c_str()))
	cout << "  saved " << fname.str() << " (" << sample << ")" << endl;
    }
    // copy label
    if (labels.dim(0) > data_cnt)
      labels.set(label, data_cnt);
    // increment data count
    data_cnt++;
  }
  
  template <class Tdata>
  bool dataset<Tdata>::add_class(const string &class_name) {
    vector<string>::iterator res;
    string name = class_name;
    // if force label is on, only add force label
    if (strcmp(force_label.c_str(), ""))
      name = force_label;
    res = find(classes.begin(), classes.end(), name);
    if (res == classes.end()) {// not found
      classes.push_back(name);
      nclasses++;
    } else { // found
      //t_label i = res - classes.begin();
      //      cout << "found class " << name << " at index " << i << endl;
    }
    return true;
  }

  template <class Tdata>
  void dataset<Tdata>::set_classes(idx<ubyte> &classidx) {    
    // add classes to each dataset
    string s;
    idx_bloop1(classe, classidx, ubyte) {
      s = (const char *) classe.idx_ptr();
      add_class(s);
    }
    // init max_per_class
    max_per_class = idx<intg>(classes.size());
    max_per_class_set = false;
    idx_fill(max_per_class, (numeric_limits<intg>::max)());    
  }

  template <class Tdata>
  void dataset<Tdata>::set_outdims(const idxdim &d) {
    no_outdims = false;
    cout << "Setting target dimensions to " << d << endl;
    outdims = d;
    if (interleaved_input || (outdims.order() == 2)) {
      height = outdims.dim(0);
      width = outdims.dim(1);
    } else {
      height = outdims.dim(1);
      width = outdims.dim(2);
    }
  }

  template <class Tdata>
  void dataset<Tdata>::set_outdir(const string &s) {
    outdir = s;
  }

  template <class Tdata>
  void dataset<Tdata>::set_mindims(const idxdim &d) {
    cout << "Setting minimum input dimensions to " << d << endl;
    mindims = d;
  }

  template <class Tdata>
  void dataset<Tdata>::set_scales(const vector<double> &sc, const string &od) {
    scales = sc;
    scale_mode = true;
    outdir = od;
    cout << "Enabling scaling mode. Scales: ";
    for (vector<double>::iterator i = scales.begin(); i != scales.end(); ++i)
      cout << *i << " ";
    cout << endl;
  }

  template <class Tdata>
  void dataset<Tdata>::set_max_per_class(intg max) {
    if (max < 0)
      eblerror("cannot set max_per_class to < 0");
    if (max > 0) {
      mpc = max;
      max_per_class_set = true;
      max_per_class = idx<intg>(nclasses);
      idx_fill(max_per_class, mpc);
      cout << "Max number of samples per class: " << max << endl;
    }
  }
  
  template <class Tdata>
  void dataset<Tdata>::set_max_data(intg max) {
    if (max < 0)
      eblerror("cannot set max_data to < 0");
    if (max > 0) {
      max_data = max;
      max_data_set = true;
      cout << "Max number of samples: " << max << endl;
    }
  }
  
  template <class Tdata>
  void dataset<Tdata>::set_image_pattern(const string &p) {
    extension = p;
    cout << "Setting image pattern to " << extension << endl;
  }
  
  template <class Tdata>
  void dataset<Tdata>::set_exclude(const vector<string> &ex) {
    if (ex.size()) {
      cout << "Excluded class(es): ";
      for (vector<string>::const_iterator i = ex.begin(); i != ex.end(); ++i) {
	exclude.push_back(*i);
	if (i != ex.begin()) cout << ",";
	cout << " " << *i;
      }
      cout << endl;
    }
  }
  
  template <class Tdata>
  void dataset<Tdata>::set_include(const vector<string> &inc) {
    if (inc.size()) {
      cout << "Included class(es): ";
      for (vector<string>::const_iterator i = inc.begin(); i != inc.end(); ++i){
	include.push_back(*i);
	if (i != inc.begin()) cout << ",";
	cout << " " << *i;
      }
      cout << endl;
    }
  }
  
  template <class Tdata>
  void dataset<Tdata>::set_save(const string &s) {
    save_mode = s;
    cout << "Setting saving mode to: " << save_mode << endl;
  }
    
  template <class Tdata>
  void dataset<Tdata>::set_name(const string &s) {
    name = s;
    build_fname(name, DATA_NAME, data_fname);
    build_fname(name, LABELS_NAME, labels_fname);
    build_fname(name, CLASSES_NAME, classes_fname);
    build_fname(name, CLASSPAIRS_NAME, classpairs_fname);
    build_fname(name, DEFORMPAIRS_NAME, deformpairs_fname);
    cout << "Setting dataset name to: " << name << endl;
  }
    
  template <class Tdata>
  void dataset<Tdata>::set_label(const string &s) {
    force_label = s;
    add_class(force_label);
    cout << "Forcing label for all samples to: " << s << endl;
  }

  template <class Tdata>
  void dataset<Tdata>::set_bboxfact(float factor) {
    bboxhfact = factor;
    bboxwfact = factor;
    cout << "Setting bounding box height and width factor to " << bboxhfact
	 << endl;
  }
    
  template <class Tdata>
  void dataset<Tdata>::set_bboxhfact(float factor) {
    bboxhfact = factor;
    cout << "Setting bounding box height factor to " << bboxhfact << endl;
  }
    
  template <class Tdata>
  void dataset<Tdata>::set_bboxwfact(float factor) {
    bboxwfact = factor;
    cout << "Setting bounding box width factor to " << bboxwfact << endl;
  }
    
  template <class Tdata>
  void dataset<Tdata>::set_bbox_woverh(float factor) {
    bbox_woverh = factor;
    cout << "Forcing width to be h * " << bbox_woverh << endl;
  }
    
  template <class Tdata>
  void dataset<Tdata>::set_nopadded(bool nopadded_) {
    nopadded = nopadded_;
    if (nopadded)
      cout << "Ignoring samples that have padding areas, i.e. too"
	   << " small for target size." << endl;
  }
  
  template <class Tdata>
  void dataset<Tdata>::set_jitter(uint h, uint w, uint ns, float s, uint n) {
    hjitter = h;
    wjitter = w;
    nsjitter = ns;
    sjitter = s;
    njitter = n;
    cout << "Adding " << n << " samples randomly jittered over a " << h
	 << "x" << w << " neighborhood and over " << nsjitter << " scales"
	 << " within a " << sjitter << " scale range around original location"
	 << "/scale" << endl;
  }
  
  template <class Tdata>
  void dataset<Tdata>::set_wmirror() {
    wmirror = true;
    cout << "Adding vertical-axis mirror." << endl;
  }
      
  template <class Tdata>
  void dataset<Tdata>::use_pose() {
    usepose = true;
    cout << "Using pose to separate classes." << endl;
  }
  
  template <class Tdata>
  void dataset<Tdata>::use_parts() {
    useparts = true;
    cout << "Extracting parts." << endl;
  }
  
  template <class Tdata>
  void dataset<Tdata>::use_parts_only() {
    usepartsonly = true;
    cout << "Extracting parts only." << endl;
  }
  
  template <class Tdata>
  bool dataset<Tdata>::count_samples() {
#ifdef __BOOST__
    total_samples = 0;
    regex hidden_dir(".svn");    
    cmatch what;
    directory_iterator end_itr; // default construction yields past-the-end
    path p(inroot);
    if (!exists(p)) {
      cerr << "path " << inroot << " does not exist." << endl;
      return false;
    }
    // loop over all directories
    for (directory_iterator itr(inroot); itr != end_itr; itr++) {
      if (is_directory(itr->status())
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	// ignore excluded classes and use included if defined
	if (included(itr->leaf())) {
	  // add directory as new class
	  add_class(itr->leaf());
	  // recursively search each directory
	  total_samples += count_matches(itr->path().string(), extension);
	}
      }
    }
#endif /* __BOOST__ */
    return true;
  }
  
  template <class Tdata>
  void dataset<Tdata>::split(dataset<Tdata> &ds1, dataset<Tdata> &ds2) {
    // data already preprocessed
    ds1.do_preprocessing = false;
    ds2.do_preprocessing = false;
    ds1.interleaved_input = false;
    ds2.interleaved_input = false;
    cout << "Input data samples: " << data << endl;
    // alloc each dataset
    if (!ds1.allocate(data.dim(0), outdims) ||
	!ds2.allocate(data.dim(0), outdims))
      eblerror("Failed to allocate new datasets");
    // add samples 1st dataset, if not add to 2nd.
    // if 1st has reached max per class, it will return false upon addition
    cout << "Adding data to \"" << ds1.name << "\" and \"" << ds2.name << "\".";
    cout << endl;
    // using the shuffle() method is a problem with big datasets because
    // it requires allocation of an extra dataset.
    // instead, we use a random list of indices to assign the first random
    // samples to dataset 1 and the remaining to dataset 2.
    vector<intg> ids;
    idx<Tdata> sample;
    t_label label;
    for (intg i = 0; i < data.dim(0); ++i) ids.push_back(i);
    random_shuffle(ids.begin(), ids.end());
    for (vector<intg>::iterator i = ids.begin(); i != ids.end(); ++i) {
      sample = data[*i];
      label = labels.get(*i);
      cout << "(original index " << *i << ") ";
      if (ds1.full(label) || 
	  !ds1.add_data(sample, label, 
			classes.size() ? &(classes[(size_t)label]) : NULL))
	ds2.add_data(sample, label, 
		     classes.size() ? &(classes[(size_t)label]):NULL);
    }
    ds1.data_cnt = idx_sum(ds1.add_tally);
    ds2.data_cnt = idx_sum(ds2.add_tally);
    print_stats();
    ds1.print_stats();
    ds2.print_stats();
  }

  template <class Tdata>
  void dataset<Tdata>::merge(const char *name1, const char *name2,
			     const string &inroot) {
    dataset<Tdata> ds1(name1), ds2(name2);
    // load 2 datasets
    ds1.load(inroot);
    ds2.load(inroot);
    interleaved_input = false;
    intg newsz = ds1.size() + ds2.size();
    idxdim d1 = ds1.get_sample_outdim(), d2 = ds2.get_sample_outdim();
    if (!(d1 == d2)) {
      cerr << "sample sizes for dataset 1 and 2 are different: ";
      cerr << d1 << " and " << d2 << endl;
      eblerror("incompatible datasets");
    }
    // allocate new dataset
    allocate(newsz, d1);
    idx<Tdata> datanew;
    idx<t_label> labelsnew;
    // update classes
    idx<ubyte> classidx = ds1.build_classes_idx();
    set_classes(classidx); // initialize with ds1's class names
    cout << "Added all classes to new dataset from " << ds1.name << endl;
    // for each ds2 class name, push on new class names vector if not found
    vector<string>::iterator res, i;
    for (i = ds2.classes.begin(); i != ds2.classes.end(); ++i){
      res = find(classes.begin(), classes.end(), *i);
      if (res == classes.end()) { // not found
	classes.push_back(*i); // add new class name
	nclasses++;
	cout << "Adding class " << *i << " from dataset " << ds2.name << endl;
      }
    }
    // update each ds2 label based on new class numbering
    idx_bloop1(lab, ds2.labels, t_label) {
      string s = ds2.get_class_string(lab.get());
      lab.set(get_class_id(s));
    }
    // copy data 1 into new dataset
    datanew = data.narrow(0, ds1.size(), 0);
    labelsnew = labels.narrow(0, ds1.size(), 0);
    idx_copy(ds1.data, datanew);
    idx_copy(ds1.labels, labelsnew);
    // copy data 2 into new dataset
    datanew = data.narrow(0, ds2.size(), ds1.size());
    labelsnew = labels.narrow(0, ds2.size(), ds1.size());
    idx_copy(ds2.data, datanew);
    idx_copy(ds2.labels, labelsnew);
    // update counter
    data_cnt = newsz;
    cout << "Copied data from " << ds1.name << " and " << ds2.name;
    cout << " into new dataset." << endl;
    // print info
    print_stats();
  }
    
  template <class Tdata> template <class Toriginal>
  bool dataset<Tdata>::save_scales(idx<Toriginal> &dat, const string &filename){
    // copy data into target type
    idxdim d(dat);
    idx<Tdata> sample(d);
    idx_copy(dat, sample);
    // do preprocessing for each scale, then save image
    ostringstream base_name, ofname;
    base_name << outdir << "/" << filename << "_scale";
    string class_name = "noclass";
    for (vector<double>::iterator i = scales.begin(); i != scales.end(); ++i) {
      idx<Tdata> s = preprocess_data(sample, &class_name, false,
				     filename.c_str(), NULL, *i);
      // put sample's channels dimensions first, if defined.
      //s = s.shift_dim(2, 0);
      // save image
      ofname.str(""); ofname << base_name.str() << *i << ".mat";
      if (save_matrix(s, ofname.str())) {
	cout << data_cnt++ << ": saved " << ofname.str();
	cout << "(" << s << ")" << endl;
      }
    }
    return true;
  }
    
  template <class Tdata>
  bool dataset<Tdata>::included(const string &class_name) {
    return // is not excluded
      find(exclude.begin(), exclude.end(), class_name) == exclude.end()
      // and is included
      && ((find(include.begin(), include.end(), class_name) != include.end())
	  // or everything is included
	  || (include.size() == 0));
  }
  
  ////////////////////////////////////////////////////////////////
  // data preprocessing

  template <class Tdata>
  idx<Tdata> dataset<Tdata>::
  preprocess_data(idx<Tdata> &dat, const string *class_name, bool squared,
		  const char *filename, const rect<int> *r, double scale,
		  bool active_sleepd, rect<int> *outr,
		  pair<uint,uint> *center, jitter *jitt) {
    // input region
    rect<int> inr(0, 0, dat.dim(0), dat.dim(1));
    if (r) inr = *r;
    // multiply input region by factor (keeping same center)
    if (bboxhfact != 1.0 || bboxwfact != 1.0)
      inr.scale_centered(bboxhfact, bboxwfact);
    // force width to be h * bbox_woverh
    if (bbox_woverh > 0) {
      int addw = (int) (inr.height * bbox_woverh - inr.width);
      inr.w0 -= addw/2;
      inr.width += addw;
    }
    // resize image to target dims
    rect<int> out_region, cropped;
    idxdim d(outdims);
    idx<Tdata> resized;
    // add jitter
    if (jitt)
      resizepp->set_jitter(jitt->h, jitt->w, jitt->s);
    // default dimensions are same as input
    resizepp->set_dimensions(inr.height, inr.width);
    if (!no_outdims)
      resizepp->set_dimensions(outdims.dim(0), outdims.dim(1));
    if (scale > 0) // resize entire image at specific scale
      resizepp->set_dimensions((uint) (outdims.dim(0) * scale), 
    			       (uint) (outdims.dim(1) * scale));
    //   resizepp->set_input_region(inr);
    // } else if (bboxhfact != 1.0 || bboxwfact != 1.0) // resize if factor
      resizepp->set_input_region(inr);
    idx<Tdata> tmp = dat.shift_dim(2, 0);
    fstate_idx<Tdata> in(tmp.get_idxdim()), out(1,1,1);
    idx_copy(tmp, in.x);
    resizepp->fprop(in, out);
    // remember bbox of original image in resized image
    original_bbox = resizepp->get_original_bbox(); 
    if (outr)
      *outr = original_bbox;
    idx<Tdata> res = out.x.shift_dim(0, 2);
    // display each step
#ifdef __GUI__
    if (display_extraction) {
      disable_window_updates();
      clear_window();
      uint h = 0, w = 0;
      uint dh = 0, dw = 1;
      ostringstream oss;
      // display resized
      oss.str("");
      h = 16;
      gui << gui_only() << black_on_white();
      gui << at(h, w) << "out: " << res;
      h += 16;
      gui << at(h, w) << "pproc: " << ppconv_type;
      h += 16;
//       // draw original output before channel formatting in RGB
//       oss.str("");
//       oss << "RGB";
//       draw_matrix(original, oss.str().c_str(), h, w);
//       h += original.dim(dh) + 5;
      // draw output in RGB
      gui << at(h, w) << ppconv_type; h += 15;
      draw_matrix(res, h, w, 1, 1, minval, maxval);
      // draw crossing arrows at center
      draw_box(h + res.dim(dh)/2, w + res.dim(dw)/2,
	       res.dim(dh)/2, res.dim(dw)/2, 0,0,0);
      h += res.dim(dh) + 5;
      // display all channels
      int i = 0;
      idx_eloop1(chan, res, Tdata) {
	gui << at(h, w) << "chan " << i++; h += 15;
	draw_matrix(chan, h, w, 1, 1, minval, maxval);
	h += chan.dim(dh) + 5;
      }
      w += chan.dim(dw) + 5;
      h = 0;
      // display original
      gui << gui_only() << black_on_white();
      oss.str("");
      if (filename) {
	oss << "file: " << filename;
	gui << gui_only() << black_on_white();
	gui << at(h, w) << oss.str();
      }
      h += 16;
      oss.str("");
      oss << "adding sample #" << data_cnt+1 << " / " << total_samples;
      gui << at(h, w) << oss.str(); h += 16;
      gui << at(h, w) << "input: " << dat; h += 16;
      oss.str("");
      if (class_name)
	oss << *class_name;
      draw_matrix(dat, oss.str().c_str(), h, w);
      // draw object's center
      if (center)
	draw_box(h + center->second - 5, w + center->first - 5, 10, 10,
		 0, 0, 255);
      // draw object's original box
      if (r)
	draw_box(h + r->h0, w + r->w0, r->height, r->width, 255, 0, 0);
      // draw object's factored box if factor != 1.0
      if (bboxhfact != 1.0 || bboxwfact != 1.0)
	draw_box(h + inr.h0, w + inr.w0, inr.height, inr.width, 0, 255, 0);
      h += dat.dim(dh) + 5;
      oss.str("");
      // display object
      idx<Tdata> obj = dat;
      int offh = std::max((int)0, inr.h0);
      int offw = std::max((int)0, inr.w0);
      obj = obj.narrow(dh, std::min((int)obj.dim(0) - offh, inr.height), offh);
      obj = obj.narrow(dw, std::min((int)obj.dim(1) - offw, inr.width), offw);
      // display object
      if (class_name)
	oss << *class_name << " ";
      oss << obj;
      draw_matrix(obj, h, w);
      // draw crossing arrows at center
      draw_box(h + inr.height/2, w + inr.width/2, inr.height/2,
	       inr.width/2, 0,0,0);
      gui << black_on_white() << at(h + inr.height, w) << oss.str();
      // paint
      enable_window_updates();
      if (sleep_display && active_sleepd)
	millisleep((long) sleep_delay);
    }
#endif
    // return preprocessed image
    return res;
  }

  ////////////////////////////////////////////////////////////////
  // Helper functions
    
  template <class Tdata>
  void dataset<Tdata>::compute_stats() {
    // collect stats
    if (nclasses > 0) {
      class_tally = idx<intg>(nclasses);
      idx_clear(class_tally);
      for (intg i = 0; i < data_cnt && i < labels.dim(0); ++i) {
	class_tally.set(class_tally.get(labels.get(i)) + 1,
			(intg) labels.get(i));
      }
    }
  }

  template <class Tdata>
  idx<ubyte> dataset<Tdata>::build_classes_idx() {
    // determine max length of strings
    uint max = 0;
    vector<string>::iterator i = classes.begin();
    for ( ; i != classes.end(); ++i)
      max = (std::max)((size_t) max, i->length());
    // allocate classes idx
    idx<ubyte> classes_idx(classes.size(), max + 1);
    // copy classes strings
    idx_clear(classes_idx);
    idx<ubyte> tmp;
    for (i = classes.begin(); i != classes.end(); ++i) {
      tmp = classes_idx.select(0, i - classes.begin());
      memcpy(tmp.idx_ptr(), i->c_str(), i->length() * sizeof (ubyte));
    }
    return classes_idx;
  }

  template <class Tdata>
  string& dataset<Tdata>::get_class_string(t_label id) {
    if (((uint) id < 0) || ((uint) id >= classes.size()))
      eblerror("trying to access a class with wrong id.");
    return classes[id];
  }

  template <class Tdata>
  t_label dataset<Tdata>::get_class_id(const string &name) {
    vector<string>::iterator res;
    res = find(classes.begin(), classes.end(), name);
    if (res == classes.end()) // not found
      eblerror("class not found");
    return (t_label) (res - classes.begin());
  }      

  // Recursively goes through dir, looking for files matching extension ext.
  template <class Tdata>
  uint dataset<Tdata>::count_matches(const string &dir, const string &pattern) {
    uint total = 0;
#ifdef __BOOST__
    regex eExt(pattern);
    cmatch what;
    path p(dir);
    if (!exists(p))
      return 0;
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (is_directory(itr->status()))
	total += count_matches(itr->path().string(), pattern);
      else if (regex_match(itr->leaf().c_str(), what, eExt))
	total++;
    }
#endif /* __BOOST__ */
    return total;
  }
   
  template <class Tdata>
  void dataset<Tdata>::process_dir(const string &dir, const string &ext,
				   const string &class_name_) {
#ifdef __BOOST__
    string class_name = class_name_;
    // if force label is on, replace label by force_label
    if (strcmp(force_label.c_str(), ""))
      class_name = force_label;
    t_label label = get_label_from_class(class_name);
    cmatch what;
    regex r(ext);
    path p(dir);
    if (!exists(p))
      return ;
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (is_directory(itr->status()))
	process_dir(itr->path().string(), ext, class_name);
      else if (regex_match(itr->leaf().c_str(), what, r)) {
	try {
	  // if full for this class, skip this directory
	  if ((full(get_label_from_class(class_name)) || !included(class_name)))
	    break ;
	  // load data
	  //	  cout << "loading: " << itr->path().string() << endl;
	  load_data(itr->path().string());
	  // add sample data
// 	  if (scale_mode) // saving image at different scales
// 	    save_scales(load_img, itr->leaf());
// 	  else // adding data to dataset
	  this->add_data(load_img, label, &class_name,
			 itr->path().string().c_str());
	} catch(const char *err) {
	  cerr << "error: failed to add " << itr->path().string();
	  cerr << ": " << endl << err << endl;
	  add_errors++;
	} catch(string &err) {
	  cerr << "error: failed to add " << itr->path().string();
	  cerr << ": " << endl << err << endl;
	  add_errors++;
	}
      }}
#endif /* __BOOST__ */
  }

  template <class Tdata>
  void dataset<Tdata>::load_data(const string &fname) {
    load_img = load_image<Tdata>(fname.c_str());
  }  

  template <class Tdata>
  void dataset<Tdata>::compute_random_jitter() {
    // compute all possible jitters
    random_jitter.clear();
    // min/max scale jitter
    float min_sjitt = 1.0 - sjitter / 2;
    float max_sjitt = 1.0 + sjitter / 2;
    // scale step
    float sstep = sjitter / std::max((int) 0, (int) nsjitter - 1);
    for (float sj = min_sjitt; sj <= max_sjitt; sj += sstep) {
      // take into account the fact that scaling already does some spatial
      // jitter, by multiplying min/max spatial jitters by scaling
      float f = min_sjitt + max_sjitt - sj;
      int hhalf = (int) (hjitter * f / 2), whalf = (int) (wjitter * f / 2);
      for (int hj = -hhalf; hj <= hhalf; ++hj)
	for (int wj = -whalf; wj <= whalf; ++wj)
	  random_jitter.push_back(jitter(hj, wj, sj));
    }
    // randomize possibilities
    random_shuffle(random_jitter.begin(), random_jitter.end());
  }

  ////////////////////////////////////////////////////////////////
  // loading errors

  //! required datasets, throw error.
  template <typename T>
  bool loading_error(idx<T> &mat, string &fname) {
    try {
      mat = load_matrix<T>(fname);
    } catch (const string &err) {
      cerr << "error: " << err << endl;
      cerr << "error: failed to load dataset file " << fname << endl;
      eblerror("failed to load dataset file");
      return false;
    }
    cout << "Loaded " << fname << " (" << mat << ")" << endl;
    return true;
  }

  //! optional datasets, issue warning.
  template <typename T>
  bool loading_warning(idx<T> &mat, string &fname) {
    try {
      mat = load_matrix<T>(fname);
    } catch (const string &err) {
      cerr << err << endl;
      cerr << "warning: failed to load dataset file " << fname << endl;
      return false;
    }
    cout << "Loaded " << fname << endl;
    return true;
  }
  
} // end namespace ebl

#endif /* DATASET_HPP_ */