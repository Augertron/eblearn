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
    name = name_;
    if (inroot_) {
      inroot = inroot_;
      inroot += "/";
    } else inroot = "";
    outdims = idxdim(96, 96, 3);
    build_fname(name, DATA_NAME, data_fname);
    build_fname(name, LABELS_NAME, labels_fname);
    build_fname(name, CLASSES_NAME, classes_fname);
    build_fname(name, CLASSPAIRS_NAME, classpairs_fname);
    build_fname(name, DEFORMPAIRS_NAME, deformpairs_fname);
    max_data = 0;
    max_data_set = false;
    total_samples = 0;
    display_extraction = false;
    display_result = false;
    ppconv_type = "";
    ppconv_set = false;
    extension = IMAGE_PATTERN;
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
    mpc = numeric_limits<intg>::max();
    init_drand(time(NULL)); // initialize random seed
#ifndef __BOOST__
    eblerror("Boost libraries not available, install libboost-filesystem-dev libboost-regex-dev and recompile");
#endif
  }

  template <class Tdata>
  dataset<Tdata>::~dataset() {
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
    cout << "Found: "; print_classes(); cout << "." << endl;
    // (re)init max per class, knowing number of classes
    intg m = numeric_limits<intg>::max();
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
#ifdef __BOOST__
    cmatch what;
    regex hidden_dir(".svn");    
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(inroot); itr != end_itr; itr++) {
      if (is_directory(itr->status())
	  && !regex_match(itr->leaf().c_str(), what, hidden_dir)) {
	// process subdirs to extract images into the single image idx
	process_dir(itr->path().string(), extension, itr->leaf());
      }}
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

    idx<ubyte> classidx = build_classes_idx();
    ds1.set_classes(classidx);
    ds2.set_classes(classidx);
    // set max samples per class for dataset 1 (ds2 takes whatever is left)
    ds1.set_max_per_class(max);
    // split
    split(ds1, ds2);
  }

  template <class Tdata>
  void dataset<Tdata>::shuffle() {
    cout << "Shuffling dataset \"" << name << "\"." << endl;
    init_drand(time(NULL)); // initialize random seed
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
    if (label == -42) // excluded class
      return true;
    if ((max_data_set && (data_cnt >= max_data)) ||
	((data.order() > 0) && (data_cnt >= data.dim(0))))
      return true;
    if (max_per_class_set && (label >= 0) &&
	(add_tally.get(label) >= max_per_class.get(label)))
      return true;
    return false;
  }

  ////////////////////////////////////////////////////////////////
  // print methods
  
  template <class Tdata>
  void dataset<Tdata>::print_classes() {
    cout << classes.size() << " classe";
    if (classes.size() > 1) cout << "s";
    cout << " (";
    uint i;
    for (i = 0; i < classes.size() - 1; ++i)
      cout << classes[i] << ", ";
    cout << classes[i] << ")";
  }

  template <class Tdata>
  void dataset<Tdata>::print_stats() {
    compute_stats();
    // print stats
    cout << "Dataset \"" << name << "\" contains " << data_cnt;
    cout << " samples (of dimensions " << outdims << " and ";
    cout << typeid(Tdata).name() << " precision)";
    cout << ", distributed in " << classes.size() << " classes: ";
    uint i;
    for (i = 0; i < classes.size() - 1; ++i) {
      cout << class_tally.get(i) << " " << classes[i];
      class_tally.get(i) > 1 ? cout << "s, " : cout << ", ";
    }
    cout << class_tally.get(i) << " " << classes[i];
    class_tally.get(i) > 1 ? cout << "s." : cout << ".";
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
    cout << "_*" << MATRIX_EXTENSION << endl;
    // load data
    data = idx<Tdata>(1,1,1,1); // TODO: implement generic load_matrix
    fname = root1; fname += data_fname;
    loading_error(load_matrix(data, fname), fname);
    // load labels
    labels = idx<t_label>(1); // TODO: implement generic load_matrix
    fname = root1; fname += labels_fname;
    loading_error(load_matrix(labels, fname), fname);
    // load classes
    idx<ubyte> classidx;
    classidx = idx<ubyte>(1,1); // TODO: implement generic load_matrix
    fname = root1; fname += classes_fname;
    loading_warning(load_matrix(classidx, fname), fname);
    set_classes(classidx);
    // load classpairs
    classpairs = idx<t_label>(1,1); // TODO: implement generic load_matrix
    fname = root1; fname += classpairs_fname;
    loading_warning(load_matrix(classpairs, fname), fname);
    // load deformation pairs
    deformpairs = idx<t_label>(1,1); // TODO: implement generic load_matrix
    fname = root1; fname += deformpairs_fname;
    loading_warning(load_matrix(deformpairs, fname), fname);
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
    string root1 = root;
    root1 += "/";
    cout << "Saving dataset " << name << " in " << root << "/";
    cout << name << "_*" << MATRIX_EXTENSION << endl;
    // return false if no samples
    if (data_cnt <= 0) {
      cerr << "Warning: No samples were added to the dataset, nothing to save.";
      cerr << endl;
      return false;
    }
    // creating directory
    mkdir(root1.c_str(), MKDIR_RIGHTS);
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
    fname = root1;
    fname += classes_fname;
    idx<ubyte> classes_idx = build_classes_idx();
    cout << "Saving " << fname << " (" << classes_idx << ")"  << endl;
    if (!save_matrix(classes_idx, fname)) {
      cerr << "error: failed to save classes into " << fname << endl;
      return false;
    } else cout << "Saved " << fname << endl;
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
      n = MAX(0, MIN(n, idx_sum(max_per_class)));
    cout << "Allocating dataset \"" << name << "\" with " << n;
    cout << " samples of size " << d << " ..." << endl;
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
    add_tally = idx<intg>(classes.size());
    idx_clear(add_tally);
    return true;
  }

  ////////////////////////////////////////////////////////////////
  // data
    
  template <class Tdata> template <class Toriginal>
  bool dataset<Tdata>::add_data(idx<Toriginal> &dat, const string &class_name,
				const char *filename, const rect *r) { 
    // check for errors
    if (!allocated) {
      cerr << "error: dataset has not been allocated, cannot add data." << endl;
      return false;
    }
    // check that class exists (may not exist if excluded)
    if (find(classes.begin(), classes.end(), class_name) == classes.end())
      return false;
    // compute label
    t_label label = get_label_from_class(class_name);
    // check for capacity
    if (full(label)) // reached full capacity
      return false;
    // increase counter for that class
    add_tally.set(add_tally.get(label) + 1, label);
    // print info
    cout << data_cnt+1 << ": add ";
    cout << (filename ? filename : "sample" ) << " as " << class_name;
    cout << " (" << label << ")" << endl;
    // copy data into target type
    idxdim d(dat);
    idx<Tdata> sample(d);
    idx_copy(dat, sample);
    // do preprocessing
    if (do_preprocessing)
      sample = preprocess_data(sample, class_name, true, filename, r);
    // check for dimensions
    if (!sample.same_dim(outdims)) {
      cerr << "error: expected data with dimensions " << outdims;
      cerr << " but found " << sample.get_idxdim() << endl;
      return false;
    }
    // put sample's channels dimensions first, if interleaved.
    if (interleaved_input)
      sample = sample.shift_dim(2, 0);
    // copy sample
    idx<Tdata> tgt = data.select(0, data_cnt);
    idx_copy(sample, tgt);
    // copy label
    labels.set(label, data_cnt);
    // increment data count
    data_cnt++;
    return true;
  }
    
  template <class Tdata>
  bool dataset<Tdata>::add_class(const string &class_name) {
    vector<string>::iterator res;
    res = find(classes.begin(), classes.end(), class_name);
    if (res == classes.end()) // not found
      classes.push_back(class_name);
    else { // found
      //t_label i = res - classes.begin();
      //      cout << "found class " << class_name << " at index " << i << endl;
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
    idx_fill(max_per_class, numeric_limits<intg>::max());    
  }

  template <class Tdata>
  void dataset<Tdata>::set_outdims(const idxdim &d) {
    cout << "Setting target dimensions to " << d << endl;
    outdims = d;
  }

  template <class Tdata>
  void dataset<Tdata>::set_scales(const vector<uint> &sc, const string &od) {
    scales = sc;
    scale_mode = true;
    outdir = od;
    cout << "Enabling scaling mode. Scales: ";
    for (vector<uint>::iterator i = scales.begin(); i != scales.end(); ++i)
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
      max_per_class = idx<intg>(classes.size());
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
	// ignore excluded classes
	if (find(exclude.begin(), exclude.end(), itr->leaf()) == exclude.end()){
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
    // alloc each dataset
    ds1.allocate(data.dim(0), outdims);
    ds2.allocate(data.dim(0), outdims);
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
    for (intg i = 0; i < data.dim(0); ++i) ids.push_back(i);
    random_shuffle(ids.begin(), ids.end());
    for (vector<intg>::iterator i = ids.begin(); i != ids.end(); ++i) {
      sample = data[*i];
      cout << "(original index " << *i << ") ";
      if (!ds1.add_data(sample, classes[ (size_t)labels.get(*i) ]))
	ds2.add_data(sample, classes[ (size_t)labels.get(*i) ]);
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
    for (vector<uint>::iterator i = scales.begin(); i != scales.end(); ++i) {
      idx<Tdata> s = preprocess_data(sample, class_name, false,
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
    
  ////////////////////////////////////////////////////////////////
  // data preprocessing

  template <class Tdata>
  idx<Tdata> dataset<Tdata>::
  preprocess_data(idx<Tdata> &dat, const string &class_name, bool squared,
		  const char *filename, const rect *r, uint scale,
		  bool active_sleepd) {
    uint dh = 0, dw = 1;
    // resize image to target dims
    rect out_region, cropped;
    idxdim d(outdims);
    idx<Tdata> resized;
    if (scale > 0) {
      if (!strcmp(resize_mode.c_str(), "bilinear"))
	resized = image_resize(dat, (double)1/scale, (double)1/scale, 2);
      else if (!strcmp(resize_mode.c_str(), "gaussian"))
	resized = gaussian_resize_image_to(dat, scale);
    } else
      resized = resize_image_to(dat, d, out_region, r);
    rect out_entire(0, 0, resized.dim(dh), resized.dim(dw));
    // convert image to target format
    idx<Tdata> original;
    if (display_extraction) {
      original = idx<Tdata>(resized.get_idxdim());
      idx_copy(resized, original);
      if (squared)
	original = image_region_to_rect(original, out_region, outdims.dim(0),
					outdims.dim(1), cropped);
    }
    idx<Tdata> formatted = resized;
    if (ppconv_set)
      formatted = convert_image_to(formatted, ppconv_type, out_entire);
    idx<Tdata> res = formatted;
    if (squared)
      res = image_region_to_rect(res, out_region, outdims.dim(0),
				 outdims.dim(1), cropped);
    // display each step
#ifdef __GUI__
    if (display_extraction) {
      disable_window_updates();
      clear_window();
      uint h = 0, w = 0;
      ostringstream oss;
      // display resized
      oss.str("");
      h = 16;
      gui << gui_only() << black_on_white();
      gui << at(h, w) << "out: " << res;
      h += 16;
      gui << at(h, w) << "pproc: " << ppconv_type;
      h += 16;
      // draw original output before channel formatting in RGB
      oss.str("");
      oss << "RGB";
      draw_matrix(original, oss.str().c_str(), h, w);
      h += original.dim(dh) + 5;
      // draw output in RGB
      oss.str("");
      oss << ppconv_type;
      draw_matrix(res, oss.str().c_str(), h, w);
      // draw crossing arrows at center
      draw_box(h + res.dim(dh)/2, w + res.dim(dw)/2,
	       res.dim(dh)/2, res.dim(dw)/2, 0,0,0);
      h += res.dim(dh) + 5;
      // display all channels
      int i = 0;
      idx_eloop1(chan, res, Tdata) {
	oss.str("");
	oss << "chan " << i++ ;
	draw_matrix(chan, oss.str().c_str(), h, w, 1, 1, minval, maxval);
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
      oss << class_name;
      draw_matrix(dat, oss.str().c_str(), h, w);
      h += dat.dim(dh) + 5;
      oss.str("");
      // display object
      if (r) {
	idx<Tdata> obj = dat;
	obj = obj.narrow(dh, r->height, r->h0);
	obj = obj.narrow(dw, r->width, r->w0);
	// display object
	oss << class_name << " " << obj;
	draw_matrix(obj, h, w);
	// draw crossing arrows at center
	draw_box(h + r->height/2, w + r->width/2, r->height/2,
		 r->width/2, 0,0,0);
	gui << black_on_white() << at(h + r->height, w) << oss.str();
      }
      // paint
      enable_window_updates();
      if (sleep_display && active_sleepd)
	sleep(sleep_delay / 1000.0);
    }
#endif
    // return preprocessed image
    return res;
  }

  template <class Tdata>
  idx<Tdata> dataset<Tdata>::
  convert_image_to(idx<Tdata> &img, const string &conv_type,
		   const rect &cropped) {
    uint dh = 0, dw = 1;
    // cropped_img is the part of img that contains input
    idx<Tdata> cropped_img = img.narrow(dh, cropped.height, cropped.h0);
    cropped_img = cropped_img.narrow(dw, cropped.width, cropped.w0);
    // switch based on format
    // YUV
    if (!strcmp(conv_type.c_str(), "YUV"))
      return rgb_to_yuv(img);
    else if (!strcmp(conv_type.c_str(), "HSV"))
      return img;
    // YpUV (Y is preprocessed with local and global normalization)
    else if (!strcmp(conv_type.c_str(), "YpUV")) {
      // convert img to YUV
      idx<Tdata> yuv = rgb_to_yuv(cropped_img);
      idx<Tdata> uv = yuv.narrow(2, 2, 1);
      idx_addc(uv, (Tdata)-128, uv);
      idx_dotc(uv, (Tdata).01, uv);
      // convert Y to Yp
      idx<Tdata> yp = yuv.select(2, 0);
      idxdim d(yp);
      idx<Tdata> tmp(d);
      idx_copy(yp, tmp);
      image_global_normalization(tmp);
      image_local_normalization(tmp, yp, ppkernel_size);
      // copy cropped yuv into normal yuv image
      idxdim dimg(img);
      idx<Tdata> res(dimg);
      idx_clear(res);
      tmp = res.narrow(0, cropped.height, cropped.h0);
      tmp = tmp.narrow(1, cropped.width, cropped.w0);
      idx_copy(yuv, tmp);
      // set min/max val for display
      minval = -1;
      maxval = 1;
      return res;
    }
    // RGB
    else if (!strcmp(conv_type.c_str(), "RGB")) {
      minval = 0;
      maxval = 255;
      return img;
    }
    cerr << "error: trying to convert image to unknown preprocessing";
    cerr << " conversion format: " << conv_type << endl;
    eblerror("unknown preprocessing conversion format");
  }

  template <class Tdata>
  idx<Tdata> dataset<Tdata>::resize_image_to(idx<Tdata> &img, const idxdim &d,
					     rect &cropped, const rect *r) {
    // do nothing if dims are already the target dims.
    if (img.same_dim(d)) {
      cropped = rect(0, 0, img.dim(0), img.dim(1));
      return img;
    }
    idx<Tdata> res;
    // if r is not specified, take entire image
    rect rr(0, 0, img.dim(0), img.dim(1));
    if (r) rr = *r;
    // bilinear interpolation resizing
    if (!strcmp(resize_mode.c_str(), "bilinear")) {
      res = image_resize(img, outdims.dim(0), outdims.dim(1), 0, &rr, &cropped);
      //      cropped = rect(0, 0, outdims.dim(0), outdims.dim(1));
      return res;
    }
    // gaussian pyramid resizing
    else if (!strcmp(resize_mode.c_str(), "gaussian")) {
      res = image_gaussian_resize(img, outdims.dim(0), outdims.dim(1), 0.0,
				  &rr, &cropped);
      return res;
    } else {
      cerr << "unknown resize mode: " << resize_mode << endl;
      eblerror("unknown resize mode");
    }
    return res;
  }

  template <class Tdata>
  idx<Tdata> dataset<Tdata>::gaussian_resize_image_to(idx<Tdata> &img,
						      uint scale) {
    gaussian_pyramid<Tdata> gp;
    idx<Tdata> res = img.shift_dim(2, 0);
    uint reductions = (uint) (scale / (float)2.0);
    if (reductions > 0)
      res = gp.reduce(res, reductions);
    //    idx<Tdata> res = image_gaussian_square_resize(img, rr, d.dim(0),
    //					  cropped, .3);
    return res.shift_dim(0, 2);
  }  

  ////////////////////////////////////////////////////////////////
  // Helper functions
    
  template <class Tdata>
  void dataset<Tdata>::compute_stats() {
    // collect stats
    class_tally = idx<intg>(classes.size());
    idx_clear(class_tally);
    for (intg i = 0; i < data_cnt; ++i) {
      class_tally.set(class_tally.get(labels.get(i)) + 1,
		      (intg) labels.get(i));
    }
  }

  template <class Tdata>
  idx<ubyte> dataset<Tdata>::build_classes_idx() {
    // determine max length of strings
    uint max = 0;
    vector<string>::iterator i = classes.begin();
    for ( ; i != classes.end(); ++i)
      max = MAX(max, i->length());
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
				   const string &class_name) {
#ifdef __BOOST__
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
	  if (full(get_label_from_class(class_name)) && !scale_mode)
	    break ;
	  // load data
	  load_data(itr->path().string());
	  // add sample data
	  if (scale_mode) // saving image at different scales
	    save_scales(load_img, itr->leaf());
	  else // adding data to dataset
	    add_data(load_img, class_name, itr->path().string().c_str());
	} catch(const char *err) {
	  cerr << "error: failed to add " << itr->path().string();
	  cerr << ": "<< err << endl;
	}
      }}
#endif /* __BOOST__ */
  }

  template <class Tdata>
  void dataset<Tdata>::load_data(const string &fname) {
    idx<ubyte> tmp = load_image<ubyte>(fname.c_str());
    idxdim d(tmp);
    load_img = idx<Tdata>(d);
    idx_copy(tmp, load_img);
  }
  
  
} // end namespace ebl

#endif /* DATASET_HPP_ */
