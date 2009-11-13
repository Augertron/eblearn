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

#include "libidx.h"

#define DATA_NAME "data"
#define LABELS_NAME "labels"
#define CLASSES_NAME "classes"
#define CLASSPAIRS_NAME "classpairs"
#define DEFORMPAIRS_NAME "deformpairs"

#define MKDIR_RIGHTS 0755
#define IMAGE_PATTERN ".*[.](png|jpg|jpeg|PNG|JPG|JPEG|bmp|BMP)"

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

    //! Shuffle order of samples.
    void shuffle();

    ////////////////////////////////////////////////////////////////
    // data preprocessing

    //! Set type of image conversion with string conv_type.
    //! con_type can accept a number of strings,
    //! including RGB, Y, YUV, HSV, etc.
    void set_pp_conversion(const char *conv_type);

    ////////////////////////////////////////////////////////////////
    // accessors    

    //! Get sample dimensions
    const idxdim &get_sample_outdim();

    //! Return unsigned int label corresponding to class name
    t_label get_label_from_class(const string &class_name);
    
    //! Turn display on or off.
    void set_display(bool display);

    //! Make the display sleep delay milliseconds between frames
    void set_sleepdisplay(uint delay);

    //! Specify the dimensions of 1 output sample.
    //! The default is: 96x96x3
    void set_outdims(const idxdim &d);

    //! set all max per class to max.
    void set_max_per_class(intg max);

    //! Dataset has reached maximum sample capacity (this can be controlled
    //! by setting max_data variable).
    bool full();
    
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

    //! Allocate dataset using the max_per_class array.
    //! For safety, it is also bounded by max.
    //! The buffers can be allocated only once, this is not meant to
    //! be used as a buffer resize method.
    bool alloc_from_max_per_class(intg max);
    
    ////////////////////////////////////////////////////////////////
    // data manipulation

    //! add sample d to the data with label class_name
    //! (and converting from Toriginal to Tdata type).
    //! r is an optional region of interest rectangle in the image d.
    template <class Toriginal>
      bool add_data(idx<Toriginal> &d, const string &class_name,
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

    ////////////////////////////////////////////////////////////////
    // data preprocessing

    //! Preprocess data d of type Toriginal into an idx of type Tdata
    //! with output dimensions outdims and return the result.
    //! The type of preprocessing can be selected using set_pp_conversion().
    idx<Tdata> preprocess_data(idx<Tdata> &d, const string &class_name,
			       const char *filename = NULL,
			       const rect *r = NULL);

    //! Convert input image img to format of type conv_type and return the
    //! result. con_type can accept a number of strings,
    //! including RGB, Y, YUV, HSV, etc.
    idx<Tdata> convert_image_to(idx<Tdata> &img, const string &conv_type,
				const rect &cropped);

    //! Resize image img to dimensions d and returns the result.
    virtual idx<Tdata> resize_image_to(idx<Tdata> &img, const idxdim &d,
				       rect &cropped, const rect *r = NULL);
    
    ////////////////////////////////////////////////////////////////
    // Helper functions

    //! Returns a matrix of class names based on the classes vector
    idx<ubyte> build_classes_idx();

    //! Compute statistics about the dataset
    void compute_stats();

    //! Count (recursively) how many files matching pattern are inside dir.
    uint count_matches(const string &dir, const string &pattern);

    //! Recursively add all files matching the pattern ext to the dataset
    //! with class class_name.
    void process_dir(const string &dir, const string &ext,
		     const string &class_name);
    
  protected:
    // data ////////////////////////////////////////////////////////
    idx<Tdata>		data;	//!< data matrix
    idx<t_label>	labels;	//!< labels matrix
    vector<string>	classes;	//!< list of classes strings
    idx<t_label>        classpairs;	//!< sample pairs class-wise
    idx<t_label>        deformpairs;	//!< sample pairs deformation-wise
    // data helpers ////////////////////////////////////////////////
    bool		allocated;	//!< data matrices allocated or not
    idxdim		outdims;	//!< dimensions of sample out dimensions
    intg		data_cnt;	//!< number of samples added so far
    intg		max_data;	//!< user can limit samples# with this
    bool		max_data_set;	//!< max_data been set by user or not
    intg                total_samples;	//!< number of samples of dataset
    idx<intg>           max_per_class;	//!< max # samples per class
    bool                max_per_class_set;	//!< mpc has been set or not
    // names ///////////////////////////////////////////////////////
    string		name;	//!< dataset name
    string		data_fname;	//!< data filename
    string		labels_fname;	//!< labels filename
    string		classes_fname;	//!< classes filename
    string		classpairs_fname;	//!< classpairs filename
    string		deformpairs_fname;	//!< deformpairs filename
    // directories /////////////////////////////////////////////////
    string		inroot; //!< root directory of input files
    string              extension;	//!< extension of files to extract
    // display /////////////////////////////////////////////////////
    bool		display_extraction;	//!< display during extraction
    bool		display_result;	//!< display extracted dataset
    Tdata               minval; //!< minimum value to display
    Tdata               maxval; //!< minimum value to display
    bool                sleep_display;	//!< enable sleeping when displaying
    uint                sleep_delay;	//!< display sleep delay in ms
    // stats ///////////////////////////////////////////////////////
    idx<intg>           class_tally;	//!< counter for class tally
    idx<intg>		add_tally;	//!< counter for additions tally
    // preprocessing ///////////////////////////////////////////////
    string		ppconv_type;	//!< name of image conversion
    bool		ppconv_set;	//!< ppconv_type has been set or not
    bool		do_preprocessing;	//!< activate or deactivate pp
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
  bool loading_error(bool success, string &fname);

  //! optional datasets, issue warning if bool is false, otherwise print
  //! success. return succcess.
  bool loading_warning(bool success, string &fname);

} // end namespace ebl

#include "dataset.hpp"

#endif /* DATASET_H_ */
