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
    idxdim dataDim = data.getidxdim(dataDim);
    idx<Tin1> shuffledData(dataDim);
    idxdim labelsDim = labels.getidxdim(labelsDim);
    idx<Tin2> shuffledLabels(labelsDim);
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
    ++dataIter;
    ++labelsIter;

    if(!dataIter.notdone()) {
      dataIter = data.dim_begin(0);
      labelsIter = labels.dim_begin(0);
    }
  }

  template<typename Tin1, typename Tin2>
  void datasource<Tin1,Tin2>::seek_begin() {
    dataIter = data.dim_begin(0);
    labelsIter = labels.dim_begin(0);
  }

  template<typename Tin1, typename Tin2>
  datasource<Tin1,Tin2>* datasource<Tin1,Tin2>::copy() {
    cout << "datasource::copy."<<endl;
    // copy data
    idxdim cdatadim(data);
    idx<Tin1> cdata(cdatadim);
    idx_copy(data, cdata);
    // copy labels
    idxdim clabelsdim(labels);
    idx<Tin2> clabels(clabelsdim);
    idx_copy(labels, clabels);
    return new datasource<Tin1,Tin2>(cdata, clabels, bias, coeff);
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

  template<class Tdata, class Tlabel>
  void labeled_datasource<Tdata, Tlabel>::init(idx<Tdata> &inp,
					       idx<Tlabel> &lbl,
					       double b, double c, 
					       const char *name,
					       vector<string*> *lblstr_) {
    datasource<Tdata, Tlabel>::init(inp, lbl, b, c, name);
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

  template<typename Tdata, typename Tlabel>
  labeled_datasource<Tdata,Tlabel>* labeled_datasource<Tdata,Tlabel>::copy() {
    cout << "labeled_datasource::copy."<<endl;
    // copy data
    idxdim cdatadim(this->data);
    idx<Tdata> cdata(cdatadim);
    idx_copy(this->data, cdata);
    // copy labels
    idxdim clabelsdim(this->labels);
    idx<Tlabel> clabels(clabelsdim);
    idx_copy(this->labels, clabels);
    return new labeled_datasource<Tdata,Tlabel>(cdata, clabels,
						this->bias, this->coeff);
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
    intg ni = this->data.dim(1);
    intg nj = this->data.dim(2);
    intg di = 0.5 * (this->height - ni);
    intg dj = 0.5 * (this->width - nj);
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
