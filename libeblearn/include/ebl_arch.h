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

#include "libidx.h"
#include "ebl_defines.h"
#include "ebl_states.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // module_1_1

#define fs(T) T,fstate_idx<T>
#define bs(T) T,bstate_idx<T>
#define bbs(T) T,bbstate_idx<T>
#define check_bstate(ptr) \
  if (!ptr) eblerror("module buffers should be bstate_idx")
#define check_bbstate(ptr) \
  if (!ptr) eblerror("module buffers should be bbstate_idx")
  
  //! An abstract class for a module with one input and one output.
  template<typename T, class Tin = bbstate_idx<T>, class Tout = Tin>
    class module_1_1 {
  public:
    module_1_1(bool bResize = true); //!< by default, resize output
    virtual ~module_1_1();
    virtual void fprop(Tin &in, Tout &out);
    virtual void bprop(Tin &in, Tout &out);
    virtual void bbprop(Tin &in, Tout &out);
    virtual void forget(forget_param_linear& fp);
    virtual void normalize();
    //! returns the order at which the module operates.
    virtual int  replicable_order();
    virtual void resize_output(Tin &in, Tout &out);
    //! given the input dimensions, modifies it to be compliant with module's
    //! architecture, and returns the output dimensions corresponding to
    //! modified input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual idxdim fprop_size(idxdim &i_size);
    //! given the output dimensions, returns the input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual idxdim bprop_size(const idxdim &o_size);
    //! prints the forward transformation of dimensions. this method calls
    //! fprop_size to determine the output size given the input.
  virtual void pretty(idxdim &isize);
    //! Returns a deep copy of current module.
  virtual module_1_1<T, Tin, Tout>* copy();
    
  protected:
    bool bResize; //!< tells module to resize output or not
    const char *name; //!< optional name of module.
  };

  ////////////////////////////////////////////////////////////////
  // module_2_1
  
  //! An abstract class for a module with two inputs and one output.
  template<typename T, class Tin1 = bbstate_idx<T>, class Tin2 = Tin1,
    class Tout = Tin1>
    class module_2_1 {
  public:
    virtual ~module_2_1() {};
    virtual void fprop(Tin1 &in1, Tin2 &in2, Tout &out);
    virtual void bprop(Tin1 &in1, Tin2 &in2, Tout &out);
    virtual void bbprop(Tin1 &in1, Tin2 &in2, Tout &out);
    virtual void forget(forget_param &fp);
    virtual void normalize();
  };

  ////////////////////////////////////////////////////////////////
  // ebm_1
  
  //! An abstract class for a module with one inputs and one energy output.
  template<typename T, class Tin = bbstate_idx<T>, class Ten = Tin>
    class ebm_1 {
  public:
    virtual ~ebm_1() {};
    virtual void fprop(Tin &in, Ten &energy);
    virtual void bprop(Tin &in, Ten &energy);
    virtual void bbprop(Tin &in, Ten &energy);
    virtual void forget(forget_param &fp);
    virtual void normalize();
  };

  ////////////////////////////////////////////////////////////////
  // ebm_2

  //! An abstract class for a module with two inputs and one energy output.
  template<class Tin1, class Tin2 = Tin1, class Ten = Tin1>
    class ebm_2 {
  public:
    virtual ~ebm_2() {};
    //! fprop: compute output from input
    virtual void fprop(Tin1 &i1, Tin2 &i2, Ten &energy);
    //! bprop: compute gradient wrt inputs, given gradient wrt output
    virtual void bprop(Tin1 &i1, Tin2 &i2, Ten &energy);
    //! bprop: compute diaghession wrt inputs, given diaghessian wrt output
    virtual void bbprop(Tin1 &i1, Tin2 &i2, Ten &energy);

    virtual void bprop1_copy(Tin1 &i1, Tin2 &i2, Ten &energy);
    virtual void bprop2_copy(Tin1 &i1, Tin2 &i2, Ten &energy);
    virtual void bbprop1_copy(Tin1 &i1, Tin2 &i2, Ten &energy);
    virtual void bbprop2_copy(Tin1 &i1, Tin2 &i2, Ten &energy);
    virtual void forget(forget_param_linear &fp);
    virtual void normalize();
    //! compute value of in1 that minimizes the energy, given in2
    virtual double infer1(Tin1 &i1, Tin2 &i2, Ten &energy, infer_param &ip);
    //! compute value of in2 that minimizes the energy, given in1
    //! if label is given, fill the corresponding energy.
    virtual double infer2(Tin1 &i1, Tin2 &i2, infer_param &ip,
			  Tin2 *label = NULL, Ten *energy = NULL);
    virtual void infer2_copy(Tin1 &i1, Tin2 &i2, Ten &energy);
  };

  ////////////////////////////////////////////////////////////////
  // layers

  //! A stack of module_1_1 modules.
  template<typename T, class Tstate = bbstate_idx<T> >
    class layers : public module_1_1<T, Tstate, Tstate> {
  public:
    //! If oc is true, this class owns all its content and is responsible for
    //! deleting modules and buffers.
    //! hi and ho are buffers used over and over by swapping them between
    //! each operation for memory efficency. If they are both not null,
    //! then the buffers given as parameters to the fprop function are ignored,
    //! assuming that hi will contain the input data. When using this
    //! memory efficiency mechanism, bprop and bbprop cannot be called as they
    //! require keeping buffers for each fprop operation.
    //! \param hi The address of the input buffer used for memory efficiency.
    //! \param ho The address of the output buffer used for memory
    //!   efficiency.
    layers(bool oc = true, Tstate* hi = NULL, Tstate* ho = NULL);
    virtual ~layers();
    //! Add a module to the stack of modules.
    virtual void add_module(module_1_1<T, Tstate, Tstate>* module);
    virtual void fprop(Tstate &in, Tstate &out);
    virtual void bprop(Tstate &in, Tstate &out);
    virtual void bbprop(Tstate &in, Tstate &out);
    virtual void forget(forget_param_linear &fp);
    virtual void normalize();
    //! given the input dimensions, modifies it to be compliant with module's
    //! architecture, and returns the output dimensions corresponding to
    //! modified input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual idxdim fprop_size(idxdim &i_size);
    //! given the output dimensions, returns the input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual idxdim bprop_size(const idxdim &o_size);
    //! prints the forward transformation of dimensions. this method calls
    //! fprop_size to determine the output size given the input.
    virtual void pretty(idxdim &isize);
    //! Returns a deep copy of current module.
    virtual layers<T, Tstate>* copy();
    //! Swap the dual buffers used for memory optimization.
    virtual void swap_buffers();

    // friends
    friend class layers_gui;
    friend class layers_gui;
    
    // class member variables
  public:
  vector<module_1_1<T, Tstate, Tstate>*>	*modules;
    vector<Tstate*>				*hiddens;    
  protected:
    bool own_contents;
    bool mem_optimization; //! we are using dual buffer memory optimization.
    Tstate* hi; //! temporary buffer pointer
    Tstate* ho; //! temporary buffer pointer
    Tstate* htmp; //! temporary buffer pointer used for swapping
    Tstate* hi0; //! original input buffer for optimization.
    Tstate* ho0; //! original output dual for optimization.
  };

  ////////////////////////////////////////////////////////////////
  // layers_2

  template<typename T, class Tin = bbstate_idx<T>, class Thid = Tin,
    class Tout = Tin> 
    class layers_2 : public module_1_1<T, Tin, Tout> {
  public:
    module_1_1<T, Tin, Thid>	&layer1;
    Thid	        	&hidden;
    module_1_1<T, Thid, Tout>	&layer2;

    layers_2(module_1_1<T, Tin, Thid> &l1, Thid &h,
	     module_1_1<T, Thid, Tout> &l2);
    virtual ~layers_2();
    virtual void fprop(Tin &in, Tout &out);
    virtual void bprop(Tin &in, Tout &out);
    virtual void bbprop(Tin &in, Tout &out);
    virtual void forget(forget_param_linear &fp);
    virtual void normalize();
    //! given the input dimensions, modifies it to be compliant with module's
    //! architecture, and returns the output dimensions corresponding to
    //! modified input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual idxdim fprop_size(idxdim &i_size);
    //! given the output dimensions, returns the input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual idxdim bprop_size(const idxdim &o_size);
    //! prints the forward transformation of dimensions. this method calls
    //! fprop_size to determine the output size given the input.
    virtual void pretty(idxdim &isize);
  };

  ////////////////////////////////////////////////////////////////
  //! standard 1 input EBM with one module-1-1, and one ebm-1 on top.
  //! fc stands for "function+cost".
  template<typename T, class Tin = bbstate_idx<T>, class Thid = Tin,
    class Ten = Tin>
    class fc_ebm1 : public ebm_1<T, Tin, Ten> {
  public:
    module_1_1<T,Tin,Thid>	&fmod;
    Thid			&fout;
    ebm_1<T,Thid,Ten>		&fcost;

    fc_ebm1(module_1_1<T,Tin,Thid> &fm, Thid &fo, ebm_1<T,Thid,Ten> &fc);
    virtual ~fc_ebm1();

    virtual void fprop(Tin &in, Ten &energy);
    virtual void bprop(Tin &in, Ten &energy);
    virtual void bbprop(Tin &in, Ten &energy);
    virtual void forget(forget_param &fp);
  };

  ////////////////////////////////////////////////////////////////
  //! standard 2 input EBM with one module-1-1, and one ebm-2 on top.
  //! fc stands for "function+cost".
  template<typename T, class Tin1 = bbstate_idx<T>, class Tin2 = Tin1,
    class Ten = Tin1> 
    class fc_ebm2 : public ebm_2<Tin1, Tin2, Ten> {
  public:
    module_1_1<T, Tin1, Tin1>	&fmod;
    Tin1			&fout;
    ebm_2<Tin1, Tin2, Ten>	&fcost;

    fc_ebm2(module_1_1<T, Tin1> &fm, Tin1 &fo, ebm_2<Tin1, Tin2, Ten> &fc);
    virtual ~fc_ebm2();

    virtual void fprop(Tin1 &in1, Tin2 &in2, Ten &energy);
    virtual void bprop(Tin1 &in1, Tin2 &in2, Ten &energy);
    virtual void bbprop(Tin1 &in1, Tin2 &in2, Ten &energy);
    virtual void forget(forget_param_linear &fp);
    virtual double infer2(Tin1 &i1, Tin2 &i2, infer_param &ip, 
			  Tin2 *label = NULL, Ten *energy = NULL);
  };

  ////////////////////////////////////////////////////////////////
  // helper functions

  //! check that m and in are compatible for replication
  template<typename T, class Tstate>
    void check_replicable_orders(module_1_1<T,Tstate> &m, Tstate& in);

  ////////////////////////////////////////////////////////////////
  // generic replicable module classes

  //! This modules loops over replicable dimensions and calls <module>'s
  //! functions when reaching <module>'s replicable_order.
  //! Note: Multiple inheritance could have been cleaner but would have required
  //! frequent dynamic_casts from the user.
  template<class Tmodule, typename T, class Tstate = bbstate_idx<T> >
    class module_1_1_replicable {
  public:
    Tmodule &module;
    module_1_1_replicable(Tmodule &m);
    virtual ~module_1_1_replicable();
    virtual void fprop(Tstate &in, Tstate &out);
    virtual void bprop(Tstate &in, Tstate &out);
    virtual void bbprop(Tstate &in, Tstate &out);
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
				      T, Tstate,			\
				      types_arguments, arguments)	\
  template <typename T, class Tstate = bbstate_idx<T> >			\
    class replicable_module : public base_module<T,Tstate> {		\
  public:								\
    module_1_1_replicable<base_module<T,Tstate>,T,Tstate> *rep;		\
    replicable_module types_arguments : base_module<T,Tstate> arguments { \
      rep = new module_1_1_replicable<base_module<T,Tstate>,T,Tstate>(*this); \
      this->bResize = false;						\
      if (this->replicable_order() <= 0)				\
	eblerror("this module is not replicable"); }			\
    virtual ~replicable_module() { delete rep; }			\
    virtual void fprop(Tstate &in, Tstate &out)				\
    { rep->fprop(in, out); }						\
    virtual void bprop(Tstate &in, Tstate &out)				\
    { rep->bprop(in, out); }						\
    virtual void bbprop(Tstate &in, Tstate &out)			\
    { rep->bbprop(in, out); }						\
  }

} // namespace ebl {

#include "ebl_arch.hpp"

#endif /* EBL_ARCH_H_ */
