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

#ifndef GBL_H_
#define GBL_H_

#include "Idx.h"
#include "Blas.h"
#include "EblStates.h"
#include "Ebl.h"
#include "EblMachines.h"

namespace ebl {

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
    f_layer(parameter &p, intg tin, intg tout, intg si, intg sj, 
	    module_1_1<state_idx, state_idx> *sq);
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
    c_layer(parameter &prm, intg ki, intg kj, intg ri, intg rj, Idx<intg> *tbl,
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
  c_layer_ipp(parameter &prm, intg ki, intg kj, intg ri, intg rj, 
	      Idx<intg> *tbl, intg thick, intg si, intg sj, 
	      module_1_1<state_idx,state_idx> *sqsh)
    : c_layer(prm, ki, kj, ri, rj, tbl, thick, si, sj, sqsh)
      {}
    //! fprop
    virtual void fprop(state_idx *in, state_idx *out);
    //! bprop
    virtual void bprop(state_idx *in, state_idx *out);
  };

#endif

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
    //! <prm> pointer to a "parameter" object, from which the parameters 
    //! will be allocated
    //! <ki>  vertical subsampling ratio.
    //! <kj>  horizontal subsampling ratio.
    //! <thick> thickness of output layer (number of feature maps)
    //! <si>  vertical size for preallocation of internal state
    //! <sj>  horizontal size for preallocation of internal state
    //! <sqsh> pointer to a squashing function module that operates
    s_layer(parameter &p, intg ki, intg kj, intg thick, intg si, intg sj,
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
    //! a vector that maps output unit index to a label
    Idx<ubyte> *classindex2label; 

    //! makes a new max-classer. <classes> is an integer vector
    //! which contains the labels associated with each output.
    max_classer(Idx<ubyte> *classes);
    ~max_classer() {
    }
    ;

    void fprop(state_idx *in, class_state *out);
  };

  ////////////////////////////////////////////////////////////////

  //! a module that takes an idx3 as input, runs it through
  //! a machine, and runs the output of the machine through
  //! a cost function whose second output is the desired label
  //! stored in an idx0 of int.
  class idx3_supervised_module {
  public:
    nn_machine_cscscf	*machine;
    state_idx		*mout;
    edist_cost 		*cost;
    max_classer 	*classifier;

    idx3_supervised_module(nn_machine_cscscf *m, edist_cost *c, 
			   max_classer *cl);
    virtual ~idx3_supervised_module();

    void fprop(state_idx *input, class_state *output, 
	       Idx<ubyte> *desired, state_idx *energy);
    void use(state_idx *input, class_state *output);
    void bprop(state_idx *input, class_state *output, 
	       Idx<ubyte> *desired, state_idx *energy);
    void bbprop(state_idx *input, class_state *output, 
  		Idx<ubyte> *desired, state_idx *energy);
  };

} // namespace ebl {

#endif /* GBL_H_ */
