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

#ifndef DETECTOR_H_
#define DETECTOR_H_

#include "libeblearn.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // bbox
  
  //! bounding box class.
  class bbox {
  public:
    //! object class
    int class_id;
    //! detection confidence, 1 is the best.
    double confidence;
    //! scale factor at which object was detected
    double scaleh;
    //! scale factor at which object was detected
    double scalew;
    //! scale index at which object was detected
    int scale_index;
    //! scaled input image height
    unsigned int iheight;
    //! scaled input image width
    unsigned int iwidth;
    //! height top left pixel origin in original image
    unsigned int h0;
    //! width top left pixel origin in original image
    unsigned int w0;
    //! height of bounding box in original image
    unsigned int height;
    //! width of bounding box in original image
    unsigned int width;
    //! output height
    unsigned int oheight;
    //! output width
    unsigned int owidth;
    //! output pixel height
    unsigned int oh;
    //! output pixel width
    unsigned int ow;
  };
  
  ////////////////////////////////////////////////////////////////
  // detector
  
  template <class Tdata> class detector {
  public:
    module_1_1<state_idx,state_idx>	&thenet;
    int					 height;
    int					 width;
    idx<Tdata>				 grabbed;
    idx<Tdata>				 grabbed2;
    double				 contrast;
    double				 brightness;
    double				 coef;
    double				 bias;    
    idx<float>				 sizes;
    idx<void*>				 inputs;	//! state_idx*
    idx<void*>				 outputs;	//! state_idx*
    idx<void*>				 results;	//! idx<double>*
    idx<double>				 smoothing_kernel;
    idx<const char*>			 labels;
    ////////////////////////////////////////////////////////////////
  private:
    // dimensions
    idxdim				 in_mindim;
    idxdim				 in_maxdim;
    unsigned int			 nresolutions;
    idx<unsigned int>			 resolutions;
    bool				 manual_resolutions;
    
  public:
    
    ////////////////////////////////////////////////////////////////
    // constructors
    
    //! Constructor.
    detector(module_1_1<state_idx, state_idx> &thenet, 
		 unsigned int nresolutions, 
		 idx<const char*> &lbls, double b, double c);
    
    //! Constructor.
    detector(module_1_1<state_idx, state_idx> &thenet, 
		 idx<unsigned int> &resolutions, 
		 idx<const char*> &lbls, double b, double c);
    //    detector(module_1_1<state_idx, state_idx> &thenet);
    virtual ~detector();

    ////////////////////////////////////////////////////////////////
    
    vector<bbox> fprop(idx<Tdata> &img, double threshold = 1.8);

  private:
    //! initialize dimensions and multi-resolution buffers.
    void init(idx<Tdata> &input);

    //! compute sizes of each resolutions based on input size <input_dims>.
    void compute_minmax_resolutions(idxdim &input_dims);
    
    //! compute sizes of each resolutions based on input size <input_dims>.
    void compute_resolutions(idxdim &input_dims, unsigned int nresolutions);

    //! print all resolutions.
    void print_resolutions();

    //! checks that resolutions match the network size, if not adjust them.
    //! this method assumes nresolutions and resolutions members have already
    //! been initialized.
    void validate_resolutions();
    
    // Sub functions
    //    idx<Tdata> multi_res_prep(idx<Tdata> &img, float zoom);

    //! do a fprop on thenet with multiple rescaled inputs
    void multi_res_fprop(idx<Tdata> &sample);   
    //    idx<double> multi_res_fprop(double threshold, int objsize);
    
    //    idx<double> postprocess_output(double threshold, int objsize);
    

    //! find maximas in output layer
    void mark_maxima(double threshold);

/*     //! mark local maxima (in space and feature) of in r. */
/*     //! Put winning class in (r i j 0) and score (normalized */
/*     //! to 0 1) in (r i j 1). */
/*     void mark_maxima(idx<double> &in, idx<double> &inc,  */
/* 		     idx<double> &r, double threshold); */
    
//    idx<double> prune(idx<double> &res);

    //! prune btwn scales
    vector<bbox> map_to_list(double threshold);
    void pretty_bboxes(vector<bbox> &vb);
  };

  ////////////////////////////////////////////////////////////////

/*   template <class Tdata> */
/*   class detector_binocular : public detector<Tdata> { */
/*   public: */
/*     using detector<Tdata>::grabbed; */
/*     using detector<Tdata>::grabbed2; */
/*     using detector<Tdata>::height; */
/*     using detector<Tdata>::width; */
/*     using detector<Tdata>::coeff; */
/*     using detector<Tdata>::bias; */
/*     using detector<Tdata>::inputs; */
/*     using detector<Tdata>::outputs; */

/*     detector_binocular(module_1_1<state_idx, state_idx> &thenet, */
/* 		      idx<int> &sz, idx<const char*> &lbls, */
/* 		      double b, double c, int h, int w); */
/*     virtual ~detector_binocular(); */
  
/*     //! Compute multi-resolution inputs and fprop through each. */
/*     idx<double> fprop(Tdata *left, Tdata *right,  */
/* 		      float zoom, int dx, int dy, double threshold = 1.8,  */
/* 		      int objsize = 60); */
  
/*     // Sub functions */
/*     void multi_res_prep(Tdata *left, Tdata *right,  */
/* 			int dx, int dy, float zoom); */
/*   }; */

  ////////////////////////////////////////////////////////////////

/*   template <class Tdata> */
/*   class classifierNMS : public detector<Tdata> { */
/*   public: */
/*     double		       coef; */
/*     double		       bias; */
/*     idx<double>		       sizes; */
/*     idx<const char*>	       labels; */
/*     idx<Tdata>                 sample; */
/*     using detector<Tdata>::width; */
/*     using detector<Tdata>::height; */
/*     using detector<Tdata>::thenet; */
/*     using detector<Tdata>::inputs; */
/*     using detector<Tdata>::outputs; */
/*     using detector<Tdata>::results; */
/*     using detector<Tdata>::grabbed; */
	
/*     //! Constructor. */
/*     classifierNMS(module_1_1<state_idx, state_idx> &thenet_, */
/* 		  idx<double> &sizes_,  */
/* 		  idx<const char*> &labels_, */
/* 		  idx<Tdata> &sample_, */
/* 		  double bias_, double coef_); */
/*     ~classifierNMS(); */

/*     //! do a fprop on thenet with multiple rescaled inputs */
/*     void multi_res_fprop(); */

/*     //! call multi_res_fprop(), and analyze the output map */
/*     idx<double> classify(double threshold); */
    
/*     //! find maximas in output layer */
/*     void mark_maxima(double threshold); */

/*     //! prune btwn scales */
/*     idx<double> map_to_list(double threshold); */

/*   }; */

} // end namespace ebl

#include "detector.hpp"

#endif /* DETECTOR_H_ */
