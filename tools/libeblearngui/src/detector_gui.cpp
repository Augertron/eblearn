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

#include "detector_gui.h"

namespace ebl {

  ubyte bbox_colors[12][3] = {
    {0,0,255}, // blue
    {0,255,0}, // green
    {255,0,0}, // red
    {255,255,255}, // white
    {0,255,255}, // light blue
    {255,0,255}, // pink
    {255,255,0}, // yellow
    {255,128,0}, // orange
    {0,128,255}, // medium blue
    {128,0,255}, // blue/pink
    {0,128,128}, // blue/green
    {128,128,128} // gray
  };

  ////////////////////////////////////////////////////////////////
  // bbox parts

  void draw_bbox(bbox &bb, idx<ubyte> &labels, uint h0, uint w0, double dzoom) {
    ostringstream label;
    int classid, colorid;
    classid = bb.class_id;
    colorid = classid % (sizeof (bbox_colors) / 3);
    uint h = (uint) (dzoom * bb.h0);
    uint w = (uint) (dzoom * bb.w0);
    label.str("");
    label.precision(2);
    label << (classid < labels.dim(0) ?(const char*)labels[classid].idx_ptr() : "****") 
	  << " " << bb.confidence;
    draw_box(h0 + h, w0 + w, (uint) (dzoom * bb.height), 
	     (uint) (dzoom * bb.width), bbox_colors[colorid][0],
	     bbox_colors[colorid][1], bbox_colors[colorid][2],
	     new string((const char *)label.str().c_str()));
  }

  ////////////////////////////////////////////////////////////////
  // bbox parts

  void draw_bbox_parts(bbox_parts &bb, idx<ubyte> &labels, uint h0, uint w0,
		       double dzoom) {
    std::vector<bbox_parts> &parts = bb.get_parts();
    for(uint i = 0; i < parts.size(); ++i) {
      bbox_parts &p = parts[i];
      draw_bbox(p, labels, h0, w0, dzoom); // draw part
      draw_bbox_parts(p, labels, h0, w0, dzoom); // explore sub parts
    }
  }

} // end namespace ebl
