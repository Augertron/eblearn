/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
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

namespace ebl {

  // s2m_module ////////////////////////////////////////////////////////////////

  template <typename T, class Tin, class Tout>
  s2m_module<T,Tin,Tout>::s2m_module(uint nstates_, const char *name_)
    : module_1_1<T,Tin,Tout>(), _nstates(nstates_) {
    this->_name = name_;
    // this module takes single-state inputs and multi-state outputs
    this->bmstate_input = false;
    this->bmstate_output = true;
  }

  template <typename T, class Tin, class Tout>
  s2m_module<T,Tin,Tout>::~s2m_module() {}

  template <typename T, class Tin, class Tout>
  void s2m_module<T,Tin,Tout>::fprop(Tin &in, mstate<Tout> &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tin, class Tout>
  void s2m_module<T,Tin,Tout>::bprop(Tin &in, mstate<Tout> &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tin, class Tout>
  void s2m_module<T,Tin,Tout>::bbprop(Tin &in, mstate<Tout> &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tin, class Tout>
  mfidxdim s2m_module<T,Tin,Tout>::bprop_size(mfidxdim &osize) {
    return osize;
  }

  template <typename T, class Tin, class Tout>
  uint s2m_module<T,Tin,Tout>::nstates() {
    return _nstates;
  }

  template <typename T, class Tin, class Tout>
  void s2m_module<T,Tin,Tout>::resize_output(Tin &in, mstate<Tout> &out,
					     idxdim *d_) {
    idxdim d;
    if (d_) // use passed dimension to allocate states
      d = *d_;
    else { // use in's order with minimum dimensions of 1
      d = in.x.get_idxdim();
      d.setdims(1); // reset all existing dimensions to 1
    }
    // check we have the right number of states
    bool reset = false;
    if (out.size() != _nstates)
      reset = true;
    // check that all states have the right orders
    for (uint i = 0; i < out.size() && !reset; ++i) {
      Tout &o = out[i];
      if (o.x.get_idxdim() != d)
	reset = true;
    }
    // allocate
    if (reset) {
      out.clear();
      for (uint i = 0; i < nstates(); ++i)
	out.push_back(new Tin(d));
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // m2s_module

  template <typename T, class Tin, class Tout>
  m2s_module<T,Tin,Tout>::m2s_module(uint nstates_, const char *name_)
    : module_1_1<T,Tin,Tout>(), _nstates(nstates_) {
    this->_name = name_;
    // this module takes multi-state inputs and single-state outputs
    this->bmstate_input = true;
    this->bmstate_output = false;
  }

  template <typename T, class Tin, class Tout>
  m2s_module<T,Tin,Tout>::~m2s_module() {}

  template <typename T, class Tin, class Tout>
  void m2s_module<T,Tin,Tout>::fprop(mstate<Tin> &in, Tout &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tin, class Tout>
  void m2s_module<T,Tin,Tout>::bprop(mstate<Tin> &in, Tout &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tin, class Tout>
  void m2s_module<T,Tin,Tout>::bbprop(mstate<Tin> &in, Tout &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tin, class Tout>
  mfidxdim m2s_module<T,Tin,Tout>::bprop_size(mfidxdim &osize) {
    eblerror("not implemented");
    mfidxdim v;
    return v;
  }

  template <typename T, class Tin, class Tout>
  uint m2s_module<T,Tin,Tout>::nstates() {
    return _nstates;
  }


  // ms_module /////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  ms_module<T,Tstate>::ms_module(bool rep_ins, const char *name_)
    : module_1_1<T,Tstate>(name_), pipes(), replicate_inputs(rep_ins) {
    init();
  }

  template <typename T, class Tstate>
  ms_module<T,Tstate>::ms_module(module_1_1<T,Tstate> *pipe, uint n,
				 bool rep_ins, const char *name_)
    : module_1_1<T,Tstate>(name_), pipes(), replicate_inputs(rep_ins) {
    if (!pipe) eblerror("expected non-NULL pipe module");
    // replicate pipe
    for (uint i = 0; i < n; ++i)
      pipes.push_back(pipe);
    init();
  }

  template <typename T, class Tstate>
  ms_module<T,Tstate>::ms_module(std::vector<module_1_1<T,Tstate>*> &pipes_,
				 bool rep_ins, const char *name_)
    : module_1_1<T,Tstate>(name_), pipes(pipes_), replicate_inputs(rep_ins) {
    if (pipes.size() == 0)
      eblerror("expected non-empty vector of pipe modules");
    init();
  }

  template <typename T, class Tstate>
  ms_module<T,Tstate>::~ms_module() {
    module_1_1<T,Tstate> *p0 = pipes[0];
    if (p0) delete p0;
    for (uint i = 1; i < pipes.size(); ++i) {
      module_1_1<T,Tstate> *p = pipes[i];
      if (p && p != p0) delete p;
    }
  }

  // multi-multi input/outputs /////////////////////////////////////////////////

  template <typename T, class Tstate>
  void ms_module<T,Tstate>::fprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    init_fprop(in, out);
    EDEBUG(this->name() << ": in " << in << " ins " << ins
	  << " used_pipes " << used_pipes);
    // fprop ins
    for (uint i = 0; i < used_pipes.size(); ++i) {
      module_1_1<T,Tstate> *p = used_pipes[i];
      if (!p) { // no pipe, just pass data along
	EDEBUG("fprop empty pipe, just passing input " << ins[i]);
	out.push_back(ins[i]);
	if (i >= pipes_noutputs.size()) pipes_noutputs.push_back(ins[i].size());
	else pipes_noutputs[i] = ins[i].size();
      } else {
	mstate<Tstate> b;
	EDEBUG("fprop " << p->name() << " with input " << ins[i]);
	p->fprop(ins[i], b);
	out.push_back_new(b);
	if (i >= pipes_noutputs.size()) pipes_noutputs.push_back(b.size());
	else pipes_noutputs[i] = b.size();
	EDEBUG(p->name() << " in " << ins[i] << " out " << b);
      }
      EDEBUG(this->name() << ": current outputs: " << out);
      // TODO: code below reuses buffers but is a problem when this module
      // is called multiple times, effectively erasing previous results
      // -> question: is code above slow? if not, remove this block.
      // used_pipes[i]->fprop(ins[i], mbuffers[i]);
      // out.push_back(mbuffers[i]);
      // pipes_noutputs[i] = mbuffers[i].size();
    }
    // remember number of outputs
    this->noutputs = out.size();
    EDEBUG(this->name() << ": " << in << " -> " << out);
  }

  template <typename T, class Tstate>
  void ms_module<T,Tstate>::bprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    uint off = out.size();
    for (int i = (int) used_pipes.size() - 1; i >= 0; --i) {
      module_1_1<T,Tstate> *p = used_pipes[i];
      off -= pipes_noutputs[i];
      if (!p) { // no pipe, just pass data along
	EDEBUG("bprop empty pipe doing nothing.");
      } else {
	mstate<Tstate> b = out.narrow(pipes_noutputs[i], (uint) off);
	EDEBUG("bprop " << p->name() << " in " << ins[i] << " out " << b);
	p->bprop(ins[i], b);
      }
    }
      //used_pipes[i]->bprop(ins[i], out[i]);
    //      used_pipes[i]->bprop(ins[i], mbuffers[i]);
  }

  template <typename T, class Tstate>
  void ms_module<T,Tstate>::bbprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    int off = out.size();
    for (int i = (int) used_pipes.size() - 1; i >= 0; --i) {
      module_1_1<T,Tstate> *p = used_pipes[i];
      off -= pipes_noutputs[i];
      if (!p) { // no pipe, just pass data along
	EDEBUG("bbprop empty pipe doing nothing.");
      } else {
	mstate<Tstate> b = out.narrow(pipes_noutputs[i], (uint) off);
	p->bbprop(ins[i], b);
      }
    }
    //used_pipes[i]->bbprop(ins[i], out[i]);
    //      used_pipes[i]->bbprop(ins[i], mbuffers[i]);
  }

  template <typename T, class Tstate>
  void ms_module<T,Tstate>::dump_fprop(mstate<Tstate> &in, mstate<Tstate> &out){
    // remember number of input
    this->ninputs = in.size();
    ins.clear();
    out.clear();
    // replicate input states if only 1 state but multiple pipes
    // or if manually requested with replicate_inputs
    if ((in.size() == 1 && pipes.size() > 1) || replicate_inputs)
      for (uint i = 0; i < pipes.size(); ++i) ins.push_back(in);
    else ins.push_back(in); // just feed in to pipes
    EDEBUG("input " << in << " -> " << ins << " in " << this->name());
    // allocate buffers
    if (pipes.size() > 0) mbuffers.resize_default(pipes.size());
    // fprop ins
    for (uint i = 0; i < pipes.size(); ++i) {
      pipes[i]->dump_fprop(ins[i], mbuffers[i]);
      out.push_back(mbuffers[i]);
    }
    // remember number of outputs
    this->noutputs = out.size();
    EDEBUG(this->name() << ": " << in << " -> " << out);
  }

  // forget methods ////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void ms_module<T,Tstate>::forget(forget_param_linear& fp) {
    for (uint i = 0; i < pipes.size(); ++i)
      if (pipes[i]) pipes[i]->forget(fp);
  }

  //////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  fidxdim ms_module<T,Tstate>::fprop_size(fidxdim &isize) {
    fidxdim d = isize;
    // use 1st pipe as base size
    module_1_1<T,Tstate> *p0 = pipes[0];
    d = p0->fprop_size(d);
    return d;
  }

  template <typename T, class Tstate>
  mfidxdim ms_module<T,Tstate>::fprop_size(mfidxdim &isize) {
    this->ninputs = isize.size();
    mfidxdim osize;
    for (uint i = 0; i < pipes.size(); ++i) {
      mfidxdim o;
      if (replicate_inputs || (isize.size() == 1 && pipes.size() > 1))
	o = isize;
      else {
	if (isize.exists(i)) o.push_back(isize[i]);
	else o.push_back_empty();
      }
      if (pipes[i]) o = pipes[i]->fprop_size(o);
      osize.push_back_new(o);
    }
    this->noutputs = osize.size();
    EDEBUG(this->name() << " (" << this->noutputs << " outputs): "
	  << isize << " f-> " << osize);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim ms_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    fidxdim d = osize;
    vector<fidxdim> os;
    os.assign(pipes.size(), d);
    if (os.size() != pipes.size())
      eblerror("expected same size vector of idxdim as pipes but found "
	       << os.size() << " and " << pipes.size());
    // bprop in all pipes
    for (uint i = 0; i < pipes.size(); ++i) {
      module_1_1<T,Tstate> *p = pipes[i];
      fidxdim &pi = os[i];
      if (p) pi = p->bprop_size(pi);
    }
    return d;
  }

  template <typename T, class Tstate>
  mfidxdim ms_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    EDEBUG(this->name() << " (ms, " << this->noutputs << " outputs): "
	  << osize << " b-> ...");
    mfidxdim isize;
    // we replicated the input for each pipe, so fuse inputs into 1
    if (replicate_inputs) {
      //uint cap = std::max((uint) pipes.size(), this->ninputs);
      uint sz = osize.size() / pipes.size();
      for (uint i = 0; i < pipes.size(); i++) {
	mfidxdim o1;
	for (uint j = 0; j < sz; ++j)
	  if (osize.exists(j + i * sz)) o1.push_back_new(osize[j + i * sz]);
	  else o1.push_back_empty();
	module_1_1<T,Tstate> *p = pipes[i];
	mfidxdim i1 = o1;
	if (p) i1 = p->bprop_size(o1);
	// fuse inputs from different pipes into 1
	//EDEBUG(this->name() << ": noutputs " << this->noutputs);
	//	if (i == 0 || this->noutputs != osize.size()) // first pipe, just add
	if (i == 0) // first pipe, just add
	  isize.push_back(i1);
	else {
	  for (uint j = 0; j < i1.size(); ++j) {
	    if (!isize.exists(j) && i1.exists(j)) { // fuse if empty
	      if (j >= isize.size())
		isize.push_back(i1[j]);
	      else
		isize.set_new(i1[j], j);
	    }
	  }
	}
      }
    } else { // bprop in all pipes
      uint j = 0;
      for (uint i = 0; i < pipes.size(); i++) {
	module_1_1<T,Tstate> *p = pipes[i];
	mfidxdim pi = osize;
	if (p) {
	  // narrow to number of outputs
	  pi = osize.narrow_copy(p->get_noutputs(), j);
	  pi = p->bprop_size(pi);
	  j += p->get_noutputs();
	}
	isize.push_back(pi);
      }
    }
    EDEBUG(this->name() << ": " << osize << " b-> " << isize);
    return isize;
  }

  // printing //////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  std::string ms_module<T,Tstate>::pretty(idxdim &isize) {
    std::string s;
    idxdim is(isize);
    fidxdim fis(isize);
    s << pipes[0]->pretty(is);
    fis = pipes[0]->fprop_size(fis);
    return s;
  }

  template <typename T, class Tstate>
  std::string ms_module<T,Tstate>::pretty(mfidxdim &isize) {
    std::string s;
    mfidxdim is(isize);
    s << "\n";
    for (uint i = 0; i < pipes.size(); ++i) {
      mfidxdim j;
      int id = std::min((int) i, (int) is.size() - 1);
      if (replicate_inputs) j.push_back(is);
      else if (is.exists(id)) j.push_back(is[id]);
      s << j << " -> " << this->name() << " pipe " << i;
      if (!pipes[i]) s << ": empty pipe (just passing data)\n";
      else s << pipes[i]->pretty(j) << "\n";
    }
    return s;
  }

  template <typename T, class Tstate>
  std::string ms_module<T, Tstate>::describe() {
    std::string desc;
    desc << "ms module " << this->name();
    desc << ", pipes ";
    for (uint i = 0; i < pipes.size(); ++i) {
      if (!pipes[i]) desc << "empty pipe";
      else desc << pipes[i]->name();
      if (i != pipes.size() - 1) desc << ", ";
    }
    if (switches.size() > 0)
      desc << ", switching based on input sizes: " << switches;
    desc << ":\n";
    for (uint i = 0; i < pipes.size(); ++i) {
      desc << this->name() << " pipe[" << i << "]: ";
      if (!pipes[i]) desc << "empty pipe (just passing data)\n";
      else desc << pipes[i]->describe(1) << "\n";
    }
    return desc;
  }

  // accessors /////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void ms_module<T, Tstate>::set_switch(midxdim &s) {
    switches = s;
  }

  template <typename T, class Tstate>
  void ms_module<T, Tstate>::set_switch(intg id) {
    EDEBUG("switching scale to " << id);
    bindex = true;
    switch_id = id;
  }

  template <typename T, class Tstate>
  uint ms_module<T, Tstate>::npipes() {
    return (uint) pipes.size();
  }

  template <typename T, class Tstate>
  module_1_1<T,Tstate>* ms_module<T, Tstate>::get_pipe(uint i) {
    if (i > pipes.size())
      eblerror("cannot access pipe " << i << ", only "
	       << pipes.size() << "pipes");
    return pipes[i];
  }

  template <typename T, class Tstate>
  module_1_1<T,Tstate>* ms_module<T, Tstate>::last_module() {
    if (pipes.size())
      return pipes[0]->last_module();
    return this;
  }

  // protected methods /////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  void ms_module<T,Tstate>::init() {
    // this module takes multi-state inputs and multi-state outputs
    this->bmstate_input = true;
    this->bmstate_output = true;
    // check that modules exist
    for (uint i = 0; i < pipes.size(); ++i)
      if (pipes[i] == NULL)
	eblwarn(this->name() << ": pipe " << i
		<< " is empty (just passing data long)");
    // initialize actually used pipes
    used_pipes = pipes;
    // resize
    if (used_pipes.size() != pipes_noutputs.size())
      pipes_noutputs.resize(used_pipes.size());
    // switching
    bindex = false;
    switch_id = -1;
  }

  template <typename T, class Tstate>
  void ms_module<T,Tstate>::init_fprop(mstate<Tstate> &in,mstate<Tstate> &out) {
    // remember number of input
    this->ninputs = in.size();
    ins.clear();
    out.clear();
    // switch used pipes if switches are defined
    switch_pipes(in);
    // replicate input states if only 1 state but multiple pipes
    // or if manually requested with replicate_inputs
    if ((in.size() == 1 && used_pipes.size() > 1) || replicate_inputs)
      for (uint i = 0; i < used_pipes.size(); ++i) ins.push_back_new(in);
    else if (in.size() == used_pipes.size())
      for (uint i = 0; i < used_pipes.size(); ++i) {
	mstate<Tstate> tin;
	tin.push_back_new(in[i]);
	ins.push_back_new(tin);
      }
    else ins.push_back_new(in); // just feed in to pipes
    EDEBUG("input " << in << " -> " << ins << " for used_pipes "
	  << used_pipes << " in " << this->name());
    // allocate buffers
    if (used_pipes.size() != mbuffers.size())
      mbuffers.resize_default(used_pipes.size());
    if (used_pipes.size() != pipes_noutputs.size())
      pipes_noutputs.resize(used_pipes.size());
  }

  template <typename T, class Tstate>
  void ms_module<T,Tstate>::switch_pipes(mstate<Tstate> &in) {
    // if switching is on, limit to only 1 pipe
    intg id = -1;
    // determine switch id
    if (bindex) // switching based on given pipe index
      id = switch_id;
    else if (switches.size() > 0) {
      // find the switch index matching the first input size
      idxdim d0 = in[0].x.get_idxdim();
      d0.remove_dim(0);
      bool found = false;
      for (intg i = 0; i < (intg) switches.size(); ++i)
	if (switches[i] == d0) {
	  id = i;
	  found = true;
	  break ;
	}
      if (!found)
	eblerror("switch with dimensions " << d0 << " not found in "<<switches);
    }
    // actual switch
    if (id >= 0) {
      used_pipes.clear();
      used_pipes.push_back(pipes[id]);
      EDEBUG("switching to pipe " << id << " (input is " << in[0].x << ")");
    } else used_pipes = pipes;
  }

  //////////////////////////////////////////////////////////////////////////////
  // msc_module

  template <typename T, class Tstate>
  msc_module<T,Tstate>::msc_module
  (std::vector<module_1_1<T,Tstate>*> &pipes_, uint nsize_, uint stride_,
   uint nsize2_, const char *name_)
    : ms_module<T,Tstate>(pipes_, false, name_), nsize(nsize_), stride(stride_),
      nsize2(nsize2_) {
    if (nsize2 == 0) nsize2 = nsize;
    if (nsize2 < nsize) eblerror("nsize2 must be greater or equal to nsize");
  }

  template <typename T, class Tstate>
  msc_module<T,Tstate>::~msc_module() {
  }

  //////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  fidxdim msc_module<T,Tstate>::fprop_size(fidxdim &isize) {
    fidxdim d = isize;
    // use 1st pipe as base size
    module_1_1<T,Tstate> *p0 = pipes[0];
    d = p0->fprop_size(d);
    return d;
  }

  template <typename T, class Tstate>
  fidxdim msc_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    fidxdim d = osize;
    vector<fidxdim> os;
    os.assign(pipes.size(), d);
    if (os.size() != pipes.size())
      eblerror("expected same size vector of idxdim as pipes but found "
	       << os.size() << " and " << pipes.size());
    // bprop in all pipes
    for (uint i = 0; i < pipes.size(); ++i) {
      module_1_1<T,Tstate> *p = pipes[i];
      fidxdim &pi = os[i];
      pi = p->bprop_size(pi);
    }
    return d;
  }

  template <typename T, class Tstate>
  mfidxdim msc_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    EDEBUG(this->name() << " (msc): " << osize << " b-> ...");
    if (osize.size() != this->noutputs)
      eblerror("expected osize to have " << this->noutputs << " elements but"
	       << " it has only: " << osize);
    mfidxdim isize(this->ninputs);
    uint i = 0, o = 0, off = 0;
    // loop on high-level groups
    uint n2 = this->ninputs / nsize2;
    for (uint j = 0; j < n2; ++j) {
      // loop on low-level groups
      for (uint k = 0; k < nsize2 - nsize + 1; ++k) {
	mfidxdim md;
	if (o >= pipes_noutputs.size())
	  eblerror("expected at least " << o + 1 << " outputs sizes but got "
		   << pipes_noutputs);
	// loop on number of outputs for this pipe
	for (uint l = 0; l < pipes_noutputs[o]; ++l, ++off) {
	  if (osize.exists(off)) md.push_back(osize[off]);
	  else md.push_back_empty();
	}
	md = pipes[std::min(pipes.size() - 1, (size_t)o)]->bprop_size(md);
	// copy all inputs to size
	for (uint l = 0; l < md.size(); ++l)
	  if (md.exists(l)) isize.set(md[l], l + i);
	o++;
	i++;
      }
      i += nsize - 1;
      // // fill remaining inputs before next group with empty elements
      // for (uint k = 0; k < nsize - 1; ++k)
      // 	isize.push_back_empty();
    }
    EDEBUG(this->name() << ": " << osize << " b-> " << isize);
    return isize;
  }

template <typename T, class Tstate>
  std::string msc_module<T, Tstate>::describe() {
    std::string desc;
    desc << "msc module " << this->name();
    desc << " with nsize " << nsize << ", stride " << stride << ", nsize2 "
	 << nsize2 << ", pipes ";
    for (uint i = 0; i < pipes.size(); ++i)
      desc << pipes[i]->name() << ", ";
    desc << ":\n";
    for (uint i = 0; i < pipes.size(); ++i)
      desc << this->name() << " pipe[" << i << "]: "
	   << pipes[i]->describe(1);
    return desc;
  }

  //////////////////////////////////////////////////////////////////////////////
  // protected methods

  template <typename T, class Tstate>
  void msc_module<T,Tstate>::init_fprop(mstate<Tstate> &in,mstate<Tstate> &out){
    // remember number of input
    this->ninputs = in.size();
    // in has to have at least nsize states
    if (in.size() < nsize)
      eblerror("input must have at least " << nsize << " states but has only "
	       << in.size());
    if (in.size() % nsize2 != 0)
      eblerror("expected input to be multiple of " << nsize2 << " but got "
	       << in.size() << " states instead");
    ins.clear();
    out.clear();
    used_pipes.clear();
    mstate<Tstate> mi; // buffers
    uint o = 0; // number of output groups
    // pipes
    for (uint i = 0; i < pipes.size(); ++i) {
      if (!pipes[i]->mstate_input())
	eblerror("expected module with multi-inputs capabilities");
      // loop on high-level groups
      uint n2 = in.size() / nsize2;
      for (uint j = 0; j < n2; ++j) {
	// loop on low-level groups
	for (uint k = 0; k < nsize2 - nsize + 1; ++k) {
	  // create buffer with nsize states
	  mi = in.narrow(nsize, k + j * nsize2);
	  // TODO: temporarly lock elements manually
	  // pb: operator= when returning narrow doesnt lock elements,
	  // if defining it, then assigning to a reference gives
	  // negative references counters (?).
	  mi.lock_all();
	  used_pipes.push_back(pipes[i]);
	  ins.push_back_new(mi);
	  o++;
	}
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // arch find

  template <typename T, class Tstate, class Tcast>
  Tcast* arch_find(module_1_1<T,Tstate> *m, Tcast *c) {
    Tcast *ret = dynamic_cast<Tcast*>(m);
    if (ret) return ret;
    layers<T,Tstate> *l = dynamic_cast<layers<T,Tstate>*>(m);
    if (l) return arch_find(l, c);
    ms_module<T,Tstate> *ms = dynamic_cast<ms_module<T,Tstate>*>(m);
    if (ms) return arch_find(ms, c);
    return NULL;
  }

  template <typename T, class Tstate, class Tcast>
  Tcast* arch_find(layers<T,Tstate> *m, Tcast *c) {
    if (!m) return NULL;
    for (uint i = 0; i < m->modules.size(); ++i) {
      module_1_1<T, Tstate, Tstate>* mo = m->modules[i];
      Tcast *ret = arch_find(mo, c);
      if (ret) return ret;
    }
    return NULL; // not found
  }

  template <typename T, class Tstate, class Tcast>
  Tcast* arch_find(ms_module<T,Tstate> *m, Tcast *c) {
    if (!m) return NULL;
    Tcast *ret = NULL;
    for (uint i = 0; i < m->pipes.size(); ++i) {
      ret = arch_find(m->pipes[i], c);
      if (ret) return ret;
    }
    return NULL;
  }

  /////////////////////////////////////////////////////////////////////////////
  // arch find_all

  template <typename T, class Tstate, class Tcast>
  std::vector<Tcast*> arch_find_all(module_1_1<T,Tstate> *m, Tcast *c,
				    std::vector<Tcast*> *v) {
    std::vector<Tcast*> v2;
    std::vector<Tcast*> *all = &v2;
    if (v) all = v;
    Tcast *ret = dynamic_cast<Tcast*>(m);
    if (ret) all->push_back(ret);
    layers<T,Tstate> *l = dynamic_cast<layers<T,Tstate>*>(m);
    if (l) return arch_find_all(l, c, all);
    ms_module<T,Tstate> *ms = dynamic_cast<ms_module<T,Tstate>*>(m);
    if (ms) return arch_find_all(ms, c, all);
    return *all;
  }

  template <typename T, class Tstate, class Tcast>
  std::vector<Tcast*> arch_find_all(layers<T,Tstate> *m, Tcast *c,
				    std::vector<Tcast*> *v) {
    std::vector<Tcast*> v2;
    std::vector<Tcast*> *all = &v2;
    if (v) all = v;
    if (!m) return *all;
    for (uint i = 0; i < m->modules.size(); ++i) {
      module_1_1<T, Tstate, Tstate>* mo = m->modules[i];
      *all = arch_find_all(mo, c, all);
    }
    return *all;
  }

  template <typename T, class Tstate, class Tcast>
  std::vector<Tcast*> arch_find_all(ms_module<T,Tstate> *m, Tcast *c,
				    std::vector<Tcast*> *v) {
    std::vector<Tcast*> v2;
    std::vector<Tcast*> *all = &v2;
    if (v) all = v;
    if (!m) return *all;
    for (uint i = 0; i < m->pipes.size(); ++i)
      *all = arch_find_all(m->pipes[i], c, all);
    return *all;
  }

  /////////////////////////////////////////////////////////////////////////////
  // arch narrow

  template <typename T, class Tstate, class Tcast>
  module_1_1<T,Tstate>* arch_narrow(module_1_1<T,Tstate> *m, Tcast *c,
				    bool included, bool post, bool *found) {
    if (!m) return NULL;
    Tcast *ret = dynamic_cast<Tcast*>(m);
    if (ret == c) { // this module matches the target type Tcast*
      if (found) *found = true;
      if (included) return m->copy(); // include found module
      else return NULL; // do not include found module
    }
    layers<T,Tstate> *l = dynamic_cast<layers<T,Tstate>*>(m);
    if (l) return (module_1_1<T,Tstate>*) arch_narrow(l, c, included, post, found);
    ms_module<T,Tstate> *ms = dynamic_cast<ms_module<T,Tstate>*>(m);
    if (ms) return (module_1_1<T,Tstate>*) arch_narrow(ms, c, included, post, found);
    // postfix narrow and haven't found pivot module yet, do not add
    if (!*found && post) return NULL;
    // prefix narrow and haven't found pivot module yet, add it
    return m->copy();
  }

  template <typename T, class Tstate, class Tcast>
  layers<T,Tstate>* arch_narrow(layers<T,Tstate> *m, Tcast *c,
				bool included, bool post, bool *found) {
    if (!m) return NULL;
    bool lfound = false;
    if (!found) found = &lfound;
    layers<T,Tstate> *m2 = new layers<T,Tstate>(true, m->name());
    for (uint i = 0; i < m->modules.size(); ++i) {
      module_1_1<T,Tstate,Tstate>* mo = m->modules[i];
      mo = arch_narrow(mo, c, included, post, found);
      if (*found) {
	if (post) {
	  // we want postfix copy and have reached pivot module, add current
	  if (mo) m2->add_module(mo);
	} else return m2; // prefix, reach module, stop and return
      } else {
	// we want prefix copy and haven't reached pivot module yet, add current
	if (!post && mo) m2->add_module(mo);
      }
    }
    return m2;
  }

  template <typename T, class Tstate, class Tcast>
  ms_module<T,Tstate>* arch_narrow(ms_module<T,Tstate> *m, Tcast *c,
				   bool included, bool post, bool *found) {
    if (!m) return NULL;
    bool lfound = false;
    if (!found) found = &lfound;
    ms_module<T,Tstate> *m2 =
      new ms_module<T,Tstate>(m->replicate_inputs, m->name());
    for (uint i = 0; i < m->pipes.size(); ++i) {
      module_1_1<T,Tstate> *p2 = NULL;
      if (m->pipes[i]) p2 = arch_narrow(m->pipes[i], c, included, post, found);
      m2->pipes.push_back(p2);
      if (*found) {
	m2->pipes.clear();
	m2->pipes.push_back(p2);
	return m2;
      }
    }
    return m2;
  }

} // end namespace ebl
