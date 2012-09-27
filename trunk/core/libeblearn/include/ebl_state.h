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

#ifndef EBL_STATE_H_
#define EBL_STATE_H_

#include "libidx.h"

namespace ebl {

// forward declarations
template <typename T> class parameter;

// state ///////////////////////////////////////////////////////////////////////

//! A state carries information between layers.
//! It may carry forward information only, but also backward and
//! 2nd order backward information.
//! A state may be a single forward tensor or a collection of forward tensors,
//! useful when a layer processes multiple tensors of different sizes
//! for example.
//! Here are different ways to access tensors of a state s:
//! idx<T> t;    // my tensor
//! state<T> s;  // a state
//! t = s;       // the first forward tensor
//! t = s.f[0];  // the first forward tensor
//! t = s.f[1];  // the second forward tensor
//! t = s.b[0];  // the first backward tensor
//! t = s.bb[0]; // the first 2nd order backward tensor.
template <typename T> class state : public idx<T> {
 public:
  // Constructors //////////////////////////////////////////////////////////////

  //! Destructor.
  virtual ~state();
  //! Constructs a state of order 0.
  state();
  //! Constructs a state of order 1 and size (s0).
  //! If p is not null, extend p and allocate memory in p.
  //! If p contains b and bb tensors, also allocate b and bb tensors.
  state(intg s0, parameter<T> *p = NULL);
  //! Constructs a state of order 2 and size (s0xs1).
  //! If p is not null, extend p and allocate memory in p.
  //! If p contains b and bb tensors, also allocate b and bb tensors.
  state(intg s0, intg s1, parameter<T> *p = NULL);
  //! Constructs a state of order 3 and size (s0xs1xs2).
  //! If p is not null, extend p and allocate memory in p.
  //! If p contains b and bb tensors, also allocate b and bb tensors.
  state(intg s0, intg s1, intg s2, parameter<T> *p = NULL);
  //! Constructs a state of order 4 and size (s0xs1xs2xs3).
  //! If p is not null, extend p and allocate memory in p.
  //! If p contains b and bb tensors, also allocate b and bb tensors.
  state(intg s0, intg s1, intg s2, intg s3, parameter<T> *p = NULL);
  //! Constructs a state of order 5 or more.
  //! If p is not null, extend p and allocate memory in p.
  //! If p contains b and bb tensors, also allocate b and bb tensors.
  state(intg s0, intg s1, intg s2, intg s3, intg s4,
        intg s5 = -1, intg s6 = -1, intg s7 = -1, parameter<T> *p = NULL);
  //! Constructor. Use the order and dimensions contained in passed idxdim d.
  //! If p is not null, extend p and allocate memory in p.
  //! If p contains b and bb tensors, also allocate b and bb tensors.
  state(const idxdim &d, parameter<T> *p = NULL);
  //! Creates a state identical to 's' (pointing to the same data).
  state(const state<T> &s);
  //! This constructor initializes as many tensors as 's', each with same order
  //! but sets all dimensions to 'dim_size'.
  //! \param n If >= 0, limit the number of tensors per propagation type
  //!   to n, otherwise allocate the same number as 's'.
  state(state<T> &s, intg dim_size, uint n = -1);
  //! Constructs a forward state from an existing tensor 'f'.
  //! Note: the data pointed to by idxs is not copied, we only create new idx
  //!   pointing to the same data.
  state(const idx<T> &f);
  //! Constructs a forward/backward state from existing tensors 'f' and 'b'.
  //! Note: the data pointed to by idxs is not copied, we only create new idx
  //!   pointing to the same data.
  state(const idx<T> &f, const idx<T> &b);
  //! Constructs a forward/backward state from existing tensors 'f' and 'b'.
  //! Note: the data pointed to by idxs is not copied, we only create new idx
  //!   pointing to the same data.
  state(const idx<T> &f, const idx<T> &b, const idx<T> &bb);

  //! Enable internal flags forbidding allocation of non-forward tensors.
  //! This is just a failsafe for users to check they don't unintentinally
  //! allocate more tensors than intended/needed.
  void set_forward_only();

  // Data manipulation methods /////////////////////////////////////////////////

  //! Zero (idx_clear) all allocated tensors.
  virtual void zero_all();
  //! Zero (idx_clear) forward tensors only.
  virtual void zero_f();
  //! Zero (idx_clear) backward tensors only.
  virtual void zero_b();
  //! Zero (idx_clear) bbackward tensors only.
  virtual void zero_bb();
  //! Empty all forward, backward and bbackward tensor vectors.
  virtual void clear_all();

  // resize methods ////////////////////////////////////////////////////////////

  //! Resize f[0], b[0] and bb[0] (if they exist). Order cannot be changed.
  virtual void resize(intg s0 = -1, intg s1 = -1, intg s2 = -1, intg s3 = -1,
                      intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 = -1);
  //! Resizes with dimensions contained in an idxdim. order cannot be changed.
  //! \param n Set the number of tensors of out to n if > 0.
  virtual void resize(const idxdim &d, intg n = -1);
  //! Resizes one dimension <dimn> with size <size>.
  //! The order cannot be changed.
  virtual void resize1(intg dimn, intg size);
  //! Resizes this state with same sizes a 's'.
  //! Both states are required to have the same order.
  virtual void resize_as(state<T>& s);
  //! Same as resize_as but leave dimension <fixed_dim> untouched.
  //! Both states are required to have the same order.
  virtual void resize_as_but1(state<T>& s, intg fixed_dim);
  //! Make sure that this state has the same number of forward tensors as 's',
  //! and that each have the same order, otherwise add or replace with
  //! tensors of correct orders but dimensions 1.
  //! \param dim_sizes If > 0, set all dimensions of out to this size
  //!   otherwise set same dimension sizes as 'in'.
  //! \param n Set the number of tensors of out to n if > 0.
  template <typename T2>
  void resize_forward_orders(state<T2>& s, intg dim_sizes = -1, intg n = -1);

  //! If necessary, resizes the backward tensors exactly like the forward ones.
  virtual void resize_b();
  //! If necessary, resizes the bbackward tensors exactly like the forward ones.
  virtual void resize_bb();

  //! Resets tensors to size d and clears them.
  //! This bypasses the order change restriction.
  //! \param n Set the number of tensors of out to n if > 0.
  virtual void reset(const idxdim &d, intg n = 1);

  //! Returns true if b contains the same number of tensors with each the same
  //! dimensions as in f.
  virtual bool allocated_b();
  //! Returns true if bb contains the same number of tensors with each the same
  //! dimensions as in f.
  virtual bool allocated_bb();

  // slicing methods /////////////////////////////////////////////////////////

  //! Same as idx::select(), applied to internal tensors.
  //! This returns a state pointing to the same data as current state,
  //! but pointing to a slice of it.
  //! \param dimension The dimension to slice.
  //! \param slice_index The slice to return.
  state<T> select_state(int dimension, intg slice_index);

  //! Same as idx::narrow(), applied to internal tensors.
  //! This returns a state_idx pointing to the same data as current state,
  //! but pointing to a slice of it.
  //! \param dimension The dimension to slice.
  //! \param slice_index The size of the slize to return.
  //! \param offset The offset to start slicing.
  virtual state<T> narrow_state(int dimension, intg size, intg offset);
  //! Returns an state of size 'size' starting at 'offset' in current vector.
  virtual state<T> narrow_state(intg size, intg offset);
  //! Narrows this multi-state given multiple input 'regions'.
  virtual state<T> narrow_state(mfidxdim &regions);
  //! Narrows this multi-state given multiple input 'regions'.
  virtual state<T> narrow_state(midxdim &regions);
  //! Narrows this multi-state given multiple input 'regions' to the maximum
  //! possible region (output may be smaller than requested regions).
  virtual state<T> narrow_state_max(mfidxdim &regions);

  //! Returns a state containing f tensors at index 'i' only.
  state<T> get_f(int i);
  //! Returns a state containing f/b tensors at index 'i' only.
  state<T> get_b(int i);
  //! Returns a state containing f/b/bb tensors at index 'i' only.
  state<T> get_bb(int i);

  //! Pushes back to the f tensor vector, and links it to this state's main
  //! tensor if first element of f.
  virtual void add_f(idx<T> &m);
  //! Pushes back to the f tensor vector, and links it to this state's main
  //! tensor if first element of f.
  virtual void add_f(idx<T> *m);
  //! Pushes back a new copy of (rather than increment m's reference counter)
  //!  to the f tensor vector, and links it to this state's main
  //! tensor if first element of f.
  virtual void add_f_new(const idx<T> &m);

  // copy methods //////////////////////////////////////////////////////////////

  //! Returns a deep copy of this state.
  virtual state<T> deep_copy();
  //! Copy all data from 'other' into this state.
  virtual void deep_copy(state<T> &other);
  //! Copy matrices 'other' into internal forward tensors.
  virtual void shallow_copy(midx<T> &other);
  //! Returns a matrix pointing to internal forward tensors.
  virtual midx<T> shallow_copy_midx();

  //! Assignment operator, involves assignment of internal idx
  //! (avoid using this in critical loops).
  virtual state<T>& operator=(const state<T>& other);

  //! Narrows this multi-state's x buffer given multiple input 'regions'
  //! and puts result in multi-idx 'all'.
  virtual void get_midx(mfidxdim &regions, midx<T> &all);
  //! Narrows this multi-state's x buffer given multiple input 'regions'
  //! to the maximum possible region (output may be smaller than requested
  //! regions) and puts result in multi-idx 'all'.
  virtual void get_max_midx(mfidxdim &regions, midx<T> &all);
  //! Narrows this multi-state's x buffer given multiple input 'regions'
  //! and pad areas that do not overlap, then puts result in multi-idx 'all'.
  virtual void get_padded_midx(mfidxdim &regions, midx<T> &all);

  // info printing /////////////////////////////////////////////////////////////

  //! Prints a string describing internal tensors.
  virtual void pretty() const;
  //! Returns a string describing internal tensors.
  virtual std::string pretty_str() const;
  //! Returns a string with all values of internal tensors.
  virtual std::string str() const;
  //! Prints all elements to output (same as str()).
  virtual void print() const;
  //! Returns a string with statistics about all internal tensors (min/max/mean)
  virtual std::string info();

  // internal methods ////////////////////////////////////////////////////////
 protected:
  //! Initialize defaults.
  void init();
  //! Allocate internal tensors.
  void allocate(const idxdim &d, parameter<T> *p);
  //! This assigns f[0] to (idx<T>)*this, then links f[0] to *this.
  void link_f0();
  //! This assigns to f[0] a reference to (idx<T>)*this.
  void link_main_to_f0();
  //! Resize 'in' exactly as 'out' if needed.
  void resize_vidx_as(svector<idx<T> > &in, svector<idx<T> > &out);
  //! Make sure that this tensor vector out has the same number of tensors as in
  //! and that each have the same order, otherwise add or replace with
  //! tensors of correct orders but dimensions 'dim_sizes'.
  //! \param dim_sizes If > 0, set all dimensions of out to this size
  //!   otherwise set same dimension sizes as 'in'.
  //! \param n Set the number of tensors of out to n if > 0.
  void resize_vidx_orders(svector<idx<T> > &in, svector<idx<T> > &out,
                          intg dim_sizes = -1, intg n = -1);
  //! Reset out to min(n, in.size()) tensors with same orders as the ones
  //! in 'in' but with dimensions 'dim_sizes'.
  //! \param dim_sizes If > 0, set all dimensions of out to this size
  //!   otherwise set same dimension sizes as 'in'.
  //! \param n Set the number of tensors of out to n if > 0.
  void reset_tensors(svector<idx<T> > &in, svector<idx<T> > &out,
                     intg dim_sizes = -1, intg n = -1);

  // friends ///////////////////////////////////////////////////////////////////

  template <typename T2> friend void state_copy(state<T2> &in, state<T2> &out);

  // member variables //////////////////////////////////////////////////////////
 public:
  svector<idx<T> > f; //!< Forward tensors.
  svector<idx<T> > b; //!< Backward tensors.
  svector<idx<T> > bb; //!< 2nd order backward tensors.
 private:
  bool forward_only;
};

// state operations //////////////////////////////////////////////////////////

//! Performs a deep copy of in's tensor to out's tensors.
template <typename T> void state_copy(state<T> &in, state<T> &out);

// stream operators ////////////////////////////////////////////////////////////

template <typename T, class stream>
EXPORT stream& operator<<(stream &out, const state<T> &s);
template <typename T, class stream>
EXPORT stream& operator<<(stream &out, const svector<state<T> > &s);
template <typename T, class stream>
EXPORT stream& operator<<(stream &out, svector<state<T> > &s);

// state looper ////////////////////////////////////////////////////////////////

// An iterator class for states.
template <typename T> class state_looper : public state<T> {
 public:
  //! Constructor to loop over dimension d.
  state_looper(state<T> &s, int d);
  //! Destructor.
  virtual ~state_looper();
  //! return true if loop is over
  bool notdone();
  //! increment to next item.
  void next();

  // members
 public:
  idxlooper<T> lf, lb, lbb;
};

//! loop macro on 1 state
#define state_eloop1(dst0, src0, type0)                 \
  state_looper<type0> dst0(src0, (src0).order() - 1);   \
  for ( ; dst0.notdone(); dst0.next())

//! loop macro on 2 state
#define state_eloop2(dst0,src0,type0,dst1,src1,type1)   \
  if ((src0).dim((src0).order() - 1)                    \
      != (src1).dim((src1).order() - 1))                \
    eblerror("incompatible state for eloop\n");         \
  state_looper<type0> dst0(src0,(src0).order()-1);      \
  state_looper<type1> dst1(src1,(src1).order()-1);      \
  for ( ; dst0.notdone(); dst0.next(), dst1.next())

//! loop macro on 3 state_idx
#define state_eloop3(src0,type0,dst1,src1,type1,dst2,src2,type2)	\
  if (((src0).dim((src0).order() - 1)                                   \
       != (src1).dim((src1).order() - 1))				\
      || ((src0).dim((src0).order() - 1)				\
	  != (src2).dim((src2).order() - 1)))                           \
    eblerror("incompatible idxs for eloop\n");				\
  state_looper<type0> dst0(src0,(src0).order()-1);			\
  state_looper<type1> dst1(src1,(src1).order()-1);			\
  state_looper<type2> dst2(src2,(src2).order()-1);			\
  for ( ; dst0.notdone(); dst0.next(), dst1.next(), dst2.next())

} // namespace ebl {

#include "ebl_state.hpp"

#endif /* EBL_STATE_H_ */
