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
  
  template <class T> class detector {
  public:
    module_1_1<T>	&thenet;
    int			 height;
    int			 width;
    idx<T>		 grabbed;
    idx<T>		 grabbed2;
    double		 contrast;
    double		 brightness;
    float		 coef;
    T			 bias;    
    idx<float>		 sizes;
    idx<void*>		 inputs;	//! state_idx*
    idx<void*>		 outputs;	//! state_idx*
    idx<void*>		 results;	//! idx<double>*
    idx<T>		 smoothing_kernel;
    idx<ubyte>   	 labels;
    ////////////////////////////////////////////////////////////////
  private:
    // dimensions
    idxdim		 in_mindim;
    idxdim		 in_maxdim;
    uint		 nresolutions;
    idx<uint>		 resolutions;
    idx<uint>		 original_bboxes; //!< bboxes orig image after resizing
    bool		 manual_resolutions;
    int                  bgclass;
    idxdim               input_dim;
    
  public:
    
    ////////////////////////////////////////////////////////////////
    // constructors
    
    //! Constructor.
    //! \param lbls A const char* idx containing class name strings.
    detector(module_1_1<T> &thenet, unsigned int nresolutions, 
	     idx<const char*> &lbls, T bias = 0, float coeff = 1.0);
    
    //! Constructor. lbls is an idx containing each class name.
    //! \param lbls A ubyte idx containing class name strings.
    detector(module_1_1<T> &thenet, unsigned int nresolutions, 
	     idx<ubyte> &lbls, T bias = 0, float coeff = 1.0);
    
    //! Constructor.
    //! \param lbls A ubyte idx containing class name strings.
    //! \param resolutions A uint idx containing resolutions (of size nx2)
    detector(module_1_1<T> &thenet, idx<unsigned int> &resolutions,
	     idx<ubyte> &lbls, T bias = 0, float coeff = 1.0);

    //! Destructor.
    virtual ~detector();

    ////////////////////////////////////////////////////////////////

    //! fprop input image throught network.
    //! if image's and network's type differ, cast image into network's type
    //! through an idx_copy (avoid for better performance).
    template <class Tin>
      vector<bbox> fprop(idx<Tin> &img, T threshold);

    //! set background class (which will be ignored).
    void set_bgclass(const char *bg);

  private:
    //! initialize dimensions and multi-resolution buffers.
    void init(idxdim &dinput);

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

    //! do a fprop on thenet with multiple rescaled inputs
    void multi_res_fprop(idx<T> &sample);       

    //! find maximas in output layer
    void mark_maxima(T threshold);

    //! prune btwn scales
    vector<bbox> map_to_list(T threshold);
    void pretty_bboxes(vector<bbox> &vb);
  };

} // end namespace ebl

#include "detector.hpp"

#endif /* DETECTOR_H_ */
