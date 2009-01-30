/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun   *
 *   yann@cs.nyu.edu   *
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

////////////////////////////////////////////////////////////////
namespace ebl {

  ////////////////////////////////////////////////////////////////

  template<class Tin, class Tout>
  void module_1_1<Tin,Tout>::fprop(Tin *in, Tout *out) { 
    err_not_implemented(); }

  template<class Tin, class Tout>
  void module_1_1<Tin,Tout>::bprop(Tin *in, Tout *out) { 
    err_not_implemented(); }

  template<class Tin, class Tout>
  void module_1_1<Tin,Tout>::bbprop(Tin *in, Tout *out) { 
    err_not_implemented(); }

  template<class Tin, class Tout>
  void module_1_1<Tin,Tout>::forget(forget_param_linear& fp) { 
    err_not_implemented(); }

  template<class Tin, class Tout>
  void module_1_1<Tin,Tout>::normalize() { err_not_implemented(); }

  ////////////////////////////////////////////////////////////////

  template<class Tin1, class Tin2, class Tout>
  void module_2_1<Tin1,Tin2,Tout>::fprop(Tin1 *in1, Tin2 *in2, Tout *out) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class Tout>
  void module_2_1<Tin1,Tin2,Tout>::bprop(Tin1 *in1, Tin2 *in2, Tout *out) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class Tout>
  void module_2_1<Tin1,Tin2,Tout>::bbprop(Tin1 *in1, Tin2 *in2, Tout *out) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class Tout>
  void module_2_1<Tin1,Tin2,Tout>::forget(forget_param &fp) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2, class Tout>
  void module_2_1<Tin1,Tin2,Tout>::normalize() { err_not_implemented(); }

  ////////////////////////////////////////////////////////////////

  template<class Tin>
  void ebm_1<Tin>::fprop(Tin *in, state_idx *energy) { err_not_implemented(); }

  template<class Tin>
  void ebm_1<Tin>::bprop(Tin *in, state_idx *energy) { err_not_implemented(); }

  template<class Tin>
  void ebm_1<Tin>::bbprop(Tin *in, state_idx *energy) { err_not_implemented(); }

  template<class Tin>
  void ebm_1<Tin>::forget(forget_param &fp) { err_not_implemented(); }

  template<class Tin>
  void ebm_1<Tin>::normalize() { err_not_implemented(); }

  ////////////////////////////////////////////////////////////////

  template<class Tin1, class Tin2>
  void ebm_2<Tin1,Tin2>::fprop(Tin1 *i1, Tin2 *i2, state_idx *energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2>
  void ebm_2<Tin1,Tin2>::bprop(Tin1 *i1, Tin2 *i2, state_idx *energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2>
  void ebm_2<Tin1,Tin2>::bbprop(Tin1 *i1, Tin2 *i2, state_idx *energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2>
  void ebm_2<Tin1,Tin2>::bprop1_copy(Tin1 *i1, Tin2 *i2, state_idx *energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2>
  void ebm_2<Tin1,Tin2>::bprop2_copy(Tin1 *i1, Tin2 *i2, state_idx *energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2>
  void ebm_2<Tin1,Tin2>::bbprop1_copy(Tin1 *i1, Tin2 *i2, state_idx *energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2>
  void ebm_2<Tin1,Tin2>::bbprop2_copy(Tin1 *i1, Tin2 *i2, state_idx *energy) { 
    err_not_implemented(); }

  template<class Tin1, class Tin2>
  void ebm_2<Tin1,Tin2>::forget(forget_param &fp) { err_not_implemented(); }

  template<class Tin1, class Tin2>
  void ebm_2<Tin1,Tin2>::normalize() { err_not_implemented(); }


  ////////////////////////////////////////////////////////////////
  // two layer module

  template<class Tin, class Thid, class Tout>
  layers_2<Tin,Thid,Tout>::layers_2(module_1_1<Tin,Thid> *l1, Thid *h,
				    module_1_1<Thid,Tout> *l2) {
    layer1 = l1;
    hidden = h;
    layer2 = l2;
  }

  // Do nothing. Module doesn't have ownership of sub-modules
  template<class Tin, class Thid, class Tout>
  layers_2<Tin,Thid,Tout>::~layers_2() { }

  template<class Tin, class Thid, class Tout>
  void layers_2<Tin,Thid,Tout>::fprop(Tin *in, Tout *out) {
    layer1->fprop(in, hidden);
    layer2->fprop(hidden, out);
  }

  template<class Tin, class Thid, class Tout>
  void layers_2<Tin,Thid,Tout>::bprop(Tin *in, Tout *out) {
    hidden->clear_dx();
    layer2->bprop(hidden, out);
    layer1->bprop(in, hidden);
  }

  template<class Tin, class Thid, class Tout>
  void layers_2<Tin,Thid,Tout>::bbprop(Tin *in, Tout *out) {
    hidden->clear_ddx();
    layer2->bbprop(hidden, out);
    layer1->bbprop(in, hidden);
  }

  template<class Tin, class Thid, class Tout>
  void layers_2<Tin,Thid,Tout>::forget(forget_param &fp) {
    layer1->forget(fp);
    layer2->forget(fp);
  }

  template<class Tin, class Thid, class Tout>
  void layers_2<Tin,Thid,Tout>::normalize() {
    layer1->normalize();
    layer2->normalize();
  }


  ////////////////////////////////////////////////////////////////
  // N layer module

  template<class T> layers_n<T>::layers_n() {
    modules = new  std::vector< module_1_1 <T,T>* >();
    hiddens = new std::vector< T* >();
    this->own_contents = true;
  }

  template<class T> layers_n<T>::layers_n(bool oc) {
    modules = new  std::vector< module_1_1 <T,T>* >();
    hiddens = new std::vector< T* >();
    this->own_contents = oc;
  }

  // Clean vectors. Module doesn't have ownership of sub-modules
  template<class T> layers_n<T>::~layers_n() {
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
  void layers_n<T>::addModule(module_1_1 <T, T>* module, T* hidden) {
    if (modules->size() > hiddens->size())
      ylerror("Inconsistency in layers_n, probably you have passed null \
hidden layer before");

    modules->push_back(module);
    if (hidden != NULL)
      hiddens->push_back(hidden);

    if (modules->size() - hiddens->size() > 1)
      ylerror("Inconsistency in layers_n, probably you did not allocate enough \
hidden states in layers_n");
  }

  template<class T>
  void layers_n<T>::fprop(T* in, T* out){
    if (modules->empty())
      ylerror("trying to fprop through empty layers_n");

    T* hi = in;
    T* ho = in;

    // last will be manual
    int niter = modules->size()-1;
    for(int i=0; i<niter; i++){
      ho = (*hiddens)[i];
      (*modules)[i]->fprop(hi,ho);
      hi = ho;
    }
    (*modules)[niter]->fprop(ho,out);
  }

  template<class T>
  void layers_n<T>::bprop(T* in, T* out){
    if (modules->empty())
      ylerror("trying to bprop through empty layers_n");

    // clear hidden states
    for (unsigned int i=0; i<hiddens->size(); i++){
      (*hiddens)[i]->clear_dx();
    }
    T* hi = out;
    T* ho = out;

    // last will be manual
    int niter = modules->size()-1;
    for(int i=niter; i>0; i--){
      hi = (*hiddens)[i-1];
      (*modules)[i]->bprop(hi,ho);
      ho = hi;
    }
    (*modules)[0]->bprop(in,ho);
  }

  template<class T>
  void layers_n<T>::bbprop(T* in, T* out){
    if (modules->empty())
      ylerror("trying to bbprop through empty layers_n");

    // clear hidden states
    for (unsigned int i=0; i<hiddens->size(); i++){
      (*hiddens)[i]->clear_ddx();
    }

    T* hi = out;
    T* ho = out;

    // last will be manual
    int niter = modules->size()-1;
    for(int i=niter; i>0; i--){
      hi = (*hiddens)[i-1];
      (*modules)[i]->bbprop(hi,ho);
      ho = hi;
    }
    (*modules)[0]->bbprop(in,ho);
  }

  template<class T>
  void layers_n<T>::forget(forget_param_linear& fp){
    if (modules->empty())
      ylerror("trying to forget through empty layers_n");

    for(unsigned int i=0; i<modules->size(); i++){
      module_1_1<state_idx,state_idx> *tt = (*modules)[i];
      tt->forget(fp);
    }
  }

  template<class T>
  void layers_n<T>::normalize(){
    if (modules->empty())
      ylerror("trying to normalize through empty layers_n");

    for(unsigned int i=0; i<modules->size(); i++){
      (*modules)[i]->normalize();
    }
  }

  ////////////////////////////////////////////////////////////////

  template<class Tin, class Thid>
  fc_ebm1<Tin,Thid>::fc_ebm1(module_1_1<Tin,Thid> *fm, Thid *fo, 
			     ebm_1<Thid> *fc) {
    fmod = fm;
    fout = fo;
    fcost = fc;
  }

  template<class Tin, class Thid>
  fc_ebm1<Tin,Thid>::~fc_ebm1() {}

  template<class Tin, class Thid>
  void fc_ebm1<Tin,Thid>::fprop(Tin *in, state_idx *energy) {
    fmod->fprop(in, fout);
    fcost->fprop(fout, energy);
  }

  template<class Tin, class Thid>
  void fc_ebm1<Tin,Thid>::bprop(Tin *in, state_idx *energy) {
    fout->clear_dx();
    fcost->bprop(fout, energy);
    fmod->bprop(in, fout);
  }

  template<class Tin, class Thid>
  void fc_ebm1<Tin,Thid>::bbprop(Tin *in, state_idx *energy) {
    fout->clear_ddx();
    fcost->bbprop(fout, energy);
    fmod->bbprop(in, fout);
  }

  template<class Tin, class Thid>
  void fc_ebm1<Tin,Thid>::forget(forget_param &fp) {
    fmod->forget(fp);
    fcost->forget(fp);
  }

  ////////////////////////////////////////////////////////////////

  template<class Tin1, class Tin2, class Thid>
  fc_ebm2<Tin1,Tin2,Thid>::fc_ebm2(module_1_1<Tin1,Thid> *fm, Thid *fo, 
				   ebm_2<Thid,Tin2> *fc) {
    fmod = fm;
    fout = fo;
    fcost = fc;
  }

  template<class Tin1, class Tin2, class Thid>
  fc_ebm2<Tin1,Tin2,Thid>::~fc_ebm2() {}

  template<class Tin1, class Tin2, class Thid>
  void fc_ebm2<Tin1,Tin2,Thid>::fprop(Tin1 *in1, Tin2 *in2, state_idx *energy) {
    fmod->fprop(in1, fout);
    fcost->fprop(fout, in2, energy);
  }

  template<class Tin1, class Tin2, class Thid>
  void fc_ebm2<Tin1,Tin2,Thid>::bprop(Tin1 *in1, Tin2 *in2, state_idx *energy) {
    fout->clear_dx();
    in2->clear_dx();
    fcost->bprop(fout, in2, energy);
    fmod->bprop(in1, fout);
  }


  template<class Tin1, class Tin2, class Thid>
  void fc_ebm2<Tin1,Tin2,Thid>::bbprop(Tin1 *in1, Tin2 *in2, state_idx *energy){
    fout->clear_ddx();
    in2->clear_ddx();
    fcost->bbprop(fout, in2, energy);
    fmod->bbprop(in1, fout);
  }

  template<class Tin1, class Tin2, class Thid>
  void fc_ebm2<Tin1,Tin2,Thid>::forget(forget_param &fp) {
    fmod->forget(fp);
    fcost->forget(fp);
  }

} // end namespace ebl
