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

#ifndef EBL_NONLINEARITY_H_
#define EBL_NONLINEARITY_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_states.h"
#include "ebl_basic.h"
#include "ebl_arch.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  //! a slab of standard Lush sigmoids
  template <typename T, class Tstate = bbstate_idx<T> >
    class stdsigmoid_module: public module_1_1<T,Tstate> {
  public:
    //! empty constructor
    stdsigmoid_module();
    virtual ~stdsigmoid_module();
    //! fprop from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! bprop
    virtual void bprop(Tstate &in, Tstate &out);
    //! bbprop
    virtual void bbprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module.
    virtual stdsigmoid_module<T,Tstate>* copy();
  protected:
    idx<T> tmp; //!< Temporary buffer.
  };

  ////////////////////////////////////////////////////////////////
  //! a slab of tanh
  template <typename T, class Tstate = bbstate_idx<T> >
    class tanh_module: public module_1_1<T,Tstate> {
  public:
    //! default constructor
    tanh_module(
#ifdef __CUDA__
                     bool use_gpu_ = false, int gpu_id_ = -1
#endif
);
    virtual ~tanh_module();
    //! fprop from in to out
    void fprop(Tstate &in, Tstate &out);
    //! bprop
    void bprop(Tstate &in, Tstate &out);
    //! bbprop
    void bbprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module.
    virtual tanh_module<T,Tstate>* copy();
  protected:
    idx<T> tmp; //!< Temporary buffer.
#ifdef __CUDA__
    // GPU members /////////////////////////////////////////////////////////////
    bool                use_gpu; //!< Whether to use gpu or not
    int                 gpu_id; //!< Whether to use gpu or not
    bool                float_precision; //!< used for CUDA
#endif

  };

  ////////////////////////////////////////////////////////////////
  //! softmax module
  //! if in is idx0 -> out is idx0 and equal to 1
  //! if in is idx1 -> it is just one pool
  //! if in is idx2 -> it is just one pool
  //! if in is idx3 -> the last two dimensions are pools
  //! if in is idx4 -> the last two dimensions are pools
  //! if in is idx5 -> the last four dimensions are pools
  //! if in is idx6 -> the last four dimensions are pools
  template <typename T, class Tstate = bbstate_idx<T> >
    class softmax: public module_1_1<T,Tstate> {
  public:
    double beta;

    // <b> is the parameter beta in the softmax
    // large <b> turns the softmax into a max
    // <b> equal to 0 turns the softmax into 1/N

  private:
    void resize_nsame(Tstate &in, Tstate &out, int n);

  public:
    softmax(double b);
    ~softmax() {};
    void fprop(Tstate &in, Tstate &out);
    void bprop(Tstate &in, Tstate &out);
    void bbprop(Tstate &in, Tstate &out);
  };

  ////////////////////////////////////////////////////////////////
  // abs_module
  //! This module takes the absolute value of its input.
  //! This module is spatially replicable 
  //! (the input can have an order greater than 1 and the operation will apply
  //! to all elements).
  template <typename T, class Tstate = bbstate_idx<T> >
    class abs_module: public module_1_1<T, Tstate> {    
  public:
    //! Constructor. threshold makes the derivative of abs flat around zero
    //! with radius threshold.
    abs_module(double thresh = 0.0);
    //! Destructor.
    virtual ~abs_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! backward propagation from out to in
    virtual void bprop(Tstate &in, Tstate &out);
    //! second-derivative backward propagation from out to in
    virtual void bbprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module.
    virtual abs_module<T,Tstate>* copy();
  private:
    double threshold;
  };

  ////////////////////////////////////////////////////////////////
  // linear_shrink_module
  //! A piece-wise linear shrinkage module that parametrizes
  //! the location of the shrinkage operator. This function is 
  //! useful for learning since there is always gradients flowing 
  //! through it.
  template <typename T, class Tstate = bbstate_idx<T> >
    class linear_shrink_module: public module_1_1<T, Tstate> {
  public:
    //! Constructor.
    //! \param nf The number of features.
    linear_shrink_module(parameter<T,Tstate> *p, intg nf, T bias = 0);
    //! Destructor.
    virtual ~linear_shrink_module();
    //! forward
    virtual void fprop(Tstate &in, Tstate &out);
    //! backward
    virtual void bprop(Tstate &in, Tstate &out);
    //! 2nd deriv backward
    virtual void bbprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module.
    virtual linear_shrink_module<T,Tstate>* copy();
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
  protected:
    Tstate bias;
    T default_bias;
  };

  ////////////////////////////////////////////////////////////////
  // smooth_shrink_module
  //! A smoothed shrinkage module that parametrizes the steepnes
  //! and location of the shrinkage operator. This function is 
  //! useful for learning since there is always gradients flowing 
  //! through it.
  template <typename T, class Tstate = bbstate_idx<T> >
    class smooth_shrink_module: public module_1_1<T, Tstate> {
  public:
    //! Constructor.
    //! \param nf The number of features.
    smooth_shrink_module(parameter<T,Tstate> *p, intg nf, T beta = 10,
			 T bias = .3);
    //! Destructor.
    virtual ~smooth_shrink_module();
    //! forward
    virtual void fprop(Tstate &in, Tstate &out);
    //! backward
    virtual void bprop(Tstate &in, Tstate &out);
    //! 2nd deriv backward
    virtual void bbprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module.
    virtual smooth_shrink_module<T,Tstate>* copy();

  public:
    Tstate beta, bias;
  private:
    Tstate ebb, ebx, tin;
    abs_module<T,Tstate> absmod;
    T default_beta, default_bias;
  };

  ////////////////////////////////////////////////////////////////
  // tanh_shrink_module
  //! A smoothed shrinkage module using (x - tanh(x))
  //! that parametrizes the steepnes of the shrinkage operator.
  //! This function is useful for learning since there is always gradients
  //! flowing through it.
  template <typename T, class Tstate = bbstate_idx<T> >
    class tanh_shrink_module: public module_1_1<T, Tstate> {
  public:
    //! Constructor.
    //! \param nf The number of features.
    //! \param diags If true, alpha and beta coefficients are learned
    //!   such that the output is: a * x - tanh(b * x)
    tanh_shrink_module(parameter<T,Tstate> *p, intg nf, bool diags = false);
    //! Destructor.
    virtual ~tanh_shrink_module();
    //! forward
    virtual void fprop(Tstate &in, Tstate &out);
    //! backward
    virtual void bprop(Tstate &in, Tstate &out);
    //! 2nd deriv backward
    virtual void bbprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module.
    virtual tanh_shrink_module<T,Tstate>* copy();
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
  protected:
    intg			 nfeatures;
    Tstate			 abuf, tbuf, bbuf;
    diag_module<T,Tstate>	*alpha, *beta;
    tanh_module<T,Tstate>	 mtanh;
    diff_module<T,Tstate>	 difmod;	//!< difference module
    bool                         diags; //!< Use coefficients or not.
  };

} // namespace ebl {

#include "ebl_nonlinearity.hpp"

#endif /* EBL_NONLINEARITY_H_ */
