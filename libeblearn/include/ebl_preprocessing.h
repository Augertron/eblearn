/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#ifndef EBL_PREPROCESSING_H_
#define EBL_PREPROCESSING_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_states.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // rgb_to_ypuv_module
  //! convert an RGB input into a YpUV output, Yp being a Y channel
  //! with a local normaliztion.
  template <class T> class rgb_to_ypuv_module: public module_1_1<T> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    rgb_to_ypuv_module(uint normalization_size);
    //! destructor
    virtual ~rgb_to_ypuv_module();
    //! forward propagation from in to out
    virtual void fprop(state_idx<T> &in, state_idx<T> &out);
    //! resize the output based on input dimensions
    virtual void resize_output(state_idx<T> &in, state_idx<T> &out);

  private:
    uint	normalization_size;	//!< normalization kernel size
    idx<T>	tmp;		        //!< temporary buffer
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_yp_module
  //! convert an RGB input into a Yp output, Yp being a Y channel
  //! with a local normaliztion.
  template <class T> class rgb_to_yp_module: public module_1_1<T> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    rgb_to_yp_module(uint normalization_size);
    //! destructor
    virtual ~rgb_to_yp_module();
    //! forward propagation from in to out
    virtual void fprop(state_idx<T> &in, state_idx<T> &out);
    //! resize the output based on input dimensions
    virtual void resize_output(state_idx<T> &in, state_idx<T> &out);

  private:
    uint	normalization_size;	//!< normalization kernel size
    idx<T>	tmp;		        //!< temporary buffer
  };

  ////////////////////////////////////////////////////////////////
  // resizepp_module
  //! Resize the input to the desired output (while preserving aspect ratio)
  //! and apply a preprocessing module.
  //! This is useful because in some situations preprocessing needs
  //! to be done within the resizing operation. e.g. when resizing
  //! while preserving aspect ratio, the output must eventually be copied into
  //! the true desired output dimensions, but preprocessing must be done before
  //! copying it to avoid edge detection between the empty parts of the image.
  template <class T> class resizepp_module: public module_1_1<T> {
  public:
    //! Constructor.
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is not responsible for
    //!           destroying the preprocessing module.
    //! \param height target height for resizing.
    //! \param width target width for resizing.
    resizepp_module(uint height, uint width, module_1_1<T> *pp = NULL);
    //! destructor
    virtual ~resizepp_module();
    //! sets the desired output dimensions.
    void set_dimensions(uint height_, uint width_);
    //! forward propagation from in to out
    virtual void fprop(state_idx<T> &in, state_idx<T> &out);
    //! return the bounding box of the original input in the output coordinate
    //! system.
    rect get_original_bbox();

  private:
    module_1_1<T>	*pp;	        //!< preprocessing module
    uint		 height;	//!< target height
    uint		 width;         //!< target width
    state_idx<T>         inpp, outpp;   //!< input/output buffers for pp
    idx<T>               tmp;           //!< temporary buffer
    rect                 original_bbox; //!< bbox of original input in output
  };

} // namespace ebl {

#include "ebl_preprocessing.hpp"

#endif /* EBL_PREPROCESSING_H_ */
