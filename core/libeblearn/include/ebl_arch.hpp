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
  // module_1_1

  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>::module_1_1(const char *name_, bool bResize_)
    : bResize(bResize_), _name(name_), memoptimized(false) { 
  }

  template <typename T, class Tin, class Tout>
  module_1_1<T,Tin,Tout>::~module_1_1() {
#ifdef __DEBUG__
    cout << "deleting module_1_1: " << _name << endl;
#endif
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
  
  template <typename T, class Tin, class Tout>
  bool module_1_1<T,Tin,Tout>::optimize_fprop(Tin& in, Tout& out){
    return true;
  }
  
  template <typename T, class Tin, class Tout>
  const char *module_1_1<T,Tin,Tout>::name() {
    return this->_name.c_str();
  }
  
  template <typename T, class Tin, class Tout>
  std::string module_1_1<T,Tin,Tout>::describe() {
    std::string desc = _name; // default, just return the module's name
    return desc;
  }
  
  template <typename T, class Tin, class Tout>
  void module_1_1<T,Tin,Tout>::load_x(idx<T> &weights) { 
    err_not_implemented(); }

  ////////////////////////////////////////////////////////////////
  // module_2_1

  template <typename T, class Tin1, class Tin2, class Tout>
  module_2_1<T,Tin1,Tin2,Tout>::module_2_1(const char *name_)
    : _name(name_) { 
  }

  template <typename T, class Tin1, class Tin2, class Tout>
  module_2_1<T,Tin1,Tin2,Tout>::~module_2_1() {
#ifdef __DEBUG__
    cout << "deleting module_2_1: " << _name << endl;
#endif
  }

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

  template <typename T, class Tin1, class Tin2, class Tout>
  const char *module_2_1<T,Tin1,Tin2,Tout>::name() {
    return _name;
  }
  
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
  void layers<T,Tstate>::add_module(module_1_1<T, Tstate, Tstate>* module) {
    // regular addition
    modules.push_back(module);
    hiddens.push_back(NULL);
  }

  template <typename T, class Tstate>
  bool layers<T,Tstate>::optimize_fprop(Tstate& in, Tstate& out){
    this->memoptimized = true;
    if (modules.empty())
      eblerror("trying to fprop through empty layers");
    // initialize buffers
    hi = &in;
    ho = &out;
    // parallelism: do not modify input nor output
    if (branch) {
      // create idxdim of same order but sizes 1
      idxdim d = in.x.get_idxdim();
      for (int k = 0; k < d.order(); ++k)
	d.setdim(k, 1);
      // create our internal buffers
      intern_h0 = new Tstate(d);
      intern_h1 = new Tstate(d);
      ho = intern_h0;
    }
    // loop over modules
    for (uint i = 0; i < modules.size(); i++) {
      hiddens[i] = ho;
      // parallelism: for first module, do not allow optim with in buffer
      if (branch && i == 0) {
	hi = intern_h1; // now we use only internal buffers
	swap_buffers(); // swap hi and ho
      } else {
	// call optimization on submodules, and remember if they put
	// the output in ho (swap == true) or not (swap == false).
	bool swap = modules[i]->optimize_fprop(*hi,*ho);
	// if output is truly in ho, swap buffers, otherwise do nothing.
	// if module was a branch, it di
	if (swap)
	  swap_buffers();
      }
    }
    // parallelism: remember which buffer contains the output
    if (branch) {
      intern_out = hiddens[modules.size() - 1];
      // a branch does not output to current track, so the output for the
      // mother branch is actually the branch's input, which is left in in
      return false; // output is in in
    }
    // tell the outside if the output is in in or out
    if (hiddens[modules.size() - 1] == &out)
      return true; // output is in out
    return false; // output is in in
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::fprop(Tstate& in, Tstate& out){
    if (modules.empty())
      eblerror("trying to fprop through empty layers");
    // initialize buffers
    hi = &in;
    ho = &out;
    // create idxdim of same order but sizes 1
    idxdim d = hi->x.get_idxdim();
    for (int k = 0; k < d.order(); ++k)
      d.setdim(k, 1);
    // narrow input data if required by branch
    Tstate narrowed;
    if (branch && branch_narrow) {
      narrowed = hi->narrow(narrow_dim, narrow_size, narrow_offset);
      DEBUG("branch narrowing input " << hi->x << " to " << narrowed.x);
      hi = &narrowed;
    }
    // loop over modules
    for(uint i = 0; i < modules.size(); i++){
      // if last module, output into out
      if (i == modules.size() - 1 && !branch)
	ho = &out;
      else { // not last module, use hidden buffers
	ho = (Tstate*) hiddens[i];
	// allocate hidden buffer if necessary
	if (ho == NULL) {
	  hiddens[i] = new Tstate(d);
	  ho = (Tstate*) hiddens[i];
	}
      }
      // run module
      modules[i]->fprop(*hi, *ho);
      DEBUG("fprop " << this->name() << " " << hi->x << " -> "
	    << modules[i]->name() << " -> " << ho->x);
      // keep same input if current module is a branch, otherwise take out as in
      bool isbranch = false;
      if (dynamic_cast<layers<T,Tstate>*>(modules[i]) &&
	  ((layers<T,Tstate>*)modules[i])->branch)
	isbranch = true;
      if (!isbranch)
	hi = ho;
      if (isbranch && i + 1 == modules.size())
	ho = hi; // if last module is branch, set the input to be the branch out
    }
    if (branch) // remember output buffer (did not output to out)
      intern_out = ho;
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bprop(Tstate& in, Tstate& out){
    if (this->memoptimized)
      eblerror("cannot bprop while using dual-buffer memory optimization");
    if (modules.empty())
      eblerror("trying to bprop through empty layers");

    // clear hidden states
    // do not clear if we are a branch, it must have been cleared already by
    // main branch
    if (!branch)
      clear_dx();
    
    hi = &out;
    ho = &out;

    if (branch) // we are a branch, use the internal output
      ho = intern_out;

    // last will be manual
    for (uint i = modules.size() - 1; i > 0; i--){
      // set input
      hi = hiddens[i - 1];
      // if previous module is a branch, take its input as input
      if (dynamic_cast<layers<T,Tstate>*>(modules[i - 1]) &&
	  ((layers<T,Tstate>*)modules[i - 1])->branch) {
	if (i >= 2)
	  hi = hiddens[i - 2];
	else // i == 1
	  hi = &in;
      }
      // bprop
      DEBUG("bprop " << this->name() << " " << hi->dx << " <- "
	    << modules[i]->name() << " <- " << ho->dx);
      modules[i]->bprop(*hi, *ho);
      // shift output pointer to input
      ho = hi;
    }
    DEBUG("bprop " << this->name() << " " << in.dx << " <- "
	  << modules[0]->name() << " <- " << ho->dx);
    modules[0]->bprop(in, *ho);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::bbprop(Tstate& in, Tstate& out){
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
    for(uint i = modules.size() - 1; i > 0; i--){
      // set input
      hi = hiddens[i-1];
      // if previous module is a branch, take its input as input
      if (dynamic_cast<layers<T,Tstate>*>(modules[i - 1]) &&
	  ((layers<T,Tstate>*)modules[i - 1])->branch) {
	if (i >= 2)
	  hi = hiddens[i - 2];
	else // i == 1
	  hi = &in;
      }
      // bbprop
      DEBUG("bbprop " << this->name() << " " << hi->ddx << " <- "
	    << modules[i]->name() << " <- " << ho->ddx);
      modules[i]->bbprop(*hi,*ho);
      // shift output pointer to input
      ho = hi;
    }
    DEBUG("bbprop " << this->name() << " " << in.ddx << " <- "
	  << modules[0]->name() << " <- " << ho->ddx);
    modules[0]->bbprop(in,*ho);
  }

  template <typename T, class Tstate>
  void layers<T,Tstate>::forget(forget_param_linear& fp){
    if (modules.empty())
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

  //! This method modifies i_size to be compliant with the architecture of 
  //! the module. It also returns the output size corresponding to the new
  //! input...
  template <typename T, class Tstate>
  idxdim layers<T,Tstate>::fprop_size(idxdim &isize) {
    idxdim os(isize);
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

  //! This method computes the input size of the module for a given output
  //! size.
  template <typename T, class Tstate>
  idxdim layers<T,Tstate>::bprop_size(const idxdim &osize) {
    idxdim isize(osize);
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
  layers<T,Tstate>* layers<T,Tstate>::copy() {
    layers<T,Tstate> *l2 = new layers<T,Tstate>(true);
    //! Loop through all the modules and buffers and copy them
    int niter = this->modules.size();
    for(int i = 0; i < niter; i++) {
      l2->add_module((module_1_1<T,Tstate>*)this->modules[i]->copy());
      if (this->hiddens[i] != NULL) {
	l2->hiddens[i] =
	  new Tstate(this->hiddens[i]->x.get_idxdim());
	idx_copy(this->hiddens[i]->x, l2->hiddens[i]->x);
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
  void layers<T,Tstate>::pretty(idxdim &isize) {
    idxdim is(isize);
    cout << is;
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
	tt->pretty(is);
	is = tt->fprop_size(is);
      }
    }
    cout << endl;
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
  module_1_1<T, Tstate, Tstate>* layers<T,Tstate>::find(const char *name) {
    for (uint i = 0; i < modules.size(); ++i) {
      module_1_1<T, Tstate, Tstate>* m = modules[i];
      if (!strcmp(name, m->name()))
	return m;
    }
    return NULL; // not found
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
  // flat_merge_module

  template <typename T, class Tstate>
  flat_merge_module<T, Tstate>::flat_merge_module(std::vector<Tstate**> &ins,
						  std::vector<uint> &insh_,
						  std::vector<uint> &insw_,
						  uint inh_, uint inw_,
						  std::vector<uint> &stridesh_,
						  std::vector<uint> &stridesw_,
						  uint instrideh_,
						  uint instridew_,
						  const char *name_,
						  const char *list)
    : module_1_1<T,Tstate>(name_), inh(inh_), inw(inw_), 
      instrideh(instrideh_), instridew(instridew_), merge_list(list) {
    if (ins.size() != insh_.size() || ins.size() != insw_.size()
	|| ins.size() != stridesw_.size() || ins.size() != stridesw_.size())
      eblerror("expected same size lists of inputs and strides but got "
	       << ins.size() << ", " << insh_.size()  << ", " << insw_.size() 
	       << ", " << stridesh_.size() << " and " << stridesw_.size());
    for (uint i = 0; i < ins.size(); ++i)
      inputs.push_back(ins[i]);
    for (uint i = 0; i < insh_.size(); ++i)
      insh.push_back(insh_[i]);
    for (uint i = 0; i < insw_.size(); ++i)
      insw.push_back(insw_[i]);
    for (uint i = 0; i < stridesh_.size(); ++i)
      stridesh.push_back(stridesh_[i]);
    for (uint i = 0; i < stridesw_.size(); ++i)
      stridesw.push_back(stridesw_[i]);
  }

  template <typename T, class Tstate>
  flat_merge_module<T, Tstate>::~flat_merge_module() {
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    // check that input is compatible with windows sizes
    if ((in.x.dim(1) - inh) % instrideh != 0 ||
	(in.x.dim(2) - inw) % instridew != 0)
      eblerror("input " << in.x << " incompatible with window " << inh << "x"
	       << inw << " and stride " << instrideh << "x" << instridew);
    // compute new size and resize output if necessary
    intg fsize = inh * inw * in.x.dim(0); // feature size for main input
    intg nh = 1 + (in.x.dim(1) - inh) / instrideh; // number of possible windows
    intg nw = 1 + (in.x.dim(2) - inw) / instridew; // number of possible windows
    for (uint i = 0; i < inputs.size(); ++i) // total feature size for all input
      fsize += (*inputs[i])->x.dim(0) * insh[i] * insw[i];
    idxdim d(fsize, nh, nw);
    if (!out.x.same_dim(d))
      out.resize(d);
    intg offset = 0;
    // copy main input to out
    fsize = inh * inw * in.x.dim(0); // feature size for main input
    idx<T> uin(in.x.unfold(1, inh, instrideh));
    uin = uin.unfold(2, inw, instridew);
    idx<T> o = out.x.narrow(0, fsize, offset);
    for (uint p = 0; p < o.dim(1); ++p) {
      for (uint q = 0; q < o.dim(2); ++q) {
	idx<T> flat = uin.select(1, p);
	flat = flat.select(1, q);
	idx<T> tmp(flat.get_idxdim());
	// TODO: tmp buffer less efficient than direct copy which but requires
	// continuous data, make idx pointing to oo with flat's dims?
	idx_copy(flat, tmp); 
	flat = tmp.view_as_order(1);
	idx<T> oo = o.select(1, p);
	oo = oo.select(1, q);
	idx_copy(flat, oo);
      }
    }
    offset += fsize;
    // copy inputs to out
    for (uint i = 0; i < inputs.size(); ++i) {
      idx<T> input = (*inputs[i])->x;
      fsize = insh[i] * insw[i] * input.dim(0); // feature size from input
      idx<T> uin(input.unfold(1, insh[i], stridesh[i]));
      uin = uin.unfold(2, insw[i], stridesw[i]);
      idx<T> o = out.x.narrow(0, fsize, offset);
      for (uint p = 0; p < o.dim(1); ++p) {
	for (uint q = 0; q < o.dim(2); ++q) {
	  idx<T> flat = uin.select(1, p);
	  flat = flat.select(1, q);
	  idx<T> tmp(flat.get_idxdim());
	  // TODO: tmp buffer less efficient than direct copy which but requires
	  // continuous data, make idx pointing to oo with flat's dims?
	  idx_copy(flat, tmp); 
	  flat = tmp.view_as_order(1);
	  idx<T> oo = o.select(1, p);
	  oo = oo.select(1, q);
	  idx_copy(flat, oo);
	}
      }
      offset += fsize;
    }
#ifdef __DEBUG__
    cout << describe() << ": " << in.x << " (in " << inh << "x" << inw
	 << " stride " << instrideh << "x" << instridew << ")";
    for (uint i = 0; i < inputs.size(); ++i)
      cout << " + " << (*inputs[i])->x << " (in " << insh[i] << "x" << insw[i]
	   << " stride " << stridesh[i] << "x" << stridesw[i] << ")";
    cout << " -> " << out.x << endl;
#endif
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    // copy out to main input
    intg offset = 0;
    idx<T> o1 = out.dx.view_as_order(1);
    idx<T> o = o1.narrow(0, in.dx.nelements(), offset);
    idx<T> input = in.dx.view_as_order(1);
    idx_add(o, input, input);
    offset += input.nelements();
    // copy out to inputs
    for (uint i = 0; i < inputs.size(); ++i) {
      input = (*inputs[i])->dx.view_as_order(1);
      o = o1.narrow(0, input.nelements(), offset);
      idx_add(o, input, input);
      offset += input.nelements();
    }
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::bbprop(Tstate &in,
					Tstate &out) {
    // copy out to main input
    intg offset = 0;
    idx<T> o1 = out.ddx.view_as_order(1);
    idx<T> o = o1.narrow(0, in.ddx.nelements(), offset);
    idx<T> input = in.ddx.view_as_order(1);
    idx_add(o, input, input);
    offset += input.nelements();
    // copy out to inputs
    for (uint i = 0; i < inputs.size(); ++i) {
      input = (*inputs[i])->ddx.view_as_order(1);
      o = o1.narrow(0, input.nelements(), offset);
      idx_add(o, input, input);
      offset += input.nelements();
    }
  }

  template <typename T, class Tstate>
  idxdim flat_merge_module<T,Tstate>::fprop_size(idxdim &isize) {
    intg fsize = inh * inw * isize.dim(0); // feature size for main input
    intg nh = 1 + (isize.dim(1) - inh) / instrideh; // number of possible windows
    intg nw = 1 + (isize.dim(2) - inw) / instridew; // number of possible windows
    //! Extract its dimensions, update output size
    idxdim osize(fsize, std::max((intg) 1, nh),
		 std::max((intg) 1, nw));
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  idxdim flat_merge_module<T,Tstate>::bprop_size(const idxdim &osize) {
    intg fsize = osize.dim(0) / inh / inw; // feature size for main input
    intg ih = ((osize.dim(1) - 1) * instrideh) + inh; // number of possible windows
    intg iw = ((osize.dim(2) - 1) * instridew) + inw; // number of possible windows
    //! Extract its dimensions, update output size
    idxdim isize(fsize, ih, iw);
    return isize;
  }

  template <typename T, class Tstate>
  std::string flat_merge_module<T, Tstate>::describe() {
    std::string desc;
    desc << "flat_merge module " << this->name() << ", merging main input "
	 << " (in " << inh << "x" << inw << " stride " << instrideh << "x" 
	 << instridew << ") + " << inputs.size() << " inputs: ";
    for (uint i = 0; i < inputs.size(); ++i)
      desc << " (in " << insh[i] << "x" << insw[i]
	   << " stride " << stridesh[i] << "x" << stridesw[i] << "), ";
    if (!merge_list.empty())
      desc << " (" << merge_list << ")";
    return desc;
  }
  
  ////////////////////////////////////////////////////////////////
  // merge
  
  template <typename T, class Tstate>
  merge_module<T, Tstate>::merge_module(std::vector<Tstate**> &ins,
					intg concat_dim_,
					const char *name_,
					const char *list)
    : module_1_1<T,Tstate>(name_), concat_dim(concat_dim_), merge_list(list) {
    for (uint i = 0; i < ins.size(); ++i)
      inputs.push_back(ins[i]);
  }

  template <typename T, class Tstate>
  merge_module<T, Tstate>::~merge_module() {
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    idxdim d(in.x), dtmp(in.x);
    // check that all inputs are compatible and compute output size
    for (uint i = 0; i < inputs.size(); ++i) {
      Tstate *input = *(inputs[i]);
      dtmp.setdim(concat_dim, input->x.dim(concat_dim));
      if (!input->x.same_dim(dtmp))
	eblerror("expected same dimensions but got " << input->x.get_idxdim()
		 << " and " << dtmp);
      // increment dimension
      d.setdim(concat_dim, d.dim(concat_dim) + input->x.dim(concat_dim));
    }
    // check that output has the right size, if not, resize
    if (out.x.get_idxdim() != d)
      out.resize(d);
    // copy main input to out
    intg offset = 0;
    idx<T> o = out.x.narrow(concat_dim, in.x.dim(concat_dim), offset);
    idx_copy(in.x, o);
    offset += in.x.dim(concat_dim);
    // copy inputs to out
    for (uint i = 0; i < inputs.size(); ++i) {
      Tstate *input = *(inputs[i]);
      o = out.x.narrow(concat_dim, input->x.dim(concat_dim), offset);
      idx_copy(input->x, o);
      offset += input->x.dim(concat_dim);
    }
#ifdef __DEBUG__
    cout << describe() << ": " << in.x;
    for (uint i = 0; i < inputs.size(); ++i)
      cout << " + " << (*inputs[i])->x;
    cout << " -> " << out.x << endl;
#endif
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    // copy out to main input
    intg offset = 0;
    idx<T> o = out.dx.narrow(concat_dim, in.dx.dim(concat_dim), offset);
    idx_add(o, in.dx, in.dx);
    offset += in.dx.dim(concat_dim);
    // copy out to inputs
    for (uint i = 0; i < inputs.size(); ++i) {
      Tstate *input = *(inputs[i]);
      o = out.dx.narrow(concat_dim, input->dx.dim(concat_dim), offset);
      idx_add(o, input->dx, input->dx);
      offset += input->dx.dim(concat_dim);
    }
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::bbprop(Tstate &in,
					Tstate &out) {
    // copy out to main input
    intg offset = 0;
    idx<T> o = out.ddx.narrow(concat_dim, in.ddx.dim(concat_dim), offset);
    idx_add(o, in.ddx, in.ddx);
    offset += in.ddx.dim(concat_dim);
    // copy out to inputs
    for (uint i = 0; i < inputs.size(); ++i) {
      Tstate *input = *(inputs[i]);
      o = out.ddx.narrow(concat_dim, input->ddx.dim(concat_dim), offset);
      idx_add(o, input->ddx, input->ddx);
      offset += input->ddx.dim(concat_dim);
    }
  }

  template <typename T, class Tstate>
  std::string merge_module<T, Tstate>::describe() {
    std::string desc;
    desc << "merge module " << this->name() << ", merging main input + "
	 << (uint) inputs.size() << " inputs";
    if (!merge_list.empty())
      desc << " (" << merge_list << ")";
    return desc;
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
