/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef EBLTRAINER_H_
#define EBLTRAINER_H_

#include "Blas.h"
#include "EblArch.h"
#include "EblMachines.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // trainer

  class trainer {
  public:
    trainable_machine<state_idx,state_idx,state_idx>	&tmachine;
    parameter					        &param;
    Idx<double> *input;
    
    trainer(trainable_machine<state_idx,state_idx,state_idx> &tm, parameter &p);
    virtual ~trainer();

    //! take an input and a vector of possible labels (each of which
    //! is a vector, hence <label-set> is a matrix) and
    //! return the index of the label that minimizes the energy
    //! fill up the vector <energies> with the energy produced by each
    //! possible label. The first dimension of <label-set> must be equal
    //! to the dimension of <energies>.
    intg run(Idx<double> &sample, Idx<double> &energies);

    //! Test a single sample and its label <label> (an integer), and
    //! return a list with the energy for the correct label and an
    //! integer which is equal to 1 if the sample was incorrectly classified
    //! and 0 if it was correctly classified.
    //! <label-set> is a matrix where the i-th row is the desired output
    //! for the i-th category.
    bool test_sample(Idx<double> &sample, Idx<double> &label);

    //! perform a learning update on one sample. <sample> is the input
    //! sample, <label> is the desired category (an integer), <label-set> is
    //! a matrix where  the i-th row is the desired output
    //! for the i-th category, and <update-args> is a list of arguments
    //! for the parameter update method (e.g. learning rate and weight decay).
    Idx<double> learn_sample(Idx<double> &sample, Idx<double> &label,
			     gd_param &arg);

    //! Measure the average energy and classification error rate
    //! on a dataset. <samples> is a matrix that contains the
    //! training samples, <labels> contains the desired categories,
    //! and <label-set> is a matrix whose rows are
    //! the desired output for each category. This
    //! returns a list with average loss and proportion of errors
    Idx<double>	test(Idx<double> &samples, Idx<double> &labels);

    //! train for <niter> sweeps over the training set. <samples> contains the
    //! inputs samples, and <labels> the corresponding desired categories
    //! <labels>.
    //! return the average energy computed on-the-fly.
    //! <update-args> is a list of arguments for the parameter
    //! update method (e.g. learning rate and weight decay).
    double train(Idx<double> &samples, Idx<double> &label,
		 int niter, gd_param &arg);
  };

} // namespace ebl {

#endif /* EBLTRAINER_H_ */
