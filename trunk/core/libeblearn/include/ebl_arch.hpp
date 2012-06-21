/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun, Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

namespace ebl {

  // module_1_1 ////////////////////////////////////////////////////////////////

  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>::module_1_1(const char *name, bool bresize_)
    : module(name), bresize(bresize_), memoptimized(false),
      bmstate_input(false), bmstate_output(false), ninputs(1), noutputs(1)
#ifdef __CUDA__
    , in_gpu(false), out_gpu(false) 
#endif
{
  }

  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>::~module_1_1() {
    EDEBUG("deleting module_1_1: " << _name);
  }

  // single-state methods //////////////////////////////////////////////////////

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::fprop(Tin &in, Tout &out) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::bprop(Tin &in, Tout &out) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::bbprop(Tin &in, Tout &out) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::dump_fprop(Tin &in, Tout &out) {
    fprop(in, out); // no dumping by default, just fproping.
  }

  // multi-state methods ///////////////////////////////////////////////////////

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::fprop(mstate<Tin> &in, mstate<Tout> &out) {
    // check that in/out have at least 1 state and the same number of them.
    if (in.size() == 0) eblerror("input should have at least 1");
    //    if (in.size() != out.size()) out.resize(in);
    out.resize(in);
    // run regular fprop on each states
    for (uint i = 0; i < in.size(); ++i) {
      EDEBUG(this->name() << ": fprop at index " << i << " in: "
	    << in << " and out: " << out);
      Tin &fin = in[i];
      Tout &fout = out[i];
      EDEBUG(this->name() << ": in.x " << fin.x << ", min " << idx_min(fin.x)
	    << " max " << idx_max(fin.x));
      fprop(fin, fout);
    }
    // remember number of input/outputs
    ninputs = in.size();
    noutputs = out.size();
  }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::bprop(mstate<Tin> &in, mstate<Tout> &out) {
    // run regular bbprop on each states
    for (int i = (int) in.size() - 1; i >= 0; --i) {
      EDEBUG(this->name() << ": bprop at index " << i << " in: "
	    << in << " and out: " << out);
      Tin &bin = in[i];
      Tout &bout = out[i];
      EDEBUG(this->name() << ": bprop in.x " << bin.x
	    << " min " << idx_min(bin.x) << " max " << idx_max(bin.x)
	    << " out.x " << bout.x
	    << " min " << idx_min(bout.x) << " max " << idx_max(bout.x));
      bprop(bin, bout);
    }
  }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::bbprop(mstate<Tin> &in, mstate<Tout> &out) {
    // run regular bbprop on each states
    for (int i = (int) in.size() - 1; i >= 0; --i) {
      Tin &bbin = in[i];
      Tout &bbout = out[i];
      bbprop(bbin, bbout);
    }
  }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::dump_fprop(mstate<Tin> &in, mstate<Tout> &out) {
    // check that in/out have at least 1 state and the same number of them.
    if (in.size() == 0)
      eblerror("input should have at least 1");
    if (in.size() != out.size())
      out.resize(in);
    // run regular bbprop on each states
    for (uint i = 0; i < in.size(); ++i) {
      Tin &fin = in[i];
      Tout &fout = out[i];
      EDEBUG(this->name() << ": in.x " << fin.x << ", min " << idx_min(fin.x)
	    << " max " << idx_max(fin.x));
      this->dump_fprop(fin, fout);
    }
    // remember number of input/outputs
    ninputs = in.size();
    noutputs = out.size();
  }

  // multi to single state methods /////////////////////////////////////////////

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::fprop(mstate<Tin> &in, Tout &out) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::bprop(mstate<Tin> &in, Tout &out) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::bbprop(mstate<Tin> &in, Tout &out) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::dump_fprop(mstate<Tin> &in, Tout &out) {
  } //    eblerror("not implemented for " << this->name()); }

  // single to multi state methods /////////////////////////////////////////////

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::fprop(Tin &in, mstate<Tout> &out) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::bprop(Tin &in, mstate<Tout> &out) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::bbprop(Tin &in, mstate<Tout> &out) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::dump_fprop(Tin &in, mstate<Tout> &out) {
    err_not_implemented(); }

  //////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::forget(forget_param_linear& fp) {
  }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::normalize() {
  }

  template <typename T, class Tin, class Tout>
  int module_1_1<T,Tin,Tout>::replicable_order() { return -1; }

  template <typename T, class Tin, class Tout>
  bool module_1_1<T,Tin,Tout>::ignored(Tin &in, Tout &out) {
    if (this->_enabled) return false;
    idx_copy(in.x, out.x);
    return true;
  }

  // resizing //////////////////////////////////////////////////////////////////

  template <typename T, class Tin, class Tout>
  bool module_1_1<T,Tin,Tout>::resize_output(Tin &in, Tout &out, idxdim *d) {
    //if (!this->bresize) return false; // no resizing
    if (&in == &out) {
      outdims = out.x.get_idxdim(); // remember out size
      return false; // resize only when in and out are different
    }
    TIMING_RESIZING_ACCSTART(); // start accumulating resizing time
    if (d) { // use d as target dims
      outdims = *d; // remember out size
      if (d->order() != out.x.order()) { // re-allocate buffer
	EDEBUG(this->name() << ": reallocating output from " << out.x
	      << " to " << d);
	out = Tout(*d);
      } else if (*d != out.x.get_idxdim()) { // resize buffer
	EDEBUG(this->name() << ": resizing output from " << out.x << " to "
	      << *d);
	out.resize(*d);
      } else {
	TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
	return false;
      }
    } else { // use in.x as target dims
      outdims = in.x.get_idxdim(); // remember out size
      if (in.x.order() != out.x.order()) { // re-allocate buffer
	EDEBUG(this->name() << ": reallocating output from " << out.x
	      << " to " << in.x.get_idxdim());
	out = Tout(in.x.get_idxdim());
      } else if (in.x.get_idxdim() != out.x.get_idxdim()) { // resize buffer
	EDEBUG(this->name() << ": resizing output from " << out.x << " to "
	      << in.x.get_idxdim());
	out.resize(in.x.get_idxdim());
      } else {
	TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
	return false;
      }
    }
    TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
    return true;
  }

  template <typename T, class Tin, class Tout>
  bool module_1_1<T,Tin,Tout>::resize_output(Tin &in, idx<T> &out, idxdim *d) {
    //if (!this->bresize) return false; // no resizing
    if (&in.x == &out) {
      outdims = out.get_idxdim(); // remember out size
      return false; // resize only when different
    }
    TIMING_RESIZING_ACCSTART(); // start accumulating resizing time
    if (d) { // use d as target dims
      outdims = *d; // remember out size
      if (d->order() != out.order()) { // re-allocate buffer
	EDEBUG(this->name() << ": reallocating output from " << out
	      << " to " << d);
	out = idx<T>(*d);
      } else if (*d != out.get_idxdim()) { // resize buffer
	EDEBUG(this->name() << ": resizing output from " << out << " to " <<*d);
	out.resize(*d);
      } else {
	TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
	return false;
      }
    } else { // use in.x as target dims
      outdims = in.x.get_idxdim(); // remember out size
      if (in.x.order() != out.order()) { // re-allocate buffer
	EDEBUG(this->name() << ": reallocating output from " << out
	      << " to " << in.x.get_idxdim());
	out = idx<T>(in.x.get_idxdim());
      } else if (in.x.get_idxdim() != out.get_idxdim()) { // resize buffer
	EDEBUG(this->name() << ": resizing output from " << out << " to "
	      << in.x.get_idxdim());
	out.resize(in.x.get_idxdim());
      } else {
	TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
	return false;
      }
    }
    TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
    return true;
  }

  template <typename T, class Tin, class Tout>
  fidxdim module_1_1<T,Tin,Tout>::fprop_size(fidxdim &isize) {
    return isize;
  }

  template <typename T, class Tin, class Tout>
  fidxdim module_1_1<T,Tin,Tout>::bprop_size(const fidxdim &osize) {
    //EDEBUG(this->name() << ": " << osize << " -> same");
    return osize;
  }

  template <typename T, class Tin, class Tout>
  mfidxdim module_1_1<T,Tin,Tout>::fprop_size(mfidxdim &isize) {
    EDEBUG(this->name() << ": " << isize << " f-> ...");
    mfidxdim osize;
    for (uint i = 0; i < isize.size(); ++i)
      if (!isize.exists(i)) osize.push_back_empty();
      else osize.push_back(this->fprop_size(isize[i]));
    EDEBUG(this->name() << ": " << isize << " f-> " << osize);
    return osize;
  }

  // template <typename T, class Tin, class Tout>
  // mfidxdim module_1_1<T,Tin,Tout>::bprop_size(const mfidxdim &osize) {
  //   EDEBUG(this->name() << ": " << osize << " -> ...");
  //   mfidxdim isize;
  //   for (mfidxdim::const_iterator i = osize.begin(); i != osize.end(); ++i) {
  //     if (i.exists())
  // 	isize.push_back(this->bprop_size(*i));
  //     else
  // 	isize.push_back_empty();
  //   }
  //   EDEBUG(this->name() << ": " << osize << " -> " << isize);
  //   return isize;
  // }

  template <typename T, class Tin, class Tout>
  mfidxdim module_1_1<T,Tin,Tout>::bprop_size(mfidxdim &osize) {
    EDEBUG(this->name() << ": " << osize << " b-> ...");
    mfidxdim isize;
    for (mfidxdim::iterator i = osize.begin(); i != osize.end(); ++i) {
      if (i.exists())
	isize.push_back(this->bprop_size(*i));
      else
	isize.push_back_empty();
    }
    EDEBUG(this->name() << ": " << osize << " b-> " << isize);
    return isize;
  }

  template <typename T, class Tin, class Tout>
  std::string module_1_1<T,Tin,Tout>::pretty(idxdim &isize) {
    std::string s;
    fidxdim d = isize;
    s << " -> " << this->_name.c_str() << " -> " << fprop_size(d);
    return s;
  }

  template <typename T, class Tin, class Tout>
  std::string module_1_1<T,Tin,Tout>::pretty(mfidxdim &isize) {
    std::string s;
    midxdim d = fprop_size(isize);
    s << " -> " << this->_name.c_str() << " -> " << d;
    return s;
  }

  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>* module_1_1<T,Tin,Tout>::copy() {
    return this->copy(NULL);
  }

  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>* module_1_1<T,Tin,Tout>::copy(parameter<T,Tin> *p) {
    eblerror("deep copy not implemented in " << this->name());
    return NULL;
  }

  template <typename T, class Tin, class Tout>
  bool module_1_1<T,Tin,Tout>::optimize_fprop(Tin& in, Tout& out){
    return true;
  }

  template <typename T, class Tin, class Tout>
  bool module_1_1<T,Tin,Tout>::optimize_fprop(mstate<Tin>& in,
					      mstate<Tout>& out){
    eblerror("memory optimization not implemented for mstates");
	return false;
  }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::load_x(idx<T> &weights) {
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>* module_1_1<T,Tin,Tout>::last_module() {
    return this;
  }

  template <typename T, class Tin, class Tout>
  bool module_1_1<T,Tin,Tout>::mstate_input() {
    return bmstate_input;
  }

  template <typename T, class Tin, class Tout>
  bool module_1_1<T,Tin,Tout>::mstate_output() {
    return bmstate_output;
  }

  template <typename T, class Tin, class Tout>
  uint module_1_1<T,Tin,Tout>::get_ninputs() {
    return ninputs;
  }

  template <typename T, class Tin, class Tout>
  uint module_1_1<T,Tin,Tout>::get_noutputs() {
    return noutputs;
  }

  template <typename T, class Tin, class Tout>
  idxdim module_1_1<T,Tin,Tout>::get_outdims() {
    return outdims;
  }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::update_outdims(Tout &out) {
    outdims = out.x.get_idxdim();
  }

  ////////////////////////////////////////////////////////////////
  // module_2_1

  template <typename T, class Tin1, class Tin2, class Tout>
  module_2_1<T,Tin1,Tin2,Tout>::module_2_1(const char *name_)
    : module(name_), bresize(true) {
  }

  template <typename T, class Tin1, class Tin2, class Tout>
  module_2_1<T,Tin1,Tin2,Tout>::~module_2_1() {
#ifdef __DEBUG__
    cout << "deleting module_2_1: " << _name << endl;
#endif
  }

  //////////////////////////////////////////////////////////////////////////////
  // generic state methods

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::fprop(Tin1 &in1, Tin2 &in2, Tout &out) {
    err_not_implemented(); }

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::bprop(Tin1 &in1, Tin2 &in2, Tout &out) {
    err_not_implemented(); }

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::bbprop(Tin1 &in1, Tin2 &in2, Tout &out){
    err_not_implemented(); }

  //////////////////////////////////////////////////////////////////////////////
  // multi-state methods

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::fprop(mstate<Tin1> &in1, mstate<Tin2> &in2,
					   mstate<Tout> &out) {
    // check that in/out have at least 1 state and the same number of them.
    if (in1.size() == 0 || in2.size() == 0 || out.size() == 0
	|| in1.size() != out.size() || in2.size() != out.size())
      eblerror("in1, in2 and out don't have at least 1 state or don't have the "
	       << "same number of states: in1: " << in2.size()
	       << " in2: " << in2.size() << " out: " << out.size());
    // run regular bbprop on each states
    for (uint i = 0; i < in1.size(); ++i) {
      Tin1 &fin1 = in1[i];
      Tin2 &fin2 = in2[i];
      Tout &fout = out[i];
      fprop(fin1, fin2, fout);
    }
  }

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::bprop(mstate<Tin1> &in1, mstate<Tin2> &in2,
					   mstate<Tout> &out) {
    // check that in/out have at least 1 state and the same number of them.
    if (in1.size() == 0 || in2.size() == 0 || out.size() == 0
	|| in1.size() != out.size() || in2.size() != out.size())
      eblerror("in1, in2 and out don't have at least 1 state or don't have the "
	       << "same number of states: in1: " << in2.size()
	       << " in2: " << in2.size() << " out: " << out.size());
    // run regular bbprop on each states
    for (uint i = 0; i < in1.size(); ++i) {
      Tin1 &bin1 = in1[i];
      Tin2 &bin2 = in2[i];
      Tout &bout = out[i];
      bprop(bin1, bin2, bout);
    }
  }

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::bbprop(mstate<Tin1> &in1,
					    mstate<Tin2> &in2,
					    mstate<Tout> &out) {
    // check that in/out have at least 1 state and the same number of them.
    if (in1.size() == 0 || in2.size() == 0 || out.size() == 0
	|| in1.size() != out.size() || in2.size() != out.size())
      eblerror("in1, in2 and out don't have at least 1 state or don't have the "
	       << "same number of states: in1: " << in2.size()
	       << " in2: " << in2.size() << " out: " << out.size());
    // run regular bbprop on each states
    for (uint i = 0; i < in1.size(); ++i) {
      Tin1 &bin1 = in1[i];
      Tin2 &bin2 = in2[i];
      Tout &bout = out[i];
      bbprop(bin1, bin2, bout);
    }
  }

  //////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::forget(forget_param &fp) {
    err_not_implemented(); }

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::normalize() { err_not_implemented(); }

  template <typename T, class Tin1, class Tin2, class Tout>
  bool module_2_1<T,Tin1,Tin2,Tout>::resize_output(Tin1 &in1, Tin2 &in2,
						   Tout &out, idxdim *d) {
    if (!bresize) return false; // no resizing
    if (&in1 == &out) return false; // resize only when in and out are different
    if (d) { // use d as target dims
      if (d->order() != out.x.order()) { // re-allocate buffer
	EDEBUG(this->name() << ": reallocating output from " << out.x
	      << " to " << d);
	out = Tout(*d);
      } else if (*d != out.x.get_idxdim()) { // resize buffer
	EDEBUG(this->name() << ": resizing output from " << out.x << " to "
	      << *d);
	out.resize(*d);
      } else return false;
    } else { // use in.x as target dims
      if (in1.x.order() != out.x.order()) { // re-allocate buffer
	EDEBUG(this->name() << ": reallocating output from " << out.x
	      << " to " << in1.x.get_idxdim());
	out = Tout(in1.x.get_idxdim());
      } else if (in1.x.get_idxdim() != out.x.get_idxdim()) { // resize buffer
	EDEBUG(this->name() << ": resizing output from " << out.x << " to "
	      << in1.x.get_idxdim());
	out.resize(in1.x.get_idxdim());
      } else return false;
    }
    if (in2.x.get_idxdim() != in1.x.get_idxdim())
      eblerror("expected same size inputs " << in1.x << " and " << in2.x);
    return true;
  }

  ////////////////////////////////////////////////////////////////
  // ebm_1

  template <typename T, class Tin, class Ten>
  ebm_1<T,Tin,Ten>::ebm_1(const char *n) : module(n) {
  }

  template <typename T, class Tin, class Ten>
  ebm_1<T,Tin,Ten>::~ebm_1() {
  }

  template <typename T, class Tin, class Ten>
  void ebm_1<T,Tin,Ten>::fprop(Tin &in, Ten &energy) {
    err_not_implemented(); }

  template <typename T, class Tin, class Ten>
  void ebm_1<T,Tin,Ten>::bprop(Tin &in, Ten &energy) {
    err_not_implemented(); }

  template <typename T, class Tin, class Ten>
  void ebm_1<T,Tin,Ten>::bbprop(Tin &in, Ten &energy) {
    err_not_implemented(); }

  template <typename T, class Tin, class Ten>
  void ebm_1<T,Tin,Ten>::forget(forget_param &fp) {
    err_not_implemented(); }

  template <typename T, class Tin, class Ten>
  void ebm_1<T,Tin,Ten>::normalize() { err_not_implemented(); }

  ////////////////////////////////////////////////////////////////
  // ebm_module_1_1

  template <typename T, class Tin, class Tout, class Ten>
  ebm_module_1_1<T,Tin,Tout,Ten>::
  ebm_module_1_1(module_1_1<T,Tin,Tout> *m, ebm_1<T,Ten> *e, const char *name_)
    : module_1_1<T,Tin,Tout>(name_), module(m), ebm(e) {
    if (!m) eblerror("expected non-null module");
    if (!e) eblerror("expected non-null ebm");
    energy.dx.set((T)1.0); // d(E)/dE is always 1
    energy.ddx.set((T)0.0); // dd(E)/dE is always 0
  }

  template <typename T, class Tin, class Tout, class Ten>
  ebm_module_1_1<T,Tin,Tout,Ten>::~ebm_module_1_1() {
    delete module;
    delete ebm;
  }

  //////////////////////////////////////////////////////////////////////////////
  // single-state methods

  template <typename T, class Tin, class Tout, class Ten>
  void ebm_module_1_1<T,Tin,Tout,Ten>::fprop(Tin &in, Tout &out) {
    EDEBUG(this->name() << ": " << module->name() << ": in " << in);
    module->fprop(in, out);
    ebm->fprop(out, energy);
  }

  template <typename T, class Tin, class Tout, class Ten>
  void ebm_module_1_1<T,Tin,Tout,Ten>::bprop(Tin &in, Tout &out) {
    EDEBUG(this->name() << ": " << module->name() << ": bprop in " << in);
    ebm->bprop(out, energy);
    module->bprop(in, out);
  }

  template <typename T, class Tin, class Tout, class Ten>
  void ebm_module_1_1<T,Tin,Tout,Ten>::bbprop(Tin &in, Tout &out) {
    ebm->bbprop(out, energy);
    module->bbprop(in, out);
  }

  template <typename T, class Tin, class Tout, class Ten>
  void ebm_module_1_1<T,Tin,Tout,Ten>::forget(forget_param_linear &fp) {
    module->forget(fp);
  }

  template <typename T, class Tin, class Tout, class Ten>
  Ten& ebm_module_1_1<T,Tin,Tout,Ten>::get_energy() {
    return energy;
  }

  template <typename T, class Tin, class Tout, class Ten>
  fidxdim ebm_module_1_1<T,Tin,Tout,Ten>::fprop_size(fidxdim &isize) {
    return module->fprop_size(isize);
  }

  template <typename T, class Tin, class Tout, class Ten>
  fidxdim ebm_module_1_1<T,Tin,Tout,Ten>::bprop_size(const fidxdim &osize) {
    return module->bprop_size(osize);
  }

  template <typename T, class Tin, class Tout, class Ten>
  std::string ebm_module_1_1<T,Tin,Tout,Ten>::describe() {
    std::string desc;
    desc << "ebm_module_1_1 " << this->name() << " contains a module_1_1: "
	 << module->describe() << ", and an ebm1: " << ebm->describe();
    return desc;
  }

  ////////////////////////////////////////////////////////////////
  // ebm_2

  template <class Tin1, class Tin2, class Ten>
  ebm_2<Tin1,Tin2,Ten>::ebm_2(const char *name_) : module(name_) {
  }

  template <class Tin1, class Tin2, class Ten>
  ebm_2<Tin1,Tin2,Ten>::~ebm_2() {
  }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::fprop(Tin1 &i1, Tin2 &i2,Ten &energy){
    err_not_implemented(); }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::bprop(Tin1 &i1, Tin2 &i2,Ten &energy){
    err_not_implemented(); }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::bbprop(Tin1 &i1, Tin2 &i2,Ten &energy)
  { err_not_implemented(); }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::bprop1_copy(Tin1 &i1, Tin2 &i2, Ten &energy) {
    err_not_implemented(); }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::bprop2_copy(Tin1 &i1, Tin2 &i2, Ten &energy) {
    err_not_implemented(); }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::bbprop1_copy(Tin1 &i1, Tin2 &i2, Ten &energy) {
    err_not_implemented(); }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::bbprop2_copy(Tin1 &i1, Tin2 &i2, Ten &energy) {
    err_not_implemented(); }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::forget(forget_param_linear &fp) {
    err_not_implemented(); }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::normalize() {
    err_not_implemented(); }

  template <class Tin1, class Tin2, class Ten>
  double ebm_2<Tin1,Tin2,Ten>::infer1(Tin1 &i1, Tin2 &i2, Ten &energy,
				      infer_param &ip) {
    err_not_implemented(); return 0; }

  template <class Tin1, class Tin2, class Ten>
  double ebm_2<Tin1,Tin2,Ten>::infer2(Tin1 &i1, Tin2 &i2, infer_param &ip,
				      Tin2 *label, Ten *energy) {
    err_not_implemented(); return 0; }

  template <class Tin1, class Tin2, class Ten>
  void ebm_2<Tin1,Tin2,Ten>::infer2_copy(Tin1 &i1, Tin2 &i2, Ten &energy) {
    err_not_implemented(); }

  ////////////////////////////////////////////////////////////////
  // layers

  template <typename T, class Tstate>
  layers<T,Tstate>::layers(bool oc, const char *name_,
			   bool is_branch, bool narrow, intg dim,
			   intg sz, intg offset)
    : module_1_1<T,Tstate>(name_), intern_out(NULL),
      hi(NULL), ho(NULL), htmp(NULL),
      /* parallelism */
      branch(is_branch), intern_h0(NULL), intern_h1(NULL),
      // narrowing
      branch_narrow(narrow), narrow_dim(dim), narrow_size(sz),
      narrow_offset(offset) {
    this->own_contents = oc;
    msin.push_back(new Tstate(1));
    msout.push_back(new Tstate(1));
  }

  // Clean vectors. Module doesn't have ownership of sub-modules
  template <typename T, class Tstate>
  layers<T,Tstate>::~layers() {
    if (this->own_contents) {
      for (unsigned int i=0; i < modules.size(); i++)
    	delete modules[i];
      if (!this->memoptimized) {
	for(unsigned int i=0;i < hiddens.size(); i++)
	  delete hiddens[i];
      }
    }
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::
  add_module(module_1_1<T, Tstate, Tstate>* module) {
    // regular addition
    modules.push_back(module);
    hiddens.push_back(NULL);
    // update what type of input/output are expected
    this->bmstate_input = modules[0]->mstate_input();
    this->bmstate_output = modules[modules.size() - 1]->mstate_output();
  }

  // TODO: fix optimize fprop

  // template <typename T, class Tstate>
  // bool layers<T,Tstate>::optimize_fprop(Mstate& in, Mstate& out){
  //   this->memoptimized = true;
  //   if (modules.empty())
  //     eblerror("trying to fprop through empty layers");
  //   // initialize buffers
  //   hi = &in;
  //   ho = &out;
  //   // parallelism: do not modify input nor output
  //   if (branch) {
  //     // create our internal buffers with all dimensions set to 1
  //     intern_h0 = new Mstate(in);
  //     intern_h1 = new Mstate(in);
  //     ho = intern_h0;
  //   }
  //   // loop over modules
  //   for (uint i = 0; i < modules.size(); i++) {
  //     hiddens[i] = ho;
  //     // parallelism: for first module, do not allow optim with in buffer
  //     if (branch && i == 0) {
  // 	hi = intern_h1; // now we use only internal buffers
  // 	swap_buffers(); // swap hi and ho
  //     } else {
  // 	// call optimization on submodules, and remember if they put
  // 	// the output in ho (swap == true) or not (swap == false).
  // 	bool swap = modules[i]->optimize_fprop(*hi,*ho);
  // 	// if output is truly in ho, swap buffers, otherwise do nothing.
  // 	// if module was a branch, it di
  // 	if (swap)
  // 	  swap_buffers();
  //     }
  //   }
  //   // parallelism: remember which buffer contains the output
  //   if (branch) {
  //     intern_out = hiddens[modules.size() - 1];
  //     // a branch does not output to current track, so the output for the
  //     // mother branch is actually the branch's input, which is left in in
  //     return false; // output is in in
  //   }
  //   // tell the outside if the output is in in or out
  //   if (hiddens[modules.size() - 1] == &out)
  //     return true; // output is in out
  //   return false; // output is in in
  // }

  // fprop /////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void layers<T,Tstate>::fprop(Tstate& in, Tstate& out) {
    msin[0] = in;
    msout[0] = out;
    fprop(msin, msout);
    out = msout[0];
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::fprop(mstate<Tstate>& in, Tstate& out) {
    msout[0] = out;
    fprop(in, msout);
    out = msout[0];
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::fprop(Tstate& in, mstate<Tstate>& out) {
    msin[0] = in;
    fprop(msin, out);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::fprop(mstate<Tstate>& in, mstate<Tstate>& out) {
    if (modules.empty() && !branch)
      eblerror("trying to fprop through empty layers");
    // initialize buffers
    hi = &in;
    ho = &out;
    // narrow input data if required by branch
    mstate<Tstate> narrowed;
    if (branch && branch_narrow) {
      eblerror("not implemented");
      // narrowed = hi->narrow(narrow_dim, narrow_size, narrow_offset);
      // //EDEBUG("branch narrowing input " << hi->x << " to " << narrowed.x);
      // hi = &narrowed;
    }
    // loop over modules
    for(int i = 0; i < (int) modules.size(); i++){
      LOCAL_TIMING_START(); // timing debugging
      // if last module, output into out
      if (i == (int) modules.size() - 1 && !branch) ho = &out;
      else { // not last module, use hidden buffers
	ho = (mstate<Tstate>*) hiddens[i];
	// allocate hidden buffer if necessary
	if (ho == NULL) {
	  // allocate mstates with only 1 state.
	  hiddens[i] = new mstate<Tstate>(in, 1, 1);
	  ho = (mstate<Tstate>*) hiddens[i];
	}
      }
      // run module
      //      modules[i]->fprop(*hi, *ho);
      module_1_1<T,Tstate> *mod = modules[i];
      DEBUGMEM_PRETTY("before " << mod->name() << " fprop: ");
      if (mod->mstate_input() == mod->mstate_output()) // s-s or ms-ms
	mod->fprop(*hi, *ho);
      else { // s-ms or ms-s
	if (mod->mstate_output()) { // s-ms
	  Tstate &sin = (*hi)[0];
	  mod->fprop(sin, *ho);
	} else { // ms-s
	  if (ho->size() == 0) ho->push_back(new Tstate(1));
	  Tstate &sout = (*ho)[0];
	  mod->fprop(*hi, sout);
	}
      }

      // keep same input if current module is a branch, otherwise take out as in
      bool isbranch = false;
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch)
	isbranch = true;
      if (!isbranch)
	hi = ho;
      if (isbranch && i + 1 == (int) modules.size())
	ho = hi; // if last module is branch, set the input to be the branch out
      LOCAL_TIMING_REPORT(mod->name()); // timing debugging
    }
    if (branch) // remember output buffer (did not output to out)
      intern_out = ho;
    // remember number of input/outputs
    this->ninputs = in.size();
    this->noutputs = out.size();
  }

  // bprop /////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void layers<T,Tstate>::bprop(Tstate& in, Tstate& out) {
    msin[0] = in;
    msout[0] = out;
    bprop(msin, msout);
    in = msin[0];
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bprop(mstate<Tstate>& in, Tstate& out) {
    msout[0] = out;
    bprop(in, msout);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bprop(Tstate& in, mstate<Tstate>& out) {
    msin[0] = in;
    bprop(msin, out);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bprop(mstate<Tstate>& in, mstate<Tstate>& out) {
    if (this->memoptimized)
      eblerror("cannot bprop while using dual-buffer memory optimization");
    if (modules.empty())
      eblerror("trying to bprop through empty layers");
    // clear hidden states
    clear_dx();
    EDEBUG(this->name() << ": in " << in);
    // init buffers
    hi = &out;
    ho = &out;
    // last will be manual
    for (int i = (int) modules.size() - 1; i >= 0; i--){
      LOCAL_TIMING_START(); // timing debugging
      // set input
      if (i == 0) hi = &in;
      else hi = hiddens[i - 1];
      // run module
      EDEBUG(this->name() << " layers bprop hi " << *hi << " ho " << *ho);
      // modules[i]->bprop(*hi, *ho);
      module_1_1<T,Tstate> *mod = modules[i];
      if (mod->mstate_input() == mod->mstate_output()) // s-s or ms-ms
	mod->bprop(*hi, *ho);
      else { // s-ms or ms-s
	if (mod->mstate_output()) { // s-ms
	  Tstate &sin = (*hi)[0];
	  mod->bprop(sin, *ho);
	} else { // ms-s
	  Tstate &sout = (*ho)[0];
	  mod->bprop(*hi, sout);
	}
      }
      // shift output pointer to input
      ho = hi;
      LOCAL_TIMING_REPORT(mod->name() << " bprop"); // timing debugging
    }
  }

  // bbprop ////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void layers<T,Tstate>::bbprop(Tstate& in, Tstate& out) {
    msin[0] = in;
    msout[0] = out;
    bbprop(msin, msout);
    in = msin[0];
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bbprop(mstate<Tstate>& in, Tstate& out) {
    msout[0] = out;
    bbprop(in, msout);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bbprop(Tstate& in, mstate<Tstate>& out) {
    msin[0] = in;
    bbprop(msin, out);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bbprop(mstate<Tstate>& in, mstate<Tstate>& out) {
    if (this->memoptimized)
      eblerror("cannot bbprop while using dual-buffer memory optimization");
    if (modules.empty())
      eblerror("trying to bbprop through empty layers");

    // clear hidden states
    // do not clear if we are a branch, it must have been cleared already by
    // main branch
    if (!branch)
      clear_ddx();

    hi = &out;
    ho = &out;

    if (branch) // we are a branch, use the internal output
      ho = intern_out;

    // last will be manual
    for(int i = (int) modules.size() - 1; i >= 0; i--){
      LOCAL_TIMING_START(); // timing debugging
      // set input
      if (i == 0)
	hi = &in;
      else
	hi = hiddens[i-1];
      // if previous module is a branch, take its input as input
      if (i > 0 && dynamic_cast<layers<T,Tstate>*>(modules[i - 1]) &&
	  ((layers<T,Tstate>*)modules[i - 1])->branch) {
	if (i >= 2)
	  hi = hiddens[i - 2];
	else // i == 1
	  hi = &in;
      }
      // run module
      // modules[i]->bbprop(*hi, *ho);
      module_1_1<T,Tstate> *mod = modules[i];
      if (mod->mstate_input() == mod->mstate_output()) // s-s or ms-ms
	mod->bbprop(*hi, *ho);
      else { // s-ms or ms-s
	if (mod->mstate_output()) { // s-ms
	  Tstate &sin = (*hi)[0];
	  mod->bbprop(sin, *ho);
	} else { // ms-s
	  Tstate &sout = (*ho)[0];
	  mod->bbprop(*hi, sout);
	}
      }


      // shift output pointer to input
      ho = hi;
      LOCAL_TIMING_REPORT(mod->name() << " bbprop"); // timing debugging
    }
  }

  // dump_fprop ////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void layers<T,Tstate>::dump_fprop(Tstate& in, Tstate& out) {
    msin[0] = in;
    msout[0] = out;
    dump_fprop(msin, msout);
    out = msout[0];
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::dump_fprop(mstate<Tstate>& in, Tstate& out) {
    msout[0] = out;
    dump_fprop(in, msout);
    out = msout[0];
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::dump_fprop(Tstate& in, mstate<Tstate>& out) {
    msin[0] = in;
    dump_fprop(msin, out);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::dump_fprop(mstate<Tstate>& in, mstate<Tstate>& out) {
    if (modules.empty() && !branch)
      eblerror("trying to dump_fprop through empty layers");
    // initialize buffers
    hi = &in;
    ho = &out;
    // narrow input data if required by branch
    mstate<Tstate> narrowed;
    if (branch && branch_narrow) {
      eblerror("not implemented");
      // narrowed = hi->narrow(narrow_dim, narrow_size, narrow_offset);
      // //EDEBUG("branch narrowing input " << hi->x << " to " << narrowed.x);
      // hi = &narrowed;
    }
    // loop over modules
    for(int i = 0; i < (int) modules.size(); i++){
      // if last module, output into out
      if (i == (int) modules.size() - 1 && !branch)
	ho = &out;
      else { // not last module, use hidden buffers
	ho = (mstate<Tstate>*) hiddens[i];
	// allocate hidden buffer if necessary
	if (ho == NULL) {
	  // allocate mstates with only 1 state.
	  hiddens[i] = new mstate<Tstate>(in, 1, 1);
	  ho = (mstate<Tstate>*) hiddens[i];
	}
      }
      // run module

      //      modules[i]->dump_fprop(*hi, *ho);
      module_1_1<T,Tstate> *mod = modules[i];
      if (mod->mstate_input() == mod->mstate_output()) // s-s or ms-ms
	mod->dump_fprop(*hi, *ho);
      else { // s-ms or ms-s
	if (mod->mstate_output()) { // s-ms
	  Tstate &sin = (*hi)[0];
	  mod->dump_fprop(sin, *ho);
	} else { // ms-s
	  Tstate &sout = (*ho)[0];
	  mod->dump_fprop(*hi, sout);
	}
      }
      TIMING1(mod->name());

      // keep same input if current module is a branch, otherwise take out as in
      bool isbranch = false;
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch)
	isbranch = true;
      if (!isbranch)
	hi = ho;
      if (isbranch && i + 1 == (int) modules.size())
	ho = hi; // if last module is branch, set the input to be the branch out
    }
    if (branch) // remember output buffer (did not output to out)
      intern_out = ho;
    // remember number of input/outputs
    this->ninputs = in.size();
    this->noutputs = out.size();
  }

  //////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void layers<T,Tstate>::forget(forget_param_linear& fp){
    if (modules.empty() && !branch)
      eblerror("trying to forget through empty layers");

    for(unsigned int i=0; i<modules.size(); i++){
      module_1_1<T,Tstate,Tstate> *tt = modules[i];
      tt->forget(fp);
    }
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::normalize(){
    if (modules.empty())
      eblerror("trying to normalize through empty layers");

    for(unsigned int i=0; i<modules.size(); i++){
      modules[i]->normalize();
    }
  }

  template <typename T, class Tstate>
  fidxdim layers<T,Tstate>::fprop_size(fidxdim &isize) {
    fidxdim os(isize);
    //! Loop through all the layers of the module, and update output
    //! size accordingly.
    for (unsigned int i = 0; i < modules.size(); i++) {
      module_1_1<T,Tstate,Tstate> *tt = modules[i];
      // determine if module is a branch
      bool isbranch = false;
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch)
	isbranch = true;
      // do not go to branches
      if (!isbranch)
	os = tt->fprop_size(os);
    }
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(os);
    return os;
  }

  template <typename T, class Tstate>
  fidxdim layers<T,Tstate>::bprop_size(const fidxdim &osize) {
    fidxdim isize(osize);
    //! Loop through all the layers of the module, from the end to the beg.
    for (int i = (int) modules.size() - 1; i >= 0; i--) {
      module_1_1<T,Tstate,Tstate> *tt = modules[i];
      // determine if module is a branch
      bool isbranch = false;
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch)
	isbranch = true;
      // do not go to branches
      if (!isbranch)
	isize = tt->bprop_size(isize);
    }
    return isize;
  }

  template <typename T, class Tstate>
  mfidxdim layers<T,Tstate>::fprop_size(mfidxdim &isize) {
    mfidxdim os(isize);
    //! Loop through all the layers of the module, and update output
    //! size accordingly.
    for (unsigned int i = 0; i < modules.size(); i++) {
      module_1_1<T,Tstate,Tstate> *tt = modules[i];
      // determine if module is a branch
      bool isbranch = false;
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch)
	isbranch = true;
      // do not go to branches
      if (!isbranch)
	os = tt->fprop_size(os);
    }
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(os);
    this->ninputs = isize.size();
    this->noutputs = os.size();
    return os;
  }

  template <typename T, class Tstate>
  mfidxdim layers<T,Tstate>::bprop_size(mfidxdim &osize) {
    mfidxdim isize(osize);
    //! Loop through all the layers of the module, from the end to the beg.
    for (int i = (int) modules.size() - 1; i >= 0; i--) {
      module_1_1<T,Tstate,Tstate> *tt = modules[i];
      // determine if module is a branch
      bool isbranch = false;
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch)
	isbranch = true;
      // do not go to branches
      if (!isbranch) {
	//EDEBUG(this->name() << ": layers bprop_size before: " << isize);
	isize = tt->bprop_size(isize);
	//EDEBUG(this->name() << ": layers bprop_size after: " << isize);
      }
    }
    //EDEBUG(this->name() << ": " << osize << " -> " << isize);
    return isize;
  }

  template <typename T, class Tstate>
  layers<T,Tstate>* layers<T,Tstate>::copy() {
    layers<T,Tstate> *l2 = new layers<T,Tstate>(true);
    //! Loop through all the modules and buffers and copy them
    int niter = this->modules.size();
    for(int i = 0; i < niter; i++) {
      l2->add_module((module_1_1<T,Tstate>*)this->modules[i]->copy());
      if (this->hiddens[i] != NULL) {
	l2->hiddens[i] = new mstate<Tstate>(*(this->hiddens[i]));
	l2->hiddens[i]->copy(*(l2->hiddens[i]));
      }
    }
    return l2;
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::swap_buffers() {
    htmp = hi;
    hi = ho;
    ho = htmp;
  }

  template <typename T, class Tstate>
  uint layers<T,Tstate>::size() {
    return modules.size();
  }

  template <typename T, class Tstate>
  std::string layers<T,Tstate>::pretty(idxdim &isize) {
    mfidxdim is(isize);
    return this->pretty(is);
  }

  template <typename T, class Tstate>
  std::string layers<T,Tstate>::pretty(mfidxdim &isize) {
    std::string s;
    mfidxdim is(isize);
    //! Loop through all the layers of the module, and update output
    //! size accordingly.
    for (unsigned int i = 0; i < modules.size(); i++) {
      module_1_1<T,Tstate> *tt = modules[i];
      // determine if module is a branch
      bool isbranch = false;
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch)
	isbranch = true;
      // do not go to branches
      if (!isbranch) {
	s << tt->pretty(is);
	mfidxdim mis(is);
	mis = tt->fprop_size(mis);
	is = mis;
      }
    }
    return s;
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::clear_dx() {
    // clear hidden states
    for (uint i = 0; i<hiddens.size(); i++){
      if (hiddens[i])
	hiddens[i]->clear_dx();
    }
    // clear hidden states of branches
    for (uint i = 0; i < modules.size(); ++i) {
      // check if this module is a branch
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch) {
	// if yes, clear its hidden states
	layers<T,Tstate> *branch = (layers<T,Tstate>*) modules[i];
	branch->clear_dx();
      }
    }
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::clear_ddx() {
    // clear hidden states
    for (uint i = 0; i < hiddens.size(); i++) {
      if (hiddens[i])
	hiddens[i]->clear_ddx();
    }
    // clear hidden states of branches
    for (uint i = 0; i < modules.size(); ++i) {
      // check if this module is a branch
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch) {
	// if yes, clear its hidden states
	layers<T,Tstate> *branch = (layers<T,Tstate>*) modules[i];
	branch->clear_ddx();
      }
    }
  }

  template <typename T, class Tstate>
  bool layers<T,Tstate>::is_branch() {
    return branch;
  }

  template <typename T, class Tstate>
  module_1_1<T, Tstate, Tstate>*
  layers<T,Tstate>::find(const char *name) {
    for (uint i = 0; i < modules.size(); ++i) {
      module_1_1<T, Tstate, Tstate>* m = modules[i];
      if (!strcmp(name, m->name()))
	return m;
    }
    return NULL; // not found
  }

  template <typename T, class Tstate>
  module_1_1<T, Tstate, Tstate>*
  layers<T,Tstate>::last_module() {
    if (modules.size() == 0)
      eblerror("requires at least 1 module");
    return modules[modules.size() - 1]->last_module();
  }

  template <typename T, class Tstate>
  std::string layers<T,Tstate>::describe(uint indent) {
    std::string desc;
    desc << "Module " << this->name() << " contains "
	 << (int) modules.size() << " modules:\n";
    for(uint i = 0; i < modules.size(); ++i) {
      for (uint j = 0; j < indent; ++j) desc << "\t";
      desc << i << ": " << modules[i]->describe();
      if (i != modules.size() - 1) desc << "\n";
    }
    return desc;
  }

  template <typename T, class Tstate>
  bool layers<T,Tstate>::mstate_input() {
    if (modules.size())
      return modules[0]->mstate_input();
    return this->bmstate_input;
  }

  template <typename T, class Tstate>
  bool layers<T,Tstate>::mstate_output() {
    if (modules.size())
      return modules[modules.size() - 1]->mstate_output();
    return this->bmstate_output;
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::set_output_streams(std::ostream &out,
					    std::ostream &err) {
    for(uint i = 0; i < modules.size(); ++i)
      modules[i]->set_output_streams(out, err);
  }

  ////////////////////////////////////////////////////////////////
  // layers_2

  template <typename T, class Tin, class Thid, class Tout>
  layers_2<T,Tin,Thid,Tout>::layers_2(module_1_1<T,Tin,Thid> &l1,
				      Thid &h, module_1_1<T,Thid,Tout> &l2)
    : layer1(l1), hidden(h), layer2(l2) {
  }

  // Do nothing. Module doesn't have ownership of sub-modules
  template <typename T, class Tin, class Thid, class Tout>
  layers_2<T,Tin,Thid,Tout>::~layers_2() {
  }

  template <typename T, class Tin, class Thid, class Tout>
  void layers_2<T,Tin,Thid,Tout>::fprop(Tin &in, Tout &out) {
    layer1.fprop(in, hidden);
    layer2.fprop(hidden, out);
  }

  template <typename T, class Tin, class Thid, class Tout>
  void layers_2<T,Tin,Thid,Tout>::bprop(Tin &in, Tout &out) {
    hidden.clear_dx();
    layer2.bprop(hidden, out);
    layer1.bprop(in, hidden);
  }

  template <typename T, class Tin, class Thid, class Tout>
  void layers_2<T,Tin,Thid,Tout>::bbprop(Tin &in, Tout &out) {
    hidden.clear_ddx();
    layer2.bbprop(hidden, out);
    layer1.bbprop(in, hidden);
  }

  template <typename T, class Tin, class Thid, class Tout>
  void layers_2<T,Tin,Thid,Tout>::forget(forget_param_linear &fp) {
    layer1.forget(fp);
    layer2.forget(fp);
  }

  template <typename T, class Tin, class Thid, class Tout>
  void layers_2<T,Tin,Thid,Tout>::normalize() {
    layer1.normalize();
    layer2.normalize();
  }

  template <typename T, class Tin, class Thid, class Tout>
  fidxdim layers_2<T,Tin,Thid,Tout>::fprop_size(fidxdim &isize) {
    fidxdim os(isize);
    os = layer1.fprop_size(os);
    os = layer2.fprop_size(os);
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(os);
    return os;
  }

  template <typename T, class Tin, class Thid, class Tout>
  fidxdim layers_2<T,Tin,Thid,Tout>::bprop_size(const fidxdim &osize) {
    fidxdim isize(osize);
    isize = layer2.bprop_size(isize);
    isize = layer1.bprop_size(isize);
    return isize;
  }

  template <typename T, class Tin, class Thid, class Tout>
  std::string layers_2<T,Tin,Thid,Tout>::pretty(idxdim &isize) {
    std::string s;
    idxdim is(isize);
    s << layer1.pretty(is);
    s << " -> ";
    is = layer1.fprop_size(is);
    s << layer2.pretty(is);
    return s;
  }

  ////////////////////////////////////////////////////////////////

  template <typename T, class Tin, class Thid, class Ten>
  fc_ebm1<T,Tin,Thid,Ten>::fc_ebm1(module_1_1<T,Tin,Thid> &fm,
				   Thid &fo, ebm_1<T,Thid,Ten> &fc)
    : fmod(fm), fout(fo), fcost(fc) {
  }

  template <typename T, class Tin, class Thid, class Ten>
  fc_ebm1<T,Tin,Thid,Ten>::~fc_ebm1() {}

  template <typename T, class Tin, class Thid, class Ten>
  void fc_ebm1<T,Tin,Thid,Ten>::fprop(Tin &in, Ten &energy) {
    fmod.fprop(in, fout);
    fcost.fprop(fout, energy);
  }

  template <typename T, class Tin, class Thid, class Ten>
  void fc_ebm1<T,Tin,Thid,Ten>::bprop(Tin &in, Ten &energy) {
    fout.clear_dx();
    fcost.bprop(fout, energy);
    fmod.bprop(in, fout);
  }

  template <typename T, class Tin, class Thid, class Ten>
  void fc_ebm1<T,Tin,Thid,Ten>::bbprop(Tin &in, Ten &energy) {
    fout.clear_ddx();
    fcost.bbprop(fout, energy);
    fmod.bbprop(in, fout);
  }

  template <typename T, class Tin, class Thid, class Ten>
  void fc_ebm1<T,Tin,Thid,Ten>::forget(forget_param &fp) {
    fmod.forget(fp);
    fcost.forget(fp);
  }

  ////////////////////////////////////////////////////////////////

  template <typename T, class Tin1, class Tin2, class Ten>
  fc_ebm2<T,Tin1,Tin2,Ten>::fc_ebm2(module_1_1<T,Tin1,Tin1> &fm,
				    Tin1 &fo,
				    ebm_2<Tin1,Tin2,Ten> &fc)
    : fmod(fm), fout(fo), fcost(fc) {
  }

  template <typename T, class Tin1, class Tin2, class Ten>
  fc_ebm2<T,Tin1,Tin2,Ten>::~fc_ebm2() {}

  template <typename T, class Tin1, class Tin2, class Ten>
  void fc_ebm2<T,Tin1,Tin2,Ten>::fprop(Tin1 &in1, Tin2 &in2, Ten &energy) {
    fmod.fprop(in1, fout);
    fcost.fprop(fout, in2, energy);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(energy.x, "dump_fc_ebm2_energy.x.mat");
    save_matrix(in1.x, "dump_fc_ebm2_cost_in1.x.mat");
#endif
  }

  template <typename T, class Tin1, class Tin2, class Ten>
  void fc_ebm2<T,Tin1,Tin2,Ten>::bprop(Tin1 &in1, Tin2 &in2, Ten &energy) {
    fout.clear_dx();
    // in2.clear_dx(); // TODO this assumes Tin2 == fstate_idx
    fcost.bprop(fout, in2, energy);
    fmod.bprop(in1, fout);
  }

  template <typename T, class Tin1, class Tin2, class Ten>
  void fc_ebm2<T,Tin1,Tin2,Ten>::bbprop(Tin1 &in1, Tin2 &in2, Ten &energy){
    fout.clear_ddx();
    // in2.clear_ddx(); // TODO this assumes Tin2 == fstate_idx
    fcost.bbprop(fout, in2, energy);
    fmod.bbprop(in1, fout);
  }

  template <typename T, class Tin1, class Tin2, class Ten>
  void fc_ebm2<T,Tin1,Tin2,Ten>::forget(forget_param_linear &fp) {
    fmod.forget(fp);
    fcost.forget(fp);
  }

  template <typename T, class Tin1, class Tin2, class Ten>
  double fc_ebm2<T,Tin1,Tin2,Ten>::infer2(Tin1 &i1, Tin2 &i2,
					  infer_param &ip, Tin2 *label,
					  Ten *energy) {
    fmod.fprop(i1, fout); // first propagate all the way up
    return fcost.infer2(fout, i2, ip, label, energy); //then infer from energies
  }

  ////////////////////////////////////////////////////////////////
  // generic replicable modules classes

  // check that orders of input and module are compatible
  template <typename T, class Tstate>
  void check_replicable_orders(module_1_1<T,Tstate> &m, Tstate& in) {
    if (in.x.order() < 0)
      eblerror("module_1_1_replicable cannot replicate this module (order -1)");
    if (in.x.order() < m.replicable_order())
      eblerror("input order must be >= to module's operating order, input is "
	       << in.x << " but module operates with order "
	       << m.replicable_order());
    if (in.x.order() > MAXDIMS)
      eblerror("cannot replicate using more dimensions than MAXDIMS");
  }

  //! recursively loop over the last dimensions of input in and out until
  //! reaching the operating order, then call the original fprop of module m.
  template <class Tmodule, class Tstate>
  void module_eloop2_fprop(Tmodule &m, Tstate &in, Tstate &out) {
    if (m.replicable_order() == in.x.order()) {
      m.Tmodule::fprop(in, out);
    } else if (m.replicable_order() > in.x.order()) {
      eblerror("input order must be >= to module's operating order, input is "
	       << in.x << " but module operates with order "
	       << m.replicable_order());
    } else {
      state_idx_eloop2(iin, in, Tstate, oout, out, Tstate) {
	module_eloop2_fprop<Tmodule,Tstate>(m, (Tstate&) iin, (Tstate&) oout);
      }
    }
  }

  //! recursively loop over the last dimensions of input in and out until
  //! reaching the operating order, then call the original bprop of module m.
  template <class Tmodule, class Tstate>
  void module_eloop2_bprop(Tmodule &m, Tstate &in, Tstate &out) {
    if (m.replicable_order() == in.x.order()) {
      m.Tmodule::bprop(in, out);
    } else if (m.replicable_order() > in.x.order()) {
      eblerror("the order of the input should be greater or equal to module's\
 operating order");
    } else {
      state_idx_eloop2(iin, in, Tstate, oout, out, Tstate) {
	module_eloop2_bprop<Tmodule,Tstate>(m, (Tstate&) iin, (Tstate&) oout);
      }
    }
  }

  //! recursively loop over the last dimensions of input in and out until
  //! reaching the operating order, then call the original bbprop of module m.
  template <class Tmodule, class Tstate>
  void module_eloop2_bbprop(Tmodule &m, Tstate &in, Tstate &out) {
    if (m.replicable_order() == in.x.order()) {
      m.Tmodule::bbprop(in, out);
    } else if (m.replicable_order() > in.x.order()) {
      eblerror("the order of the input should be greater or equal to module's\
 operating order");
    } else {
      state_idx_eloop2(iin, in, Tstate, oout, out, Tstate) {
	module_eloop2_bbprop<Tmodule,Tstate>(m, (Tstate&) iin, (Tstate&) oout);
      }
    }
  }

  template <class Tmodule, typename T, class Tstate>
  module_1_1_replicable<Tmodule,T,Tstate>::module_1_1_replicable(Tmodule &m)
    : module(m) {
  }

  template <class Tmodule, typename T, class Tstate>
  module_1_1_replicable<Tmodule,T,Tstate>::~module_1_1_replicable() {
  }

  template <class Tmodule, typename T, class Tstate>
  void module_1_1_replicable<Tmodule,T,Tstate>::fprop(Tstate &in, Tstate &out) {
    check_replicable_orders(module, in); // check for orders compatibility
    module.resize_output(in, out); // resize output
    module_eloop2_fprop<Tmodule,Tstate>(module, (Tstate&) in, (Tstate&) out);
    }

  template <class Tmodule, typename T, class Tstate>
  void module_1_1_replicable<Tmodule,T,Tstate>::bprop(Tstate &in, Tstate &out) {
    check_replicable_orders(module, in); // check for orders compatibility
    module_eloop2_bprop<Tmodule,Tstate>(module, (Tstate&) in, (Tstate&) out);
  }

  template <class Tmodule, typename T, class Tstate>
  void module_1_1_replicable<Tmodule,T,Tstate>::bbprop(Tstate &in, Tstate &out){
    check_replicable_orders(module, in); // check for orders compatibility
    module_eloop2_bbprop<Tmodule,Tstate>(module, (Tstate&) in, (Tstate&) out);
  }

  /////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  narrow_module<T,Tstate>::narrow_module(int dim_, intg size_, intg offset_,
					 bool narrow_states_)
    : module_1_1<T,Tstate>("narrow_module"), dim(dim_), size(size_),
      narrow_states(narrow_states_) {
    this->bmstate_input = true; // this module takes multi-state inputs
    this->bmstate_output = true; // this module takes multi-state outputs
    offsets.push_back(offset_);
  }

  template <typename T, class Tstate>
  narrow_module<T,Tstate>::
  narrow_module(int dim_, intg size_, vector<intg> &offsets_, bool states_,
		const char *name_)
    : module_1_1<T,Tstate>(name_),
      dim(dim_), size(size_), offsets(offsets_), narrow_states(states_) {
    this->bmstate_input = true; // this module takes multi-state inputs
    this->bmstate_output = true; // this module takes multi-state outputs
  }

  template <typename T, class Tstate>
  narrow_module<T,Tstate>::~narrow_module() {
  }

  template <typename T, class Tstate>
  void narrow_module<T,Tstate>::fprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    // narrow each state of multi-state in
    if (narrow_states) {
      out.resize(in);
      for (uint i = 0; i < in.size(); ++i)
	fprop(in[i], out[i]);
    } else { // narrow multi-state itself
      if (dim == 0) { // narrow on states
	out.resize(in, offsets.size() * size);
	for (uint o = 0; o < offsets.size(); ++o) {
	  intg offset = offsets[o];
	  if ((intg) in.size() < offset + size)
	    eblerror("expected at least " << offset + size
		     << " states in narrow of dimension "
		     << dim << " at offset " << offset << " to size " << size
		     << " but found only " << in.size() << " states");
	  for (intg i = offset; i < offset + size; ++i)
	    out[i - offset + o * size] = in[i];
	}
      } else eblerror("not implemented");
    }
    this->ninputs = in.size();
    this->noutputs = out.size();
    EDEBUG("narrowed " << in << " to " << out);
  }

  template <typename T, class Tstate>
  void narrow_module<T,Tstate>::bprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    // TODO: assign states back to their input location?
  }

  template <typename T, class Tstate>
  void narrow_module<T,Tstate>::bbprop(mstate<Tstate> &in, mstate<Tstate> &out){
    // TODO: assign states back to their input location?
  }

  template <typename T, class Tstate>
  void narrow_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    // TODO: handle multiple offsets by copying narrows next to each other
    intg offset = offsets[0];
    out = in.narrow(dim, size, offset);
  }

  template <typename T, class Tstate>
  std::string narrow_module<T,Tstate>::describe() {
    std::string s;
    s << "narrow_module " << this->name() << " narrowing dimension " << dim
      << " to size " << size << " starting at offset(s) " << offsets;
    return s;
  }

  template <typename T, class Tstate>
  narrow_module<T,Tstate>* narrow_module<T,Tstate>::copy() {
    narrow_module<T,Tstate> *l2 =
      new narrow_module<T,Tstate>(dim, size, offsets, narrow_states);
    return l2;
  }

  template <typename T, class Tstate>
  mfidxdim narrow_module<T,Tstate>::fprop_size(mfidxdim &isize) {
    EDEBUG(this->name() << ": " << isize << " f-> ...");
    mfidxdim osize;
    if (narrow_states) { eblerror("not implemented");
    } else {
      if (dim == 0) { // narrow on states
	osize.resize_default(offsets.size() * size);
	for (uint o = 0; o < offsets.size(); ++o) {
	  intg offset = offsets[o];
	  if ((intg) isize.size() < offset + size)
	    eblerror("expected at least " << offset + size
		     << " states in narrow of dimension "
		     << dim << " at offset " << offset << " to size " << size
		     << " but found only " << isize.size() << " states");
	  for (intg i = offset; i < offset + size; ++i)
	    if (isize.exists(i)) osize.set(isize[i], i - offset + o * size);
	}
      } else eblerror("not implemented");
    }
    this->ninputs = isize.size();
    this->noutputs = osize.size();
    EDEBUG(this->name() << ": " << isize << " f-> " << osize);
    return osize;
  }

  template <typename T, class Tstate>
  mfidxdim narrow_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    EDEBUG(this->name() << ": " << osize << " b-> ...");
    // eblwarn("temporary no bpropsize in narrow");
    // return osize;
    mfidxdim isize;
    uint offset = offsets[0];
    for (uint i = 0; i < offset; ++i)
      isize.push_back_empty();
    isize.push_back(osize);
    for (uint i = offset + size; i < this->ninputs; ++i)
      isize.push_back_empty();
    EDEBUG(this->name() << ": " << osize << " b-> " << isize);
    return isize;
  }

  /////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  table_module<T,Tstate>::table_module(vector<intg> &tbl, intg tot,
				       const char *name_)
    : module_1_1<T,Tstate>(name_), table(tbl), total(tot) {
    this->bmstate_input = true; // this module takes multi-state inputs
    this->bmstate_output = true; // this module takes multi-state outputs
  }

  template <typename T, class Tstate>
  table_module<T,Tstate>::~table_module() {
  }

  template <typename T, class Tstate>
  void table_module<T,Tstate>::fprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    out.clear();
    for (uint i = 0; i < table.size(); ++i) {
      intg k = table[i];
      if (k < 0 || k >= (intg) in.size())
	eblerror("trying to access index " << k << " in inputs " << in);
      out.push_back(in[k]);
    }
    this->ninputs = in.size();
    this->noutputs = out.size();
    EDEBUG(this->name() << ": mapped " << in << " to " << out);
  }

  template <typename T, class Tstate>
  void table_module<T,Tstate>::bprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    EDEBUG(this->name() << " bprop: in: " << in);
    EDEBUG(this->name() << " bprop: out: " << out);
    // TODO: assign states back to their input location?
  }

  template <typename T, class Tstate>
  void table_module<T,Tstate>::bbprop(mstate<Tstate> &in, mstate<Tstate> &out){
    // TODO: assign states back to their input location?
  }

  template <typename T, class Tstate>
  std::string table_module<T,Tstate>::describe() {
    std::string s;
    s << "table_module " << this->name() << " with input list " << table;
    return s;
  }

  template <typename T, class Tstate>
  table_module<T,Tstate>* table_module<T,Tstate>::copy() {
    table_module<T,Tstate> *l2 =
      new table_module<T,Tstate>(table, total, this->name());
    return l2;
  }

  template <typename T, class Tstate>
  mfidxdim table_module<T,Tstate>::fprop_size(mfidxdim &isize) {
    mfidxdim osize;
    for (uint i = 0; i < table.size(); ++i) {
      intg k = table[i];
      if (k < 0 || k >= (intg) isize.size())
	eblerror("trying to access index " << k << " in inputs " << isize);
      osize.push_back(isize[k]);
    }
    return osize;
  }

  template <typename T, class Tstate>
  mfidxdim table_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    mfidxdim isize;
    uint n = total;
    for (uint i = 0; i < table.size(); ++i)
      if (table[i] + 1 > n) n = table[i] + 1;
    for (uint i = 0; i < n; ++i)
      isize.push_back_empty();
    for (uint i = 0; i < table.size(); ++i) {
      intg k = table[i];
      if (osize.exists(i)) isize.set(osize[i], k);
    }
    EDEBUG(this->name() << ": " << osize << " b-> " << isize);
    return isize;
  }

  //////////////////////////////////////////////////////////////////////////////
  // network sizes methods

  template <typename T, class Tstate>
  idxdim network_mindims(module_1_1<T,Tstate> &m, uint order) {
    idxdim d;
    for (uint i = 0; i < order; ++i)
      d.insert_dim(0, 1);
    fidxdim fd = d;
    mfidxdim mm;
    mm.push_back_new(fd);
    mm = m.bprop_size(mm);
    if (mm.exists(0)) d = mm[0];
    return d;
  }

} // end namespace ebl
