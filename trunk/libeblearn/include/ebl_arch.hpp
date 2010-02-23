/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun, Pierre Sermanet *
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

namespace ebl {
  
  ////////////////////////////////////////////////////////////////

  template <class T> layers_n<T>::layers_n(bool oc)
    : layers_n_gen<state_idx<T> >(oc) {
  }

  template <class T> layers_n<T>* layers_n<T>::copy() {
    layers_n<T> *l2 = new layers_n<T>(true);
    //! Loop through all the modules and copy them
    int niter = this->modules->size()-1;
    for(int i = 0; i < niter; i++)
      l2->add_module((*this->modules)[i]->copy(),
		     new state_idx<T>((*this->hiddens)[i]->x.get_idxdim()));
    l2->add_last_module((*this->modules)[niter]->copy());
    return l2;
  }

  ////////////////////////////////////////////////////////////////

  template<class T>
  layers_2<T>::layers_2(module_1_1<T> &l1_, state_idx<T> &h_,
			module_1_1<T> &l2_)
    : layers_2_gen<state_idx<T>,state_idx<T>,state_idx<T> >(l1_, h_, l2_) {
  }  

  ////////////////////////////////////////////////////////////////
  // generic replicable modules classes

  // check that orders of input and module are compatible
  template <class T>
  void check_replicable_orders(module_1_1<T> &m, state_idx<T>& in) {
    if (in.x.order() < 0)
      eblerror("module_1_1_replicable cannot replicate this module (order -1)");
    if (in.x.order() < m.replicable_order())
      eblerror("input order must be greater or equal to module's operating \
order");
    if (in.x.order() > MAXDIMS)
      eblerror("cannot replicate using more dimensions than MAXDIMS");
  }

  //! recursively loop over the last dimensions of input in and out until
  //! reaching the operating order, then call the original fprop of module m.
  template<class T1, class T2>
  void module_eloop2_fprop(T1 &m, state_idx<T2> &in, state_idx<T2> &out) {
    if (m.replicable_order() == in.x.order()) {
      m.T1::fprop(in, out);
    } else if (m.replicable_order() > in.x.order()) {
      eblerror("the order of the input should be greater or equal to module's\
 operating order");
    } else {
      state_idx_eloop2(iin, in, T2, oout, out, T2) {
	module_eloop2_fprop(m, iin, oout);
      }
    }
  }

  //! recursively loop over the last dimensions of input in and out until
  //! reaching the operating order, then call the original bprop of module m.
  template<class T1, class T2>
  void module_eloop2_bprop(T1 &m, state_idx<T2> &in, state_idx<T2> &out) {
    if (m.replicable_order() == in.x.order()) {
      m.T1::bprop(in, out);
    } else if (m.replicable_order() > in.x.order()) {
      eblerror("the order of the input should be greater or equal to module's\
 operating order");
    } else {
      state_idx_eloop2(iin, in, T2, oout, out, T2) {
	module_eloop2_bprop(m, iin, oout);
      }
    }
  }

  //! recursively loop over the last dimensions of input in and out until
  //! reaching the operating order, then call the original bbprop of module m.
  template<class T1, class T2>
  void module_eloop2_bbprop(T1 &m, state_idx<T2> &in, state_idx<T2> &out) {
    if (m.replicable_order() == in.x.order()) {
      m.T1::bbprop(in, out);
    } else if (m.replicable_order() > in.x.order()) {
      eblerror("the order of the input should be greater or equal to module's\
 operating order");
    } else {
      state_idx_eloop2(iin, in, T2, oout, out, T2) {
	module_eloop2_bbprop(m, iin, oout);
      }
    }
  }

  template<class T, class T2>
  module_1_1_replicable<T,T2>::module_1_1_replicable(T &m) : module(m) {}
  template<class T, class T2>
  module_1_1_replicable<T,T2>::~module_1_1_replicable() {}

  template<class T1, class T2>
  void module_1_1_replicable<T1, T2>::
  fprop(state_idx<T2> &in, state_idx<T2> &out) {
    check_replicable_orders(module, in); // check for orders compatibility
    module.resize_output(in, out); // resize output
    module_eloop2_fprop<T1>(module, in, out);
    }

  template<class T1, class T2>
  void module_1_1_replicable<T1, T2>::
  bprop(state_idx<T2> &in, state_idx<T2> &out) {
    check_replicable_orders(module, in); // check for orders compatibility
    module_eloop2_bprop<T1>(module, in, out);
  }

  template<class T1, class T2>
  void module_1_1_replicable<T1, T2>::
  bbprop(state_idx<T2> &in, state_idx<T2> &out) {
    check_replicable_orders(module, in); // check for orders compatibility
    module_eloop2_bbprop<T1>(module, in, out);
  }

} // end namespace ebl
