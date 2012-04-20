/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
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

#ifndef EBL_PREPROCESSING_H_
#define EBL_PREPROCESSING_H_

#include "ebl_defines.h"
#include "libidx.h"
#include "ebl_arch.h"
#include "ebl_march.h"
#include "ebl_states.h"
#include "ebl_normalization.h"
#include "ebl_basic.h"

namespace ebl {

  //! Normalization types.
  enum t_norm { WSTD_NORM = 0, /* local contrast normalization */
		LAPLACIAN_NORM = 1 /* laplacian normalization */
  };
  
  ////////////////////////////////////////////////////////////////
  // channels_module
  //! Abstract class for image channels preprocessing.
  template <typename T, class Tstate = bbstate_idx<T> >
    class channels_module: public module_1_1<T,Tstate> {
  public:
    channels_module(bool global_norm = true, const char *name = "channels");
    virtual ~channels_module();
  protected:
    //! Resize the output based on input dimensions
    //! \param dim0 An optional size for the first dimension. Set it to 1
    //!             when converting from color to greyscale.
    virtual void resize_output(Tstate &in, Tstate &out, int dim0 = -1);
    //! Friends.
    template <typename T1, class T2> friend class laplacian_pyramid_module;  
  protected:
    bool globnorm; //!< Normalize globally or not.
  };

  ////////////////////////////////////////////////////////////////
  // channorm_module
  //! Abstract class for normalization of image channels.
  template <typename T, class Tstate = bbstate_idx<T> >
    class channorm_module: public channels_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param kerdim Kernel dimensions for local normalization.
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    //! \param nf The number of features to normalize across.
    //! \param globnorm Normalize channels globally or not.
    channorm_module(idxdim &kerdim, bool mirror = true,
		    t_norm norm_mode = WSTD_NORM,
		    const char *name = "channorm", int nf = 1,
		    bool globnorm = true);
    //! Destructor
    virtual ~channorm_module();
    //! Forward propagation from in to out (abstract).
    virtual void fprop(Tstate &in, Tstate &out) = 0;
    //! Returns a deep copy of this module (abstract).
    virtual channorm_module<T,Tstate>* copy() = 0;
    //! Returns a string containing describe() string of all modules.
    virtual std::string describe();
    //! Returns a new norm module.
    module_1_1<T,Tstate>* new_norm(idxdim &normker, bool mirror, 
				   t_norm norm_mode, int nf);
  protected:
    //! Resize the output based on input dimensions
    //! \param dim0 An optional size for the first dimension. Set it to 1
    //!             when converting from color to greyscale.
    virtual void resize_output(Tstate &in, Tstate &out,
			       int dim0 = -1);

  protected:
    idxdim			normker;	//!< norm kernel size
    Tstate      	        tmp;	//!< temporary buffer
    module_1_1<T,Tstate>	*norm;	//!< contrast normalization module
    bool                        mirror; //!< Mirror padding or not.
    t_norm                      norm_mode; //!< Normalization mode.
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_ynuv_module
  //! convert an RGB input into a YpUV output, Yp being a Y channel
  //! with a local normaliztion.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_ynuv_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    rgb_to_ynuv_module(idxdim &norm_kernel, bool mirror = true,
		       t_norm norm_mode = WSTD_NORM, bool globnorm = true);
    //! destructor
    virtual ~rgb_to_ynuv_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_ynuv_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_ynuvn_module
  //! convert an RGB input into a YnUVn output, Yn being a Y channel
  //! with a local normaliztion, and UV normalized locally together.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_ynuvn_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    rgb_to_ynuvn_module(idxdim &norm_kernel, bool mirror = true,
			t_norm norm_mode = WSTD_NORM, bool globnorm = true);
    //! destructor
    virtual ~rgb_to_ynuvn_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_ynuvn_module<T,Tstate>* copy();
  protected:
    module_1_1<T,Tstate> *norm2; //!< Normalization for UV.
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_ynunvn_module
  //! convert an RGB input into a Ynunvn output, Yn being a Y channel
  //! with a local normaliztion, and UV normalized locally together.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_ynunvn_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    rgb_to_ynunvn_module(idxdim &norm_kernel, bool mirror = true,
			t_norm norm_mode = WSTD_NORM, bool globnorm = true);
    //! destructor
    virtual ~rgb_to_ynunvn_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_ynunvn_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_yuv_module
  //! convert an RGB input into a YUV output.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_yuv_module: public channels_module<T,Tstate> {
  public:
    //! Constructor.
    rgb_to_yuv_module(bool globnorm = true);
    //! destructor
    virtual ~rgb_to_yuv_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_yuv_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_yuvn_module
  //! Convert an RGB input into a YUVn output, where YUV is normalized
  //! locally across all channels.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_yuvn_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    rgb_to_yuvn_module(idxdim &norm_kernel, bool mirror = true,
		       t_norm norm_mode = WSTD_NORM, bool globnorm = true);
    //! destructor
    virtual ~rgb_to_yuvn_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_yuvn_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_rgbn_module
  //! Convert an RGB input into a YUVn output, where YUV is normalized
  //! locally across all channels.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_rgbn_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    rgb_to_rgbn_module(idxdim &norm_kernel, bool mirror = true,
		       t_norm norm_mode = WSTD_NORM, bool globnorm = true);
    //! destructor
    virtual ~rgb_to_rgbn_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_rgbn_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_y_module
  //! convert an RGB input into a Y channel.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_y_module: public channels_module<T,Tstate> {
  public:
    //! Constructor.
    rgb_to_y_module(bool globnorm = true);
    //! destructor
    virtual ~rgb_to_y_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_y_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // rgb_to_yn_module
  //! convert an RGB input into a Yp output, Yp being a Y channel
  //! with a local normaliztion.
  template <typename T, class Tstate = bbstate_idx<T> >
    class rgb_to_yn_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    rgb_to_yn_module(idxdim &norm_kernel, bool mirror = true,
		     t_norm norm_mode = WSTD_NORM, bool globnorm = true);
    //! destructor
    virtual ~rgb_to_yn_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual rgb_to_yn_module<T,Tstate>* copy();
  };

  ////////////////////////////////////////////////////////////////
  // y_to_yp_module
  //! convert an Y input into a Yp output, Yp being a Y channel
  //! with a local normaliztion.
  template <typename T, class Tstate = bbstate_idx<T> >
    class y_to_yp_module: public channorm_module<T,Tstate> {
  public:
    //! Constructor.
    //! \param normalization_size is the size of the kernel used for Yp's
    //!        local normalization.
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    y_to_yp_module(idxdim &norm_kernel, bool mirror = true,
		   t_norm norm_mode = WSTD_NORM, bool globnorm = true);
    //! destructor
    virtual ~y_to_yp_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module (abstract).
    virtual y_to_yp_module<T,Tstate>* copy();
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
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    bgr_to_ypuv_module(idxdim &norm_kernel, bool mirror = true,
		       t_norm norm_mode = WSTD_NORM, bool globnorm = true);
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
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    bgr_to_yp_module(idxdim &norm_kernel, bool mirror = true,
		     t_norm norm_mode = WSTD_NORM, bool globnorm = true);
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
    //! \param mirror If true, pad normalization if a mirror of the image
    //!   instead of with zeros. This can be useful in object detection when
    //!   objects are close to borders.
    //! \param norm_mode The type of normalization (WSTD_NORM by default).
    rgb_to_hp_module(idxdim &norm_kernel, bool mirror = true,
		     t_norm norm_mode = WSTD_NORM, bool globnorm = true);
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
    class resizepp_module: virtual public module_1_1<T,Tstate> {
  public:
    //! Constructor. Preprocessing module pp will be deleted upon destruction.
    //! \param size The target dimensions (heightxwidth)
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param zpad Optional zero-padding is added on each side
    //! \param preserve_ratio If true, fit the image into target size while
    //!   keeping aspect ratio, potential empty areas are filled with zeros.
    resizepp_module(idxdim &size, uint mode = MEAN_RESIZE,
		    module_1_1<T,Tstate> *pp = NULL,
		    bool own_pp = true, idxdim *zpad = NULL,
		    bool preserve_ratio = true);
    //! Constructor without target dimensions. set_dimensions should be called
    //! later. Preprocessing module pp will be deleted upon destruction.
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param size The target dimensions (heightxwidth)
    //! \param zpad Optional zero-padding is added on each side
    //! \param preserve_ratio If true, fit the image into target size while
    //!   keeping aspect ratio, potential empty areas are filled with zeros.
    resizepp_module(uint mode = MEAN_RESIZE, module_1_1<T,Tstate> *pp = NULL,
		    bool own_pp = true, idxdim *zpad = NULL,
		    bool preserve_ratio = true);
    //! This constructor specifies resizing ratio for each dimension instead
    //! of fixed target sizes. The default resizing method is bilinear, as
    //! as other methods do not currently implement ratio inputs.
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param size The target dimensions (heightxwidth)
    //! \param zpad Optional zero-padding is added on each side
    //! \param preserve_ratio If true, fit the image into target size while
    //!   keeping aspect ratio, potential empty areas are filled with zeros.
    resizepp_module(double hratio, double wratio,
		    uint mode = MEAN_RESIZE, module_1_1<T,Tstate> *pp = NULL,
		    bool own_pp = true, idxdim *zpad = NULL,
		    bool preserve_ratio = true);
    //! destructor
    virtual ~resizepp_module();
    //! sets the desired output dimensions.
    void set_dimensions(intg height_, intg width_);
    //! set the region to use in the input image.
    //! by default, the input region is the entire image.
    void set_input_region(const rect<int> &inr);
    //! set the region to use in the output image.
    //! by default, the output region is the entire size defined by
    //! set_dimensions().
    void set_output_region(const rect<int> &outr);
    //! Shift input region by h and w pixels, multiply scale by s and 
    //! rotate by r.
    void set_jitter(int h, int w, float s, float r);
    //! Scale input region by factor s.
    void set_scale_factor(double s);
    //! Scale input region hxw by factors shxsw.
    void set_scale_factor(double sh, double sw);
    //! Set zero padding on each side for each dimension.
    void set_zpads(intg hpad, intg wpad);
    //! Set zero padding based on 'kernel'.
    void set_zpad(idxdim &kernel);
    //! Set zero padding based on 'kernel'.
    void set_zpad(midxdim &kernels);
    //! Returns the input box in output space.
    rect<int> get_original_bbox();
    //! Returns the input box in input image space.
    rect<int> get_input_bbox();
    //! Returns all bounding boxes extracted in input space.
    const vector<rect<int> >& get_input_bboxes();
    //! Returns all bounding boxes extracted in the output space.
    const vector<rect<int> >& get_original_bboxes();
    //! Compute the input and output regions given 'in' and return input region.
    virtual rect<int> compute_regions(Tstate &in);
    //! Set the input_bbox given output height and width.
    virtual void remember_regions(intg outh, intg outw, rect<int> &r);
    //! Set the displayable range of values for outputs of this module.
    virtual void set_display_range(T min, T max);
    //! Set 'min' and 'max' to the displayable range of this module's outputs.
    virtual void get_display_range(T &min, T &max);
  
    // fprop methods ///////////////////////////////////////////////////////////
  
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, idx<T> &out);
    //! Process 'in' into 'out' which will contain an array of idx, where each
    //! idx has different scale with different dimensions.
    virtual void fprop(Tstate &in, midx<T> &out);

    //! Returns a deep copy of this module.
    virtual resizepp_module<T,Tstate>* copy();
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns a reference to the last output state set by fprop.
    virtual mstate<Tstate>* last_output();
    //! Set a buffer in which to copy the output of future fprop() calls.
    //! This is useful to keep the preprocessed input around when discarding
    //! intermediate buffers for memory optimization.
    virtual void set_output_copy(mstate<Tstate> &out);
    //! Returns input dimensions corresponding to output dimensions 'osize'.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual fidxdim bprop_size(const fidxdim &osize);
    //! Returns input dimensions corresponding to output dimensions 'osize'.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim bprop_size(mfidxdim &osize);
    //! Returns a vector of idxdim stored after a call to fprop_size() or
    //! bprop_size() on this module. If containing multiple elements, they
    //! correspond to each state size contained in last_output().
    virtual mfidxdim get_msize();
    //! Returns the number of layers produced by this resizing module.
    virtual uint nlayers();
    //! Copy outputs 'out' into internal buffers.
    virtual void copy_outputs(mstate<Tstate> &out);
    
    // members /////////////////////////////////////////////////////////////////
  protected:
    module_1_1<T,Tstate> *pp;	        //!< preprocessing module
    bool                 own_pp;        //!< responsible for pp's deletion
    idxdim               size;          //!< target sizes
    intg		 height;	//!< target height
    intg		 width;         //!< target width
    Tstate               inpp, outpp;   //!< input/output buffers for pp
    idx<T>               tmp;           //!< temporary buffer
    idx<T>               tmp2;          //!< temporary buffer
    Tstate               tmp3;          //!< temporary buffer
    rect<int>            input_bbox;    //!< bbox of last extracted box in input
    uint                 mode;          //!< resizing mode.
    int                  input_mode;    //!< mode parameter to resize function.
    rect<int>            inrect;        //!< input region of image
    rect<int>            outrect;       //!< input region in output image
    bool                 inrect_set;    //!< use input region or not.
    bool                 outrect_set;   //!< use output region or not.
    idxdim               *dzpad;        //!< zero-padding for each side
    zpad_module<T,Tstate> *zpad;        //!< Zero padding module.
    int                  hjitter;       //!< Shift output by this many pixels
    int                  wjitter;       //!< Shift output by this many pixels
    float                sjitter;       //!< Multiply scale by this
    float                rjitter;       //!< Rotate by this degrees.
    double               scale_hfactor;  //!< Input region scale h factor.
    double               scale_wfactor;  //!< Input region scale w factor.
    bool                 preserve_ratio;//!< Preserve aspect ratio or not.
    double               hratio;        //!< Resizing ratio in height dim.
    double               wratio;        //!< Resizing ratio in width dim.
    mstate<Tstate>       *lastout;      //!< Pointer to last out set by fprop.
    mstate<Tstate>       lout;          //!< Container for last out of fprop.
    mfidxdim             msize;         //!< Resulting dims of bprop_size.
    mstate<Tstate>       *out_copy;     //!< A copy of last fprop output.
    T                    display_min;   //!< Lower bound of displayable range.
    T                    display_max;   //!< Higher bound of displayable range.
    vector<rect<int> >   input_bboxes;  //!< Vector of input bboxes.
    vector<rect<int> >   original_bboxes;//!< bbox of original inputs in outputs
  };

  ////////////////////////////////////////////////////////////////
  // fovea_module
  //! Same as resizepp_module, except that it replicates the output at
  //! different scales.
  template <typename T, class Tstate = bbstate_idx<T> >
    class fovea_module
    : public resizepp_module<T,Tstate>, public s2m_module<T,Tstate> {
  public:
    //! Constructor. Preprocessing module pp will be deleted upon destruction.
    //! \param boxscale If true, rescale input box by fovea factors and input is
    //!   unchanged, otherwise resize input by 1/(fovea factors) and box is
    //!   unchanged. Box scaling should be used for training extraction
    //!   and image scaling for detection (default is false).
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param size The target dimensions (heightxwidth)
    //! \param zpad Optional zero-padding is added on each side
    // TODO: the comments dont match the constructor arguments anymore
  fovea_module(std::vector<double> &fovea, 
               midxdim &fovea_scales_size, idxdim &dsize, 
               bool boxscale = false, uint mode = MEAN_RESIZE, 
	       module_1_1<T,Tstate> *pp = NULL,
               bool own_pp = true, idxdim *dzpad = NULL, 
               const char *name = "fovea");
    //! Constructor without target dimensions. set_dimensions should be called
    //! later. Preprocessing module pp will be deleted upon destruction.
    //! \param boxscale If true, rescale input box by fovea factors and input is
    //!   unchanged, otherwise resize input by 1/(fovea factors) and box is
    //!   unchanged. Box scaling should be used for training extraction
    //!   and image scaling for detection (default is false).
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param zpad Optional zero-padding is added on each side
    fovea_module(std::vector<double> &fovea, bool boxscale = false,
		 uint mode = MEAN_RESIZE,
		 module_1_1<T,Tstate> *pp = NULL,
		 bool own_pp = true, idxdim *dzpad = NULL,
		 const char *name = "fovea");
    virtual ~fovea_module();
    //! Process 'in' into 'out' which will contain all foveas stacked in first
    //! dimensions.
    virtual void fprop(Tstate &in, Tstate &out);
    //! Process 'in' into 'out' which will contain an array of idx, where each
    //! idx is a fovea scale
    virtual void fprop(Tstate &in, midx<T> &out);
    //! Process 'in' into 'out' which will contain all foveas separated in
    //! each state of the multi-state 'out'.
    virtual void fprop(Tstate &in, mstate<Tstate> &out);
    //! backward propagation from in to out (empty)
    virtual void bprop(Tstate &in, mstate<Tstate> &out);
    //! bbackward propagation from in to out (empty)
    virtual void bbprop(Tstate &in, mstate<Tstate> &out);
    //! Returns input dimensions corresponding to output dimensions 'osize'.
    virtual mfidxdim bprop_size(mfidxdim &osize);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns bounding boxes of each scale in the input space.
    const vector<rect<int> >& get_input_bboxes();
    //! Returns bounding boxes of each scale in the output space.
    const vector<rect<int> >& get_original_bboxes();
    //! Returns the number of layers produced by this resizing module.
    virtual uint nlayers();

    // members ////////////////////////////////////////////////////////
  protected:
    std::vector<double> fovea; //!< A vector of all fovea scales.
    //! A vector for sizes of each scale
    midxdim fovea_scales_size; 
    bool boxscale; //!< Scaling input box or input image.
    std::vector<rect<int> > obboxes; //!< Boxes in output.
    std::vector<rect<int> > ibboxes; //!< Boxes in input.
  };

  ////////////////////////////////////////////////////////////////
  // laplacian_pyramid_module
  //! Creates a laplacian pyramid of the input.
  template <typename T, class Tstate = bbstate_idx<T> >
    class laplacian_pyramid_module
    : public resizepp_module<T,Tstate>, public s2m_module<T,Tstate> {
  public:
    //! Constructor. Preprocessing module pp will be deleted upon destruction.
    //! \param nscales Number of scales of pyramid, starting from target
    //!   dimensions down with a subsampling ratio of 2.
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param pp_original Preprocessing for original channel.
    //! \param add_original If true, the first object in the output will
    //!   be the original preprocessed patch of target size, without Laplacian.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param sizes A vector of target dimensions (heightxwidth). If it
    //!   contains 1 target only, simply scale by .5 at each scale. Otherwise,
    //!   manually set each target to the ones found in this vector.
    //! \param zpad Optional zero-padding is added on each side
    //! \param global_norm If true (default), removes global mean from output
    //!   and divides it by standard deviation.
    //! \param local_norm If true, removes local mean and divide by std dev
    //!   in 5x5 neighborhood.
    //! \param color_norm If true, contrast-normalize color channels.
    //! \param cnorm_across If true and color_norm is true, color is normalized
    //!   across each other, rather than layer by layer.
    //! \param keep_aspect_ratio If true (default), aspect ratio is kept.
    laplacian_pyramid_module(uint nscales, midxdim &sizes,
			     uint mode = MEAN_RESIZE, 
			     module_1_1<T,Tstate> *pp = NULL,
			     bool own_pp = true, idxdim *dzpad = NULL,
			     bool global_norm = true, bool local_norm = false,
			     bool local_norm2 = false, bool color_norm = false,
			     bool cnorm_across = true,
			     bool keep_aspect_ratio = true,
			     const char *name = "laplacian_pyramid");
    //! Constructor. Preprocessing module pp will be deleted upon destruction.
    //! \param nscales Number of scales of pyramid, starting from target
    //!   dimensions down with a subsampling ratio of 2.
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param pp_original Preprocessing for original channel.
    //! \param add_original If true, the first object in the output will
    //!   be the original preprocessed patch of target size, without Laplacian.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param sizes A vector of target dimensions (heightxwidth). If it
    //!   contains 1 target only, simply scale by .5 at each scale. Otherwise,
    //!   manually set each target to the ones found in this vector.
    //! \param zpad Optional zero-padding is added on each side
    //! \param global_norm If true (default), removes global mean from output
    //!   and divides it by standard deviation.
    //! \param local_norm If true, removes local mean and divide by std dev
    //!   in 5x5 neighborhood.
    //! \param color_norm If true, contrast-normalize color channels.
    //! \param cnorm_across If true and color_norm is true, color is normalized
    //!   across each other, rather than layer by layer.
    //! \param keep_aspect_ratio If true (default), aspect ratio is kept.
    laplacian_pyramid_module(uint nscales, idxdim &kerdims,
			     midxdim &sizes, uint mode = MEAN_RESIZE, 
			     module_1_1<T,Tstate> *pp = NULL,
			     bool own_pp = true, idxdim *dzpad = NULL, 
			     bool global_norm = true, bool local_norm = false,
			     bool local_norm2 = false, bool color_norm = false,
			     bool cnorm_across = true,
			     bool keep_aspect_ratio = true,
			     const char *name = "laplacian_pyramid");
    //! Constructor. Preprocessing module pp will be deleted upon destruction.
    //! \param nscales Number of scales of pyramid, starting from target
    //!   dimensions down with a subsampling ratio of 2.
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param pp_original Preprocessing for original channel.
    //! \param add_original If true, the first object in the output will
    //!   be the original preprocessed patch of target size, without Laplacian.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param sizes A vector of target dimensions (heightxwidth). If it
    //!   contains 1 target only, simply scale by .5 at each scale. Otherwise,
    //!   manually set each target to the ones found in this vector.
    //! \param zpad Optional zero-padding is added on each side
    //! \param global_norm If true (default), removes global mean from output
    //!   and divides it by standard deviation.
    //! \param local_norm If true, removes local mean and divide by std dev
    //!   in 5x5 neighborhood.
    //! \param color_norm If true, contrast-normalize color channels.
    //! \param cnorm_across If true and color_norm is true, color is normalized
    //!   across each other, rather than layer by layer.
    //! \param keep_aspect_ratio If true (default), aspect ratio is kept.
    laplacian_pyramid_module(uint nscales, midxdim &kerdims,
			     midxdim &sizes, uint mode = MEAN_RESIZE, 
			     module_1_1<T,Tstate> *pp = NULL,
			     bool own_pp = true, idxdim *dzpad = NULL, 
			     bool global_norm = true, bool local_norm = false,
			     bool local_norm2 = false, bool color_norm = false,
			     bool cnorm_across = true,
			     bool keep_aspect_ratio = true,
			     const char *name = "laplacian_pyramid");
    //! Constructor without target dimensions. set_dimensions should be called
    //! later. Preprocessing module pp will be deleted upon destruction.
    //! \param nscales Number of scales of pyramid, starting from target
    //!   dimensions down with a subsampling ratio of 2.
    //! \param pp An optional pointer to a  preprocessing module. If NULL, no 
    //!           preprocessing is performed. This module is responsible for
    //!           destroying the preprocessing module.
    //! \param pp_original Preprocessing for original channel.
    //! \param add_original If true, the first object in the output will
    //!   be the original preprocessed patch of target size, without Laplacian.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param zpad Optional zero-padding is added on each side
    //! \param global_norm If true (default), removes global mean from output
    //!   and divides it by standard deviation.
    //! \param local_norm If true, removes local mean and divide by std dev
    //!   in 5x5 neighborhood.
    //! \param color_norm If true, contrast-normalize color channels.
    //! \param cnorm_across If true and color_norm is true, color is normalized
    //!   across each other, rather than layer by layer.
    //! \param keep_aspect_ratio If true (default), aspect ratio is kept.
    laplacian_pyramid_module(uint nscales,
			     uint mode = MEAN_RESIZE,
			     module_1_1<T,Tstate> *pp = NULL,
			     bool own_pp = true, idxdim *dzpad = NULL,
			     bool global_norm = true, bool local_norm = false,
			     bool local_norm2 = false, bool color_norm = false,
			     bool cnorm_across = true,
			     bool keep_aspect_ratio = true,
			     const char *name = "laplacian_pyramid");
    virtual ~laplacian_pyramid_module();
    //! Init filters and normalizations.
    virtual void init();

    // processing //////////////////////////////////////////////////////////////
  
    //! 
    virtual void fprop(Tstate &in, Tstate &out);
    //! Process 'in' into 'out' which will contain all scales separated in
    //! each state of the multi-state 'out'.
    virtual void fprop(Tstate &in, mstate<Tstate> &out);
    //! Process 'in' into 'out' which will contain an array of idx, where each
    //! idx has different scale with different dimensions.
    virtual void fprop(Tstate &in, midx<T> &out);
    //! backward propagation from in to out (empty)
    virtual void bprop(Tstate &in, mstate<Tstate> &out);
    //! bbackward propagation from in to out (empty)
    virtual void bbprop(Tstate &in, mstate<Tstate> &out);

    // size propagation ////////////////////////////////////////////////////////
  
    //! Modifies multi-input dimensions 'isize' to be compliant with module's
    //! architecture, and returns corresponding output dimensions.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim fprop_size(mfidxdim &isize);
    //! Returns input dimensions corresponding to output dimensions 'osize'.
    //! Implementation of this method helps automatic scaling of input data
    //! but is optional.
    virtual mfidxdim bprop_size(mfidxdim &osize);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns the number of layers produced by this resizing module.
    virtual uint nlayers();
    //! Produce multiple pyramids, each starting at a scaling of the original
    //! input.
    virtual void set_scalings(vector<float> &scalings);

    // internal methods ////////////////////////////////////////////////////////
  protected:
    //! Resize 'im' to target 'tgt' and update 'inr' and 'outr'.
    void resize(idx<T> &im, idxdim &tgt, rect<int> &inr, rect<int> &outr);
    //! Blur 'in' into 'out' using blurring filter 'filter'.
    //! \param roi The region of interest, to be reduced if no padding is used.
    void blur(idx<T> &filter, idx<T> &in, idx<T> &out, rect<int> &roi);
    //! Highpass 'in' by subtracting 'blurred' version of the image
    //! into a new buffer and return it.
    //! The input region 'inr' will be centered in the returned buffer.
    //! The returned buffer will have 'tgt' dimensions.
    //! \param first It is the first time highpass is called or not.
    idx<T> highpass(idx<T> &in, idx<T> &blurred, idxdim &tgt, rect<int> &inr, 
		    bool first);
    //! Subsample 'in' into 'out' with a factor of 2. If 'in' dimensions are not
    //! a factor of 2, the last odd row or column are simply dropped.
    //! This downsamples 'inr' accordingly.
    void subsample(idx<T> &in, idx<T> &out, rect<int> &inr);
    //! Normalize entire 'in' image, using mean and standard deviation of the
    //! region of interest of 'roi' of the image. Intensity (0) and color
    //! channels (1 & 2) are normalized independently.
    //! Normalizing from the ROI only can be important, if the non-ROI contains
    //! zero-padding.
    void normalize_globally(idx<T> &in); //, rect<int> &roi);
    //! Called by normalize_globally().
    void normalize_intensity_globally(idx<T> &in); // , rect<int> &roi);
    //! Called by normalize_globally().
    void normalize_color_globally(idx<T> &in); // , rect<int> &roi);
    //! Called by normalize_globally().
    void normalize_globally2(idx<T> &in); // , rect<int> &roi);
  
    // members ////////////////////////////////////////////////////////
  protected:
    uint nscales; //!< Number of scales in the pyramid.
    uint iscale; //!< Current scale being produced during fprop.
    vector<float> scalings; //!< Scaling factor for multiple pyramids.
    midxdim sizes; //!< The target dimensions for each scale.
    midxdim kerdims; //!< The kernel dimensions for each scale.
    idx<T> burt; //!< The Burt-Adelson blurring kernel.
    vector<idx<T> > filters; //!< The filtering gaussian kernels for each scale.
    vector<zpad_module<T,Tstate>*> pads; //!< Padding class for input.
    bool use_pad; //!< Pad before filtering or not.
    bool global_norm; //!< Globally normalize outputs or not.
    bool local_norm; //!< Locally normalize outputs or not.
    bool local_norm2; //!< Locally normalize outputs or not.
    bool color_lnorm; //!< Locally normalize color or not.
    bool cnorm_across; //!< Locally color across each other or separately.
    bool keep_aspect_ratio; //!< Keep aspect ratio or not.
    bool mirror; //!< Use mirror padding instead of zero padding.
    idx<T> blurred; //!< The blurred buffer.
    idx<T> blurred_high; //!< The blurred buffer for high frequency extraction.
    idx<T> high0; //!< Temporary buffer for high frequencies.
    Tstate padded; //!< A temporary buffer for padding.
    vector<layers<T,Tstate>*> norms; //!< Brightness normalization modules.
    vector<layers<T,Tstate>*> cnorms; //!< Color normalization modules.
    midx<T> tmp;
    mstate<Tstate> tmp2, tmp3;
    Tstate inpp, outpp; //!< Temporary buffers for preprocessing.
    midx<T> outs; //!< Latest outputs.
    mstate<Tstate> zpad_out; //!< Zero-padded outputs.
    using resizepp_module<T,Tstate>::zpad;
    using resizepp_module<T,Tstate>::input_bboxes;
    using resizepp_module<T,Tstate>::original_bboxes;
    bool burt_filtering_only; //!< Only use Burt-Adelson for filtering.
  };

  ////////////////////////////////////////////////////////////////
  // mschan_module
  //! This module takes each channel of the input and create an mstate
  //! where each state contains a different channel.
  //! This is useful for fovea processing, when preprocessing was already
  //! performed by outputting each scale in a different channel.
  template <typename T, class Tstate = bbstate_idx<T> >
    class mschan_module : public s2m_module<T,Tstate> {
  public:
    //! \param nstates The number of states to produce, i.e. in how many
    //!   groups to output the channels. E.g. if input is RGBRGB and nstates = 2
    //!   then it will separate the input into 2 states of RGB and RGB.
    mschan_module(uint nstates, const char *name = "mschan");
    virtual ~mschan_module();
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, mstate<Tstate> &out);
    //! backward propagation from in to out (empty)
    virtual void bprop(Tstate &in, mstate<Tstate> &out);
    //! bbackward propagation from in to out (empty)
    virtual void bbprop(Tstate &in, mstate<Tstate> &out);
  };

  ////////////////////////////////////////////////////////////////
  // resize_module
  //! Resize the input to the desired output (not preserving aspect ratio by
  //! default, see 'preserve_ratio' parameter).
  template <typename T, class Tstate = bbstate_idx<T> >
    class resize_module: public module_1_1<T,Tstate> {
  public:
    //! This constructor specifies resizing ratio for each dimension instead
    //! of fixed target sizes. The default resizing method is bilinear, as
    //! as other methods do not currently implement ratio inputs.
    //! \param height target height for resizing.
    //! \param width target width for resizing.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param hzpad Optional vertical zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    //! \param wzpad Optional horizontal zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    //! \param preserve_ratio If true, fit the image into target size while
    //!   keeping aspect ratio, eventual empty areas are filled with zeros.
    resize_module(double hratio, double wratio, uint mode = BILINEAR_RESIZE,
		  uint hzpad = 0, uint wzpad = 0, bool preserve_ratio = false);
    //! Constructor.
    //! \param height target height for resizing.
    //! \param width target width for resizing.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param hzpad Optional vertical zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    //! \param wzpad Optional horizontal zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    //! \param preserve_ratio If true, fit the image into target size while
    //!   keeping aspect ratio, eventual empty areas are filled with zeros.
    resize_module(intg height, intg width, uint mode = MEAN_RESIZE,
		  uint hzpad = 0, uint wzpad = 0, bool preserve_ratio = false);
    //! Constructor without target dimensions. set_dimensions should be called
    //! later.
    //! \param mode The type of resizing (MEAN_RESIZE, BILINEAR_RESIZE,
    //!             GAUSSIAN_RESIZE).
    //! \param hzpad Optional vertical zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    //! \param wzpad Optional horizontal zero-padding is added on each size
    //!   and taken into account to reach the desired target size.
    //! \param preserve_ratio If true, fit the image into target size while
    //!   keeping aspect ratio, potential empty areas are filled with zeros.
    resize_module(uint mode = MEAN_RESIZE, uint hzpad = 0, uint wzpad = 0,
		  bool preserve_ratio = false);
    //! destructor
    virtual ~resize_module();
    //! sets the desired output dimensions.
    void set_dimensions(intg height, intg width);
    //! set the region to use in the input image.
    //! by default, the input region is the entire image.
    void set_input_region(const rect<int> &inr);
    //! set the region to use in the output image.
    //! by default, the output region is the entire size defined by
    //! set_dimensions().
    void set_output_region(const rect<int> &outr);
    //! Shift output by h and w pixels, multiply scale by s and rotate by r.
    void set_jitter(int h, int w, float s, float r);
    //! Set zero padding on each side for each dimension.
    void set_zpads(intg hpad, intg wpad);
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! bprop from in to out
    virtual void bprop(Tstate &in, Tstate &out);
    //! bbprop from in to out
    virtual void bbprop(Tstate &in, Tstate &out);
    //! return the bounding box of the original input in the output coordinate
    //! system.
    rect<int> get_original_bbox();
    //! Returns a deep copy of this module.
    virtual resize_module<T,Tstate>* copy();
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    
    // members ////////////////////////////////////////////////////////
  private:
    intg		 height;	//!< target height
    intg		 width;         //!< target width
    idx<T>               tmp;           //!< temporary buffer
    idx<T>               tmp2;          //!< temporary buffer
    Tstate               tmp3;          //!< temporary buffer
    rect<int>            original_bbox; //!< bbox of original input in output
    uint                 mode;          //!< resizing mode.
    int                  input_mode;    //!< mode parameter to resize function.
    rect<int>            inrect;        //!< input region of image
    rect<int>            outrect;       //!< input region in output image
    bool                 inrect_set;    //!< use input region or not.
    bool                 outrect_set;   //!< use output region or not.
    uint                 hzpad;         //!< vertical zero-padding for each side
    uint                 wzpad;         //!< horiz. zero-padding for each side
    zpad_module<T,Tstate> *zpad;        //!< Zero padding module.
    int                  hjitter;       //!< Shift output by this many pixels
    int                  wjitter;       //!< Shift output by this many pixels
    float                sjitter;       //!< Multiply scale by this
    float                rjitter;       //!< Rotate by this degrees.
    bool                 preserve_ratio;//!< Preserve aspect ratio or not.
    double               hratio;        //!< Resizing ratio in height dim.
    double               wratio;        //!< Resizing ratio in width dim.
  };

  // jitter ////////////////////////////////////////////////////////////////////

  //! This module jitters inputs into outputs.
  template<typename T, class Tstate = bbstate_idx<T> >
    class EXPORT jitter_module : public module_1_1<T,Tstate,Tstate> {
  public:
    //! Creates a jitter module.
    jitter_module(const char *name = "jitter_module");
    //! Destructor.
    virtual ~jitter_module();

    // jitters setting /////////////////////////////////////////////////////////
    //! Set translation jitter between v[0] and v[1] for height
    //! and v[2] and v[3] for width (in pixels).
    void set_translations(vector<int> &v);
    //! Set rotation jitter between v[0] and v[1] (in degrees).
    void set_rotations(vector<float> &v);
    //! Set scaling jitter between v[0] and v[1] for height
    //! and v[2] and v[3] for width.
    void set_scalings(vector<float> &v);
    //! Set shearing jitter between v[0] and v[1] for height
    //! and v[2] and v[3] for width.
    void set_shears(vector<float> &v);
    //! Set elastic smoothing size between v[0] and v[1] and output
    //! factors between v[2] and v[3].
    void set_elastics(vector<float> &v);
    //! Adds padding on top, left, bottom and right from pads[0], pads[1],
    //! pads[2], pads[3].
    void set_padding(vector<uint> &pads);
  
    // multi-state inputs and outputs //////////////////////////////////////////
    virtual void fprop(Tstate &in, Tstate &out);
    virtual void bprop(Tstate &in, Tstate &out);
    virtual void bbprop(Tstate &in, Tstate &out);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();
    //! Returns a deep copy of current module.
    virtual jitter_module<T,Tstate>* copy();
  protected:
    // variable members ////////////////////////////////////////////////////////
    int th0, th1, tw0, tw1; //!< Min and max height and widths translations.
    float deg0, deg1; //!< Min and max rotation in degrees.
    float sh0, sh1, sw0, sw1; //!< Min and max height/width scaling ratios.
    float shh0, shh1, shw0, shw1; //!< Min and max height/width shearing ratios.
    uint elsz0, elsz1; //!< Min/max elastic smoothing size.
    float elcoeff0, elcoeff1; //!< Min/max elastic coefficient.
    zpad_module<T,Tstate> *zp; //!< Zero-padding.
    Tstate tmp;
  };

} // namespace ebl {

#include "ebl_preprocessing.hpp"

#endif /* EBL_PREPROCESSING_H_ */
