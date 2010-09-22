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

#include "ebl_states.h"
#include "libidx.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // state

  state::state() {
  }

  state::~state() {
  }

  void state::clear() {
  }
  
  void state::clear_x() {
    eblerror("state::clear_x not implemented");
  }
  
  void state::clear_dx() {
    eblerror("state::clear_dx not implemented");
  }
  
  void state::clear_ddx() {
    eblerror("state::clear_ddx not implemented");
  }
  
  void state::update_gd(gd_param &arg) {
    eblerror("state::update_gd not implemented");
  }

  state& state::operator=(const state& other) {
    eblerror("state::operator= not implemented");
    return *this; // just for windows to compile
  }
  
  ////////////////////////////////////////////////////////////////

  gd_param::gd_param(double leta, double ln, double l1, double l2, int dtime,
		     double iner, double a_v, double a_t, double g_t)
  {
    eta = leta;
    n = ln;
    decay_time = dtime;
    decay_l2 = l2;
    decay_l1 = l1;
    inertia = iner;
    anneal_value = a_v;
    anneal_time = a_t;
    gradient_threshold = g_t;
    niter_done = 0;
  }

  ////////////////////////////////////////////////////////////////

  forget_param_linear::forget_param_linear(double v, double e)
  {
    value = v;
    exponent = e;
    if (e == 0)
      eblerror("Cannot use an exponent of 0 in forget param");
  }

  ////////////////////////////////////////////////////////////////

} // end namespace ebl
