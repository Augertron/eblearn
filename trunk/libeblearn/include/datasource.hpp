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
      energies(1), pick_count(1), count_pickings(true), state_saved(false),
      dataIter(data, 0), labelsIter(labels, 0), 
      dataIter_test(data, 0), labelsIter_test(labels, 0), 
      dataIter_train(data, 0), labelsIter_train(labels, 0), 
      probasIter_train(probas, 0), energiesIter_train(energies, 0),
      pickIter_train(pick_count, 0),
      height(0), width(0), balance(true), isample(0), test_set(false),
      sample_min_proba(0.0), epoch_sz(0), epoch_cnt(0), epoch_pick_cnt(0),
      epoch_mode(1) {
  }

  template <class Tnet, class Tin1, class Tin2>
  datasource<Tnet, Tin1, Tin2>::
  datasource(const datasource<Tnet, Tin1, Tin2> &ds)
    : nclasses(0), bias(ds.bias), coeff(ds.coeff), data(ds.data),
      labels(ds.labels), probas(ds.probas), energies(ds.energies),
      pick_count(ds.pick_count), count_pickings(ds.count_pickings),
      state_saved(false), dataIter(data, 0), labelsIter(labels, 0),
      dataIter_test(data, 0), labelsIter_test(labels, 0),
      dataIter_train(data, 0), labelsIter_train(labels, 0), 
      probasIter_train(probas, 0), energiesIter_train(energies, 0),
      pickIter_train(pick_count, 0),
      height(ds.height), width(ds.width), _name(ds._name), balance(true),
      isample(0), test_set(false),
      sample_min_proba(0.0), epoch_sz(0), epoch_cnt(0), epoch_pick_cnt(0),
      epoch_mode(1) {
  }

  template <class Tnet, class Tin1, class Tin2>
  datasource<Tnet, Tin1, Tin2>::
  datasource(idx<Tin1> &data_, idx<Tin2> &labels_, const char *name_,
	     Tin1 b, float c)
    : nclasses(0), bias(b), coeff(c), data(data_), labels(labels_),
      probas(data_.dim(0)), energies(data_.dim(0)), pick_count(data_.dim(0)),
      count_pickings(true), state_saved(false),
      dataIter(data, 0), labelsIter(labels, 0),
      dataIter_test(data, 0), labelsIter_test(labels, 0),
      dataIter_train(data, 0), labelsIter_train(labels, 0), 
      probasIter_train(probas, 0), energiesIter_train(energies, 0),
      pickIter_train(pick_count, 0),
      height(data.dim(1)), width(data.dim(2)), balance(true), isample(0), 
      test_set(false),
      sample_min_proba(0.0), epoch_sz(0), epoch_cnt(0), epoch_pick_cnt(0),
      epoch_mode(1) {
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
    probas = idx<double>(data.dim(0));
    energies = idx<double>(data.dim(0));
    pick_count = idx<uint>(data.dim(0));
    idx_clear(pick_count);
    dynamic_init_drand(); // initialize random seed
    // default probability for a sample of being used is 1
    idx_fill(probas, 1.0);
    idx_fill(energies, -1.0);
    height = data.dim(1);
    width = data.dim(2);
    nclasses = (intg) idx_max(labels) + 1;
    bias = b;
    coeff = c;
    _name = (name_ ? name_ : "Unknown Dataset");
    // regular iterators
    typename idx<Tin1>::dimension_iterator	 dIter(data, 0);
    typename idx<Tin2>::dimension_iterator	 lIter(labels, 0);
    dataIter = dIter;
    labelsIter = lIter;
    // testing iterators
    typename idx<Tin1>::dimension_iterator	 dIter_test(data, 0);
    typename idx<Tin2>::dimension_iterator	 lIter_test(labels, 0);
    dataIter_test = dIter_test;
    labelsIter_test = lIter_test;
    // training only iterators
    typename idx<Tin1>::dimension_iterator	 dIter_train(data, 0);
    typename idx<Tin2>::dimension_iterator	 lIter_train(labels, 0);
    typename idx<double>::dimension_iterator	 pIter_train(probas, 0);
    typename idx<double>::dimension_iterator	 eIter_train(energies, 0);
    typename idx<uint>::dimension_iterator	 pcIter_train(pick_count, 0);
    dataIter_train = dIter_train;
    labelsIter_train = lIter_train;
    probasIter_train = pIter_train;
    energiesIter_train = eIter_train;
    pickIter_train = pcIter_train;
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
    set_weigh_samples(true); // for next_train only
    set_weigh_normalization(true); // per class by default
    datasource<Tnet, Tin1, Tin2>::pretty();
    seek_begin();
    seek_begin_train();
    epoch_sz = size(); //get_lowest_common_size();
    epoch_mode = 1;
    cout << _name << ": Each training epoch sees " << epoch_sz << " samples." << endl;
    init_epoch();
    not_picked = 0;
    epoch_show = 50; // print epoch count message every epoch_show
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
    // get the nb of classes
    intg nbOfClasses = (intg) (1 + idx_max(labels));
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

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::
  fprop(fstate_idx<Tnet> &out, fstate_idx<Tin2> &label) {
    out.resize(sample_dims());
    idx_copy(*(this->dataIter), out.x);
    idx_addc(out.x, bias, out.x);
    idx_dotc(out.x, coeff, out.x);
    idx_copy(*labelsIter, label.x);
  }

  template <class Tnet, class Tin1, class Tin2>
  idx<Tin1> datasource<Tnet, Tin1, Tin2>::get_sample(intg index) {
    return data.select(0, index);
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::next() {
    // increment data and labels iterators
    ++dataIter_test;
    ++labelsIter_test;
    // reset if reached end
    if(!dataIter_test.notdone()) {
      // reset iterators
      dataIter_test = data.dim_begin(0);
      labelsIter_test = labels.dim_begin(0);
    }
    // set regular iters used by fprop
    dataIter = dataIter_test;
    labelsIter = labelsIter_test;
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
    if (r <= probasIter_train->get())
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
    if (balance) {
      intg maxsize = 0;
      // for balanced training, set each class to not done.
      for (uint k = 0; k < label_indices.size(); ++k) {
	epoch_done_counters[k] = label_indices[k].size();
	if (label_indices[k].size() > maxsize)
	  maxsize = label_indices[k].size();
      }
      // for ETA estimation only, estimate epoch size
      epoch_sz = maxsize * label_indices.size();
    }
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
      // increment iterators
      ub_itr++;
      // reset if reached end
      if(ub_itr == ub_indices.end()) {
	// shuffling list for this class
	if (shuffle_passes)
	  random_shuffle(ub_indices.begin(), ub_indices.end());
	// reset iterator
	ub_itr = ub_indices.begin();
	// normalize probabilities for all classes, mapping [0..max] to [0..1]
	if (weigh_samples) 
	  normalize_probas();
      }
      dataIter_train = dataIter_train.at(*ub_itr);
      labelsIter_train = labelsIter_train.at(*ub_itr);
      probasIter_train = probasIter_train.at(*ub_itr);
      energiesIter_train = energiesIter_train.at(*ub_itr);
      pickIter_train = pickIter_train.at(*ub_itr);
      isample = *ub_itr; // current sample's index
      // recursively loop until we find a sample that is picked for this class
      pick = pick_current();
    } else { // return samples in class-balanced order
      // get pointer to first non empty class
      while (!label_indices[iitr].size()) {
	iitr++; // next class if class is empty
	if (iitr >= label_indices.size())
	  iitr = 0;
      }
      intg i = label_indices[iitr][indices_itr[iitr]];
      //      cout << "#" << i << "/" << iitr << " ";
      dataIter_train = dataIter_train.at(i);
      labelsIter_train = labelsIter_train.at(i);
      probasIter_train = probasIter_train.at(i);
      energiesIter_train = energiesIter_train.at(i);
      pickIter_train = pickIter_train.at(i);
      isample = i; // current sample's index
      indices_itr[iitr] += 1;
      // decide if we want to select this sample for training
      pick = pick_current();
      // decrement epoch counter
      //      if (epoch_done_counters[iitr] > 0)
      epoch_done_counters[iitr] = epoch_done_counters[iitr] - 1;
      if (indices_itr[iitr] >= label_indices[iitr].size()) {
	// returning to begining of list for this class
	indices_itr[iitr] = 0;
	// shuffling list for this class
	if (shuffle_passes) {
	  vector<intg> &clist = label_indices[iitr];
	  random_shuffle(clist.begin(), clist.end());
	}
	normalize_probas(iitr);
      }
      // recursion failsafe, allow 1000 max recursions
      if (not_picked > MIN(1000, (intg) label_indices[iitr].size())) {
	// we called recursion on this method more than number of class samples
	// give up and show current sample
	pick = true;
      }
    }
    // set regular iters used by fprop
    dataIter = dataIter_train;
    labelsIter = labelsIter_train;
    epoch_cnt++;
    if (pick) {
      // increment pick counter for this sample
      if (count_pickings)
	pickIter_train->set(pickIter_train->get() + 1);
#ifdef __DEBUG__
	cout << "Picking sample " << isample << " (label: " << labelsIter.get()
	     << ", pickings: " << pickIter_train->get() << ", energy: "
	     << energiesIter_train->get();
	if (weigh_samples)
	  cout << ", proba: " << probasIter_train->get();
	cout << ")" << endl;
#endif
      // if we picked a sample, jump to next class
      iitr++; // next class
      if (iitr >= label_indices.size())
	iitr = 0; // reseting to first class in class list
      // increment sample counter
      epoch_pick_cnt++;
      not_picked = 0;
      pretty_progress();
      return true;
  } else {
#ifdef __DEBUG__
      cout << "Not picking sample " << isample << " (label: " << labelsIter.get()
	   << ", proba: " << probasIter_train->get() << ", energy: "
	   << energiesIter_train->get() << ") " << endl;
#endif
      pretty_progress();
      return false;
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::pretty_progress() {
    if (epoch_show > 0 && epoch_cnt % epoch_show == 0) {
      cout << "epoch_cnt: " << epoch_cnt << " / " << epoch_sz 
	   << ", used samples: " << epoch_pick_cnt << ", epoch elapsed: ";
      epoch_timer.pretty_elapsed();
      cout << ", ETA: ";
      timer::pretty_secs((long) epoch_timer.elapsed_seconds() 
			 * (epoch_sz / epoch_cnt - 1));
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
  void datasource<Tnet, Tin1, Tin2>::normalize_all_probas() {
    if (weigh_samples) {
      if (perclass_norm && balance) {
	for (uint i = 0; i < label_indices.size(); ++i)
	  normalize_probas(i);
      } else
	normalize_probas();
    }
  }
    
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::normalize_probas(int classid) {
    double maxproba, minproba, maxenergy, sum, energy_ratio, maxenergy2;
    if (weigh_samples && !is_test()) {
      if (perclass_norm && balance) {
	uint iitr = (uint) classid;
	if (classid < 0)
	  eblerror("class id cannot be negative");
	// normalize probas for this class, mapping [0..max] to [0..1]
	maxenergy = 0; sum = 0;
	// get max and sum
	for (vector<intg>::iterator j = label_indices[iitr].begin();
	     j != label_indices[iitr].end(); ++j) {
	  maxenergy = (std::max)(energiesIter_train.at(*j)->get(), maxenergy);
	  sum += energiesIter_train.at(*j)->get();
	}
	// the ratio of total energies over n times the max energy
	energy_ratio = sum / (maxenergy * label_indices[iitr].size());
	// the max probability will be proportional to the energy ratio
	// this balances the probabilities so that outliers don't take
	// all the probabilites
	maxenergy2 = maxenergy * energy_ratio;
	cout << _name << ": Normalizing probabilities for class " << iitr
	     << ", with maximum energy " << maxenergy 
	     << ", energy ratio " << energy_ratio 
	     << " and normalized max energy " << maxenergy2;
	// normalize
	for (vector<intg>::iterator j = label_indices[iitr].begin();
	     j != label_indices[iitr].end(); ++j) {
	  // compute probas
	  double e = energiesIter_train.at(*j)->get();
	  if (e < 0 || maxenergy == 0) // energy not set yet
	    probasIter_train.at(*j)->set(1.0);
	  else
	    probasIter_train.at(*j)->set((std::max)(sample_min_proba, 
						    e / maxenergy2));
	  // first time, init min and max
	  if (j == label_indices[iitr].begin()) {
	    maxproba = probasIter_train.at(*j)->get();
	    minproba = probasIter_train.at(*j)->get();
	  } else {
	    maxproba = (std::max)(probasIter_train.at(*j)->get(), maxproba);
	    minproba = (std::min)(probasIter_train.at(*j)->get(), minproba);
	  }
	}
	//	  cout << "avg proba = " << avg / maxproba << endl;
      } else {
	// normalize probas for all classes, mapping [0..max] to [0..1]
	maxenergy = idx_max(energies);
	sum = idx_sum(energies);
	cout << _name << ": Normalizing probabilities with maximum energy: " 
	     << maxenergy;
	// the ratio of total energies over n times the max energy
	energy_ratio = sum / (maxenergy * energies.dim(0));
	// the max probability will be proportional to the energy ratio
	// this balances the probabilities so that outliers don't take
	// all the probabilites
	maxenergy2 = maxenergy * energy_ratio;
	cout << _name << ": Normalizing probabilities with maximum energy: "
	     << maxenergy << ", energy ratio " << energy_ratio 
	     << " and normalized max energy " << maxenergy2;
	if (maxenergy2 <= 0)
	  maxenergy2 = 1.0;
	idx_bloop2(e, energies, double, p, probas, double) {
	  if (e.get() < 0 || maxenergy2 == 0)
	    p.set(1.0); // energy hasn't been set yet, use proba 1.
	  else
	    p.set(e.get() / maxenergy2);
	}
	idx_threshold(probas, sample_min_proba);
	maxproba = idx_max(probas);
	minproba = idx_min(probas);
      }
      cout << ", Min/Max probas are: " << minproba << ", "
	   << maxproba << endl;
    }  
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_sample_energy(double e) {
    if (weigh_samples) { // if false, keep default probability 1 for all samples
      energiesIter_train->set(fabs(e));
      //      probasIter_train->set((std::max)(sample_min_proba, MIN(1.0, fabs(dist))));
    }
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_min_proba(double min_proba) {
    sample_min_proba = MIN(1.0, min_proba);
    cout << _name << ": Setting minimum probability to " << sample_min_proba << endl;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::seek_begin() {
    dataIter_test = data.dim_begin(0);
    labelsIter_test = labels.dim_begin(0);
    // set regular iters used by fprop
    dataIter = dataIter_test;
    labelsIter = labelsIter_test;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::seek_begin_train() {
    dataIter_train = data.dim_begin(0);
    labelsIter_train = labels.dim_begin(0);
    probasIter_train = probas.dim_begin(0);
    energiesIter_train = energies.dim_begin(0);
    pickIter_train = pick_count.dim_begin(0);
    // set regular iters used by fprop
    dataIter = dataIter_train;
    labelsIter = labelsIter_train;
  }

  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_balanced(bool bal) {
    balance = bal;
    if (!balance) { // unbalanced
      cout << _name << ": Setting training as unbalanced (not taking class "
	   << "distributions into account)." << endl;
      // initialize a random list of indexes for each sample, so that
      // we can randomize the access without moving data around.
      ub_indices.clear();
      for (intg i = 0; i < data.dim(0); ++i)
	ub_indices.push_back(i);
      random_shuffle(ub_indices.begin(), ub_indices.end());
      // reset iterator to 0
      ub_itr = ub_indices.begin();
    } else { // balanced
      cout << _name << ": Setting training as balanced (taking class "
	   << "distributions into account)." << endl;
      if (discrete_labels) {
	// compute vector of sample indices for each class
	label_indices.clear();
	indices_itr.clear();
	epoch_done_counters.clear();
	iitr = 0;
	for (intg i = 0; i < nclasses; ++i) {
	  vector<intg> indices;
	  label_indices.push_back(indices);
	  indices_itr.push_back(0); // init iterators
	}
	// distribute sample indices into each vector based on label
	for (uint i = 0; i < size(); ++i)
	  label_indices[(intg) (labels.get(i))].push_back(i);
	for (uint i = 0; i < label_indices.size(); ++i) {
	  // shuffle
	  random_shuffle(label_indices[i].begin(), label_indices[i].end());
	  // init epoch counters
	  epoch_done_counters.push_back(label_indices[i].size());
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
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_weigh_samples(bool activate) {
    weigh_samples = activate;
    cout << _name
	 << ": Weighing of samples (training only) based on classification is "
	 << (weigh_samples ? "activated" : "deactivated") << "." << endl;
  }
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::set_weigh_normalization(bool perclass) {
    perclass_norm = perclass;
    cout << _name << ": Weighing normalization (training only) is "
	 << (perclass_norm ? "per class" : "global") << "." << endl;
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
    write_classed_pickings(pick_count, name);
    write_classed_pickings(energies, name, "_energies");
    idx<double> e = idx_copy(energies);
    idx_sortup(e);
    write_classed_pickings(e, name, "_sorted_energies");
    idx<double> p = idx_copy(probas);
    idx_sortup(p);
    write_classed_pickings(p, name, "_sorted_probas");
  }

  template <class Tnet, class Tin1, class Tin2> template <typename T>
  void datasource<Tnet, Tin1, Tin2>::
  write_classed_pickings(idx<T> &m, string &name_, const char *name2_) {
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
      uint j = 0;
      for ( ; i.notdone(); i++, j++, l++) {
	fp << j;
	for (Tin2 k = 0; k < (Tin2) nclasses; ++k) {
	  if (k == l.get())
	    fp << "\t" << i->get();
	  else
	    fp << "\t?";
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
      fp2 << "plot \"" << fname 
	  << "\" using 1:2 title \"class 0\" with impulse";
      for (uint k = 1; k < nclasses; ++k) {
	fp2 << ", \"" << fname << "\" using 1:" << k + 2 
	    << " title \"class " << k << "\" with impulse";
      }
      fp << endl;
      fp2.close();
      cout << _name << ": Wrote gnuplot file in " << fname2 << endl;
    }
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
  string& datasource<Tnet, Tin1, Tin2>::name() {
    return _name;
  }
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::save_state() {
    state_saved = true;
    count_pickings_save = count_pickings;
    ub_itr_saved = ub_itr;
    indices_itr_saved.clear();
    label_indices_saved.clear();
    for (uint k = 0; k < indices_itr.size(); ++k) {
      indices_itr_saved.push_back(indices_itr[k]);
      vector<intg> indices;
      for (uint l = 0; l < label_indices[k].size(); ++l)
	indices.push_back(label_indices[k][l]);
      label_indices_saved.push_back(indices);
    }
    iitr_saved = iitr;
  }
  
  template <class Tnet, class Tin1, class Tin2>
  void datasource<Tnet, Tin1, Tin2>::restore_state() {
    if (!state_saved)
      eblerror("state not saved, call save_state() before restore_state()");
    count_pickings = count_pickings_save;
    if (!balance) {
      ub_itr = ub_itr_saved; // reset unbalanced iterator
      dataIter_train = dataIter_train.at(*ub_itr);
      labelsIter_train = labelsIter_train.at(*ub_itr);
      probasIter_train = probasIter_train.at(*ub_itr);
      energiesIter_train = energiesIter_train.at(*ub_itr);
      pickIter_train = pickIter_train.at(*ub_itr);
      isample = *ub_itr; // current sample's index
    } else {
      for (uint k = 0; k < indices_itr.size(); ++k) {
	indices_itr[k] = indices_itr_saved[k];
	for (uint l = 0; l < label_indices[k].size(); ++l)
	  label_indices[k][l] = label_indices_saved[k][l];
      }
      iitr = iitr_saved;
      intg i = label_indices[iitr][indices_itr[iitr]];
      dataIter_train = dataIter_train.at(i);
      labelsIter_train = labelsIter_train.at(i);
      probasIter_train = probasIter_train.at(i);
      energiesIter_train = energiesIter_train.at(i);
      pickIter_train = pickIter_train.at(i);
      isample = i; // current sample's index
    }
  }
  
  template <class Tnet, class Tdata, class Tlabel>
  void datasource<Tnet, Tdata, Tlabel>::set_epoch_show(uint modulo) {
    cout << _name << "Print training count every " << modulo << " samples." << endl;
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
    int id = -1;
    vector<string*>::iterator i = lblstr->begin();
    for (int j = 0; i != lblstr->end(); ++i, ++j) {
      if (!strcmp(name, (*i)->c_str()))
	id = j;
    }
    return id;
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
	this->set_test(); // remember that this is the testing set
	setname = "t10k";
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
    label.x.set((this->labelsIter).get());
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
