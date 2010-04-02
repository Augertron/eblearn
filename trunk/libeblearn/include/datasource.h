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

#ifndef DATASOURCE_H_
#define DATASOURCE_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_states.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  //! datasource
  template<class Tnet, class Tin1, class Tin2> class datasource {
  public:
    //! CAUTION: This empty constructor requires a subsequent call to init().
    datasource();

    //! initialization, called by constructor.
    void init(idx<Tin1> &inp, idx<Tin2> &lbl, const char *name,
	      Tin1 bias, float coeff);

    //! constructor.
    datasource(idx<Tin1> &inputs, idx<Tin2> &labels, const char *name = NULL, 
	       Tin1 bias = 0.0, float coeff = 0.01);

    //! copy constructor
    datasource(const datasource<Tnet, Tin1, Tin2> &ds);

    //! destructor
    virtual ~datasource();

    //! Copies the current datum to a state and label.
    virtual void fprop(state_idx<Tnet> &datum, idx<Tin2> &label);

    //! shuffle dataset, based on the number of classes
    //! assume the same nb of samples in each class
    virtual void shuffle();

    //! Returns the number of data instances contained in this data source.
    virtual unsigned int size();

    //! Returns an idxdim object describing the order (number of dimensions)
    //! and the size of each dimension of a single sample outputed by fprop.
    virtual idxdim sample_dims();

    //! Returns the index of the datum currently pointed to.
    // TODO: implement or get rid of tell?
    virtual int tell() { return -1; };

    //! Move to the next datum (in the original order of the dataset).
    //! This should be used during testing.
    //! It will always return the data in the same order with the same
    //! probability of 1. See next_train() for data returned with
    //! variable probability, balance, etc. (used for training only).
    virtual void next();

    //! Draw a random number between 0 and 1 and return true if higher
    //! than current sample's probability.
    virtual bool pick_current();
    
    //! Move to the next datum, in a way suited for training (_not_ for testing,
    //! for testing see next()): depending on the configuration, this will
    //! return samples in a class-balanced way, i.e. showing each class
    //! sequentially, with different probabilities based on sample's difficulty,
    //! or/and in a random order after each pass.
    //! When all samples of a class have been shown, it loops back to the first
    //! sample of that class. This should be used during training only.
    //! \param callcnt A counter of the number of recursive calls, used
    //!   to stop recursion if called more than the # of samples for cur class.
    virtual void next_train(intg *callcnt = NULL);

    //! Set the distance between the answer of the model to train and the
    //! true answer. This is used to give more or less probability for a
    //! sample to be used for training. At the beginning of training, all
    //! samples start with a probability of 1, thus all samples are used,
    //! but as training goes, easy samples are given lower probability while
    //! harder samples are given higher probability.
    //! The absolute distance is directly mapped into a probability, i.e.
    //! a distance of 0 will give probability 0 to be used, and distance of
    //! 1 and higher give probability 1 to be used. Therefore distance
    //! should be normalized so that a distance of 1 represents an offending
    //! answer.
    //! This is used only by next_train(), not by next().
    virtual void set_answer_distance(double dist);

    //! Set the minimum probaility of a sample to be picked by next_train().
    //! By default, this is zero. Acceptable range is [0 .. 1].
    virtual void set_min_proba(double proba);

    //! Move to the beginning of the data, for the test iterators only,
    //! i.e. only next() is affected, next_train() is unaffected.
    virtual void seek_begin();

    //! Move to the beginning of the data, for the train iterators only,
    //! i.e. only next_train() is affected, next() is unaffected.
    virtual void seek_begin_train();

    //! Make the next_train() method call sequentially one sample of each class
    //! instead of following the dataset's distribution.
    //! This is important to use when the dataset is unbalanced.
    //! This is set to true by default.
    //! This is used only by next_train(), not by next().
    virtual void set_balanced();

    //! Activate or deactivate shuffling of list of samples for each class
    //! after reaching the end of the sample list. This has an effect only
    //! when set_balanced() is set.
    //! This is activated by default.
    //! This is used only by next_train(), not by next().
    virtual void set_shuffle_passes(bool activate);

    //! Activate or deactivate weighing of samples based on classification
    //! results. Wrong answers give a higher probability for a sample
    //! to be used for training, correct answers a lower probability.
    //! This is activated by default.
    //! This is used only by next_train(), not by next().
    virtual void set_weigh_samples(bool activate);

    //! Set the normalization of the sample probabilities to be per class or
    //! global. This matters only if sample weighing is actived
    //! (see set_weigh_samples()).
    //! If perclass is true, the probabilities for
    //! each sample of a same class are normalized from [0..max_class] to [0..1]
    //! otherwise from [0..max_global]. Perclass can be used (or not) for
    //! discrete classification problems, but should be global for continuous
    //! labels.
    //! This normalization avoids looping on samples rarely picking any if
    //! all probabilities tend to zero for example.
    //! This is used only by next_train(), not by next().
    virtual void set_weigh_normalization(bool perclass);
    
    //! Return the number of classes.
    virtual intg get_nclasses();

    //! Return the lowest (non-zero) size per class, multiplied by the number
    //! of classes.
    //! e.g. if a dataset has 10 classes with 100 examples and 5 classes with
    //! 50 examples, it will return 50 * (10 + 5) = 750, whereas size()
    //! will return 1250.
    //! This is useful to keep iterations to a meaningful size when a class
    //! has many more examples than another.
    virtual intg get_lowest_common_size();
    
    //! Print info about the datasource on the standard output.
    virtual void pretty();

    //! Set this datasource to be a test datasource. This is optional but
    //! useful for reporting and to verify that no training only methods are
    //! called on this datasrouces (e.g. next_train()).
    void set_test();

    //! Returns true if this datasource is a test datasource only.
    bool is_test();
    
    ////////////////////////////////////////////////////////////////
    // members
  protected:
    intg                                        nclasses; // # of classes
    vector<intg>                                counts; // # of samples / class
  public:
    Tnet					bias;
    float					coeff;
    idx<Tin1>					data; // samples
    idx<Tin2>					labels; // labels
    idx<double>					probas;//!< sample probabilities
    // regular iterators
    typename idx<Tin1>::dimension_iterator	dataIter;
    typename idx<Tin2>::dimension_iterator	labelsIter;
    // test iterators
    typename idx<Tin1>::dimension_iterator	dataIter_test;
    typename idx<Tin2>::dimension_iterator	labelsIter_test;
    // training only iterators
    typename idx<Tin1>::dimension_iterator	dataIter_train;
    typename idx<Tin2>::dimension_iterator	labelsIter_train;
    typename idx<double>::dimension_iterator	probasIter_train;
    unsigned int				height;
    unsigned int				width;
    string					name;
  protected:
    bool					balance;
    vector<vector<intg> >                       label_indices;
    vector<uint>                                indices_itr;
    uint                                        iitr;
    // switches to activate or deactivate features
    bool                                        shuffle_passes;
    bool                                        weigh_samples;
    bool                                        perclass_norm;
    bool                                        test_set; //!< Is a test set.
    bool                                        discrete_labels;
    double                                      sample_min_proba;
  };

  ////////////////////////////////////////////////////////////////
  //! labeled_datasource
  template<class Tnet, class Tdata, class Tlabel>
    class labeled_datasource : public datasource<Tnet, Tdata, Tlabel> {
  public:
    vector<string*>				*lblstr;

    //! CAUTION: This empty constructor requires a subsequent call to init().
    labeled_datasource();

    //! initialize from matrices.
    void init(idx<Tdata> &inp, idx<Tlabel> &lbl, const char *name,
	      Tdata bias, float coeff, vector<string*> *lblstr);

    //! intialize from matrices filenames.
    void init(const char *data_fname, const char *labels_fname,
	      const char *classes_fname, const char *name,
	      Tdata bias = 0, float coeff = 1.0);

    //! Constructor takes all input data and corresponding labels.
    //! @param inputs: An N+1-dimensional idx of N-dimensional inputs.
    //! @param labels: A 1-dimensional idx of corresponding labels.
    //! @param lblstr: A vector of strings describing each label. When passed,
    //! this class takes ownership of the data and will destroy the vector and
    //! its content in the destructor.
    labeled_datasource(idx<Tdata> &inputs, idx<Tlabel> &labels, 
		      const char *name = NULL,
		      Tdata bias = 0, float coeff = 1.0,
		      vector<string*> *lblstr = NULL);

    labeled_datasource(idx<Tdata> &inputs, idx<Tlabel> &labels,
		       idx<ubyte> &classes, const char *name = NULL,
		       Tdata bias = 0, float coeff = 1.0);

    //! Constructor from data root and general name (appending names and
    //! extension of each subfile of a dataset created with the dscompiler).
    labeled_datasource(const char *root, const char *dsname,
		       const char *name = NULL,
		       Tdata bias = 0, float coeff = 1.0);
		       
    //! copy constructor
    labeled_datasource(const labeled_datasource<Tnet, Tdata, Tlabel> &ds);

    //! Destructor.
    virtual ~labeled_datasource();

    //! Print info about the datasource on the standard output.
    virtual void pretty();

    ////////////////////////////////////////////////////////////////
    // friends
    template <typename T1, typename T2, typename T3>
    friend class labeled_datasource_gui;
  };

  ////////////////////////////////////////////////////////////////
  //! labeled_pair_datasource
  template<class Tnet, class Tdata, class Tlabel>
    class labeled_pair_datasource
    : public labeled_datasource<Tnet, Tdata, Tlabel> {
  public:
    idx<intg>					pairs;
    typename idx<intg>::dimension_iterator	pairsIter;

    //! Constructor from dataset file names.
    labeled_pair_datasource(const char *data_fname, const char *labels_fname,
			    const char *classes_fname, const char *pairs_fname,
			    const char *name_ = NULL,
			    Tdata bias = 0, float coeff = 1.0);

    //! Constructor from dataset matrices.
    labeled_pair_datasource(idx<Tdata> &data_, idx<Tlabel> &labels_,
			    idx<ubyte> &classes_, idx<intg> &pairs_,
			    const char *name_ = NULL,
			    Tdata bias = 0, float coeff = 1.0);
    
    //! destructor.
    virtual ~labeled_pair_datasource();

    //! Copies the current datum to a state and label.
    virtual void fprop(state_idx<Tnet> &d1, state_idx<Tnet> &d2,
		       idx<Tlabel> &label);

    //! Move to the next datum.
    virtual void next();

    //! Move to the beginning of the data, for the test iterators only,
    //! i.e. only next() is affected, next_train() is unaffected.
    virtual void seek_begin();

    //! Returns the number of pairs contained in this data source.
    virtual unsigned int size();
  };

  ////////////////////////////////////////////////////////////////
  //! mnist_datasource
  //! a data source appropriate for most learning algorithms
  //! that take input data in the form of an idx3
  //! and a label in the form of an idx0 of L.
  //! This includes most supervised learning algorithms
  //! implemented in this library.
  template<class Tnet, class Tdata, class Tlabel>
    class mnist_datasource : public labeled_datasource<Tnet, Tdata, Tlabel> {
  public:
    
    //! Create an MNIST dataset using files found in root directory.
    //! type is either "t10k" or "train".
    //! size can go up to 10k for testing and 60k for training.
    mnist_datasource(const char *root, const char *type, uint size);

    //! Destructor.
    virtual ~mnist_datasource () {}

    //! init method.
    virtual void init(idx<Tdata> &inp, idx<Tlabel> &lbl, const char *name,
		      Tdata bias, float coeff);

    //! Returns an idxdim object describing the order (number of dimensions)
    //! and the size of each dimension of a single sample outputed by fprop.
    virtual idxdim sample_dims();

    //! get the current item and copy the sample into
    //! <out> (an idx3-state) and the corresponding
    //! label into <lbl> (and idx0 of int).
    virtual void fprop(state_idx<Tnet> &out, idx<Tlabel> &label);

  public:
    using datasource<Tnet, Tdata, Tlabel>::bias;
    using datasource<Tnet, Tdata, Tlabel>::coeff;
    using datasource<Tnet, Tdata, Tlabel>::height;
    using datasource<Tnet, Tdata, Tlabel>::width;
    using datasource<Tnet, Tdata, Tlabel>::data;
  };

  ////////////////////////////////////////////////////////////////
  // Helper functions
  
  //! Fill a matrix with 1-of-n code.
  //! Return an idx<double> with <nclasses> targets in it, where the target
  //! value for the correct answer is target and -target for the rest.
  template <class Tdata>
  idx<Tdata> create_target_matrix(intg nclasses, Tdata target);

} // end namespace ebl

#include "datasource.hpp"

#endif /* DATASOURCE_H_ */
