/***************************************************************************
 *   Copyright (C) 2012 by Yann LeCun and Pierre Sermanet *
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

// parameter /////////////////////////////////////////////////////////////////

template <typename T> parameter<T>::parameter(intg initial_size)
    : state<T>(initial_size),
      deltax(initial_size), epsilons(initial_size), ddeltax(initial_size) {
  // this class is not meant to be used for training
  this->set_forward_only();
  // clear buffers
  idx_clear(deltax);
  idx_clear(epsilons);
  idx_clear(ddeltax);
  resize_parameter(0);
}

template <typename T>
parameter<T>::parameter(const char *param_filename)
    : state<T>(1), deltax(1), epsilons(1), ddeltax(1) {
  // this class is not meant to be used for training
  this->set_forward_only();
  // load data
  if (!load_x(param_filename)) {
    std::cerr << "failed to open " << param_filename << std::endl;
    eblerror("failed to load bbparameter file in bbparameter constructor");
  }
}

template <typename T>
parameter<T>::~parameter() {
}

// TODO-0: BUG: a bbparameter object casted in state_idx* and called
// with resize(n) calls state_idx::resize instead of
// parameter<T>::resize
// a temporary unclean solution is to use the same bbparameters as
// in state_idx::resize in parameter<T>::resize:
// resize(intg s0, intg s1, intg s2, intg s3, intg s4, intg s5,
//		intg s6, intg s7);
template <typename T>
void parameter<T>::resize_parameter(intg s0) {
  f[0].resize(s0);
  if (!b.empty()) b[0].resize(s0);
  if (!bb.empty()) bb[0].resize(s0);
  deltax.resize(s0);
  epsilons.resize(s0);
  ddeltax.resize(s0);
}

// I/O methods ///////////////////////////////////////////////////////////////

template <typename T>
bool parameter<T>::load_x(std::vector<std::string> &files) {
  if (files.size() == 0) eblerror("expected at least 1 file to load");
  try {
    idx<T> w = load_matrix<T>(files[0]);
    for (uint i = 1; i < files.size(); ++i) {
      idx<T> tmp = load_matrix<T>(files[i]);
      w = idx_concat(w, tmp);
    }
    std::cout << "Concatenated " << files.size() << " matrices into 1: "
              << w << " from " << files << std::endl;
    load_x(w);
    return true;
  } eblcatcherror_msg("failed to load weights");
  return false;
}

template <typename T>
bool parameter<T>::load_x(const char *s) {
  try {
    idx<T> m = load_matrix<T>(s);
    if ((idx<T>::dim(0) != 1) // param has been enlarged by network building
        && (idx<T>::dim(0) != m.dim(0))) // trying to load incompatible network
      eblerror("Trying to load a network with " << m.dim(0)
               << " parameters into a network with " << idx<T>::dim(0)
               << " parameters");
    this->resize_parameter(m.dim(0));
    idx_copy(m, *this);
    std::cout << "Loaded weights from " << s << ": " << *this << std::endl;
    return true;
  } eblcatcherror_msg("failed to load weights");
  return false;
}

template <typename T>
bool parameter<T>::load_x(idx<T> &m) {
  if ((idx<T>::dim(0) != 1) // param has been enlarged by network building
      && (idx<T>::dim(0) != m.dim(0))) { // trying to load incompatible net
    eblerror("Trying to load a network with " << m.dim(0)
             << " parameters into a network with " << idx<T>::dim(0)
             << " parameters");
  }
  this->resize_parameter(m.dim(0));
  idx_copy(m, *this);
  std::cout << "Loaded weights from " << m << ": " << *this << std::endl;
  return true;
}

template <typename T>
bool parameter<T>::save_x(const char *s) {
  if (!save_matrix(*this, s))
    return false;
  return true;
}

template <typename T>
void parameter<T>::permute_x(std::vector<intg> &blocks,
                             std::vector<uint> &permutations) {
  if (blocks.size() != permutations.size())
    eblerror("expected same number of elements in " << blocks
             << " and " << permutations);
  // copy all blocks into temporaries
  svector<idx<T> > copies;
  intg offset = 0;
  for (uint i = 0; i < blocks.size(); ++i) {
    intg sz = blocks[i] - offset;
    idx<T> *m = new idx<T>(sz);
    idx<T> tmp = f[0].narrow(0, sz, offset);
    idx_copy(tmp, *m);
    copies.push_back(m);
    offset += sz;
  }
  // permute list of blocks
  copies.permute(permutations);
  // copy blocks back
  offset = 0;
  for (uint i = 0; i < copies.size(); ++i) {
    intg sz = copies[i].nelements();
    idx<T> tmp = f[0].narrow(0, sz, offset);
    idx_copy(copies[i], tmp);
    offset += sz;
  }
  std::cout << "Permuted weight blocks " << blocks
            << " with permutation vector " << permutations << std::endl;
}

// weights manipulation ////////////////////////////////////////////////////

template <typename T>
void parameter<T>::clear_deltax() {
  idx_clear(deltax);
}

template <typename T>
void parameter<T>::clear_ddeltax() {
  idx_clear(ddeltax);
}

template <typename T>
void parameter<T>::set_epsilon(T m) {
  idx_fill(epsilons, m);
}

template <typename T>
void parameter<T>::compute_epsilons(T mu) {
  idx_addc(ddeltax, mu, epsilons);
  idx_inv(epsilons, epsilons);
}

template <typename T>
void parameter<T>::update(gd_param &arg) {
  update_weights(arg);
}
template <typename T>
void parameter<T>::update_ddeltax(T knew, T kold) {
  idx_lincomb(this->bb[0], knew, ddeltax, kold, ddeltax);
}

// protected methods /////////////////////////////////////////////////////////

template <typename T>
void parameter<T>::update_weights(gd_param &arg) {
  if (b.empty()) eblerror("gradient tensors not found");
  // L2 gradients regularization
  if (arg.decay_l2 > 0) idx_dotcacc(f[0], arg.decay_l2, b[0]);
  // L1 gradients regularization
  if (arg.decay_l1 > 0) idx_signdotcacc(f[0], (T) arg.decay_l1, b[0]);
  // weights update
  if (arg.inertia == 0) {
    idx_mul(b[0], epsilons, b[0]);
    idx_dotcacc(b[0], -arg.eta, f[0]);
  } else {
    update_deltax((T) (1 - arg.inertia), (T) arg.inertia);
    idx_mul(deltax, epsilons, deltax);
    idx_dotcacc(deltax, -arg.eta, f[0]);
  }
}

template <typename T>
void parameter<T>::update_deltax(T knew, T kold) {
  if (b.empty()) eblerror("gradient tensors not found");
  idx_lincomb(b[0], knew, deltax, kold, deltax);
}

// bparameter /////////////////////////////////////////////////////////////////

template <typename T> bparameter<T>::bparameter(intg initial_size)
    : parameter<T>(initial_size) {
  // initialize backward tensors
  this->b.push_back(new idx<T>(this->get_idxdim()));
  this->resize_parameter(0);
}

template <typename T>
bparameter<T>::bparameter(const char *param_filename) : state<T>(1) {
  // initialize backward tensors
  this->b.push_back(new idx<T>(this->get_idxdim()));
  // load data
  if (!this->load_x(param_filename)) {
    std::cerr << "failed to open " << param_filename << std::endl;
    eblerror("failed to load bbbparameter file in bbbparameter constructor");
  }
}

template <typename T>
bparameter<T>::~bparameter() {
}

// bbparameter /////////////////////////////////////////////////////////////////

template <typename T> bbparameter<T>::bbparameter(intg initial_size)
    : parameter<T>(initial_size) {
  // initialize backward tensors
  this->b.push_back(new idx<T>(this->get_idxdim()));
  this->bb.push_back(new idx<T>(this->get_idxdim()));
  this->resize_parameter(0);
}

template <typename T>
bbparameter<T>::bbparameter(const char *param_filename) : parameter<T>(1) {
  // initialize backward tensors
  this->b.push_back(new idx<T>(this->get_idxdim()));
  this->bb.push_back(new idx<T>(this->get_idxdim()));
  // load data
  if (!this->load_x(param_filename)) {
    std::cerr << "failed to open " << param_filename << std::endl;
    eblerror("failed to load bbbbparameter file in bbbbparameter constructor");
  }
}

template <typename T>
bbparameter<T>::~bbparameter() {
}

} // end namespace ebl
