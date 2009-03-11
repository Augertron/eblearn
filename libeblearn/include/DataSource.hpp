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

  template<typename Tdata, typename Tlabel>
  LabeledDataSource<Tdata,Tlabel>::LabeledDataSource()
    : bias(0.0), coeff(0.0), data(1), labels(1), dataIter(data, 0), 
      labelsIter(labels, 0), height(0), width(0), lblstr(NULL) {
  }

  template<typename Tdata, typename Tlabel>
  LabeledDataSource<Tdata,Tlabel>::LabeledDataSource(Idx<Tdata> &data_, 
						     Idx<Tlabel> &labels_,
						     double b, double c,
						     const char *name_,
						     vector<string*> *lblstr_) 
    : bias(b), coeff(c), data(data_), labels(labels_), dataIter(data, 0), 
      labelsIter(labels, 0) {
    init(data_, labels_, b, c, name_, lblstr_);
  }

  template<class Tdata, class Tlabel>
  void LabeledDataSource<Tdata, Tlabel>::init(Idx<Tdata> &data_, 
					      Idx<Tlabel> &labels_,
					      double b, double c,
					      const char *name_,
					      vector<string*> *lblstr_) {
    this->data = data_;
    this->labels = labels_;
    this->height = data.dim(1);
    this->width = data.dim(2);
    this->lblstr = lblstr_;
    this->bias = b;
    this->coeff = c;
    this->name = (name_ ? name_ : "Unknown Dataset");
    typename Idx<Tdata>::dimension_iterator	 dIter(this->data, 0);
    typename Idx<Tlabel>::dimension_iterator	 lIter(this->labels, 0);
    this->dataIter = dIter;
    this->labelsIter = lIter;
    if (!this->lblstr) { // no names are given, use indexes as names
      this->lblstr = new vector<string*>;
      ostringstream o;
      int imax = idx_max(labels_);
      for (int i = 0; i <= imax; ++i) {
	o << i;
	this->lblstr->push_back(new string(o.str()));
	o.str("");
      }
    }
  }

  template<typename Tdata, typename Tlabel>
  LabeledDataSource<Tdata,Tlabel>::~LabeledDataSource() {
    if (lblstr) { // this class owns lblstr and its content
      vector<string*>::iterator i = lblstr->begin();
      for ( ; i != lblstr->end(); ++i)
	if (*i)
	  delete *i;
      delete lblstr;
    }
  }

  template<typename Tdata, typename Tlabel>
  int LabeledDataSource<Tdata,Tlabel>::size() {
    return data.dim(0);
  }

//   template<typename Tdata, typename Tlabel>
//   void LabeledDataSource<Tdata,Tlabel>::fprop(state_idx &state, 
// 					      Idx<Tlabel> &label) {
//     IdxDim d(data.spec);
//     d.setdim(0, 1);
//     state.resize(d);
//     //label.resize();
//     idx_copy(*dataIter, state.x);
//     idx_copy(*labelsIter, label);
//   }

  template<typename Tdata, typename Tlabel>
  void LabeledDataSource<Tdata,Tlabel>::fprop(state_idx &out, 
					      Idx<Tlabel> &label) {
    IdxDim d(data.spec);
    d.setdim(0, 1);
    out.resize(d);
    idx_fill(out.x, bias * coeff);
    Idx<double> tgt = out.x.select(0, 0);
    idx_copy(*(this->dataIter), tgt);
    idx_addc(out.x, bias, out.x);
    idx_dotc(out.x, coeff, out.x);
    idx_copy(*labelsIter, label);
  }

  template<typename Tdata, typename Tlabel>
  void LabeledDataSource<Tdata,Tlabel>::next() {
    ++dataIter;
    ++labelsIter;

    if(!dataIter.notdone()) {
      dataIter = data.dim_begin(0);
      labelsIter = labels.dim_begin(0);
    }
  }

  template<typename Tdata, typename Tlabel>
  void LabeledDataSource<Tdata,Tlabel>::seek_begin() {
    dataIter = data.dim_begin(0);
    labelsIter = labels.dim_begin(0);
  }

  template<typename Tdata, typename Tlabel>
  void LabeledDataSource<Tdata,Tlabel>::display(unsigned int nh, 
						unsigned int nw,
						unsigned int h0,
						unsigned int w0,
						double zoom,
						int wid,
						const char *wname) {
#ifdef __GUI__
    display_wid = (wid >= 0) ? wid : 
      gui.new_window((wname ? wname : name), 
		     nh * (height + 1) - 1, nw * (width + 1) - 1);
    gui.select_window(display_wid);
    draw(nh, nw, h0, w0, zoom);
#endif
  }

  template<typename Tdata, typename Tlabel>
  void LabeledDataSource<Tdata,Tlabel>::draw(unsigned int nh, unsigned int nw,
					     unsigned int h0, unsigned int w0,
					     double zoom) {
#ifdef __GUI__
    gui << gui_only();
    IdxDim d(data.spec);
    state_idx s(d);
    Idx<double> m = s.x.select(0, 0);
    Idx<Tlabel> lbl;
    seek_begin();
    unsigned int h = h0, w = w0;
    for (unsigned int ih = 0; ih < nh; ++ih) {
      for (unsigned int iw = 0; iw < nw; ++iw) {
	fprop(s, lbl);
	next();
	m = s.x.select(0, 0);
	gui.draw_matrix(m, h, w, 0.0, 0.0, zoom, zoom);
	if ((lblstr) && (lblstr->at((int)lbl.get())))
	  gui << at(h + 1, w + 1) << (lblstr->at((int)lbl.get()))->c_str();
	w += m.dim(1) + 1;
      }
      w = w0;
      h += m.dim(0) + 1;
    }
    seek_begin();
#endif
  }

  ////////////////////////////////////////////////////////////////

  template<class Tdata, class Tlabel>
  MnistDataSource<Tdata, Tlabel>::MnistDataSource(Idx<Tdata> &inp, 
						  Idx<Tlabel> &lbl,
						  intg w, intg h, 
						  double b, double c,
						  const char *name_)
    : LabeledDataSource<Tdata, Tlabel>(inp, lbl, b, c, name_, NULL) {
    this->width = w;
    this->height = h;
  }

  template<class Tdata, class Tlabel>
  void MnistDataSource<Tdata, Tlabel>::init(Idx<Tdata> &inp, Idx<Tlabel> &lbl, 
					    intg w, intg h, double b, double c,
    					    const char *name_) {
    LabeledDataSource<Tdata, Tlabel>::init(inp, lbl, b, c, name_, NULL);
    bias = b;
    coeff = c;
    this->width = w;
    this->height = h;
  }

  template<class Tdata, class Tlabel>
  void MnistDataSource<Tdata, Tlabel>::fprop(state_idx &out, 
					     Idx<Tlabel> &label) {
    out.resize(1, this->height, this->width);
    intg ni = this->data.dim(1);
    intg nj = this->data.dim(2);
    intg di = 0.5 * (this->height - ni);
    intg dj = 0.5 * (this->width - nj);
    idx_fill(out.x, bias * coeff);
    Idx<double> tgt = out.x.select(0, 0);
    tgt = tgt.narrow(0, ni, di);
    tgt = tgt.narrow(1, nj, dj);
    idx_copy(*(this->dataIter), tgt);
    idx_addc(out.x, bias, out.x);
    idx_dotc(out.x, coeff, out.x);
    label.set((this->labelsIter).get());
  }

  template<class Tdata, class Tlabel>
  bool load_mnist_dataset(const char *dir, 
			  MnistDataSource<Tdata,Tlabel> &train_ds, 
			  MnistDataSource<Tdata,Tlabel> &test_ds,
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
    Idx<Tdata> train_data(1, 1, 1), test_data(1, 1, 1);
    Idx<Tlabel> train_labels(1), test_labels(1);
    if (!load_matrix<Tdata>(train_data, train_datafile.c_str())) {
      std::cerr << "Mnist dataset, failed to load " << train_datafile << endl;
      ylerror("Failed to load dataset file");
    }
    if (!load_matrix<Tlabel>(train_labels, train_labelfile.c_str())) {
      std::cerr << "Mnist dataset, failed to load " << train_labelfile << endl;
      ylerror("Failed to load dataset file");
    }
    if (!load_matrix<Tdata>(test_data, test_datafile.c_str())) {
      std::cerr << "Mnist dataset, failed to load " << test_datafile << endl;
      ylerror("Failed to load dataset file");
    }
    if (!load_matrix<Tlabel>(test_labels, test_labelfile.c_str())) {
      std::cerr << "Mnist dataset, failed to load " << test_labelfile << endl;
      ylerror("Failed to load dataset file");
    }
    
    // TODO: implement DataSourceNarrow instead of manually narrowing here.
    train_data = train_data.narrow(0, train_size, 0); 
    train_labels = train_labels.narrow(0, train_size, 0);
    test_data = test_data.narrow(0, test_size, 5000 - (0.5 * test_size)); 
    test_labels = test_labels.narrow(0, test_size, 5000 - (0.5 * test_size));

    test_ds.init(test_data, test_labels, 32, 32, 0.0, 0.01, 
		 "MNIST TESTING set");
    train_ds.init(train_data, train_labels, 32, 32, 0.0, 0.01,
		  "MNIST TRAINING set");
    return true;
  }

  template<typename Tdata, typename Tlabel>
  void MnistDataSource<Tdata,Tlabel>::display(unsigned int nh, unsigned int nw,
					      unsigned int h0, unsigned int w0,
					      double zoom, int wid,
					      const char *wname) {
    LabeledDataSource<Tdata, Tlabel>::display(nh, nw, h0, w0, zoom, wid, wname);
  }

  ////////////////////////////////////////////////////////////////
  /*
  // TODO: implement seek
  template<class Tdata, class Tlabel>
  DataSourceNarrow<Tdata, Tlabel>::DataSourceNarrow(LabeledDataSource<Tdata, Tlabel> *b,
  intg siz, intg off) {
  if ((siz + off) > b.size())
  ylerror("illegal range for narrow-db");
  if ((off < 0) || (siz < 0))
  ylerror("offset and size of narrow-db must be positive");
  base = b;
  offset = off;
  size = siz;
  current = 0;
  }

  intg DataSourceNarrow<Tdata, Tlabel>::size() {
  return size;
  }

  void fprop(state_idx &out, Idx<Tlabel> &label) {
  base.seek(offset + current);
  base.fprop(out,label);
  }
  */

} // end namespace ebl

#endif /*DATASOURCE_HPP_*/
