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

#ifndef EBL_MERGE_H_
#define EBL_MERGE_H_

#include "ebl_arch.h"
#include "ebl_march.h"
#include "ebl_basic.h"
#include "ebl_utils.h"

namespace ebl {

  // flat_merge ////////////////////////////////////////////////////////////////

  //! Forward declaration of zpad_module.
  template <typename T, class Tstate> class zpad_module;

  //! A module that flattens and concatenate multiple inputs.
  template <typename T, class Tstate = bbstate_idx<T> >
    class flat_merge_module : public m2s_module<T, Tstate> {
  public:
    //! Merging inputs of size 'ins' with their corresponding 'strides'.
    //! \param scales Specifies to which input scale each input corresponds to.
    flat_merge_module(midxdim &ins, mfidxdim &strides,
		      const char *name_ = "flatmerge", mfidxdim *scales = NULL);
    virtual ~flat_merge_module();

    // multi-states methods ////////////////////////////////////////////////////
    //! forward propagation from in to out
    virtual void fprop(mstate<Tstate> &in, Tstate &out);
    //! backward propagation from out to in
    virtual void bprop(mstate<Tstate> &in, Tstate &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(mstate<Tstate> &in, Tstate &out);

    ///////////////////////////////////////////////////////////////////////////
    //! Return dimensions that are compatible with this module.
    //! See module_1_1_gen's documentation for more details.
    virtual fidxdim fprop_size(fidxdim &i_size);
    //! Return dimensions that are compatible with this module.
    //! See module_1_1_gen's documentation for more details.
    virtual mfidxdim fprop_size(mfidxdim &i_size);
    //! Return dimensions compatible with this module given output dimensions.
    //! See module_1_1_gen's documentation for more details.
    virtual fidxdim bprop_size(const fidxdim &o_size);
    //! Returns multiple input dimensions corresponding to output dims 'osize'.
    virtual mfidxdim bprop_size(mfidxdim &osize);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns the number of expected inputs.
    virtual uint get_ninputs();
    //! Returns the strides for each input.
    virtual mfidxdim get_strides();
    //! Returns the scales for each input.
    virtual mfidxdim get_scales();
    //! Returns a deep copy of current module.
    virtual flat_merge_module<T,Tstate>* copy();
    //! Set offsets to be applied to each input scale.
    virtual void set_offsets(vector<vector<int> > &off);
    //! Set strides.
    virtual void set_strides(mfidxdim &s);
  
    // internal methods ////////////////////////////////////////////////////////
  protected:
    //! Compute appropriate padding for current input in order to align
    //! all inputs, given current window of merging, subsampling ratio of this
    //! input w.r.t to image input, the edge or top-left coordinate of
    //! center of top-left pixel in image input (assuming square kernel),
    //! the scale of the input w.r.t to the original scale of the image,
    //! and finally the stride of this window.
    //! This returns the padding to apply to input.
    idxdim compute_pad(idxdim &window, float subsampling, float edge,
		       float scale, fidxdim &stride);
    //! Set n default scales of input for merging.
    void default_scales(uint n);
    //! Returns the output size yielded by scale k of in.
    //! \param dref If non-null, check that output size is at least as big as reference
    //!   target size and pad if necessary.
    idxdim compute_output_sizes(mstate<Tstate> &in, uint k, idxdim *dref = NULL);

    // class variables /////////////////////////////////////////////////////////
  protected:
    idxdim din, din2;
    midxdim dins, dins2;
    fidxdim stride;
    mfidxdim strides;
    mfidxdim scales; //!< Scale of each input wrt input image.
    vector<vector<int> > offsets; //!< Offsets.
    uint reference_scale; //!< This scale's step is one and is the reference.
  };

  // linear_merge ////////////////////////////////////////////////////////////////

  //! This module is equivalent to a flat_merge_module followed by a linear_module
  //! but executes much faster.
  template <typename T, class Tstate = bbstate_idx<T> >
    class linear_merge_module : public flat_merge_module<T, Tstate> {
  public:
    //! \param nout Number of outputs of linear combination.
    //! \param dins The input sizes for each state, including the features 
    //!   dimension.
    //! \param scales Specifies to which input scale each input corresponds to.
    //! \param features Number of features of each input state.
    linear_merge_module(parameter<T,Tstate> *p, intg nout, 
			midxdim &ins, mfidxdim &strides,
			const char *name_ = "linear_merge", 
			mfidxdim *scales = NULL);
    virtual ~linear_merge_module();

    // multi-states methods ////////////////////////////////////////////////////
    //! forward propagation from in to out
    virtual void fprop(mstate<Tstate> &in, Tstate &out);
    //! backward propagation from out to in
    virtual void bprop(mstate<Tstate> &in, Tstate &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(mstate<Tstate> &in, Tstate &out);

    ///////////////////////////////////////////////////////////////////////////
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns a deep copy of current module.
    //! \param p If NULL, reuse current parameter space, otherwise allocate new
    //!   weights on parameter 'p'.
    virtual linear_merge_module<T,Tstate>* copy(parameter<T,Tstate> *p = NULL);

    // class variables /////////////////////////////////////////////////////////
  private:
    using flat_merge_module<T,Tstate>::din;
    using flat_merge_module<T,Tstate>::din2;
    using flat_merge_module<T,Tstate>::dins;
    using flat_merge_module<T,Tstate>::dins2;
    using flat_merge_module<T,Tstate>::stride;
    using flat_merge_module<T,Tstate>::strides;
    using flat_merge_module<T,Tstate>::scales;
    vector<convolution_module<T,Tstate>*> convs;
    mstate<Tstate> buffers1;
    midxdim dins_original;
    intg nout;
    Tstate buffer2;
  };

  // mstate_merge //////////////////////////////////////////////////////////////

  //! A module that flattens and concatenate multiple states.
  template <typename T, class Tstate = bbstate_idx<T> >
    class mstate_merge_module : public module_1_1<T, Tstate> {
  public:
    //! Initialize inputs list.
    //! \param ins A vector of input regions.
    //! \param strides A vector of input strides.
    mstate_merge_module(midxdim &ins, mfidxdim &strides,
			const char *name_ = "mstate_merge");
    virtual ~mstate_merge_module();
    // multi-states methods ////////////////////////////////////////////////////
    virtual void fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bprop(mstate<Tstate> &in, mstate<Tstate> &out);
    virtual void bbprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //////////////////////////////////////////////////////////////////
    //! Return dimensions that are compatible with this module.
    //! See module_1_1_gen's documentation for more details.
    virtual idxdim fprop_size(idxdim &i_size);
    //! Return dimensions compatible with this module given output dimensions.
    //! See module_1_1_gen's documentation for more details.
    virtual fidxdim bprop_size(const fidxdim &o_size);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();

  private:
    midxdim dins;
    mfidxdim dstrides;
  };

  // merge /////////////////////////////////////////////////////////////////////

  //! A module that can concatenate multiple inputs. It takes one
  //! primary input to which inputs will be
  //! concatenated into the output (by allocating a bigger output
  //! and copying all data to that output).
  template <typename T, class Tstate = bbstate_idx<T> >
    class merge_module : public module_1_1<T, Tstate> {
  public:
    //! Merge multi-state inputs given a vector of vector of indexes to merge,
    //! i.e. all states which id are in the first vector are merge together,
    //! and this process is repeated for each vector<uint>.
    merge_module(std::vector<std::vector<uint> > &states, intg concat_dim,
		 const char *name_ = "merge");
    virtual ~merge_module();
    //! forward propagation from in to out
    virtual void fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! backward propagation from out to in
    virtual void bprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns a deep copy of current module.
    virtual merge_module<T,Tstate>* copy();
    //! Return dimensions that are compatible with this module.
    //! See module_1_1_gen's documentation for more details.
    virtual mfidxdim fprop_size(mfidxdim &isize);
    //! Returns multiple input dimensions corresponding to output dims 'osize'.
    virtual mfidxdim bprop_size(mfidxdim &osize);
 
    // internal members ////////////////////////////////////////////////////////
  protected:
    //! Merge all states in 'in' into one state 'out'.
    virtual void merge(mstate<Tstate> &in, Tstate &out);
    //! Backpropagate dx weights.
    virtual void merge_dx(mstate<Tstate> &in, Tstate &out);
    //! Backpropagate ddx weights.
    virtual void merge_ddx(mstate<Tstate> &in, Tstate &out);

  private:
    std::vector<std::vector<uint> > states_list;
    intg concat_dim; //! The dimension to concatenante.
  };

  // interlace /////////////////////////////////////////////////////////////////

  //! A module that interlaces multiple inputs, e.g. with input states
  //! [1 2 3 4], output states will be [1 3 2 4].
  template <typename T, class Tstate = bbstate_idx<T> >
    class interlace_module : public module_1_1<T, Tstate> {
  public:
    //! Constructs an interlacer with stride 'stride'.
    interlace_module(uint stride, const char *name = "interlace_module");
    //! Destructor.
    virtual ~interlace_module();
    //! forward propagation from in to out
    virtual void fprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! backward propagation from out to in
    virtual void bprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(mstate<Tstate> &in, mstate<Tstate> &out);
    //! Returns multiple input dimensions corresponding to output dims 'osize'.
    virtual mfidxdim bprop_size(mfidxdim &osize);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns a deep copy of current module.
    virtual interlace_module<T,Tstate>* copy();
  private:
    uint stride; //! The interlacing stride.
  };

} // namespace ebl {

#include "ebl_merge.hpp"

#endif /* EBL_MERGE_H_ */
