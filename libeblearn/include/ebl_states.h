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
  class EXPORT gd_param: public infer_param {
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

  class EXPORT forget_param_linear: public forget_param {
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
  class EXPORT state {
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

  template <typename T> class bbstate_idx;
  template <typename T, class Tstate = bbstate_idx<T> > class parameter;

  ////////////////////////////////////////////////////////////////
  //! A class that stores a vector/tensor state.
  template <typename T> class fstate_idx: public state {
  public:
    virtual ~fstate_idx();

    ////////////////////////////////////////////////////////////////
    //! constructors from specific dimensions

    //! Constructs a fstate_idx of order 0
    fstate_idx();
    //! Constructs a fstate_idx of order 1
    fstate_idx(intg s0);
    //! Constructs a fstate_idx of order 2
    fstate_idx(intg s0, intg s1);
    //! Constructs a fstate_idx of order 3
    fstate_idx(intg s0, intg s1, intg s2);
    //! Constructor. A fstate_idx can have up to 8 dimensions.
    fstate_idx(intg s0, intg s1, intg s2, intg s3, intg s4 = -1, intg s5 = -1,
	      intg s6 = -1, intg s7 = -1);
    //! Constructor. Use the order and dimensions contained in passed idxdim d.
    fstate_idx(const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! constructors from specific dimensions using a fparameter

    //! Constructs a fstate_idx of order 0, by appending the fstate_idx into
    //! the same Srg as the fparameter passed as argument. This is useful for
    //! allocating multiple fstate_idx inside a fparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    fstate_idx(parameter<T,fstate_idx<T> > *st);
    //! Constructs a fstate_idx of order 1, by appending the fstate_idx into
    //! the same Srg as the fparameter passed as argument. This is useful for
    //! allocating multiple fstate_idx inside a fparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    fstate_idx(parameter<T,fstate_idx<T> > *st, intg s0);
    //! Constructs a fstate_idx of order 2, by appending the fstate_idx into
    //! the same Srg as the fparameter passed as argument. This is useful for
    //! allocating multiple fstate_idx inside a fparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    fstate_idx(parameter<T,fstate_idx<T> > *st, intg s0, intg s1);
    //! Constructs a fstate_idx of order 3, by appending the fstate_idx into
    //! the same Srg as the fparameter passed as argument. This is useful for
    //! allocating multiple fstate_idx inside a fparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    fstate_idx(parameter<T,fstate_idx<T> > *st, intg s0, intg s1, intg s2);
    //! Constructs a fstate_idx of order up to 8, by appending the fstate_idx
    //! into
    //! the same Srg as the fparameter passed as argument. This is useful for
    //! allocating multiple fstate_idx inside a fparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    fstate_idx(parameter<T,fstate_idx<T> > *st, intg s0, intg s1, intg s2,
	       intg s3, intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 = -1);
    //! Constructs a fstate_idx using the order and dimensions contained in
    //! passed idxdim d, by appending the fstate_idx into
    //! the same Srg as the fparameter passed as argument. This is useful for
    //! allocating multiple fstate_idx inside a fparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    fstate_idx(parameter<T,fstate_idx<T> > *st, const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! constructors from other fstate_idx

    //! Constructs a fstate_idx from an existing idx.
    //! Note: the data pointed to by idxs is not copied, we only create new idx
    //!   pointing to the same data.
    fstate_idx(const idx<T> &x);

    ////////////////////////////////////////////////////////////////
    //! clear methods

    //! clear all buffers (x, dx and ddx).
    virtual void clear();

    //! clear x
    virtual void clear_x();

    ////////////////////////////////////////////////////////////////
    //! information methods

    //! return number of elements
    virtual intg nelements();

    //! return footprint in storages
    virtual intg footprint();

    //! same as footprint
    virtual intg size();

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
    
    //! resizes this fstate_idx with same sizes a <s>.
    //! Both fstate_idx are required to have the same order.
    virtual void resize_as(fstate_idx& s);

    //! same as resize_as but leave dimension <fixed_dim> untouched.
    //! Both fstate_idx are required to have the same order.
    virtual void resize_as_but1(fstate_idx<T>& s, intg fixed_dim);

    //    virtual void resize(const intg* dimsBegin, const intg* dimsEnd);

    //! make a new copy of self
    fstate_idx<T> make_copy();

    ////////////////////////////////////////////////////////////////
    //! member variables
  public:
    //! state itself
    idx<T> x;
    //! gradient of loss with respect to state
    idx<T> dx;
    //! diag hessian of loss with respect to state
    idx<T> ddx;
  };

  
  ////////////////////////////////////////////////////////////////
  //! A class that stores a vector/tensor state.
  template <typename T> class bstate_idx: public fstate_idx<T> {
  public:
    virtual ~bstate_idx();

    ////////////////////////////////////////////////////////////////
    //! constructors from specific dimensions

    //! Constructs a bstate_idx of order 0
    bstate_idx();
    //! Constructs a bstate_idx of order 1
    bstate_idx(intg s0);
    //! Constructs a bstate_idx of order 2
    bstate_idx(intg s0, intg s1);
    //! Constructs a bstate_idx of order 3
    bstate_idx(intg s0, intg s1, intg s2);
    //! Constructor. A bstate_idx can have up to 8 dimensions.
    bstate_idx(intg s0, intg s1, intg s2, intg s3, intg s4 = -1, intg s5 = -1,
	      intg s6 = -1, intg s7 = -1);
    //! Constructor. Use the order and dimensions contained in passed idxdim d.
    bstate_idx(const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! constructors from specific dimensions using a bparameter

    //! Constructs a bstate_idx of order 0, by appending the bstate_idx into
    //! the same Srg as the bparameter passed as argument. This is useful for
    //! allocating multiple bstate_idx inside a bparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bstate_idx(parameter<T,bstate_idx<T> > *st);
    //! Constructs a bstate_idx of order 1, by appending the bstate_idx into
    //! the same Srg as the bparameter passed as argument. This is useful for
    //! allocating multiple bstate_idx inside a bparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bstate_idx(parameter<T,bstate_idx<T> > *st, intg s0);
    //! Constructs a bstate_idx of order 2, by appending the bstate_idx into
    //! the same Srg as the bparameter passed as argument. This is useful for
    //! allocating multiple bstate_idx inside a bparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bstate_idx(parameter<T,bstate_idx<T> > *st, intg s0, intg s1);
    //! Constructs a bstate_idx of order 3, by appending the bstate_idx into
    //! the same Srg as the bparameter passed as argument. This is useful for
    //! allocating multiple bstate_idx inside a bparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bstate_idx(parameter<T,bstate_idx<T> > *st, intg s0, intg s1, intg s2);
    //! Constructs a bstate_idx of order up to 8, by appending the bstate_idx
    //! into
    //! the same Srg as the bparameter passed as argument. This is useful for
    //! allocating multiple bstate_idx inside a bparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bstate_idx(parameter<T,bstate_idx<T> > *st, intg s0, intg s1, intg s2,
	       intg s3, intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 = -1);
    //! Constructs a bstate_idx using the order and dimensions contained in
    //! passed idxdim d, by appending the bstate_idx into
    //! the same Srg as the bparameter passed as argument. This is useful for
    //! allocating multiple bstate_idx inside a bparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bstate_idx(parameter<T,bstate_idx<T> > *st, const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! constructors from other bstate_idx

    //! Constructs a bstate_idx from a bstate_idx's 3 internal idx
    //! Note: the data pointed to by idxs is not copied, we only create new idx
    //!   pointing to the same data.
    bstate_idx(const idx<T> &x, const idx<T> &dx);

    ////////////////////////////////////////////////////////////////
    //! clear methods

    //! clear x and dx.
    virtual void clear();

    //! clear gradients dx
    virtual void clear_dx();

    //! update with gradient descent
    virtual void update_gd(gd_param &arg);
      
    ////////////////////////////////////////////////////////////////
    //! information methods

    //! return number of elements
    using fstate_idx<T>::nelements;

    //! return footprint in storages
    using fstate_idx<T>::footprint;

    //! same as footprint
    using fstate_idx<T>::size;
    
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
    
    //! resizes this bstate_idx with same sizes a <s>.
    //! Both bstate_idx are required to have the same order.
    virtual void resize_as(bstate_idx& s);

    //! same as resize_as but leave dimension <fixed_dim> untouched.
    //! Both bstate_idx are required to have the same order.
    virtual void resize_as_but1(bstate_idx<T>& s, intg fixed_dim);

    //    virtual void resize(const intg* dimsBegin, const intg* dimsEnd);

    //! make a new copy of self
    bstate_idx<T> make_copy();

    ////////////////////////////////////////////////////////////////
    //! member variables
  public:
    //! state itself
    using fstate_idx<T>::x;
    //! gradient of loss with respect to state
    using fstate_idx<T>::dx;
    //! diag hessian of loss with respect to state
    using fstate_idx<T>::ddx;
  };
  
  ////////////////////////////////////////////////////////////////
  //! A class that stores a vector/tensor state.
  template <typename T> class bbstate_idx: public bstate_idx<T> {
  public:
    virtual ~bbstate_idx();

    ////////////////////////////////////////////////////////////////
    //! constructors from specific dimensions

    //! Constructs a bbstate_idx of order 0
    bbstate_idx();
    //! Constructs a bbstate_idx of order 1
    bbstate_idx(intg s0);
    //! Constructs a bbstate_idx of order 2
    bbstate_idx(intg s0, intg s1);
    //! Constructs a bbstate_idx of order 3
    bbstate_idx(intg s0, intg s1, intg s2);
    //! Constructor. A bbstate_idx can have up to 8 dimensions.
    bbstate_idx(intg s0, intg s1, intg s2, intg s3, intg s4 = -1, intg s5 = -1,
	      intg s6 = -1, intg s7 = -1);
    //! Constructor. Use the order and dimensions contained in passed idxdim d.
    bbstate_idx(const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! constructors from specific dimensions using a bbparameter

    //! Constructs a bbstate_idx of order 0, by appending the bbstate_idx into
    //! the same Srg as the bbparameter passed as argument. This is useful for
    //! allocating multiple bbstate_idx inside a bbparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bbstate_idx(parameter<T,bbstate_idx<T> > *st);
    //! Constructs a bbstate_idx of order 1, by appending the bbstate_idx into
    //! the same Srg as the bbparameter passed as argument. This is useful for
    //! allocating multiple bbstate_idx inside a bbparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bbstate_idx(parameter<T,bbstate_idx<T> > *st, intg s0);
    //! Constructs a bbstate_idx of order 2, by appending the bbstate_idx into
    //! the same Srg as the bbparameter passed as argument. This is useful for
    //! allocating multiple bbstate_idx inside a bbparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bbstate_idx(parameter<T,bbstate_idx<T> > *st, intg s0, intg s1);
    //! Constructs a bbstate_idx of order 3, by appending the bbstate_idx into
    //! the same Srg as the bbparameter passed as argument. This is useful for
    //! allocating multiple bbstate_idx inside a bbparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bbstate_idx(parameter<T,bbstate_idx<T> > *st, intg s0, intg s1, intg s2);
    //! Constructs a bbstate_idx of order up to 8, by appending the
    //! bbstate_idx into
    //! the same Srg as the bbparameter passed as argument. This is useful for
    //! allocating multiple bbstate_idx inside a bbparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bbstate_idx(parameter<T,bbstate_idx<T> > *st, intg s0, intg s1, intg s2,
		intg s3, intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 =-1);
    //! Constructs a bbstate_idx using the order and dimensions contained in
    //! passed idxdim d, by appending the bbstate_idx into
    //! the same Srg as the bbparameter passed as argument. This is useful for
    //! allocating multiple bbstate_idx inside a bbparameter.
    //! If st is null, it is just ignore and behaves as regular constructor.
    bbstate_idx(parameter<T,bbstate_idx<T> > *st, const idxdim &d);

    ////////////////////////////////////////////////////////////////
    //! constructors from other bbstate_idx

    //! Constructs a bbstate_idx from a bbstate_idx's 3 internal idx
    //! Note: the data pointed to by idxs is not copied, we only create new idx
    //!   pointing to the same data.
    bbstate_idx(const idx<T> &x, const idx<T> &dx, 
	      const idx<T> &ddx);

    ////////////////////////////////////////////////////////////////
    //! clear methods

    //! clear x, dx and ddx
    virtual void clear();

    //! clear diag hessians ddx
    virtual void clear_ddx();

    //! update with gradient descent
    using bstate_idx<T>::update_gd;
      
    ////////////////////////////////////////////////////////////////
    //! information methods

    //! return number of elements
    using fstate_idx<T>::nelements;

    //! return footprint in storages
    using fstate_idx<T>::footprint;

    //! same as footprint
    using fstate_idx<T>::size;
    
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
    
    //! resizes this bbstate_idx with same sizes a <s>.
    //! Both bbstate_idx are required to have the same order.
    virtual void resize_as(bbstate_idx& s);

    //! same as resize_as but leave dimension <fixed_dim> untouched.
    //! Both bbstate_idx are required to have the same order.
    virtual void resize_as_but1(bbstate_idx<T>& s, intg fixed_dim);

    //    virtual void resize(const intg* dimsBegin, const intg* dimsEnd);

    //! make a new copy of self
    bbstate_idx<T> make_copy();

    ////////////////////////////////////////////////////////////////
    //! member variables
  public:
    //! state itself
    using fstate_idx<T>::x;
    //! gradient of loss with respect to state
    using fstate_idx<T>::dx;
    //! diag hessian of loss with respect to state
    using fstate_idx<T>::ddx;
  };

  ////////////////////////////////////////////////////////////////
  //! parameter<T,fstate_idx<T> >: a forward (non-trainable) parameter vector.
  template <typename T>
    class parameter<T, fstate_idx<T> > : public fstate_idx<T> {
  public:
    //! initialize the fparameter with size initial_size.
    parameter(intg initial_size = 100);

    //! initialize the fparameter with a previously saved x component.
    parameter(const char *param_filename);

    //! destructor
    virtual ~parameter();

    virtual void resize(intg s0);

    ////////////////////////////////////////////////////////////////
    // I/O methods
    
    //! load a fparameter file into the x component.
    bool load_x(const char *param_filename);

    //! save the x component to a file.
    bool save_x(const char *param_filename);

    ////////////////////////////////////////////////////////////////
    //! information methods

    //! return number of elements
    using fstate_idx<T>::nelements;

    //! return footprint in storages
    using fstate_idx<T>::footprint;

    //! same as footprint
    using fstate_idx<T>::size;
    
    ////////////////////////////////////////////////////////////////
    //! members
  public:
    using fstate_idx<T>::x;
  };

  ////////////////////////////////////////////////////////////////
  //! bparameter: the main class for a trainable
  //! bparameter vector.

  template <typename T>
    class parameter<T, bstate_idx<T> >
    : public bstate_idx<T>, public parameter<T,fstate_idx<T> > {
  public:
    //! initialize the bparameter with size initial_size.
    parameter(intg initial_size = 100);

    //! initialize the bparameter with a previously saved x component.
    parameter(const char *param_filename);

    //! destructor
    virtual ~parameter();

    virtual void resize(intg s0);
    void update_gd(gd_param &arg);
    virtual void update(gd_param &arg);
    void clear_deltax();
    void update_deltax(T knew, T kold);
    void set_epsilons(T m);

    ////////////////////////////////////////////////////////////////
    // I/O methods
    
    //! load a bparameter file into the x component.
    bool load_x(const char *param_filename);

    //! save the x component to a file.
    bool save_x(const char *param_filename);

    ////////////////////////////////////////////////////////////////
    //! information methods

    //! return number of elements
    using fstate_idx<T>::nelements;

    //! return footprint in storages
    using fstate_idx<T>::footprint;

    //! same as footprint
    using fstate_idx<T>::size;
    
    ////////////////////////////////////////////////////////////////
    //! members
  public:
    using fstate_idx<T>::x;
    using bstate_idx<T>::dx;

    idx<T> gradient;
    idx<T> deltax;
    idx<T> epsilons;
  };

  ////////////////////////////////////////////////////////////////
  //! bbparameter: the main class for a trainable
  //! bbparameter vector.

  template <typename T>
    class parameter<T, bbstate_idx<T> > : public bbstate_idx<T> {
  public:
    //! initialize the bbparameter with size initial_size.
    parameter(intg initial_size = 100);

    //! initialize the bbparameter with a previously saved x component.
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

    ////////////////////////////////////////////////////////////////
    // I/O methods
    
    //! load a bbparameter file into the x component.
    bool load_x(const char *param_filename);

    //! save the x component to a file.
    bool save_x(const char *param_filename);

    ////////////////////////////////////////////////////////////////
    //! information methods

    //! return number of elements
    using fstate_idx<T>::nelements;

    //! return footprint in storages
    using fstate_idx<T>::footprint;

    //! same as footprint
    using fstate_idx<T>::size;
    
    ////////////////////////////////////////////////////////////////
    //! members
  public:
    using fstate_idx<T>::x;
    using fstate_idx<T>::dx;
    using fstate_idx<T>::ddx;
    idx<T> gradient;
    idx<T> deltax;
    idx<T> epsilons;
    idx<T> ddeltax;
  };

  ////////////////////////////////////////////////////////////////
  // state_idxloopers

  template <typename Tstate> class state_idxlooper;
  
  //! fstate_idx iterator
  template <typename T>
    class state_idxlooper<fstate_idx<T> > : public fstate_idx<T> {
  public:
    using fstate_idx<T>::x;

    idxlooper<T> lx;

    //! generic constructor loops over dimensin ld
    state_idxlooper(fstate_idx<T> &s, int ld);
    virtual ~state_idxlooper();

    //! return true if loop is over
    bool notdone();

    //! increment to next item.
    void next();
  };

  ////////////////////////////////////////////////////////////////
  //! bstate_idx iterator
  template <typename T>
    class state_idxlooper<bstate_idx<T> > : public bstate_idx<T> {
  public:
    using bstate_idx<T>::x;
    using bstate_idx<T>::dx;

    idxlooper<T> lx;
    idxlooper<T> ldx;

    //! generic constructor loops over dimensin ld
    state_idxlooper(bstate_idx<T> &s, int ld);
    virtual ~state_idxlooper();

    //! return true if loop is over
    bool notdone();

    //! increment to next item.
    void next();
  };

  ////////////////////////////////////////////////////////////////
  //! bbstate_idx iterator
  template <typename T>
    class state_idxlooper<bbstate_idx<T> > : public bbstate_idx<T> {
  public:
    using bbstate_idx<T>::x;
    using bbstate_idx<T>::dx;
    using bbstate_idx<T>::ddx;

    idxlooper<T> lx;
    idxlooper<T> ldx;
    idxlooper<T> lddx;

    //! generic constructor loops over dimensin ld
    state_idxlooper(bbstate_idx<T> &s, int ld);
    virtual ~state_idxlooper();

    //! return true if loop is over
    bool notdone();

    //! increment to next item.
    void next();
  };

  ////////////////////////////////////////////////////////////////
  //! loop macro on 1 state_idx
#define state_idx_eloop1(dst0,src0,type0)				\
  state_idxlooper<type0> dst0(src0, (src0).x.order() - 1);		\
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
#define state_idx_eloop3(src0,type0,dst1,src1,type1,dst2,src2,type2)	\
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
