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

#ifndef EBL_ARCH_GEN_H_
#define EBL_ARCH_GEN_H_

#include "libidx.h"
#include "ebl_defines.h"
#include "ebl_states.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // templates for generic modules

  //! abstract class for a module with one input and one output.
  template<class Tin, class Tout>
    class module_1_1_gen {
  public:
    module_1_1_gen(bool bResize = true); //!< by default, resize output
    virtual ~module_1_1_gen() {}
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
    virtual module_1_1_gen<Tin, Tout>* copy();
    
  protected:
    bool bResize; //!< tells module to resize output or not
  };

  //! abstract class for a module with two inputs and one output.
  template<class Tin1, class Tin2, class Tout>
    class module_2_1_gen {
  public:
    virtual ~module_2_1_gen() {};
    virtual void fprop(Tin1 &in1, Tin2 &in2, Tout &out);
    virtual void bprop(Tin1 &in1, Tin2 &in2, Tout &out);
    virtual void bbprop(Tin1 &in1, Tin2 &in2, Tout &out);
    virtual void forget(forget_param &fp);
    virtual void normalize();
  };

  ////////////////////////////////////////////////////////////////
  //! abstract class for a module with one inputs and one energy output.
  template<class Tin, class T>
    class ebm_1_gen {
  public:
    virtual ~ebm_1_gen() {};
    virtual void fprop(Tin &in, state_idx<T> &energy);
    virtual void bprop(Tin &in, state_idx<T> &energy);
    virtual void bbprop(Tin &in, state_idx<T> &energy);
    virtual void forget(forget_param &fp);
    virtual void normalize();
  };

  ////////////////////////////////////////////////////////////////
  //! abstract class for a module with two inputs and one energy output.
  template<class Tin1, class Tin2, class T>
    class ebm_2_gen {
  public:
    virtual ~ebm_2_gen() {};
    //! fprop: compute output from input
    virtual void fprop(Tin1 &i1, Tin2 &i2, state_idx<T> &energy);
    //! bprop: compute gradient wrt inputs, given gradient wrt output
    virtual void bprop(Tin1 &i1, Tin2 &i2, state_idx<T> &energy);
    //! bprop: compute diaghession wrt inputs, given diaghessian wrt output
    virtual void bbprop(Tin1 &i1, Tin2 &i2, state_idx<T> &energy);

    virtual void bprop1_copy(Tin1 &i1, Tin2 &i2, state_idx<T> &energy);
    virtual void bprop2_copy(Tin1 &i1, Tin2 &i2, state_idx<T> &energy);
    virtual void bbprop1_copy(Tin1 &i1, Tin2 &i2, state_idx<T> &energy);
    virtual void bbprop2_copy(Tin1 &i1, Tin2 &i2, state_idx<T> &energy);
    virtual void forget(forget_param_linear &fp);
    virtual void normalize();

    //! compute value of in1 that minimizes the energy, given in2
    virtual double infer1(Tin1 &i1, Tin2 &i2, state_idx<T> &energy,
			  infer_param &ip);
    //! compute value of in2 that minimizes the energy, given in1
    //! if label is given, fill the corresponding energy.
    virtual double infer2(Tin1 &i1, Tin2 &i2, infer_param &ip,
			  Tin2 *label = NULL, state_idx<T> *energy = NULL);
    virtual void infer2_copy(Tin1 &i1, Tin2 &i2, state_idx<T> &energy);
  };

  ////////////////////////////////////////////////////////////////
  // generic architectures

  template<class Tin, class Thid, class Tout> 
    class layers_2_gen: public module_1_1_gen<Tin, Tout> {
  public:
    module_1_1_gen<Tin, Thid>	&layer1;
    Thid			&hidden;
    module_1_1_gen<Thid, Tout>	&layer2;

    layers_2_gen(module_1_1_gen<Tin, Thid> &l1, Thid &h,
		 module_1_1_gen<Thid, Tout> &l2);
    virtual ~layers_2_gen();
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

  template<class T> class layers_n_gen: public module_1_1_gen<T, T> {
  public:
    vector<module_1_1_gen<T, T>*>	*modules;
    vector<T*>				*hiddens;

    //! If oc is true, this class owns all its content and is responsible for
    //! deleting modules and buffers.
    layers_n_gen(bool oc = true);
    virtual ~layers_n_gen();
    //! Add a module to the stack of modules.
    virtual void add_module(module_1_1_gen <T, T>* module, T* hidden);
    virtual void add_last_module(module_1_1_gen <T, T>* module);
    virtual void fprop(T &in, T &out);
    virtual void bprop(T &in, T &out);
    virtual void bbprop(T &in, T &out);
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
    virtual layers_n_gen<T>* copy();

  protected:
    bool own_contents;
  };

  ////////////////////////////////////////////////////////////////
  //! standard 1 input EBM with one module-1-1, and one ebm-1 on top.
  //! fc stands for "function+cost".
  template<class Tin, class Thid, class T> class fc_ebm1_gen
    : public ebm_1_gen<Tin, T> {
  public:
    module_1_1_gen<Tin, Thid>	&fmod;
    Thid			&fout;
    ebm_1_gen<Thid,T>		&fcost;

    fc_ebm1_gen(module_1_1_gen<Tin, Thid> &fm, Thid &fo,
		ebm_1_gen<Thid, T> &fc);
    virtual ~fc_ebm1_gen();

    virtual void fprop(Tin &in, state_idx<T> &energy);
    virtual void bprop(Tin &in, state_idx<T> &energy);
    virtual void bbprop(Tin &in, state_idx<T> &energy);
    virtual void forget(forget_param &fp);
  };

  ////////////////////////////////////////////////////////////////
  //! standard 2 input EBM with one module-1-1, and one ebm-2 on top.
  //! fc stands for "function+cost".
  template<class Tin1, class Tin2, class T> 
    class fc_ebm2_gen: public ebm_2_gen<Tin1, Tin2, T> {
  public:
    module_1_1_gen<Tin1, Tin1>	&fmod;
    Tin1			&fout;
    ebm_2_gen<Tin1, Tin2, T>	&fcost;

    fc_ebm2_gen(module_1_1_gen<Tin1, Tin1> &fm, Tin1 &fo,
		ebm_2_gen<Tin1, Tin2, T> &fc);
    virtual ~fc_ebm2_gen();

    virtual void fprop(Tin1 &in1, Tin2 &in2, state_idx<T> &energy);
    virtual void bprop(Tin1 &in1, Tin2 &in2, state_idx<T> &energy);
    virtual void bbprop(Tin1 &in1, Tin2 &in2, state_idx<T> &energy);
    virtual void forget(forget_param_linear &fp);
    virtual double infer2(Tin1 &i1, Tin2 &i2, infer_param &ip, 
			  Tin2 *label = NULL, state_idx<T> *energy = NULL);
  };

} // namespace ebl {

#include "ebl_arch_gen.hpp"

#endif /* EBL_ARCH_GEN_H_ */
