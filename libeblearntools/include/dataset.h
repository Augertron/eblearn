/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef DATASET_H_
#define DATASET_H_

#define MKDIR_RIGHTS 0755

#define DATASET_SAVE "dataset"

#include "libidx.h"
#include "libeblearn.h"

typedef int t_label;

namespace ebl {

  //! The dataset class allows to extract a dataset from sample files and
  //! compile all samples into one dataset matrix, formatted for learning.
  template <class Tdata> class dataset {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors/allocation

    //! Initialize the dataset's name and other internal variables, but does
    //! not allocate data matrices, user must call alloc for that effect.
    //! outdims are the target output dimensions of each sample.
    //! inroot is the root directory where to extract data.
    dataset(const char *name, const char *inroot = NULL);

    //! Destructor.
    virtual ~dataset();

    //! Count how many samples are present in files to be compiled,
    //! and allocate buffers to the number of samples.
    //! If max is given (default is 0), then the dataset is limited
    //! to max samples (or counted samples if less).
    bool alloc(intg max = 0);

    ////////////////////////////////////////////////////////////////
    // data manipulation

    //! Extract data from files into dataset.
    virtual bool extract();

    //! Split dataset into datasets ds1 and ds2, by limiting ds1 to max
    //! samples per class, putting anything left into ds2.
    //! Then save each dataset in outroot directory.
    bool split_max_and_save(const char *name1, const char *name2,
			    intg max, const string &outroot);
  
    //! Split dataset into datasets ds1 and ds2, by limiting ds1 to max
    //! samples per class, putting anything left into ds2.
    void split_max(dataset<Tdata> &ds1, dataset<Tdata> &ds2, intg max);

    //! merge datasets with names name1 and name2 found in outroot into
    //! this dataset.
    void merge(const char *name1, const char *name2, const string &outroot);

    //! Shuffle order of samples.
    void shuffle();

    ////////////////////////////////////////////////////////////////
    // data preprocessing

    //! Set type of image conversion with string conv_type.
    //! con_type can accept a number of strings,
    //! including RGB, Y, YUV, HSV, etc.
    void set_pp_conversion(const char *conv_type, uint ppkernel_size = 9);

    ////////////////////////////////////////////////////////////////
    // accessors    

    //! Get sample dimensions
    const idxdim &get_sample_outdim();

    //! Return the number of samples in the dataset.
    intg size();

    //! Return unsigned int label corresponding to class name
    t_label get_label_from_class(const string &class_name);
    
    //! Turn display on or off.
    void set_display(bool display);

    //! Make the display sleep delay milliseconds between frames
    void set_sleepdisplay(uint delay);

    //! Select the method for resizing. default is set to gaussian pyramids.
    //! options are: "gaussian" or "bilinear" for bilinear interpolation.
    void set_resize(const string &resize_mode);

    //! Specify the dimensions of 1 output sample.
    //! The default is: 96x96x3
    void set_outdims(const idxdim &d);

    //! Specify the minimum dimensions of input samples.
    //! The default is: 1x1
    void set_mindims(const idxdim &d);

    //! Setting scale mode and scales: preprocess and save each image
    //! in each scale in outdir directory.
    void set_scales(const vector<double> &sc, const string &od);
    
    //! Set all max per class to max.
    void set_max_per_class(intg max);

    //! Set maximum number of samples.
    void set_max_data(intg max);

    //! Set the pattern used to find image.
    void set_image_pattern(const string &p);

    //! Set the list of classes to exclude (including everything else).
    void set_exclude(const vector<string> &ex);

    //! Set the list of classes to include (excluding everything else).
    void set_include(const vector<string> &inc);

    //! Set saving mode, default is "dataset", saving everything in single
    //! lush matrix format. other values can be "ppm", "jpg", etc.
    void set_save(const string &save);

    //! Set name of dataset. This name will be used by load and save methods.
    //! It is usually set in the constructor, but one can also use this method
    //! instead.
    void set_name(const string &name);

    //! If called, this method will force the dataset to assign this one
    //! label to all collected images.
    void set_label(const string &label);

    //! Multiply bounding boxes by factor. This is useful to move object's
    //! boundaries away from borders when bounding boxes are too tight around
    //! objects.
    void set_bboxfact(float factor);

    //! If true, ignore samples with padded areas, i.e. too small for target 
    //! size.
    void set_nopadded(bool nopadded);

    //! use pose information to separate classes. e.g. if for class "person"
    //! we have "front" and "side" pose, create 2 classes "person_front"
    //! and "person_side" instead of 1 class "person".
    void use_pose();

    //! also extract parts of objects if available.
    //! e.g. in pascal if for class "person", parts "face", "hand" and "foot"
    //! are available.
    void use_parts();

    //! only extract parts of objects if available.
    //! e.g. in pascal if for class "person", parts "face", "hand" and "foot"
    //! are available.
    void use_parts_only();

    //! Dataset has reached maximum sample capacity (this can be controlled
    //! by setting max_data variable).
    //! A label can be optionally passed to return if the dataset is full for
    //! a particular class (this is relevant only if set_max_per_class has been
    //! previously set.
    bool full(t_label label = -1);
    
    ////////////////////////////////////////////////////////////////
    // I/O

    //! Load dataset found in root. root should be including the name of the
    //! dataset, e.g.: "/data/dataset1" is going to load dataset1_*.mat
    bool load(const string &root);

    //! Save dataset in root
    bool save(const string &root);

    ////////////////////////////////////////////////////////////////
    // print methods

    //! Prints number of classes and list on standard output
    void print_classes();

    //! Prints number of samples for each class on std output
    void print_stats();
    
  protected:
    
    ////////////////////////////////////////////////////////////////
    // allocation
    
    //! Allocate dataset with n samples, each with dimensions d.
    //! This method is not called by the constructor and must be called
    //! before adding data samples.
    //! The buffers can be allocated only once, this is not meant to
    //! be used as a buffer resize method.
    bool allocate(intg n, idxdim &d);

    //! Allocate preprocessing and resizing modules.
    void init_preprocessing();

    ////////////////////////////////////////////////////////////////
    // data manipulation

    //! add sample d to the data with label class_name
    //! (and converting from Toriginal to Tdata type).
    //! r is an optional region of interest rectangle in the image d.
    virtual bool add_data(idx<Tdata> &d, const t_label label,
			  const string *class_name,
			  const char *filename = NULL, const rect *r = NULL);

    //! add a class name
    bool add_class(const string &class_name);

    //! set all classes directly using an idx of classes strings
    void set_classes(idx<ubyte> &classidx);

    //! count how many samples are present in dataset files to be compiled.
    virtual bool count_samples();

    //! split current dataset into ds1 and ds2, using their max_per_class
    //! array (first filling ds1 until full, then ds2, therefore assuming
    //! that maximums are lower for ds1 than ds2).
    void split(dataset<Tdata> &ds1, dataset<Tdata> &ds2);

    template <class Toriginal>
      bool save_scales(idx<Toriginal> &d, const string &filename);

    //! return true if class_name is authorized (based on excluded and included
    //! variables).
    virtual bool included(const string &class_name);

    ////////////////////////////////////////////////////////////////
    // data preprocessing

    //! Preprocess data d of type Toriginal into an idx of type Tdata
    //! with output dimensions outdims and return the result.
    //! The type of preprocessing can be selected using set_pp_conversion().
    //! @param outr If not null, copy the rect of the input region in the
    //!        output image.
    idx<Tdata> preprocess_data(idx<Tdata> &d, const string *class_name,
			       bool squared = true, const char *filename = NULL,
			       const rect *r = NULL, double scale = 0,
			       bool active_sleepd = true, rect *outr = NULL);

    ////////////////////////////////////////////////////////////////
    // Helper functions

    //! Returns a matrix of class names based on the classes vector
    idx<ubyte> build_classes_idx();

    //! Return the class name associated with label id.
    string& get_class_string(t_label id);

    //! Return the label id associated with class name
    t_label get_class_id(const string &name);
    
    //! Compute statistics about the dataset
    void compute_stats();

    //! Count (recursively) how many files matching pattern are inside dir.
    uint count_matches(const string &dir, const string &pattern);

    //! Recursively add all files matching the pattern ext to the dataset
    //! with class class_name.
    void process_dir(const string &dir, const string &ext,
		     const string &class_name);
    
    //! Method to load an image.
    virtual void load_data(const string &fname);
    
  protected:
    // data ////////////////////////////////////////////////////////
    idx<Tdata>		data;	//!< data matrix
    idx<t_label>	labels;	//!< labels matrix
    vector<string>	classes;	//!< list of classes strings
    idx<t_label>        classpairs;	//!< sample pairs class-wise
    idx<t_label>        deformpairs;	//!< sample pairs deformation-wise
    // data helpers ////////////////////////////////////////////////
    uint                height;         //!< height of output
    uint                width;          //!< width of output
    bool		allocated;	//!< data matrices allocated or not
    idxdim		outdims;	//!< dims of sample out dimensions
    idxdim		mindims;	//!< min dims of input samples
    idxdim		datadims;	//!< dimensions of data out dimensions
    intg		data_cnt;	//!< number of samples added so far
    intg		max_data;	//!< user can limit samples# with this
    bool		max_data_set;	//!< max_data been set by user or not
    intg                total_samples;	//!< number of samples of dataset
    idx<intg>           max_per_class;	//!< max # samples per class
    intg                mpc;            //!< value to put in max_per_class
    bool                max_per_class_set;	//!< mpc has been set or not
    idx<Tdata>          load_img;       //!< temporary image loader
    bool                scale_mode;     //!< scales saving mode
    vector<double>      scales;         //!< integer scales
    bool                interleaved_input; //!< indicate input is interleaved
    vector<string>      exclude;        //!< list of excluded classes
    vector<string>      include;        //!< list of included classes
    bool                usepose;        //!< use pose or not
    bool                useparts;       //!< use parts or not
    bool                usepartsonly;   //!< use parts only or not
    string              save_mode;      //!< saving mode (dataset, ppm, png..)
    float               bboxfact;       //!< bounding boxes factor
    string              force_label;    //!< force all labels to this one
    bool                nopadded;       //!< ignore too small samples
    // names ///////////////////////////////////////////////////////
    string		name;	        //!< dataset name
    string		data_fname;	//!< data filename
    string		labels_fname;	//!< labels filename
    string		classes_fname;	//!< classes filename
    string		classpairs_fname;	//!< classpairs filename
    string		deformpairs_fname;	//!< deformpairs filename
    // directories /////////////////////////////////////////////////
    string		inroot;         //!< root directory of input files
    string		outdir;         //!< root directory of output files
    string              extension;	//!< extension of files to extract
    // display /////////////////////////////////////////////////////
    bool		display_extraction;	//!< display during extraction
    bool		display_result;	//!< display extracted dataset
    Tdata               minval;         //!< minimum value to display
    Tdata               maxval;         //!< minimum value to display
    bool                sleep_display;	//!< enable sleeping when displaying
    uint                sleep_delay;	//!< display sleep delay in ms
    // stats ///////////////////////////////////////////////////////
    uint                nclasses;       //!< Number of classes.
    idx<intg>           class_tally;	//!< counter for class tally
    idx<intg>		add_tally;	//!< counter for additions tally
    uint                add_errors;     //!< Number of adding failures.
    // preprocessing ///////////////////////////////////////////////
    string		ppconv_type;	//!< name of image conversion
    uint		ppkernel_size;	//!< size of kernel for pp
    bool		ppconv_set;	//!< ppconv_type has been set or not
    bool		do_preprocessing;	//!< activate or deactivate pp
    string              resize_mode;    //!< type of resizing (bilin, gaussian)
    module_1_1<fs(Tdata)>  *ppmodule;       //!< pp module 
    resizepp_module<fs(Tdata)> *resizepp;   //!< pp resizing module
    rect                original_bbox;  //!< bbox of image in resized image
  };
  
  ////////////////////////////////////////////////////////////////
  // Helper functions
  
  //! Builds dataset filenames based on root, name, etc.
  void build_fname(string &ds_name, const char *fname, string &fullname);

  //! Recursively goes through dir, looking for files matching extension ext,
  //! and returns the number of matches.
  uint count_matches(const string &dir, const string &pattern);

  ////////////////////////////////////////////////////////////////
  // loading errors
  
  //! required datasets, throw error if bool is false, otherwise print success.
  //! return success.
  template <typename T>
    bool loading_error(idx<T> &mat, string &fname);

  //! optional datasets, issue warning if bool is false, otherwise print
  //! success. return succcess.
  template <typename T>
    bool loading_warning(idx<T> &mat, string &fname);

} // end namespace ebl

#include "dataset.hpp"

#endif /* DATASET_H_ */
