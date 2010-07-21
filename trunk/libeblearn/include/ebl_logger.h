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

#ifndef EBL_LOGGER_H_
#define EBL_LOGGER_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_states.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////

  //! a special kind of state used to store the output of a classifier.
  //! class-state are generated by modules such as class-max, and used
  //! by meters such as classifier-meter. No backprop is possible through
  //! a class-state.
  class class_state {
  public:
    ubyte output_class;
    float confidence;
    idx<ubyte> *sorted_classes;
    idx<float> *sorted_scores;

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
  class EXPORT classifier_meter {
  public:
    //! Create a new <classifier-meter> using <comparison-function>
    //! to compare actual and desired answers. By default
    //! the <same-class?> function is used for that purpose.
    //! It takes two integer arguments, and returns 1 if they
    //! are equal, -1 if they are different, and 0 if
    //! the first argument is -1 (which means reject).
    // TODO: allow passing of comparison function
    classifier_meter();

    //! Destructor.
    ~classifier_meter();

    //! Initialize the meter.
    //! \param nclasses The number of classes.
    void init(uint nclasses);
    
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
    char update(intg a, class_state *co, ubyte cd, double energy);
    void update(intg age_, bool correct, double energy);
    // TODO: clean up design
    // TODO: add confusion matrix computation
    void update(intg age, uint desired, uint infered, double energy);

    void test(class_state *co, ubyte cd, double energy);

    //! Returns the average error percentage computed from the confusion matrix,
    //! i.e. taking into account the class distributions, in other words,
    //! returns the mean of the per-class mean error.
    double class_normalized_average_error();

    //! Returns the average error percentage computed regardless of the class.
    //! This measure might be bias with unbalanced datasets.
    double overall_average_error();

    //! Returns the average success percentage computed
    //! from the confusion matrix.
    double class_normalized_average_success();

    //! Returns the confusion matrix;
    idx<int>& get_confusion();

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
    //! names of each class (lblstr) are optional.
    //! \param ds_is_test If true, prepend "test_" in front of varialbes.
    void display(int iteration, string &dsname, vector<string*> *lblstr = NULL,
		 bool ds_is_test = false);

    //! display ROC points for each class.
    //! names of each class (lblstr) are optional.
    void display_positive_rates(double threshold,
				vector<string*> *lblstr = NULL);
    
    bool save();
    bool load();
    
  public:
    double		energy;
    float		confidence;
    intg		size;
    intg		age;
    intg		total_correct;
    intg		total_error;
    intg		total_punt;
    double		total_energy;
    vector<uint>	class_errors;
    vector<uint>	class_totals;
    vector<uint>	class_tpr;
    vector<uint>	class_fpr;
  private:
    idx<int>		confusion;	//!< confusion matrix
    uint                nclasses;       //!< number of classes
  };

  ////////////////////////////////////////////////////////////////

  //! a module that takes an fstate_idx, finds the lowest value
  //! and output the label associated with the index (in the first dimension
  //! of the state) of this lowest value.
  //! It actually sorts the labels according to their score (or costs)
  //! and outputs the sorted list.
  template <class T> class max_classer { // TODO: idx3-classer
  public:
    //! a vector that maps output unit index to a label
    idx<ubyte> *classindex2label; 

    //! makes a new max-classer. <classes> is an integer vector
    //! which contains the labels associated with each output.
    max_classer(idx<ubyte> *classes);
    ~max_classer() {
    }
    ;

    void fprop(fstate_idx<T> *in, class_state *out);
  };

} // namespace ebl {

#endif /* EBL_LOGGER_H_ */
