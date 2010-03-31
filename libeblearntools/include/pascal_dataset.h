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

#ifndef PASCAL_DATASET_H_
#define PASCAL_DATASET_H_

#include "dataset.h"
#include "xml_utils.h"

#define XML_PATTERN ".*[.]xml"

namespace ebl {

  //! The pascal_dataset class allows to extract a dataset of type PASCAL
  //! from sample files and
  //! compile all samples into one dataset matrix, formatted for learning.
  //! It derives from the dataset class, reimplementing only PASCAL specifics,
  //! such as reading xml files, etc.
  template <class Tdata> class pascal_dataset : public dataset<Tdata> {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors

    //! Initialize the dataset's name and other internal variables, but does
    //! not allocate data matrices, user must call alloc for that effect.
    //! outdims are the target output dimensions of each sample.
    //! inroot is the root directory from which we extract data.
    //! ignore_diff ignores difficult objects if true.
    pascal_dataset(const char *name, const char *inroot = NULL,
		   bool ignore_diff = true, bool ignore_trunc = false,
		   bool ignore_occluded = false);

    //! Destructor.
    virtual ~pascal_dataset();

    ////////////////////////////////////////////////////////////////
    // data

    //! Extract data from files into dataset.
    virtual bool extract();

  protected:

#ifdef __XML__ // disable some derived methods if XML not available
    
    //! return true if class_name is authorized (based on excluded and included
    //! variables, and difficult, truncated, occluded flags).
    virtual bool included(const string &class_name, uint difficult,
			  uint truncated, uint occluded);
    
    ////////////////////////////////////////////////////////////////
    // data
    
    //! count how many samples are present in dataset files to be compiled.
    virtual bool count_samples();

    //! count sample or not given an xml node. This will update the total_sample
    //! and total_difficult counters.
    virtual void count_sample(Node::NodeList &olist);
    
    ////////////////////////////////////////////////////////////////
    // internal methods

    //! process an xml file.
    virtual bool process_xml(const string &xmlfile);

    //! process one object from an xml file.
    virtual void process_object(Node* onode, idx<Tdata> &img, uint &h0,
				uint &w0, uint obj_number,
				const string &image_filename);

    //! process image for one object.
    virtual void process_image(idx<Tdata> &img, uint &h0, uint &w0,
			       uint xmin, uint ymin,
			       uint xmax, uint ymax, uint sizex, uint sizey,
			       string &obj_class, uint obj_number,
			       uint difficult, const string &image_filename);
    
#endif /* __XML__ */
    
  protected:
    // "difficult" samples /////////////////////////////////////////
    bool	ignore_difficult;	//!< ignore difficult or not
    intg	total_difficult;	//!< total number of difficult samples
    bool	ignore_truncated;	//!< ignore truncated or not
    intg	total_truncated;	//!< total number of truncated samples
    bool	ignore_occluded;	//!< ignore occluded or not
    intg	total_occluded;	        //!< total number of occluded samples
    intg        total_ignored;          //!< total ignore any catergory
    // directories /////////////////////////////////////////////////
    string	annroot;	//!< directory of annotation xml files
    string	imgroot;	//!< directory of images
    // base class members to be used ///////////////////////////////
    using dataset<Tdata>::usepose;
    using dataset<Tdata>::useparts;
    using dataset<Tdata>::usepartsonly;
    using dataset<Tdata>::allocated;
    using dataset<Tdata>::total_samples;
    using dataset<Tdata>::inroot;
    using dataset<Tdata>::display_extraction;
    using dataset<Tdata>::display_result;
    using dataset<Tdata>::outdims;
    using dataset<Tdata>::full;
    using dataset<Tdata>::add_data;
    using dataset<Tdata>::print_stats;
    using dataset<Tdata>::data_cnt;
    using dataset<Tdata>::extension;
    using dataset<Tdata>::ppconv_type;
    using dataset<Tdata>::scale_mode;
    using dataset<Tdata>::exclude;
    using dataset<Tdata>::include;
  };

} // end namespace ebl

#include "pascal_dataset.hpp"

#endif /* PASCAL_DATASET_H_ */
