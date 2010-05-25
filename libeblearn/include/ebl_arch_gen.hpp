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
  // module_1_1_gen

  template<class Tin, class Tout>
  module_1_1_gen<Tin,Tout>::module_1_1_gen(bool bResize_) : bResize(bResize_) { 
  }

  template<class Tin, class Tout>
  void module_1_1_gen<Tin,Tout>::fprop(Tin &in, Tout &out) { 
    err_not_implemented(); }

  template<class Tin, class Tout>
  void module_1_1_gen<Tin,Tout>::bprop(Tin &in, Tout &out) { 
    err_not_implemented(); }

  template<class Tin, class Tout>
  void module_1_1_gen<Tin,Tout>::bbprop(Tin &in, Tout &out) { 
    err_not_implemented(); }

  template<class Tin, class Tout>
  void module_1_1_gen<Tin,Tout>::forget(forget_param_linear& fp) { 
  }

  template<class Tin, class Tout>
  void module_1_1_gen<Tin,Tout>::normalize() {
  }

  template<class Tin, class Tout>
  int module_1_1_gen<Tin,Tout>::replicable_order() { return -1; }

  template<class Tin, class Tout>
  void module_1_1_gen<Tin,Tout>::resize_output(Tin &in, Tout &out) { 
    err_not_implemented(); }

  template<class Tin, class Tout>
  idxdim module_1_1_gen<Tin,Tout>::fprop_size(idxdim &isize) {
    return isize;
  }

  template<class Tin, class Tout>
  idxdim module_1_1_gen<Tin,Tout>::bprop_size(const idxdim &osize) {
    return osize;
  }

  template<class Tin, class Tout>
  void module_1_1_gen<Tin,Tout>::pretty(idxdim &isize) {
    cout << " -> " << fprop_size(isize);
  }
  
  template<class Tin, class Tout>
  module_1_1_gen<Tin, Tout>* module_1_1_gen<Tin,Tout>::copy() {
    eblerror("deep copy not implemented for this module");
    return NULL;
  }
  
  ////////////////////////////////////////////////////////////////
  // module_2_1_gen

  template<class Tin1, class Tin2, class Tout>
  void module_2_1_gen<Tin1,Tin2,Tout>::fprop(Tin1 &in1, Tin2 &in2, Tout &out) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class Tout>
  void module_2_1_gen<Tin1,Tin2,Tout>::bprop(Tin1 &in1, Tin2 &in2, Tout &out) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class Tout>
  void module_2_1_gen<Tin1,Tin2,Tout>::bbprop(Tin1 &in1, Tin2 &in2, Tout &out){ 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class Tout>
  void module_2_1_gen<Tin1,Tin2,Tout>::forget(forget_param &fp) {
    err_not_implemented(); }

  template<class Tin1, class Tin2, class Tout>
  void module_2_1_gen<Tin1,Tin2,Tout>::normalize() { err_not_implemented(); }

  ////////////////////////////////////////////////////////////////
  // ebm_1_gen

  template<class Tin, class T>
  void ebm_1_gen<Tin, T>::fprop(Tin &in, state_idx<T> &energy) {
    err_not_implemented(); }

  template<class Tin, class T>
  void ebm_1_gen<Tin, T>::bprop(Tin &in, state_idx<T> &energy) {
    err_not_implemented(); }

  template<class Tin, class T>
  void ebm_1_gen<Tin, T>::bbprop(Tin &in, state_idx<T> &energy) {
    err_not_implemented(); }

  template<class Tin, class T>
  void ebm_1_gen<Tin, T>::forget(forget_param &fp) {
    err_not_implemented(); }

  template<class Tin, class T>
  void ebm_1_gen<Tin, T>::normalize() { err_not_implemented(); }

  ////////////////////////////////////////////////////////////////
  // ebm_2_gen

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::fprop(Tin1 &i1, Tin2 &i2, state_idx<T> &energy){ 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::bprop(Tin1 &i1, Tin2 &i2, state_idx<T> &energy){ 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::bbprop(Tin1 &i1, Tin2 &i2, state_idx<T> &energy)
  { err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::bprop1_copy(Tin1 &i1, Tin2 &i2,
				     state_idx<T> &energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::bprop2_copy(Tin1 &i1, Tin2 &i2,
				     state_idx<T> &energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::bbprop1_copy(Tin1 &i1, Tin2 &i2,
				      state_idx<T> &energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::bbprop2_copy(Tin1 &i1, Tin2 &i2, 
				      state_idx<T> &energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::forget(forget_param_linear &fp) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::normalize() { err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  double ebm_2_gen<Tin1,Tin2,T>::infer1(Tin1 &i1, Tin2 &i2,
					state_idx<T> &energy,
					infer_param &ip) {
    err_not_implemented(); }
  
  template<class Tin1, class Tin2, class T>
  double ebm_2_gen<Tin1,Tin2,T>::infer2(Tin1 &i1, Tin2 &i2, infer_param &ip,
				  Tin2 *label, state_idx<T> *energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class T>
  void ebm_2_gen<Tin1,Tin2,T>::infer2_copy(Tin1 &i1, Tin2 &i2,
				     state_idx<T> &energy) { 
    err_not_implemented(); }

  ////////////////////////////////////////////////////////////////
  // two layer module

  template<class Tin, class Thid, class Tout>
  layers_2_gen<Tin,Thid,Tout>::layers_2_gen(module_1_1_gen<Tin,Thid> &l1,
					    Thid &h,
					    module_1_1_gen<Thid,Tout> &l2)
    : layer1(l1), hidden(h), layer2(l2) {
  }

  // Do nothing. Module doesn't have ownership of sub-modules
  template<class Tin, class Thid, class Tout>
  layers_2_gen<Tin,Thid,Tout>::~layers_2_gen() { }

  template<class Tin, class Thid, class Tout>
  void layers_2_gen<Tin,Thid,Tout>::fprop(Tin &in, Tout &out) {
    layer1.fprop(in, hidden);
    layer2.fprop(hidden, out);
  }

  template<class Tin, class Thid, class Tout>
  void layers_2_gen<Tin,Thid,Tout>::bprop(Tin &in, Tout &out) {
    hidden.clear_dx();
    layer2.bprop(hidden, out);
    layer1.bprop(in, hidden);
  }

  template<class Tin, class Thid, class Tout>
  void layers_2_gen<Tin,Thid,Tout>::bbprop(Tin &in, Tout &out) {
    hidden.clear_ddx();
    layer2.bbprop(hidden, out);
    layer1.bbprop(in, hidden);
  }

  template<class Tin, class Thid, class Tout>
  void layers_2_gen<Tin,Thid,Tout>::forget(forget_param_linear &fp) {
    layer1.forget(fp);
    layer2.forget(fp);
  }

  template<class Tin, class Thid, class Tout>
  void layers_2_gen<Tin,Thid,Tout>::normalize() {
    layer1.normalize();
    layer2.normalize();
  }

  template<class Tin, class Thid, class Tout>
  idxdim layers_2_gen<Tin,Thid,Tout>::fprop_size(idxdim &isize) {
    idxdim os(isize);
    os = layer1.fprop_size(os);
    os = layer2.fprop_size(os);
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(os);
    return os;
  }

  template<class Tin, class Thid, class Tout>
  idxdim layers_2_gen<Tin,Thid,Tout>::bprop_size(const idxdim &osize) {
    idxdim isize(osize);
    isize = layer2.bprop_size(isize);
    isize = layer1.bprop_size(isize);
    return isize;
  }  

  template<class Tin, class Thid, class Tout>
  void layers_2_gen<Tin,Thid,Tout>::pretty(idxdim &isize) {
    idxdim is(isize);
    layer1.pretty(is);
    cout << " -> ";
    is = layer1.fprop_size(is);
    layer2.pretty(is);
  }
  
  ////////////////////////////////////////////////////////////////
  // N layer module

  template<class T> layers_n_gen<T>::layers_n_gen(bool oc) {
    modules = new std::vector< module_1_1_gen <T,T>* >();
    hiddens = new std::vector< T* >();
    this->own_contents = oc;
  }

  // Clean vectors. Module doesn't have ownership of sub-modules
  template<class T> layers_n_gen<T>::~layers_n_gen() {
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

  template<class T>
  void layers_n_gen<T>::add_module(module_1_1_gen <T, T>* module, T* hidden) {
    if (modules->size() > hiddens->size())
      eblerror("Inconsistency in layers_n_gen, probably you have passed null \
hidden layer before");

    modules->push_back(module);
    if (hidden != NULL)
      hiddens->push_back(hidden);

    if (modules->size() - hiddens->size() > 1)
      eblerror("Inconsistency in layers_n_gen, probably you did not allocate \
enough hidden states in layers_n_gen");
  }

  template<class T>
  void layers_n_gen<T>::add_last_module(module_1_1_gen <T, T>* module) {
    add_module(module, NULL);
  }

  template<class T>
  void layers_n_gen<T>::fprop(T& in, T& out){
    if (modules->empty())
      eblerror("trying to fprop through empty layers_n_gen");

    T* hi = &in;
    T* ho = &in;

    // last will be manual
    int niter = modules->size()-1;
    for(int i = 0; i < niter; i++){
      ho = (*hiddens)[i];
      (*modules)[i]->fprop(*hi,*ho);
      hi = ho;
    }
    (*modules)[niter]->fprop(*ho,out);
  }

  template<class T>
  void layers_n_gen<T>::bprop(T& in, T& out){
    if (modules->empty())
      eblerror("trying to bprop through empty layers_n_gen");

    // clear hidden states
    for (unsigned int i=0; i<hiddens->size(); i++){
      if ((*hiddens)[i])
	(*hiddens)[i]->clear_dx();
    }
    T* hi = &out;
    T* ho = &out;

    // last will be manual
    int niter = modules->size()-1;
    for(int i=niter; i>0; i--){
      hi = (*hiddens)[i-1];
      (*modules)[i]->bprop(*hi,*ho);
      ho = hi;
    }
    (*modules)[0]->bprop(in,*ho);
  }

  template<class T>
  void layers_n_gen<T>::bbprop(T& in, T& out){
    if (modules->empty())
      eblerror("trying to bbprop through empty layers_n_gen");

    // clear hidden states
    for (unsigned int i=0; i<hiddens->size(); i++){
      if ((*hiddens)[i])
	(*hiddens)[i]->clear_ddx();
    }

    T* hi = &out;
    T* ho = &out;

    // last will be manual
    int niter = modules->size()-1;
    for(int i=niter; i>0; i--){
      hi = (*hiddens)[i-1];
      (*modules)[i]->bbprop(*hi,*ho);
      ho = hi;
    }
    (*modules)[0]->bbprop(in,*ho);
  }

  template<class T>
  void layers_n_gen<T>::forget(forget_param_linear& fp){
    if (modules->empty())
      eblerror("trying to forget through empty layers_n_gen");

    for(unsigned int i=0; i<modules->size(); i++){
      module_1_1_gen<T, T> *tt = (*modules)[i];
      tt->forget(fp);
    }
  }

  template<class T>
  void layers_n_gen<T>::normalize(){
    if (modules->empty())
      eblerror("trying to normalize through empty layers_n_gen");

    for(unsigned int i=0; i<modules->size(); i++){
      (*modules)[i]->normalize();
    }
  }

  //! This method modifies i_size to be compliant with the architecture of 
  //! the module. It also returns the output size corresponding to the new
  //! input...
  template<class T>
  idxdim layers_n_gen<T>::fprop_size(idxdim &isize) {
    idxdim os(isize);
    //! Loop through all the layers of the module, and update output
    //! size accordingly.
    for (unsigned int i = 0; i < modules->size(); i++) {
      module_1_1_gen<T, T> *tt = (*modules)[i];
      os = tt->fprop_size(os);
    }
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(os);
    return os;
  }

  //! This method computes the input size of the module for a given output
  //! size.
  template<class T>
  idxdim layers_n_gen<T>::bprop_size(const idxdim &osize) {
    idxdim isize(osize);
    //! Loop through all the layers of the module, from the end to the beg.
    for (int i = (int) modules->size() - 1; i >= 0; i--) {
      module_1_1_gen<T, T> *tt = (*modules)[i];
      isize = tt->bprop_size(isize);
    }
    return isize;
  }

  template<class T>
  void layers_n_gen<T>::pretty(idxdim &isize) {
    idxdim is(isize);
    cout << is;
    //! Loop through all the layers of the module, and update output
    //! size accordingly.
    for (unsigned int i = 0; i < modules->size(); i++) {
      module_1_1_gen<T, T> *tt = (*modules)[i];
      tt->pretty(is);
      is = tt->fprop_size(is);
    }
    cout << endl;
  }

  template<class T>
  layers_n_gen<T>* layers_n_gen<T>::copy() {
    eblerror("not implemented");
  }
  
  ////////////////////////////////////////////////////////////////

  template<class Tin, class Thid, class T>
  fc_ebm1_gen<Tin,Thid,T>::fc_ebm1_gen(module_1_1_gen<Tin,Thid> &fm,
			       Thid &fo, ebm_1_gen<Thid, T> &fc)
    : fmod(fm), fout(fo), fcost(fc) {
  }

  template<class Tin, class Thid, class T>
  fc_ebm1_gen<Tin,Thid,T>::~fc_ebm1_gen() {}

  template<class Tin, class Thid, class T>
  void fc_ebm1_gen<Tin,Thid,T>::fprop(Tin &in, state_idx<T> &energy) {
    fmod.fprop(in, fout);
    fcost.fprop(fout, energy);
  }

  template<class Tin, class Thid, class T>
  void fc_ebm1_gen<Tin,Thid,T>::bprop(Tin &in, state_idx<T> &energy) {
    fout.clear_dx();
    fcost.bprop(fout, energy);
    fmod.bprop(in, fout);
  }

  template<class Tin, class Thid, class T>
  void fc_ebm1_gen<Tin,Thid,T>::bbprop(Tin &in, state_idx<T> &energy) {
    fout.clear_ddx();
    fcost.bbprop(fout, energy);
    fmod.bbprop(in, fout);
  }

  template<class Tin, class Thid, class T>
  void fc_ebm1_gen<Tin,Thid,T>::forget(forget_param &fp) {
    fmod.forget(fp);
    fcost.forget(fp);
  }

  ////////////////////////////////////////////////////////////////

  template<class Tin1, class Tin2,  class T>
  fc_ebm2_gen<Tin1,Tin2,T>::fc_ebm2_gen(module_1_1_gen<Tin1,Tin1> &fm,
					Tin1 &fo, 
					ebm_2_gen<Tin1,Tin2,T> &fc)
    : fmod(fm), fout(fo), fcost(fc) {
  }

  template<class Tin1, class Tin2,  class T>
  fc_ebm2_gen<Tin1,Tin2,T>::~fc_ebm2_gen() {}

  template<class Tin1, class Tin2,  class T>
  void fc_ebm2_gen<Tin1,Tin2,T>::fprop(Tin1 &in1, Tin2 &in2,
				       state_idx<T> &energy) {
    fmod.fprop(in1, fout);
    fcost.fprop(fout, in2, energy);
#ifdef __DUMP_STATES__ // used to debug
    save_matrix(energy.x, "dump_fc_ebm2_energy.x.mat");
    save_matrix(in1.x, "dump_fc_ebm2_cost_in1.x.mat");
#endif
  }

  template<class Tin1, class Tin2,  class T>
  void fc_ebm2_gen<Tin1,Tin2,T>::bprop(Tin1 &in1, Tin2 &in2,
				       state_idx<T> &energy) {
    fout.clear_dx();
    // in2.clear_dx(); // TODO this assumes Tin2 == state_idx
    fcost.bprop(fout, in2, energy);
    fmod.bprop(in1, fout);
  }

  template<class Tin1, class Tin2,  class T>
  void fc_ebm2_gen<Tin1,Tin2,T>::bbprop(Tin1 &in1, Tin2 &in2,
					state_idx<T> &energy){
    fout.clear_ddx();
    // in2.clear_ddx(); // TODO this assumes Tin2 == state_idx
    fcost.bbprop(fout, in2, energy);
    fmod.bbprop(in1, fout);
  }

  template<class Tin1, class Tin2,  class T>
  void fc_ebm2_gen<Tin1,Tin2,T>::forget(forget_param_linear &fp) {
    fmod.forget(fp);
    fcost.forget(fp);
  }

  template<class Tin1, class Tin2,  class T>
  double fc_ebm2_gen<Tin1,Tin2,T>::infer2(Tin1 &i1, Tin2 &i2,
					  infer_param &ip,
					  Tin2 *label,
					  state_idx<T> *energy) {
    fmod.fprop(i1, fout); // first propagate all the way up
    return fcost.infer2(fout, i2, ip, label, energy); //then infer from energies
  }

} // end namespace ebl
