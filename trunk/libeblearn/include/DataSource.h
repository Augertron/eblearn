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

#include "libidx.h"
#include "Ebm.h"

namespace ebl {

  template<typename I, typename L> class LabeledDataSource {
  protected:
    Idx<I> *data;
    Idx<L> *labels;
    typename Idx<I>::dimension_iterator dataIter;
    typename Idx<L>::dimension_iterator labelsIter;

  public:
	//! Do not use, for subclasses using different kind of inputs
	LabeledDataSource(){};


    //! Constructor takes all input data and corresponding labels.
    //! @param inputs: An N+1-dimensional Idx of N-dimensional inputs.
    //! @param labels: A 1-dimensional Idx of corresponding labels.
    LabeledDataSource(Idx<I> *inputs, Idx<L> *labels);

    virtual ~LabeledDataSource() {};

    //! Copies the current datum to a state and label.
    void virtual fprop(state_idx *datum, Idx<L> *label);

    //! Returns the number of data instances contained in this data source.
    virtual int size();

    //! Returns the index of the datum currently pointed to.
    // TODO: implement or get rid of tell?
    virtual int tell(){return -1;};

    //! Move to the next datum.
    virtual void next();

    //! Move to the beginning of the data.
    virtual void seek_begin();
  };

  ////////////////////////////////////////////////////////////////

  //! a data source appropriate for most learning algorithms
  //! that take input data in the form of an idx3
  //! and a label in the form of an idx0 of L.
  //! This includes most supervised learning algorithms
  //! implemented in this library.
  template<class I, class L>
    class MnistDataSource : public LabeledDataSource<I, L> {
  public:
    intg width;
    intg height;
    double bias;
    double coeff;

    //! create a <dsource-mnist>.
    //! <inp> must be a ubyte-matrix of input patterns
    //! and <lbl> a ubyte-matrix of labels.
    //! <w> and <h> are the size of the output, in which
    //! the actual images will be centered.
    //! <bias> and <coeff> are used to shift and scale
    //! the values.
    MnistDataSource(Idx<I> *inp, Idx<L> *lbl, intg w, intg h, double b,
		    double c);
    virtual ~MnistDataSource () {}

    //! get the current item and copy the sample into
    //! <out> (an idx3-state) and the corresponding
    //! label into <lbl> (and idx0 of int).
    void virtual fprop(state_idx *out, Idx<L> *label);
  };

  ////////////////////////////////////////////////////////////////
  /*
  //! a data source constructed by taking patterns in
  //! an existing data source whose indices are within a
  //! given range.
  template<class I, class L>
  class DataSourceNarrow : public LabeledDataSource<I, L> {
  public:
  LabeledDataSource<I, L> *base;
  intg offset;
  intg size;

  //! make a new data source by taking <size> items
  //! from the data source passed as argument, starting
  //! at item <offset>.
  DataSourceNarrow(LabeledDataSource<I, L> *b, intg siz, intg off);

  virtual ~DataSourceNarrow () {}

  //! returns size given through constructor.
  intg size();

  //! copy current item and label into <out> and <lbl>.
  void fprop(state_idx &out, Idx<L> &label);
  };
  */

} // end namespace ebl

#include "DataSource.hpp"

#endif /* DATASOURCE_H_ */
