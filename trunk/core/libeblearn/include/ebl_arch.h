/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef EBL_ARCH_H_
#define EBL_ARCH_H_

#include "libidx.h"
#include "ebl_defines.h"
#include "ebl_states.h"

#ifndef __NOSTL__
#include <vector>
#endif

using namespace std;

namespace ebl {

#define check_bstate(ptr) \
  if (!ptr) eblerror("module buffers should be bstate_idx")
#define check_bbstate(ptr) \
  if (!ptr) eblerror("module buffers should be bbstate_idx")


  ////////////////////////////////////////////////////////////////
  // module

  //! A module class containing a name.
  class EXPORT module {
  public:
    module(const char *name = "module");
    virtual ~module();
    //! Return the name of this module.
    virtual const char* name();
    //! Set the name of this module to 'name'.
    virtual void set_name(const char *name);
    //! Replace standard output streams for this module.
    virtual void set_output_streams(std::ostream &out, std::ostream &err);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns a string describing this module and its parameters.
    virtual std::string describe(uint indent);
    //! Allow this module to process.
    virtual void enable();
    //! Forbid this module to process.
    virtual void disable();
    // members /////////////////////////////////////////////////////////////////
  protected:
    std::string _name; //!< Name of this module.
    std::ostream *mout, *merr; //!< Output streams.
    bool silent;
    bool _enabled;
  };

  ////////////////////////////////////////////////////////////////
  // module_1_1

  //! An abstract class for a module with one input and one output.
  template<typename T, class Tin = bbstate_idx<T>, class Tout = Tin>
    class EXPORT module_1_1 : public module {
  public:
    //! \param bresize By default, resize output.
    module_1_1(const char *name = "module_1_1", bool bresize = true);
    virtual ~module_1_1();
    // generic states methods //////////////////////////////////////////////////
    virtual void fprop(Tin &in, Tout &out);
    virtual void bprop(Tin &in, Tout &out);
    virtual void bbprop(Tin &in, Tout &out);
    //! Calls fprop and then dumps internal buffers, inputs and outputs
    //! into files. This can be useful for debugging.
    virtual void dump_fprop(Tin &in, Tout &out);
    // multi-states methods ////////////////////////////////////////////////////
    virtual void fprop(mstate<Tin> &in, mstate<Tout> &out);
    virtual void bprop(mstate<Tin> &in, mstate<Tout> &out);
    virtual void bbprop(mstate<Tin> &in, mstate<Tout> &out);
    //! Calls fprop and then dumps internal buffers, inputs and outputs
    //! into files. This can be useful for debugging.
    virtual void dump_fprop(mstate<Tin> &in, mstate<Tout> &out);
    // multi-states to single-state methods ////////////////////////////////////
    virtual void fprop(mstate<Tin> &in, Tout &out);
    virtual void bprop(mstate<Tin> &in, Tout &out);
    virtual void bbprop(mstate<Tin> &in, Tout &out);
    //! Calls fprop and then dumps internal buffers, inputs and outputs
    //! into files. This can be useful for debugging.
    virtual void dump_fprop(mstate<Tin> &in, Tout &out);
    // single-states to multi-state methods ////////////////////////////////////
    virtual void fprop(Tin &in, mstate<Tout> &out);
    virtual void bprop(Tin &in, mstate<Tout> &out);
    virtual void bbprop(Tin &in, mstate<Tout> &out);
    //! Calls fprop and then dumps internal buffers, inputs and outputs
    //! into files. This can be useful for debugging.
    virtual void dump_fprop(Tin &in, mstate<Tout> &out);
    ////////////////////////////////////////////////////////////////////////////
    virtual void forget(forget_param_linear& fp);
    virtual void normalize();
    //! returns the order at which the module operates.
    virtual int  replicable_order();
    // resizing ////////////////////////////////////////////////////////////////
    //! This tests if this module is enabled, if it is it returns false,
    //! otherwise simply copies data from in to out and returns true;
    virtual bool ignored(Tin &in, Tout &out);
    //! Resizes 'out' to the same dimensions as 'in'. If dimensions already
    //! match, nothing is changed. If orders differ, then out is assigned a new
    //! 'Tout' buffer if the correct order and dimensions.
    //! \param d If not null, use these dimensions as target, otherwise use in.x
    //! This returns true if output was resized/reallocated, false otherwise.
    virtual bool resize_output(Tin &in, Tout &out, idxdim *d = NULL);
    //! Resizes 'out' to the same dimensions as 'in'. If dimensions already
    //! match, nothing is changed. If orders differ, then out is assigned a new
    //! 'Tout' buffer if the correct order and dimensions.
    //! \param d If not null, use these dimensions as target, otherwise use in.x
    //! This returns true if output was resized/reallocated, false otherwise.
    virtual bool resize_output(Tin &in, idx<T> &out, idxdim *d = NULL);
    //! Modifies input dimensions 'isize' to be compliant with module's
    //! architecture, and returns corresponding output dimensions.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim fprop_size(fidxdim &isize);
    //! Returns input dimensions corresponding to output dimensions 'osize'.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim bprop_size(const fidxdim &osize);
    //! Modifies multi-input dimensions 'isize' to be compliant with module's
    //! architecture, and returns corresponding output dimensions.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim fprop_size(mfidxdim &isize);
    /* //! Returns input dimensions corresponding to multiple output dimensions */
    /* //! 'osize'. Implementation of this method helps automatic scaling of input */
    /* //! data but is optional. */
    /* virtual mfidxdim bprop_size(const mfidxdim &osize); */
    //! Returns input dimensions corresponding to multiple output dimensions
    //! 'osize'. Implementation of this method helps automatic scaling of input
    //! data but is optional.
    virtual mfidxdim bprop_size(mfidxdim &osize);
    //! Prints the forward transformation of dimensions in a string and return
    //! it. This method calls fprop_size to determine the output size given
    //! the input.
    virtual std::string pretty(idxdim &isize);
    //! Prints the forward transformation of multi-dimensions in a string and
    //! return it. This method calls fprop_size to determine the output size
    //! given the input.
    virtual std::string pretty(mfidxdim &isize);
    //! Returns a deep copy of current module.
    virtual module_1_1<T, Tin, Tout>* copy();
    //! Returns a deep copy of current module.
    //! \param p If NULL, the copy points to the same weights as this module.
    virtual module_1_1<T, Tin, Tout>* copy(parameter<T,Tin> *p);
    //! Pre-determine the order of hidden buffers to use only 2 buffers
    //! in order to reduce memory footprint.
    //! This returns true if outputs is actually put in out, false if it's
    //! in in.
    virtual bool optimize_fprop(Tin &in, Tout &out);
    //! Memory optimization is not implemented in case of mstates (TODO).
    virtual bool optimize_fprop(mstate<Tin> &in, mstate<Tout> &out);
    //! Load internal weights of module with passed weights w.
    //! TODO: there should be not idx specialization at this level.
    virtual void load_x(idx<T> &weights);
    //! Returns the last module contained in this module, or itself if composed
    //! of only 1 module.
    virtual module_1_1<T,Tin,Tout>* last_module();
    //! Returns true if this module takes a multi-state as input.
    virtual bool mstate_input();
    //! Returns true if this module produces a multi-state as output.
    virtual bool mstate_output();
    //! Returns the number of input states after last fprop.
    virtual uint get_ninputs();
    //! Returns the number of output states after last fprop.
    virtual uint get_noutputs();

  // variable members //////////////////////////////////////////////////////////
  public:
    // these variables describe internal buffers declared to be displayed
    // by external display objects.
    std::vector<idx<T> >     internals;	//!< Internal buffers to display
    std::vector<std::string>	internals_str;	//!< Internal buffers desc.
  protected:
    bool			bresize; //!< Tells module to resize output.
    bool			memoptimized; //!< Using mem optim or not.
    bool			bmstate_input;	//!< Input is multi-state.
    bool			bmstate_output;	//!< Output is multi-state.
    uint			ninputs, noutputs; //!< Current # of i/o states.
  };

  ////////////////////////////////////////////////////////////////
  // module_2_1

  //! An abstract class for a module with two inputs and one output.
  template<typename T, class Tin1 = bbstate_idx<T>, class Tin2 = Tin1,
    class Tout = Tin1>
    class EXPORT module_2_1 : public module {
  public:
    module_2_1(const char *name = "module_2_1");
    virtual ~module_2_1();
    //////////////////////////////////////////////////////////////////
    // generic states methods
    virtual void fprop(Tin1 &in1, Tin2 &in2, Tout &out);
    virtual void bprop(Tin1 &in1, Tin2 &in2, Tout &out);
    virtual void bbprop(Tin1 &in1, Tin2 &in2, Tout &out);
    //////////////////////////////////////////////////////////////////
    // multi-states methods
    virtual void fprop(mstate<Tin1> &in1, mstate<Tin2> &in2,
		       mstate<Tout> &out);
    virtual void bprop(mstate<Tin1> &in1, mstate<Tin2> &in2,
		       mstate<Tout> &out);
    virtual void bbprop(mstate<Tin1> &in1, mstate<Tin2> &in2,
			mstate<Tout> &out);
    //////////////////////////////////////////////////////////////////
    virtual void forget(forget_param &fp);
    virtual void normalize();
    //! Resizes 'out' to the same dimensions as 'in'. If dimensions already
    //! match, nothing is changed. If orders differ, then out is assigned a new
    //! 'Tout' buffer if the correct order and dimensions.
    //! \param d If not null, use these dimensions as target, otherwise use in.x
    //! This returns true if output was resized/reallocated, false otherwise.
    virtual bool resize_output(Tin1 &in1, Tin2 &in2, Tout &out,
			       idxdim *d = NULL);

  protected:
    bool	 bresize;	//!< Tells module to resize output or not.
  };

  //! An abstract class for a module with one inputs and one energy output.
  template<typename T, class Tin = bbstate_idx<T>, class Ten = Tin>
    class ebm_1 : public module {
  public:
    ebm_1(const char *name = "ebm_1");
    virtual ~ebm_1();
    virtual void fprop(Tin &in, Ten &energy);
    virtual void bprop(Tin &in, Ten &energy);
    virtual void bbprop(Tin &in, Ten &energy);
    virtual void forget(forget_param &fp);
    virtual void normalize();
  };

  // ebm_module_1_1 ////////////////////////////////////////////////////////////

  //! A module containing both a module_1_1 that takes 1 input and produces
  //! 1 output but that also an ebm_1 that produces an energy given module_1_1's
  //! output.
  template<typename T, class Tin = bbstate_idx<T>, class Tout = Tin,
    class Ten = Tin>
    class ebm_module_1_1 : public module_1_1<T,Tin,Tout> {
  public:
    //! Construct with module 'm' and ebm 'e'.
    //! This module is responsible for deleting 'm' and 'e' at destruction.
    ebm_module_1_1(module_1_1<T,Tin,Tout> *m, ebm_1<T,Ten> *e,
		   const char *name = "ebm_module_1_1");
    virtual ~ebm_module_1_1();
    virtual void fprop(Tin &in, Tout &out);
    virtual void bprop(Tin &in, Tout &out);
    virtual void bbprop(Tin &in, Tout &out);
    virtual void forget(forget_param_linear &fp);
    //! Returns a reference to the energy output of fprop throught the ebm_1.
    virtual Ten& get_energy();
    //! Modifies input dimensions 'isize' to be compliant with module's
    //! architecture, and returns corresponding output dimensions.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim fprop_size(fidxdim &isize);
    //! Returns input dimensions corresponding to output dimensions 'osize'.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim bprop_size(const fidxdim &osize);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
  protected:
    module_1_1<T,Tin,Tout>	*module;
    ebm_1<T,Ten>		*ebm;
    Ten                          energy;
  };

  ////////////////////////////////////////////////////////////////
  // ebm_2

  //! An abstract class for a module with two inputs and one energy output.
  template<class Tin1, class Tin2 = Tin1, class Ten = Tin1>
    class ebm_2 : public module {
  public:
    ebm_2(const char *name = "ebm_2");
    virtual ~ebm_2();
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
    //! Constructor.
    //! \param oc If true, this class owns all its content and is responsible
    //! for deleting modules and buffers.
    //! \param is_branch If true, this branch will not modify its input
    //!   and output buffers, instead will output to public intern_out buffer.
    layers(bool oc = true, const char *name = "layers",
	   bool is_branch = false, bool narrow = false,
	   intg dim = 0, intg sz = 0, intg offset = 0);
    virtual ~layers();
    //! Add a module to the stack of modules.
    //! \param branch_id If > 0, add this module to branch with this id.
    virtual void add_module(module_1_1<T, Tstate, Tstate>* module);
    //! Pre-determine the order of hidden buffers to use only 2 buffers
    //! in order to reduce memory footprint.
    //! This returns true if outputs is actually put in out, false if it's
    //! in in.
  // TODO: fix optimize fprop
  //virtual bool optimize_fprop(Mstate &in, Mstate &out);
    // single states methods ///////////////////////////////////////////////////
    virtual void fprop(Tstate &in, Tstate &out);
    virtual void bprop(Tstate &in, Tstate &out);
    virtual void bbprop(Tstate &in, Tstate &out);
    //! Calls fprop and then dumps internal buffers, inputs and outputs
    //! into files. This can be useful for debugging.
    virtual void dump_fprop(Tstate &in, Tstate &out);
    // multi to single states methods //////////////////////////////////////////
    virtual void fprop(mstate<Tstate> &in, Tstate &out);
    virtual void bprop(mstate<Tstate> &in, Tstate &out);
    virtual void bbprop(mstate<Tstate> &in, Tstate &out);
    //! Calls fprop and then dumps internal buffers, inputs and outputs
    //! into files. This can be useful for debugging.
    virtual void dump_fprop(mstate<Tstate> &in, Tstate &out);
    // single to multi states methods //////////////////////////////////////////
    virtual void fprop(Tstate &in, mstate<Tstate> &out);
    virtual void bprop(Tstate &in, mstate<Tstate> &out);
    virtual void bbprop(Tstate &in, mstate<Tstate> &out);
    //! Calls fprop and then dumps internal buffers, inputs and outputs
    //! into files. This can be useful for debugging.
    virtual void dump_fprop(Tstate &in, mstate<Tstate> &out);
    // multi to multi methods //////////////////////////////////////////////////
    virtual void fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bbprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! Calls fprop and then dumps internal buffers, inputs and outputs
    //! into files. This can be useful for debugging.
    virtual void dump_fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //////////////////////////////////////////////////////////////////
    virtual void forget(forget_param_linear &fp);
    virtual void normalize();
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
    //! Modifies multi-input dimensions 'isize' to be compliant with module's
    //! architecture, and returns corresponding output dimensions.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim fprop_size(mfidxdim &isize);
    //! Given multiple output dimensions, returns the input dimensions.
    //! The implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim bprop_size(mfidxdim &o_size);
    //! Prints the forward transformation of dimensions in a string and return
    //! it. This method calls fprop_size to determine the output size given
    //! the input.
    virtual std::string pretty(idxdim &isize);
    //! Prints the forward transformation of multi-dimensions in a string and
    //! return it. This method calls fprop_size to determine the output size
    //! given the input.
    virtual std::string pretty(mfidxdim &isize);
    //! Returns a deep copy of current module.
    virtual layers<T,Tstate>* copy();
    //! Swap the dual buffers used for memory optimization.
    virtual void swap_buffers();
    //! Return the number of layers contained in this object.
    virtual uint size();
    //! Clear the hidden's states dx and recursively clear all branches
    //! contained in modules.
    virtual void clear_dx();
    //! Clear the hidden's states ddx and recursively clear all branches
    //! contained in modules.
    virtual void clear_ddx();
    //! Returns true if this layer is being used as a branch, false otherwise.
    bool is_branch();
    //! Find first module whose name matches 'name', return NULL if not found.
    module_1_1<T, Tstate, Tstate>* find(const char *name);
    //! Returns the last module contained in this module, or itself if composed
    //! of only 1 module.
    virtual module_1_1<T,Tstate>* last_module();
    //! Returns a string containing describe() string of all modules.
    virtual std::string describe(uint indent = 0);
    //! Returns true if this module takes a multi-state as input.
    virtual bool mstate_input();
    //! Returns true if this module produces a multi-state as output.
    virtual bool mstate_output();
    //! Replace standard output streams for this module.
    virtual void set_output_streams(std::ostream &out, std::ostream &err);

    // friends /////////////////////////////////////////////////////////////////
    friend class layers_gui;

    // member variables ////////////////////////////////////////////////////////
  public:
    std::vector<module_1_1<T, Tstate, Tstate>*>	modules;
    std::vector<mstate<Tstate>*>			hiddens;
    mstate<Tstate>* intern_out; //! internal output, set if this is a branch
  protected:
    bool own_contents;
    mstate<Tstate>* hi; //! temporary buffer pointer
    mstate<Tstate>* ho; //! temporary buffer pointer
    mstate<Tstate>* htmp; //! temporary buffer pointer used for swapping
    // used for parallelism ///////////////////////////////////////////////////
    bool branch; //! this is a branch or not
    mstate<Tstate>* intern_h0; //! internal buffer 0 if branch
    mstate<Tstate>* intern_h1; //! internal bufer 1 if branch
    bool branch_narrow; //! narrow input data for branch
    intg narrow_dim; //! The dimension to narrow
    intg narrow_size; //! The number of slices
    intg narrow_offset; //! The offset.
    mstate<Tstate> msin, msout; //! mstate holders for in and out.
  };

  ////////////////////////////////////////////////////////////////
  // layers_2

  template<typename T, class Tin = bbstate_idx<T>, class Thid = Tin,
    class Tout = Tin>
    class EXPORT layers_2 : public module_1_1<T, Tin, Tout> {
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
    virtual fidxdim fprop_size(fidxdim &i_size);
    //! given the output dimensions, returns the input dimensions.
    //! the implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim bprop_size(const fidxdim &o_size);
    //! Prints the forward transformation of dimensions in a string and return
    //! it. This method calls fprop_size to determine the output size given
    //! the input.
    virtual std::string pretty(idxdim &isize);
  };

  ////////////////////////////////////////////////////////////////
  //! standard 1 input EBM with one module-1-1, and one ebm-1 on top.
  //! fc stands for "function+cost".
  template<typename T, class Tin = bbstate_idx<T>, class Thid = Tin,
    class Ten = Tin>
    class EXPORT fc_ebm1 : public ebm_1<T, Tin, Ten> {
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
    class EXPORT fc_ebm2 : public ebm_2<Tin1, Tin2, Ten> {
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
    class EXPORT module_1_1_replicable {
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
    class EXPORT replicable_module : public base_module<T,Tstate> {		\
  public:								\
    module_1_1_replicable<base_module<T,Tstate>,T,Tstate> *rep;		\
    replicable_module types_arguments : base_module<T,Tstate> arguments { \
      rep = new module_1_1_replicable<base_module<T,Tstate>,T,Tstate>(*this); \
      this->bresize = false;						\
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

  //! This module narrow's its input into its output. By default, it narrows
  //! the number of states of multi-state inputs, but it can also narrow
  //! each state themselves.
  template<typename T, class Tstate = bbstate_idx<T> >
    class EXPORT narrow_module : public module_1_1<T,Tstate,Tstate> {
  public:
    //! Construct a narrow module, narrowing dimension 'd' of input to 'size',
    //! starting at 'offset'.
    //! \param narrow_states Narrow each state of multi-states instead of
    //!   narrowing multi-state themselves.
    narrow_module(int d, intg size, intg offset, bool narrow_states = false);
    //! Construct a narrow module, narrowing dimension 'd' of input to 'size',
    //! starting at multiple 'offsets'.
    //! \param narrow_states Narrow each state of multi-states instead of
    //!   narrowing multi-state themselves.
    narrow_module(int d, intg size, vector<intg> &offsets,
		  bool narrow_states = false, const char *name = "narrow");
    //! Destructor.
    virtual ~narrow_module();
    // multi-state inputs and outputs //////////////////////////////////////////
    virtual void fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bbprop(mstate<Tstate> &in, mstate<Tstate> &out);
    // single-state inputs and outputs /////////////////////////////////////////
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns a deep copy of current module.
    virtual narrow_module<T,Tstate>* copy();
    //! Modifies multi-input dimensions 'isize' to be compliant with module's
    //! architecture, and returns corresponding output dimensions.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim fprop_size(mfidxdim &isize);
    //! Returns input dimensions corresponding to multiple output dimensions
    //! 'osize'. Implementation of this method helps automatic scaling of input
    //! data but is optional.
    virtual mfidxdim bprop_size(mfidxdim &osize);
  protected:
    int dim; //!< Dimension to be narrowed.
    intg size; //!< Target size for narrowed dimension.
    vector<intg> offsets; //!< Offset in input for narrowing.
    bool narrow_states; //!< Narrow states instead of multi-states.
  };

  //! This module connects inputs and outputs with a connection table.
  template<typename T, class Tstate = bbstate_idx<T> >
    class EXPORT table_module : public module_1_1<T,Tstate,Tstate> {
  public:
    //! Outputs will be the inputs arranged according to the list of
    //! input indices 'inputs'.
    table_module(vector<intg> &inputs, intg total,
		 const char *name = "table_module");
    //! Destructor.
    virtual ~table_module();
    // multi-state inputs and outputs //////////////////////////////////////////
    virtual void fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bbprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns a deep copy of current module.
    virtual table_module<T,Tstate>* copy();
    //! Modifies multi-input dimensions 'isize' to be compliant with module's
    //! architecture, and returns corresponding output dimensions.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim fprop_size(mfidxdim &isize);
    //! Returns input dimensions corresponding to multiple output dimensions
    //! 'osize'. Implementation of this method helps automatic scaling of input
    //! data but is optional.
    virtual mfidxdim bprop_size(mfidxdim &osize);
  protected:
    vector<intg> table; //!< List of inputs indices.
    intg total; //!< Total number of inputs.
  };

  // network sizes methods /////////////////////////////////////////////////////

  //! Returns the minimum input size with order 'order'
  //! that can be fed to network m.
  template <typename T, class Tstate>
    EXPORT idxdim network_mindims(module_1_1<T,Tstate> &m, uint order);

} // namespace ebl {

#include "ebl_arch.hpp"

#endif /* EBL_ARCH_H_ */
