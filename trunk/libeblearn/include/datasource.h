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

#include "defines.h"
#include "libidx.h"
#include "ebl_states.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif 

using namespace std;

namespace ebl {

  template<typename Tdata, typename Tlabel> class labeled_datasource {
  public:
    double					 bias;
    double					 coeff;
    idx<Tdata>					 data;
    idx<Tlabel>					 labels;
    typename idx<Tdata>::dimension_iterator	 dataIter;
    typename idx<Tlabel>::dimension_iterator	 labelsIter;
    unsigned int				 height;
    unsigned int				 width;
    vector<string*>				*lblstr;
    const char					*name;
    unsigned int				 display_wid;

    //! CAUTION: This empty constructor requires a subsequent call to init().
    labeled_datasource();

    void init(idx<Tdata> &inp, idx<Tlabel> &lbl, double b, double c, 
	      const char *name, vector<string*> *lblstr);

    //! Constructor takes all input data and corresponding labels.
    //! @param inputs: An N+1-dimensional idx of N-dimensional inputs.
    //! @param labels: A 1-dimensional idx of corresponding labels.
    //! @param lblstr: A vector of strings describing each label. When passed,
    //! this class takes ownership of the data and will destroy the vector and
    //! its content in the destructor.
    labeled_datasource(idx<Tdata> &inputs, idx<Tlabel> &labels, 
		      double b = 0.0, double c = 0.01,
		      const char *name = NULL,
		      vector<string*> *lblstr = NULL);

    virtual ~labeled_datasource();

    //! Copies the current datum to a state and label.
    void virtual fprop(state_idx &datum, idx<Tlabel> &label);

    //! shuffle dataset, based on the number of classes
    //! assume the same nb of samples in each class
    void virtual shuffle();

    //! Returns the number of data instances contained in this data source.
    virtual int size();

    //! Returns an idxdim object describing the order (number of dimensions)
    //! and the size of each dimension of a single sample outputed by fprop.
    virtual idxdim sample_dims();

    //! Returns the index of the datum currently pointed to.
    // TODO: implement or get rid of tell?
    virtual int tell() { return -1; };

    //! Move to the next datum.
    virtual void next();

    //! Move to the beginning of the data.
    virtual void seek_begin();

    virtual void display(unsigned int nh, unsigned int nw, 
			 unsigned int h0 = 0, unsigned int w0 = 0, 
			 double zoom = 1.0, int wid = -1, 
			 const char *wname = NULL);

    virtual void draw(unsigned int nh, unsigned int nw, unsigned int h0 = 0, 
		      unsigned int w0 = 0, double zoom = 1.0);
  };

  ////////////////////////////////////////////////////////////////
  // mnist_datasource

  //! a data source appropriate for most learning algorithms
  //! that take input data in the form of an idx3
  //! and a label in the form of an idx0 of L.
  //! This includes most supervised learning algorithms
  //! implemented in this library.
  template<class Tdata, class Tlabel>
    class mnist_datasource : public labeled_datasource<Tdata, Tlabel> {
  public:
    double bias;
    double coeff;
    
    //! Empty constructor. CAUTION: Requires subsequent call to init().
    mnist_datasource() {};

    //! create a <dsource-mnist>.
    //! <inp> must be a ubyte-matrix of input patterns
    //! and <lbl> a ubyte-matrix of labels.
    //! <w> and <h> are the size of the output, in which
    //! the actual images will be centered.
    //! <bias> and <coeff> are used to shift and scale
    //! the values.
    mnist_datasource(idx<Tdata> &inp, idx<Tlabel> &lbl, double b, double c, 
		    const char *name = NULL);
    virtual ~mnist_datasource () {}

    virtual void init(idx<Tdata> &inp, idx<Tlabel> &lbl,
		      double b, double c, const char *name);

    //! Returns an idxdim object describing the order (number of dimensions)
    //! and the size of each dimension of a single sample outputed by fprop.
    virtual idxdim sample_dims();

    //! get the current item and copy the sample into
    //! <out> (an idx3-state) and the corresponding
    //! label into <lbl> (and idx0 of int).
    virtual void fprop(state_idx &out, idx<Tlabel> &label);

    virtual void display(unsigned int nh, unsigned int nw, 
			 unsigned int h0 = 0, unsigned int w0 = 0, 
			 double zoom = 1.0, int wid = -1,
			 const char *wname = NULL);
  };

  ////////////////////////////////////////////////////////////////
  // Helper functions
  
  //! look for mnist dataset in <directory> and load training and testing set
  //! into train_ds and test_ds.
  //! return true upon success.
  template<class Tdata, class Tlabel>
  bool load_mnist_dataset(const char *directory,
			  mnist_datasource<Tdata,Tlabel> &train_ds, 
			  mnist_datasource<Tdata,Tlabel> &test_ds,
			  int train_size, int test_size);

  //! Fill a matrix with 1-of-n code.
  //! Return an idx<double> with <nclasses> targets in it, where the target
  //! value for the correct answer is target and -target for the rest.
  idx<double> create_target_matrix(intg nclasses, double target);

  ////////////////////////////////////////////////////////////////
  /*
  //! a data source constructed by taking patterns in
  //! an existing data source whose indices are within a
  //! given range.
  template<class Tdata, class Tlabel>
  class DataSourceNarrow : public labeled_datasource<Tdata, Tlabel> {
  public:
  labeled_datasource<Tdata, Tlabel> *base;
  intg offset;
  intg size;

  //! make a new data source by taking <size> items
  //! from the data source passed as argument, starting
  //! at item <offset>.
  DataSourceNarrow(labeled_datasource<Tdata, Tlabel> *b, intg siz, intg off);

  virtual ~DataSourceNarrow () {}

  //! returns size given through constructor.
  intg size();

  //! copy current item and label into <out> and <lbl>.
  void fprop(state_idx &out, idx<Tlabel> &label);
  };
  */

} // end namespace ebl

#include "datasource.hpp"

#endif /* DATASOURCE_H_ */
