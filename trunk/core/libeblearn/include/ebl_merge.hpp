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

  ////////////////////////////////////////////////////////////////
  // flat_merge_module

  template <typename T, class Tstate>
  flat_merge_module<T, Tstate>::
  flat_merge_module(midxdim &ins_, mfidxdim &strides_,
		    const char *name_, mfidxdim *scales_)
    : m2s_module<T,Tstate>(ins_.size(), name_), reference_scale(0) {
    if (scales_) scales = *scales_;
    else default_scales(ins_.size());
    // check there are enough elements
    if (ins_.size() < 1 || strides_.size() < 1
	|| ins_.size() != strides_.size())
      eblerror("expected at least 1 dim and stride (matching) but got: dims: "
	       << ins_.size() << " strides: " << strides_.size());
    // separate first dim/strides from rest
    din = ins_[0];
    din2 = ins_[0];
    stride = strides_[0];
    // add remaining ones
    for (uint i = 0; i < ins_.size(); ++i) {
      dins.push_back(ins_[i]);
      dins2.push_back(ins_[i]);
      strides.push_back(strides_[i]);
    }
  }

  template <typename T, class Tstate>
  flat_merge_module<T, Tstate>::~flat_merge_module() {
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::
  fprop(mstate<Tstate> &in, Tstate &out) {
    LOCAL_TIMING_START(); // profiling
    EDEBUG(this->name() << ": " << in << ", wins: " << dins << ", strides: "
	  << strides << ", scales: " << scales);
    //    strides.clear();

    // output size for reference scale
    idxdim dref = compute_output_sizes(in, reference_scale); 
    // check/pad output sizes for all non-reference scales to match the reference output size
    for (uint k = 0; k < in.size(); ++k)
      if (k != reference_scale) compute_output_sizes(in, k, &dref);

    LOCAL_TIMING_REPORT("merge padding");


    // if (inputs.size() == 0)
    //   eblerror("no inputs to merge");
    // feature size for main input
    idx<T> &in0 = in[0].x;
    intg fsize = din.dim(0) * din.dim(1) * in0.dim(0);
    // number of possible windows
    // intg nh = 1 + (intg) ((in0.dim(1) - din.dim(0)) / stride.dim(0));
    // intg nw = 1 + (intg) ((in0.dim(2) - din.dim(1)) / stride.dim(1));
    intg nh = dref.dim(0), nw = dref.dim(1);
    // compute new size and resize output if necessary
    for (uint i = 1; i < in.size(); ++i) {
      idxdim &d = dins[i];
      fidxdim &s = strides[i];
      idx<T> &input = in[i].x;
      fsize += d.nelements() * input.dim(0);
      // check that strides match possible windows
      intg nh2 = (intg) ceil((input.dim(1) - d.dim(0) + 1)
			     / std::max(10e-9, (double) s.dim(0)));
      intg nw2 = (intg) ceil((input.dim(2) - d.dim(1) + 1)
			     / std::max(10e-9, (double) s.dim(1)));
      if (nh2 < nh || nw2 < nw) {
	*(this->mout) << "COUT input " << input << " and window " << d << " with stride " <<s
	      << " produce " << nh2 << "x" << nw2
	     << " outputs but expected at least " << nh << "x" << nw << endl;
	eblerror("input " << input << " and window " << d << " with stride " <<s
	      << " produce " << nh2 << "x" << nw2
	      << " outputs but expected at least " << nh << "x" << nw);
      } else if (nh2 != nh || nw2 != nw)
	EDEBUG("warning: input " << input << " and window " << d << " with stride " <<s
	      << " produce " << nh2 << "x" << nw2
	      << ", ignoring extra cells and using only " <<nh << "x" << nw);
      EDEBUG("input " << i << " " << input << ", min " << idx_min(input)
	    << " max " << idx_max(input));
    }
    LOCAL_TIMING_REPORT("merge check");
    idxdim d(fsize, nh, nw);
    if (!out.x.same_dim(d)) {
      if (out.x.order() != d.order())
	out = Tstate(d);
      else
	out.resize(d);
    }
    LOCAL_TIMING_REPORT("merge resize");
    idx_clear(out.x);
    LOCAL_TIMING_REPORT("merge clear of " << out.x);

    intg offset = 0;
    int h = 0, w = 0;
    float fh, fw;
    uint uh = 0, uw = 0, uw0 = 0;
    idx<T> iw, ow, onarrowed, inarrowed;
    // copy inputs to out
    for (uint i = 0; i < in.size(); ++i) {
      idxdim dd = dins[i];
      intg dd0 = dd.dim(0), dd1 = dd.dim(1);
      fidxdim s = strides[i];
      float s0 = s.dim(0), s1 = s.dim(1);
      idx<T> &input = in[i].x;
      if (!input.contiguousp()) eblerror("expected contiguous");
      fsize = dd.nelements() * input.dim(0); // feature size from input
      onarrowed = out.x.narrow(0, fsize, offset);
      h = 0; w = 0;
      intg wmod = onarrowed.mod(2);
      // copy
      for (h = 0, fh = 0; h < nh; h++, fh += s0) {
	uh = (uint) fh;
	// select 1 output pixel in the correct feature range
	ow = onarrowed.select(2, 0);
	ow = ow.select(1, h);
	inarrowed = input.narrow(1, dd0, uh);
	intg iwmod = inarrowed.mod(2);
	uw = 0; uw0 = 0;
	iw = inarrowed.narrow(2, dd1, uw);
	for (w = 0, fw = 0; w < nw; ++w, fw += s1) {
	  // integer positions
	  uw = (uint) fw;
	  // select input window
	  if (uw != uw0)
	    iw.add_offset(iwmod);
	  // copy flat input to output
	  // TODO: tmp buffer less efficient than direct copy which but requires
	  // continuous data, make idx pointing to oo with flat's dims?
	  // idx<T> tmp(iw.get_idxdim());
	  // idx_copy(iw, tmp);
	  // iw = tmp.view_as_order(1);
	  idx_copy(iw, ow);

	  uw0 = uw;
	  ow.add_offset(wmod);
	}
      }
      offset += fsize;
    }

    LOCAL_TIMING_REPORT("merge copies");

#ifdef __DEBUG_PRINT__
    // cout << describe() << ": " << in0 << " (in " << din
    //      << " stride " << stride << ")";
    // for (uint i = 1; i < in.size(); ++i)
    //   cout << " + " << in[i].x
    //        << " (in " << dins[i] << " stride " << strides[i] << ")";
    // cout << " -> " << out.x << endl;
    // cout << "output min: " << idx_min(out.x) << " max: " << idx_max(out.x)
    //      << endl;
#endif
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::bprop(mstate<Tstate> &in, Tstate &out) {
    idx<T> o, input, o1 = out.dx.view_as_order(1);
    intg offset = 0;
    // copy out to inputs
    for (uint i = 0; i < in.size(); ++i) {
      input = in[i].dx;
      input = input.view_as_order(1);
      o = o1.narrow(0, input.nelements(), offset);
      idx_add(o, input, input);
      offset += input.nelements();
    }
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::bbprop(mstate<Tstate> &in,
					    Tstate &out) {
    idx<T> o, input, o1 = out.ddx.view_as_order(1);
    intg offset = 0;
    // copy out to inputs
    for (uint i = 0; i < in.size(); ++i) {
      input = in[i].ddx;
      input = input.view_as_order(1);
      o = o1.narrow(0, input.nelements(), offset);
      idx_add(o, input, input);
      offset += input.nelements();
    }
  }

  //////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  fidxdim flat_merge_module<T,Tstate>::fprop_size(fidxdim &isize) {
    // feature size for main input
    intg fsize = (intg) (din2.dim(0) * din2.dim(1) * isize.dim(0));
    // number of possible windows
    intg nh = 1 + (intg) ((isize.dim(1) - din2.dim(0)) / stride.dim(0));
    intg nw = 1 + (intg) ((isize.dim(2) - din2.dim(1)) / stride.dim(1));
    //! Extract its dimensions, update output size
    fidxdim osize(fsize, std::max((intg) 1, nh),
		 std::max((intg) 1, nw));
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  mfidxdim flat_merge_module<T,Tstate>::fprop_size(mfidxdim &isize) {
    EDEBUG(this->name() << ": " << isize << " f-> ...");    
    mfidxdim osize;
    if (isize.exists(0)) {
      fidxdim d = isize[0];
      d = this->fprop_size(d);
      osize.push_back(d);
    }
    EDEBUG(this->name() << ": " << isize << " f-> " << osize);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim flat_merge_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    EDEBUG(this->name() << ": " << osize << " -> ...");
    // feature size for main input
    intg fsize = (intg) (osize.dim(0) / din2.dim(0) / din2.dim(1));
    // number of possible windows
    intg ih = (intg) (((osize.dim(1) - 1) * stride.dim(0)) + din2.dim(0));
    intg iw = (intg) (((osize.dim(2) - 1) * stride.dim(1)) + din2.dim(1));
    // extract its dimensions, update output size
    fidxdim isize(fsize, ih, iw);
    // set offsets
    for (uint j = 1; j < isize.order(); ++j)
      isize.setoffset(j, (intg) (osize.offset(j) * stride.dim(j - 1)));
    return isize;
  }

  template <typename T, class Tstate>
  mfidxdim flat_merge_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    EDEBUG(this->name() << ": " << osize << " b-> ...");
    if (osize.size() == 0) eblerror("expected at least 1 idxdim");
    mfidxdim isize;
    idxdim pa, d;
    fidxdim s;
    // all inputs
    //float pix1 = .5;
    for (uint i = 0; i < dins2.size(); ++i) {
      if (!osize.exists(0)) {
	isize.push_back_empty();
	continue ;
      }
      idxdim o0 = osize[0];
      d = dins2[i];
      s = strides[i];
      d.insert_dim(0, o0.dim(0)); // add feature dimension
      // set offsets
      fidxdim fd(d);
      for (uint j = 1; j < d.order(); ++j) {
	float o = (o0.offset(j) + d.offset(j)) * s.dim(j - 1);
	if (j-1 < pa.order()) o -= pa.dim(j-1);
	fd.setoffset(j, o);
      }
      isize.push_back(fd);
    }
    EDEBUG(this->name() << ": " << osize << " -> " << isize);
    return isize;
  }

  template <typename T, class Tstate>
  std::string flat_merge_module<T, Tstate>::describe() {
    std::string desc;
    desc << "flat_merge module " << this->name() << ", merging "
	 << (int) dins.size() << " inputs: ";
    for (uint i = 0; i < dins.size(); ++i) {
      desc << " (in " << dins[i] << " stride " << strides[i];
      if (i < scales.size()) desc << " scale " << scales[i];
      desc << "), ";
    }
    return desc;
  }

  template <typename T, class Tstate>
  uint flat_merge_module<T, Tstate>::get_ninputs() {
    return (uint) dins.size();
  }

  template <typename T, class Tstate>
  mfidxdim flat_merge_module<T, Tstate>::get_strides() {
    return strides;
  }

  template <typename T, class Tstate>
  mfidxdim flat_merge_module<T, Tstate>::get_scales() {
    return scales;
  }

  template <typename T, class Tstate>
  flat_merge_module<T,Tstate>* flat_merge_module<T,Tstate>::copy() {
    flat_merge_module<T,Tstate> *l2 =
      new flat_merge_module<T,Tstate>(dins, strides, this->name(),
				      &scales);
    return l2;
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::set_offsets(vector<vector<int> > &off) {
    offsets = off;
    EDEBUG(this->name() << ": setting offsets to " << offsets);
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::set_strides(mfidxdim &s) {
    strides = s;
    fidxdim ref(1, 1);
    for (uint i = 0; i < strides.size(); ++i)
      if (ref == strides[i]) reference_scale = i;
    cout << this->name() << ": setting strides to " << strides 
	 << " with reference scale " << reference_scale << " with stride: " 
	 << strides[reference_scale] << endl;
  }

  // protected /////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  idxdim flat_merge_module<T, Tstate>::
  compute_pad(idxdim &window, float subsampling, float edge,
	      float scale, fidxdim &stride) {
    float hoff = (edge * scale * stride.dim(0)) / subsampling + .5;
    float woff = (edge * scale * stride.dim(1)) / subsampling + .5;
    idxdim d = window;
    d.setdim(0, (int) (d.dim(0) + hoff * 2));
    d.setdim(1, (int) (d.dim(1) + woff * 2));
    return d;
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::default_scales(uint n) {
    scales.clear();
    fidxdim f(1, 1);
    for (uint i = 0; i < n; ++i) scales.push_back_new(f);
  }

  template <typename T, class Tstate>
  idxdim flat_merge_module<T, Tstate>::compute_output_sizes(mstate<Tstate> &in, uint k,
							    idxdim *dref) {
    Tstate i = in[k];
    idxdim d = dins[k];
    fidxdim &s = strides[k];
    // compute number of outputs for this kernel
    idxdim dout((intg) ((i.x.dim(1) - d.dim(0) + 1) / s.dim(0)),
		(intg) ((i.x.dim(2) - d.dim(1) + 1) / s.dim(1)));
    string msg;
    msg << this->name() << ": in " << i.x << " (min: " << idx_min(i.x)
	<< ", max: " << idx_max(i.x) << ") with window " << d
	<< " and stride " << s << " -> " << dout;
    EDEBUG(msg);
    if (dout.dim(0) <= 0 || dout.dim(1) <= 0)
      eblerror("input is too small for this network: " << msg);
    return dout;
  }

  ////////////////////////////////////////////////////////////////
  // linear_merge_module

  template <typename T, class Tstate>
  linear_merge_module<T, Tstate>::
  linear_merge_module(parameter<T,Tstate> *p, intg nout_,
		      midxdim &ins_, mfidxdim &strides_,
		      const char *name_, mfidxdim *scales_)
    : flat_merge_module<T,Tstate>(ins_, strides_, name_, scales_),
      dins_original(ins_), nout(nout_) {
    // allocate convolutions for each state
    idxdim cstride(1, 1);
    for (uint i = 0; i < ins_.size(); ++i) {
      idxdim in = ins_[i];
      intg f = in.remove_dim(0);
      idx<intg> table = full_table(f, nout);
      string cname;
      cname << "linear_merge_conv" << i;
      convs.push_back(new convolution_module<T,Tstate>
		      (p, in, cstride, table, cname.c_str()));
      buffers1.push_back(new Tstate);
      // set original window sizes
      dins2[i] = in;
      if (i == 0) din2 = in;
      // set inputs windows of 1x1 for flat_merge
      in.setdims(1);
      dins[i] = in;
      if (i == 0) din = in;
    }
  }

  template <typename T, class Tstate>
  linear_merge_module<T, Tstate>::~linear_merge_module() {
    for (uint i = 0; i < convs.size(); ++i) delete convs[i];
  }

  template <typename T, class Tstate>
  void linear_merge_module<T, Tstate>::
  fprop(mstate<Tstate> &in, Tstate &out) {
    LOCAL_TIMING_START(); // profiling
    EDEBUG(this->name() << " (linear merge): " << in << ", wins: " << dins2 
	   << ", strides: " << strides << ", scales: " << scales);

    if (in.size() != convs.size())
      eblerror("expected " << convs.size() << " inputs but got "
	       << in.size());
    // linear combinations
    for (uint i = 0; i < convs.size(); ++i)
      convs[i]->fprop(in[i], buffers1[i]);
    // combine each state
    flat_merge_module<T,Tstate>::fprop(buffers1, buffer2);
    // add states together
    idxdim d(buffer2.x);
    intg thick = d.dim(0) / dins.size();
    d.setdim(0, thick);
    this->resize_output(in[0], out, &d);
    idx_clear(out.x);
    for (uint i = 0; i < dins.size(); ++i) {
      idx<T> slice = buffer2.x.narrow(0, thick, i * thick);
      idx_add(slice, out.x);
    }
    LOCAL_TIMING_REPORT("linear merge");
  }

  template <typename T, class Tstate>
  void linear_merge_module<T, Tstate>::bprop(mstate<Tstate> &in, Tstate &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tstate>
  void linear_merge_module<T, Tstate>::bbprop(mstate<Tstate> &in,
					    Tstate &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tstate>
  std::string linear_merge_module<T, Tstate>::describe() {
    std::string desc;
    desc << "linear_merge module " << this->name() << ", merging "
	 << (int) dins_original.size() << " inputs: ";
    for (uint i = 0; i < dins_original.size(); ++i) {
      desc << " (in " << dins_original[i] << " stride " << strides[i];
      if (i < scales.size()) desc << " scale " << scales[i];
      desc << "), ";
    }
    return desc;
  }

  template <typename T, class Tstate>
  linear_merge_module<T,Tstate>* linear_merge_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    linear_merge_module<T,Tstate> *l2 =
      new linear_merge_module<T,Tstate>(p, nout, dins_original, strides, 
					this->name(), &scales);
    // assign same parameter state if no parameters were specified
    if (!p) {
      for (uint i = 0; i < convs.size(); ++i)
	l2->convs[i]->kernel = convs[i]->kernel;
    }
    return l2;
  }

  ////////////////////////////////////////////////////////////////
  // mstate_merge_module

  template <typename T, class Tstate>
  mstate_merge_module<T, Tstate>::
  mstate_merge_module(midxdim &ins, mfidxdim &strides, const char *name_)
    : module_1_1<T,Tstate>(name_), dins(ins), dstrides(strides) {
  }

  template <typename T, class Tstate>
  mstate_merge_module<T, Tstate>::~mstate_merge_module() {
  }

  //////////////////////////////////////////////////////////////////////////////
  // multi-state methods

  template <typename T, class Tstate>
  void mstate_merge_module<T, Tstate>::
  fprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    // use state 0 as base for sizes
    Tstate &in0 = in[0];
    Tstate o0 = out[0];
    idxdim &d0 = dins[0];
    fidxdim &s0 = dstrides[0];
    // number of possible windows
    intg nh = (intg) (1 + (in0.x.dim(1) - d0.dim(0)) / s0.dim(0));
    intg nw = (intg) (1 + (in0.x.dim(2) - d0.dim(1)) / s0.dim(1));
    // compute new size and resize output if necessary
    intg fsize = 0;
    for (uint i = 0; i < dins.size(); ++i) {
      idxdim &d = dins[i];
      fidxdim &s = dstrides[i];
      Tstate &tin = in[i];
      fsize += d.nelements() * tin.x.dim(0);
      // check that strides match possible windows
      if (tin.x.dim(1) / s.dim(0) != nh || tin.x.dim(2) / s.dim(1) != nw)
	eblerror("input " << tin.x << " with stride " << s
		 << " does not produce " << nh << "x" << nw << " windows");
    }
    // resize output (only 1 state)
    idxdim d(fsize, nh, nw);
    if (out.size() != 1) {
      out.clear();
      out.push_back(d);
    } else {
      if (!o0.x.same_dim(d))
	o0.resize(d);
    }
    intg offset = 0;
    // copy all inputs to outputs
    for (uint i = 0; i < dins.size(); ++i) {
      idxdim &d = dins[i];
      fidxdim &s = dstrides[i];
      Tstate &tin = in[i];
      // feature size for this state
      fsize = d.nelements() * in0.x.dim(0);
      // loop on all possible windows for this state
      float fh, fw;
      uint uh, uw, h, w;
      for (h = 0, fh = 0; h < nh; h++, fh += s.dim(0)) {
	for (w = 0, fw = 0; w < nw; w++, fw += s.dim(1)) {
	  // integer positions
	  uh = (uint) h;
	  uw = (uint) w;
	  // select 1 output pixel in the corect feature range
	  idx<T> o = o0.x.select(2, w);
	  o = o.select(1, h);
	  o = o.narrow(0, fsize, offset);
	  // select input window
	  idx<T> iw = tin.x.select(2, uw);
	  iw = iw.select(1, uh);
	  // copy flat input to output
	  // TODO: tmp buffer less efficient than direct copy which but requires
	  // continuous data, make idx pointing to oo with flat's dims?
	  idx<T> tmp(iw.get_idxdim());
	  idx_copy(iw, tmp);
	  iw = tmp.view_as_order(1);
	  idx_copy(iw, o);
	}
      }
      offset += fsize;
    }
#ifdef __DEBUG_PRINT__
    // cout << describe() << ": " << in.x << " (in " << din
    // 	 << " stride " << stride << ")";
    // for (uint i = 0; i < inputs.size(); ++i)
    //   cout << " + " << (*inputs[i])->x << " (in " << dins[i]
    // 	   << " stride " << strides[i] << ")";
    // cout << " -> " << out.x << endl;
#endif
  }

  template <typename T, class Tstate>
  void mstate_merge_module<T, Tstate>::
  bprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    // expect only 1 state in output
    if (out.size() != 1)
      eblerror("expected only 1 state in output but found " << out.size());
    Tstate &to = out[0];
    idx<T> o = to.dx.view_as_order(1);
    // copy out to inputs
    intg offset = 0;
    for (uint i = 0; i < in.size(); ++i) {
      Tstate &tin = in[i];
      idx<T> ii = tin.dx.view_as_order(1);
      idx<T> oo = o.narrow(0, ii.nelements(), offset);
      idx_add(oo, ii, ii);
      offset += ii.nelements();
    }
  }

  template <typename T, class Tstate>
  void mstate_merge_module<T, Tstate>::
  bbprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    // expect only 1 state in output
    if (out.size() != 1)
      eblerror("expected only 1 state in output but found " << out.size());
    Tstate &to = out[0];
    idx<T> o = to.ddx.view_as_order(1);
    // copy out to inputs
    intg offset = 0;
    for (uint i = 0; i < in.size(); ++i) {
      Tstate &tin = in[i];
      idx<T> ii = tin.ddx.view_as_order(1);
      idx<T> oo = o.narrow(0, ii.nelements(), offset);
      idx_add(oo, ii, ii);
      offset += ii.nelements();
    }
  }

  //////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  idxdim mstate_merge_module<T,Tstate>::fprop_size(idxdim &isize) {
    // use state 0 as base for sizes
    idxdim &d0 = dins[0];
    fidxdim &s0 = dstrides[0];
    // number of possible windows
    intg nh = (intg) (1 + (isize.dim(1) - d0.dim(0)) / s0.dim(0));
    intg nw = (intg) (1 + (isize.dim(2) - d0.dim(1)) / s0.dim(1));
    // compute new size and resize output if necessary
    intg fsize = 0;
    for (uint i = 0; i < dins.size(); ++i) {
      idxdim &d = dins[i];
      fsize += d.nelements() * isize.dim(0);
    }
    //! Extract its dimensions, update output size
    idxdim osize(fsize, std::max((intg) 1, nh),
		 std::max((intg) 1, nw));
    fidxdim os = osize;
    isize = bprop_size(os);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim mstate_merge_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    // use state 0 as base for sizes
    fidxdim &d0 = dins[0];
    fidxdim &s0 = dstrides[0];
    // number of possible windows
    intg ih = (intg) (((osize.dim(1) - 1) * s0.dim(0)) + d0.dim(0));
    intg iw = (intg) (((osize.dim(2) - 1) * s0.dim(1)) + d0.dim(1));
    // compute new size and resize output if necessary
    intg fsize = osize.dim(0) / d0.dim(0) / d0.dim(1);
    //! Extract its dimensions, update output size
    fidxdim isize(fsize, ih, iw);
    return isize;
  }

  template <typename T, class Tstate>
  std::string mstate_merge_module<T, Tstate>::describe() {
    std::string desc;
    desc << "mstate_merge module " << this->name() << ", merging states ";
    for (uint i = 0; i < dins.size(); ++i)
      desc << " (in " << dins[i] << " stride " << dstrides[i] << "), ";
    return desc;
  }

  ////////////////////////////////////////////////////////////////
  // merge

  template <typename T, class Tstate>
  merge_module<T, Tstate>::
  merge_module(std::vector<std::vector<uint> > &states, intg concat_dim_,
	       const char *name_)
    : module_1_1<T,Tstate>(name_), states_list(states), concat_dim(concat_dim_){
    this->ninputs = states[0].size();
  }

  template <typename T, class Tstate>
  merge_module<T, Tstate>::~merge_module() {
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::fprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    if (states_list.size() == 0) eblerror("expected non-empty states_list");
    // resize out if necessary
    out.resize(in, states_list.size());
    // loop on each merging
    for (uint i = 0; i < states_list.size(); ++i) {
      vector<uint> ids = states_list[i];
      mstate<Tstate> mi;
      // create multi-state of states to merge
      for (uint j = 0; j < ids.size(); ++j) {
	uint id = ids[j];
	if (id >= in.size())
	  eblerror("trying to access state " << id << " but multi-state only "
		   << "contains " << in.size() << " states: " << in);
	mi.push_back(new Tstate(in[id]));
      }
      EDEBUG("merging states with ids " << ids << ": " << mi);
      // merge them
      merge(mi, out[i]);
    }
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::bprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    // loop on each merging
    for (uint i = 0; i < states_list.size(); ++i) {
      vector<uint> ids = states_list[i];
      mstate<Tstate> mi;
      // create multi-state of states to merge
      for (uint j = 0; j < ids.size(); ++j) {
	uint id = ids[j];
	if (id >= in.size())
	  eblerror("trying to access state " << id << " but multi-state only "
		   << "contains " << in.size() << " states: " << in);
	mi.push_back(new Tstate(in[id]));
      }
      // merge them
      merge_dx(mi, out[i]);
    }
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::bbprop(mstate<Tstate> &in, mstate<Tstate> &out){
    // loop on each merging
    for (uint i = 0; i < states_list.size(); ++i) {
      vector<uint> ids = states_list[i];
      mstate<Tstate> mi;
      // create multi-state of states to merge
      for (uint j = 0; j < ids.size(); ++j) {
	uint id = ids[j];
	if (id >= in.size())
	  eblerror("trying to access state " << id << " but multi-state only "
		   << "contains " << in.size() << " states: " << in);
	mi.push_back(new Tstate(in[id]));
      }
      // merge them
      merge_ddx(mi, out[i]);
    }
  }

  template <typename T, class Tstate>
  std::string merge_module<T, Tstate>::describe() {
    std::string desc;
    desc << "merge module " << this->name();
    return desc;
  }

  template <typename T, class Tstate>
  merge_module<T,Tstate>* merge_module<T,Tstate>::copy() {
    merge_module<T,Tstate> *m =
      new merge_module<T,Tstate>(states_list, concat_dim, this->name());
    return m;
  }

  template <typename T, class Tstate>
  mfidxdim merge_module<T,Tstate>::fprop_size(mfidxdim &isize) {
    EDEBUG(this->name() << ": " << isize << " f-> ...");
    mfidxdim osize;
    if (isize.exists(0)) {
      fidxdim d = isize[0];
      d.setdim(concat_dim, 0);
      for (uint i = 0; i < isize.size(); ++i)
	d.setdim(concat_dim, d.dim(concat_dim) + isize[i].dim(concat_dim));
      osize.push_back(d);
    }
    this->ninputs = isize.size();
    this->noutputs = osize.size();
    EDEBUG(this->name() << ": " << isize << " f-> " << osize);
    return osize;
  }

  template <typename T, class Tstate>
  mfidxdim merge_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    EDEBUG(this->name() << ": " << osize << " b-> ...");
    mfidxdim isize;
    for (uint i = 0; i < this->ninputs; ++i) isize.push_back_new(osize);
    EDEBUG(this->name() << ": " << osize << " b-> " << isize);
    return isize;
  }

  // internal members //////////////////////////////////////////////////////////
  
  template <typename T, class Tstate>
  void merge_module<T, Tstate>::merge(mstate<Tstate> &in, Tstate &out) {
    if (in.size() == 0) eblerror("expected at least 1 state in input");
    idxdim d(in[0].x), dtmp(in[0].x);
    // check that all inputs are compatible and compute output size
    for (uint i = 1; i < in.size(); ++i) {
      Tstate &s = in[i];
      dtmp.setdim(concat_dim, s.x.dim(concat_dim));
      if (!s.x.same_dim(dtmp))
	eblerror("expected same dimensions but got " << s.x.get_idxdim()
		 << " and " << dtmp);
      // increment dimension
      d.setdim(concat_dim, d.dim(concat_dim) + s.x.dim(concat_dim));
    }
    // check that output has the right size, if not, resize
    if (out.x.get_idxdim() != d) out.resize(d);
    // copy inputs to out
    intg offset = 0;
    idx<T> o;
    for (uint i = 0; i < in.size(); ++i) {
      Tstate &s = in[i];
      o = out.x.narrow(concat_dim, s.x.dim(concat_dim), offset);
      idx_copy(s.x, o);
      offset += s.x.dim(concat_dim);
    }
#ifdef __DEBUG_PRINT__
    cout << describe() << ": " << in[0].x;
    for (uint i = 1; i < in.size(); ++i) cout << " + " << in[i].x;
    cout << " -> " << out.x << endl;
#endif
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::merge_dx(mstate<Tstate> &in, Tstate &out) {
    // accumulate outputs to inputs
    intg offset = 0;
    idx<T> o;
    for (uint i = 0; i < in.size(); ++i) {
      Tstate &s = in[i];
      o = out.dx.narrow(concat_dim, s.x.dim(concat_dim), offset);
      idx_add(o, s.dx, s.dx);
      offset += s.x.dim(concat_dim);
    }
#ifdef __DEBUG_PRINT__
    cout << describe() << ": bprop " << in[0].x;
    for (uint i = 1; i < in.size(); ++i) cout << " + " << in[i].x;
    cout << " <- " << out.x << endl;
#endif
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::merge_ddx(mstate<Tstate> &in, Tstate &out) {
    // accumulate outputs to inputs
    intg offset = 0;
    idx<T> o;
    for (uint i = 0; i < in.size(); ++i) {
      Tstate &s = in[i];
      o = out.ddx.narrow(concat_dim, s.x.dim(concat_dim), offset);
      idx_add(o, s.ddx, s.ddx);
      offset += s.x.dim(concat_dim);
    }
#ifdef __DEBUG_PRINT__
    cout << describe() << ": bbprop " << in[0].x;
    for (uint i = 1; i < in.size(); ++i) cout << " + " << in[i].x;
    cout << " <- " << out.x << endl;
#endif
  }

  ////////////////////////////////////////////////////////////////
  // interlace

  template <typename T, class Tstate>
  interlace_module<T, Tstate>::interlace_module(uint stride_, const char *name_)
    : module_1_1<T,Tstate>(name_), stride(stride_) {
  }

  template <typename T, class Tstate>
  interlace_module<T, Tstate>::~interlace_module() {
  }

  template <typename T, class Tstate>
  void interlace_module<T, Tstate>::
  fprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    if (in.size() % stride != 0)
      eblerror("expected number of states to be a multiple of " << stride
	       << " but got: " << in);
    out.clear();
    // interlace
    for (uint i = 0; i < stride; ++i) {
      for (uint j = 0; j < in.size() / stride; ++j) {
	out.push_back(in[j * stride + i]);
      }
    }
    EDEBUG(this->name() << ": " << in << " -> " << out);
  }

  template <typename T, class Tstate>
  void interlace_module<T, Tstate>::
  bprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    not_implemented();
  }

  template <typename T, class Tstate>
  void interlace_module<T, Tstate>::
    bbprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    not_implemented();
  }

  template <typename T, class Tstate>
  mfidxdim interlace_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    if (osize.size() % stride != 0) {
      eblwarn(this->name() << ": expected midxdim size to be a multiple of "
	      << stride << " but got " << osize);
      return osize;
    }
    mfidxdim isize;
    uint step = osize.size() / stride;
    // interlace
    for (uint i = 0; i < step; ++i) {
      for (uint j = 0; j < stride; ++j) {
	if (osize.exists(j * step + i))
	  isize.push_back(osize[j * step + i]);
	else
	  isize.push_back_empty();
      }
    }
    EDEBUG(this->name() << ": " << osize << " -> " << isize);
    return isize;
  }

  template <typename T, class Tstate>
  std::string interlace_module<T, Tstate>::describe() {
    std::string desc;
    desc << "interlacing module " << this->name() << " with stride " << stride;
    return desc;
  }

  template <typename T, class Tstate>
  interlace_module<T,Tstate>* interlace_module<T,Tstate>::copy() {
    interlace_module<T,Tstate> *l2 =
      new interlace_module<T,Tstate>(stride, this->name());
    return l2;
  }

} // end namespace ebl
