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

namespace ebl {

  // flat_merge ////////////////////////////////////////////////////////////////

  //! Forward declaration of zpad_module.
  template <typename T, class Tstate> class zpad_module;

  //! A module that flattens and concatenate multiple inputs. It takes one
  //! primary input to which inputs will be
  //! concatenated into the output (by allocating a bigger output
  //! and copying all data to that output).
  template <typename T, class Tstate = bbstate_idx<T> >
    class flat_merge_module : public m2s_module<T, Tstate> {
  public:
    //! Initialize inputs list.
    //! \param inputs A vector of pointers to the input states
    //!   pointers to concatenate.
    flat_merge_module(std::vector<Tstate**> &inputs,
		      idxdim &in, midxdim &ins,
		      fidxdim &stride, mfidxdim &strides,
		      const char *name_ = "flatmerge", const char *list = NULL);
    //! Initialize multi-state inputs list.
    //! \param inputs A vector of pointers to the input states
    //!   pointers to concatenate.
    flat_merge_module(std::vector<mstate<Tstate>**> &inputs,
    		      idxdim &in, midxdim &ins,
    		      fidxdim &stride, mfidxdim &strides,
    		      const char *name_ = "flatmerge", const char *list = NULL);
    //! Constructor for mstate inputs only. Buffers to be merge are not
    //! specified here, rather in fprop(mstate..). fprop(Tstate...) should
    //! not be used with this constructor.
    //! \param pad If true, pad inputs so that windows are centered on edge
    //!    pixels at image borders. This should be false during training
    //!    and true during detection.
    flat_merge_module(midxdim &ins, mfidxdim &strides, bool pad = false,
		      const char *name_ = "flatmerge", mfidxdim *scales = NULL,
		      intg hextra = 0, intg wextra = 0, float ss = 1,
		      float edge = 0);
    virtual ~flat_merge_module();
    //////////////////////////////////////////////////////////////////
    // generic states methods
    //! forward propagation from in to out, using internal bufs for merging.
    virtual void fprop(Tstate &in, Tstate &out);
    //! backward propagation from out to in, using internal bufs for merging.
    virtual void bprop(Tstate &in, Tstate &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(Tstate &in, Tstate &out);
    //////////////////////////////////////////////////////////////////
    // multi-states methods
    //! forward propagation from in to out
    virtual void fprop(mstate<Tstate> &in, Tstate &out);
    //! backward propagation from out to in
    virtual void bprop(mstate<Tstate> &in, Tstate &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(mstate<Tstate> &in, Tstate &out);
    //////////////////////////////////////////////////////////////////
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
    /* //! Set paddings to be applied to each input scale. */
    /* virtual void set_paddings(mfidxdim &pads); */
    //! Set offsets to be applied to each input scale.
    virtual void set_offsets(vector<vector<int> > &off);
    //! Set strides.
    virtual void set_strides(mfidxdim &s);

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

  private:
    std::vector<Tstate**> inputs;
    idxdim din;
    midxdim dins;
    fidxdim stride;
    mfidxdim strides;
    std::string merge_list; //!< Names of elements to be merged.
    std::vector<zpad_module<T,Tstate>*> zpads;
    Tstate *in0;
    std::vector<Tstate*> pinputs;
    bool use_pinputs;
    mstate<Tstate> padded;
    zpad_module<T,Tstate> padder;
    bool bpad; //!< Pad inputs to center windows at edges.
    mfidxdim scales; //!< Scale of each input wrt input image.
    mfidxdim paddings; //!< Padding for each scale.
    vector<vector<int> > offsets; //!< Offsets.

  // TEMP
  intg hextra, wextra;
  float subsampling, edge;
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
    //! Initialize inputs list.
    //! \param inputs A vector of pointers to the input states
    //!   pointers to concatenate.
    //! \param concat_dim Input dimension to assuming all other dimensions
    //!   have the same size.
    merge_module(std::vector<Tstate**> &inputs, intg concat_dim,
		 const char *name_ = "merge", const char *list = NULL);
    //! Merge multi-state inputs.
    merge_module(std::vector<mstate<Tstate>**> &inputs, intg concat_dim,
		 const char *name_ = "merge", const char *list = NULL);
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
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! backward propagation from out to in
    virtual void bprop(Tstate &in, Tstate &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(Tstate &in, Tstate &out);
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
    std::vector<Tstate**> inputs;
    std::vector<mstate<Tstate>**> msinputs;
    std::string merge_list;
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
