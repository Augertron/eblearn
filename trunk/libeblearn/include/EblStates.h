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

#ifndef EBLSTATES_H_
#define EBLSTATES_H_

#include "Idx.h"
#include "Blas.h"

namespace ebl {

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
    //! Constructs a state_idx with order 1 based on given state_idx si.
    state_idx(state_idx *si);

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

} // namespace ebl {

#endif /* EBLSTATES_H_ */
