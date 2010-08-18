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
#include "ebl_transfer.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // channorm_module
  //! Abstract class for normalization of image channels.
  template <typename T, class Tstate = bbstate_idx<T> >
    class channorm_module: public module_1_1<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    channorm_module(uint normalization_size, const char *name = "channorm");
    //! Destructor
    virtual ~channorm_module();
    //! Forward propagation from in to out (abstract).
    virtual void fprop(Tstate &in, Tstate &out) = 0;
    //! Returns a deep copy of this module (abstract).
    virtual channorm_module<T,Tstate>* copy() = 0;
    
  protected:
    //! Resize the output based on input dimensions
    //! \param dim0 An optional size for the first dimension. Set it to 1
    //!             when converting from color to greyscale.
    virtual void resize_output(Tstate &in, Tstate &out,
			       int dim0 = -1);

  protected:
    uint			        normalization_size;//!< norm kernel size
    Tstate      	        	tmp;	//!< temporary buffer
    weighted_std_module<T,Tstate>	norm;//!< contrast normalization module
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_ypuv_module
  //! convert an RGB input into a YpUV output, Yp being a Y channel
  //! with a local normaliztion.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_ypuv_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    rgb_to_ypuv_module(uint normalization_size);
    //! destructor
    virtual ~rgb_to_ypuv_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_ypuv_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_yp_module
  //! convert an RGB input into a Yp output, Yp being a Y channel
  //! with a local normaliztion.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_yp_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    rgb_to_yp_module(uint normalization_size);
    //! destructor
    virtual ~rgb_to_yp_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_yp_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // bgr_to_ypuv_module
  //! convert an BGR input into a YpUV output, Yp being a Y channel
  //! with a local normaliztion.
  template <typename T, class Tstate = bbstate_idx<T> >
    class bgr_to_ypuv_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    bgr_to_ypuv_module(uint normalization_size);
    //! destructor
    virtual ~bgr_to_ypuv_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual bgr_to_ypuv_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // bgr_to_yp_module
  //! convert an BGR input into a Yp output, Yp being a Y channel
  //! with a local normaliztion.
  template <typename T, class Tstate = bbstate_idx<T> >
    class bgr_to_yp_module : public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    bgr_to_yp_module(uint normalization_size);
    //! destructor
    virtual ~bgr_to_yp_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual bgr_to_yp_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_hp_module
  //! convert an RGB input into a Hp output, Hp being a H channel (from HSV)
  //! with a local normaliztion.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_hp_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Hp's
    //!        local normalization.
    rgb_to_hp_module(uint normalization_size);
    //! destructor
    virtual ~rgb_to_hp_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_hp_module<T,Tstate>* copy();
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
  template <typename T, class Tstate = bbstate_idx<T> >
    class resizepp_module: public module_1_1<T,Tstate> {
  public:
    //! Constructor. Preprocessing module pp will be deleted upon destruction.
    //! \param height target height for resizing.
    //! \param width target width for resizing.
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param kernelsz The kernel size used by the preprocessing module.
    //!           This is used to take kernel's (if any) border effect into
    //!           account during resizing. The default value (0) has no effect.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param hzpad Optional vertical zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    //! \param wzpad Optional horizontal zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    resizepp_module(intg height, intg width, uint mode = MEAN_RESIZE,
		    module_1_1<T,Tstate> *pp = NULL, uint kernelsz = 0,
		    bool own_pp = false, uint hzpad = 0, uint wzpad = 0);
    //! Constructor without target dimensions. set_dimensions should be called
    //! later. Preprocessing module pp will be deleted upon destruction.
    //! \param height target height for resizing.
    //! \param width target width for resizing.
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param kernelsz The kernel size used by the preprocessing module.
    //!           This is used to take kernel's (if any) border effect into
    //!           account during resizing. The default value (0) has no effect.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param hzpad Optional vertical zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    //! \param wzpad Optional horizontal zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    resizepp_module(uint mode = MEAN_RESIZE,
		    module_1_1<T,Tstate> *pp = NULL, uint kernelsz = 0,
		    bool own_pp = false, uint hzpad = 0, uint wzpad = 0);
    //! destructor
    virtual ~resizepp_module();
    //! sets the desired output dimensions.
    void set_dimensions(intg height_, intg width_);
    //! set the region to use in the input image.
    //! by default, the input region is the entire image.
    void set_input_region(const rect &inr);
    //! Set zero padding on each side for each dimension.
    void set_zpads(intg hpad, intg wpad);
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! return the bounding box of the original input in the output coordinate
    //! system.
    rect get_original_bbox();
    //! Returns a deep copy of this module.
    virtual resizepp_module<T,Tstate>* copy();
    
    // members ////////////////////////////////////////////////////////
  private:
    module_1_1<T,Tstate> *pp;	        //!< preprocessing module
    bool                 own_pp;        //!< responsible for pp's deletion
    uint                 kernelsz;      //!< size of pp's kernel
    intg		 height;	//!< target height
    intg		 width;         //!< target width
    Tstate               inpp, outpp;   //!< input/output buffers for pp
    idx<T>               tmp;           //!< temporary buffer
    idx<T>               tmp2;          //!< temporary buffer
    Tstate               tmp3;          //!< temporary buffer
    rect                 original_bbox; //!< bbox of original input in output
    uint                 mode;          //!< resizing mode.
    rect                 inrect;        //!< input region of image
    rect                 outrect;       //!< input region in output image
    bool                 inrect_set;    //!< use input region or not.
    uint                 hzpad;         //!< vertical zero-padding for each side
    uint                 wzpad;         //!< horiz. zero-padding for each side
    zpad_module<T,Tstate> *zpad;        //!< Zero padding module.
  };

} // namespace ebl {

#include "ebl_preprocessing.hpp"

#endif /* EBL_PREPROCESSING_H_ */
