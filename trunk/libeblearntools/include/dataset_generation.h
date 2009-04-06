/***************************************************************************
 *   Copyright (C) 2008 by Matt Grimes and Pierre Sermanet   *
 *   mkg@cs.nyu.edu, pierre.sermanet@gmail.com   *
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

#ifndef DATASET_GENERATION_H_
#define DATASET_GENERATION_H_

#include "libidx.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // Utility functions to prepare a dataset from raw images.

  const char defaultExtensionPattern[] =  ".*[.]ppm";
  //! Finds all images corresponding to the imgPatternLeft pattern in directory 
  //! imgDir, assigns for each a class name corresponding to the first directory
  //! level found, crops to square and resizes all images to width*width size.
  //! If imgPatternRight is not null, two images are stored for each example, 
  //! bringing the stereo dimension sdim from 1 to 2.
  //! Finally outputs all N examples found in a single matrix files as follow:
  //! outDir/dset_images.mat: 
  //!   (ubyte) N x width x width x 6 (G or GG, RGB or RGBRGB)
  //! outDir/dset_labels.mat: 	(int)   N
  //! outDir/dset_classes.mat:  (ubyte) Nclasses x 128
  bool imagedir_to_idx(const char *imgDir, 
		       unsigned int width,
		       //eg: ".*[.]ppm"
		       const char *imgExtension = defaultExtensionPattern,
		       const char *imgPatternLeft = NULL, // eg: "_L"
		       const char *outDir = NULL, 
		       const char *imgPatternRight = NULL,  // eg: "_R"
		       bool verbose = false,
		       const char *prefix = NULL, // eg: "_train" or "test_"
		       bool toYUV = false);  // convert images to YUV if true

} // end namespace ebl

#endif /* DATASET_GENERATION_H_ */
