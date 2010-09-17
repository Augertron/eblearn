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

#ifndef EBL_TRAINER_H_
#define EBL_TRAINER_H_

#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_machines.h"
#include "ebl_logger.h"
#include "datasource.h"

namespace ebl {

/*   //////////////////////////////////////////////////////////////// */
/*   //! Generic Stochastic Gradient Trainer */
/*   template<class Tin1, class Tin2, class T> */
/*     class stochastic_gd_trainer { */
/*   private: */
/*     int		 iteration; */
/*     void	*iteration_ptr; */
/*     bool         prettied; */

/*   public: */
/*     fc_ebm2_gen<Tin1,Tin2,T>	&machine; */
/*     parameter<T>		&param; */
/*     fstate_idx<T>		 energy; */
/*     intg			 age; */
/*     Tin1			*in1; */
/*     Tin2			*in2; */
    
/*     stochastic_gd_trainer(fc_ebm2_gen<Tin1, Tin2, T> &m, parameter<T> &p); */
/*     virtual ~stochastic_gd_trainer(); */

/*     //! train for <niter> sweeps over the training set. <samples> contains the */
/*     //! inputs samples, and <labels> the corresponding desired categories */
/*     //! <labels>. */
/*     //! return the average energy computed on-the-fly. */
/*     //! <update-args> is a list of arguments for the parameter */
/*     //! update method (e.g. learning rate and weight decay). */
/*     //! if <compute_hessian> is true, then recompute the hessian matrix */
/*     //! every <hessian_interval> steps with the parameters <niter_hessian> */
/*     //! and <mu_hessian>. */
/*     void train(datasource<Tin1, Tin2> &ds, classifier_meter &log,  */
/* 	       gd_param &args, int niter, */
/* 	       bool compute_hessian, int hessian_interval, */
/* 	       int niter_hessian, double mu_hessian, intg max = 0); */

/*     //! train on one sample. */
/*     void train_sample(datasource<Tin1, Tin2> &ds, gd_param &args); */

/*     //! compute hessian */
/*     void compute_diaghessian(datasource<Tin1, Tin2> &ds, intg niter,  */
/* 			     double mu); */

/*     //! Resize <input> based on the datasource. If <input> is not allocated, */
/*     //! allocate it.  */
/*     //! TODO: If order or dimensions have changed, reallocate. */
/*     void resize_input(datasource<Tin1, Tin2> &ds); */

/*     template <class T1, class T2> */
/*       friend class stochastic_gd_trainer_gui; */
/*   }; */

  ////////////////////////////////////////////////////////////////
  //! Supervised Trainer. A specialisation of the generic trainer, taking
  //! samples (of type Tnet) and labels (of type Tlabel) as training input.
  //! Template Tnet is the network's type and also the input data's type.
  //! However datasources with different data type may be provided in which
  //! case a conversion will occur after each sample extraction from the
  //! datasource (via a deep idx_copy).
  template<class Tnet, class Tdata, class Tlabel>
    class supervised_trainer {
  public:
    //! constructor.
    supervised_trainer(fc_ebm2<Tnet,bbstate_idx<Tnet>,bbstate_idx<Tlabel> > &m,
		       parameter<Tnet, bbstate_idx<Tnet> > &p);

    //! destructor.
    virtual ~supervised_trainer();

    //! take an input and a vector of possible labels (each of which
    //! is a vector, hence <label-set> is a matrix) and
    //! return the index of the label that minimizes the energy
    //! fill up the vector <energies> with the energy produced by each
    //! possible label. The first dimension of <label-set> must be equal
    //! to the dimension of <energies>.
    void run(bbstate_idx<Tnet> &input, bbstate_idx<Tlabel> &label,
	     infer_param &infp);

    //! Test a single sample and its label <label> (an integer).
    //! Returns true if the sample was correctly classified, false otherwise.
    bool test_sample(bbstate_idx<Tnet> &input, bbstate_idx<Tlabel> &label,
		     infer_param &infp);

    //! perform a learning update on one sample. <sample> is the input
    //! sample, <label> is the desired category (an integer), <label-set> is
    //! a matrix where  the i-th row is the desired output
    //! for the i-th category, and <update-args> is a list of arguments
    //! for the parameter update method (e.g. learning rate and weight decay).
    double learn_sample(bbstate_idx<Tnet> &input, bbstate_idx<Tlabel> &label,
			gd_param &arg);

    //! Measure the average energy and classification error rate
    //! on a dataset.
    void test(labeled_datasource<Tnet, Tdata, Tlabel> &ds,
	      classifier_meter &log, infer_param &infp);

    /* //! Measures for each class the rates of true and false positives */
    /* //! (TPR and FPR) given a threshold. */
    /* //! The answer for each class is the default class (usually a junk class) */
    /* //! if the maximum response is below the threshold, otherwise it is the */
    /* //! class with maximum response. */
    /* //! TODO: use energy for detection instead of raw net outputs?(more generic) */
    /* void test_threshold(labeled_datasource<Tnet, Tdata, Tlabel> &ds, */
    /* 			classifier_meter &log, infer_param &infp, */
    /* 			double threshold, Tlabel defclass); */

    //! train for <niter> sweeps over the training set. <samples> contains the
    //! inputs samples, and <labels> the corresponding desired categories
    //! <labels>.
    //! return the average energy computed on-the-fly.
    //! <update-args> is a list of arguments for the parameter
    //! update method (e.g. learning rate and weight decay).
    void train(labeled_datasource<Tnet, Tdata, Tlabel> &ds,
	       classifier_meter &log, gd_param &args, int niter,
	       infer_param &infp);

    //! compute hessian
    void compute_diaghessian(labeled_datasource<Tnet, Tdata, Tlabel> &ds,
			     intg niter, double mu);

    //! init datasource to begining and assign indata to a buffer
    //! corresponding to ds's sample size. also increment iteration counter,
    //! unless new_iteration is false.
    void init(labeled_datasource<Tnet, Tdata, Tlabel> &ds,
	      classifier_meter *log = NULL, bool new_iteration = false);

    //! pretty some information about training, e.g. input and network sizes.
    void pretty(labeled_datasource<Tnet, Tdata, Tlabel> &ds);

    // friends
    // template <class Tdata, class Tlabel> friend class supervised_trainer_gui;
    template <class T1, class T2, class T3>
      friend class supervised_trainer_gui;

    //! returns a pointer to a copy on this datasource
    //    supervised_trainer<Tdata, Tlabel>* copy();
  public:
    fc_ebm2<Tnet,bbstate_idx<Tnet>,bbstate_idx<Tlabel> > &machine;
    parameter<Tnet, bbstate_idx<Tnet> >	  &param;	//!< the learned params
    //! net's tmp input buf. it is a pointer because the order of the input
    //! is not known in advance, and fstate_idx cannot change order dynamically.
    bbstate_idx<Tnet>	*input;
    bbstate_idx<Tnet>	 energy;//!< tmp energy buf
    bbstate_idx<Tlabel>	 label;
    bbstate_idx<Tlabel>	 answer;
    intg		 age;
  private:
    int			 iteration;
    void		*iteration_ptr;
    bool		 prettied;//!< flag used to pretty info just once
  };

} // namespace ebl {

#include "ebl_trainer.hpp"

#endif /* EBL_TRAINER_H_ */
