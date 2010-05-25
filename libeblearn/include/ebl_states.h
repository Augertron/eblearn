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

#ifndef EBL_STATES_H_
#define EBL_STATES_H_

#include "libidx.h"

namespace ebl {

  class infer_param {
  };

  ////////////////////////////////////////////////////////////////
  //! A class that contains all the parameters
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
  //! clear (clear all), clear_x (clear values), clear_dx (clear gradients),
  //! clear_ddx (clear hessian), and update_gd(arg) (update
  //! with gradient descent.
  class state {
  public:
    //! constructor
    state();
    //! destructor
    virtual ~state();
    //! clear x, dx and ddx
    virtual void clear();
    //! clear x
    virtual void clear_x();
    //! clear dx
    virtual void clear_dx();
    //! clear ddx
    virtual void clear_ddx();
    virtual void update_gd(gd_param &arg);
  };

  template <class T> class parameter;
  
  ////////////////////////////////////////////////////////////////
  //! A class that stores a vector/tensor state.
  template <class T> class state_idx: public state {
  public:
    virtual ~state_idx();

    ////////////////////////////////////////////////////////////////
    //! member variables
  private:
    //! an object containing an order and dimensons to help for idx creations
    idxdim	dims;

  public:
    //! state itself
    idx<T> x;
    //! gradient of loss with respect to state
    idx<T> dx;
    //! diag hessian of loss with respect to state
    idx<T> ddx;

    ////////////////////////////////////////////////////////////////
    //! constructors from specific dimensions

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
    //! Constructor. Use the order and dimensions contained in passed idxdim d.
    state_idx(const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! constructors from specific dimensions using a parameter

    //! Constructs a state_idx of order 0, by appending the state_idx into
    //! the same Srg as the parameter passed as argument. This is useful for
    //! allocating multiple state_idx inside a parameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    state_idx(parameter<T> *st);
    //! Constructs a state_idx of order 1, by appending the state_idx into
    //! the same Srg as the parameter passed as argument. This is useful for
    //! allocating multiple state_idx inside a parameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    state_idx(parameter<T> *st, intg s0);
    //! Constructs a state_idx of order 2, by appending the state_idx into
    //! the same Srg as the parameter passed as argument. This is useful for
    //! allocating multiple state_idx inside a parameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    state_idx(parameter<T> *st, intg s0, intg s1);
    //! Constructs a state_idx of order 3, by appending the state_idx into
    //! the same Srg as the parameter passed as argument. This is useful for
    //! allocating multiple state_idx inside a parameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    state_idx(parameter<T> *st, intg s0, intg s1, intg s2);
    //! Constructs a state_idx of order up to 8, by appending the state_idx into
    //! the same Srg as the parameter passed as argument. This is useful for
    //! allocating multiple state_idx inside a parameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    state_idx(parameter<T> *st, intg s0, intg s1, intg s2, intg s3,
	      intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 = -1);
    //! Constructs a state_idx using the order and dimensions contained in
    //! passed idxdim d, by appending the state_idx into
    //! the same Srg as the parameter passed as argument. This is useful for
    //! allocating multiple state_idx inside a parameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    state_idx(parameter<T> *st, const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! constructors from other state_idx

    //! Constructs a state_idx from a state_idx's 3 internal idx
    //! Note: the data pointed to by idxs is not copied, we only create new idx
    //!   pointing to the same data.
    state_idx(const idx<T> &x, const idx<T> &dx, 
	      const idx<T> &ddx);

    ////////////////////////////////////////////////////////////////
    //! clear methods

    //! clear x, dx and ddx
    virtual void clear();

    //! clear x
    virtual void clear_x();

    //! clear gradients dx
    virtual void clear_dx();

    //! clear diag hessians ddx
    virtual void clear_ddx();

    ////////////////////////////////////////////////////////////////
    //! information methods

    //! return number of elements
    virtual intg nelements();

    //! return footprint in storages
    virtual intg footprint();

    //! same as footprint
    virtual intg size();

    //! update with gradient descent
    virtual void update_gd(gd_param &arg);
      
    ////////////////////////////////////////////////////////////////
    //! resize methods

    //! resize. The order cannot be changed with this.
    virtual void resize(intg s0 = -1, intg s1 = -1, intg s2 = -1, intg s3 = -1,
			intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 = -1);

    //! resize with dimensions contained in an idxdim. order cannot be changed.
    virtual void resize(const idxdim &d);

    //! resize one dimension <dimn> with size <size>. 
    //! The order cannot be changed.
    virtual void resize1(intg dimn, intg size);
    
    //! resizes this state_idx with same sizes a <s>.
    //! Both state_idx are required to have the same order.
    virtual void resize_as(state_idx& s);

    //! same as resize_as but leave dimension <fixed_dim> untouched.
    //! Both state_idx are required to have the same order.
    virtual void resize_as_but1(state_idx<T>& s, intg fixed_dim);

    virtual void resize(const intg* dimsBegin, const intg* dimsEnd);

    //! make a new copy of self
    virtual state_idx<T> make_copy();
  };

  ////////////////////////////////////////////////////////////////
  //! parameter: the main class for a trainable
  //! parameter vector.

  template <class T> class parameter: public state_idx<T> {
  public:
    using state_idx<T>::x;
    using state_idx<T>::dx;
    using state_idx<T>::ddx;

    idx<T> gradient;
    idx<T> deltax;
    idx<T> epsilons;
    idx<T> ddeltax;

    //! initialize the parameter with size initial_size.
    parameter(intg initial_size = 100);

    //! initialize the parameter with a previously saved x component.
    parameter(const char *param_filename);

    //! destructor
    virtual ~parameter();

    virtual void resize(intg s0);
    void update_gd(gd_param &arg);
    virtual void update(gd_param &arg);
    void clear_deltax();
    void update_deltax(T knew, T kold);
    void clear_ddeltax();
    void update_ddeltax(T knew, T kold);
    void set_epsilons(T m);
    void compute_epsilons(T mu);

    //! load a parameter file into the x component.
    bool load_x(const char *param_filename);

    //! save the x component to a file.
    bool save_x(const char *param_filename);
  };

  ////////////////////////////////////////////////////////////////
  //! state_idx iterator
  template <class T> class state_idxlooper : public state_idx<T> {
  public:
    using state_idx<T>::x;
    using state_idx<T>::dx;
    using state_idx<T>::ddx;

    idxlooper<T> lx;
    idxlooper<T> ldx;
    idxlooper<T> lddx;

    //! generic constructor loops over dimensin ld
    state_idxlooper(state_idx<T> &s, int ld);
    virtual ~state_idxlooper();

    //! return true if loop is over
    bool notdone();

    //! increment to next item.
    void next();
  };

  ////////////////////////////////////////////////////////////////
  //! loop macro on 1 state_idx
#define state_idx_eloop1(dst0,src0,type0)			\
  state_idxlooper<type0> dst0(src0, (src0).x.order() - 1);	\
  for ( ; dst0.notdone(); dst0.next())

  ////////////////////////////////////////////////////////////////
  //! loop macro on 2 state_idx
#define state_idx_eloop2(dst0,src0,type0,dst1,src1,type1)		\
  if ((src0).x.dim((src0).x.order() - 1)				\
  != (src1).x.dim((src1).x.order() - 1))				\
  eblerror("incompatible state_idx for eloop\n");			\
  state_idxlooper<type0> dst0(src0,(src0).x.order()-1);			\
  state_idxlooper<type1> dst1(src1,(src1).x.order()-1);			\
  for ( ; dst0.notdone(); dst0.next(), dst1.next())

  ////////////////////////////////////////////////////////////////
  //! loop macro on 3 state_idx
#define state_idx_eloop3(dst0,src0,type0,dst1,src1,type1,dst2,src2,type2) \
  if (((src0).x.dim((src0).x.order() - 1)				\
       != (src1).x.dim((src1).x.order() - 1))				\
      || ((src0).x.dim((src0).x.order() - 1)				\
	  != (src2).x.dim((src2).x.order() - 1)))			\
    eblerror("incompatible idxs for eloop\n");				\
  state_idxlooper<type0> dst0(src0,(src0).x.order()-1);			\
  state_idxlooper<type1> dst1(src1,(src1).x.order()-1);			\
  state_idxlooper<type2> dst2(src2,(src2).x.order()-1);			\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next())

} // namespace ebl {

#include "ebl_states.hpp"

#endif /* EBL_STATES_H_ */
