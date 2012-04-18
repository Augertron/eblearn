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

#include "bbox.h"
#include <iomanip>
#include "utils.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // bbox

  uint bbox::iid_cnt = 0;

  bbox::bbox()
    : smart_pointer(), nacc(1), scaleh(1), scalew(1),
      iscale_index(0), oscale_index(0) {
    new_instance_id();
  }

  bbox::bbox(float h0, float w0, float height, float width)
    : rect<float>(h0, w0, height, width), smart_pointer(), nacc(1),
      scaleh(1), scalew(1), iscale_index(0), oscale_index(0) {
  }

  bbox::bbox(const bbox &other)
    : rect<float>(other), smart_pointer(),
      class_id(other.class_id),
      confidence(other.confidence),
      nacc(other.nacc),
      scaleh(other.scaleh),
      scalew(other.scalew),
      iscale_index(other.iscale_index), oscale_index(other.oscale_index),
      i(other.i),
      mi(other.mi),
      i0(other.i0),
      iheight(other.iheight),
      iwidth(other.iwidth),
      o(other.o),
      oheight(other.oheight),
      owidth(other.owidth),
      children(other.children) {
  }

  bbox::~bbox() {
  }

  void bbox::init_instance_id() {
    iid_cnt = 0;
  }

  void bbox::new_instance_id() {
    iid_cnt++;
    instance_id = iid_cnt;
  }

  void bbox::accumulate(bbox &b) {
    float acc = (float) b.nacc;
    nacc += b.nacc;
    confidence += b.confidence;
    h0 += b.h0 * acc;
    w0 += b.w0 * acc;
    height += b.height * acc;
    width += b.width * acc;
    iheight += (int) (b.iheight * acc);
    iwidth += (int) (b.iwidth * acc);
    i.h0 += b.i.h0 * acc;
    i.w0 += b.i.w0 * acc;
    i.height += b.i.height * acc;
    i.width += b.i.width * acc;
    i0.h0 += b.i0.h0 * acc;
    i0.w0 += b.i0.w0 * acc;
    i0.height += b.i0.height * acc;
    i0.width += b.i0.width * acc;
    oheight += (int) (b.oheight * acc);
    owidth += (int) (b.owidth * acc);
    o.h0 += (int) (b.o.h0 * acc);
    o.w0 += (int) (b.o.w0 * acc);
  }

  void bbox::mul(float d) {
    h0 *= d;
    w0 *= d;
    height *= d;
    width *= d;
    iheight = (int) (iheight * d);
    iwidth = (int) (iwidth * d);
    i.h0 *= d;
    i.w0 *= d;
    i.height *= d;
    i.width *= d;
    i0.h0 *= d;
    i0.w0 *= d;
    i0.height *= d;
    i0.width *= d;
    oheight = (int) (oheight * d);
    owidth = (int) (owidth * d);
    o.h0 = (int) (o.h0 * d);
    o.w0 = (int) (o.w0 * d);
  }

  string bbox::pretty(vector<string> *labels) {
    string s;
    if (labels && class_id < (int)labels->size()) s << (*labels)[class_id];
    else s << "bbox";
    s << " with confidence " << confidence
      << " in scale #" << iscale_index
      // << " (" << indim.dim(1) << "x" << indim.dim(2)
      << " / " << scaleh << "x" << scalew
      << " = " << iheight << "x" << iwidth << ")"
      << "\n"
      << "  bounding box: top left " << h0 << "x" << w0
      << " and size " << height << "x" << width
      << " out position: " << o.h0 << "x" << o.w0
      << " in " << oheight << "x" << owidth;
    return s;
  }

  bbox mean_bbox(vector<bbox*> &boxes, float bonus_per_bbox, int classid) {
    if (boxes.size() == 0)
      eblerror("expected non-empty bbox vector");
    float hcenter = 0.0, wcenter = 0.0, confidence = 0.0,
      height = 0.0, width = 0.0;
    // deviations from mean
    float hcenter_dev = 0.0, wcenter_dev = 0.0,
      height_dev = 0.0, width_dev = 0.0;
    bbox *bb;
    for (uint i = 0; i < boxes.size(); ++i) {
      bb = boxes[i];
      if (bb->confidence < 0) eblerror("expected confidence >= 0");
      hcenter += bb->hcenter();
      wcenter += bb->wcenter();
      height += bb->height;
      width += bb->width;
      confidence += bb->confidence;
    }
    // mean
    hcenter /= boxes.size();
    wcenter /= boxes.size();
    height /= boxes.size();
    width /= boxes.size();
    confidence /= boxes.size();
    // deviations
    for (uint i = 0; i < boxes.size(); ++i) {
      bb = boxes[i];
      hcenter_dev += (bb->hcenter() - hcenter) * bb->confidence ;
      wcenter_dev += (bb->wcenter() - wcenter) * bb->confidence ;
      height_dev += (bb->height - height) * bb->confidence ;
      width_dev += (bb->width - width) * bb->confidence ;
    }
    hcenter_dev /= boxes.size();
    wcenter_dev /= boxes.size();
    height_dev /= boxes.size();
    width_dev /= boxes.size();
    // apply weighted deviations
    hcenter += hcenter_dev;
    wcenter += wcenter_dev;
    height += height_dev;
    width += width_dev;

    bbox b;
    b.class_id = classid;
    b.confidence = confidence + boxes.size() * bonus_per_bbox;
    b.height =  height;
    b.width = width;
    b.h0 = (hcenter - height / 2);
    b.w0 = (wcenter - width / 2);
    return b;
  }

  std::ostream& operator<<(std::ostream& out, const bbox& b) {
    std::string s;
    s << b;
    out << s;
    out.flush();
    return out;
  }

  string& operator<<(string& out, const bbox& b) {
    out << "bbox:<class " << b.class_id << ", conf " << b.confidence << ", "
	<< "nacc " << b.nacc << ", " << (rect<float>&) b << ">";
#ifdef __DEBUG__
    out << " input win: " << b.i << " output win: " << b.o
	<< " input scale: " << b.iheight << "x" << b.iwidth;
    if (b.children.size() > 0) {
      out << ", " << (int) b.children.size() << " children:";
      for (bboxes::const_iterator i = b.children.begin();
	   i != b.children.end(); ++i)
	out << "\n  " << *i;
    }
#endif
    return out;
  }

  ////////////////////////////////////////////////////////////////
  // bbox_parts

  bbox_parts::bbox_parts() : bbox(), parts() {
  }

  bbox_parts::bbox_parts(const bbox &other)
    : bbox(other), parts() {
  }

  bbox_parts::bbox_parts(const bbox_parts &other)
    : bbox(other), parts() {
    for (uint i = 0; i < other.parts.size(); ++i) {
      add_part(other.parts[i]);
    }
  }

  bbox_parts::~bbox_parts() {
  }

  void bbox_parts::add_part(const bbox &p) {
    // if (dynamic_cast<const bbox_parts*>(&p)) {
    //   const bbox_parts &bp = (const bbox_parts&)p;
    //   parts.push_back(bp);
    // } else
    //   parts.push_back(p);
  }

  void bbox_parts::add_part(const bbox_parts &p) {
    // parts.push_back(p);
  }

  bool bbox_parts::share_parts(bbox_parts &bbp) {
    // check that these bb are not the same
    if (instance_id == bbp.instance_id)
      return true;
    // check all parts
    uint i, j;
    for (i = 0; i < parts.size(); ++i) {
       for (j = 0; j < bbp.parts.size(); ++j) {
	if (parts[i].share_parts(bbp.parts[j]))
	  return true; // we found parts match
      }
    }
    return false;
  }

  vector<bbox_parts>& bbox_parts::get_parts() {
    return parts;
  }

  // bboxes ////////////////////////////////////////////////////////////////////

  bboxes::bboxes(t_bbox_saving saving_type_, string *outdir_,
		 std::ostream &out, std::ostream &err)
    : mout(&out), merr(&err), saving_type(saving_type_) {
    if (outdir_)
      outdir = *outdir_;
  }

  bboxes::~bboxes() {
    // delete groups
    for (uint i = 0; i < grouped_boxes.size(); ++i) {
      svector<bbox> *g = grouped_boxes[i];
      if (g) delete g;
    }
  }

  void bboxes::new_group(idxdim &dims, string *name, int index) {
    if (index < 0) {
      grouped_boxes.push_back(new svector<bbox>());
      group_names.push_back(name ? *name : "");
      group_dims.push_back(dims);
    } else {
      uint ind = (uint) index;
      if (grouped_boxes.size() < ind + 1) {
	grouped_boxes.resize(ind + 1, NULL); // resize and init with NULL
	group_names.resize(ind + 1, "");
	group_dims.resize(ind + 1);
      }
      if (grouped_boxes[ind] != NULL) {
	(*merr) << "Warning: replacing existing group of bbox "
		<< group_names[ind]
		<< " at index " << index << " with new group "
		<< (name ? *name : "") << "." << endl;
	delete grouped_boxes[ind];
      }
      grouped_boxes[ind] = new svector<bbox>();
      if (name)
	group_names[ind] = *name;
      group_dims[ind] = dims;
    }
  }

  void bboxes::add(bbox *b, idxdim &dims, string *name, int index) {
    if (!b) eblerror("cannot add null bbox");
    svector<bbox> *current_group = NULL;
    if (grouped_boxes.size() == 0)
      new_group(dims, name, index);
    if (index < 0) { // addition ordering, just push_back
      current_group = grouped_boxes[grouped_boxes.size() - 1];
      current_group->push_back(*b);
    } else { // force ordering by index
      if (grouped_boxes.size() < (uint) (index + 1))
	new_group(dims, name, index);
      current_group = grouped_boxes[index];
      current_group->push_back(*b);
    }
    this->push_back(b); // also add to non-grouped vector
  }

  void bboxes::add(bbox &b, idxdim &dims, string *name, int index) {
    add(&b, dims, name, index);
  }

  void bboxes::add(bboxes &bbs, idxdim &dims, string *name, int index) {
    for (bboxes::iterator i = bbs.begin(); i != bbs.end(); ++i) {
      if (!i.exists()) eblerror("expected non-null bbox");
      add(*i, dims, name, index);
    }
  }

  void bboxes::add(vector<bbox> &bbs, idxdim &dims, string *name, int index) {
    for (vector<bbox>::iterator i = bbs.begin(); i != bbs.end(); ++i)
      add(new bbox(*i), dims, name, index);
  }

  bboxes bboxes::exclude(bboxes &other) {
    bboxes res;
    for (bboxes::iterator i = this->begin(); i != this->end(); ++i) {
      bool found = false;
      for (bboxes::iterator j = other.begin(); j != other.end(); ++j)
	if (i.ptr() == j.ptr()) {
	  found = true;
	  break ;
	}
      if (!found) res.push_back(*i);
    }
    return res;
  }

  void bboxes::save(string *dir_) {
    string dir = outdir;
    if (dir_) dir = *dir_;
    switch (saving_type) {
    case bbox_none: (*mout) << "No bboxes saved." << endl; break ;
    case bbox_all:
      (*mout) << "Saving bboxes in all formats." << endl;
      save_eblearn(&outdir);
      save_caltech(&outdir);
      break ;
    case bbox_eblearn:
      save_eblearn(&outdir);
      break ;
    case bbox_caltech:
      save_caltech(&outdir);
      break ;
    case bbox_class:
      save_class(&outdir);
      break ;
    default:
      eblerror("Unknown saving type " << saving_type);
    }
  }

  void bboxes::load_eblearn(const string &fname) {
    (*mout) << "Loading bounding boxes from bbox-eblearn format file "
	 << fname << endl;
    // open file
    FILE *fp = fopen(fname.c_str(), "r");
    if (!fp) {
      (*merr) << "failed to open " << fname << endl;
      eblerror("open failed");
    }
    string name, lastname;
      while (!feof(fp)) {
	char *str = fscan_str(fp);
	name = str;
	delete str;
	int imh = fscan_int(fp);
	int imw = fscan_int(fp);
	idxdim d(imh, imw);
	// add new group if we have a new name (this assumes we won't go back
	// to the same name again).
	if (name.compare(lastname))
	  new_group(d, &name);
	lastname = name;
	// get values
	bbox bb;
	bb.class_id = fscan_int(fp);
	bb.confidence = fscan_float(fp);
	bb.w0 = fscan_float(fp);
	bb.h0 = fscan_float(fp);
	float w1 = fscan_float(fp);
	float h1 = fscan_float(fp);
	bb.width = w1 - bb.w0;
	bb.height = h1 - bb.h0;
	add(bb, d);
	// get new line character
	fgetc(fp);
	int c = fgetc(fp);
	if (feof(fp))
	  break ;
	ungetc(c, fp);
      }
      fclose(fp);
    (*mout) << "Loaded " << describe() << " from " << fname << endl;
  }

  void bboxes::save_eblearn(string *dir_) {
    string dir = outdir;
    if (dir_) dir = *dir_;
    mkdir_full(dir);
    string fname = dir;
    fname << "/" << "bbox.txt";
    (*mout) << "Saving bboxes in eblearn format to " << fname << endl;
    // open file
    std::ofstream fp;
    fp.open(fname.c_str());
    if (!fp.is_open()) {
      (*merr) << "failed to open " << fname << endl;
      eblerror("open failed");
    }
    // write
    if (group_names.size() != grouped_boxes.size())
      eblerror("group_names and boxes should have the same size but have "
	       << group_names.size() << " and " << grouped_boxes.size());
    // loop on groups
    for (uint i = 0; i < grouped_boxes.size(); ++i) {
      svector<bbox> *bb = grouped_boxes[i];
      if (bb) {
	string &name = group_names[i];
	idxdim d = group_dims[i];
	// loop on boxes
	for (uint j = 0; j < bb->size(); ++j) {
	  bbox &b = (*bb)[j];
	  fp << name << " " << d.dim(0) << " " << d.dim(1) << " "
	     << b.class_id << " "
	     << b.confidence << " " << b.w0 << " " << b.h0 << " "
	     << b.w0 + b.width << " " << b.h0 + b.height << endl;
	}
      }
    }
    fp.close();
    (*mout) << "Saved " << describe() << " (eblearn style) to " << fname <<endl;
  }

  void bboxes::save_caltech(string *dir_) {
    string dir = outdir;
    if (dir_) dir = *dir_;
    dir << "/bbox_caltech/";
    if (!mkdir_full(dir))
      eblerror("failed to create directory " << dir);
    (*mout) << "Saving bboxes in caltech format to " << dir << endl;

    if (group_names.size() != grouped_boxes.size())
      eblerror("group_names and boxes should have the same size but have "
	       << group_names.size() << " and " << grouped_boxes.size());
    // loop on groups
    for (uint i = 0; i < grouped_boxes.size(); ++i) {
      svector<bbox> *bb = grouped_boxes[i];
      if (bb) {
	// make names
	string gname = group_names[i];
	string subdir = dirname(gname.c_str());
	string name = basename(gname.c_str());
	name = noext_name(name.c_str());
	string fn, fdir;
	fdir << dir << "/" << subdir;
	fn << fdir << "/" << name << ".txt";
	if (!mkdir_full(fdir))
	  eblerror("failed to create directory " << fdir);
// 	std::ostringstream fn;
// 	fn << dir << "/I" << setw(5) << setfill('0') << i << ".txt";
	// open file
	std::ofstream fp;
	fp.open(fn.c_str());
	if (!fp.is_open())
	  eblerror("failed to open " << fn);
	// loop on boxes
	for (uint j = 0; j < bb->size(); ++j) {
	  bbox &b = (*bb)[j];
	  fp << b.w0 << "," << b.h0 << "," << b.width << "," << b.height
	     << "," << b.confidence << endl;
	}
	fp.close();
      }
    }
    (*mout) << "Saved " << describe() << " (caltech style) to " << dir << endl;
  }

  void bboxes::save_class(string *dir_) {
    string dir = outdir;
    if (dir_) dir = *dir_;
    mkdir_full(dir);
    string fname = dir;
    fname << "/" << "bbox_class.txt";
    (*mout) << "Saving bboxes in classification format to " << fname << endl;
    // open file
    std::ofstream fp;
    fp.open(fname.c_str());
    if (!fp) {
      (*merr) << "failed to open " << fname << endl;
      eblerror("open failed");
    }
    // write
    if (group_names.size() != grouped_boxes.size())
      eblerror("group_names and boxes should have the same size but have "
	       << group_names.size() << " and " << grouped_boxes.size());
    // loop on groups
    for (uint i = 0; i < grouped_boxes.size(); ++i) {
      svector<bbox> *bb = grouped_boxes[i];
      if (bb) {
	string &name = group_names[i];
	idxdim d = group_dims[i];
	// loop on boxes
	for (uint j = 0; j < bb->size(); ++j) {
	  bbox &b = (*bb)[j];
	  fp << name << "; " << b.class_id << endl;
	}
      }
    }
    fp.close();
    (*mout) << "Saved " << describe() << " (classification style) to "
	 << fname << endl;
  }

  // sorting ///////////////////////////////////////////////////////////////////

  inline bool bbox_compare(bbox *i, bbox *j) {
    return (i->confidence > j->confidence);
  }
  
  void bboxes::sort_by_confidence() {
    std::vector<bbox*>::iterator b = ((std::vector<bbox*>*)this)->begin();
    std::vector<bbox*>::iterator e = ((std::vector<bbox*>*)this)->end();
    std::sort(b, e, bbox_compare);
  }

  bbox* bboxes::get_most_confident() {
    bbox *res = NULL;
    float confidence = limits<float>::min();
    for (bboxes::iterator i = this->begin(); i != this->end(); ++i)
      if (i->confidence > confidence) {
	confidence = i->confidence;
	res = &(*i);
      }
    return res;
  }

  bboxes bboxes::get_most_confidents() {
    bboxes res;
    // first find highest score
    float confidence = limits<float>::min();
    for (bboxes::iterator i = this->begin(); i != this->end(); ++i)
      if (i->confidence > confidence) confidence = i->confidence;
    // then extract all boxes with that score
    for (bboxes::iterator i = this->begin(); i != this->end(); ++i)
      if (i->confidence == confidence) res.push_back(*i);
    return res;
  }

  // scaling ///////////////////////////////////////////////////////////////////

  void bboxes::normalize_widths(float woverh) {
    for (bboxes::iterator i = this->begin(); i != this->end(); ++i) {
      EDEBUG("normalizing box ratio from " << i->width / i->height);
      i->scale_width(woverh);
      EDEBUG("to " << i->width / i->height);
    }
  }

  void bboxes::scale_centered(float hfact, float wfact) {
    if (hfact == 1.0 && wfact == 1.0) return ; // nothing to do
    // scale while keeping same center
    for (bboxes::iterator i = this->begin(); i != this->end(); ++i)
      i->scale_centered(hfact, wfact);
  }

  // thresholding //////////////////////////////////////////////////////////////

  void bboxes::threshold(float threshold) {
    for (bboxes::iterator i = this->begin(); i != this->end(); ) {
      if (i->confidence < threshold) this->erase(i);
      else i++;
    }
  }

  // printing //////////////////////////////////////////////////////////////////

  string bboxes::describe() {
    string desc = "bboxes (";
    uint nbb = 0, ngroups = 0;
    for (uint i = 0; i < grouped_boxes.size(); ++i)
      if (grouped_boxes[i]) {
	nbb += grouped_boxes[i]->size(); // size of each group
	ngroups++;
      }
    desc << nbb << " boxes in " << ngroups << " groups)";
    return desc;
  }

  void bboxes::print() {
    if (this->size() == 0)
      (*mout) << this->size() << " boxes.";
    else {
      (*mout) << this->size() << " boxes:\n";
      for (bboxes::iterator bb = this->begin(); bb != this->end(); ++bb)
	(*mout) << "class " << bb->class_id << " " << *bb << "\n";
    }
  }

  void bboxes::print_saving_type() {
    (*mout) << "Saving bboxes ";
    switch (saving_type) {
    case bbox_none: (*mout) << "disabled." << endl; break ;
    case bbox_all: (*mout) << "in all formats." << endl; break ;
    case bbox_eblearn: (*mout) << "in eblearn formats." << endl; break ;
    case bbox_caltech: (*mout) << "in caltech formats." << endl; break ;
    case bbox_class: (*mout) << "in classification format." << endl; break ;
    default:
      eblerror("Unknown saving type " << saving_type);
    }
  }

  string bboxes::pretty(vector<string> *labels) {
    string s;
    if (this->size() == 0) {
      s << (int) this->size() << " boxes.";
      return s;
    }
    s << (int) this->size() << " boxes:\n";
    for (bboxes::iterator i = this->begin() ; i != this->end(); ++i)
      s << i->pretty(labels) << "\n";
    return s;
  }

  string bboxes::pretty_short(vector<string> &labels) {
    string s;
    if (this->size() == 0) {
      s << (int) this->size() << " boxes.";
      return s;
    }
    s << (int) this->size() << " boxes:\n";
    for (bboxes::iterator bb = this->begin(); bb != this->end(); ++bb) {
      s << (bb->class_id < (int)labels.size() ? labels[bb->class_id] : "")
	<< " " << *bb << "\n";
    }
    return s;
  }

  string bboxes::str() {
    string s;
    if (this->size() == 0) {
      s << (int) this->size() << " boxes.";
      return s;
    }
    s << (int) this->size() << " boxes:";
    for (bboxes::iterator bb = this->begin(); bb != this->end(); ++bb) {
      s << " (class " << bb->class_id << " conf " << bb->confidence << ")";
    }
    return s;
  }

  bboxes* bboxes::get_group(const string &name) {
    uint i;
    for (i = 0; i < group_names.size(); ++i)
      if (!name.compare(group_names[i]))
	break ;
    if (i >= group_names.size())
      eblthrow("could not find image " << name << " in existing bboxes");
    svector<bbox> &bbs = *(grouped_boxes[i]);
    bboxes *ret = new bboxes;
    for (i = 0; i < bbs.size(); ++i)
      ret->push_back(bbs[i]);
    return ret;
  }

  idxdim bboxes::get_group_dims(const string &name) {
    uint i;
    for (i = 0; i < group_names.size(); ++i)
      if (!name.compare(group_names[i]))
	break ;
    if (i >= group_names.size())
      eblthrow("could not find image " << name << " in existing bboxes");
    return group_dims[i];
  }

} // end namespace ebl

