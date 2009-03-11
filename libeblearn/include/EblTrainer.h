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
#include "EblLogger.h"
#include "DataSource.h"

#ifdef __GUI__
#include "libidxgui.h"
#endif 

namespace ebl {

  ////////////////////////////////////////////////////////////////
  //! Generic Trainer
  // TODO: templated generic trainer class

  ////////////////////////////////////////////////////////////////
  //! Supervised Trainer
  template<class Tdata, class Tlabel>
    class supervised_trainer {
  private:
    bool		display;
    unsigned int	display_nh;
    unsigned int	display_nw;
    unsigned int	display_h0;
    unsigned int	display_w0;
    double		display_zoom;
    unsigned int	display_wid;
    unsigned int	iteration;

  public:
    fc_ebm2<state_idx,int,state_idx>	&machine;
    parameter				&param;
    state_idx				*input;
    state_idx				 energy;
    Idx<Tlabel>				 label;
    intg				 age;
    ostream				&cout;
    
    supervised_trainer(fc_ebm2<state_idx,int,state_idx> &m, 
		       parameter &p, ostream& cout = std::cout);
    virtual ~supervised_trainer();

    void set_display(unsigned int nh, unsigned int nw, unsigned int h0 = 0, 
		     unsigned int w0 = 0, double zoom = 1.0, int wid = -1,
		     const char *title = NULL);

    //! take an input and a vector of possible labels (each of which
    //! is a vector, hence <label-set> is a matrix) and
    //! return the index of the label that minimizes the energy
    //! fill up the vector <energies> with the energy produced by each
    //! possible label. The first dimension of <label-set> must be equal
    //! to the dimension of <energies>.
    int run(state_idx &input, infer_param &infp);

    //! Test a single sample and its label <label> (an integer).
    //! Returns true if the sample was correctly classified, false otherwise.
    bool test_sample(state_idx &input, int label, int &answer, 
		     infer_param &infp);

    //! perform a learning update on one sample. <sample> is the input
    //! sample, <label> is the desired category (an integer), <label-set> is
    //! a matrix where  the i-th row is the desired output
    //! for the i-th category, and <update-args> is a list of arguments
    //! for the parameter update method (e.g. learning rate and weight decay).
    Idx<double> learn_sample(state_idx &input, int label, gd_param &arg);

    //! Measure the average energy and classification error rate
    //! on a dataset.
    //! returns a list with average loss and proportion of errors
    void test(LabeledDataSource<Tdata, Tlabel> &ds, classifier_meter &log,
	      infer_param &infp);

    //! train for <niter> sweeps over the training set. <samples> contains the
    //! inputs samples, and <labels> the corresponding desired categories
    //! <labels>.
    //! return the average energy computed on-the-fly.
    //! <update-args> is a list of arguments for the parameter
    //! update method (e.g. learning rate and weight decay).
    void train(LabeledDataSource<Tdata, Tlabel> &ds, classifier_meter &log, 
	       gd_param &args, int niter);

    //! compute hessian
    void compute_diaghessian(LabeledDataSource<Tdata, Tlabel> &ds, intg niter, 
			     double mu);

    //! Resize <input> based on the datasource. If <input> is not allocated,
    //! allocate it. 
    //! TODO: If order or dimensions have changed, reallocate.
    void resize_input(LabeledDataSource<Tdata, Tlabel> &ds);
  };

} // namespace ebl {

#include "EblTrainer.hpp"

#endif /* EBLTRAINER_H_ */
