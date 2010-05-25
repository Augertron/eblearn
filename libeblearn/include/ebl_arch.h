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

#ifndef EBL_ARCH_H_
#define EBL_ARCH_H_

#include "ebl_arch_gen.h"
#include "libidx.h"
#include "ebl_defines.h"
#include "ebl_states.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // templates for generic modules

  //! abstract class for a module with one input and one output.
  template<class T> class module_1_1
    : public module_1_1_gen< state_idx<T>, state_idx<T> > {
/*   public: */
/*     module_1_1(bool bResize = true); //!< by default, resize output */
/*     virtual ~module_1_1() {}; */
/*     virtual void fprop(state_idx<T> &in, state_idx<T> &out); */
/*     virtual void bprop(state_idx<T> &in, state_idx<T> &out); */
/*     virtual void bbprop(state_idx<T> &in, state_idx<T> &out); */
/*     virtual void forget(forget_param_linear& fp); */
/*     virtual void normalize(); */
/*     //! returns the order at which the module operates. */
/*     virtual int  replicable_order(); */
/*     virtual void resize_output(state_idx<T> &in, state_idx<T> &out); */
/*     //! given the input dimensions, modifies it to be compliant with module's */
/*     //! architecture, and returns the output dimensions corresponding to */
/*     //! modified input dimensions. */
/*     //! the implementation of this method helps automatic scaling of input data */
/*     //! but is optional. */
/*     virtual idxdim fprop_size(idxdim &isize); */
/*     //! given the output dimensions, returns the input dimensions. */
/*     //! the implementation of this method helps automatic scaling of input data */
/*     //! but is optional. */
/*     virtual idxdim bprop_size(const idxdim &osize); */
/*     //! prints the forward transformation of dimensions. this method calls */
/*     //! fprop_size to determine the output size given the input. */
/*     virtual void pretty(idxdim &isize); */
  };

  //! abstract class for a module with two inputs and one output.
  template<class T> class module_2_1
    : public module_2_1_gen< state_idx<T>, state_idx<T>, state_idx<T> > {
/*   public: */
/*     virtual ~module_2_1() {}; */
/*     virtual void fprop(state_idx<T> &in1, state_idx<T> &in2, state_idx<T> &out); */
/*     virtual void bprop(state_idx<T> &in1, state_idx<T> &in2, state_idx<T> &out); */
/*     virtual void bbprop(state_idx<T> &in1, state_idx<T> &in2,state_idx<T> &out); */
/*     virtual void forget(forget_param &fp); */
/*     virtual void normalize(); */
  };

  ////////////////////////////////////////////////////////////////
  //! abstract class for a module with one inputs and one energy output.
  template<class T> class ebm_1 : public ebm_1_gen<state_idx<T>, T> {
/*   public: */
/*     virtual ~ebm_1() {}; */
/*     virtual void fprop(state_idx<T> &in, state_idx<T> &energy); */
/*     virtual void bprop(state_idx<T> &in, state_idx<T> &energy); */
/*     virtual void bbprop(state_idx<T> &in, state_idx<T> &energy); */
/*     virtual void forget(forget_param &fp); */
/*     virtual void normalize(); */
  };

  ////////////////////////////////////////////////////////////////
  //! abstract class for a module with two inputs and one energy output.
  template<class T> class ebm_2
    : public ebm_2_gen<state_idx<T>, state_idx<T>, T> {
/*   public: */
/*     virtual ~ebm_2() {}; */
/*     //! fprop: compute energy output from inputs i1 and i2 */
/*     virtual void fprop(state_idx<T> &i1, state_idx<T> &i2, */
/* 		       state_idx<T> &energy); */
/*     //! bprop: compute gradient wrt inputs, given gradient wrt output */
/*     virtual void bprop(state_idx<T> &i1, state_idx<T> &i2, */
/* 		       state_idx<T> &energy); */
/*     //! bprop: compute diaghession wrt inputs, given diaghessian wrt output */
/*     virtual void bbprop(state_idx<T> &i1, state_idx<T> &i2, */
/* 			state_idx<T> &energy); */

/*     virtual void bprop1_copy(state_idx<T> &i1, state_idx<T> &i2, */
/* 			     state_idx<T> &energy); */
/*     virtual void bprop2_copy(state_idx<T> &i1, state_idx<T> &i2, */
/* 			     state_idx<T> &energy); */
/*     virtual void bbprop1_copy(state_idx<T> &i1, state_idx<T> &i2, */
/* 			      state_idx<T> &energy); */
/*     virtual void bbprop2_copy(state_idx<T> &i1, state_idx<T> &i2, */
/* 			      state_idx<T> &energy); */
/*     virtual void forget(forget_param_linear &fp); */
/*     virtual void normalize(); */

/*     //! compute value of in1 that minimizes the energy, given in2 */
/*     virtual double infer1(state_idx<T> &i1, state_idx<T> &i2, */
/* 			  state_idx<T> &energy, */
/* 			  infer_param &ip); */
/*     //! compute value of in2 that minimizes the energy, given in1 */
/*     //! if label is given, fill the corresponding energy. */
/*     virtual double infer2(state_idx<T> &i1, state_idx<T> &i2, infer_param &ip, */
/* 			  state_idx<T> *label = NULL, */
/* 			  state_idx<T> *energy = NULL); */
/*     virtual void infer2_copy(state_idx<T> &i1, state_idx<T> &i2, */
/* 			     state_idx<T> &energy); */
  };

  ////////////////////////////////////////////////////////////////
  // generic architectures

  template<class T> 
    class layers_2
    : public layers_2_gen<state_idx<T>, state_idx<T>, state_idx<T> > {
  public: 
     layers_2(module_1_1<T> &l1, state_idx<T> &h, module_1_1<T> &l2); 
/*     virtual ~layers_2(); */
/*     virtual void fprop(state_idx<T> &in, state_idx<T> &out); */
/*     virtual void bprop(state_idx<T> &in, state_idx<T> &out); */
/*     virtual void bbprop(state_idx<T> &in, state_idx<T> &out); */
/*     virtual void forget(forget_param &fp); */
/*     virtual void normalize(); */
  };

  template<class T> class layers
    : public layers_gen<state_idx<T> > {
  public:
/*     //! constructor. */
/*     layers(); */
    //! constructor. if oc is true, then this module owns its content and
    //! is responsible for deleting modules that are given to it.
    layers(bool oc);
/*     //! destructor. */
/*     virtual ~layers(); */
    
    //! Add a module to the stack of modules. The 'hidden' buffer
    //! between modules is optional, and will be automatically allocated
    //! if NULL.
    virtual void add_module(module_1_1<T>* module,
			    state_idx<T>* hidden = NULL);
    virtual void fprop(state_idx<T> &in, state_idx<T> &out);
    
/*     virtual void bprop(state_idx<T> &in, state_idx<T> &out); */
/*     virtual void bbprop(state_idx<T> &in, state_idx<T> &out); */
/*     virtual void forget(forget_param_linear &fp); */
/*     virtual void normalize(); */
/*     virtual idxdim fprop_size(idxdim &isize); */
/*     virtual idxdim bprop_size(const idxdim &osize); */
/*     virtual void pretty(idxdim &isize); */
    
    //! Returns a deep copy of current module.
    virtual layers<T>* copy();

    // class member variables
  public:
    using layers_gen<state_idx<T> >::modules;
    using layers_gen<state_idx<T> >::hiddens;
  };

  ////////////////////////////////////////////////////////////////
  //! standard 1 input EBM with one module-1-1, and one ebm-1 on top.
  //! fc stands for "function+cost".
  template<class T> class fc_ebm1
    : public fc_ebm1_gen<state_idx<T>, state_idx<T>, T> {
/*   public: */
/*     fc_ebm1(module_1_1<T> &fm, state_idx<T> &fo, ebm_1<T> &fc); */
/*     virtual ~fc_ebm1(); */
/*     virtual void fprop(state_idx<T> &in, state_idx<T> &energy); */
/*     virtual void bprop(state_idx<T> &in, state_idx<T> &energy); */
/*     virtual void bbprop(state_idx<T> &in, state_idx<T> &energy); */
/*     virtual void forget(forget_param &fp); */
  };

  ////////////////////////////////////////////////////////////////
  //! standard 2 input EBM with one module-1-1, and one ebm-2 on top.
  //! fc stands for "function+cost".
  template<class T> class fc_ebm2
    : public fc_ebm2_gen<state_idx<T>, state_idx<T>, T> {
/*   public: */
/*     fc_ebm2(module_1_1<T> &fm, state_idx<T> &fo, ebm_2<T> &fc); */
/*     virtual ~fc_ebm2(); */
/*     virtual void fprop(state_idx<T> &in1, state_idx<T> &in2, */
/* 		       state_idx<T> &energy); */
/*     virtual void bprop(state_idx<T> &in1, state_idx<T> &in2, */
/* 		       state_idx<T> &energy); */
/*     virtual void bbprop(state_idx<T> &in1, state_idx<T> &in2, */
/* 			state_idx<T> &energy); */
/*     virtual void forget(forget_param_linear &fp); */
/*     virtual double infer2(state_idx<T> &i1, state_idx<T> &i2, infer_param &ip,  */
/* 			  int *label = NULL, state_idx<T> *energy = NULL); */
  };

  ////////////////////////////////////////////////////////////////
  // helper functions

  //! check that m and in are compatible for replication
  template<class T>
  void check_replicable_orders(module_1_1<T> &m, state_idx<T>& in);

  ////////////////////////////////////////////////////////////////
  // generic replicable module classes

  //! This modules loops over replicable dimensions and calls <module>'s
  //! functions when reaching <module>'s replicable_order.
  //! Note: Multiple inheritance could have been cleaner but would have required
  //! frequent dynamic_casts from the user.
  template<class T, class T2> class module_1_1_replicable {
  public:
    T &module;
    module_1_1_replicable(T &m);
    virtual ~module_1_1_replicable();
    virtual void fprop(state_idx<T2> &in, state_idx<T2> &out);
    virtual void bprop(state_idx<T2> &in, state_idx<T2> &out);
    virtual void bbprop(state_idx<T2> &in, state_idx<T2> &out);
  };

  //! A macro to declare a module_1_1<state_idx,state_idx> as replicable
  //! over extra dimensions beyond the module's replicable_order.
  //! \param replicable_module is the new class name
  //! \param base_module is the module_1_1<state_idx,state_idx> to be replicated
  //! \param type_arguments are the arguments to the base_module's constructor 
  //!   along with their type (use parenthesis)
  //! \param arguments are the naked arguments (use parenthesis)
  //! \example   DECLARE_REPLICABLE_MODULE_1_1(linear_module_replicable, 
  //!                                          linear_module,
  //!			                       (parameter &p, intg in, intg ou),
  //!			                       (p, in, out));
  //! Note: Multiple inheritance could have been cleaner but would have required
  //! frequent dynamic_casts from the user.
#define DECLARE_REPLICABLE_MODULE_1_1(replicable_module, base_module,	\
				      types_arguments, arguments)	\
  template <class T>							\
    class replicable_module : public base_module {			\
  public:								\
    module_1_1_replicable<base_module, T> rep;				\
    replicable_module types_arguments : base_module arguments, rep(*this) { \
      this->bResize = false;						\
      if (this->replicable_order() <= 0)				\
	eblerror("this module is not replicable"); }			\
    virtual ~replicable_module() {}					\
    virtual void fprop(state_idx<T> &in, state_idx<T> &out)		\
    { rep.fprop(in, out); }						\
    virtual void bprop(state_idx<T> &in, state_idx<T> &out)		\
    { rep.bprop(in, out); }						\
    virtual void bbprop(state_idx<T> &in, state_idx<T> &out)		\
    { rep.bbprop(in, out); }						\
  }

} // namespace ebl {

#include "ebl_arch.hpp"

#endif /* EBL_ARCH_H_ */
