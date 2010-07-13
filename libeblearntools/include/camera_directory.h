/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
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

#ifndef CAMERA_DIRECTORY_H_
#define CAMERA_DIRECTORY_H_

#include "camera.h"
#include "tools_utils.h"

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

using namespace std;

namespace ebl {

  //! The camera_directory class interfaces with images found (recursively)
  //! in a directory, grabbing all images into idx format, and also
  //! to save gui outputs into video files.
  template <typename Tdata> class camera_directory : public camera<Tdata> {
  public:

    ////////////////////////////////////////////////////////////////
    // constructors/allocation

    //! Initialize a directory camera from a root directory.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param directory The root directory to recursively search.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    camera_directory(const char *directory, int height_ = -1, int width_ = -1);

    //! Initialize a directory camera without a root directory. This constructor
    //! requires a subsequent call to read_directory to initialize images.
    //! height and width are optional parameters that resize the input image
    //! to those dimensions if given (different than -1). One may want to
    //! decrease the input resolution first to speed up operations, for example
    //! when computing multiple resolutions.
    //! \param height Resize input frame to this height if different than -1.
    //! \param width Resize input frame to this width if different than -1.
    camera_directory(int height_ = -1, int width_ = -1);

    //! Find all images recursively from this directory.
    bool read_directory(const char *directory);
    
    //! Destructor.
    virtual ~camera_directory();

    ////////////////////////////////////////////////////////////////
    // frame grabbing

    //! Return a new frame.
    virtual idx<Tdata> grab();

    //! Return true until all images have been processed.
    virtual bool empty();

    //! Return a name for current frame.
    virtual string frame_name();

    // members ////////////////////////////////////////////////////////
  protected:
    using camera<Tdata>::frame;	//!< frame buffer 
    using camera<Tdata>::frame_id;	//!< frame counter
    using camera<Tdata>::frame_name_;	//!< frame name
    files_list		*fl;	//!< list of images
    string		 indir; //!< input directory name
    string		 fdir;	//!< directory name
    string		 fname;	//!< file name
    ostringstream	 oss;	//!< temporary string
    uint                 flsize;	//!< original size of list
  };

} // end namespace ebl

#include "camera_directory.hpp"


#endif /* CAMERA_DIRECTORY_H_ */
