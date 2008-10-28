/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun   *
 *   yann@cs.nyu.edu   *
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

#ifndef EBM_H_
#define EBM_H_

#include "Idx.h"
#include "Blas.h"

namespace ebl {

/*! \mainpage libeblearn Library Main Page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: TODO
 *  
 * TODO
 */

//! see numerics.h for description
extern bool drand_ini;

void err_not_implemented();

class infer_param {
};

//! class that contains all the parameters
//! for a stochastic gradient descent update,
//! including step sizes, regularizer coefficients...
class gd_param: public infer_param {
public:
	//! global step size
	double eta;
	//! time at which to start using decay values
	int decay_time;
	//! L2 regularizer coefficient
	double decay_l2;
	//! L1 regularizer coefficient
	double decay_l1;
	//! stopping criterion threshold
	double n;
	//! momentum term
	double inertia;
	//! annealing coefficient for the learning rate
	double anneal_value;
	//! number of iteration beetween two annealings
	double anneal_time;
	//! threshold on square norm of gradient for stopping
	double gradient_threshold;
	//! for debugging purpose
	int niter_done;

	gd_param(double leta, double ln, double l1, double l2, int dtime,
			double iner, double a_v, double a_t, double g_t);

};

////////////////////////////////////////////////////////////////

//! abstract class for randomization parameters
class forget_param {
};

class forget_param_linear: public forget_param {
public:
	//! each random value will be drawn uniformly
	//! from [-value/(fanin**exponent), +value/(fanin**exponent)]
	double value;
	double exponent;

	//! constructor.
	//! each random value will be drawn uniformly
	//! from [-v/(fanin**exponent), +v/(fanin**exponent)]
	forget_param_linear(double v, double e);
};

////////////////////////////////////////////////////////////////

//! abstract class that stores a state.
//! it must support the following methods
//! clear (clear values), clear_dx (clear gradients),
//! clear_ddx (clear hessian), and update_gd(arg) (update
//! with gradient descent.
class state {

public:

	virtual void clear();
	virtual void clear_dx();
	virtual void clear_ddx();
	virtual void update_gd(gd_param &arg);

	state();

	virtual ~state();
};

class parameter;

//! class that stores a vector/tensor state
class state_idx: public state {
public:
	//! state itself
	Idx<double> x;
	//! gradient of loss with respect to state
	Idx<double> dx;
	//! diag hessian of loss with respect to state
	Idx<double> ddx;

	//! Constructs a state_idx of order 0
	state_idx();
	//! Constructs a state_idx of order 1
	state_idx(intg s0);
	//! Constructs a state_idx of order 2
	state_idx(intg s0, intg s1);
	//! Constructs a state_idx of order 3
	state_idx(intg s0, intg s1, intg s2);
	//! Constructor. A state_idx can have up to 8 dimensions.
	state_idx(intg s0, intg s1, intg s2, intg s3, intg s4 = -1, intg s5 = -1,
			intg s6 = -1, intg s7 = -1);

	//! this appends the state_idx into the same Srg as the
	//! state_idx passed as argument. This is useful for
	//! allocating multiple state_idx inside a parameter.
	//! This replaces the Lush function alloc_state_idx.
	state_idx(parameter *st);
	state_idx(parameter *st, intg s0);
	state_idx(parameter *st, intg s0, intg s1);
	state_idx(parameter *st, intg s0, intg s1, intg s2);
	state_idx(parameter *st, intg s0, intg s1, intg s2, intg s3, intg s4 = -1,
			intg s5 = -1, intg s6 = -1, intg s7 = -1);

	virtual ~state_idx();

	//! clear x
	virtual void clear();

	//! clear gradients dx
	virtual void clear_dx();

	//! clear diag hessians ddx
	virtual void clear_ddx();

	//! return number of elements
	virtual intg nelements();

	//! return footprint in storages
	virtual intg footprint();

	//! same as footprint
	virtual intg size();

	//! update with gradient descent
	virtual void update_gd(gd_param &arg);

	//! resize. The order cannot be changed with this.
	virtual void resize(intg s0 = -1, intg s1 = -1, intg s2 = -1, intg s3 = -1,
			intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 = -1);

	virtual void resizeAs(state_idx &s);

	virtual void resize(const intg* dimsBegin, const intg* dimsEnd);

	//! make a new copy of self
	virtual state_idx make_copy();
};

////////////////////////////////////////////////////////////////

//! parameter: the main class for a trainable
//! parameter vector.
class parameter: public state_idx {
public:
	Idx<double> gradient;
	Idx<double> deltax;
	Idx<double> epsilons;
	Idx<double> ddeltax;

	//! constructor
	parameter(intg initial_size = 100);
	virtual ~parameter();

	virtual void resize(intg s0);
	void update_gd(gd_param &arg);
	virtual void update(gd_param &arg);
	void clear_deltax();
	void update_deltax(double knew, double kold);
	void clear_ddeltax();
	void update_ddeltax(double knew, double kold);
	void set_epsilons(double m);
	void compute_epsilons(double mu);
	bool load(const char *s);
	void save(const char *s);
};

////////////////////////////////////////////////////////////////
// templates for generic modules

//! abstract class for a module with one input and one output.
template<class Tin, class Tout> class module_1_1 {
public:
	virtual ~module_1_1() {
	}
	virtual void fprop(Tin *in, Tout *out);
	virtual void bprop(Tin *in, Tout *out);
	virtual void bbprop(Tin *in, Tout *out);
	virtual void forget(forget_param_linear& fp);
	virtual void normalize();
};

////////////////////////////////////////////////////////////////

//! abstract class for a module with two inputs and one output.
template<class Tin1, class Tin2, class Tout> class module_2_1 {
public:
	virtual ~module_2_1() {
	}
	;
	virtual void fprop(Tin1 *in1, Tin2 *in2, Tout *out);
	virtual void bprop(Tin1 *in1, Tin2 *in2, Tout *out);
	virtual void bbprop(Tin1 *in1, Tin2 *in2, Tout *out);
	virtual void forget(forget_param &fp);
	virtual void normalize();
};

////////////////////////////////////////////////////////////////

//! abstract class for a module with one inputs and one energy output.
template<class Tin> class ebm_1 {
public:
	virtual ~ebm_1() {
	}
	;
	virtual void fprop(Tin *in, state_idx *energy);
	virtual void bprop(Tin *in, state_idx *energy);
	virtual void bbprop(Tin *in, state_idx *energy);
	virtual void forget(forget_param &fp);
	virtual void normalize();
};

////////////////////////////////////////////////////////////////

//! abstract class for a module with two inputs and one energy output.
template<class Tin1, class Tin2> class ebm_2 {
public:
	virtual ~ebm_2() {
	}
	;
	//! fprop: compute output from input
	virtual void fprop(Tin1 *i1, Tin2 *i2, state_idx *energy);
	//! bprop: compute gradient wrt inputs, given gradient wrt output
	virtual void bprop(Tin1 *i1, Tin2 *i2, state_idx *energy);
	//! bprop: compute diaghession wrt inputs, given diaghessian wrt output
	virtual void bbprop(Tin1 *i1, Tin2 *i2, state_idx *energy);

	virtual void bprop1_copy(Tin1 *i1, Tin2 *i2, state_idx *energy);
	virtual void bprop2_copy(Tin1 *i1, Tin2 *i2, state_idx *energy);
	virtual void bbprop1_copy(Tin1 *i1, Tin2 *i2, state_idx *energy);
	virtual void bbprop2_copy(Tin1 *i1, Tin2 *i2, state_idx *energy);
	virtual void forget(forget_param &fp);
	virtual void normalize();

	//! compute value of in1 that minimizes the energy, given in2
	virtual double infer1(Tin1 *i1, Tin2 *i2, state_idx *energy,
			infer_param *ip) {
		return 0;
	}
	//! compute value of in2 that minimizes the energy, given in1
	virtual double infer2(Tin1 *i1, Tin2 *i2, state_idx *energy,
			infer_param *ip) {
		return 0;
	}
};

////////////////////////////////////////////////////////////////
// generic architectures

template<class Tin, class Thid, class Tout> class layers_2: public module_1_1<
		Tin, Tout> {
public:
	module_1_1<Tin, Thid> *layer1;
	Thid *hidden;
	module_1_1<Thid, Tout> *layer2;

	layers_2(module_1_1<Tin, Thid> *l1, Thid *h, module_1_1<Thid, Tout> *l2);
	virtual ~layers_2();
	void fprop(Tin *in, Tout *out);
	void bprop(Tin *in, Tout *out);
	void bbprop(Tin *in, Tout *out);
	void forget(forget_param &fp);
	void normalize();
};

template<class T> class layers_n: public module_1_1<T, T> {
public:
	std::vector< module_1_1<T, T>* > *modules;
	std::vector< T* > *hiddens;

	layers_n();
	layers_n(bool oc);
	virtual ~layers_n();
	void addModule(module_1_1 <T, T>* module, T* hidden);
	void fprop(T *in, T *out);
	void bprop(T *in, T *out);
	void bbprop(T *in, T *out);
	void forget(forget_param_linear &fp);
	void normalize();
private:
	bool own_contents;
};

////////////////////////////////////////////////////////////////
//

//! standard 1 input EBM with one module-1-1, and one ebm-1 on top.
//! fc stands for "function+cost".
template<class Tin, class Thid> class fc_ebm1: public ebm_1<Tin> {
public:
	module_1_1<Tin, Thid> *fmod;
	Thid *fout;
	ebm_1<Thid> *fcost;

	fc_ebm1(module_1_1<Tin, Thid> *fm, Thid *fo, ebm_1<Thid> *fc);
	virtual ~fc_ebm1();

	void fprop(Tin *in, state_idx *energy);
	void bprop(Tin *in, state_idx *energy);
	void bbprop(Tin *in, state_idx *energy);
	void forget(forget_param &fp);
};

////////////////////////////////////////////////////////////////

//! standard 2 input EBM with one module-1-1, and one ebm-2 on top.
//! fc stands for "function+cost".
template<class Tin1, class Tin2, class Thid> class fc_ebm2: public ebm_2<Tin1,
		Tin2> {
public:
	module_1_1<Tin1, Thid> *fmod;
	Thid *fout;
	ebm_2<Thid, Tin2> *fcost;

	fc_ebm2(module_1_1<Tin1, Thid> *fm, Thid *fo, ebm_2<Thid, Tin2> *fc);
	virtual ~fc_ebm2();

	void fprop(Tin1 *in1, Tin2 *in2, state_idx *energy);
	void bprop(Tin1 *in1, Tin2 *in2, state_idx *energy);
	void bbprop(Tin1 *in1, Tin2 *in2, state_idx *energy);
	void forget(forget_param &fp);
};

////////////////////////////////////////////////////////////////
// linear module
// It's different from f_layer in that it is
// not spatially replicable and does not operate
// on 3D state_idx.

class linear_module: public module_1_1<state_idx, state_idx> {
public:
	state_idx *w;

	virtual ~linear_module();
	linear_module(parameter *p, intg in0, intg out0);
	void fprop(state_idx *in, state_idx *out);
	void bprop(state_idx *in, state_idx *out);
	void bbprop(state_idx *in, state_idx *out);
	void forget(forget_param_linear &fp);
	void normalize();
};

////////////////////////////////////////////////////////////////

//! a slab of standard Lush sigmoids
class stdsigmoid_module: public module_1_1<state_idx, state_idx> {
public:
	//! empty constructor
	stdsigmoid_module();
	virtual ~stdsigmoid_module();
	//! fprop from in to out
	virtual void fprop(state_idx *in, state_idx *out);
	//! bprop
	virtual void bprop(state_idx *in, state_idx *out);
	//! bbprop
	virtual void bbprop(state_idx *in, state_idx *out);
};

////////////////////////////////////////////////////////////////

//! a slab of tanh
class tanh_module: public module_1_1<state_idx, state_idx> {
public:
	//! fprop from in to out
	void fprop(state_idx *in, state_idx *out);
	//! bprop
	void bprop(state_idx *in, state_idx *out);
	//! bbprop
	void bbprop(state_idx *in, state_idx *out);
	void forget(forget_param_linear &fp);
	void normalize();
};

////////////////////////////////////////////////////////////////

//! constant add
class addc_module: public module_1_1<state_idx, state_idx> {
public:
	// coefficients
	state_idx* bias;
	addc_module(parameter *p, intg size);
	~addc_module();
	//! fprop from in to out
	void fprop(state_idx *in, state_idx *out);
	//! bprop
	void bprop(state_idx *in, state_idx *out);
	//! bbprop
	void bbprop(state_idx *in, state_idx *out);
	void forget(forget_param_linear &fp);
	void normalize();
};



////////////////////////////////////////////////////////////////

//! a simple fully-connected neural net layer: linear + tanh non-linearity.
//! Unlike the f-layer class, this one is not spatially replicable.
class nn_layer_full: public module_1_1<state_idx, state_idx> {
public:
	//! linear module for weight matrix
	linear_module *linear;
	//! bias vector
	state_idx *bias;
	//! weighted sum
	state_idx *sum;
	//! the non-linear function
	tanh_module *sigmoid;

	//! constructor. Arguments are a pointer to a parameter
	//! in which the trainable weights will be appended,
	//! the number of inputs, and the number of outputs.
	nn_layer_full(parameter *p, intg ninputs, intg noutputs);
	virtual ~nn_layer_full();
	//! fprop from in to out
	void fprop(state_idx *in, state_idx *out);
	//! bprop
	void bprop(state_idx *in, state_idx *out);
	//! bbprop
	void bbprop(state_idx *in, state_idx *out);
	//! initialize the weights to random values
	void forget(forget_param_linear &fp);
};

////////////////////////////////////////////////////////////////

//! full connection between replicable 3D layers
//! the full connection is only across the first dimension
//! of the input and output layer.
//! the other two dimensions are spatial dimensions accross which the
//! weight matrix is shared. This is much more efficient than using
//! a c-layer with a 1x1 convolution and a full-table.
class f_layer: public module_1_1<state_idx, state_idx> {
public:
	//! weight matrix
	state_idx *weight;
	//! bias vector
	state_idx *bias;
	//! weighted sums
	state_idx *sum;
	//! squashing function module
	module_1_1<state_idx, state_idx> *squash;

	virtual ~f_layer();
	//! constructor. Arguments are:
	//! p: parameter object in which the trainable parameters will be allocated,
	//! tin: number of slices of the input
	//! tout: number of slices of the output
	//! si: initial size in first spatial dimension
	//! sj: initial size in decond spatial dimension
	//! sq: pointer to squashing module
	f_layer(parameter *p, intg tin, intg tout, intg si, intg sj, module_1_1<
			state_idx, state_idx> *sq);
	//! fprop from in to out
	void fprop(state_idx *in, state_idx *out);
	//! bprop
	void bprop(state_idx *in, state_idx *out);
	//! bbprop
	void bbprop(state_idx *in, state_idx *out);
	//! initialize the weights to random values
	void forget(forget_param_linear &fp);
};

////////////////////////////////////////////////////////////////

//! convolutional layer module. Performs multiple convolutions
//! between an idx3-state input and an idx3-state output.
//! includes bias and sigmoid.
class c_layer: public module_1_1<state_idx, state_idx> {
public:
	//! thickness of output layer (number of feature maps)
	int thickness;
	//! vertical size for preallocation of internal state
	int stridei;
	//! horizontal size for preallocation of internal state
	int stridej;
	//! N by 2 matrix containing source and destination
	//! feature maps for coresponding kernel
	Idx<intg> *table;
	//! convolution kernel
	state_idx *kernel;
	//! bias vector
	state_idx *bias;
	//! weighted sum
	state_idx *sum;
	//! squashing function module
	module_1_1<state_idx, state_idx> *squash;

	virtual ~c_layer();
	//! constructor. Arguments are:
	//! ki: vertical kernel size.
	//! kj: horizontal kernel size.
	//! ri: vertical stride (number of pixels by which the kernels are stepped)
	//! rj: horizontal stride
	//! tbl: N by 2 matrix containing source and destination
	//!   feature maps for coresponding kernel
	//! thick: thickness of output layer (number of feature maps)
	//! si: vertical size for preallocation of internal state
	//! sj: horizontal size for preallocation of internal state
	//! sqsh: a squashing function module that operates on idx3-state.
	//! prm: an idx1-ddparam from which the parameters will be allocated
	c_layer(parameter *prm, intg ki, intg kj, intg ri, intg rj, Idx<intg> *tbl,
			intg thick, intg si, intg sj,
			module_1_1<state_idx, state_idx> *sqsh);
	//! set the convolution stride
	void set_stride(intg ri, intg rj);
	//! initialize the weights to random values
	void forget(forget_param_linear &fp);
	//! fprop from in to out
	virtual void fprop(state_idx *in, state_idx *out);
	//! bprop
	virtual void bprop(state_idx *in, state_idx *out);
	//! bbprop
	virtual void bbprop(state_idx *in, state_idx *out);
};

////////////////////////////////////////////////////////////////

#ifdef USE_IPP

#include "Ipp.h"

//! same as c_layer class but using Intel IPP functions for optimization.
class c_layer_ipp : public c_layer
{
public:
	//! constructor. Arguments are:
	//! ki: vertical kernel size.
	//! kj: horizontal kernel size.
	//! ri: vertical stride (number of pixels by which the kernels are stepped)
	//! rj: horizontal stride
	//! tbl: N by 2 matrix containing source and destination
	//!   feature maps for coresponding kernel
	//! thick: thickness of output layer (number of feature maps)
	//! si: vertical size for preallocation of internal state
	//! sj: horizontal size for preallocation of internal state
	//! sqsh: a squashing function module that operates on idx3-state.
	//! prm: an idx1-ddparam from which the parameters will be allocated
	c_layer_ipp(parameter *prm, intg ki, intg kj, intg ri, intg rj, Idx<intg> *tbl,
			intg thick, intg si, intg sj, module_1_1<state_idx,state_idx> *sqsh)
	: c_layer(prm, ki, kj, ri, rj, tbl, thick, si, sj, sqsh)
	{}
	//! fprop
	virtual void fprop(state_idx *in, state_idx *out);
	//! bprop
	virtual void bprop(state_idx *in, state_idx *out);
};

#endif

////////////////////////////////////////////////////////////////

//! Creates a table of full connections between layers.
//! An Idx<intg> is allocated and returned. The caller is responsible
//! for deleting this Idx.
Idx<intg> full_table(intg a, intg b);

////////////////////////////////////////////////////////////////

//! subsampling layer class
class s_layer: public module_1_1<state_idx, state_idx> {
public:
	intg stridei, stridej;
	state_idx *coeff;
	state_idx *bias;
	state_idx *sub;
	state_idx *sum;
	module_1_1<state_idx, state_idx> *squash;

	//! constructor arguments are:
	//! <prm> pointer to a "parameter" object, from which the parameters will be allocated
	//! <ki>  vertical subsampling ratio.
	//! <kj>  horizontal subsampling ratio.
	//! <thick> thickness of output layer (number of feature maps)
	//! <si>  vertical size for preallocation of internal state
	//! <sj>  horizontal size for preallocation of internal state
	//! <sqsh> pointer to a squashing function module that operates
	s_layer(parameter *p, intg ki, intg kj, intg thick, intg si, intg sj,
			module_1_1<state_idx, state_idx> *sqsh);
	virtual ~s_layer();
	void fprop(state_idx *in, state_idx *out);
	void bprop(state_idx *in, state_idx *out);
	void bbprop(state_idx *in, state_idx *out);
	void forget(forget_param_linear &fp);
};

////////////////////////////////////////////////////////////////

//! performs a log-add over spatial dimensions of an idx3-state
//! output is an idx1-state
class logadd_layer { //: public module_1_1<state_idx, state_idx> { // TODO
public:
	Idx<double> expdist;
	Idx<double> sumexp;

	logadd_layer(intg thick, intg si, intg sj);
	virtual ~logadd_layer() {
	}
	void fprop(state_idx *in, state_idx *out);
	void bprop(state_idx *in, state_idx *out);

	//! this is not algebraically correct, but it's
	//! numerically more stable (at least, I think so).
	void bbprop(state_idx *in, state_idx *out);
};

////////////////////////////////////////////////////////////////

// TODO: for all classes below, templatize type of classes (currently ubyte).

//! a replicable Euclidean distance cost function.
//! computes the log-sum over the 2D spatial output of
//! the half squared error between the output and the
//! prototype with the desired label.
//! this does not generate gradients on the prototypes
class edist_cost { //: public module_1_1<state_idx, state_idx> { // TODO
public:
	logadd_layer *logadder;
	state_idx *dist;
	state_idx *logadded_dist;
	Idx<double> *proto;
	Idx<ubyte> label2classindex;

	//! make a new edist-cost. <classes> is an integer vector
	//! which contains the labels associated with each output.
	//! From that vector, the reverse table is constructed
	//! to map labels to class indices.
	//! Elements in <classes> must be positive or 0, and
	//! not be too large, as a table as large as the
	//! maximum value is allocated.
	//! <si> and <sj> are the expected maximum sizes in
	//! the spatial dimensions (used for preallocation to
	//! prevent memory fragmentation).
	//! <p> is an idx2 containing the prototype for each
	//! class label. The first dimension of <p> should be
	//! equal to the dimension of <classes>.
	//! the second dimension of <p> should be equal to the
	//! number of outputs of the previous module.
	//! The costs are "log-summed" over spatial dimensions
	edist_cost(Idx<ubyte> *classes, intg ini, intg inj, Idx<double> *p);

	virtual ~edist_cost() {
	}
	virtual void fprop(state_idx *in, Idx<ubyte> *desired, state_idx *energy);
	virtual void bprop(state_idx *in, Idx<ubyte> *desired, state_idx *energy);
	virtual void bbprop(state_idx *in, Idx<ubyte> *desired, state_idx *energy);
};

////////////////////////////////////////////////////////////////

//! a special kind of state used to store the output of a classifier.
//! class-state are generated by modules such as class-max, and used
//! by meters such as classifier-meter. No backprop is possible through
//! a class-state.
class class_state {
public:
	ubyte output_class;
	float confidence;
	Idx<ubyte> *sorted_classes;
	Idx<float> *sorted_scores;

	class_state(ubyte n);
	~class_state();
	void resize(ubyte n);
};

////////////////////////////////////////////////////////////////

//! Meters are classes used to measure the performance
//! of learning machines. There are several types
//! of meters for each specific situation.
//! meters are generally assumed to have at least
//! the following methods:
//! {<ul>
//!  {<li> update: updates the meter with the objects
//!        and values passed as argument.}
//!  {<li> clear: resets the meter, so it can be used
//!        for a new series of measurements.}
//!  {<li> test: simply prints performance information
//!        for the data passed as argument. This
//!        does not update any internal state.}
//! }
//! Methods are provided to compute and display the
//! information measured by a meter.
//! {<ul>
//!  {<li> display: display performance information on the terminal}
//!  {<li> info: returns a list of the informations printed by display}
//! }

//! a class that can be used to measure the performance of
//! classifiers. This is a simple version that does not
//! record anything but simply computes performance measures.
// TODO: allow definition of different comparison functions.
class classifier_meter {
public:
	double energy;
	float confidence;
	intg size;
	intg age;
	intg total_correct;
	intg total_error;
	intg total_punt;
	intg total_energy;

	//! Create a new <classifier-meter> using <comparison-function>
	//! to compare actual and desired answers. By default
	//! the <same-class?> function is used for that purpose.
	//! It takes two integer arguments, and returns 1 if they
	//! are equal, -1 if they are different, and 0 if
	//! the first argument is -1 (which means reject).
	classifier_meter(); // TODO: allow passing of comparison function
	~classifier_meter() {
	}
	;

	//! return 0 if <actual> equals -1, otherwise, return 1 if <actual>
	//! and <desired> are equal, -1 otherwise.
	int correctp(ubyte co, ubyte cd);

	//! reset the meter. This must be called
	//! before a measurement campaign is started.
	void clear();
	void resize(intg sz);

	//! update the meter with results from a new sample.
	//! <age> is the number of training iterations so far,
	//! <actual> (a <class-state>) the actual output of the machine,
	//! <desired> (an idx0 of int) the desired category,
	//! and <energy> (an idx0-state) the energy.
	char update(intg a, class_state *co, ubyte cd, state_idx *en);

	void test(class_state *co, ubyte cd, state_idx *en);

	//! return a list with the age, the number of samples
	//! (number of calls to update since the last clear),
	//! the average energy, the percentage of correctly
	//! recognize samples, the percentage of erroneously
	//! recognized samples, and the percentage of rejected samples.
	void info();
	void info_sprint();
	void info_print();

	//! Display the meter's information on the terminal.
	//! namely, the age, the number of samples
	//! (number of calls to update since the last clear),
	//! the average energy, the percentage of correctly
	//! recognize samples, the percentage of erroneously
	//! recognized samples, and the percentage of rejected samples.
	void display();
	bool save();
	bool load();
};

////////////////////////////////////////////////////////////////

//! a module that takes an state_idx, finds the lowest value
//! and output the label associated with the index (in the first dimension
//! of the state) of this lowest value.
//! It actually sorts the labels according to their score (or costs)
//! and outputs the sorted list.
class max_classer { // TODO: idx3-classer
public:
	Idx<ubyte> *classindex2label; //! a vector that maps output unit index to a label

	//! makes a new max-classer. <classes> is an integer vector
	//! which contains the labels associated with each output.
	max_classer(Idx<ubyte> *classes);
	~max_classer() {
	}
	;

	void fprop(state_idx *in, class_state *out);
};

////////////////////////////////////////////////////////////////

//! softmax module
//! if in is idx0 -> out is idx0 and equal to 1
//! if in is idx1 -> it is just one pool
//! if in is idx2 -> it is just one pool
//! if in is idx3 -> the last two dimensions are pools
//! if in is idx4 -> the last two dimensions are pools
//! if in is idx5 -> the last four dimensions are pools
//! if in is idx6 -> the last four dimensions are pools

class softmax: public module_1_1<state_idx, state_idx> {

public:
	double beta;

	// <b> is the parameter beta in the softmax
	// large <b> turns the softmax into a max
	// <b> equal to 0 turns the softmax into 1/N

private:
	void resize_nsame(state_idx *in, state_idx *out, int n);

public:
	softmax(double b);
	~softmax() {
	}
	;
	void fprop(state_idx *in, state_idx *out);
	void bprop(state_idx *in, state_idx *out);
	void bbprop(state_idx *in, state_idx *out);

};
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
//! Generic Jacobian tester
//!

class Jacobian_tester {

public:
	Jacobian_tester() {
	}
	;
	~Jacobian_tester() {
	}
	;

	// this function take any module_1_1 with a fprop et bprop implemented, and tests
	// if the jacobian is correct (by pertubation) (on a state_idx with 3 dimensions)
	void test(module_1_1<state_idx, state_idx> *module);

};

////////////////////////////////////////////////////////////////
//
//! Generic BBprop tester tester
//!

class Bbprop_tester {

public:
	Bbprop_tester() {
	}
	;
	~Bbprop_tester() {
	}
	;

	// this function take any module_1_1 with a fprop et bbprop implemented, and tests
	// if the Bbprop is correct (by pertubation) (on a state_idx with 3 dimensions)
	void test(module_1_1<state_idx, state_idx> *module);

};

////////////////////////////////////////////////////////////////
//
//! Generic Bprop tester tester
//!

class Bprop_tester {

public:
	Bprop_tester() {
	}
	;
	~Bprop_tester() {
	}
	;

	// this function take any module_1_1 with a fprop et bprop implemented, and tests
	// if the bprop is correct (by pertubation) (on a state_idx with 3 dimensions)
	void test(module_1_1<state_idx, state_idx> *module);

};

} // namespace ebl {

#include "Ebm.hpp"

#endif /* EBM_H_ */
