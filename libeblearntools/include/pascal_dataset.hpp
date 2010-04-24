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

#ifndef PASCAL_DATASET_HPP_
#define PASCAL_DATASET_HPP_

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
  pascal_dataset<Tdata>::pascal_dataset(const char *name_, const char *inroot_,
					bool ignore_diff, bool ignore_trunc,
					bool ignore_occl)
    : dataset<Tdata>(name_, inroot_) {
    // initialize pascal-specific members
    if (inroot_) {
      annroot = inroot;
      annroot += "/Annotations/"; // look for xml files in annotations
      imgroot = inroot;
      imgroot += "JPEGImages/"; // image directory
    }
    ignore_difficult = ignore_diff;
    ignore_truncated = ignore_trunc;
    ignore_occluded = ignore_occl;
#ifndef __XML__ // return error if xml not enabled
    eblerror("XML libraries not available, install libxml++ and recompile");
#endif /* __XML__ */
    extension = XML_PATTERN;
    cout << "Image search extension pattern: " << extension << endl;
  }

  template <class Tdata>
  pascal_dataset<Tdata>::~pascal_dataset() {
  }

  ////////////////////////////////////////////////////////////////
  // data extraction

  template <class Tdata>
  bool pascal_dataset<Tdata>::extract() {
    this->init_preprocessing();
#ifdef __XML__    
    if (!allocated)
      return false;
    cout << "Extracting samples from PASCAL files into dataset..." << endl;
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
	this->process_xml(itr->path().string());
	if (full())
	  break;
      }
    }
    cout << "Extracted " << data_cnt << " elements into dataset." << endl;
    print_stats();
#endif /* __XML__ */
    return true;
  }

#ifdef __XML__ // disable some derived methods if XML not available

  ////////////////////////////////////////////////////////////////
  // data

  template <class Tdata>
  bool pascal_dataset<Tdata>::included(const string &class_name,
				       uint difficult, uint truncated,
				       uint occluded) {
    return dataset<Tdata>::included(class_name)
      && !(ignore_difficult && difficult)
      && !(ignore_truncated && truncated) 
      && !(ignore_occluded && occluded);
  }

  template <class Tdata>
  bool pascal_dataset<Tdata>::count_samples() {
    total_difficult = 0;
    total_truncated = 0;
    total_occluded = 0;
    total_ignored = 0;
    total_samples = 0;
    regex eExt(XML_PATTERN);
    cmatch what;
    string xmlpath;
    path p(annroot);
    if (!exists(p)) {
      cerr << "path " << annroot << " does not exist." << endl;
      return false;
    }
    // read all xml files in annroot
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(p); itr != end_itr; ++itr) {
      if (!is_directory(itr->status()) &&
	  regex_match(itr->leaf().c_str(), what, eExt)) {
	xmlpath = itr->path().string();
	// parse xml
	try {
	  DomParser parser;
	  parser.parse_file(xmlpath);
	  if (parser) {
	    // initialize root node and list
	    const Node* pNode = parser.get_document()->get_root_node();
	    Node::NodeList list = pNode->get_children();
	    // parse all objects in image
	    for(Node::NodeList::iterator iter = list.begin();
		iter != list.end(); ++iter) {
	      if (!strcmp((*iter)->get_name().c_str(), "object")) {
		// check for difficult flag in object node
		Node::NodeList olist = (*iter)->get_children();
		count_sample(olist);
	      }
	    }
	  }
	} catch (const std::exception& ex) {
	  cerr << "Xml exception caught: " << ex.what() << endl;
	  return false;
	}
      }
    }
    cout << "Found: " << total_samples << " samples, including ";
    cout << total_difficult << " difficult, " << total_truncated;
    cout << " truncated and " << total_occluded << " occluded." << endl;
    ignore_difficult ? cout << "Ignoring" : cout << "Using";
    cout << " difficult samples." << endl;
    ignore_truncated ? cout << "Ignoring" : cout << "Using";
    cout << " truncated samples." << endl;
    ignore_occluded ? cout << "Ignoring" : cout << "Using";
    cout << " occluded samples." << endl;
    total_samples = total_samples - total_ignored;
    return true;
  }

  template <class Tdata>
  void pascal_dataset<Tdata>::count_sample(Node::NodeList &olist) {
    uint difficult = 0, truncated = 0, occluded = 0;
    string obj_classname, pose;
    bool pose_found = false;
    Node::NodeList::iterator oiter;
      
    for(oiter = olist.begin(); oiter != olist.end(); ++oiter) {
      if (!strcmp((*oiter)->get_name().c_str(), "difficult"))
	difficult = xml_get_uint(*oiter);
      else if (!strcmp((*oiter)->get_name().c_str(), "truncated"))
	truncated = xml_get_uint(*oiter);
      else if (!strcmp((*oiter)->get_name().c_str(), "occluded"))
	occluded = xml_get_uint(*oiter);
      else if (!strcmp((*oiter)->get_name().c_str(), "name"))
	xml_get_string(*oiter, obj_classname);
      else if (!strcmp((*oiter)->get_name().c_str(), "pose")) {
	xml_get_string(*oiter, pose);
	pose_found = true;
      }
    }
    
    ////////////////////////////////////////////////////////////////
    // object
    if (!usepartsonly) {
      // add object's class to dataset
      if (included(obj_classname, difficult, truncated, occluded)) {
	if (usepose && pose_found) { // append pose to class name
	  obj_classname += "_";
	  obj_classname += pose;
	}
	if (included(obj_classname, difficult, truncated, occluded))
	  this->add_class(obj_classname);
      }
    }
    // increment samples numbers
    total_samples++;
    if (difficult) total_difficult++;
    if (truncated) total_truncated++;
    if (occluded) total_occluded++;
    if ((difficult && ignore_difficult)
	|| (truncated && ignore_truncated)
	|| (occluded && ignore_occluded))
      total_ignored++;
    
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
	      if (included(part_classname, difficult, truncated, occluded)) {
		if (usepose && pose_found) { // append pose to class name
		  part_classname += "_";
		  part_classname += pose;
		}
		if (dataset<Tdata>::included(part_classname)) {
		  this->add_class(part_classname);
		  // increment samples numbers
		  this->total_samples++;
		  if (difficult) total_difficult++;
		  if (truncated) total_truncated++;
		  if (occluded) total_occluded++;
		  if ((difficult && ignore_difficult)
		      || (truncated && ignore_truncated)
		      || (occluded && ignore_occluded))
		    total_ignored++;
		}
	      }
	    }
	  }
	}
      }
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // process xml

  template <class Tdata>
  bool pascal_dataset<Tdata>::process_xml(const string &xmlfile) {
    string image_filename;
    string image_fullname;
    unsigned int h0 = 0, w0 = 0;
    unsigned int obj_number = 0;

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
	// load image 
	idx<Tdata> img = load_image<Tdata>(image_fullname);
	// parse all objects in image
	for(Node::NodeList::iterator iter = list.begin();
	    iter != list.end(); ++iter) {
	  if (!strcmp((*iter)->get_name().c_str(), "object"))
	    process_object(*iter, img, h0, w0, obj_number++, image_filename);
	}
      }
    } catch (const std::exception& ex) {
      cerr << "error: Xml exception caught: " << ex.what() << endl;
      return false;
    } catch (const char *err) {
      cerr << "error: " << err << endl;
      return false;
    }
    return true;
  }
  
  ////////////////////////////////////////////////////////////////
  // process 1 object of an xml file

  template <class Tdata>
  void pascal_dataset<Tdata>::process_object(Node* onode, idx<Tdata> &img,
					     uint &h0, uint &w0,uint obj_number,
					     const string &image_filename) {
    unsigned int xmin, ymin, xmax, ymax;
    unsigned int sizex, sizey;
    string obj_class, pose;
    unsigned int difficult, truncated, occluded;
    bool pose_found = false;
  
    ////////////////////////////////////////////////////////////////
    // object
    Node::NodeList list = onode->get_children();
    if (!usepartsonly) {
      // parse object node
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
	else if (!strcmp((*iter)->get_name().c_str(), "name"))
	  xml_get_string(*iter, obj_class);
	else if (!strcmp((*iter)->get_name().c_str(), "difficult"))
	  difficult = xml_get_uint(*iter);
	else if (!strcmp((*iter)->get_name().c_str(), "truncated"))
	  truncated = xml_get_uint(*iter);
	else if (!strcmp((*iter)->get_name().c_str(), "occluded"))
	  occluded = xml_get_uint(*iter);
	else if (!strcmp((*iter)->get_name().c_str(), "pose")) {
	  xml_get_string(*iter, pose);
	  pose_found = true;
	}
      }
      if (usepose && pose_found) { // append pose to class name
	obj_class += "_";
	obj_class += pose;
      }
      // compute size of bbox
      sizex = xmax - xmin;
      sizey = ymax - ymin;
      // process image  
      if (included(obj_class, difficult, truncated, occluded))
	process_image(img, h0, w0, xmin, ymin, xmax, ymax, sizex, sizey,
		      obj_class, obj_number, difficult, image_filename);
    }
    
    ////////////////////////////////////////////////////////////////
    // parts
    if (useparts || usepartsonly) {
      string part_class;

      // parse object node to get parts
      for(Node::NodeList::iterator iter = list.begin();
	  iter != list.end(); ++iter) {
	// parse parts
	if (!strcmp((*iter)->get_name().c_str(), "part")) {
	  Node::NodeList plist = (*iter)->get_children();
	  for(Node::NodeList::iterator piter = plist.begin();
	      piter != plist.end(); ++piter) {
	    // parse bounding box
	    if (!strcmp((*piter)->get_name().c_str(), "bndbox")) {
	      Node::NodeList blist = (*piter)->get_children();
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
	    else if (!strcmp((*piter)->get_name().c_str(), "name")) {
	      xml_get_string(*piter, part_class);
	    }
	  }
	  if (usepose && pose_found) { // append pose to class name
	    part_class += "_";
	    part_class += pose;
	  }
	  // compute size of bbox
	  sizex = xmax - xmin;
	  sizey = ymax - ymin;
	  // process image
	  if (included(part_class, difficult, truncated, occluded))
	    this->process_image(img, h0, w0, xmin, ymin, xmax, ymax, sizex,
				sizey, part_class, obj_number, difficult,
				image_filename);
	}
      }
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // process object's image

  template <class Tdata>
  void pascal_dataset<Tdata>::
  process_image(idx<Tdata> &img, uint &h0, uint &w0,
		uint xmin, uint ymin, uint xmax,
		uint ymax, uint sizex, uint sizey, string &obj_class,
		uint obj_number, uint difficult, const string &image_filename) {
    rect r(ymin, xmin, sizey, sizex);
    t_label label = this->get_label_from_class(obj_class);
    add_data(img, label, &obj_class, image_filename.c_str(), &r);
  }

#endif /* __XML__ */

} // end namespace ebl

#endif /* PASCAL_DATASET_HPP_ */
