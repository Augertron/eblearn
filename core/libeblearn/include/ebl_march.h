/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
 *   All rights reserved.
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

#ifndef EBL_MARCH_H_
#define EBL_MARCH_H_

#include "ebl_arch.h"

namespace ebl {

  //////////////////////////////////////////////////////////////////////////////
  //! Single-state input to multi-state output module.
  template<typename T, class Tin = bbstate_idx<T>, class Tout = Tin>
    class s2m_module : virtual public module_1_1<T,Tin,Tout> {
  public:
    //! Constructs a module that take a single-state input and produces
    //! a multi-state output with 'nstates' states.
    s2m_module(uint nstates, const char *name = "s2m_module");
    virtual ~s2m_module();
    virtual void fprop(Tin &in, mstate<Tout> &out);
    virtual void bprop(Tin &in, mstate<Tout> &out);
    virtual void bbprop(Tin &in, mstate<Tout> &out);
    //! Returns input dimensions corresponding to output dimensions 'osize'.
    virtual mfidxdim bprop_size(mfidxdim &osize);
    //! Returns the number of states produced by this module.
    virtual uint nstates();
  protected:
    //! Resize out based on the number of target states and the order of in.
    //! By default, states will have the order of in but each dimension will
    //! be the minimum size (1). If 'd' is not null, then d are the dimensions
    //! used to allocate the states.
    virtual void resize_output(Tin &in, mstate<Tout> &out, idxdim *d = NULL);
    // members /////////////////////////////////////////////////////////////////
  protected:
    uint _nstates;
  };

  //////////////////////////////////////////////////////////////////////////////
  //! Multi-state input to single-state output module.
  template<typename T, class Tin = bbstate_idx<T>, class Tout = Tin>
    class m2s_module : virtual public module_1_1<T,Tin,Tout> {
  public:
    //! Constructs a module that take a multi-state input with 'nstates' states
    //! and produces a single-state output.
    m2s_module(uint nstates, const char *name = "m2s_module");
    virtual ~m2s_module();
    virtual void fprop(mstate<Tin> &in, Tout &out);
    virtual void bprop(mstate<Tin> &in, Tout &out);
    virtual void bbprop(mstate<Tin> &in, Tout &out);
    //! Returns multiple input dimensions corresponding to output dims 'osize'.
    virtual mfidxdim bprop_size(mfidxdim &osize);
    //! Returns the number of states expected in input.
    virtual uint nstates();
    // members /////////////////////////////////////////////////////////////////
  protected:
    uint _nstates;
  };

  //////////////////////////////////////////////////////////////////////////////
  //! A container for one or multiple modules with one input and one output
  //! that processes a pipeline between a single-to-multi-state module
  //! and a multi-to-single-state module.
  //! Multiple pipes can be defined for multiple inputs, or a single pipe
  //! can be reused for all inputs.
  template<typename T, class Tstate = bbstate_idx<T> >
    class ms_module : public module_1_1<T,Tstate,Tstate> {
  public:
    // constructors ////////////////////////////////////////////////////////////
    //! Construct an empty multi-state module.
    ms_module(bool replicate_inputs = false, const char *name = "ms_module");
    //! Construct a multi-state module where 'pipe' is replicated 'n' times.
    ms_module(module_1_1<T,Tstate> *pipe, uint n = 1,
	      bool replicate_inputs = false, const char *name = "ms_module");
    //! Constructor a multi-state module where each state goes through
    //! one of 'pipes' pipe.
    ms_module(std::vector<module_1_1<T,Tstate>*> &pipes,
	      bool replicate_inputs = false, const char *name = "ms_module");
    //! Destructor.
    virtual ~ms_module();

    // multi-state inputs and outputs //////////////////////////////////////////
    virtual void fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bbprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! Calls fprop and then dumps internal buffers, inputs and outputs
    //! into files. This can be useful for debugging.
    virtual void dump_fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    ////////////////////////////////////////////////////////////////////////////
    virtual void forget(forget_param_linear& fp);

    // sizes propagations //////////////////////////////////////////////////////
    //! given the input dimensions, modifies it to be compliant with module's
    //! architecture, and returns the output dimensions corresponding to
    //! modified input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim fprop_size(fidxdim &i_size);
    //! Modifies multi-input dimensions 'isize' to be compliant with module's
    //! architecture, and returns corresponding output dimensions.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim fprop_size(mfidxdim &isize);
    //! given the output dimensions, returns the input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim bprop_size(const fidxdim &o_size);
    //! Given the multi-output dimensions, returns the multi-input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim bprop_size(mfidxdim &o_size);

    // printing ////////////////////////////////////////////////////////////////
    //! Prints the forward transformation of dimensions in a string and return
    //! it. This method calls fprop_size to determine the output size given
    //! the input.
    virtual std::string pretty(idxdim &isize);
    //! Prints the forward transformation of multi-dimensions in a string and
    //! return it. This method calls fprop_size to determine the output size
    //! given the input.
    virtual std::string pretty(mfidxdim &isize);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();

    // accessors ///////////////////////////////////////////////////////////////
    //! Only propagate in 1 pipe based on the size of the first input state.
    virtual void set_switch(midxdim &sizes);
    //! Only propagate in pipe with index 'id'.
    virtual void set_switch(intg id);
    //! Returns the number of pipes.
    virtual uint npipes();
    //! Returns pointer to pipe 'i'.
    virtual module_1_1<T,Tstate>* get_pipe(uint i);
    //! Returns the last module contained in this module, or itself if composed
    //! of only 1 module.
    virtual module_1_1<T,Tstate>* last_module();

    // internal methods ////////////////////////////////////////////////////////
  protected:
    //! Initializations.
    virtual void init();
    //! Prepare pipes and buffer for fprop.
    virtual void init_fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! Switch used_pipes on input size when switches are defined.
    virtual void switch_pipes(mstate<Tstate> &in);

    // variable members ////////////////////////////////////////////////////////
  protected:
    std::vector<module_1_1<T,Tstate>*> pipes; //!< All pipes/
    std::vector<module_1_1<T,Tstate>*> used_pipes; //!< Not always all pipes.
    std::vector<uint> pipes_noutputs; //!< Number of outputs for each pipe.
    svector<mstate<Tstate> > ins; //!< Inputs of each pipe.
    svector<mstate<Tstate> > mbuffers; //!< Multi-state buffers.
    bool replicate_inputs; //!< Replicate all inputs for each pipe.
    midxdim switches; //!< Only propagate 1 pipe based on input sizes.
    bool bindex; //!< If true, use switch id to switch.
    intg switch_id; //!< Index of pipe to switch to.

    // friends /////////////////////////////////////////////////////////////////
    template <typename T1, class Ts, class Tc>
      friend EXPORT Tc* arch_find(ms_module<T1,Ts> *m, Tc *c);
    template <typename T1, class Ts, class Tc>
    friend EXPORT std::vector<Tc*> arch_find_all(ms_module<T1,Ts> *m, Tc *c,
						 std::vector<Tc*> *);
    template <typename T1, class Ts, class Tc>
    friend EXPORT ms_module<T1,Ts>*
      arch_narrow(ms_module<T1,Ts> *m, Tc *c, bool i, bool *f);
    friend class ms_module_gui;
  };

  //////////////////////////////////////////////////////////////////////////////
  //! A "convolutional" version of ms_module, where pipes are applied to
  //! a set of inputs with a certain stride.
  template<typename T, class Tstate = bbstate_idx<T> >
    class msc_module : public ms_module<T,Tstate> {
  public:
    //! Constructor a multi-state module where each state goes through
    //! one of 'pipes' pipe,
    //! and each pipe sees groups of 'nsize' inputs with stride 'stride'.
    //! \param nsize2 If non-zero, this defines the size of higher level
    //!   groups of inputs, i.e. regular grouping will be applied to each of
    //!   these higher groups individually.
    msc_module(std::vector<module_1_1<T,Tstate>*> &pipes, uint nsize = 1,
	       uint stride = 1, uint nsize2 = 0,
	       const char *name = "msc_module");
    //! Destructor.
    virtual ~msc_module();
    ////////////////////////////////////////////////////////////////////////////
    //! given the input dimensions, modifies it to be compliant with module's
    //! architecture, and returns the output dimensions corresponding to
    //! modified input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim fprop_size(fidxdim &i_size);
    //! given the output dimensions, returns the input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim bprop_size(const fidxdim &o_size);
    //! Given the multi-output dimensions, returns the multi-input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim bprop_size(mfidxdim &o_size);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();

    // internal methods ////////////////////////////////////////////////////////
  protected:
    //! Prepare pipes and buffer for fprop.
    virtual void init_fprop(mstate<Tstate> &in, mstate<Tstate> &out);

  // variable members //////////////////////////////////////////////////////////
  protected:
    using ms_module<T,Tstate>::pipes;
    using ms_module<T,Tstate>::used_pipes;
    using ms_module<T,Tstate>::ins;
    using ms_module<T,Tstate>::pipes_noutputs;
    uint nsize; //! Groups size.
    uint stride; //! Stride for regular groups.
    uint nsize2; //! High level groups size.
  };

  // arch find methods /////////////////////////////////////////////////////////

  //! Returns first module contained in 'm',
  //! whose dynamic_cast<Tcast*> works, NULL otherwise.
  template <typename T, class Tstate, class Tcast>
    EXPORT Tcast* arch_find(module_1_1<T,Tstate> *m, Tcast *c);
  //! Returns first module contained in 'm',
  //! whose dynamic_cast<Tcast*> works, NULL otherwise.
  template <typename T, class Tstate, class Tcast>
    EXPORT Tcast* arch_find(layers<T,Tstate> *m, Tcast *c);
  //! Returns first module contained in 'm',
  //! whose dynamic_cast<Tcast*> works, NULL otherwise.
  template <typename T, class Tstate, class Tcast>
    EXPORT Tcast* arch_find(ms_module<T,Tstate> *m, Tcast *c);

  // arch find_all methods /////////////////////////////////////////////////////

  //! Returns all modules contained in 'm', whose dynamic_cast<Tcast*> match.
  //! If v is not null, fill v and return it, otherwise return a new vector.
  template <typename T, class Tstate, class Tcast>
    EXPORT std::vector<Tcast*> arch_find_all(module_1_1<T,Tstate> *m, Tcast *c,
					     std::vector<Tcast*> *v = NULL);
  //! Returns all modules contained in 'm', whose dynamic_cast<Tcast*> match.
  //! If v is not null, fill v and return it, otherwise return a new vector.
  template <typename T, class Tstate, class Tcast>
    EXPORT std::vector<Tcast*> arch_find_all(layers<T,Tstate> *m, Tcast *c,
					     std::vector<Tcast*> *v = NULL);
  //! Returns all modules contained in 'm', whose dynamic_cast<Tcast*> match.
  //! If v is not null, fill v and return it, otherwise return a new vector.
  template <typename T, class Tstate, class Tcast>
    EXPORT std::vector<Tcast*> arch_find_all(ms_module<T,Tstate> *m, Tcast *c,
					     std::vector<Tcast*> *v = NULL);

  // arch narrow methods ///////////////////////////////////////////////////////

  //! Returns a copy of network 'm' which is narrowed up to the first
  //! occurence of module of type 'Tcast' with same address 'c' (included).
  //! If no occurence is found, a copy of the entire network is returned.
  //! \param included If true, also include target module.
  template <typename T, class Tstate, class Tcast>
    EXPORT module_1_1<T,Tstate>*
    arch_narrow(module_1_1<T,Tstate> *m, Tcast *c, bool included = true,
		bool *found = NULL);
  //! Returns a copy of network 'm' which is narrowed up to the first
  //! occurence of module of type 'Tcast' with same address 'c' (included).
  //! If no occurence is found, a copy of the entire network is returned.
  //! \param included If true, also include target module.
  template <typename T, class Tstate, class Tcast>
    EXPORT layers<T,Tstate>*
    arch_narrow(layers<T,Tstate> *m, Tcast *c, bool included = true,
		bool *found = NULL);
  //! Returns a copy of network 'm' which is narrowed up to the first
  //! occurence of module of type 'Tcast' with same address 'c' (included).
  //! If no occurence is found, a copy of the entire network is returned.
  //! \param included If true, also include target module.
  template <typename T, class Tstate, class Tcast>
    EXPORT ms_module<T,Tstate>*
    arch_narrow(ms_module<T,Tstate> *m, Tcast *c, bool included = true,
		bool *found = NULL);

} // namespace ebl {

#include "ebl_march.hpp"

#endif /* EBL_MARCH_H_ */
