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

#ifndef PASCALPART_DATASET_HPP_
#define PASCALPART_DATASET_HPP_

#include <algorithm>

#include "xml_utils.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // constructors & initializations

  template <class Tdata>
  pascalpart_dataset<Tdata>::
  pascalpart_dataset(const char *name_, const char *inroot_, bool ignore_diff)
    : pascal_dataset<Tdata>(name_, inroot_, ignore_diff) {
  }

  template <class Tdata>
  pascalpart_dataset<Tdata>::~pascalpart_dataset() {
  }

#ifdef __XML__ // disable some derived methods if XML not available

  ////////////////////////////////////////////////////////////////
  // data

  template <class Tdata>
  void pascalpart_dataset<Tdata>::count_sample(Node::NodeList &olist) {
    uint difficult = 0;
    string obj_classname, part_classname, pose;
    bool pose_found = false;
    Node::NodeList::iterator oiter;
    // extract name and difficult
    for(oiter = olist.begin(); oiter != olist.end(); ++oiter) {
      if (!strcmp((*oiter)->get_name().c_str(), "difficult"))
	difficult = xml_get_uint(*oiter);
      else if (!strcmp((*oiter)->get_name().c_str(), "name"))
	xml_get_string(*oiter, obj_classname);
      else if (!strcmp((*oiter)->get_name().c_str(), "pose")) {
	xml_get_string(*oiter, pose);
	pose_found = true;
      }
    }
    // extract parts
    for(oiter = olist.begin();oiter != olist.end(); ++oiter) {
      if (!strcmp((*oiter)->get_name().c_str(), "part")) {
	// get part's name
	Node::NodeList plist = (*oiter)->get_children();
	for(Node::NodeList::iterator piter = plist.begin();
	    piter != plist.end(); ++piter) {
	  if (!strcmp((*piter)->get_name().c_str(), "name")) {
	    xml_get_string(*piter, part_classname);
	    // found a part and its name, add it
	    if (!(this->ignore_difficult && difficult) && // ignore difficult?
		(find(exclude.begin(), exclude.end(),
		      part_classname) == exclude.end())) {
	      if (usepose && pose_found) { // append pose to class name
		part_classname += "_";
		part_classname += pose;
	      }
	      this->add_class(part_classname);
	    }
	    // increment samples numbers
	    this->total_samples++;
	    if (difficult)
	      this->total_difficult++;
	  }
	}
      }
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // process 1 object of an xml file

  template <class Tdata>
  void pascalpart_dataset<Tdata>::process_object(Node* onode, idx<ubyte> &img,
					     uint &h0, uint &w0,uint obj_number,
					     const string &image_filename) {
    unsigned int xmin, ymin, xmax, ymax;
    unsigned int sizex, sizey;
    string obj_class, part_class, pose;
    unsigned int difficult = 0;
    bool pose_found = false;

    // get object's name and difficulty
    Node::NodeList list = onode->get_children();
    for(Node::NodeList::iterator iter = list.begin();
	iter != list.end(); ++iter) {
      if (!strcmp((*iter)->get_name().c_str(), "name"))
	    xml_get_string(*iter, obj_class);
      else if (!strcmp((*iter)->get_name().c_str(), "difficult"))
	difficult = xml_get_uint(*iter);
      else if (!strcmp((*iter)->get_name().c_str(), "pose")) {
	xml_get_string(*iter, pose);
	pose_found = true;
      }
    }
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
	if (!(this->ignore_difficult && difficult))
	  this->process_image(img, h0, w0, xmin, ymin, xmax, ymax, sizex, sizey,
			      part_class, obj_number, difficult,
			      image_filename);
      }
    }
  }

#endif /* __XML__ */

} // end namespace ebl

#endif /* PASCALPART_DATASET_HPP_ */
