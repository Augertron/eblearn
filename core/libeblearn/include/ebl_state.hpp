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

// state /////////////////////////////////////////////////////////////////////

template<typename T>
state<T>::~state() {
	// remove f[0] which is linked to main tensor, so that it calls unlock()
	// on it before we decrease refcount. otherwise, unlock is called
	// after refcount-- which then tries to delete this twice.
	f.remove(0);
	// because we're already deleting this object, just decrease refcount
	// rather than calling unlock() which would attempt to delete this again.
	this->refcount--;
}

template<typename T>
state<T>::state() : idx<T>() {
	init();
}

template<typename T>
state<T>::state(intg s0, parameter<T> *p) : idx<T>() {
	init();
	idxdim d(s0);
	allocate(d, p);
}

template<typename T>
state<T>::state(intg s0, intg s1, parameter<T> *p) : idx<T>() {
	init();
	idxdim d(s0, s1);
	allocate(d, p);
}

template<typename T>
state<T>::state(intg s0, intg s1, intg s2, parameter<T> *p) : idx<T>() {
	init();
	idxdim d(s0, s1, s2);
	allocate(d, p);
}

template<typename T>
state<T>::state(intg s0, intg s1, intg s2, intg s3, parameter<T> *p) : idx<T>() {
	init();
	idxdim d(s0, s1, s2, s3);
	allocate(d, p);
}

template<typename T>
state<T>::state(intg s0, intg s1, intg s2, intg s3, intg s4,
		intg s5, intg s6, intg s7, parameter<T> *p) : idx<T>() {
	init();
	idxdim d(s0, s1, s2, s3, s4, s5, s6, s7);
	allocate(d, p);
}

template<typename T>
state<T>::state(const idxdim &d, parameter<T> *p) : idx<T>() {
	init();
	allocate(d, p);
}

template <typename T>
state<T>::state(const state<T> &s) : idx<T>() {
	init();
	forward_only = s.forward_only;
	f.clear();
	// forward
	for (uint i = 0; i < s.f.size(); ++i)
		if (s.f.exists(i)) add_f_new(s.f.at_const(i));
	// backward
	for (uint i = 0; i < s.b.size(); ++i)
		if (s.b.exists(i)) b.push_back_new(s.b.at_const(i));
	// bbackward
	for (uint i = 0; i < s.bb.size(); ++i)
		if (s.bb.exists(i)) bb.push_back_new(s.bb.at_const(i));
}

template <typename T>
state<T>::state(state<T> &s, intg dim_size, uint n) : idx<T>() {
	init();
	forward_only = s.forward_only;
	reset_tensors(s.f, f, dim_size, n);
	if (!s.b.empty()) reset_tensors(s.b, b, dim_size, n);
	if (!s.bb.empty()) reset_tensors(s.bb, bb, dim_size, n);
	link_f0();
	zero_all();
}

template<typename T>
state<T>::state(const idx<T> &ft) : idx<T>() {
	init();
	f.clear();
	add_f_new(ft);
}

template<typename T>
state<T>::state(const idx<T> &ft, const idx<T> &bt) : idx<T>() {
	init();
	f.clear();
	add_f_new(ft);
	b.push_back_new(bt);
}

template<typename T>
state<T>::state(const idx<T> &ft, const idx<T> &bt, const idx<T> &bbt)
		: idx<T>() {
	init();
	f.clear();
	add_f_new(ft);
	b.push_back_new(bt);
	bb.push_back_new(bbt);
}

template <typename T>
void state<T>::set_forward_only() {
	forward_only = true;
}

// data manipulation methods /////////////////////////////////////////////////

template <typename T> void state<T>::zero_all() {
	zero_f();
	zero_b();
	zero_bb();
}

template <typename T> void state<T>::zero_f() {
	for (typename svector<idx<T> >::iterator i = f.begin(); i != f.end(); ++i)
		if (i.exists()) idx_clear(*i);
}

template <typename T> void state<T>::zero_b() {
	if (!b.empty()) {
		for (typename svector<idx<T> >::iterator i = b.begin(); i != b.end(); ++i)
			if (i.exists()) idx_clear(*i);
	}
}

template <typename T> void state<T>::zero_bb() {
	if (!b.empty()) {
		for (typename svector<idx<T> >::iterator i = bb.begin(); i != bb.end(); ++i)
			if (i.exists()) idx_clear(*i);
	}
}

template <typename T> void state<T>::clear_all() {
	f.clear();
	b.clear();
	bb.clear();
}

// resize methods ////////////////////////////////////////////////////////////

template <typename T>
void state<T>::resize(intg s0, intg s1, intg s2, intg s3,
					intg s4, intg s5, intg s6, intg s7) {
	if (!idx<T>::same_dim(s0, s1, s2, s3, s4, s5, s6, s7)) {
		idx<T>::resize(s0, s1, s2, s3, s4, s5, s6, s7);
		if (!b.empty()) b[0].resize(s0, s1, s2, s3, s4, s5, s6, s7);
		if (!bb.empty()) bb[0].resize(s0, s1, s2, s3, s4, s5, s6, s7);
	}
}

template <typename T>
void state<T>::resize(const idxdim &d, intg n) {
	for (uint i = 0; i < std::max((intg)1, n); ++i) {
		if (!f.exists(i)) {
			f.push_back(new idx<T>(d));
			if (!b.empty()) b.push_back(new idx<T>(d));
			if (!bb.empty()) bb.push_back(new idx<T>(d));
		} else if (!f[i].same_dim(d)) {
			f[i].resize(d);
			if (b.exists(i)) b[i].resize(d);
			if (bb.exists(i)) bb[i].resize(d);
		}
	}
}

template <typename T>
void state<T>::resize1(intg dimn, intg size) {
	if (idx<T>::dim(dimn) != size) {
		idx<T>::resize1(dimn, size);
		if (!b.empty()) b[0].resize(dimn, size);
		if (!bb.empty()) bb[0].resize(dimn, size);
	}
}

template <typename T>
void state<T>::resize_as(state<T>& other) {
	resize_vidx_as(other.f, f);
	link_f0();
	resize_vidx_as(other.b, b);
	resize_vidx_as(other.bb, bb);
}

template <typename T>
void state<T>::resize_as_but1(state<T>& s, intg fixed_dim) {
	idxdim d(s.spec); // use same dimensions as s
	d.setdim(fixed_dim, idx<T>::dim(fixed_dim));
	state<T>::resize(d);
}

template <typename T> template <typename T2>
void state<T>::resize_forward_orders(state<T2> &other, intg dim_sizes, intg n) {
	if (forward_only && !other.b.empty())
		eblerror("trying to resize current forward-only state " << *this
			 << " as a non-forward-only state " << other);
	resize_vidx_orders(other.f, f, dim_sizes, n);
	link_f0();
}

template <typename T>
void state<T>::resize_b() {
	resize_vidx_as(f, b);
}

template <typename T>
void state<T>::resize_bb() {
	resize_vidx_as(f, bb);
}

template <typename T>
void state<T>::reset(const idxdim &d, intg n) {
	bool b_exists = !b.empty();
	bool bb_exists = !bb.empty();
	f.clear();
	b.clear();
	bb.clear();
	for (uint i = 0; i < std::max((intg)1, n); ++i) {
		f.push_back(new idx<T>(d));
		if (b_exists) b.push_back(new idx<T>(d));
		if (bb_exists) bb.push_back(new idx<T>(d));
	}
	link_f0();
	zero_all();
}

template <typename T>
bool state<T>::allocated_b() {
	if (f.size() != b.size()) return false;
	for (uint i = 0; i < f.size(); ++i) {
		if (!b[i].same_dim(f[i])) return false;
	}
	return true;
}

template <typename T>
bool state<T>::allocated_bb() {
	if (f.size() != bb.size()) return false;
	for (uint i = 0; i < f.size(); ++i) {
		if (!bb[i].same_dim(f[i])) return false;
	}
	return true;
}

// slicing methods ///////////////////////////////////////////////////////////

template <typename T>
state<T> state<T>::select_state(int dimension, intg slice_index) {
	state<T> s;
	s.f.clear();
	s.forward_only = forward_only;
	// forward
	for (uint i = 0; i < f.size(); i++)
		if (f.exists(i))
			s.f.push_back_new(s.f[i].select(dimension, slice_index));
	s.link_f0();
	// backward
	for (uint i = 0; i < b.size(); i++)
		if (b.exists(i))
			s.b.push_back_new(s.b[i].select(dimension, slice_index));
	// bbackward
	for (uint i = 0; i < bb.size(); i++)
		if (bb.exists(i))
			s.bb.push_back_new(s.bb[i].select(dimension, slice_index));
	return s;
}

template <typename T>
state<T> state<T>::narrow_state(int d, intg sz, intg o) {
	state<T> s;
	s.f.clear();
	s.forward_only = forward_only;
	// forward
	for (uint i = 0; i < f.size(); i++)
		if (f.exists(i))
			s.f.push_back_new(s.f[i].narrow(d, sz, o));
	s.link_f0();
	// backward
	for (uint i = 0; i < b.size(); i++)
		if (b.exists(i))
			s.b.push_back_new(s.b[i].narrow(d, sz, o));
	// bbackward
	for (uint i = 0; i < bb.size(); i++)
		if (bb.exists(i))
			s.bb.push_back_new(s.bb[i].narrow(d, sz, o));
	return s;
}

template <typename T>
state<T> state<T>::narrow_state(intg size, intg offset) {
	if ((uint) (size + offset) > f.size())
		eblerror("cannot narrow this vector of size " << f.size()
			 << " to size " << size << " starting at offset " << offset);
	state<T> s;
	s.f.clear();
	s.forward_only = forward_only;
	for (uint i = 0; i < size; ++i) {
		s.add_f(f.at(i + offset));
		if (i < b.size()) s.b.push_back(b.at(i + offset));
		if (i < bb.size()) s.bb.push_back(bb.at(i + offset));
	}
	return s;
}

template <typename T>
state<T> state<T>::narrow_state(mfidxdim &dims) {
	midxdim d = dims;
	return this->narrow_state(d);
}

template <typename T>
state<T> state<T>::narrow_state(midxdim &dims) {
	if (dims.size() != f.size())
		eblerror("expected same size input regions and states but got: "
			 << dims << " and " << *this);
	state<T> all;
	all.f.clear();
	all.forward_only = forward_only;
	for (uint i = 0; i < dims.size(); ++i) {
		idx<T> in = f[i];
		idxdim d = dims[i];
		// narrow input, ignoring 1st dim
		for (uint j = 1; j < d.order(); ++j)
			in = in.narrow(j, d.dim(j), d.offset(j));
		all.add_f_new(in);
	}
	return all;
}

template <typename T>
state<T> state<T>::narrow_state_max(mfidxdim &dims) {
	if (dims.size() != f.size())
		eblerror("expected same size input regions and states but got: "
			 << dims << " and " << *this);
	state<T> all;
	all.forward_only = forward_only;
	all.f.clear();
	for (uint i = 0; i < dims.size(); ++i) {
		idx<T> in = f[i];
		fidxdim d = dims[i];
		// narrow input, ignoring 1st dim
		for (uint j = 1; j < d.order(); ++j)
			in = in.narrow(j, (intg) std::min(in.dim(j) - d.offset(j), d.dim(j)),
				 (intg) d.offset(j));
		all.add_f_new(in);
	}
	return all;
}

template <typename T>
state<T> state<T>::get_f(int i) {
	if (!f.exists(i))
		eblerror("requesting an f state but tensor(s) not found at index "
			 << i << " in f: " << f);
	state s(f[i]);
	s.forward_only = forward_only;
	return s;
}

template <typename T>
state<T> state<T>::get_b(int i) {
	if (!f.exists(i) || !b.exists(i))
		eblerror("requesting an f/b state but tensor(s) not found at index "
			 << i << " in f: " << f << " and b: " << b);
	state s(f[i], b[i]);
	s.forward_only = forward_only;
	return s;
}

template <typename T>
state<T> state<T>::get_bb(int i) {
	if (!f.exists(i) || !b.exists(i) || !bb.exists(i))
		eblerror("requesting an f/b/bb state but tensor(s) not found at index "
			 << i << " in f: " << f << " and b: " << b << " and bb: " << bb);
	state s(f[i], b[i], bb[i]);
	s.forward_only = forward_only;
	return s;
}

template <typename T>
void state<T>::add_f(idx<T> &m) {
	f.push_back(m);
	// make sure the first element of 0 is same as main tensor
	if (f.size() == 1) link_f0();
}

template <typename T>
void state<T>::add_f(idx<T> *m) {
	f.push_back(m);
	// make sure the first element of 0 is same as main tensor
	if (f.size() == 1) link_f0();
}

template <typename T>
void state<T>::add_f(svector<idx<T> > &m) {
	for (uint i = 0; i < m.size(); ++i)
		add_f(m[i]);
}

template <typename T>
void state<T>::add_f_new(const idx<T> &m) {
	f.push_back_new(m);
	// make sure the first element of 0 is same as main tensor
	if (f.size() == 1) link_f0();
}

template <typename T>
void state<T>::add_f_new(svector<idx<T> > &m) {
	for (uint i = 0; i < m.size(); ++i)
		add_f_new(m[i]);
}

// copy methods //////////////////////////////////////////////////////////////

template <typename T>
state<T> state<T>::deep_copy() {
	state<T> other;
	resize_vidx_as(f, other.f);
	other.link_f0();
	resize_vidx_as(b, other.b);
	resize_vidx_as(bb, other.bb);
	state_copy(*this, other);
	return other;
}

template <typename T>
void state<T>::deep_copy(state<T> &s) {
	state_copy(s, *this);
}

template <typename T> template <typename T2>
void state<T>::deep_copy(midx<T2> &s) {
	f.clear();
	idx<T2> tmp;
	int i;
	switch (s.order()) {
		case 0:
			tmp = s.mget();
			add_f(new idx<T>(tmp.get_idxdim()));
			idx_copy(tmp, *(f.at_ptr(0)));
			break ;
		case 1:
			for (i = 0; i < s.dim(0); ++i) {
				tmp = s.mget(i);
				add_f(new idx<T>(tmp.get_idxdim()));
				idx_copy(tmp, *(f.at_ptr(0)));
			}
			break ;
		default: eblerror("not implemented");
	}
}

template <typename T>
void state<T>::shallow_copy(midx<T> &s) {
	f.clear();
	for (int i = 0; i < s.dim(0); ++i) {
		idx<T> tmp = s.mget(i);
		f.push_back_new(tmp);
	}
}

template <typename T>
midx<T> state<T>::shallow_copy_midx() {
	midx<T> s(f.size());
	for (uint i = 0; i < f.size(); ++i) s.mset(f[i], i);
	return s;
}

template <typename T>
void state<T>::get_midx(mfidxdim &dims, midx<T> &all) {
	// first narrow state
	state<T> n = narrow_state(dims);
	// now set all x into an midx
	all = midx<T>(n.f.size());
	for (uint i = 0; i < n.f.size(); ++i)
		all.mset(n.f[i], i);
}

template <typename T>
void state<T>::get_max_midx(mfidxdim &dims, midx<T> &all) {
	// first narrow state
	state<T> n = narrow_state_max(dims);
	// now set all x into an midx
	all = midx<T>(n.f.size());
	for (uint i = 0; i < n.f.size(); ++i)
		all.mset(n.f[i], i);
}

template <typename T>
void state<T>::get_padded_midx(mfidxdim &dims, midx<T> &all) {
	if (dims.size() != f.size())
		eblerror("expected same size input regions and states but got: "
			 << dims << " and " << *this);
	all.clear();
	all.resize(dims.size_existing());
	uint ooff, ioff, osize, n = 0;
	bool bcopy;
	for (uint i = 0; i < dims.size(); ++i) {
		if (dims.exists(i)) {
			idx<T> in = f[i];
			idxdim d(dims[i]);
			d.setdim(0, in.dim(0));
			idx<T> out(d);
			idx_clear(out);
			all.mset(out, n);
			bcopy = true;
			// narrow input, ignoring 1st dim
			for (uint j = 1; j < d.order(); ++j) {
	// if no overlap, skip this state
	if (d.offset(j) >= in.dim(j) || d.offset(j) + d.dim(j) <= 0) {
		bcopy = false;
		break ;
	}
	// determine narrow params
	ooff = (uint) std::max(0, (int) - d.offset(j));
	ioff = (uint) std::max(0, (int) d.offset(j));
	osize = (uint) std::min(d.dim(j) - ooff, in.dim(j) - ioff);
	// narrow
	out = out.narrow(j, osize, ooff);
	in = in.narrow(j, osize, ioff);
			}
			// copy
			if (bcopy) idx_copy(in, out);
			n++;
		}
	}
}

// info printing methods /////////////////////////////////////////////////////

template <typename T>
void state<T>::pretty() const {
	std::cout << pretty_str() << std::endl;
}

template <typename T>
std::string state<T>::pretty_str() const {
	std::string s;
	s << "(f:" << to_string(f);
	if (!b.empty()) s << ",b:" << to_string(b);
	if (!b.empty()) s << ",bb:" << to_string(bb);
	s << ")";
	return s;
}

template <typename T>
std::string state<T>::str() const {
	std::string s = "(";
	// forward
	s << "f:";
	for (uint i = 0; i < f.size(); i++) {
		if (i == 0) s << idx<T>::str();
		else s << " " << f.at_const(i).str();
	}
	// backward
	s << ",b:";
	for (uint i = 0; i < b.size(); i++) s << " " << b.at_const(i).str();
	// bbackward
	s << ",bb:";
	for (uint i = 0; i < bb.size(); i++) s << " " << bb.at_const(i).str();
	s << ")";
	return s;
}

template <typename T>
void state<T>::print() const {
	std::cout << str() << std::endl;
}

template <typename T>
std::string state<T>::info() {
	std::string s = "(";
	// forward
	s << "f:";
	for (uint i = 0; i < f.size(); i++) {
		if (i == 0) s << idx<T>::info();
		else s << " " << f[i].info();
	}
	// backward
	s << ",b:";
	for (uint i = 0; i < b.size(); i++)
		s << " " << b[i].info();
	// bbackward
	s << ",bb:";
	for (uint i = 0; i < bb.size(); i++)
		s << " " << bb[i].info();
	s << ")";
	return s;
}

// protected methods /////////////////////////////////////////////////////////

template<typename T>
void state<T>::init() {
	this->lock();
	forward_only = false;
	// add main tensor as f[0]
	f.push_back(this);
}

template<typename T>
void state<T>::allocate(const idxdim &d, parameter<T> *p) {
	// allocate main forward tensor
	((idx<T>&)*this) = idx<T>(p ? p->getstorage() : NULL,
					p ? p->footprint() : 0, d);
	// allocate b and bb tensors if present in p
	if (p) {
		if (!p->b.empty())
			b.push_back(new idx<T>(p->b[0].getstorage(), p->b[0].footprint(), d));
		if (!p->bb.empty())
			bb.push_back(new idx<T>(p->bb[0].getstorage(), p->bb[0].footprint(), d));
	}
	// resize parameter accordingly
	if (p) p->resize_parameter(p->footprint() + idx<T>::nelements());
	// clear all allocated tensors
	zero_all();
}

template <typename T>
void state<T>::link_f0() {
	// only link if not already the case
	if (f.at_ptr(0) != (idx<T>*) this) {
		((idx<T>&)*this) = f[0];
		link_main_to_f0();
	}
}

template <typename T>
void state<T>::link_main_to_f0() {
	f.set((idx<T>&)*this, 0); // f0 is a reference to the main tensor
}

template <typename T>
void state<T>::resize_vidx_as(svector<idx<T> > &in, svector<idx<T> > &out) {
	bool reset = false;
	bool resize = false;
	// reallocate entirely if different number of tensors
	if (in.size() != out.size()) reset = true;
	// reallocate if some orders are different
	// only resize if some dimensions are different
	if (in.size() == out.size()) {
		for (uint i = 0; i < in.size() && !reset; ++i) {
			if (in[i].order() != out[i].order()) reset = true;
			if (!in[i].same_dim(out[i])) resize = true;
		}
	}
	// reset/resize if necessary
	if (reset) reset_tensors(in, out);
	else if (resize) {
		for (uint i = 0; i < in.size(); ++i)
			if (in.exists(i)) out[i].resize(in[i].get_idxdim());
	}
}

template <typename T>
void state<T>::resize_vidx_orders(svector<idx<T> > &in, svector<idx<T> > &out,
					intg dim_size, intg n) {
	bool reset = false;
	// check we have the right number of tensors
	if (n <= 0) n = in.size();
	if ((intg) in.size() > n && (intg) out.size() < n) reset = true;
	if ((intg) in.size() <= n && out.size() != in.size()) reset = true;
	// check that all tensors have the right orders
	for (uint i = 0; !reset && i < in.size() && (intg) i < n; ++i)
		if (in[i].order() != out[i].order()) reset = true;
	// re-allocate if necessary
	if (reset) reset_tensors(in, out, dim_size, n);
}

template <typename T>
void state<T>::reset_tensors(svector<idx<T> > &in, svector<idx<T> > &out,
					 intg dim_size, intg n) {
	if (in.empty()) eblerror("expected non-empty input");
	if (n <= 0) n = (intg) in.size();
	out.clear();
	idxdim d;
	for (intg i = 0; i < n; ++i) {
		if (i < (intg) in.size()) {
			d = in[i].get_idxdim();
			if (dim_size > 0) d.setdims(dim_size);
		}
		out.push_back(new idx<T>(d));
	}
}

// state operations //////////////////////////////////////////////////////////

template <typename T, typename T2>
void state_copy(state<T> &in, state<T2> &out) {
	if (in.f.empty()) eblerror("expected at least 1 forward tensor");
	out.forward_only = in.forward_only;
	// forward
	for (uint i = 0; i < in.f.size(); i++)
		if (in.f.exists(i)) {
			// increase number of output tensors if necessary
			if (i >= out.f.size()) out.add_f(new idx<T2>(in.f[i].get_idxdim()));
			idx_copy(in.f[i], out.f[i]);
		} else out.f.push_back_empty();
	// backward
	for (uint i = 0; i < in.b.size(); i++)
		if (in.b.exists(i)) {
			// increase number of output tensors if necessary
			if (i >= out.f.size()) out.f.push_back(new idx<T2>(in.f[i].get_idxdim()));
			idx_copy(in.b[i], out.b[i]);
		} else out.b.push_back_empty();
	// bbackward
	for (uint i = 0; i < in.bb.size(); i++)
		if (in.bb.exists(i)) {
			// increase number of output tensors if necessary
			if (i >= out.f.size()) out.f.push_back(new idx<T2>(in.f[i].get_idxdim()));
			idx_copy(in.bb[i], out.bb[i]);
		} else out.bb.push_back_empty();
}

// stream operators //////////////////////////////////////////////////////////

template <typename T, class stream>
EXPORT stream& operator<<(stream &out, const state<T> &s) {
	out << s.pretty_str();
	return out;
}

template <typename T, class stream>
EXPORT stream& operator<<(stream &out, const svector<state<T> > &v) {
	std::string s = "[ ";
	for (typename svector<state<T> >::const_iterator i = v.begin();
			 i != v.end(); ++i) {
		if (i.exists()) s << i->str() << " ";
		else s << " null ";
	}
	s << "]";
	out << s;
	return out;
}

template <typename T, class stream>
EXPORT stream& operator<<(stream &out, svector<state<T> > &v) {
	std::string s = "[ ";
	for (typename svector<state<T> >::iterator i = v.begin();
			 i != v.end(); ++i) {
		if (i.exists()) s << i->str() << " ";
		else s << " null ";
	}
	s << "]";
	out << s;
	return out;
}

// state looper //////////////////////////////////////////////////////////////

template <typename T>
state_looper<T>::state_looper(state<T> &s, int d)
		: lf(s.f[0], d), lb(s.b[0], d), lbb(s.bb[0], d) {
	((state<T>&)*this) = s.select(d, 0);
}

template <typename T>
state_looper<T>::~state_looper() {
}

template <typename T>
void state_looper<T>::next() {
	lf.next();
	lb.next();
	lbb.next();
	state<T>::f[0] = lf;
	this->link_f0();
	state<T>::b[0] = lb;
	state<T>::bb[0] = lbb;
}

template <typename T>
bool state_looper<T>::notdone() {
	return lf.notdone();
}

} // end namespace ebl
