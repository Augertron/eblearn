/***************************************************************************
 *   Copyright (C) 2008 by Matt Grimes and Pierre Sermanet   *
 *   mkg@cs.nyu.edu, pierre.sermanet@gmail.com   *
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

#ifndef DATASOURCE_HPP_
#define DATASOURCE_HPP_

#include <ostream>
#include <algorithm>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // datasource

  template <class Tnet, class Tin1, class Tin2>
  datasource<Tnet, Tin1, Tin2>::datasource()
    : nclasses(0), bias(0), coeff(1.0), data(1), labels(1), probas(1),
      dataIter(data, 0), labelsIter(labels, 0), probasIter(probas, 0),
      height(0), width(0) {
  }

  template <class Tnet, class Tin1, class Tin2>
  datasource<Tnet, Tin1, Tin2>::
  datasource(const datasource<Tnet, Tin1, Tin2> &ds)
    : nclasses(0), bias(ds.bias), coeff(ds.coeff), data(ds.data),
      labels(ds.labels), probas(ds.probas),
      dataIter(data, 0), labelsIter(labels, 0),
      probasIter(probas, 0), height(ds.height), width(ds.width), name(ds.name) {
  }

  template <class Tnet, class Tin1, class Tin2>
  datasource<Tnet, Tin1, Tin2>::
  datasource(idx<Tin1> &data_, idx<Tin2> &labels_, const char *name_,
	     Tin1 b, float c)
    : nclasses(0), bias(b), coeff(c), data(data_), labels(labels_),
      probas(data_.dim(0)),
      dataIter(data, 0), labelsIter(labels, 0), probasIter(probas, 0),
      height(data.dim(1)), width(data.dim(2)) {
    init(data_, labels_, name_, b, c);
  }

  template<class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::
  init(idx<Tin1> &data_, idx<Tin2> &labels_, const char *name_,
       Tin1 b, float c){
    data = data_;
    labels = labels_;
    probas = idx<double>(data.dim(0));
    //    init_drand(time(NULL)); // initialize random seed
    // default probability for a sample of being used is 1
    idx_fill(probas, 1.0); 
    height = data.dim(1);
    width = data.dim(2);
    nclasses = (intg) idx_max(labels) + 1;
    bias = b;
    coeff = c;
    name = (name_ ? name_ : "Unknown Dataset");
    typename idx<Tin1>::dimension_iterator	 dIter(data, 0);
    typename idx<Tin2>::dimension_iterator	 lIter(labels, 0);
    typename idx<double>::dimension_iterator	 pIter(probas, 0);
    dataIter = dIter;
    labelsIter = lIter;
    probasIter = pIter;
    // count number of samples per class
    counts.resize(nclasses);
    fill(counts.begin(), counts.end(), 0);
    idx_bloop1(lab, labels, Tin2) {
      counts[(size_t)lab.get()]++;
    }
    set_balanced(); // balance dataset for each class in next_train
    set_shuffle_passes(true); // for next_train only
    set_weigh_samples(true); // for next_train only
    set_weigh_normalization(true); // per class by default
    test_set = false;
    datasource<Tnet, Tin1, Tin2>::pretty();
  }

  template <class Tnet, class Tin1, class Tin2>
  datasource<Tnet, Tin1, Tin2>::~datasource() {
  }

  template <class Tnet, class Tin1, class Tin2>
  unsigned int datasource<Tnet, Tin1, Tin2>::size() {
    return data.dim(0);
  }

  template <class Tnet, class Tin1, class Tin2>
  idxdim datasource<Tnet, Tin1, Tin2>::sample_dims() {
    idxdim d(data.select(0, 0));
    idxdim d2(data);
    d2.setdim(0, 1);
    if (data.order() == 3)
      return d2;
    else 
      return d;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::shuffle() {
    // create a target idx with the same dimensions
    idx<Tin1> shuffledData(data.get_idxdim());
    idx<Tin2> shuffledLabels(labels.get_idxdim());
    idx<double> shuffledProbas(probas.get_idxdim());
    // get the nb of classes
    intg nbOfClasses = 1+idx_max(labels);
    intg nbOfSamples = data.dim(0);
    intg nbOfSamplesPerClass = nbOfSamples / nbOfClasses;
    // create new dataset...
    intg iterator=0;
    for(int i = 0; i<nbOfSamples; i++){
      idx<Tin1> oneSample = data[iterator];
      idx<Tin1> destSample = shuffledData[i];
      idx_copy(oneSample, destSample);

      idx<Tin2> oneLabel = labels[iterator];
      idx<Tin2> destLabel = shuffledLabels[i];
      idx_copy(oneLabel, destLabel);

      idx<double> oneProba = probas[iterator];
      idx<double> destProba = shuffledProbas[i];
      idx_copy(oneProba, destProba);

      iterator += nbOfSamplesPerClass;
      if (iterator >= nbOfSamples)
	iterator = (iterator % nbOfSamples) + 1;
    }
    // replace the original dataset, and labels
    data = shuffledData;
    labels = shuffledLabels;
    probas = shuffledProbas;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::
  fprop(state_idx<Tnet> &out, idx<Tin2> &label) {
    out.resize(sample_dims());
    idx_copy(*(this->dataIter), out.x);
    idx_addc(out.x, bias, out.x);
    idx_dotc(out.x, coeff, out.x);
    idx_copy(*labelsIter, label);
  }

  template <class Tnet, class Tin1, class Tin2>
  bool datasource<Tnet, Tin1, Tin2>::pick_current() {
    if (test_set) // check that this datasource is allowed to call this method
      eblerror("forbidden call of pick_current() on testing sets");
    if (!weigh_samples) // always pick sample when not using probabilities
      return true;
    // draw random number between 0 and 1 and return true if lower
    // than sample's probability.
    double r = drand(); // [0..1]
    if (r <= probasIter->get())
      return true;
    return false;
  }
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::next() {
    // draw random number between
    ++dataIter;
    ++labelsIter;
    ++probasIter;
    
    if(!dataIter.notdone()) {
      // come back to begining
      dataIter = data.dim_begin(0);
      labelsIter = labels.dim_begin(0);
      probasIter = probas.dim_begin(0);
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::next_train(intg *callcnt) {
    // get pointer to first non empty class
    while (!label_indices[iitr].size()) {
      iitr++; // next class if class is empty
      if (iitr >= label_indices.size())
	iitr = 0;
    }
    // recursion failsafe, allow 1000 max recursions
    if (callcnt && *callcnt > MIN(1000, (intg) label_indices[iitr].size())) {
      // we called recursion on this method more than number of class samples
      // give up and go to next class
      iitr++; // next class
      if (iitr >= label_indices.size())
	iitr = 0; // reseting to first class in class list
      return ;
    }
    intg callcnt2 = 0;
    if (!callcnt)
      callcnt = &callcnt2;
    (*callcnt)++;
    // check that this datasource is allowed to call this method
    if (test_set)
      eblerror("forbidden call of next_train() on testing sets");
    intg i = label_indices[iitr][indices_itr[iitr]];
    //      cout << "#" << i << "/" << iitr << " ";
    dataIter = dataIter.at(i);
    labelsIter = labelsIter.at(i);
    probasIter = probasIter.at(i);
    indices_itr[iitr] += 1;
    if (indices_itr[iitr] >= label_indices[iitr].size()) {
      // returning to begining of list for this class
      indices_itr[iitr] = 0;
      // shuffling list for this class
      if (shuffle_passes) {
	vector<intg> &clist = label_indices[iitr];
	random_shuffle(clist.begin(), clist.end());
      }
      if (weigh_samples) {
	if (perclass_norm) {
	  // normalize probabilities for this class, mapping [0..max] to [0..1]
	  double maxproba = 0;
	  // get max
	  for (vector<intg>::iterator j = label_indices[iitr].begin();
	       j != label_indices[iitr].end(); ++j)
	    maxproba = MAX(probasIter.at(*j)->get(), maxproba);
	  // cout << "normalizing with maxproba for class "
	  //      << iitr << ": " << maxproba << endl;
	  // set normalized proba
	  double avg = 0;
	  for (vector<intg>::iterator j = label_indices[iitr].begin();
	       j != label_indices[iitr].end(); ++j) {
	    probasIter.at(*j)->set((maxproba == 0) ? 1.0 :
				   probasIter.at(*j)->get() / maxproba);
	    avg = probasIter.at(*j)->get();
	  }
	  //	  cout << "avg proba = " << avg / maxproba << endl;
	} else {
	  // normalize probabilities for all classes, mapping [0..max] to [0..1]
	  double maxproba = idx_max(probas);
	  idx_dotc(probas, (maxproba == 0) ? 1.0 : 1 / maxproba, probas);
	}
      }
    }
    // recursively loop until we find a sample that is picked for this class
    if (!pick_current())
      next_train(callcnt);
    else { // if we picked a sample, jump to next class
      iitr++; // next class
      if (iitr >= label_indices.size())
	iitr = 0; // reseting to first class in class list
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_answer_distance(double dist) {
    if (weigh_samples) { // if false, keep default probability 1 for all samples
      probasIter->set(MIN(1.0, fabs(dist)));
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::seek_begin() {
    dataIter = data.dim_begin(0);
    labelsIter = labels.dim_begin(0);
    probasIter = probas.dim_begin(0);
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_balanced() {
    // compute vector of sample indices for each class
    label_indices.clear();
    indices_itr.clear();
    iitr = 0;
    for (intg i = 0; i < nclasses; ++i) {
      vector<intg> indices;
      label_indices.push_back(indices);
      indices_itr.push_back(0); // init iterators
    }
    // distribute sample indices into each vector based on label
    for (uint i = 0; i < size(); ++i)
      label_indices[labels.get(i)].push_back(i);
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_shuffle_passes(bool activate) {
    shuffle_passes = activate;
    cout << "Shuffling of samples (training only) after each pass is "
	 << (shuffle_passes ? "activated" : "deactivated") << "." << endl;
  }
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_weigh_samples(bool activate) {
    weigh_samples = activate;
    cout << "Weighing of samples (training only) based on classification is "
	 << (weigh_samples ? "activated" : "deactivated") << "." << endl;
  }
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_weigh_normalization(bool perclass) {
    perclass_norm = perclass;
    cout << "Weighing normalization (training only) is "
	 << (perclass_norm ? "per class" : "global") << "." << endl;
  }
  
  template <class Tnet, class Tin1, class Tin2>
  intg datasource<Tnet, Tin1, Tin2>::get_nclasses() {
    return nclasses;
  }

  template <class Tnet, class Tin1, class Tin2>
  intg datasource<Tnet, Tin1, Tin2>::get_lowest_common_size() {
    intg min_nonzero = std::numeric_limits<intg>::max();
    for (vector<intg>::iterator i = counts.begin(); i != counts.end(); ++i) {
      if ((*i < min_nonzero) && (*i != 0))
	min_nonzero = *i;
    }
    if (min_nonzero == std::numeric_limits<intg>::max())
      eblerror("empty dataset");
    return min_nonzero * nclasses;
  }
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::pretty() {
    cout << "Dataset \"" << name << "\" contains " << data.dim(0);
    cout << " samples of dimension " << sample_dims();
    cout << " with " << get_nclasses() << " classes," << endl;
    cout << "bias is " << bias << ", coefficient is " << coeff;
    cout << " and iteration size in samples is " << get_lowest_common_size();
    cout << "." << endl;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_test() {
    test_set = true;
    cout << "This is a testing set only." << endl;
  }

  template <class Tnet, class Tin1, class Tin2>
  bool datasource<Tnet, Tin1, Tin2>::is_test() {
    return test_set;
  }
  
  ////////////////////////////////////////////////////////////////
  // labeled_datasource

  template <class Tnet, class Tdata, class Tlabel>
  labeled_datasource<Tnet, Tdata, Tlabel>::labeled_datasource()
    : lblstr(NULL) {
  }

  template <class Tnet, class Tdata, class Tlabel>
  labeled_datasource<Tnet, Tdata, Tlabel>::
  labeled_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_, const char *name_,
		     Tdata b, float c, vector<string*> *lblstr_) {
    init(data_, labels_, name_, b, c, lblstr_);
  }

  template <class Tnet, class Tdata, class Tlabel>
  labeled_datasource<Tnet, Tdata, Tlabel>::
  labeled_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
		     idx<ubyte> &classes, const char *name_, Tdata b, float c) {
    this->lblstr = new vector<string*>;
    idx_bloop1(classe, classes, ubyte) {
      this->lblstr->push_back(new string((const char*) classe.idx_ptr()));
    }
    init(data_, labels_, name_, b, c, this->lblstr);
  }

  template <class Tnet, class Tdata, class Tlabel>
  void labeled_datasource<Tnet, Tdata, Tlabel>::
  init(const char *data_fname, const char *labels_fname,
       const char *classes_fname, const char *name_, Tdata b, float c) {
    idx<Tdata> dat;
    idx<Tlabel> lab;
    idx<ubyte> classes;
    bool classes_found = true;

    try {
      dat = load_matrix<Tdata>(data_fname);
      lab = load_matrix<Tlabel>(labels_fname);
    } catch (string &err) {
      cerr << err << endl;
      eblerror("Failed to load dataset file");
    }
    try {
      classes = load_matrix<ubyte>(classes_fname);
    } catch (string &err) {
      cerr << "warning: " << err << endl;
      classes_found = false;
    }
    // classes names are optional, use numbers by default if not specified
    if (classes_found) {
      this->lblstr = new vector<string*>;
      idx_bloop1(classe, classes, ubyte) {
	this->lblstr->push_back(new string((const char*) classe.idx_ptr()));
      }
    }
    // init
    datasource<Tnet, Tdata, Tlabel>::init(dat, lab, name_, b, c);
    pretty();
  }
  
  template <class Tnet, class Tdata, class Tlabel>
  labeled_datasource<Tnet, Tdata, Tlabel>::
  labeled_datasource(const char *root, const char *dsname, const char *name_,
		     Tdata b, float c) {
    ostringstream data_fname, labels_fname, classes_fname;
    data_fname << root << "/" << dsname << "_" << DATA_NAME << MATRIX_EXTENSION;
    labels_fname << root << "/" << dsname << "_" << LABELS_NAME
		 << MATRIX_EXTENSION;
    classes_fname << root << "/" << dsname << "_" << CLASSES_NAME
		  << MATRIX_EXTENSION;
    init(data_fname.str().c_str(), labels_fname.str().c_str(),
	 classes_fname.str().c_str(), name_, b, c);
  }
  
  template <class Tnet, class Tdata, class Tlabel>
  void labeled_datasource<Tnet, Tdata, Tlabel>::
  init(idx<Tdata> &inp, idx<Tlabel> &lbl, const char *name, Tdata b, float c,
       vector<string*> *lblstr_) {
    datasource<Tnet, Tdata, Tlabel>::init(inp, lbl, name, b, c);
    this->lblstr = lblstr_;

    if (!this->lblstr) { // no names are given, use indices as names
      this->lblstr = new vector<string*>;
      ostringstream o;
      int imax = idx_max(this->labels);
      for (int i = 0; i <= imax; ++i) {
	o << i;
	this->lblstr->push_back(new string(o.str()));
	o.str("");
      }
    }
    datasource<Tnet, Tdata, Tlabel>::pretty();
    pretty();
  }

  // copy constructor.
  // deep copy of lblstr.
  template <class Tnet, class Tdata, class Tlabel>
  labeled_datasource<Tnet, Tdata, Tlabel>::
  labeled_datasource(const labeled_datasource<Tnet, Tdata, Tlabel> &ds)
    : datasource<Tnet, Tdata, Tlabel>
      ((const datasource<Tnet, Tdata, Tlabel>&) ds),
      lblstr(NULL) {
    if (ds.lblstr) {
      this->lblstr = new vector<string*>;
      for (unsigned int i = 0; i < ds.lblstr->size(); ++i) {
	this->lblstr->push_back(new string(*ds.lblstr->at(i)));
      }
    }
  }

  // destructor
  template <class Tnet, class Tdata, class Tlabel>
  labeled_datasource<Tnet, Tdata, Tlabel>::~labeled_datasource() {
    if (lblstr) { // this class owns lblstr and its content
      vector<string*>::iterator i = lblstr->begin();
      for ( ; i != lblstr->end(); ++i)
	if (*i)
	  delete *i;
      delete lblstr;
    }
  }

  template <class Tnet, class Tdata, class Tlabel>
  void labeled_datasource<Tnet, Tdata, Tlabel>::pretty() {
    if (lblstr) {
      cout << "It has: ";
      uint i;
      for (i = 0; i < this->counts.size() - 1; ++i)
	cout << this->counts[i] << " \"" << *(*lblstr)[i] << "\", ";
      cout << "and " << this->counts[i] << " \"" << *(*lblstr)[i] << "\".";
      cout << endl;
    }
  }
    
  ////////////////////////////////////////////////////////////////
  // labeled_pair_datasource

  // constructor
  template <class Tnet, class Tdata, class Tlabel>
  labeled_pair_datasource<Tnet, Tdata, Tlabel>::
  labeled_pair_datasource(const char *data_fname, const char *labels_fname,
			  const char *classes_fname, const char *pairs_fname,
			  const char *name_, Tdata b, float c)
    : labeled_datasource<Tnet, Tdata, Tlabel>(data_fname, labels_fname,
					      classes_fname, name_, b, c),
      pairs(1, 1), pairsIter(pairs, 0) {
    // init current class
    if (!load_matrix<Tlabel>(pairs, pairs_fname)) {
      std::cerr << "Failed to load dataset file " << pairs_fname << endl;
      eblerror("Failed to load dataset file");
    }
    typename idx<Tlabel>::dimension_iterator	 diter(pairs, 0);
    pairsIter = diter;
  }
  
  // constructor
  template <class Tnet, class Tdata, class Tlabel>
  labeled_pair_datasource<Tnet, Tdata, Tlabel>::
  labeled_pair_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
			  idx<ubyte> &classes_, idx<Tlabel> &pairs_,
			  const char *name_, Tdata b, float c)
    : labeled_datasource<Tnet, Tdata, Tlabel>(data_, labels_, classes_, name_,
					      b, c),
      pairs(pairs_), pairsIter(pairs, 0) {
  }
  
  // destructor
  template <class Tnet, class Tdata, class Tlabel>
  labeled_pair_datasource<Tnet, Tdata, Tlabel>::~labeled_pair_datasource() {
  }

  // fprop pair
  template <class Tnet, class Tdata, class Tlabel>
  void labeled_pair_datasource<Tnet, Tdata, Tlabel>::
  fprop(state_idx<Tnet> &in1, state_idx<Tnet> &in2, idx<Tlabel> &label) {
    in1.resize(this->sample_dims());
    in2.resize(this->sample_dims());
    Tlabel id1 = pairsIter.get(0), id2 = pairsIter.get(1);
    Tlabel lab = this->labels.get(id1);
    label.set(lab);
    idx<Tdata> im1 = this->data[id1], im2 = this->data[id2];
    idx_copy(im1, in1.x);
    idx_copy(im2, in2.x);
    idx_addc(in1.x, this->bias, in1.x);
    idx_dotc(in1.x, this->coeff, in1.x);
    idx_addc(in2.x, this->bias, in2.x);
    idx_dotc(in2.x, this->coeff, in2.x);
  }

  // next pair
  template <class Tnet, class Tdata, class Tlabel>
  void labeled_pair_datasource<Tnet, Tdata, Tlabel>::next() {
    ++pairsIter;
    if(!pairsIter.notdone())
      pairsIter = pairs.dim_begin(0);
  }

  // begin pair
  template <class Tnet, class Tdata, class Tlabel>
  void labeled_pair_datasource<Tnet, Tdata, Tlabel>::seek_begin() {
    pairsIter = pairs.dim_begin(0);
  }
  
  template <class Tnet, class Tdata, class Tlabel>
  unsigned int labeled_pair_datasource<Tnet, Tdata, Tlabel>::size() {
    return pairs.dim(0);
  }

  ////////////////////////////////////////////////////////////////
  // mnist_datasource

  template <class Tnet, class Tdata, class Tlabel>
  mnist_datasource<Tnet, Tdata, Tlabel>::
  mnist_datasource(const char *root, const char *type, uint size) {
    try {
      // load dataset
      ostringstream datafile, labelfile, name;
      name << "MNIST " << type;
      datafile << root << "/" << type << "-images-idx3-ubyte";
      labelfile << root << "/" << type << "-labels-idx1-ubyte";
      idx<Tdata> dat = load_matrix<Tdata>(datafile.str());
      idx<Tlabel> labs = load_matrix<Tlabel>(labelfile.str());
      dat = dat.narrow(0, size, 
		       strcmp("t10k", type) ? 0 : (intg) (5000 - .5 * size)); 
      labs = labs.narrow(0, size, 
			 strcmp("t10k", type) ? 0 : (intg) (5000 - .5 * size)); 
      init(dat, labs, name.str().c_str(), 0, 0.01);
    } catch(string &err) {
      cerr << err << endl;
      eblerror("failed to load mnist dataset");
    }
  }

  template <class Tnet, class Tdata, class Tlabel>
  void mnist_datasource<Tnet, Tdata, Tlabel>::
  init(idx<Tdata> &inp, idx<Tlabel> &lbl, const char *name_, Tdata b, float c) {
    labeled_datasource<Tnet, Tdata, Tlabel>::init(inp, lbl, name_, b, c, NULL);
    height = 32; // mnist is actually 28x28, but we add some padding
    width = 32;
  }

  template <class Tnet, class Tdata, class Tlabel>
  idxdim mnist_datasource<Tnet, Tdata, Tlabel>::sample_dims() {
    idxdim d(data);
    d.setdim(0, 1);
    d.setdim(1, height); // use the padding size, not the true data size
    d.setdim(2, width);
    return d;
  }

  template <class Tnet, class Tdata, class Tlabel>
  void mnist_datasource<Tnet, Tdata, Tlabel>::
  fprop(state_idx<Tnet> &out, idx<Tlabel> &label) {
    out.resize(this->sample_dims());
    uint ni = data.dim(1);
    uint nj = data.dim(2);
    uint di = (uint) (0.5 * (height - ni));
    uint dj = (uint) (0.5 * (width - nj));
    out.clear_x();
    idx<Tnet> tgt = out.x.select(0, 0);
    tgt = tgt.narrow(0, ni, di);
    tgt = tgt.narrow(1, nj, dj);
    idx_copy(*(this->dataIter), tgt);
    idx_addc(out.x, bias, out.x);
    idx_dotc(out.x, coeff, out.x);
    label.set((this->labelsIter).get());
  }

  ////////////////////////////////////////////////////////////////

  template <class Tdata>
  idx<Tdata> create_target_matrix(intg nclasses, Tdata target) {
    // fill matrix with 1-of-n code
    idx<Tdata> targets(nclasses, nclasses);
    idx_fill(targets, -target);
    for (int i = 0; i < nclasses; ++i) { 
      targets.set(target, i, i);
    }
    return targets; // return by copy
  }

} // end namespace ebl

#endif /*DATASOURCE_HPP_*/
