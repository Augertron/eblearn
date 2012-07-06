/***************************************************************************
 *   Copyright (C) 2011 by Soumith Chintala*
 *   soumith@gmail.com  *
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

#ifndef EBL_CUDANORMALIZATION_H_
#define EBL_CUDANORMALIZATION_H_

#include "ebl_normalization.h"
#include "ebl_cudaops.h"

#ifdef __CUDA__

namespace ebl {

  //////////////////////////////////////////////////////////////////////////////
  //! This module divides local neighborhoods by they standard deviation.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_divisive_norm_module : public module_1_1<T, Tstate> {
  public:
    //! \param kerdim The neighborhood dimensions.
    //! \param nf The number of feature maps input to this module.
    //! \param mirror Use mirroring of the input to pad border if true,
    //!               or use zero-padding otherwise (default).
    //! \param p If specified, parameter p holds learned weights.
    //! \param across_features If true, normalize across feature dimensions
    //!   in addition to spatial dimensions.
    //! \param cgauss Gaussian kernel coefficient.
  cuda_divisive_norm_module(idxdim &kerdim, int nf, bool mirror = false,
			 bool threshold = true, parameter<T,Tstate> *p = NULL,
			 const char *name = "divisive_norm",
			 bool across_features = true, double cgauss = 2.0,
			 bool fsum_div = false, float fsum_split = 1.0,
                            double epsilon = 1e-6, int gpu_id_=-1);
    //! destructor
    virtual ~cuda_divisive_norm_module();    
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    virtual cuda_divisive_norm_module<T,Tstate>* copy(parameter<T,Tstate> *p = NULL);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();

  // friends
  template <typename T1, class Tstate1> friend class cuda_contrast_norm_module;

    // members ////////////////////////////////////////////////////////
  protected:
    parameter<T,Tstate> 		*param;
    layers<T, Tstate>           	 convvar;
  cuda_power_module<T,Tstate>		 sqrtmod;	//!< square root module
  cuda_power_module<T,Tstate>		 sqmod;	//!< square module
    thres_module<T,Tstate>		 thres;	//!< threshold module
  cuda_convolution_module<T,Tstate> 	*divconv;
    module_1_1<T,Tstate>        	*padding;
    idx<T>              		 w;	//!< weights
    Tstate  	        		 insq, invar, instd, thstd;
    bool                		 threshold;
    int                 		 nfeatures;
    idxdim              		 kerdim;
    bool 				 across_features;//!< Norm across feats.
    double                               epsilon; //!< bias to avoid div by 0.
    double                               cgauss; 
    bool fsum_div;
    bool fsum_split;
  public:
  // GPU members /////////////////////////////////////////////////////////////
  int                 gpu_id; //!< Whether to use gpu or not
  };

  //////////////////////////////////////////////////////////////////////////////
  //! Subtractive normalization operation using a weighted expectation
  //! over a local neighborhood. An input set of feature maps is locally
  //! normalized to be zero mean.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_subtractive_norm_module : public module_1_1<T, Tstate> {
  public:
  //! \param kerdim The kernel dimensions.
  //! \param nf The number of feature maps input to this module.
  //! \param mirror Use mirroring of the input to pad border if true,
  //!               or use zero-padding otherwise (default).
  //! \param global_norm If true, apply global normalization first.
  //! \param p If specified, parameter p holds learned weights.
  //! \param across_features If true, normalize across feature dimensions
  //!   in addition to spatial dimensions.
  //! \param learn_mean If true, learn mean weighting.
  //! \param cgauss Gaussian kernel coefficient.
  cuda_subtractive_norm_module(idxdim &kerdim, int nf, bool mirror = false,
                               bool global_norm = false,
                               parameter<T,Tstate> *p = NULL,
                               const char *name = "subtractive_norm",
                               bool across_features = true,
                               double cgauss = 2.0, bool fsum_div = false,
                               float fsum_split = 1.0, int gpu_id_ = -1);
  //! destructor
  virtual ~cuda_subtractive_norm_module();    
  //! forward propagation from in to out
  virtual void fprop(Tstate &in, Tstate &out);
  //! Returns a deep copy of this module.
  //! \param p If NULL, reuse current parameter space, otherwise allocate new
  //!   weights on parameter 'p'.
  virtual cuda_subtractive_norm_module<T,Tstate>*
  copy(parameter<T,Tstate> *p = NULL);
  //! Returns a string describing this module and its parameters.
  virtual std::string describe();

  // friends
  template <typename T1, class Tstate1> friend class cuda_contrast_norm_module;
  
  // members ////////////////////////////////////////////////////////
  protected:
  parameter<T,Tstate> 		*param;
  layers<T, Tstate>   		 convmean;
  cuda_convolution_module<T,Tstate> 	*meanconv;
  module_1_1<T,Tstate>        	*padding;
  idx<T>              		 w;	//!< weights
  diff_module<T,Tstate>		 difmod;//!< difference module
  Tstate  	        		 inmean;
  bool                		 global_norm;//!< global norm first
  int                 		 nfeatures;
  idxdim              		 kerdim;
  bool 				 across_features;//!< Norm across feats.
  double                               cgauss; 
  bool fsum_div;
  bool fsum_split;
  public:
  // GPU members /////////////////////////////////////////////////////////////
  int                 gpu_id; //!< Whether to use gpu or not
  };


  //////////////////////////////////////////////////////////////////////////////
  //! Local contrast normalization operation using a weighted expectation
  //! over a local neighborhood. An input set of feature maps is locally
  //! normalized to be zero mean and unit standard deviation.
  template <typename T, class Tstate = bbstate_idx<T> >
    class cuda_contrast_norm_module : module_1_1<T,Tstate> {
  public:
    //! \param kerdim The kernel dimensions.
    //! \param nf The number of feature maps input to this module.
    //! \param mirror Use mirroring of the input to pad border if true,
    //!               or use zero-padding otherwise (default).
    //! \param global_norm If true, apply global normalization first.
    //! \param p If specified, parameter p holds learned weights.
    //! \param across_features If true, normalize across feature dimensions
    //!   in addition to spatial dimensions.
    //! \param learn_mean If true, learn mean weighting.
    //! \param cgauss Gaussian kernel coefficient.
  cuda_contrast_norm_module(idxdim &kerdim, int nf, bool mirror = false,
			 bool threshold = true, bool global_norm = false,
			 parameter<T,Tstate> *p = NULL,
			 const char *name = "contrast_norm",
			 bool across_features = true, bool learn_mean = false,
			 double cnorm = 2.0, bool fsum_div = false,
                            float fsum_split = 1.0, double epsilon = 1e-6, 
                            int gpu_id = -1);
    //! destructor
    virtual ~cuda_contrast_norm_module();    
    //! forward propagation from in to out
    virtual void fprop(Tstate &in, Tstate &out);
    //! Returns a deep copy of this module.
    //! \param p If NULL, reuse current parameter space, otherwise allocate new
    //!   weights on parameter 'p'.
    virtual cuda_contrast_norm_module<T,Tstate>* copy(parameter<T,Tstate> *p = NULL);
    //! Returns a string describing this module and its parameters.
    virtual std::string describe();

    // members ////////////////////////////////////////////////////////
  protected:
  cuda_subtractive_norm_module<T, Tstate> 	subnorm;
  cuda_divisive_norm_module<T, Tstate> 	divnorm;
  Tstate  	        		tmp;
  bool                		global_norm;//!< global norm first
  bool                		learn_mean;//!< Learn mean weighting.
  public:
  // GPU members /////////////////////////////////////////////////////////////
  int                 gpu_id; //!< Whether to use gpu or not

  };



} // namespace ebl

#include "ebl_cudanormalization.hpp"

#endif // __CUDA__

#endif /* EBL_CUDANORMALIZATION_H_ */
