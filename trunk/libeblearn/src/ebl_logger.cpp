/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#include "ebl_logger.h"

#ifdef __GUI__
#include "libidxgui.h"
//#define cout gui
#endif 

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////////////

  classifier_meter::classifier_meter() {
    this->clear();
  }

  int classifier_meter::correctp(ubyte co, ubyte cd) {
    // TODO-0: can co be negative?
    //	if (co == -1)
    //		return 0;
    if (co == cd)
      return 1;
    return -1;
  }

  void classifier_meter::clear() {
    total_correct = 0;
    total_error = 0;
    total_punt = 0;
    total_energy = 0;
    age = 0;
    size = 0;
  }

  void classifier_meter::resize (intg sz) {
    ylerror("not implemented");
  }

  char classifier_meter::update(intg a, class_state *co, ubyte cd, 
				state_idx *en) {
    intg crrct = this->correctp(co->output_class, cd);
    age = a;
    energy = en->x.get();
    confidence = co->confidence;
    total_energy += energy;
    if (crrct == 1)
      total_correct++;
    else if (crrct == 0)
      total_punt++;
    else if (crrct == -1)
      total_error++;
    size++;
    return crrct;
  }

  void classifier_meter::update(intg age_, bool correct, state_idx &en) {
    age = age_;
    energy = en.x.get();
    confidence = 0; // TODO? co->confidence;
    total_energy += energy;
    if (correct)
      total_correct++;
    else
      total_error++;
    size++;
  }

  void classifier_meter::test(class_state *co, ubyte cd, state_idx *en) {
    intg crrct = this->correctp(co->output_class, cd);
    age = 0;
    energy = en->x.get();
    confidence = co->confidence;
    total_energy = energy;
    total_correct = 0;
    total_punt = 0;
    total_error = 0;
    if (crrct == 1)
      total_correct = 1;
    else if (crrct == 0)
      total_punt = 1;
    else if (crrct == -1)
      total_error = 1;
    size = 1;
  }

  void classifier_meter::info() {
    /*
      (list
      age
      size
      (/ total-energy size)
      (/ (* 100 total-correct) size)
      (/ (* 100 total-error) size)
      (/ (* 100 total-punt) size)))
    */
    err_not_implemented();
  }

  void classifier_meter::info_sprint() {
    err_not_implemented();
  }

  void classifier_meter::info_print() {
    err_not_implemented();
  }

  void classifier_meter::display() {
    cout << "[" << (int) age << "]  sz=" <<  (int) size;
    cout << " energy=" << total_energy / (double) size;
    cout << "  correct=" <<  (total_correct * 100) / (double) size;
    cout << "% errors=" << (total_error * 100) / (double) size;
    cout << "% rejects=" << (total_punt * 100) / (double) size << "%";
  }

  bool classifier_meter::save() {
    err_not_implemented();
    return false;
  }

  bool classifier_meter::load() {
    err_not_implemented();
    return false;
  }

  ////////////////////////////////////////////////////////////////////////

  class_state::class_state(ubyte n) {
    sorted_classes = new idx<ubyte>(n);
    sorted_scores = new idx<float>(n);
  }

  class_state::~class_state() {
    delete sorted_classes;
    delete sorted_scores;
  }

  void class_state::resize(ubyte n) {
    sorted_classes->resize(n);
    sorted_scores->resize(n);
  }

  ////////////////////////////////////////////////////////////////////////

  max_classer::max_classer(idx<ubyte> *classes) {
    classindex2label = classes;
  }

  void max_classer::fprop(state_idx *in, class_state *out) {
    intg n = in->x.dim(0);
    out->resize(n);
    { idx_bloop2(sc, *(out->sorted_scores), float, insc, in->x, double) {
	sc.set(idx_max(insc));
      }
    }
    idx_copy(*classindex2label, *(out->sorted_classes));
    idx_sortdown(*(out->sorted_scores), *(out->sorted_classes));
    out->output_class = out->sorted_classes->get(0);
    out->confidence = out->sorted_scores->get(0);
  }

} // end namespace ebl
