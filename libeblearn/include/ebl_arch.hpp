
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

#include <vector>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // module_1_1

  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>::module_1_1(bool bResize_)
    : bResize(bResize_) { 
  }

  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>::~module_1_1() {
  }

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
  void module_1_1<T,Tin,Tout>::forget(forget_param_linear& fp) { 
  }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::normalize() {
  }

  template <typename T, class Tin, class Tout>
  int module_1_1<T,Tin,Tout>::replicable_order() { return -1; }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::resize_output(Tin &in, Tout &out) { 
    err_not_implemented(); }

  template <typename T, class Tin, class Tout>
  idxdim module_1_1<T,Tin,Tout>::fprop_size(idxdim &isize) {
    return isize;
  }

  template <typename T, class Tin, class Tout>
  idxdim module_1_1<T,Tin,Tout>::bprop_size(const idxdim &osize) {
    return osize;
  }

  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::pretty(idxdim &isize) {
    cout << " -> " << fprop_size(isize);
  }
  
  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>* module_1_1<T,Tin,Tout>::copy() {
    eblerror("deep copy not implemented for this module");
    return NULL;
  }
  
  ////////////////////////////////////////////////////////////////
  // module_2_1

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::fprop(Tin1 &in1, Tin2 &in2, Tout &out) { 
    err_not_implemented(); }

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::bprop(Tin1 &in1, Tin2 &in2, Tout &out) { 
    err_not_implemented(); }

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::bbprop(Tin1 &in1, Tin2 &in2, Tout &out){ 
    err_not_implemented(); }

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::forget(forget_param &fp) {
    err_not_implemented(); }

  template <typename T, class Tin1, class Tin2, class Tout>
  void module_2_1<T,Tin1,Tin2,Tout>::normalize() { err_not_implemented(); }

  ////////////////////////////////////////////////////////////////
  // ebm_1

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
  // ebm_2

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
  layers<T,Tstate>::layers(bool oc, Tstate *hi_, Tstate *ho_)
    : hi(hi_), ho(ho_), htmp(NULL), hi0(hi_), ho0(ho_) {
    mem_optimization = false;
    modules = new std::vector< module_1_1<T, Tstate>* >();
    hiddens = new std::vector< Tstate* >();
    this->own_contents = oc;
    // if we use dual buffers for memory optimization, we can't bprop.
    if (hi0 && ho0)
      mem_optimization = true;
  }

  // Clean vectors. Module doesn't have ownership of sub-modules
  template <typename T, class Tstate>
  layers<T,Tstate>::~layers() {
    if (this->own_contents){
      for(unsigned int i=0;i<modules->size(); i++){
    	delete (*modules)[i];
      }
      for(unsigned int i=0;i<hiddens->size(); i++){
    	delete (*hiddens)[i];
      }
    }
    delete modules;
    delete hiddens;
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::add_module(module_1_1<T, Tstate, Tstate>* module) {
    modules->push_back(module);
    hiddens->push_back(NULL);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::fprop(Tstate& in, Tstate& out){
    if (modules->empty())
      eblerror("trying to fprop through empty layers");
    if (mem_optimization) { // use 2 buffers for all modules
      // initialize buffers
      hi = hi0;
      ho = ho0;
      // loop over modules
      for(uint i = 0; i < modules->size(); i++){
	// run module
	(*modules)[i]->fprop(*hi,*ho);
	// swap buffers
	swap_buffers();
      }
      // if output is in input buffer, copy data into output
      // TODO: can we avoid this?
      if (ho != ho0) { // the output is in in, copy it
	ho0->resize(hi0->x.get_idxdim());
	idx_copy(hi->x, ho->x);
      }
    } else { // use one independent buffer between each module
      // initialize buffers
      hi = &in;
      ho = &out;
      // loop over modules
      for(uint i = 0; i < modules->size(); i++){
	// if last module, output into out
	if (i == modules->size() - 1)
	  ho = &out;
	else { // not last module, use hidden buffers
	  ho = (Tstate*)(*hiddens)[i];
	  // allocate hidden buffer if necessary
	  if (ho == NULL) {
#ifdef __DEBUG__
	    cout << "Allocating state_idx buffer: "<<hi->x.get_idxdim() << endl;
#endif
	    // create idxdim of same order but sizes 1
	    idxdim d = hi->x.get_idxdim();
	    for (int k = 0; k < d.order(); ++k)
	      d.setdim(k, 1);
	    // assign buffer
	    (*hiddens)[i] = new Tstate(d);
	    ho = (Tstate*)(*hiddens)[i];
	  }
	}
	// run module
	(*modules)[i]->fprop(*hi,*ho);
	hi = ho;
      }
    }
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bprop(Tstate& in, Tstate& out){
    if (mem_optimization)
      eblerror("cannot bprop while using dual-buffer memory optimization");
    if (modules->empty())
      eblerror("trying to bprop through empty layers");

    // clear hidden states
    for (unsigned int i=0; i<hiddens->size(); i++){
      if ((*hiddens)[i])
	(*hiddens)[i]->clear_dx();
    }
    hi = &out;
    ho = &out;

    // last will be manual
    int niter = modules->size()-1;
    for(int i=niter; i>0; i--){
      hi = (*hiddens)[i-1];
      (*modules)[i]->bprop(*hi,*ho);
      ho = hi;
    }
    (*modules)[0]->bprop(in,*ho);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bbprop(Tstate& in, Tstate& out){
    if (mem_optimization)
      eblerror("cannot bbprop while using dual-buffer memory optimization");
    if (modules->empty())
      eblerror("trying to bbprop through empty layers");

    // clear hidden states
    for (unsigned int i=0; i<hiddens->size(); i++){
      if ((*hiddens)[i])
	(*hiddens)[i]->clear_ddx();
    }

    hi = &out;
    ho = &out;

    // last will be manual
    int niter = modules->size()-1;
    for(int i=niter; i>0; i--){
      hi = (*hiddens)[i-1];
      (*modules)[i]->bbprop(*hi,*ho);
      ho = hi;
    }
    (*modules)[0]->bbprop(in,*ho);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::forget(forget_param_linear& fp){
    if (modules->empty())
      eblerror("trying to forget through empty layers");

    for(unsigned int i=0; i<modules->size(); i++){
      module_1_1<T,Tstate,Tstate> *tt = (*modules)[i];
      tt->forget(fp);
    }
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::normalize(){
    if (modules->empty())
      eblerror("trying to normalize through empty layers");

    for(unsigned int i=0; i<modules->size(); i++){
      (*modules)[i]->normalize();
    }
  }

  //! This method modifies i_size to be compliant with the architecture of 
  //! the module. It also returns the output size corresponding to the new
  //! input...
  template <typename T, class Tstate>
  idxdim layers<T,Tstate>::fprop_size(idxdim &isize) {
    idxdim os(isize);
    //! Loop through all the layers of the module, and update output
    //! size accordingly.
    for (unsigned int i = 0; i < modules->size(); i++) {
      module_1_1<T,Tstate,Tstate> *tt = (*modules)[i];
      os = tt->fprop_size(os);
    }
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(os);
    return os;
  }

  //! This method computes the input size of the module for a given output
  //! size.
  template <typename T, class Tstate>
  idxdim layers<T,Tstate>::bprop_size(const idxdim &osize) {
    idxdim isize(osize);
    //! Loop through all the layers of the module, from the end to the beg.
    for (int i = (int) modules->size() - 1; i >= 0; i--) {
      module_1_1<T,Tstate,Tstate> *tt = (*modules)[i];
      isize = tt->bprop_size(isize);
    }
    return isize;
  }

  template <typename T, class Tstate>
  layers<T,Tstate>* layers<T,Tstate>::copy() {
    layers<T,Tstate> *l2 = new layers<T,Tstate>(true);
    //! Loop through all the modules and buffers and copy them
    int niter = this->modules->size();
    for(int i = 0; i < niter; i++) {
      l2->add_module((module_1_1<T,Tstate>*)(*this->modules)[i]->copy());
      if ((*this->hiddens)[i] != NULL) {
	(*l2->hiddens)[i] =
	  new Tstate((*this->hiddens)[i]->x.get_idxdim());
	idx_copy((*this->hiddens)[i]->x, (*l2->hiddens)[i]->x);
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
  void layers<T,Tstate>::pretty(idxdim &isize) {
    idxdim is(isize);
    cout << is;
    //! Loop through all the layers of the module, and update output
    //! size accordingly.
    for (unsigned int i = 0; i < modules->size(); i++) {
      module_1_1<T,Tstate> *tt = (*modules)[i];
      tt->pretty(is);
      is = tt->fprop_size(is);
    }
    cout << endl;
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
  idxdim layers_2<T,Tin,Thid,Tout>::fprop_size(idxdim &isize) {
    idxdim os(isize);
    os = layer1.fprop_size(os);
    os = layer2.fprop_size(os);
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(os);
    return os;
  }

  template <typename T, class Tin, class Thid, class Tout>
  idxdim layers_2<T,Tin,Thid,Tout>::bprop_size(const idxdim &osize) {
    idxdim isize(osize);
    isize = layer2.bprop_size(isize);
    isize = layer1.bprop_size(isize);
    return isize;
  }  

  template <typename T, class Tin, class Thid, class Tout>
  void layers_2<T,Tin,Thid,Tout>::pretty(idxdim &isize) {
    idxdim is(isize);
    layer1.pretty(is);
    cout << " -> ";
    is = layer1.fprop_size(is);
    layer2.pretty(is);
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
      eblerror("input order must be greater or equal to module's operating \
order");
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
      eblerror("the order of the input should be greater or equal to module's\
 operating order");
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
  
} // end namespace ebl
