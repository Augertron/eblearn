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

#ifndef PASCALPART_DATASET_H_
#define PASCALPART_DATASET_H_

#include "pascal_dataset.h"

namespace ebl {

  //! The pascalpart_dataset class allows to extract a dataset of type PASCAL
  //! but extracting only the "part" images, i.e. head, hand and foot for the
  //! person class.
  //! It compiles all samples into one dataset matrix, formatted for learning.
  //! It derives from the pascal_dataset class, reimplementing only specific
  //! parts such as reading xml files, etc.
  template <class Tdata> class pascalpart_dataset
    : public pascal_dataset<Tdata> {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors

    //! Initialize the dataset's name and other internal variables, but does
    //! not allocate data matrices, user must call alloc for that effect.
    //! outdims are the target output dimensions of each sample.
    //! inroot is the root directory from which we extract data.
    //! ignore_diff ignores difficult objects if true.
    pascalpart_dataset(const char *name, const char *inroot = NULL,
		       bool ignore_diff = true);

    //! Destructor.
    virtual ~pascalpart_dataset();

  protected:
    
#ifdef __XML__ // disable some derived methods if XML not available
    
    ////////////////////////////////////////////////////////////////
    // data
    
    //! count sample or not given an xml node. This will update the total_sample
    //! and total_difficult counters.
    void count_sample(Node::NodeList &olist);    

    ////////////////////////////////////////////////////////////////
    // internal methods

    //! process one object from an xml file.
    virtual void process_object(Node* onode, idx<ubyte> &img, uint &h0,
				uint &w0,
				uint obj_number, const string &image_filename);
        
#endif /* __XML__ */
    
  protected:
    // "difficult" samples /////////////////////////////////////////
    bool pascal_dataset<Tdata>::ignore_difficult;//!< ignore difficult or not
    intg pascal_dataset<Tdata>::total_difficult;//!< total number of difficult
    // directories /////////////////////////////////////////////////
    string pascal_dataset<Tdata>::annroot;//!< directory of annotation xml files
    string pascal_dataset<Tdata>::imgroot;//!< directory of images
    // base class members to be used ///////////////////////////////
    using dataset<Tdata>::usepose;
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
  };

} // end namespace ebl

#include "pascalpart_dataset.hpp"

#endif /* PASCALPART_DATASET_H_ */
