/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet  *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com   *
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

#ifndef TRAINER_H_
#define TRAINER_H_

#include "Net.h"
#include "DataSource.h"

namespace ebl {

// TODO: templatize classes for generic LabeledDataSource

//! Various learning algorithm classes are defined
//! to train learning machines. Learning machines are
//! generally subclasses of <gb-module>. Learning algorithm
//! classes include gradient descent for supervised
//! learning, and others.

//! Abstract class for energy-based learning algorithms.
//! The class contains an input, a (trainable) parameter,
//! and an energy. this is an abstract class from which actual
//! trainers can be derived.
class eb_trainer {
public:
	idx3_supervised_module 	*machine;
	parameter 							*param;
	state_idx								*input;
	state_idx								*energy;
	intg 										age;
	bool										input_owned;
	bool										energy_owned;

	eb_trainer(idx3_supervised_module *m, parameter *p,
			state_idx *e = NULL, state_idx *in = NULL);
	virtual ~eb_trainer();
};

////////////////////////////////////////////////////////////////

//! A an abstract trainer class for supervised training with of a
//! feed-forward classifier with discrete class labels. Actual
//! supervised trainers can be derived from this. The machine's fprop
//! method must have four arguments: input, output, energy, and
//! desired output. A call to the machine's fprop must look like this:
//! {<code>
//!    (==> machine fprop input output desired energy)
//! </code>}
//! By default, <output> must be a <class-state>, <desired> an
//! idx0 of int (integer scalar), and <energy> and <idx0-ddstate>
//! (or subclasses thereof).
//! The meter passed to the training and testing methods
//! should be a <classifier-meter>, or any meter whose
//! update method looks like this:
//! {<code>
//!    (==> meter update output desired energy)
//! </code>}
//! where <output> must be a <class-state>, <desired> an
//! idx0 of int, and <energy> and <idx0-ddstate>.
class supervised : public eb_trainer {
public:
	class_state *output;
	Idx<ubyte> 	*desired;
	bool				output_owned;
	bool				desired_owned;

	//! create a new <supervised> trainer. Arguments are as follow:
	//! {<ul>
	//!  {<li> <m>: machine to be trained.}
	//!  {<li> <p>: trainable parameter object of the machine.}
	//!  {<li> <e>: energy object (by default an idx0-ddstate).}
	//!  {<li> <in>: input object (by default an idx3-ddstate).}
	//!  {<li> <out>: output object (by default a class-state).}
	//!  {<li> <des>: desired output (by default an idx0 of int).}
	//! }
	supervised(idx3_supervised_module *m, parameter *p,
			state_idx *e = NULL, state_idx *in = NULL,
			class_state *out = NULL, Idx<ubyte> *des = NULL);
	virtual ~supervised();

	//! train the machine with on the data source <dsource> and
	//! measure the performance with <mtr>.
	//! This is a dummy method that should be defined
	//! by subclasses.
template<class T, class L>	void train(LabeledDataSource<T, L> *ds, classifier_meter *mtr);

	//! measures the performance over all the samples of data source <dsource>.
	//!<mtr> must be an appropriate meter.
template<class T, class L>	void test(LabeledDataSource<T, L> *ds, classifier_meter *mtr);

	//! measures the performance over a single sample of data source <dsource>.
	//! This leaves the internal state of the meter unchanged, and
	//! can be used for a quick test of a whether a particular pattern
	//! is correctly recognized or not.
template<class T, class L>	void test_sample(LabeledDataSource<T, L> *ds, classifier_meter *mtr, intg i);
};

////////////////////////////////////////////////////////////////

//! A basic trainer object for supervised stochastic gradient
//! training of a classifier with discrete class labels.
//! This is a subclass of <supervised>. The machine's
//! fprop method must have four arguments: input, output, energy,
//! and desired output. A call to the machine's fprop must
//! look like this:
//! {<code>
//!    (==> machine fprop input output desired energy)
//! </code>}
//! where <output> must be a <class-state>, <desired> an
//! idx0 of int (integer scalar), and <energy> and <idx0-ddstate>.
//! The meter passed to the training and testing methods
//! should be a <classifier-meter>, or any meter whose
//! update method looks like this:
//! {<code>
//!    (==> meter update output desired energy)
//! </code>}
//! where <output> must be a <class-state>, <desired> an
//! idx0 of int, and <energy> and <idx0-ddstate>.
//! The trainable parameter object must understand the following
//! methods:
//! {<ul>
//!  {<li> {<c> (==> param clear-dx)}: clear the gradients.}
//!  {<li> {<c> (==> param update eta inertia)}: update the parameters with
//!   learning rate <eta>, and momentum term <inertia>.}
//! }
//! If the diagonal hessian estimation is to be used, the param
//! object must also understand:
//! {<ul>
//!  {<li> {<c> (==> param clear-ddx)}: clear the second derivatives.}
//!  {<li> {<c> (==> param update-ddeltas knew kold)}: update average second
//!    derivatives.}
//!  {<li> {<c> (==> param compute-epsilons <mu>)}: set the per-parameter
//!          learning rates to the inverse of the sum of the second
//!          derivative estimates and <mu>.}
//! }
class supervised_gradient : public supervised {
public:
	//! create a new <supervised-gradient> trainer. Arguments are as follow:
	//! {<ul>
	//!  {<li> <m>: machine to be trained.}
	//!  {<li> <p>: trainable parameter object of the machine.}
	//!  {<li> <e>: energy object (by default an idx0-ddstate).}
	//!  {<li> <in>: input object (by default an idx3-ddstate).}
	//!  {<li> <out>: output object (by default a class-state).}
	//!  {<li> <des>: desired output (by default an idx0 of int).}
	//! }
	supervised_gradient(idx3_supervised_module *m, parameter *p,
			state_idx *e = NULL, state_idx *in = NULL,
			class_state *out = NULL, Idx<ubyte> *des = NULL);
	virtual ~supervised_gradient();

	//! train with stochastic (online) gradient on the next <n>
	//! samples of data source <dsource> with global learning rate <eta>.
	//! and "momentum term" <inertia>.
	//! Optionally maintain a running average of the weights with positive rate <kappa>.
	//! A negative value for kappa sets a rate equal to -<kappa>/<age>.
	//! No such update is performed if <kappa> is 0.
	//!
	//! Record performance in <mtr>.
	//! <mtr> must understand the following methods:
	//! {<code>
	//!   (==> mtr update age output desired energy)
	//!   (==> mtr info)
	//! </code>}
	//! where <age> is the number of calls to parameter updates so far,
	//! <output> is the machine's output (most likely a <class-state>),
	//! <desired> is the desired output (most likely an idx0 of int),
	//! and <energy> is an <idx0-state>.
	//! The <info> should return a list of relevant measurements.
	template<class T, class L>	void train_online(LabeledDataSource<T, L> *ds, classifier_meter *mtr,
			intg n, gd_param *gdp, double kappa);

	//! train the machine on all the samples in data source
	//! <dsource> and measure the performance with <mtr>.
	template<class T, class L>  void train(LabeledDataSource<T, L> *ds, classifier_meter *mtr,
			gd_param *gdp, double kappa = 0.0);

	//! Compute per-parameter learning rates (epsilons) using the
	//! stochastic diaginal levenberg marquardt method (as described in
	//! LeCun et al.  "efficient backprop", available at
	//! {<hlink> http://yann.lecun.com}).  This method computes positive
	//! estimates the second derivative of the objective function with respect
	//! to each parameter using the Gauss-Newton approximation.  <dsource> is
	//! a data source, <n> is the number of patterns (starting at the current
	//! point in the data source) on which the
	//! estimate is to be performed. Each parameter-specific
	//! learning rate epsilon_i is computed as 1/(H_ii + mu), where H_ii
	//! are the diagonal Gauss-Newton estimates and <mu> is the blowup
	//! prevention fudge factor.
	template<class T, class L>	void compute_diaghessian(LabeledDataSource<T, L> *ds, intg n, double mu);

	//! Compute the parameters saliencies as defined in the
	//! Optimal Brain Damage algorithm of (LeCun, Denker, Solla,
	//! NIPS 1989), available at http://yann.lecun.com.
	//! This computes the first and second derivatives of the energy
	//! with respect to each parameter averaged over the next <n>
	//! patterns of data source <ds>.
	//! A vector of saliencies is returned. Component <i> of the
	//! vector contains {<c> Si = -Gi * Wi + 1/2 Hii * Wi^2 }, this
	//! is an estimate of how much the energy would increase if the
	//! parameter was eliminated (set to zero).
	//! Parameters with small saliencies can be eliminated by
	//! setting their value and epsilon to zero.
	template<class T, class L>   void saliencies(LabeledDataSource<T, L> *ds, intg n);

	//! NOT FINISHED. compute optimal learning rate for on-line gradient
	// TODO
	/*
(defmethod supervised-gradient find-eta (ds n alpha gamma)
  (let ((w (idx-copy :param:x))
	(psi (idx-copy :param:dx))
	(phi (idx-sqrt :param:epsilons))
	(gradp (idx-copy :param:dx)))
    (repeat n
      (idx-add w psi :param:x)
      (==> ds fprop input desired)
      (==> machine fprop input output desired energy)
      (==> param clear-dx)
      (==> machine bprop input output desired energy)
      (idx-copy :param:dx gp)

      (idx-copy w :param:x)
      (==> ds fprop input desired)
      (==> machine fprop input output desired energy)
      (==> param clear-dx)
      (==> machine bprop input output desired energy)

      //! some stuff goes here

      (==> ds next))) ())
	 */
};

} // end namespace ebl

#include "Trainer.hpp"

#endif /*TRAINER_H_*/
