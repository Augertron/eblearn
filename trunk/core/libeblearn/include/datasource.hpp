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
    : bias(0), coeff(1.0), data(1), labels(1), probas(1),
      energies(1), correct(1), answers(1), pick_count(1), count_pickings(true),
      height(0), width(0), nclasses(0), it(0), it_test(0), it_train(0),
      indices(1), balance(true), state_saved(false),
      sample_min_proba(0.0), test_set(false), epoch_sz(0), epoch_cnt(0),
      epoch_pick_cnt(0),
      epoch_mode(1), hardest_focus(false), ignore_correct_(false) {
  }

  template <class Tnet, class Tin1, class Tin2>
  datasource<Tnet, Tin1, Tin2>::
  datasource(const datasource<Tnet, Tin1, Tin2> &ds)
    : bias(ds.bias), coeff(ds.coeff), data(ds.data),
      labels(ds.labels), probas(ds.probas), energies(ds.energies),
      correct(ds.correct), answers(ds.answers), pick_count(ds.pick_count),
      count_pickings(ds.count_pickings),
      height(ds.height), width(ds.width), _name(ds._name), nclasses(0), it(0),
      it_test(0), it_train(0), indices(1), balance(true),
      state_saved(false), sample_min_proba(0.0), test_set(false),
      epoch_sz(0), epoch_cnt(0), epoch_pick_cnt(0),
      epoch_mode(1), hardest_focus(false), ignore_correct_(false) {
  }

  template <class Tnet, class Tin1, class Tin2>
  datasource<Tnet, Tin1, Tin2>::
  datasource(idx<Tin1> &data_, idx<Tin2> &labels_, const char *name_,
	     Tin1 b, float c)
    : bias(b), coeff(c), data(data_), labels(labels_),
      probas(data_.dim(0)), energies(data_.dim(0)),
      correct(data_.dim(0)), answers(data_.dim(0)), pick_count(data_.dim(0)),
      count_pickings(true), 
      height(data.dim(1)), width(data.dim(2)), nclasses(0), it(0), it_test(0),
      it_train(0), indices(1), balance(true), state_saved(false),
      sample_min_proba(0.0), test_set(false), epoch_sz(0), epoch_cnt(0),
      epoch_pick_cnt(0), epoch_mode(1), hardest_focus(false),
      ignore_correct_(false) {
    init(data_, labels_, name_, b, c);
  }

  template<class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::
  init(idx<Tin1> &data_, idx<Tin2> &labels_, const char *name_,
       Tin1 b, float c) {
    idx<double> *cont1 = dynamic_cast<idx<double>*>(&labels_);
    idx<float> *cont2 = dynamic_cast<idx<float>*>(&labels_);
    if (cont1 || cont2)
      discrete_labels = false; // continous labels
    else // discrete, we can use labels as classes
      discrete_labels = true; 
    data = data_;
    labels = labels_;
    indices = idx<intg>(data.dim(0));
    indices_saved = idx<intg>(data.dim(0));
    probas = idx<double>(data.dim(0));
    energies = idx<double>(data.dim(0));
    correct = idx<ubyte>(data.dim(0));
    answers = idx<Tin2>(data.dim(0));
    pick_count = idx<uint>(data.dim(0));
    idx_clear(pick_count);
    dynamic_init_drand(); // initialize random seed
    // default probability for a sample of being used is 1
    idx_fill(probas, 1.0);
    idx_fill(energies, -1.0);
    idx_fill(correct, 0);
    idx_fill(answers, 0);
    height = data.dim(1);
    width = data.dim(2);
    nclasses = (intg) idx_max(labels) + 1;
    bias = b;
    coeff = c;
    _name = (name_ ? name_ : "Unknown Dataset");
    // count number of samples per class if discrete
    if (discrete_labels) {
      counts.resize(nclasses);
      fill(counts.begin(), counts.end(), 0);
      idx_bloop1(lab, labels, Tin2) {
	counts[(size_t)lab.get()]++;
      }
      set_balanced(true); // balance dataset for each class in next_train
    }
    set_shuffle_passes(true); // for next_train only
    set_weigh_samples(true, true, true, 0.0); // for next_train only
    datasource<Tnet, Tin1, Tin2>::pretty();
    seek_begin();
    seek_begin_train();
    epoch_sz = size(); //get_lowest_common_size();
    epoch_mode = 1;
    cout << _name << ": Each training epoch sees " << epoch_sz
	 << " samples." << endl;
    init_epoch();
    not_picked = 0;
    epoch_show = 50; // print epoch count message every epoch_show
    epoch_show_printed = -1; // last epoch count we have printed
    // fill indices with original data order
    for (it = 0; it < data.dim(0); ++it)
      indices.set(it, it);
    // initialize index to 0
    it = 0;
    // shuffle data indices
    shuffle();
  }

  template <class Tnet, class Tin1, class Tin2>
  datasource<Tnet, Tin1, Tin2>::~datasource() {
  }

  //////////////////////////////////////////////////////////////////////////////
  // info access methods
  
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
  string& datasource<Tnet, Tin1, Tin2>::name() {
    return _name;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_test() {
    test_set = true;
    cout << _name << ": This is a testing set only." << endl;
  }

  template <class Tnet, class Tin1, class Tin2>
  bool datasource<Tnet, Tin1, Tin2>::is_test() {
    return test_set;
  }
  
  template <class Tnet, class Tin1, class Tin2>
  intg datasource<Tnet, Tin1, Tin2>::get_nclasses() {
    return nclasses;
  }

  template <class Tnet, class Tin1, class Tin2>
  intg datasource<Tnet, Tin1, Tin2>::get_epoch_size() {
    return epoch_sz;
  }

  template <class Tnet, class Tin1, class Tin2>
  intg datasource<Tnet, Tin1, Tin2>::get_epoch_count() {
    return epoch_cnt;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_epoch_size(intg sz) {
    cout << _name << ": Setting epoch size to " << sz << endl;
    epoch_sz = sz;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_epoch_mode(uint mode) {
    epoch_mode = mode;
    cout << _name << ": Setting epoch mode to " << epoch_mode;
    switch (epoch_mode) {
    case 0: cout << " (fixed number of samples)" << endl; break ;
    case 1: cout << " (see all samples at least once)" << endl; break ;
    default: eblerror("unknown mode");
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  intg datasource<Tnet, Tin1, Tin2>::get_lowest_common_size() {
    if (!discrete_labels) {
      cerr << "warning: get_lowest_common_size() should not be called "
	   << "with continous labels." << endl;
      return 0;
    }
    intg min_nonzero = (std::numeric_limits<intg>::max)();
    for (vector<intg>::iterator i = counts.begin(); i != counts.end(); ++i) {
      if ((*i < min_nonzero) && (*i != 0))
	min_nonzero = *i;
    }
    if (min_nonzero == (std::numeric_limits<intg>::max)())
      eblerror("empty dataset");
    return min_nonzero * nclasses;
  }
  
  //////////////////////////////////////////////////////////////////////////////
  // data access methods
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::
  fprop(fstate_idx<Tnet> &out, fstate_idx<Tin2> &label) {
    out.resize(sample_dims());
    idx<Tin1> dat = data[it];
    idx<Tin2> lab = labels[it];
    idx_copy(dat, out.x);
    idx_addc(out.x, bias, out.x);
    idx_dotc(out.x, coeff, out.x);
    idx_copy(lab, label.x);
  }

  template <class Tnet, class Tin1, class Tin2>
  idx<Tin1> datasource<Tnet, Tin1, Tin2>::get_sample(intg index) {
    return data[index];
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::select_sample(intg index) {
    if (index < 0 || index >= data.dim(0))
      eblthrow("cannot select index " << index
	       << " in datasource of dimensions " << data);
    it = index;
  }

  //////////////////////////////////////////////////////////////////////////////
  // iterating methods
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::shuffle() {
    // shuffle indices to the data
    idx_shuffle(indices);
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::next() {
    // increment test iterator
    it_test++;
    // reset if reached end
    if(!it_test >= data.dim(0))
      it_test = 0;
    // set main iterator used by fprop
    it = it_test;
  }

  template <class Tnet, class Tin1, class Tin2>
  bool datasource<Tnet, Tin1, Tin2>::next_train() {
    // check that this datasource is allowed to call this method
    if (test_set)
      eblerror("forbidden call of next_train() on testing sets");
    bool pick = false;
    not_picked++;
    // return samples in original order, regardless of their class
    if (!balance) {
      // increment iterator
      it_train++;
      // reset if reached end
      if(it_train >= indices.dim(0)) {
	if (shuffle_passes)
	  shuffle(); // shuffle indices to the data
	// reset iterator
	it_train = 0;
	// normalize probabilities for all classes, mapping [0..max] to [0..1]
	if (weigh_samples) 
	  normalize_probas();
      }
      it = indices.get(it_train); // set main iterator to the train iterator
      // recursively loop until we find a sample that is picked for this class
      pick = pick_current();
    } else { // return samples in class-balanced order
      // get pointer to first non empty class
      while (!bal_indices[class_it].size()) {
	class_it++; // next class if class is empty
	if (class_it >= bal_indices.size())
	  class_it = 0;
      }
      it = bal_indices[class_it][bal_it[class_it]];
      bal_it[class_it] += 1;
      // decide if we want to select this sample for training
      pick = pick_current();
      // decrement epoch counter
      //      if (epoch_done_counters[class_it] > 0)
      epoch_done_counters[class_it] = epoch_done_counters[class_it] - 1;
      if (bal_it[class_it] >= bal_indices[class_it].size()) {
	// returning to begining of list for this class
	bal_it[class_it] = 0;
	// shuffling list for this class
	if (shuffle_passes) {
	  vector<intg> &clist = bal_indices[class_it];
	  random_shuffle(clist.begin(), clist.end());
	}
	normalize_probas(class_it);
      }
      // recursion failsafe, allow 1000 max recursions
      if (not_picked > MIN(1000, (intg) bal_indices[class_it].size())) {
	// we called recursion on this method more than number of class samples
	// give up and show current sample
	pick = true;
      }
    }
    epoch_cnt++;
    if (pick) {
      // increment pick counter for this sample
      if (count_pickings)
	pick_count.set(pick_count.get(it) + 1, it);
#ifdef __DEBUG__
      cout << "Picking sample " << it << " (label: " << (int)labels.get(it)
	   << ", pickings: " << pick_count.get(it) << ", energy: "
	   << energies.get(it) << ", correct: " << (int) correct.get(it);
      if (weigh_samples) cout << ", proba: " << probas.get(it);
      cout << ")" << endl;
#endif
      // if we picked a sample, jump to next class
      class_it++; // next class
      if (class_it >= bal_indices.size())
	class_it = 0; // reseting to first class in class list
      // increment sample counter
      epoch_pick_cnt++;
      not_picked = 0;
      pretty_progress();
      return true;
  } else {
#ifdef __DEBUG__
      cout << "Not picking sample " << it << " (label: "
	   << (int) labels.get(it) << ", pickings: " << pick_count.get(it)
	   << ", energy: " << energies.get(it)
	   << ", correct: " << (int) correct.get(it)
	   << ", proba: " << probas.get(it) << ")" << endl;
#endif
      pretty_progress();
      return false;
    }
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
    if (r <= probas.get(it))
      return true;
    return false;
  }

  template <class Tnet, class Tin1, class Tin2>
  bool datasource<Tnet, Tin1, Tin2>::epoch_done() {
    switch (epoch_mode) {
    case 0: // fixed number of samples
      if (epoch_cnt >= epoch_sz)
	return true;
      break ;
    case 1: // see all samples at least once
      if (balance) {
	// check that all classes are done
	for (uint i = 0; i < epoch_done_counters.size(); ++i) {
	  if (epoch_done_counters[i] > 0)
	    return false;
	}
	return true; // all classes are done
      } else { // do not balance, use epoch_sz
	if (epoch_cnt >= epoch_sz)
	  return true;
      }
      break ;
    default: eblerror("unknown epoch_mode");
    }
    return false;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::init_epoch() {
    epoch_cnt = 0;
    epoch_pick_cnt = 0;
    epoch_timer.restart();
    epoch_show_printed = -1; // last epoch count we have printed
    if (balance) {
      uint maxsize = 0;
      // for balanced training, set each class to not done.
      for (uint k = 0; k < bal_indices.size(); ++k) {
	epoch_done_counters[k] = bal_indices[k].size();
	if (bal_indices[k].size() > maxsize)
	  maxsize = bal_indices[k].size();
      }
      if (epoch_mode == 1) // for ETA estimation only, estimate epoch size
	epoch_sz = maxsize * bal_indices.size();
    }
    // if we have prior information about each sample energy and classification
    // let's use it to initialize the picking probabilities.
    normalize_all_probas();
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::seek_begin() {
    // reset test iterator
    it_test = 0;
    // set main iterator to test iterator
    it = it_test;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::seek_begin_train() {
    // reset train iterator
    it_train = 0;
    // set main iterator to train iterator
    it = indices.get(it_train);
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_balanced(bool bal) {
    balance = bal;
    if (!balance) // unbalanced
      cout << _name << ": Setting training as unbalanced (not taking class "
	   << "distributions into account)." << endl;
    else { // balanced
      cout << _name << ": Setting training as balanced (taking class "
	   << "distributions into account)." << endl;
      if (discrete_labels) {
	// compute vector of sample indices for each class
	bal_indices.clear();
	bal_it.clear();
	epoch_done_counters.clear();
	class_it = 0;
	for (intg i = 0; i < nclasses; ++i) {
	  vector<intg> indices;
	  bal_indices.push_back(indices);
	  bal_it.push_back(0); // init iterators
	}
	// distribute sample indices into each vector based on label
	for (uint i = 0; i < size(); ++i)
	  bal_indices[(intg) (labels.get(i))].push_back(i);
	for (uint i = 0; i < bal_indices.size(); ++i) {
	  // shuffle
	  random_shuffle(bal_indices[i].begin(), bal_indices[i].end());
	  // init epoch counters
	  epoch_done_counters.push_back(bal_indices[i].size());
	}
      } else
	cerr << "warning: cannot use balanced() with continuous labels" << endl;
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_shuffle_passes(bool activate) {
    shuffle_passes = activate;
    cout << _name
	 << ": Shuffling of samples (training only) after each pass is "
	 << (shuffle_passes ? "activated" : "deactivated") << "." << endl;
  }
  
  //////////////////////////////////////////////////////////////////////////////
  // picking probability methods
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::normalize_all_probas() {
    if (weigh_samples) {
      if (perclass_norm && balance) {
	for (uint i = 0; i < bal_indices.size(); ++i)
	  normalize_probas(i);
      } else
	normalize_probas();
    }
  }
    
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::normalize_probas(int classid) {
    double maxproba = 0, minproba = (numeric_limits<double>::max)();
    double maxenergy = 0, sum = 0; //, energy_ratio, maxenergy2;
    vector<intg> *cindices = NULL;
    vector<intg> allindices;
    if (weigh_samples && !is_test()) {
      cout << _name << ": Normalizing probabilities";
      if (perclass_norm && balance) { // use only class_it class samples
	if (classid < 0)
	  eblerror("class id cannot be negative");
	uint class_it = (uint) classid;
	cindices = &(bal_indices[class_it]);
	cout << " for class " << class_it;
      } else { // use all samples
	allindices.resize(energies.dim(0)); // allocate
	for (intg i = 0; i < energies.dim(0); ++i)
	  allindices[i] = i;
	cindices = &allindices;
	cout << " for all classes";
      }
      idx<double> sorted_energies(cindices->size());
      // normalize probas for this class, mapping [0..max] to [0..1]
      maxenergy = 0; sum = 0;
      intg nincorrect = 0, ncorrect = 0, i = 0;
      // get max and sum
      for (vector<intg>::iterator j = cindices->begin();
	   j != cindices->end(); ++j) {
	// don't take correct ones into account
	if (energies.get(*j) < 0) // energy not set yet
	  continue ;
	if (correct.get(*j) == 1) { // correct	  
	  ncorrect++;
	  if (ignore_correct_)
	    continue ; // skip this one
	} else 
	  nincorrect++;
	// max and sum
	maxenergy = (std::max)(energies.get(*j), maxenergy);
	sum += energies.get(*j);
	sorted_energies.set(energies.get(*j), i++);
      }
      cout << ", nincorrect: " << nincorrect << ", ncorrect: " << ncorrect;
      // no incorrect set all to 1
      if (!nincorrect) {
	idx_fill(probas, 1.0);
	return ;
      }
      // We choose 2 pivot points in the sorted energies curve,
      // one will be used as maximum energy and the other as the minimum
      // energy. This helps to have a meaningful range of energies not
      // biased by single extrema.
      double e1, e2;
      sorted_energies.resize(nincorrect);
      idx_sortup(sorted_energies);      
      intg pivot1 = (intg) (sorted_energies.dim(0) * (float) .25);
      intg pivot2 = std::min(sorted_energies.dim(0) - 1,
			     (intg) (sorted_energies.dim(0)*(float)1.0));//.75);
      if (sorted_energies.dim(0) == 0) {
	e1 = 0; e2 = 1;
      } else {
	e1 = sorted_energies.get(pivot1);
	e2 = sorted_energies.get(pivot2);
      }
      // the ratio of total energies over n times the max energy
      //energy_ratio = sum / (maxenergy * cindices->size());
      // the max probability will be proportional to the energy ratio
      // this balances the probabilities so that outliers don't take
      // all the probabilites
      //maxenergy2 = maxenergy * energy_ratio;
      cout << ", max energy: " << maxenergy;
	   // << ", energy ratio " << energy_ratio 
	   // << " and normalized max energy " << maxenergy2;
      // normalize
      for (vector<intg>::iterator j = cindices->begin();
	   j != cindices->end(); ++j) {
	double e = energies.get(*j);
	// set proba 0 for correct samples if we ignore correct ones
	if (e >= 0 && ignore_correct_ && correct.get(*j) == 1)
	  probas.set(0.0, *j);
	else {
	  // compute probas
	  double den = e2 - e1;
	  if (e < 0 || maxenergy == 0 || den == 0) // energy not set yet
	    probas.set(1.0, *j);
	  else {
	    probas.set((std::max)((double) 0, (std::min)((e - e1) / den,
							  (double) 1)), *j);
	    if (!hardest_focus) // concentrate on easiest misclassified
	      probas.set(1 - probas.get(*j), *j); // reverse proba
	    // iprobas.at(*j)->set((std::max)(sample_min_proba, 
	    // 				   e / maxenergy2));
	    // remember min and max proba
	    maxproba = (std::max)(probas.get(*j), maxproba);
	    minproba = (std::min)(probas.get(*j), minproba);
	  }
	}
      }
      cout << ", Min/Max probas are: " << minproba << ", "
	   << maxproba << endl;
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_sample_energy(double e, bool correct_, 
						       Tin2 label_estimate) {
    energies.set(e, it);
    correct.set(correct_ ? 1 : 0, it);
    answers.set(label_estimate, it);
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::save_pickings(const char *name_) {
    // plot file
    string name = "pickings";
    if (name_)
      name = name_;
    string fname = name;
    fname += ".plot"; 
    ofstream fp(fname.c_str());
    if (!fp) {
      cerr << "failed to open " << fname << endl;
      eblerror("failed to open file for writing");
    }
    typename idx<uint>::dimension_iterator i = pick_count.dim_begin(0);
    uint j = 0;
    for ( ; i.notdone(); i++, j++)
      fp << j << " " << i->get() << endl;
    fp.close();
    cout << _name << ": Wrote picking statistics in " << fname << endl; 
    // p file
    string fname2 = name;
    fname2 += ".p";
    ofstream fp2(fname2.c_str());
    if (!fp2) {
      cerr << "failed to open " << fname2 << endl;
      eblerror("failed to open file for writing");
    }
    fp2 << "plot \"" << fname << "\" with impulse" << endl;
    fp2.close();
    cout << _name << ": Wrote gnuplot file in " << fname2 << endl;

    // plot by class
    write_classed_pickings(pick_count, correct, name);
    write_classed_pickings(energies, correct, name, "_energies");
    idx<double> e = idx_copy(energies);
    idx<ubyte> c = idx_copy(correct);
    idx_sortup(e, c);
    write_classed_pickings(e, c, name, "_sorted_energies");
    idx<double> p = idx_copy(probas);
    c = idx_copy(correct);
    idx_sortup(p, c);
    write_classed_pickings(p, c, name, "_sorted_probas");
    p = idx_copy(probas);
    e = idx_copy(energies);
    c = idx_copy(correct);
    idx_sortup(e, c, p);
    write_classed_pickings(p, c, name, "_probas_sorted_by_energy", true,
			   "Picking probability");
    write_classed_pickings(p, c, name, "_probas_sorted_by_energy_wrong_only",
			   false, "Picking probability");
    write_classed_pickings(e, c, name, "_energies_sorted_by_energy_wrong_only",
			   false, "Energy");
  }

  template <class Tnet, class Tin1, class Tin2> template <typename T>
  void datasource<Tnet, Tin1, Tin2>::
  write_classed_pickings(idx<T> &m, idx<ubyte> &c, string &name_,
			 const char *name2_, bool plot_correct,
			 const char *ylabel) {
    string name = name_;
    if (name2_)
      name += name2_;
    name += "_classed";
    // sorted classed plot file
    if (labels.order() == 1) { // single label value
      string fname = name;
      fname += ".plot"; 
      ofstream fp(fname.c_str());
      if (!fp) {
	cerr << "failed to open " << fname << endl;
	eblerror("failed to open file for writing");
      }
      typename idx<T>::dimension_iterator i = m.dim_begin(0);
      typename idx<Tin2>::dimension_iterator l = labels.dim_begin(0);
      typename idx<ubyte>::dimension_iterator ic = c.dim_begin(0);
      uint j = 0;
      for ( ; i.notdone(); i++, l++, ic++) {
	if (!plot_correct && ic->get() == 1)
	  continue ; // don't plot correct ones
	fp << j++;
	for (Tin2 k = 0; k < (Tin2) nclasses; ++k) {
	  if (k == l.get()) {
	    if (ic->get() == 0) { // wrong
	      fp << "\t" << i->get();
	      if (plot_correct)
		fp << "\t?";
	    } else if (plot_correct) // correct
	      fp << "\t?\t" << i->get();
	  } else {
	    fp << "\t?";
	    if (plot_correct)
	      fp << "\t?";
	  }
	}
	fp << endl;
      }
      fp.close();
      cout << _name << ": Wrote picking statistics in " << fname << endl;
      // p file
      string fname2 = name;
      fname2 += ".p";
      ofstream fp2(fname2.c_str());
      if (!fp2) {
	cerr << "failed to open " << fname2 << endl;
	eblerror("failed to open file for writing");
      }
      fp2 << "set title \"" << name << "\"; set ylabel \"" << ylabel
	  << "\"; plot \""
	  << fname << "\" using 1:2 title \"class 0 wrong\" with impulse";
      if (plot_correct)
	fp2 << ", \""
	    << fname << "\" using 1:3 title \"class 0 correct\" with impulse";
      for (uint k = 1; k < nclasses; ++k) {
	fp2 << ", \"" << fname << "\" using 1:" << k * (plot_correct?2:1) + 2 
	    << " title \"class " << k << " wrong\" with impulse";
	if (plot_correct)
	  fp2 << ", \"" << fname << "\" using 1:" << k * 2 + 3 
	      << " title \"class " << k << " correct\" with impulse";
      }
      fp << endl;
      fp2.close();
      cout << _name << ": Wrote gnuplot file in " << fname2 << endl;
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::ignore_correct(bool ignore) {
    ignore_correct_ = ignore;
    if (ignore)
      cout << (ignore ? "Ignoring" : "Using") <<
	" correctly classified samples for training." << endl;
  }
    
  template <class Tnet, class Tin1, class Tin2>
  bool datasource<Tnet, Tin1, Tin2>::get_count_pickings() {
    return count_pickings;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_count_pickings(bool count) {
    count_pickings = count;
  }

  template <class Tnet, class Tin1, class Tin2>
    map<uint,intg>& datasource<Tnet, Tin1, Tin2>::get_pickings() {
    picksmap.clear();
    typename idx<uint>::dimension_iterator i = pick_count.dim_begin(0);
    uint j = 0;
    for ( ; i.notdone(); i++, j++)
      picksmap[i->get()] = j;
    return picksmap;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::
  set_weigh_samples(bool activate, bool hardest_focus_, bool perclass_norm_,
		    double min_proba) {
    weigh_samples = activate;
    hardest_focus = hardest_focus_;
    perclass_norm = perclass_norm_;
    sample_min_proba = MIN(1.0, min_proba);
    cout << _name
	 << ": Weighing of samples (training only) based on classification is "
	 << (weigh_samples ? "activated" : "deactivated") << "." << endl;
    if (activate) {
      cout << _name << ": learning is focused on "
	   << (hardest_focus ? "hardest" : "easiest")
	   << " misclassified samples" << endl;
      if (!ignore_correct_ && !hardest_focus)
	cerr << "Warning: correct samples are not ignored and focus is on "
	     << "easiest samples, this may not be optimal" << endl;
      cout << "Sample picking probabilities are normalized "
	   << (perclass_norm ? "per class" : "globally") 
	   << " with minimum probability " << sample_min_proba << endl;
    }
  }
  
  //////////////////////////////////////////////////////////////////////////////
  // pretty methods
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::pretty_progress() {
    if (epoch_show > 0 && epoch_pick_cnt % epoch_show == 0 &&
	epoch_show_printed != (intg) epoch_pick_cnt) {
      epoch_show_printed = (intg) epoch_pick_cnt; // remember last time printed
      cout << "epoch_cnt: " << epoch_cnt << " picked: " << epoch_pick_cnt
	   << " / " << epoch_sz 
	   << ", epoch elapsed: " << epoch_timer.elapsed() << ", ETA: "
	   << epoch_timer.
	elapsed((long) ((epoch_sz - epoch_pick_cnt) *
		(epoch_timer.elapsed_seconds() 
		 /(double)std::max((intg)1,epoch_pick_cnt))));
      if (balance) {
	cout << ", remaining:";
	for (uint i = 0; i < epoch_done_counters.size(); ++i) {
	  cout << " " << i << ": " << epoch_done_counters[i];
	}
      }
      cout << endl;
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::pretty() {
    cout << _name << ": Dataset \"" << _name << "\" contains " << data.dim(0);
    cout << " samples of dimension " << sample_dims();
    if (discrete_labels)
      cout << " with " << get_nclasses() << " classes," << endl;
    else
      cout << " with continous labels," << endl;
    cout << _name << ": bias is " << bias << ", coefficient is " << coeff;
    if (discrete_labels)
      cout << " and iteration size in samples is " << get_lowest_common_size();
    cout << "." << endl;
  }

  //////////////////////////////////////////////////////////////////////////////
  // state saving
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::save_state() {
    state_saved = true;
    count_pickings_save = count_pickings;
    it_saved = it; // save main iterator
    if (!balance) { // save (unbalanced) iterators
      it_test_saved = it_test;
      it_train_saved = it_train;
      for (intg k = 0; k < indices.dim(0); ++k)
	indices_saved[k] = indices[k];
    } else { // save balanced iterators
      bal_it_saved.clear();
      bal_indices_saved.clear();
      for (uint k = 0; k < bal_it.size(); ++k) {
	bal_it_saved.push_back(bal_it[k]);
	vector<intg> indices;
	for (uint l = 0; l < bal_indices[k].size(); ++l)
	  indices.push_back(bal_indices[k][l]);
	bal_indices_saved.push_back(indices);
      }
      class_it_saved = class_it;
    }
  }
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::restore_state() {
    if (!state_saved)
      eblerror("state not saved, call save_state() before restore_state()");
    count_pickings = count_pickings_save;
    it = it_saved; // restore main iterator
    if (!balance) { // restore unbalanced
      it_test = it_test_saved;
      it_train = it_train_saved;
      for (intg k = 0; k < indices.dim(0); ++k)
	indices[k] = indices_saved[k];
    } else { // restore balanced iterators
      for (uint k = 0; k < bal_it.size(); ++k) {
	bal_it[k] = bal_it_saved[k];
	for (uint l = 0; l < bal_indices[k].size(); ++l)
	  bal_indices[k][l] = bal_indices_saved[k][l];
      }
      class_it = class_it_saved;
    }
  }
  
  template <class Tnet, class Tdata, class Tlabel>
  void datasource<Tnet, Tdata, Tlabel>::set_epoch_show(uint modulo) {
    cout << _name << "Print training count every " << modulo
	 << " samples." << endl;
    epoch_show = modulo;
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
    this->lblstr = NULL;
    idx<double> *cont1 = dynamic_cast<idx<double>*>(&labels_);
    idx<float> *cont2 = dynamic_cast<idx<float>*>(&labels_);
    if ((classes.order() == 2) && // assign labels only if non continuous
	!cont1 && !cont2) {
      this->lblstr = new vector<string*>;
      idx_bloop1(classe, classes, ubyte) {
	this->lblstr->push_back(new string((const char*) classe.idx_ptr()));
      }
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
    // if no names are given and discrete, use indices as names
    if (!this->lblstr && this->discrete_labels) { 
      this->lblstr = new vector<string*>;
      ostringstream o;
      int imax = (int) idx_max(this->labels);
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
      cout << this->_name << ": It has: ";
      uint i;
      for (i = 0; i < this->counts.size() - 1; ++i)
	cout << this->counts[i] << " \"" << *(*lblstr)[i] << "\", ";
      cout << "and " << this->counts[i] << " \"" << *(*lblstr)[i] << "\".";
      cout << endl;
    }
  }
    
  template <class Tnet, class Tdata, class Tlabel>
  int labeled_datasource<Tnet, Tdata, Tlabel>::get_class_id(const char *name) {
    int id_ = -1;
    vector<string*>::iterator i = lblstr->begin();
    for (int j = 0; i != lblstr->end(); ++i, ++j) {
      if (!strcmp(name, (*i)->c_str()))
	id_ = j;
    }
    return id_;
  }
  
  template <class Tnet, class Tdata, class Tlabel>
  std::string &labeled_datasource<Tnet, Tdata, Tlabel>::get_class_name(int id) {
    if (!lblstr) 
      eblerror("no label strings");
    if (id >= (int) lblstr->size())
      eblerror("requesting label string at index " << id 
	       << " but string vector has only " << lblstr->size() 
	       << " elements.");
    string *s = (*lblstr)[id];
    if (!s)
      eblerror("empty label string");
    return *s;
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
    try {
      pairs = load_matrix<intg>(pairs_fname);
    } catch(string &err) {
      cerr << "error: " << err << endl;
      cerr << "failed to load dataset file " << pairs_fname << endl;
      eblerror("Failed to load dataset file");
    }
    typename idx<intg>::dimension_iterator	 diter(pairs, 0);
    pairsIter = diter;
  }
  
  // constructor
  template <class Tnet, class Tdata, class Tlabel>
  labeled_pair_datasource<Tnet, Tdata, Tlabel>::
  labeled_pair_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
			  idx<ubyte> &classes_, idx<intg> &pairs_,
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
  fprop(fstate_idx<Tnet> &in1, fstate_idx<Tnet> &in2,
	fstate_idx<Tlabel> &label) {
    eblerror("fixme");
    // in1.resize(this->sample_dims());
    // in2.resize(this->sample_dims());
    // intg id1 = pairsIter.get(0), id2 = pairsIter.get(1);
    // Tlabel lab = this->labels.get(id1);
    // label.x.set(lab);
    // idx<Tdata> im1 = this->data[id1], im2 = this->data[id2];
    // idx_copy(im1, in1.x);
    // idx_copy(im2, in2.x);
    // idx_addc(in1.x, this->bias, in1.x);
    // idx_dotc(in1.x, this->coeff, in1.x);
    // idx_addc(in2.x, this->bias, in2.x);
    // idx_dotc(in2.x, this->coeff, in2.x);
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
  mnist_datasource(const char *root, bool train_data, uint size) {
    try {
      // load dataset
      ostringstream datafile, labelfile, name;
      string setname = "MNIST";
      if (train_data) { // training set
	setname = "train";
      } else { // testing set
	setname = "t10k";
	this->set_test(); // remember that this is the testing set
      }
      datafile << root << "/" << setname << "-images-idx3-ubyte";
      labelfile << root << "/" << setname << "-labels-idx1-ubyte";
      name << "MNIST " << setname;
      idx<Tdata> dat = load_matrix<Tdata>(datafile.str());
      idx<Tlabel> labs = load_matrix<Tlabel>(labelfile.str());
      dat = dat.narrow(0, MIN(dat.dim(0), (intg) size), 0);
      labs = labs.narrow(0, MIN(labs.dim(0), (intg) size), 0);
      init(dat, labs, name.str().c_str(), (Tdata) 0, (float) 0.01);
    } catch(string &err) {
      cerr << err << endl;
      eblerror("failed to load mnist dataset");
    }
  }

  template <class Tnet, class Tdata, class Tlabel>
  mnist_datasource<Tnet, Tdata, Tlabel>::
  mnist_datasource(const char *root, const char *name, uint size) {
    try {
      // load dataset
      ostringstream datafile, labelfile;
      datafile << root << "/" << name << "_" << DATA_NAME << MATRIX_EXTENSION;
      labelfile << root << "/" << name 
		<< "_" << LABELS_NAME << MATRIX_EXTENSION;
      idx<Tdata> dat = load_matrix<Tdata>(datafile.str());
      idx<Tlabel> labs = load_matrix<Tlabel>(labelfile.str());
      dat = dat.narrow(0, MIN((uint) dat.dim(0), size), 0);
      labs = labs.narrow(0, MIN((uint) labs.dim(0), size), 0);
      init(dat, labs, name, (Tdata) 0, (float) 0.01);
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
  fprop(fstate_idx<Tnet> &out, fstate_idx<Tlabel> &label) {
    out.resize(this->sample_dims());
    idx<Tdata> dat = data[it];
    idx<Tlabel> lab = labels[it];
    uint ni = data.dim(1);
    uint nj = data.dim(2);
    uint di = (uint) (0.5 * (height - ni));
    uint dj = (uint) (0.5 * (width - nj));
    out.clear_x();
    idx<Tnet> tgt = out.x.select(0, 0);
    tgt = tgt.narrow(0, ni, di);
    tgt = tgt.narrow(1, nj, dj);
    idx_copy(dat, tgt);
    idx_addc(out.x, bias, out.x);
    idx_dotc(out.x, coeff, out.x);
    label.x.set(lab.get());
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
