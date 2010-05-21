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

#ifndef PASCALBG_DATASET_HPP_
#define PASCALBG_DATASET_HPP_

#include <algorithm>

#include "xml_utils.h"

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <class Tdata>
  pascalbg_dataset<Tdata>::pascalbg_dataset(const char *name_,
					    const char *inroot_,
					    const char *outdir_,
					    uint max_folders_,
					    bool ignore_diff, bool ignore_trunc,
					    bool ignore_occl)
    : pascal_dataset<Tdata>(name_, inroot_, ignore_diff, ignore_trunc,
			    ignore_occl) {
    outdir = outdir_;
    cout << "output directory: " << outdir << endl;
    max_folders = max_folders_;
    data_cnt = 0;
    save_mode = "mat";
  }

  template <class Tdata>
  pascalbg_dataset<Tdata>::~pascalbg_dataset() {
  }

  ////////////////////////////////////////////////////////////////
  // data extraction

  template <class Tdata>
  bool pascalbg_dataset<Tdata>::extract() {
    this->init_preprocessing();
#ifdef __BOOST__    
#ifdef __XML__    
    cout << "Extracting samples from files into dataset..." << endl;
    // adding data to dataset using all xml files in annroot
    regex eExt(XML_PATTERN);
    cmatch what;
    path p(annroot);
    if (!exists(p)) {
      cerr << "path " << annroot << " does not exist." << endl;
      return false;
    }    
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (!is_directory(itr->status()) &&
	  regex_match(itr->leaf().c_str(), what, eExt)) {
	process_xml(itr->path().string());
	if (this->full()) //max_data_set && (data_cnt >= max_data))
	  break ;
      }
    }
    cout << "Extracted and saved " << data_cnt;
    cout << " background patches from dataset." << endl;
#endif /* __XML__ */
#endif /* __BOOSt__ */
    return true;
  }

#ifdef __BOOST__ // disable some derived methods if BOOST not available
#ifdef __XML__ // disable some derived methods if XML not available

  ////////////////////////////////////////////////////////////////
  // process xml

  // Note: the difficult flag is ignored, so that we don't take
  // background patches even in difficult bounding boxes.
  template <class Tdata>
  bool pascalbg_dataset<Tdata>::process_xml(const string &xmlfile) {
    string image_filename, image_fullname;
    vector<rect> bboxes;
    string obj_classname, pose;
    bool pose_found = false;
    Node::NodeList::iterator oiter;
      
    // parse xml file
    try {
      DomParser parser;
      //    parser.set_validate();
      parser.parse_file(xmlfile);
      if (parser) {
	// initialize root node and list
	const Node* pNode = parser.get_document()->get_root_node();
	Node::NodeList list = pNode->get_children();
	// get image filename
	for(Node::NodeList::iterator iter = list.begin();
	    iter != list.end(); ++iter) {
	  if (!strcmp((*iter)->get_name().c_str(), "filename")) {
	    xml_get_string(*iter, image_filename);
	    iter = list.end(); iter--; // stop loop
	  }
	}
	image_fullname = imgroot;
	image_fullname += image_filename;
	// parse all objects in image
	for(Node::NodeList::iterator iter = list.begin();
	    iter != list.end(); ++iter) {
	  if (!strcmp((*iter)->get_name().c_str(), "object")) {
	    // get object's properties
	    Node::NodeList olist = (*iter)->get_children();
	    for(oiter = olist.begin(); oiter != olist.end(); ++oiter) {
	      if (!strcmp((*oiter)->get_name().c_str(), "name"))
		xml_get_string(*oiter, obj_classname);
	      else if (!strcmp((*oiter)->get_name().c_str(), "pose")) {
		xml_get_string(*oiter, pose);
		pose_found = true;
	      }
	    }
	    // add object's bbox
	    if (!usepartsonly) {
	      // add object's class to dataset
	      if (usepose && pose_found) { // append pose to class name
		obj_classname += "_";
		obj_classname += pose;
	      }
	      if (dataset<Tdata>::included(obj_classname)) {
		bboxes.push_back(get_object(*iter));
	      }
	    }
	    ////////////////////////////////////////////////////////////////
	    // parts
	    if (useparts || usepartsonly) {
	      string part_classname;
      
	      // add part's class to dataset
	      for(oiter = olist.begin();oiter != olist.end(); ++oiter) {
		if (!strcmp((*oiter)->get_name().c_str(), "part")) {
		  // get part's name
		  Node::NodeList plist = (*oiter)->get_children();
		  for(Node::NodeList::iterator piter = plist.begin();
		      piter != plist.end(); ++piter) {
		    if (!strcmp((*piter)->get_name().c_str(), "name")) {
		      xml_get_string(*piter, part_classname);
		      // found a part and its name, add it
		      if (usepose && pose_found) {
			part_classname += "_";
			part_classname += pose;
		      }
		      if (dataset<Tdata>::included(part_classname)) {
			bboxes.push_back(get_object(*oiter));
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    } catch (const std::exception& ex) {
      cerr << "error: Xml exception caught: " << ex.what() << endl;
      return false;
    } catch (const char *err) {
      cerr << "error: " << err << endl;
      return false;
    }
    // load image 
    idx<ubyte> img = load_image<ubyte>(image_fullname);
    // extract patches given image and bounding boxes
    process_image(img, bboxes, image_filename);
    return true;
  }
  
  ////////////////////////////////////////////////////////////////
  // process 1 object of an xml file

  template <class Tdata>
  rect pascalbg_dataset<Tdata>::get_object(Node* onode) {
    unsigned int xmin = 0, ymin = 0, xmax = 0, ymax = 0;
    
    // parse object node
    Node::NodeList list = onode->get_children();
    for(Node::NodeList::iterator iter = list.begin();
	iter != list.end(); ++iter) {
      // parse bounding box
      if (!strcmp((*iter)->get_name().c_str(), "bndbox")) {
	Node::NodeList blist = (*iter)->get_children();
	for(Node::NodeList::iterator biter = blist.begin();
	    biter != blist.end(); ++biter) {
	  // save xmin, ymin, xmax and ymax
	  if (!strcmp((*biter)->get_name().c_str(), "xmin"))
	    xmin = xml_get_uint(*biter);
	  else if (!strcmp((*biter)->get_name().c_str(), "ymin"))
	    ymin = xml_get_uint(*biter);
	  else if (!strcmp((*biter)->get_name().c_str(), "xmax"))
	    xmax = xml_get_uint(*biter);
	  else if (!strcmp((*biter)->get_name().c_str(), "ymax"))
	    ymax = xml_get_uint(*biter);
	}
      } // else get object class name
    }
    rect r(ymin, xmin, ymax - ymin, xmax - xmin);
    return r;
  }
  
  ////////////////////////////////////////////////////////////////
  // process object's image

  template <class Tdata>
  void pascalbg_dataset<Tdata>::
  process_image(idx<ubyte> &img, vector<rect>& bboxes,
		const string &image_filename) {
    vector<rect> scaled_bboxes;
    vector<rect> patch_bboxes;
    vector<rect>::iterator ibb;
    idxdim d(img);
    idx<Tdata> im(d);
    string cname = "background";
    ostringstream fname;
    vector<idx<Tdata> > patches;
    bool overlap;
    
    // for each scale, find patches and save them
    for (vector<double>::iterator i = scales.begin(); i != scales.end(); ++i) {
      patches.clear();
      patch_bboxes.clear();
      scaled_bboxes.clear(); 
      idx_copy(img, im); // initialize im to original image
      // rescale original bboxes
      double ratio = MAX(im.dim(0) / (double) outdims.dim(0),
			 im.dim(1) / (double) outdims.dim(1)) / *i;
      // do not upsample to avoid creating artefacts
      // if (ratio < 1)
      // 	continue ; // do nothing for this scale
      // rescale original bboxes
      for (ibb = bboxes.begin(); ibb != bboxes.end(); ++ibb) {
	rect rs = *ibb / ratio;
	if (im.dim(0) < im.dim(1))
	  rs.h0 += (uint) (((outdims.dim(0) * *i) - (im.dim(0) / ratio)) / 2);
	else 
	  rs.w0 += (uint) (((outdims.dim(1) * *i) - (im.dim(1) / ratio)) / 2);
	scaled_bboxes.push_back(rs);
      }
      // preprocess image
      rect r(0, 0, im.dim(0), im.dim(1));
      idx<Tdata> im2 =
	this->preprocess_data(im, &cname, false, image_filename.c_str(),
			      &r, *i, false);
      // extract all non overlapping patches with dimensions outdims that
      // do not overlap with bounding boxes
      rect patch(0, 0, outdims.dim(0), outdims.dim(1));
      for (patch.h0 = 0; patch.h0 + patch.height <= (uint) im2.dim(0);
	   patch.h0 += patch.height) {
	for (patch.w0 = 0; patch.w0 + patch.width <= (uint) im2.dim(1);
	     patch.w0 += patch.width) {
	  // test if patch overlaps with any bounding box or is outside of image
	  overlap = false;
	  for (ibb = scaled_bboxes.begin(); ibb != scaled_bboxes.end(); ++ibb) {
	    if (patch.overlap(*ibb)) {
	      overlap = true;
	      break ;
	    }
	  }
	  if (!patch.is_within(original_bbox))
	    overlap = true;
	  if (!overlap) {
	    // not overlapping, add patch
	    idx<Tdata> p = im2.narrow(0, patch.height, patch.h0);
	    p = p.narrow(1, patch.width, patch.w0);
	    patches.push_back(p);
	    patch_bboxes.push_back(patch);
	  }
	}
      }
#ifdef __GUI__
      if (display_extraction) {
	uint h = 63, w = 0;
	disable_window_updates();
	// draw bboxes
	for (ibb = scaled_bboxes.begin(); ibb != scaled_bboxes.end(); ++ibb)
	  draw_box(h + ibb->h0, w + ibb->w0,
		   ibb->height, ibb->width, 255, 0, 0);
	// draw patches
	for (ibb = patch_bboxes.begin(); ibb != patch_bboxes.end(); ++ibb)
	  draw_box(h + ibb->h0, w + ibb->w0,
		   ibb->height, ibb->width, 0, 255, 0);
	// draw original bbox
	draw_box(h + original_bbox.h0, w + original_bbox.w0,
		 original_bbox.height, original_bbox.width, 0, 0, 255);
// 	// draw original image
// 	h = im2.dim(0) + 5, w = 0;
// 	idx<Tdata> tmp = im2.select(2, 0);
// 	draw_matrix(tmp, h, w, 1.0, 1.0, (Tdata) -1, (Tdata) 1);
// 	// draw bboxes on original
// 	for (ibb = scaled_bboxes.begin(); ibb != scaled_bboxes.end(); ++ibb)
// 	  draw_box(h + ibb->h0, w + ibb->w0,
// 		   ibb->height, ibb->width, 255, 0, 0);
	enable_window_updates();
	if (sleep_display)
	  usleep((uint) (sleep_delay * 1000.0));
      }
#endif
      fname.str("");
      fname << image_filename << "_scale" << *i;
      save_patches(patches, outdir, max_folders, fname.str());
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // save patches

  template <class Tdata>
  void pascalbg_dataset<Tdata>::save_patches(vector<idx<Tdata> > &patches,
					     const string &outdir,
					     uint max_folders,
					     const string &filename) {
    ostringstream folder, fname;
    try {
      mkdir(outdir.c_str(), MKDIR_RIGHTS);
      uint i;
      // shuffle randomly vector of patches to avoid taking top left corner
      // as first patch every time
      random_shuffle(patches.begin(), patches.end());
      // loop on patches
      for (i = 0; (i < patches.size()) && (i < max_folders); ++i) {
	// create folder if doesn't exist
	folder.str("");
	folder << outdir << "/" << "bg" << i+1 << "/";
	mkdir(folder.str().c_str(), MKDIR_RIGHTS);
	folder << "/background/";
	mkdir(folder.str().c_str(), MKDIR_RIGHTS);
	// save patch in folder
	// switch saving behavior
	fname.str("");
	fname << folder.str() << filename << ".bg" << i+1;
	if (!strcmp(save_mode.c_str(), "mat")) { // lush matrix mode
	  fname << MATRIX_EXTENSION;
	  if (!save_matrix(patches[i], fname.str()))
	    throw fname.str();
	} else { // image file mode
	  fname << "." << save_mode;
	  idx<Tdata> tmp = patches[i];
	  // scale image to 0 255 if preprocessed
	  if (strcmp(ppconv_type.c_str(), "RGB")) {
	    idx_addc(tmp, (Tdata) 1.0, tmp);
	    idx_dotc(tmp, (Tdata) 127.5, tmp);
	  }
	  save_image(fname.str(), tmp, save_mode.c_str());
	}
	cout << data_cnt++ << ": saved " << fname.str().c_str() << endl;
      }
//       if (i < patches.size()) // reached max_folders, fill-up last one
// 	for ( ; i < patches.size(); ++i) {
// 	  // save patch in folder
// 	  fname.str("");
// 	  fname << folder.str() << filename << ".bg" << i+1 << ".mat";
// 	  if (!save_matrix(patches[i], fname.str()))
// 	    throw fname.str();
// 	  cout << data_cnt++ << ": saved " << fname.str().c_str() << endl;
// 	}
    } catch (const string &err) {
      cerr << "error: failed to save patch in " << err << endl;
    }
  }

#endif /* __XML__ */
#endif /* __BOOST__ */

} // end namespace ebl

#endif /* PASCALBG_DATASET_HPP_ */
