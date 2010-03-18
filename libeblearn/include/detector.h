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
    int		class_id;	//<! object class
    double	confidence;	//<! detection confidence, 1 is the best.
    double	scaleh;		//<! scale factor at which object was detected
    double	scalew;		//<! scale factor at which object was detected
    int		scale_index;	//<! scale index at which object was detected
    // original map //////////////////////////////////////////////
    uint	h0;		//<! height of top left pixel
    uint	w0;		//<! width of top left pixel
    uint	height;		//<! height of bounding box in original image
    uint	width;		//<! width of bounding box in original image
    // input map /////////////////////////////////////////////////
    uint	iheight;	//<! scaled input image height
    uint	iwidth;		//<! scaled input image width
    uint	ih0;		//<! height0 of bbox in network's input map
    uint	iw0;		//<! width0 of bbox in network's input map
    uint	ih;		//<! height of bbox in network's input map
    uint	iw;		//<! width of bbox in network's input map
    // output map ////////////////////////////////////////////////
    uint	oheight;	//<! height of network's output map
    uint	owidth;		//<! width of network's output map
    uint	oh0;		//<! pixel's height in network's output map
    uint	ow0;		//<! pixel's width in network's output map
  };

  ////////////////////////////////////////////////////////////////
  // detector

  //! different types of resolutions
  //! MANUAL: resolutions are specified manually by height and width
  //! SCALES: a series of scaling factors, 1 being the network's size
  //! NSCALES: n scales are computed by evenely interpolating between network's
  //!          size and maximum resolution.
  //! SCALES_STEP: scales range from 1 to maximum resolution, with a step size
  enum t_resolution { MANUAL, SCALES, NSCALES, SCALES_STEP };
  
  template <class T> class detector {
  public:    
    ////////////////////////////////////////////////////////////////
    // constructors
    
    //! Constructor. Default resolutions are 1, 2 and 4 times the network's 
    //! size. Resolutions can be set using set_resolutions().
    //! Background class name default "bg" will be searched in the list
    //! of class names. To specify another background class, pass a non NULL
    //! background parameter.
    //! \param lbls A const char* idx containing class name strings.
    //! \param pp A preprocessing module, e.g. rgb_to_yp_module.
    //! \param ppkersz The size of the preprocessing kernel (if any), to take
    //!               border effects into account during resizing.
    //!               Default value 0 has no effect.
    //! \param background The name of the background class. Default is "bg".
    //!          If given, positive answers for this class are ignored.
    detector(module_1_1<T> &thenet, idx<ubyte> &lbls,
	     module_1_1<T> *pp = NULL, uint ppkersz = 0,
	     const char *background = NULL, T bias = 0, float coeff = 1.0);

    //! Destructor.
    virtual ~detector();

    ////////////////////////////////////////////////////////////////
    // configuration

    //! \param nresolutions The number of resolutions to use.
    //! \param scales This is an array of size nresolutions describing
    //!               the scales to use, taking the minimum resolution as scale
    //!               1. e.g. if our input image is 500x960, and our network
    //!               size is 96x96, the minimum resolution that fits inside
    //!               96x96 is 96x50. So this is scale 1, scale 2 will then be
    //!               192x100. Resolutions are automatically fit to the network
    //!               size, so 192x100 will be changed to closest
    //!               network-compatible size, i.e. 192x96 in that case
    //!               (still preserving the aspect ratio).
    void set_resolutions(uint nresolutions, const double *scales);

    //! Use nresolutions resolutions between the maximum resolution and the
    //! minimum resolution.
    //! \param nresolutions The number of resolutions to use.
    void set_resolutions(int resolutions);

    //! Specify resolutions by hand in an nx2 idx (heightxwidth),
    //! e.g. 240x320, 120x160.
    //! \param resolutions A uint idx containing resolutions (of size nx2)
    void set_resolutions(idx<uint> &resolutions);

    //! Specify resolutions by the factor step, starting from factor 1
    //! (network's size), adding factor_steps until reaching the original
    //! resolution.
    void set_resolutions(double scales_steps);

    //! set background class (which will be ignored).
    void set_bgclass(const char *bg);

    //! set detector to silent: do not print results on std output
    void set_silent();

    //! Enable saving of each (preprocessed) window inducing a positive
    //! detection into directory. All detections except for the background
    //! class are dumped into a directory corresponding to the class' name.
    void set_save(const string directory);

    //! Set the maximum size of each side of an input to use as input
    //! to the network. I.e. a input to the network will be at most
    //! max_size * max_size big.
    //! \param max_size The maximum width or height in pixels.
    void set_max_resolution(uint max_size);

    ////////////////////////////////////////////////////////////////
    // execution
    
    //! fprop input image throught network.
    //! if image's and network's type differ, cast image into network's type
    //! through an idx_copy (avoid for better performance).
    template <class Tin>
      vector<bbox*>& fprop(idx<Tin> &img, T threshold);

    //! Return a reference to a vector of windows in the original image that
    //! yielded a detection.
    vector<idx<T> >& get_originals();

    //! Return a reference to a vector of windows in the preprocessed/scaled
    //! image that yielded a detection.
    vector<idx<T> >& get_preprocessed();

    //! Returns the number of bboxes saved so far.
    uint get_total_saved();

    //! Limit the number of regions saved per frame.
    uint set_save_max_per_frame(uint max);

  private:
    //! initialize dimensions and multi-resolution buffers.
    void init(idxdim &dinput);

    //! compute and set minimum and maximum resolutions
    //! based on input size <input_dims>.
    void compute_minmax_resolutions(idxdim &input_dims);
    
    //! compute sizes of each resolutions based on input size <input_dims>,
    //! choosing nresolutions going from the minimum to the maximum resolutions.
    void compute_resolutions(idxdim &input_dims, uint &nresolutions);

    //! compute sizes of each resolutions based on input size <input_dims>,
    //! using scales, taking the minimum resolution as scale 1.
    void compute_resolutions(idxdim &input_dims, uint nresolutions,
			     const double *scales);

    //! compute sizes of each resolutions based on input size <input_dims>,
    //! using scales with a fixed step between each of them, from min to max
    //! resolutions.
    void compute_resolutions(idxdim &input_dims, double scales_step);

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

    //! Prune bounding boxes between scales into prune_bboxes.
    void prune(vector<bbox> &raw_bboxes, vector<bbox*> &prune_bboxes);

    //! Smooth outputs.
    void smooth_outputs();

    //! Extract bounding boxes from outputs into raw_bboxes.
    void map_to_list(T threshold, vector<bbox> &raw_bboxes);

    //! Print bounding boxes on standard output.
    void pretty_bboxes(const vector<bbox*> &bboxes);

    //! save all bounding boxes of original (in original resolution) and
    //! preprocessed (resized and filtered) input into directory dir.
    void save_bboxes(const vector<bbox*> &bboxes, const string &dir);

    ////////////////////////////////////////////////////////////////
    // members
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
    idx<void*>		 inputs;	//!< state_idx*
    idx<void*>		 outputs;	//!< state_idx*
    idx<void*>		 results;	//!< idx<double>*
    idx<void*>           resize_modules;//!< module_1_1<T>*
    idx<void*>           nets;          //!< module_1_1<T>*
    module_1_1<T>       *pp;            //!< preprocessing module
    uint                 ppkersz;       //!< size of pp kernel (if any)
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
    int                  bgclass;
    idxdim               input_dim;
    const double        *scales;
    double               scales_step;
    bool                 silent; //!< print results on std output if not silent
    t_resolution         restype; //!< resolution type
    bool                 save_mode; //!< save detected windows or not
    string               save_dir; //!< directory where to save detections
    vector<uint>         save_counts; //!< file counter for each class
    vector<bbox>         raw_bboxes; //!< raw bboxes extracted from outputs
    vector<bbox*>        pruned_bboxes; //!< scale-pruned bboxes
    uint                 max_size; //!< maximum input size to network
    vector<idx<T> >      odetections; //!< original windows yielding detection
    vector<idx<T> >      ppdetections; //!< preprocessed wins yielding detection
    bool                 bodetections; //!< odetections is up-to-date or not
    bool                 bppdetections; //!< ppdetections is up-to-date or not
    uint                 save_max_per_frame; //!< max number of region saved

    ////////////////////////////////////////////////////////////////
    // friends
    template <typename T2>
    friend class detector_gui;
  };

} // end namespace ebl

#include "detector.hpp"

#endif /* DETECTOR_H_ */
