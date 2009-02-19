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

#ifndef EBLARCH_H_
#define EBLARCH_H_

#include "Defines.h"
#include "Idx.h"
#include "Blas.h"
#include "EblStates.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // templates for generic modules

  //! abstract class for a module with one input and one output.
  template<class Tin, class Tout> class module_1_1 {
  public:
    bool bResize; // tells module to resize output or not
    module_1_1() { bResize = true; } // by default, resize output
    virtual ~module_1_1() {}
    virtual void fprop(Tin *in, Tout *out);
    virtual void bprop(Tin *in, Tout *out);
    virtual void bbprop(Tin *in, Tout *out);
    virtual void forget(forget_param_linear& fp);
    virtual void normalize();
    //! returns the order at which the module operates.
    virtual int  replicable_order();
    virtual void resize_output(Tin *in, Tout *out);
  };

  ////////////////////////////////////////////////////////////////

  //! abstract class for a module with two inputs and one output.
  template<class Tin1, class Tin2, class Tout> class module_2_1 {
  public:
    virtual ~module_2_1() {};
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

  template<class Tin, class Thid, class Tout> 
    class layers_2: public module_1_1<Tin, Tout> {
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
  template<class Tin1, class Tin2, class Thid> 
    class fc_ebm2: public ebm_2<Tin1, Tin2> {
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
  // helper functions

  //! check that m and in are compatible for replication
  void check_replicable_orders(module_1_1<state_idx, state_idx> *m, 
			       state_idx* in);

  ////////////////////////////////////////////////////////////////
  // generic replicable module classes

  //! This modules loops over replicable dimensions and calls <module>'s
  //! functions when reaching <module>'s replicable_order.
  //! Note: Multiple inheritance could have been cleaner but would have required
  //! frequent dynamic_casts from the user.
  template<class T> class module_1_1_replicable {
  public:
    T *module;
    module_1_1_replicable(T *m);
    virtual ~module_1_1_replicable();
    virtual void fprop(state_idx *in, state_idx *out);
    virtual void bprop(state_idx *in, state_idx *out);
    virtual void bbprop(state_idx *in, state_idx *out);
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
  //!			                       (parameter *p, intg in, intg ou),
  //!			                       (p, in, out));
  //! Note: Multiple inheritance could have been cleaner but would have required
  //! frequent dynamic_casts from the user.
#define DECLARE_REPLICABLE_MODULE_1_1(replicable_module, base_module,	\
				  types_arguments, arguments)		\
  class replicable_module : public base_module {			\
  public:								\
    module_1_1_replicable<base_module> rep;				\
    replicable_module types_arguments : base_module arguments, rep(this) { \
      bResize = false; }						\
    virtual ~replicable_module() {}					\
    virtual void fprop(state_idx *in, state_idx *out) { rep.fprop(in, out); } \
    virtual void bprop(state_idx *in, state_idx *out) { rep.bprop(in, out); } \
    virtual void bbprop(state_idx *in, state_idx *out){ rep.bbprop(in, out); }\
    }


} // namespace ebl {

#include "EblArch.hpp"

#endif /* EBLARCH_H_ */
