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
  flat_merge_module(std::vector<Tstate**> &inputs_, idxdim &in_, midxdim &ins_,
		    fidxdim &stride_, mfidxdim &strides_,
		    const char *name_, const char *list)
    : m2s_module<T,Tstate>(inputs_.size() + 1, name_), inputs(inputs_),
      din(in_), dins(ins_), stride(stride_), strides(strides_), in0(NULL),
      use_pinputs(false) {
    if (list)
      merge_list = list;
    // allocate zpad vector
    zpads.assign(strides.size(), NULL);
  }

  template <typename T, class Tstate>
  flat_merge_module<T, Tstate>::
  flat_merge_module(std::vector<mstate<Tstate>**> &inputs_,
  		    idxdim &in_, midxdim &ins_, fidxdim &stride_,
		    mfidxdim &strides_, const char *name_, const char *list)
    : m2s_module<T,Tstate>(inputs_.size() + 1, name_), //msinputs(inputs_),
      din(in_), dins(ins_), stride(stride_), strides(strides_),
      use_pinputs(false)  {
    if (list)
      merge_list = list;
    // allocate zpad vector
    zpads.assign(strides.size(), NULL);
    eblerror("not implemented");
  }

  template <typename T, class Tstate>
  flat_merge_module<T, Tstate>::
  flat_merge_module(midxdim &ins_, mfidxdim &strides_, bool bpad_,
		    const char *name_, mfidxdim *scales_,
		    /*TEMP*/ intg hextra_, intg wextra_, float ss_, float edge_)
    : m2s_module<T,Tstate>(ins_.size(), name_), in0(NULL), use_pinputs(true),
      bpad(bpad_),
      //TEMP
      hextra(hextra_), wextra(wextra_), subsampling(ss_), edge(edge_)
  {
    if (scales_)
      scales = *scales_;
    // check there are enough elements
    if (ins_.size() < 1 || strides_.size() < 1
	|| ins_.size() != strides_.size())
      eblerror("expected at least 1 dim and stride (matching) but got: dims: "
	       << ins_.size() << " strides: " << strides_.size());
    // separate first dim/strides from rest
    din = ins_[0];
    stride = strides_[0];
    // add remaining ones
    for (uint i = 0; i < ins_.size(); ++i) {
      dins.push_back(ins_[i]);
      strides.push_back(strides_[i]);
    }
    // allocate zpad vector
    zpads.assign(strides.size(), NULL);
  }

  template <typename T, class Tstate>
  flat_merge_module<T, Tstate>::~flat_merge_module() {
    // clean up zero padding modules
    for (uint i = 0; i < zpads.size(); ++i)
      if (zpads[i])
	delete zpads[i];
  }

  //////////////////////////////////////////////////////////////////////////////
  // generic state methods

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    // pad each input so that all windows start centered on the first actual
    // pixel top right

    // if (inputs.size() == 0)
    //   eblerror("no inputs to merge");
    // feature size for main input
    intg fsize = din.dim(0) * din.dim(1) * in.x.dim(0);
    // number of possible windows
    intg nh = 1 + (intg) ((in.x.dim(1) - din.dim(0)) / stride.dim(0));
    intg nw = 1 + (intg) ((in.x.dim(2) - din.dim(1)) / stride.dim(1));
    // compute new size and resize output if necessary
    for (uint i = 0; i < std::max(inputs.size(), pinputs.size()); ++i) {
      idxdim &d = dins[i];
      fidxdim &s = strides[i];
      idx<T> input = (use_pinputs ? pinputs[i]->x : (*inputs[i])->x);
      fsize += d.nelements() * input.dim(0);
      // check that strides match possible windows
      intg nh2 = (intg) (1 + (input.dim(1) - d.dim(0)) / s.dim(0));
      intg nw2 = (intg) (1 + (input.dim(2) - d.dim(1)) / s.dim(1));
      if (nh2 < nh || nw2 < nw) {
	eblerror("input " << input << " and window " << d << " with stride " <<s
		 << " produce " << nh2 << "x" << nw2
		 << " outputs but expected at least " << nh << "x" << nw);
      } else if (nh2 != nh || nw2 != nw)
	EDEBUG("warning: input " << input << " and window " << d
	      << " with stride " <<s << " produce " << nh2 << "x" << nw2
	      << ", ignoring extra cells and using only " <<nh << "x" << nw);
    }
    idxdim d(fsize, nh, nw);
    if (!out.x.same_dim(d)) {
      if (out.x.order() != d.order())
	out = Tstate(d);
      else
	out.resize(d);
    }
    intg offset = 0;
    // copy main input to out
    fsize = din.nelements() * in.x.dim(0); // feat size for main input
    // loop on all possible windows for this state
    float fh, fw;
    uint uh, uw, h, w;
    for (h = 0, fh = 0; h < nh; h++, fh += stride.dim(0)) {
      for (w = 0, fw = 0; w < nw; w++, fw += stride.dim(1)) {
	// integer positions
	uh = (uint) fh;
	uw = (uint) fw;
	// select 1 output pixel in the correct feature range
	idx<T> o = out.x.select(2, w);
	o = o.select(1, h);
	o = o.narrow(0, fsize, offset);
	// select input window
	idx<T> iw = in.x.narrow(2, din.dim(1), uw);
	iw = iw.narrow(1, din.dim(0), uh);
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
    // copy inputs to out
    for (uint i = 0; i < std::max(inputs.size(), pinputs.size()); ++i) {
      idxdim dd = dins[i];
      fidxdim s = strides[i];
      idx<T> input = (use_pinputs ? pinputs[i]->x : (*inputs[i])->x);
      fsize = dd.nelements() * input.dim(0); // feature size from input
      // copy
      for (h = 0, fh = 0; h < nh; h++, fh += s.dim(0)) {
	for (w = 0, fw = 0; w < nw; w++, fw += s.dim(1)) {
	  // integer positions
	  uh = (uint) fh;
	  uw = (uint) fw;
	  // select 1 output pixel in the correct feature range
	  idx<T> o = out.x.select(2, w);
	  o = o.select(1, h);
	  o = o.narrow(0, fsize, offset);
	  // select input window
	  idx<T> iw = input.narrow(2, dd.dim(1), uw);
	  iw = iw.narrow(1, dd.dim(0), uh);
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
    cout << describe() << ": " << in.x << " (in " << din
	 << " stride " << stride << ")";
    for (uint i = 0; i < std::max(inputs.size(), pinputs.size()); ++i)
      cout << " + " << (use_pinputs ? pinputs[i]->x : (*inputs[i])->x)
	   << " (in " << dins[i] << " stride " << strides[i] << ")";
    cout << " -> " << out.x << endl;
#endif
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    // if (inputs.size() == 0)
    //   eblerror("no inputs to merge");
    // copy out to main input
    intg offset = 0;
    idx<T> o1 = out.dx.view_as_order(1);
    idx<T> o = o1.narrow(0, in.dx.nelements(), offset);
    idx<T> input = in.dx.view_as_order(1);
    idx_add(o, input, input);
    offset += input.nelements();
    // copy out to inputs
    for (uint i = 0; i < std::max(inputs.size(), pinputs.size()); ++i) {
      input = (use_pinputs ? pinputs[i]->dx : (*inputs[i])->dx);
      input = input.view_as_order(1);
      o = o1.narrow(0, input.nelements(), offset);
      idx_add(o, input, input);
      offset += input.nelements();
    }
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::bbprop(Tstate &in,
					Tstate &out) {
    // if (inputs.size() == 0)
    //   eblerror("no inputs to merge");
    // copy out to main input
    intg offset = 0;
    idx<T> o1 = out.ddx.view_as_order(1);
    idx<T> o = o1.narrow(0, in.ddx.nelements(), offset);
    idx<T> input = in.ddx.view_as_order(1);
    idx_add(o, input, input);
    offset += input.nelements();
    // copy out to inputs
    for (uint i = 0; i < std::max(inputs.size(), pinputs.size()); ++i) {
      input = (use_pinputs ? pinputs[i]->ddx : (*inputs[i])->ddx);
      input = input.view_as_order(1);
      o = o1.narrow(0, input.nelements(), offset);
      idx_add(o, input, input);
      offset += input.nelements();
    }
  }

  //////////////////////////////////////////////////////////////////////////////

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

  // template <typename T, class Tstate>
  // void flat_merge_module<T, Tstate>::set_paddings(mfidxdim &pads) {
  //   paddings = pads;
  //   EDEBUG(this->name() << ": setting paddings to " << paddings);
  // }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::set_offsets(vector<vector<int> > &off) {
    offsets = off;
    EDEBUG(this->name() << ": setting offsets to " << offsets);
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::set_strides(mfidxdim &s) {
    strides = s;
    cout << this->name() << ": setting strides to " << strides << endl;
  }

  template <typename T, class Tstate>
  void flat_merge_module<T, Tstate>::
  fprop(mstate<Tstate> &in, Tstate &out) {
    LOCAL_TIMING_START(); // profiling
    EDEBUG(this->name() << ": " << in << ", wins: " << dins << ", strides: "
	  << strides << ", scales: " << scales << ", paddings: " << paddings);
    padded.resize(in);
    //    strides.clear();
    idxdim dref, dref2;
    // loop on each state
    //float pix1 = .5;
    for (uint k = 0; k < in.size(); ++k) {
      Tstate i = in[k];
      Tstate &p = padded[k];
      idxdim d = dins[k];
      fidxdim &s = strides[k];
      // pad each input so that all windows start centered on the first actual
      // pixel top right
      if (bpad) {
	// narrow input if specified
	if (k < offsets.size()) {
	  vector<int> &off = offsets[k];
	  if (off.size() != 4) eblerror("expected 4");
	  int oh0 = (int) (off[0]);
	  int ow0 = (int) (off[1]);
	  int oh1 = (int) (off[2]);
	  int ow1 = (int) (off[3]);
	  // int oh0 = (int) (off[0] * s.dim(0));
	  // int ow0 = (int) (off[1] * s.dim(1));
	  // int oh1 = (int) (off[2] * s.dim(0));
	  // int ow1 = (int) (off[3] * s.dim(1));
	  if (oh0 < 0) {
	    int sz2 = i.x.dim(1) + oh0 + oh1;
	    if (sz2 <= 0) {
	      eblwarn("trying to narrow dim 1 of " << i.x << " to size "<< sz2);
	    } else {
	      EDEBUG("narrowing height with offset " << -oh0 << " in " << i.x);
	      i = i.narrow(1, sz2, -oh0);
	    }
	  }
	  if (ow0 < 0) {
	    int sz2 = i.x.dim(2) + ow0 + ow1;
	    if (sz2 <= 0) {
	      eblwarn("trying to narrow dim 2 of " << i.x << " to size "<< sz2);
	    } else {
	      EDEBUG("narrowing width with offset " << -ow0 << " in " << i.x);
	      i = i.narrow(2, sz2, -ow0);
	    }
	  }
	}

	if (scales.size() != strides.size())
	  eblerror("expected scales to be the same size as strides but got "
		   << scales << " when strides are " << strides);
	//if (k % 4 == 0) pix1 *= 2;
	//float pix1 = scales[k].dim(0);
	//idxdim d4 = compute_pad(d, subsampling, edge, pix1, s);
	// fidxdim fpads;
	// if (k < paddings.size()) fpads = paddings[k];
	// idxdim d4 = fpads;

	// //idxdim d4 = compute_pad(d, 1, 0, pix1, s);
	// //	idxdim d4 = compute_pad(d, 4, 6, pix1, s);
	// // idxdim d4 = compute_pad(d, 6, 12, pix1, s);
	// //padder.set_kernel(d4);
	// padder.set_paddings(d4);
	// // add extra padding at ends if necessary to match target
	// //	intg hextra = 2, wextra = 2;
	// idxdim pads = padder.get_paddings();
	// // if (k > 0) {
	// //   hextra = (intg) (dref.dim(0) - ((i.x.dim(1) + pads.dim(0) + pads.dim(2)
	// // 				  - (d.dim(0) -d.dim(0) % 2)) / s.dim(0)));
	// //   wextra = (intg) (dref.dim(1) - ((i.x.dim(2) + pads.dim(1) + pads.dim(3)
	// // 				   - (d.dim(1) -d.dim(1) % 2)) / s.dim(1)));
	// // }
	// if (k > 0) {
	//   pads.setdim(2, pads.dim(2) + std::max(0, (int) hextra));
	//   pads.setdim(3, pads.dim(3) + std::max(0, (int) wextra));
	// }
	// padder.set_paddings(pads);

	// add padding if missing to reach target (failsafe)
	idxdim pads(0,0,0,0);// = padder.get_paddings();
	intg sh = (intg) ((i.x.dim(1) + pads.dim(0) + pads.dim(2)
			   - d.dim(0) + 1) / s.dim(0));
	intg sw = (intg) ((i.x.dim(2) + pads.dim(1) + pads.dim(3)
			   - d.dim(1) + 1) / s.dim(1));
	bool w = false;
	if (k > 0 && sh < dref.dim(0)) {
	  pads.setdim(2, pads.dim(2) + std::max(0, (int) (dref.dim(0) - sh)));
	  w = true;
	}
	if (k > 0 && sw < dref.dim(1)) {
	  pads.setdim(3, pads.dim(3) + std::max(0, (int) (dref.dim(1) - sw)));
	  if (!this->silent && pads.maxdim() > 2)
	  w = true;
	}
	if (w && !this->silent && pads.maxdim() > 2)
	    eblwarn("adding extra padding "<<pads<<" to match target " << dref);

	padder.set_paddings(pads);

	// EDEBUG("before adding padding: " << pads);
	// // add fixed extra padding
	// pads = padder.get_paddings();
	// sh = (intg) (4 * s.dim(0));
	// sw = (intg) (4 * s.dim(1));
	// pads.setdim(0, pads.dim(0) + sh);
	// pads.setdim(2, pads.dim(2) + sh);
	// pads.setdim(1, pads.dim(1) + sw);
	// pads.setdim(3, pads.dim(3) + sw);
	// padder.set_paddings(pads);
	// EDEBUG("after adding padding: " << pads);
	// pad
	padder.fprop(i, p);
	// } else {
	//   if (i.x.contiguousp()) padded[k] = i;
	//   else {
	//     if (padded[k].x.get_idxdim() != i.x.get_idxdim())
	//       padded[k] = Tstate(i.x.get_idxdim());
	//     idx_copy(i.x, padded[k].x);
	//   }
	// }
      } else padded[k] = in[k];
      // compute number of outputs for this kernel
      idxdim dout((intg) ((p.x.dim(1) - d.dim(0) + 1) / s.dim(0)),
		  (intg) ((p.x.dim(2) - d.dim(1) + 1) / s.dim(1)));
      idxdim dout2((intg) ((p.x.dim(1) - d.dim(0) + 1)),
		   (intg) ((p.x.dim(2) - d.dim(1) + 1)));
//       idxdim dout((p.x.dim(1) - (d.dim(0) - d.dim(0) % 2)) / s.dim(0),
// 		  (p.x.dim(2) - (d.dim(1) - d.dim(1) % 2)) / s.dim(1));
      // use 1st dout as reference
      if (k == 0) {
	dref = dout;
	dref2 = dout2;
      }
      EDEBUG(this->name() << ": in " << p.x << " (min: " << idx_min(p.x)
	    << ", max: " << idx_max(p.x) << ") with window " << d
	    << " and stride " << s << " -> " << dout);


      // // adjust strides so that all states produce dref outputs
      // fidxdim ss(1.0, 1.0);
      // if (k > 0)
      // 	ss = fidxdim(dout2.dim(0) / (float) dref2.dim(0),
      // 		     dout2.dim(1) / (float) dref2.dim(1));
      // strides[k] = ss;
      // EDEBUG("setting stride to " << ss << " for input " << p.x << " and window "
      // 	    << d << " to produce " << dref << " outputs");
    }
    LOCAL_TIMING_REPORT("merge padding");


    // if (inputs.size() == 0)
    //   eblerror("no inputs to merge");
    // feature size for main input
    idx<T> &in0 = padded[0].x;
    intg fsize = din.dim(0) * din.dim(1) * in0.dim(0);
    // number of possible windows
    // intg nh = 1 + (intg) ((in0.dim(1) - din.dim(0)) / stride.dim(0));
    // intg nw = 1 + (intg) ((in0.dim(2) - din.dim(1)) / stride.dim(1));
    intg nh = dref.dim(0), nw = dref.dim(1);
    // compute new size and resize output if necessary
    for (uint i = 1; i < padded.size(); ++i) {
      idxdim &d = dins[i];
      fidxdim &s = strides[i];
      idx<T> &input = padded[i].x;
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
    for (uint i = 0; i < padded.size(); ++i) {
      idxdim dd = dins[i];
      intg dd0 = dd.dim(0), dd1 = dd.dim(1);
      fidxdim s = strides[i];
      float s0 = s.dim(0), s1 = s.dim(1);
      idx<T> &input = padded[i].x;
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
    cout << describe() << ": " << in0 << " (in " << din
	 << " stride " << stride << ")";
    for (uint i = 1; i < padded.size(); ++i)
      cout << " + " << padded[i].x
	   << " (in " << dins[i] << " stride " << strides[i] << ")";
    cout << " -> " << out.x << endl;
    cout << "output min: " << idx_min(out.x) << " max: " << idx_max(out.x)
	 << endl;
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

//   template <typename T, class Tstate>
//   void flat_merge_module<T, Tstate>::bprop(mstate<Tstate> &in, Tstate &out) {
//     bprop(*in0, out);
//   }

//   template <typename T, class Tstate>
//   void flat_merge_module<T, Tstate>::bbprop(mstate<Tstate> &in, Tstate &out) {
//     bbprop(*in0, out);
//   }

  template <typename T, class Tstate>
  flat_merge_module<T,Tstate>* flat_merge_module<T,Tstate>::copy() {
    flat_merge_module<T,Tstate> *l2 =
      new flat_merge_module<T,Tstate>(dins, strides, bpad, this->name(),
				      &scales);
    return l2;
  }

  //////////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  idxdim flat_merge_module<T,Tstate>::fprop_size(idxdim &isize) {
    // feature size for main input
    intg fsize = din.dim(0) * din.dim(1) * isize.dim(0);
    // number of possible windows
    intg nh = 1 + (intg) ((isize.dim(1) - din.dim(0)) / stride.dim(0));
    intg nw = 1 + (intg) ((isize.dim(2) - din.dim(1)) / stride.dim(1));
    //! Extract its dimensions, update output size
    idxdim osize(fsize, std::max((intg) 1, nh),
		 std::max((intg) 1, nw));
    fidxdim os = osize;
    isize = bprop_size(os);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim flat_merge_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    //EDEBUG(this->name() << ": " << osize << " -> ...");
    // feature size for main input
    intg fsize = (intg) (osize.dim(0) / din.dim(0) / din.dim(1));
    // number of possible windows
    intg ih = (intg) (((osize.dim(1) - 1) * stride.dim(0)) + din.dim(0));
    intg iw = (intg) (((osize.dim(2) - 1) * stride.dim(1)) + din.dim(1));
    // extract its dimensions, update output size
    fidxdim isize(fsize, ih, iw);
    // set offsets
    for (uint j = 1; j < isize.order(); ++j)
      isize.setoffset(j, (intg) (osize.offset(j) * stride.dim(j - 1)));
    return isize;
  }

  template <typename T, class Tstate>
  mfidxdim flat_merge_module<T,Tstate>::bprop_size(mfidxdim &osize) {
    //EDEBUG(this->name() << ": " << osize << " -> ...");
    if (osize.size() == 0) eblerror("expected at least 1 idxdim");
    mfidxdim isize;
    idxdim pa, d;
    fidxdim s;
    // all inputs
    //float pix1 = .5;
    for (uint i = 0; i < dins.size(); ++i) {
      if (!osize.exists(0)) {
	isize.push_back_empty();
	continue ;
      }
      idxdim o0 = osize[0];
      d = dins[i];
      s = strides[i];
      if (bpad) {
	//if (i % 4 == 0) pix1 *= 2; // TODO: get from user
	//float pix1 = scales[i].dim(0);
	//	idxdim d4 = compute_pad(d, subsampling, edge, pix1, s);
	if (i < paddings.size()) {
	  fidxdim fpads = paddings[i];
	  pa = fpads;
	}
	if (i < offsets.size()) {
	  vector<int> &off = offsets[i];
	  d.setoffset(0, (int) (-off[0] / s.dim(0)));
	  d.setoffset(1, (int) (-off[1] / s.dim(1)));
	}

	// idxdim d4 = compute_pad(d, 1, 0, pix1, s);
	//idxdim d4 = compute_pad(d, 4, 6, pix1, s);
	//	idxdim d4 = compute_pad(d, 6, 12, pix1, s);
	//pa = padder.get_paddings(d4);

	// TMP
	// intg sh = (intg) (4 * s.dim(0));
	// intg sw = (intg) (4 * s.dim(1));
	// pa.setdim(0, pa.dim(0) + sh);
	// pa.setdim(1, pa.dim(1) + sh);

      }
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
    //EDEBUG(this->name() << ": " << osize << " -> " << isize);
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
    if (bpad)
      desc << ", inputs are padded to center windows on borders";
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
  merge_module<T, Tstate>::merge_module(std::vector<Tstate**> &ins,
					intg concat_dim_,
					const char *name_,
					const char *list)
    : module_1_1<T,Tstate>(name_), inputs(ins), concat_dim(concat_dim_),
      merge_list(list) {
    // for (uint i = 0; i < ins.size(); ++i)
    //   inputs.push_back(ins[i]);
  }

  template <typename T, class Tstate>
  merge_module<T, Tstate>::merge_module(std::vector<mstate<Tstate>**> &ins,
					intg concat_dim_,
					const char *name_,
					const char *list)
    : module_1_1<T,Tstate>(name_), msinputs(ins), merge_list(list),
      concat_dim(concat_dim_) {
    eblerror("not implemented");
  }

  template <typename T, class Tstate>
  merge_module<T, Tstate>::
  merge_module(std::vector<std::vector<uint> > &states, intg concat_dim_,
	       const char *name_)
    : module_1_1<T,Tstate>(name_), states_list(states), concat_dim(concat_dim_){
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
  void merge_module<T, Tstate>::bbprop(mstate<Tstate> &in, mstate<Tstate> &out) {
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
#ifdef __DEBUG_PRINT__
    cout << describe() << ": " << in.x;
    for (uint i = 0; i < inputs.size(); ++i)
      cout << " + " << (*inputs[i])->x;
    cout << " -> " << out.x << endl;
#endif
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    // TODO: implement
  }

  template <typename T, class Tstate>
  void merge_module<T, Tstate>::bbprop(Tstate &in, Tstate &out) {
    // TODO: implement
  }

  template <typename T, class Tstate>
  std::string merge_module<T, Tstate>::describe() {
    std::string desc;
    desc << "merge module " << this->name();
    return desc;
  }

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
