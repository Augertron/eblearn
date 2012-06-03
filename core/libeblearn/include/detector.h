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

#ifndef DETECTOR_H_
#define DETECTOR_H_

#include "libidx.h"
#include "ebl_states.h"
#include "ebl_arch.h"
#include "ebl_answer.h"
#include "ebl_merge.h"
#include "ebl_preprocessing.h"
#include "bbox.h"
#include "nms.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // detector

  //! different types of resolutions
  //! MANUAL: resolutions are specified manually by height and width
  //! SCALES: a series of scaling factors, 1 being the network's size
  //! NSCALES: n scales are computed by evenely interpolating between network's
  //!          size and maximum resolution.
  //! SCALES_STEP: scales range from 1 to maximum resolution, with a step size
  //! ORIGINAL: only use image's original resolution.
  //! NETWORK: resize all inputs to the minimal network's input size.
  //! SCALES_STEP_UP: scale step from min up to max scale.
  enum t_scaling { MANUAL = 0, SCALES = 1, NSCALES = 2, SCALES_STEP = 3,
		   ORIGINAL = 4, NETWORK = 5, SCALES_STEP_UP = 6 };

  template <typename T, class Tstate = fstate_idx<T> >
    class detector {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors

    //! Constructor. Default resolutions are 1, 2 and 4 times the network's
    //! size. Resolutions can be set using set_resolutions().
    //! Background class name default "bg" will be searched in the list
    //! of class names. To specify another background class, pass a non NULL
    //! background parameter.
    //! \param labels A vector of label strings.
    //! \param pp An optional resizing (and preprocessing) module,
    //!   e.g. resizepp_module. If null, use resize_module by default.
    //! \param background The name of the background class. Default is "bg".
    //!          If given, positive answers for this class are ignored.
    //! \param adapt_scales If true, adapt each scale so that they are valid
    //!          input sizes for 'thenet' network. Otherwise, the network
    //!          must crop inputs itself (see 'crop' attribute of modules).
    detector(module_1_1<T,Tstate> &thenet, vector<string> &labels,
	     answer_module<T,T,T,Tstate> *answer = NULL,
	     resizepp_module<T,Tstate> *resize = NULL,
	     const char *background = NULL,
	     std::ostream &out = std::cout, std::ostream &err = std::cerr,
	     bool adapt_scales = false);

    //! Destructor.
    virtual ~detector();

    ////////////////////////////////////////////////////////////////
    // configuration

    //! Set the multi-scale to 1 scale only: the image's original scale.
    void set_scaling_original();
    //! Set the scaling type.
    void set_scaling_type(t_scaling type);
    //! Set all scales manually.
    void set_resolutions(const midxdim &scales);
    //! Set scales a factors of the input sizes.
    void set_resolutions(const vector<double> &factors);
    //! Set 1 scale only as a factor of the input sizes.
    void set_resolution(double factor);
    //! Use nresolutions resolutions between the maximum resolution and the
    //! minimum resolution.
    //! \param nresolutions The number of resolutions to use.
    void set_resolutions(int resolutions);
    //! Specify resolutions by hand in an nx2 idx (heightxwidth),
    //! e.g. 240x320, 120x160.
    //! \param resolutions A uint idx containing resolutions (of size nx2)
    void set_resolutions(idx<uint> &resolutions);
    //! Add zero padding of (hzpad * the network's minimum input height)
    //! on each vertical sides and (wzpad * min width) on horizontal sides.
    void set_zpads(float hzpad, float wzpad);
    //! Specify resolutions by the factor step, starting from factor 1
    //! (network's size), adding factor_steps until reaching the original
    //! resolution.
    //! \param max_scale The maximum scale factor of the original resolution,
    //!    1.0 by default, i.e. the original resolution.
    //! \param min_scale The minimum scale factor of the smallest network size
    //!    1.0 by default, meaning the minimum network input size.
    void set_resolutions(double scales_steps, double max_scale = 1.0,
			 double min_scale = 1.0);
    //! Return the id of the class 'name' or -1 if not found.
    int get_class_id(const string &name);
    //! set background class (which will be ignored).
    void set_bgclass(const char *bg = NULL);
    //! Set the mask class, which is ignored by bounding box detection.
    //! Instead, call get_mask() to retrieve a mask map of values above
    //! a given threshold. This is useful for continuous classes rather
    //! than discrete classes.
    bool set_mask_class(const char *mask);
    //! set detector to silent: do not print results on std output
    void set_silent();
    //! Set the minimum size of each side of an input to use as input
    //! to the network. I.e. a input to the network will be at least
    //! min_size * min_size big.
    //! \param min_size The minimum width or height in pixels.
    void set_min_resolution(uint min_size);
    //! Set the maximum size of each side of an input to use as input
    //! to the network. I.e. a input to the network will be at most
    //! max_size * max_size big.
    //! \param max_size The maximum width or height in pixels.
    void set_max_resolution(uint max_size);
    //! Set different thresholds for each scale during raw extraction.
    void set_raw_thresholds(vector<float> &t);

    //! Enable nms of type 'type'. Refer to t_pruning declaration for
    //! different types. Default type is 1, regular pruning.
    void set_nms(t_nms type = nms_overlap, float pre_threshold = 0.0,
		 float post_threshold = 0.0,
		 float pre_hfact = 1.0, float pre_wfact = 1.0,
		 float post_hfact = 1.0, float post_wfact = 1.0,
		 float woverh = 1.0, float max_overlap = 1.0,
		 float max_hcenter_dist = 0.0, float max_wcenter_dist = 0.0,
		 float vote_max_overlap = 1.0,float vote_max_hcenter_dist = 0.0,
		 float vote_max_wcenter_dist = 0.0);

    //! Enable or disable scaler mode, i.e. using scale prediction for boxes.
    void set_scaler_mode(bool set);
    //! Set output smoothing type. 0: none, 1: 3x3 kernel.
    void set_smoothing(uint type);

    //! Enable memory optimization by using only 2 buffers (in and out)
    //! for entire flow. Those same buffers must have been passed to the
    //! network's constructor.
    //! \param keep_inputs If false, re-use input buffers for optimization.
    //!   If true, some operations may be unavailable, such as saving
    //!   the pre-processed detected windows.
    void set_mem_optimization(Tstate &in, Tstate &out,
			      bool keep_inputs = false);
    //! Set by hand the minimum network input and fix it.
    void set_netdim(idxdim &d);
    //! Enables dumping of all outputs using the base name 'name', to which
    //! is appending the idx's size and '.mat'. Each resolution
    //! will be dump as a separate matrix file.
    //! Dumping will be called at the end of each fprop call for each
    //! resolution.
    void set_outputs_dumping(const char *name);
    //! Turn off the extraction of bounding boxes.
    void set_bboxes_off();
    //! Returns the vector of label strings.
    vector<string>& get_labels();
    //! Ignore bboxes that overlap with the outside of the image.
    void set_ignore_outsiders();
    //! The image corners transformation from outputs to input is infered
    //! back through the network (type is 0 or 1), and saved into "corners.mat"
    //! (type 1), or loaded from "corners.mat" only (type 2).
    void set_corners_inference(uint type);
    //! Select the bbox extraction decision type.
    //! 0: decision based on confidence threshold.
    //! 1: only extract output corners.
    void set_bbox_decision(uint type);
    void set_bbox_scalings(mfidxdim &scalings);

    ////////////////////////////////////////////////////////////////
    // execution

    //! fprop input image throught network.
    //! if image's and network's type differ, cast image into network's type
    //! through an idx_copy (avoid for better performance).
    //! \param fname Optional name for the frame being processed, used
    //!          in the output files to be saved.
    template <class Tin> bboxes& fprop(idx<Tin> &img, const char *fname = NULL);
    //! Run non-maximum suppression on 'in' and put result in 'out'.
    void fprop_nms(bboxes &in, bboxes &out);
    //! Return a reference to a vector of windows in the original image that
    //! yielded a detection.
    vector<idx<T> >& get_originals();
    //! Return the preprocessed input corresponding to bounding box 'b'
    //! or throws an exception if out of bounds.
    midx<T> get_preprocessed(const bbox &b);
    //! Return a reference to a vector of windows in the preprocessed/scaled
    //! image that yielded a detection.
    //! \param out A vector of boxes filled with returned samples boxes.
    //! \param n Limit number of samples to 'n'. If n equals 0, return all.
    //! \param diverse If true, order samples by diversity.
    //! \param pre_diverse_max Limit the number of samples if diverse is enabled
    //!   because it is an expensive process with complexity O(n^2).
    svector<midx<T> >& get_preprocessed(bboxes &out, uint n = 0,
					bool diverse = false,
					uint pre_diverse_max = 100);
    //! Return a reference to a vector of windows in the preprocessed/scaled
    //! image that yielded a detection.
    //! \param in Input boxes of samples to return.
    //! \param out A vector of boxes filled with returned samples boxes.
    //! \param n Limit number of samples to 'n'. If n equals 0, return all.
    //! \param diverse If true, order samples by diversity.
    //! \param pre_diverse_max Limit the number of samples if diverse is enabled
    //!   because it is an expensive process with complexity O(n^2).
    svector<midx<T> >& get_preprocessed(bboxes &in, bboxes &out, uint n = 0,
					bool diverse = false,
					uint pre_diverse_max = 100);
    //! Return a mask of output maps with the same size as the input.
    //! The mask is a max of all output resolution.
    idx<T> get_mask(string &classname);
    //! Returns the number of bboxes saved so far.
    uint get_total_saved();
    //! Enable saving of each (preprocessed) window inducing a positive
    //! detection into directory. All detections except for the background
    //! class are dumped into a directory corresponding to the class' name.
    //! This returns the directory string used.
    //! \param nmax Limit the number of windows saved per frame.
    //! \param diversity If true, order samples to be saved by diversity.
    string& set_save(const string &directory, uint nmax = 0,
		     bool diversity = false);
    //! initialize dimensions and multi-resolution buffers.
    void init(idxdim &dinput, const char *frame_name = NULL);

  protected:
    // scales methods //////////////////////////////////////////////////////////

    //! Compute all scales based on minimum, maximum and input dimensions,
    //! and scaling type.
    //! \param netdim The network's minimal input size.
    //! \param mindim The minimum scale size.
    //! \param maxdim The maximum scale size.
    //! \param indim The original input dimensions.
    void compute_scales(midxdim &scales, idxdim &netdim, idxdim &mindim,
			idxdim &maxdim, idxdim &indim, t_scaling type,
			uint nscales, double scales_step,
			const char *frame_name = NULL);
    //! Compute 'nscales' scales between 'mindim' and 'maxdim' resolutions
    //! and push them into 'scales' vector.
    void compute_resolutions(midxdim &scales,
			     idxdim &mindim, idxdim &maxdim, uint nscales);
    //! Compute each scale as a factor of 'indim' for each element of
    //! 'scale_factors' and put them into 'scales' vector.
    void compute_resolutions(midxdim &scales,
			     idxdim &indim, vector<double> &scale_factors);
    //! Compute each scale with a step of 'scales_step' starting from 'maxdim'
    //! down to 'mindim'.
    //! \param mindim The minimum scale size.
    //! \param maxdim The maximum scale size.
    void compute_resolutions(midxdim &scales, idxdim &mindim,
			     idxdim &maxdim, double scales_step);
    //! Compute each scale with a step of 'scales_step' starting from 'mindim'
    //! up to 'maxdim'.
    //! \param mindim The minimum scale size.
    //! \param maxdim The maximum scale size.
    void compute_resolutions_up(midxdim &scales, idxdim &indim,
				idxdim &mindim, idxdim &maxdim,
				double scales_step);
    //! checks that resolutions match the network size, if not adjust them.
    //! this method assumes nresolutions and resolutions members have already
    //! been initialized.
    void validate_resolutions();
    //! Figure out mimimum input dimensions to network.
    void get_netdim(intg order0);

    // bboxes operations ///////////////////////////////////////////////////////

    //! Smooth outputs.
    void smooth_outputs();
    //! If a merge module was found in the network, update its parameters so
    //! that merging is aligned on top left corner of all inputs.
    void update_merge_alignment();
    //! Fills internal buffers with 4 image corners coordinates in input
    //! and preprocessing space given the output sizes of 'outputs'.
    void get_corners(mstate<Tstate> &outputs, uint scale, bool force = false);
    //! Extract bounding boxes with higher confidence than 'threshold'
    //! from internal 'outputs' into 'bboxes'.
    void extract_bboxes(T threshold, bboxes &bboxes);
    //! save all bounding boxes of original (in original resolution) and
    //! preprocessed (resized and filtered) input into directory dir.
    void save_bboxes(bboxes &bboxes, const string &dir,
		     const char *frame_name = NULL);
    //! Add a name to the vector of class names.
    //! This can be useful when generating
    //! intermediate classes from existing classes.
    void add_class(const char *name);

    // processing methods //////////////////////////////////////////////////////

    //! Prepare image and resolutions. This should be called before
    //! preprocess_resolution().
    //! This mostly involves casting image into network's type and computing
    //! each scale's dimensions (no resizing) based on image's size.
    template <class Tin> void prepare(idx<Tin> &img, const char *fname = NULL);
    //! Do preprocessing (resizing and channel/edge processing) for a particular
    //! resolution. This will set 'input' and 'output' buffers, that can then
    //! be used to fprop the network. This uses the 'image' member prepared
    //! by prepare() and should therefore be called after prepare().
    //! \param res The resolution to be preprocessed.
    void prepare_scale(uint i);
    //! do a fprop on thenet with multiple rescaled inputs
    void multi_res_fprop();

    // member variables ////////////////////////////////////////////////////////
  protected:
    module_1_1<T,Tstate>	&thenet; //!< The network.
    resizepp_module<T,Tstate>   *resizepp; //!< Resize module for multi-scaling.
    bool                 resizepp_delete; //!< We are responsible for deleting.
    idx<T>		 image;
    double		 contrast;
    double		 brightness;
    idx<float>		 sizes;
    fstate_idx<T>        finput; //! A forward buffer containing input image.
    Tstate              *input;        //!< input buffer
    mstate<Tstate>       output;       //!< output buffer
    Tstate              *tmp;           //!< tmp.
    Tstate              *minput; //!< input buffer, used with mem optim.
    svector<mstate<Tstate> > ppinputs; //!< Preprocessed inputs of all scales.
    svector<mstate<Tstate> > outputs; //!< Output buffers of all scales.
    vector<string>   	 labels; //!< String label of each class.
  protected:
    // dimensions //////////////////////////////////////////////////////////////
    idxdim               indim; //!< Input dimensions.
    idxdim		 netdim; //!< network's input dimensions
    bool                 netdim_fixed; //!< Do not update netdim if true.
    // bboxes //////////////////////////////////////////////////////////////////
    vector<rect<int> >	 original_bboxes; //!< Bboxes in image after resizing.
    int                  bgclass;
    int                  mask_class;
    idx<T>               mask;
    nms                  *pnms; //!< Non-maximum suppression object.
    // scales //////////////////////////////////////////////////////////////
    midxdim		 scales; //!< Multi-scale (ideal) scales.
    midxdim		 actual_scales;	//!< Actually used scales.
    midxdim		 manual_scales;	//!< Scales set manually.
    vector<double>       scale_factors;	//!< A list of scale factors.
    uint		 nscales; //!< Number of scales if set by hand.
    double               scales_step;
    double               min_scale;//!< Minimum scale as factor of original res.
    double               max_scale;//!< Maximum scale as factor of original res.
    t_scaling            restype; //!< resolution type
    // saving //////////////////////////////////////////////////////////////
    bool                 silent; //!< print results on std output if not silent
    bool                 save_mode; //!< save detected windows or not
    string               save_dir; //!< directory where to save detections
    vector<uint>         save_counts; //!< file counter for each class
    bboxes               raw_bboxes; //!< raw bboxes extracted from outputs
    bboxes               pruned_bboxes; //!< scale-pruned bboxes
    uint                 min_size; //!< minimum input size to network
    uint                 max_size; //!< maximum input size to network
    vector<idx<T> >      odetections; //!< original windows yielding detection
    svector<midx<T> >    ppdetections; //!< preprocessed wins yielding detection
    bool                 bodetections; //!< odetections is up-to-date or not
    bool                 bppdetections; //!< ppdetections is up-to-date or not
    uint                 save_max_per_frame; //!< max number of region saved
    bool                 diverse_ordering; //!< Saved samples diverse ordering.
    bool                 mem_optimization; //!< optimize memory or not.
    bool                 optimization_swap; //!< swap buffers or not.
    bool                 keep_inputs; //! optimize input buffers or not.
    uint                 hzpad; //! Zero-pad on height (each side).
    uint                 wzpad; //! Zero-pad on width (each side).
    // printing ////////////////////////////////////////////////////////////////
    std::ostream         &mout; //! output stream.
    std::ostream         &merr; //! error output stream.
    // smoothing //////////////////////////////////////////////////////////////
    uint                 smoothing_type;
    idx<T>               smoothing_kernel;
    bool                 initialized;
    string               outputs_dump; //!< Outputs dumping name.
    bool                 bboxes_off; //!< Do not extract bboxes if true.
    bool                 adapt_scales; //!< Adapt scales to network structure.
    bool                 scaler_mode;
    answer_module<T,T,T,Tstate> *answer;
    mstate<Tstate>       answers; //!< Buffers holding last answers.
    bool                 ignore_outsiders; //!< Ignore bbs overlapping outside.
    uint corners_inference; //!< 0: from net 1: from net + save 2: load
    bool corners_infered; //!< Allows to infer only once.
    svector<mfidxdim> itl, itr, ibl, ibr; //!< 4 corners in input space.
    svector<mfidxdim> pptl, pptr, ppbl, ppbr; //!< 4 corners in pp input space.
    float    pre_threshold; //!< Threshold for initial bbox extraction.
    vector<float> raw_thresholds; //!< Thresholds for each scale.
    vector<vector<uint> > scale_indices;//!< Input scales indices for each out
    uint bbox_decision; //!< Decision type, 0: regular, 1: corners only
    mfidxdim bbox_scalings;

    // friends /////////////////////////////////////////////////////////////////
    template <typename T2, class Tstate2> friend class detector_gui;
    template <typename T2> friend class detection_thread;
    template <typename T2, class Tstate2> friend class bootstrapping;
  };

} // end namespace ebl

#include "detector.hpp"

#endif /* DETECTOR_H_ */
