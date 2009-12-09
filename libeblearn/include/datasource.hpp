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

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // datasource

  template<typename Tin1, typename Tin2>
  datasource<Tin1,Tin2>::datasource()
    : bias(0.0), coeff(0.0), data(1), labels(1), dataIter(data, 0), 
      labelsIter(labels, 0), height(0), width(0) {
  }

  template<typename Tin1, typename Tin2>
  datasource<Tin1,Tin2>::datasource(const datasource<Tin1, Tin2> &ds)
    : bias(ds.bias), coeff(ds.coeff), data(ds.data), labels(ds.labels),
      dataIter(data, 0), labelsIter(labels, 0),
      height(ds.height), width(ds.width), name(ds.name) {
  }

  template<typename Tin1, typename Tin2>
  datasource<Tin1,Tin2>::datasource(idx<Tin1> &data_, 
				    idx<Tin2> &labels_,
				    double b, double c,
				    const char *name_)
    : bias(b), coeff(c), data(data_), labels(labels_), dataIter(data, 0), 
      labelsIter(labels, 0), height(data.dim(1)), width(data.dim(2)) {
    init(data_, labels_, b, c, name_);
  }

  template<class Tin1, class Tin2>
  void datasource<Tin1, Tin2>::init(idx<Tin1> &data_, 
				    idx<Tin2> &labels_,
				    double b, double c,
				    const char *name_) {
    this->data = data_;
    this->labels = labels_;
    this->height = data.dim(1);
    this->width = data.dim(2);
    this->bias = b;
    this->coeff = c;
    this->name = (name_ ? name_ : "Unknown Dataset");
    typename idx<Tin1>::dimension_iterator	 dIter(this->data, 0);
    typename idx<Tin2>::dimension_iterator	 lIter(this->labels, 0);
    this->dataIter = dIter;
    this->labelsIter = lIter;
    balance = false;
  }

  template<typename Tin1, typename Tin2>
  datasource<Tin1,Tin2>::~datasource() {
  }

  template<typename Tin1, typename Tin2>
  unsigned int datasource<Tin1,Tin2>::size() {
    return data.dim(0);
  }

  template<typename Tin1, typename Tin2>
  idxdim datasource<Tin1,Tin2>::sample_dims() {
    idxdim d(data.select(0, 0));
    idxdim d2(data);
    d2.setdim(0, 1);
    if (data.order() == 3)
      return d2;
    else 
      return d;
  }

  template<typename Tin1, typename Tin2>
  void datasource<Tin1,Tin2>::shuffle() {
    // create a target idx with the same dimensions
    idx<Tin1> shuffledData(data.get_idxdim());
    idx<Tin2> shuffledLabels(labels.get_idxdim());
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

      iterator += nbOfSamplesPerClass;
      if (iterator >= nbOfSamples)
	iterator = (iterator % nbOfSamples) + 1;
    }
    // replace the original dataset, and labels
    data = shuffledData;
    labels = shuffledLabels;
  }

  template<typename Tin1, typename Tin2>
  void datasource<Tin1,Tin2>::fprop(state_idx &out, 
					      idx<Tin2> &label) {
    out.resize(sample_dims());
    idx_fill(out.x, bias * coeff);
    idx_copy(*(this->dataIter), out.x);
    idx_addc(out.x, bias, out.x);
    idx_dotc(out.x, coeff, out.x);
    idx_copy(*labelsIter, label);
  }

  template<typename Tin1, typename Tin2>
  void datasource<Tin1,Tin2>::next() {
    if (balance) {
      intg i = label_indexes[iitr][indexes_itr[iitr]];
      //      cout << "#" << i << "/" << iitr << " ";
      dataIter = dataIter.at(i);
      labelsIter = labelsIter.at(i);
      indexes_itr[iitr] += 1;
      if (indexes_itr[iitr] >= label_indexes[iitr].size())
	indexes_itr[iitr] = 0;
      iitr++;
      if (iitr >= label_indexes.size())
	iitr = 0;
    } else {
      ++dataIter;
      ++labelsIter;

      if(!dataIter.notdone()) {
	dataIter = data.dim_begin(0);
	labelsIter = labels.dim_begin(0);
      }
    }
  }

  template<typename Tin1, typename Tin2>
  void datasource<Tin1,Tin2>::seek_begin() {
    if (!balance) { // do not reset when balancing
      dataIter = data.dim_begin(0);
      labelsIter = labels.dim_begin(0);
    }
  }

  template<typename Tin1, typename Tin2>
  void datasource<Tin1,Tin2>::set_balanced() {
    balance = true;
    // compute vector of sample indexes for each class
    label_indexes.clear();
    indexes_itr.clear();
    iitr = 0;
    uint nclasses = idx_max(labels) + 1;
    for (uint i = 0; i < nclasses; ++i) {
      vector<intg> indexes;
      label_indexes.push_back(indexes);
      indexes_itr.push_back(0); // init iterators
    }
    // distribute sample indexes into each vector based on label
    for (uint i = 0; i < size(); ++i)
      label_indexes[labels.get(i)].push_back(i);
    // display
//     for (uint i = 0; i < label_indexes.size(); ++i) {
//       vector<intg> &indexes = label_indexes[i];
//       cout << "label " << i << " has " << indexes.size() << " samples: ";
//       for (uint j = 0; j < indexes.size(); ++j)
// 	cout << indexes[j] << " ";
//       cout << endl;
//     }
  }

  ////////////////////////////////////////////////////////////////
  // labeled_datasource

  template<typename Tdata, typename Tlabel>
  labeled_datasource<Tdata,Tlabel>::labeled_datasource()
    : lblstr(NULL) {
  }

  template<typename Tdata, typename Tlabel>
  labeled_datasource<Tdata,Tlabel>::labeled_datasource(idx<Tdata> &data_, 
						       idx<Tlabel> &labels_,
						       double b, double c,
						       const char *name_,
						       vector<string*> *lblstr_) 
  {
    init(data_, labels_, b, c, name_, lblstr_);
  }

  template<typename Tdata, typename Tlabel>
  labeled_datasource<Tdata,Tlabel>::labeled_datasource(idx<Tdata> &data_, 
						       idx<Tlabel> &labels_,
						       idx<ubyte> &classes,
						       double b, double c,
						       const char *name_)
  {
    this->lblstr = new vector<string*>;
    idx_bloop1(classe, classes, ubyte) {
      this->lblstr->push_back(new string((const char*) classe.idx_ptr()));
    }
    init(data_, labels_, b, c, name_, this->lblstr);
  }

  template<typename Tdata, typename Tlabel>
  labeled_datasource<Tdata,Tlabel>::
  labeled_datasource(const char *data_fname, const char *labels_fname,
		     const char *classes_fname, double b, double c,
		     const char *name_) {
    idx<Tdata> dat(1, 1, 1, 1);
    idx<Tlabel> lab(1);
    idx<ubyte> classes(1, 1);
    
    if (!load_matrix<Tdata>(dat, data_fname)) {
      std::cerr << "Failed to load dataset file " << data_fname << endl;
      eblerror("Failed to load dataset file");
    }
    if (!load_matrix<Tlabel>(lab, labels_fname)) {
      std::cerr << "Failed to load dataset file " << labels_fname << endl;
      eblerror("Failed to load dataset file");
    }
    if (!load_matrix<ubyte>(classes, classes_fname)) {
      std::cerr << "Failed to load dataset file " << classes_fname << endl;
      eblerror("Failed to load dataset file");
    }

    datasource<Tdata, Tlabel>::init(dat, lab, b, c, name_);
      
    this->lblstr = new vector<string*>;
    idx_bloop1(classe, classes, ubyte) {
      this->lblstr->push_back(new string((const char*) classe.idx_ptr()));
    }
  }
  
  template<class Tdata, class Tlabel>
  void labeled_datasource<Tdata, Tlabel>::init(idx<Tdata> &inp,
					       idx<Tlabel> &lbl,
					       double b, double c, 
					       const char *name,
					       vector<string*> *lblstr_) {
    datasource<Tdata, Tlabel>::init(inp, lbl, b, c, name);
    this->lblstr = lblstr_;

    if (!this->lblstr) { // no names are given, use indexes as names
      this->lblstr = new vector<string*>;
      ostringstream o;
      int imax = idx_max(this->labels);
      for (int i = 0; i <= imax; ++i) {
	o << i;
	this->lblstr->push_back(new string(o.str()));
	o.str("");
      }
    }
  }

  // copy constructor.
  // deep copy of lblstr.
  template<typename Tdata, typename Tlabel>
  labeled_datasource<Tdata,Tlabel>::
  labeled_datasource(const labeled_datasource<Tdata,Tlabel> &ds)
    : datasource<Tdata, Tlabel>((const datasource<Tdata, Tlabel>&) ds),
      lblstr(NULL) {
    if (ds.lblstr) {
      this->lblstr = new vector<string*>;
      for (unsigned int i = 0; i < ds.lblstr->size(); ++i) {
	this->lblstr->push_back(new string(*ds.lblstr->at(i)));
      }
    }
  }

  // destructor
  template<typename Tdata, typename Tlabel>
  labeled_datasource<Tdata,Tlabel>::~labeled_datasource() {
    if (lblstr) { // this class owns lblstr and its content
      vector<string*>::iterator i = lblstr->begin();
      for ( ; i != lblstr->end(); ++i)
	if (*i)
	  delete *i;
      delete lblstr;
    }
  }

  ////////////////////////////////////////////////////////////////
  // labeled_pair_datasource

  // constructor
  template<typename Tdata, typename Tlabel>
  labeled_pair_datasource<Tdata,Tlabel>::
  labeled_pair_datasource(const char *data_fname,
			  const char *labels_fname,
			  const char *classes_fname,
			  const char *pairs_fname,
			  double b, double c,
			  const char *name_)
    : labeled_datasource<Tdata, Tlabel>(data_fname, labels_fname,
					classes_fname, b, c, name_),
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
  template<typename Tdata, typename Tlabel>
  labeled_pair_datasource<Tdata,Tlabel>::
  labeled_pair_datasource(idx<Tdata> &data_,
			  idx<Tlabel> &labels_,
			  idx<ubyte> &classes_,
			  idx<Tlabel> &pairs_,
			  double b, double c,
			  const char *name_)
    : labeled_datasource<Tdata, Tlabel>(data_, labels_, classes_, b, c, name_),
      pairs(pairs_), pairsIter(pairs, 0) {
  }
  
  // destructor
  template<typename Tdata, typename Tlabel>
  labeled_pair_datasource<Tdata,Tlabel>::~labeled_pair_datasource() {
  }

  // fprop pair
  template<typename Tdata, typename Tlabel>
  void labeled_pair_datasource<Tdata,Tlabel>::fprop(state_idx &in1,
						    state_idx &in2,
						    idx<Tlabel> &label) {
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
  template<typename Tdata, typename Tlabel>
  void labeled_pair_datasource<Tdata,Tlabel>::next() {
    ++pairsIter;
    if(!pairsIter.notdone())
      pairsIter = pairs.dim_begin(0);
  }

  // begin pair
  template<typename Tdata, typename Tlabel>
  void labeled_pair_datasource<Tdata,Tlabel>::seek_begin() {
    pairsIter = pairs.dim_begin(0);
  }
  
  template<typename Tdata, typename Tlabel>
  unsigned int labeled_pair_datasource<Tdata,Tlabel>::size() {
    return pairs.dim(0);
  }

  ////////////////////////////////////////////////////////////////
  // mnist_datasource

  template<class Tdata, class Tlabel>
  mnist_datasource<Tdata, Tlabel>::mnist_datasource(idx<Tdata> &inp, 
						  idx<Tlabel> &lbl,
						  double b, double c,
						  const char *name_)
    : labeled_datasource<Tdata, Tlabel>(inp, lbl, b, c, name_, NULL) {
  }

  template<class Tdata, class Tlabel>
  void mnist_datasource<Tdata, Tlabel>::init(idx<Tdata> &inp, idx<Tlabel> &lbl, 
					    double b, double c,
    					    const char *name_) {
    labeled_datasource<Tdata, Tlabel>::init(inp, lbl, b, c, name_, NULL);
    this->height = 32; // mnist is actually 28x28, but we add some padding
    this->width = 32;
    bias = b;
    coeff = c;
  }

  template<class Tdata, class Tlabel>
  idxdim mnist_datasource<Tdata,Tlabel>::sample_dims() {
    idxdim d(this->data);
    d.setdim(0, 1);
    d.setdim(1, this->height); // use the padding size, not the true data size
    d.setdim(2, this->width);
    return d;
  }

  template<class Tdata, class Tlabel>
  void mnist_datasource<Tdata, Tlabel>::fprop(state_idx &out, 
					     idx<Tlabel> &label) {
    out.resize(this->sample_dims());
    uint ni = this->data.dim(1);
    uint nj = this->data.dim(2);
    uint di = 0.5 * (this->height - ni);
    uint dj = 0.5 * (this->width - nj);
    idx_fill(out.x, bias * coeff);
    idx<double> tgt = out.x.select(0, 0);
    tgt = tgt.narrow(0, ni, di);
    tgt = tgt.narrow(1, nj, dj);
    idx_copy(*(this->dataIter), tgt);
    idx_addc(out.x, bias, out.x);
    idx_dotc(out.x, coeff, out.x);
    label.set((this->labelsIter).get());
  }

  template<class Tdata, class Tlabel>
  bool load_mnist_dataset(const char *dir, 
			  mnist_datasource<Tdata,Tlabel> &train_ds, 
			  mnist_datasource<Tdata,Tlabel> &test_ds,
			  int train_size, int test_size) {
    // loading train and test datasets
    string train_datafile = dir;
    string train_labelfile = dir;
    string test_datafile = dir;
    string test_labelfile = dir;
    train_datafile += "/train-images-idx3-ubyte";
    train_labelfile += "/train-labels-idx1-ubyte";
    test_datafile += "/t10k-images-idx3-ubyte";
    test_labelfile += "/t10k-labels-idx1-ubyte";
    idx<Tdata> train_data(1, 1, 1), test_data(1, 1, 1);
    idx<Tlabel> train_labels(1), test_labels(1);
    if (!load_matrix<Tdata>(train_data, train_datafile.c_str())) {
      std::cerr << "Mnist dataset, failed to load " << train_datafile << endl;
      eblerror("Failed to load dataset file");
    }
    if (!load_matrix<Tlabel>(train_labels, train_labelfile.c_str())) {
      std::cerr << "Mnist dataset, failed to load " << train_labelfile << endl;
      eblerror("Failed to load dataset file");
    }
    if (!load_matrix<Tdata>(test_data, test_datafile.c_str())) {
      std::cerr << "Mnist dataset, failed to load " << test_datafile << endl;
      eblerror("Failed to load dataset file");
    }
    if (!load_matrix<Tlabel>(test_labels, test_labelfile.c_str())) {
      std::cerr << "Mnist dataset, failed to load " << test_labelfile << endl;
      eblerror("Failed to load dataset file");
    }
    
    // TODO: implement DataSourceNarrow instead of manually narrowing here.
    train_data = train_data.narrow(0, train_size, 0); 
    train_labels = train_labels.narrow(0, train_size, 0);
    test_data = test_data.narrow(0, test_size, 5000 - (0.5 * test_size)); 
    test_labels = test_labels.narrow(0, test_size, 5000 - (0.5 * test_size));

    idx<Tdata> test_data2(test_data.dim(0), test_data.dim(1),
			  test_data.dim(2), 3);
    idx<Tdata> train_data2(test_data.dim(0), test_data.dim(1),
			   test_data.dim(2), 3);
    test_ds.init(test_data, test_labels, 0.0, 0.01, "MNIST TESTING set");
    train_ds.init(train_data, train_labels, 0.0, 0.01, "MNIST TRAINING set");
    return true;
  }

  ////////////////////////////////////////////////////////////////
  /*
  // TODO: implement seek
  template<class Tdata, class Tlabel>
  DataSourceNarrow<Tdata, Tlabel>::DataSourceNarrow(labeled_datasource<Tdata, Tlabel> *b,
  intg siz, intg off) {
  if ((siz + off) > b.size())
  eblerror("illegal range for narrow-db");
  if ((off < 0) || (siz < 0))
  eblerror("offset and size of narrow-db must be positive");
  base = b;
  offset = off;
  size = siz;
  current = 0;
  }

  intg DataSourceNarrow<Tdata, Tlabel>::size() {
  return size;
  }

  void fprop(state_idx &out, idx<Tlabel> &label) {
  base.seek(offset + current);
  base.fprop(out,label);
  }
  */

} // end namespace ebl

#endif /*DATASOURCE_HPP_*/
