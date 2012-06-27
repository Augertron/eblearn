/***************************************************************************
 *   Copyright (C) 2011 by Yann LeCun, Pierre Sermanet and Soumith Chintala*
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com, soumith@gmail.com  *
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
  // linear_module

  template <typename T, class Tstate>
  linear_module<T, Tstate>::linear_module(parameter<T,Tstate> *p,
					  intg in, intg out, const char *name_
#ifdef __CUDA__
, bool use_gpu_, int gpu_id_
#endif
)
    : module_1_1<T,Tstate>(name_), w(p, out, in) 
#ifdef __CUDA__
    ,use_gpu(use_gpu_), gpu_id(gpu_id_) 
#endif
{

#if __CUDA__
    // check precision to decide if we use TH or not
  float_precision = false;
  fstate_idx<T> temp;
  fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&temp);
  if (cont) 
    float_precision = true;
#endif
  }

  template <typename T, class Tstate>
  linear_module<T, Tstate>::~linear_module() {
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    if (!in.x.contiguousp() || !out.x.contiguousp())
      eblerror("input should be contiguous");
// #ifdef __CUDA__
//     if(float_precision && use_gpu) {
//       cuda_m2dotm2(in.x, out.x, w.x, gpu_id);
//       return;
//     }
// #endif
    // flatten dimensions starting from second one
    idxdim d(in.x);
    d.remove_dim(0);
    idx<T> inx(in.x.getstorage(), 0, in.x.dim(0), d.nelements());
    d.insert_dim(0, w.x.dim(0));
    this->resize_output(in, out, &d); // resize (iff necessary)
    idx<T> outx(out.x.getstorage(), 0, out.x.dim(0), inx.dim(1));
    // linear combination
    idx_m2dotm2(w.x, inx, outx);
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    idx<T> inx = in.x.view_as_order(1); // view as 1D idx
    idx<T> indx = in.dx.view_as_order(1); // view as 1D idx
    idx<T> outdx = out.dx.view_as_order(1); // view as 1D idx
    idx<T> twx(w.x.transpose(0, 1)); // switch dimensions 0 and 1
    if (outdx.nelements() != w.dx.dim(0))
      eblerror("output should have " << w.dx.dim(0) << " elements "
	       << "but has " << outdx.nelements() << " (" << outdx << ")");
    // bprop
    idx_m1extm1acc(outdx, inx, w.dx); // backprop to weights
    idx_m2dotm1acc(twx, outdx, indx); // backprop to input
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::bbprop(Tstate &in, Tstate &out) {
    idx<T> inx = in.x.view_as_order(1); // view as 1D idx
    idx<T> inddx = in.ddx.view_as_order(1); // view as 1D idx
    idx<T> outddx = out.ddx.view_as_order(1); // view as 1D idx
    idx<T> twx = w.x.transpose(0, 1); // switch dimensions 0 and 1
    if (outddx.nelements() != w.ddx.dim(0))
      eblerror("output should have " << w.ddx.dim(0) << " elements "
	       << "but has " << outddx.nelements() << " (" << outddx << ")");
    // bbprop
    idx_m1squextm1acc(outddx, inx, w.ddx); // backprop to weights
    idx_m2squdotm1acc(twx, outddx, inddx); // backprop to input
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::forget(forget_param_linear &fp) {
    double fanin_ = w.x.dim(1);
    double z = fp.value / pow(fanin_, fp.exponent);
    idx_aloop1(lx, w.x, T) {
      *lx = (T) fp.generator.drand(-z, z);
    }
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::normalize() {
#ifdef __CBLAS__
    norm_columns(w.x);
#else
    eblerror("norm_columns not implemented without cblas");
#endif
  }

  template <typename T, class Tstate>
  fidxdim linear_module<T, Tstate>::fprop_size(fidxdim &isize) {
    //! Update output size based on weight dimensions
    fidxdim osize = isize;
    osize.setdim(0, w.x.dim(0));
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim linear_module<T, Tstate>::bprop_size(const fidxdim &osize) {
    fidxdim isize = osize;
    isize.setdim(0, w.x.dim(1));
    return isize;
  }

  template <typename T, class Tstate>
  linear_module<T, Tstate>* linear_module<T, Tstate>::
  copy(parameter<T,Tstate> *p) {
    // new module
    linear_module<T, Tstate> *l2 =
      new linear_module<T, Tstate>(p, w.x.dim(1), w.x.dim(0), this->name());
    // assign same parameter state if no parameters were specified
    if (!p) l2->w = w;
    return l2;
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::load_x(idx<T> &weights) {
    if (!w.x.same_dim(weights)) {
      // if sizes are the same except for the feature size, load
      // into the corresponding slices with a warning
      // this allows to load grayscale pretrained weights only
      // in a grayscale + color net for example.
      idxdim d(w.x);
      d.setdim(0, weights.dim(0));
      if (d == weights.get_idxdim()) {
	cerr << "Warning: loading weights partly (the first " << d.dim(0)
	     << " features) from " << weights << " instead of entire weights ("
	     << w.x << ")." << endl;
	intg sz = std::min(w.x.dim(0), weights.dim(0));
	idx<T> slices = w.x.narrow(0, sz, 0);
	idx<T> w = weights.narrow(0, sz, 0);
	idx_copy(w, slices);
      } else
	eblthrow("expected same dimension weights but got " << w.x << " and "
		 << weights << " instead in " << this->name());
    } else
      idx_copy(weights, w.x);
  }

  template <typename T, class Tstate>
  std::string linear_module<T, Tstate>::describe() {
    std::string desc;
    desc << "linear module " << this->name() << " "
	 << w.x.dim(1) << " -> " << w.x.dim(0);
    return desc;
  }

  template <typename T, class Tstate>
  void linear_module<T, Tstate>::dump_fprop(Tstate &in, Tstate &out) {
    fprop(in, out);
    DUMP(in.x, this->name() << "_linear_module_in.x");
    DUMP(w.x, this->name() << "_linear_module_weights");
  }

  ////////////////////////////////////////////////////////////////
  // convolution_module

  template <typename T, class Tstate>
  convolution_module<T, Tstate>::
  convolution_module(parameter<T,Tstate> *p, idxdim &ker_, idxdim &stride_,
		     idx<intg> &tbl, const char *name_, bool crop_
#ifdef __CUDA__
                     , bool use_gpu_, int gpu_id_
#endif
                     )
    : module_1_1<T,Tstate>(name_), ker(ker_), stride(stride_), table(tbl),
      revtable(1,1,1), fanin(-1), warnings_shown(false),
      float_precision(false), double_precision(false), crop(crop_),
      use_ipp(false)
#ifdef __CUDA__
    , use_gpu(use_gpu_), gpu_id(gpu_id_) 
#endif
  {
    idxdim d(ker);
    d.insert_dim(0, tbl.dim(0));
    kernel = Tstate(p, d);
    // check sanity of connection table
    if (table.dim(1) != 2) // check table order
      eblerror("error: expecting table with dim 1 equal to 2 but found: " 
	       << table);
    check_table_duplicates(table);
    idx<intg> tbl0 = table.select(1, 0);
    tablemax = idx_max(tbl0);
    idx<intg> tbl1 = table.select(1, 1);
    thickness = idx_max(tbl1) + 1;
    // check table uses all inputs
    idx<bool> tblcount(tablemax + 1);
    idx_bloop1(tb, table, intg) {
      tblcount.set(true, tb.get(0));
    }
    bool not_using_all_inputs = false;
    for (int i = 0; i <= tablemax; ++i) {
      if (tblcount.get(i) == false) {
        eblwarn("input " << i 
		<< " not used by connection table in convolution module");
        not_using_all_inputs = true;
      }
    }
    fulltable = false;
    // check if its a full-table
    if ( (((tablemax + 1) * thickness) == table.dim(0)) && !not_using_all_inputs)
      fulltable = true;


    // check it the table has fixed fanin and construct the revtable
    if (!fulltable && (table.dim(0) % thickness == 0)) {
      fanin = table.dim(0) / thickness;
      revtable.resize(thickness, fanin, 2);
      // do a proper fanin check
      for (int i = 0; i < thickness; ++i) {
        int tempfan = fanin;
        for (int j=0; j < table.dim(0); ++j) {
          if( table.get(j,1) == i) {
            if(tempfan <= 0)
              break;
            revtable.set(table.get(j,0), i, tempfan - 1, 0);
            revtable.set(j, i, tempfan - 1, 1);
            tempfan--;
          }
        }
        if (tempfan != 0) {
          fanin = -1;
          break;
        }
      }
    }
#if __TH__ || __CUDA__
    // check precision to decide if we use TH or not
    fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&kernel);
    if (cont) {
      float_precision = true;
      outtmp = idx<T>(1, 1);
    }
    else {
      fstate_idx<double> *cont_d = dynamic_cast<fstate_idx<double>*>(&kernel);
      if(cont_d) {
        double_precision = true;
        outtmp = idx<T>(1, 1);
      }
    }
#else
  #ifdef __IPP__
    // check precision to decide if we use IPP or not
    fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&kernel);
    if (cont) {
      float_precision = true;
      // allocate reversed kernel
      revkernel = idx<T>(kernel.x.dim(1), kernel.x.dim(2));
      outtmp = idx<T>(1, 1);
    }
    ipp_err_printed = false;
    use_ipp = true;
  #endif
#endif
    // for external display classes, declare which internal buffers to display
    this->internals.push_back(kernel.x);
    this->internals_str.push_back("kernels");
  }

  template <typename T, class Tstate>
  convolution_module<T, Tstate>::~convolution_module() {
  }

  template <typename T, class Tstate>
  void convolution_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    if (!convolution_module<T, Tstate>::resize_output(in, out))
      return ; // do nothing if resizing failed
    // inx = input tensor over which the kernel has to be applied
    idx<T> inx = in.x;
#ifdef __CUDA__
    if (in.x.dim(0) > tablemax + 1) {
      if (tablemax == 0) {
        inx = in.x.narrow(0,1,0);
      }
      else {
        cerr << "WARNING: CUDA WILL BE DISABLED FOR MODULE: " << this->name()
             << " because all inputs are not being used and tablemax is not 0 " 
             << " (i.e. if all inputs are not used, the only supported case"
             << "using CUDA is if tablemax is 0 " <<endl;
        use_gpu = false;
      }
    }
    if(float_precision && use_gpu) {
      LOCAL_TIMING2_START();
      if (fulltable)
        cuda_convolution_3d(inx, kernel.x, out.x, stride.dim(0), stride.dim(1), 
                            gpu_id);
      else if (fanin != -1)
           cuda_convolution_3dmap(inx, kernel.x, out.x, stride.dim(0), 
                                  stride.dim(1), revtable, fanin, gpu_id);
      else
        eblerror(" Only full tables or fixed fanin tables are supported" 
                 << "in convolution_module with CUDA");
      LOCAL_TIMING2_REPORT("convgpu total execution time");
      return;
    }
#endif
    // temporarly crop input if mismatch in size
    intg ki = kernel.x.dim(1), kj = kernel.x.dim(2);
    intg si = stride.dim(0), sj = stride.dim(1);
    intg oi = inx.dim(1) - (ki - si), oj = inx.dim(2) - (kj - sj);
    if (crop && oi % stride.dim(0) != 0)
      inx = inx.narrow(1, inx.dim(1) - oi % si, 0);
    if (crop && oj % stride.dim(1) != 0)
      inx = inx.narrow(2, inx.dim(2) - oj % sj, 0);
    idx_clear(out.x);
#ifdef __TH__
    // a direct 3D-map optimization
    if((float_precision || double_precision) && in.x.order()==3) {
#ifdef __OPENMP__
      svector< idx<T> > lk_list, suin_list, sout_list;
      svector<idx<T> > outtmp_list;
      vector< idx<intg> > lt_list;
      idx_bloop2(lk, kernel.x, T, lt, table, intg) {
        lk_list.push_back(new idx<T>(lk));
        lt_list.push_back(lt);
        suin_list.push_back(new idx<T>(inx.select(0, lt.get(0))));
        sout_list.push_back(new idx<T>((out.x).select(0, lt.get(1))));
        // need own outtmp variable for parallelization
        outtmp_list.push_back(new idx<T>(outtmp.get_idxdim()));
      }
      intg i;
      intg num_outputs = lk_list.size();
#pragma omp parallel for private(i)
      for ( i = 0; i < num_outputs; ++i) {
        // 2D convolution
        th_convolution(suin_list[i], lk_list[i], outtmp_list[i], 
                       stride.dim(0), stride.dim(1)); 
      } // end of for loop
#pragma omp parallel for private(i)
      for ( i = 0; i < out.x.dim(0); ++i) {
        for(int j=0; j < lt_list.size(); j++) {
          if(lt_list[j].get(1) == i)
            th_add(outtmp_list[i],sout_list[i]);
        }
      }
#else
      //if (fulltable)
      // th_convolution_3d(inx, kernel.x, out.x, stride.dim(0), stride.dim(1));
      //else
      th_convolution_3dmap(inx, kernel.x, out.x, table, stride.dim(0), stride.dim(1));
#endif // endif __OPENMP__
      return;
    }
    else {
      // unfolding input for a faster convolution operation
      idx<T> uuin(inx.unfold(1, kernel.x.dim(1), stride.dim(0)));
      uuin = uuin.unfold(2, kernel.x.dim(2), stride.dim(1));
      idx_bloop2(lk, kernel.x, T, lt, table, intg) {
	idx<T> sout((out.x).select(0, lt.get(1)));
	idx<T> suin(uuin.select(0, lt.get(0)));
	idx_m4dotm2acc(suin, lk, sout); // 2D convolution
      }
      return;
    }
#endif // endif __TH__
    
    LOCAL_TIMING_START();
    // unfolding input for a faster convolution operation
    idx<T> uuin(inx.unfold(1, kernel.x.dim(1), stride.dim(0)));
    uuin = uuin.unfold(2, kernel.x.dim(2), stride.dim(1));
    idx_clear(out.x);
    // convolve 2D slice for each convolution kernel
    { idx_bloop2(lk, kernel.x, T, lt, table, intg) {
	idx<T> sout((out.x).select(0, lt.get(1)));
  #ifdef __IPP__
	if (float_precision && use_ipp) {
	  rev_idx2_tr(lk, revkernel);
	  //	  idx_clear(outtmp);
	  idx<T> suin(inx.select(0, lt.get(0)));
	  ipp_convolution(suin, revkernel, outtmp);
	  ipp_add(outtmp, sout);
	} else { // not using IPP
	  idx<T> suin(uuin.select(0, lt.get(0)));
	  idx_m4dotm2acc(suin, lk, sout); // 2D convolution
	}
  #else
	idx<T> suin(uuin.select(0, lt.get(0)));
	idx_m4dotm2acc(suin, lk, sout); // 2D convolution
  #endif //endif __IPP__
      }
    }
    LOCAL_TIMING_REPORT("convcpu total time");
  }

  template <typename T, class Tstate>
  void convolution_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    // temporarly crop input if mismatch in size
    idx<T> inx = in.x, indx = in.dx;
    intg ki = kernel.x.dim(1), kj = kernel.x.dim(2);
    intg si = stride.dim(0), sj = stride.dim(1);
    intg oi = inx.dim(1) - (ki - si), oj = inx.dim(2) - (kj - sj);
    if (crop && oi % stride.dim(0) != 0) {
      inx = inx.narrow(1, inx.dim(1) - oi % si, 0);
      indx = indx.narrow(1, inx.dim(1) - oi % si, 0);
    }
    if (crop && oj % stride.dim(1) != 0) {
      inx = inx.narrow(2, inx.dim(2) - oj % sj, 0);
      indx = indx.narrow(2, inx.dim(2) - oj % sj, 0);
    }
 #ifdef __TH__
     if ((float_precision || double_precision) && in.x.order() == 3) {
       idx_clear(indx);
       th_convolution_3dmap_bprop(inx, kernel.x, out.dx, indx, 
                                  kernel.dx, table, 
                                  stride.dim(0), stride.dim(1));
       return;
     }
     else {
       // use the regular method
       // backprop through convolution
       idx<T> uuin(indx.unfold(1, kernel.dx.dim(1), stride.dim(0)));
       uuin = uuin.unfold(2, kernel.dx.dim(2), stride.dim(1));
       idx<T> uuinf(inx.unfold(1, kernel.dx.dim(1), stride.dim(0)));
       uuinf = uuinf.unfold(2, kernel.dx.dim(2), stride.dim(1));
       int transp[5] = { 0, 3, 4, 1, 2 };
       idx<T> borp(uuinf.transpose(transp));
       { idx_bloop3 (lk, kernel.dx, T, lkf, kernel.x, T,
                     lt, table, intg) {
           intg islice = lt.get(0);
           idx<T> suin(uuin.select(0, islice));
           idx<T> sborp(borp.select(0, islice));
           idx<T> sout(out.dx.select(0, lt.get(1)));
           idx_m2extm2acc(sout, lkf, suin); // backward convolution
           idx_m4dotm2acc(sborp, sout, lk); // compute gradient for kernel
         }
       }
       return;
     }
 #else
    // backprop through convolution
    idx<T> uuin(indx.unfold(1, kernel.dx.dim(1), stride.dim(0)));
    uuin = uuin.unfold(2, kernel.dx.dim(2), stride.dim(1));
    idx<T> uuinf(inx.unfold(1, kernel.dx.dim(1), stride.dim(0)));
    uuinf = uuinf.unfold(2, kernel.dx.dim(2), stride.dim(1));
    int transp[5] = { 0, 3, 4, 1, 2 };
    idx<T> borp(uuinf.transpose(transp));
    { idx_bloop3 (lk, kernel.dx, T, lkf, kernel.x, T,
		  lt, table, intg) {
	intg islice = lt.get(0);
	idx<T> suin(uuin.select(0, islice));
	idx<T> sborp(borp.select(0, islice));
	idx<T> sout(out.dx.select(0, lt.get(1)));
#ifdef __IPP__
	if (float_precision && use_ipp)
	  idx_m2extm2acc(sout, revkernel, suin); // backward convolution
	else
	  idx_m2extm2acc(sout, lkf, suin); // backward convolution
	idx_m4dotm2acc(sborp, sout, lk); // compute gradient for kernel
#else
	idx_m2extm2acc(sout, lkf, suin); // backward convolution
	idx_m4dotm2acc(sborp, sout, lk); // compute gradient for kernel
#endif //IPP
      }}
#endif //TH
  }

  template <typename T, class Tstate>
  void convolution_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    // temporarly crop input if mismatch in size
    idx<T> inx = in.x, inddx = in.ddx;
    intg ki = kernel.x.dim(1), kj = kernel.x.dim(2);
    intg si = stride.dim(0), sj = stride.dim(1);
    intg oi = inx.dim(1) - (ki - si), oj = inx.dim(2) - (kj - sj);
    if (crop && oi % stride.dim(0) != 0) {
      inx = inx.narrow(1, inx.dim(1) - oi % si, 0);
      inddx = inddx.narrow(1, inx.dim(1) - oi % sj, 0);
    }
    if (crop && oj % stride.dim(1) != 0) {
      inx = inx.narrow(2, inx.dim(2) - oj % si, 0);
      inddx = inddx.narrow(2, inx.dim(2) - oj % sj, 0);
    }
    // backprop through convolution
    idx<T> uuin(inddx.unfold(1, kernel.ddx.dim(1), stride.dim(0)));
    uuin = uuin.unfold(2, kernel.ddx.dim(2), stride.dim(1));
    idx<T> uuinf(inx.unfold(1, kernel.ddx.dim(1), stride.dim(0)));
    uuinf = uuinf.unfold(2, kernel.ddx.dim(2), stride.dim(1));
    int transp[5] = { 0, 3, 4, 1, 2 };
    idx<T> borp(uuinf.transpose(transp));
    { idx_bloop3 (lk, kernel.ddx, T, lkf, kernel.x, T,
		  lt, table, intg) {
	intg islice = lt.get(0);
	idx<T> suin(uuin.select(0, islice));
	idx<T> sborp(borp.select(0, islice));
	idx<T> sout((out.ddx).select(0, lt.get(1)));

#ifdef __IPP__
	if (float_precision && use_ipp)
	  idx_m2squextm2acc(sout, revkernel, suin); // backward convolution
	else
	  idx_m2squextm2acc(sout, lkf, suin); // backward convolution
	idx_m4squdotm2acc(sborp, sout, lk); // compute gradient for kernel
#else
	idx_m2squextm2acc(sout, lkf, suin); // backward convolution
	idx_m4squdotm2acc(sborp, sout, lk); // compute gradient for kernel
#endif
      }}
  }

  template <typename T, class Tstate>
  void convolution_module<T,Tstate>::forget(forget_param_linear &fp) {
    idx<T> kx(kernel.x);
    intg vsize = kx.dim(1);
    intg hsize = kx.dim(2);
    idx<intg> ts(table.select(1, 1));
    idx<int> fanin_(1 + idx_max(ts));
    idx_clear(fanin_);
    { idx_bloop1(tab, table, intg)	{
	fanin_.set(1 + fanin_.get(tab.get(1)), tab.get(1)); }}
    { idx_bloop2(tab, table, intg, x, kx, T) {
	double s = fp.value / pow((vsize * hsize * fanin_.get(tab.get(1))),
				  fp.exponent);
	{ idx_bloop1(lx, x, T) {
	    { idx_bloop1(llx, lx, T) {
		T n = (T) fp.generator.drand(-s, s);
		llx.set(n);
	      }}
	  }}
      }}
  }

  template <typename T, class Tstate>
  bool convolution_module<T,Tstate>::resize_output(Tstate &in, Tstate &out) {
    if (!this->bresize) return false;
    TIMING_RESIZING_ACCSTART(); // start accumulating resizing time
    intg ki = kernel.x.dim(1), kj = kernel.x.dim(2);
    intg si = stride.dim(0), sj = stride.dim(1);
    // check input size for table
    if (in.x.dim(0) < tablemax + 1)
      eblerror("error: expecting input with size " << tablemax + 1
	       << " in dimension 0 but found: " << in.x);
    if (!warnings_shown && (in.x.dim(0) > tablemax + 1)) {
      warnings_shown = true;
      cerr << "warning: convolution connection table is not using all inputs "
	   << "in layer " << this->name() << " the maximum input index used by "
	   << "the table is " << tablemax << " but the input is "
	   << in.x << endl;
    }
    // check sizes
    if (!crop && (((in.x.dim(1) - (ki - si)) % si != 0) ||
		  ((in.x.dim(2) - (kj - sj)) % sj != 0)))
      eblerror("inconsistent input size, kernel size, and subsampling ratio");
#ifdef __IPP__
    if ((stride.dim(0) != 1) || (stride.dim(1) != 1)) {
      use_ipp = false;
      if (!ipp_err_printed) {
	// cerr << "Warning: not using IPP in " << this->name()
	//      << " because stride > 1 not implemented for IPP" << endl;
	ipp_err_printed = true;
      }
    } else use_ipp = true;
#endif
    idx<T> inx = in.x;
    idxdim d(in.x.spec); // use same dimensions as in
    intg oi = inx.dim(1) - (ki - si), oj = inx.dim(2) - (kj - sj);
    intg ii = inx.dim(1) - oi % si, jj = inx.dim(2) - oj % sj;
    // if kernel is smaller than input, just resize output to 1 blank pixel
    if (ii == 0 || jj == 0 || ki > inx.dim(1) || kj > inx.dim(2)) {
      d.setdims(1);
      d.setdim(0, thickness);
      out = Tstate(d);
      out.clear();
      return false;
    }
    // crop input if mismatch in size
    if (crop && oi % stride.dim(0) != 0)
      inx = inx.narrow(1, ii, 0);
    if (crop && oj % stride.dim(1) != 0)
      inx = inx.narrow(2, jj, 0);
    // unfolding input for a faster convolution operation
    idx<T> uuin(inx.unfold(1, ki, si));
    uuin = uuin.unfold(2, kj, sj);
    // resize output based in input dimensions
    d.setdim(0, thickness); // except for the first one
    d.setdim(1, uuin.dim(1)); // convolution trims dimensions a bit
    d.setdim(2, uuin.dim(2)); // convolution trims dimensions a bit
    if (out.x.get_idxdim() != d) { // resize only if necessary
      EDEBUG(this->name() << ": resizing output from " << out.x << " to " << d);
#ifdef __TH__
      if (float_precision || double_precision) {
	outtmp.resize(d.dim(1), d.dim(2));
      }
#else
  #ifdef __IPP__
      if (float_precision && use_ipp) {
	outtmp.resize(d.dim(1), d.dim(2));
      }
  #endif
#endif
      if (out.x.order() != d.order())
	out = Tstate(d);
      else
	out.resize(d);
    }
    TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
    return true;
  }

  template <typename T, class Tstate>
  fidxdim convolution_module<T,Tstate>::fprop_size(fidxdim &isize) {
    fidxdim osize = isize;
    if (osize.empty()) return osize;
    // features dimension
    osize.setdim(0, thickness);
    // update spatial dimensions
    for (uint i = 1; i < isize.order(); ++i)
      osize.setdim(i, std::max((float) 1, isize.dim(i) - kernel.x.dim(i) + 1));
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim convolution_module<T,Tstate>::bprop_size(const fidxdim &osize) {
    fidxdim isize = osize;
    // features dimension
    isize.setdim(0, thickness);
    // spatial dimensions
    for (uint i = 1; i < osize.order(); ++i) {
      isize.setdim(i, osize.dim(i) + kernel.x.dim(i) - 1);
      //      isize.setoffset(i, osize.offset(i) - kernel.x.dim(i) / 2);
    }
    return isize;
  }

  template <typename T, class Tstate>
  convolution_module<T,Tstate>* convolution_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    convolution_module<T,Tstate> *l2 =
      new convolution_module<T,Tstate>(p, ker, stride, table, this->name());
    if (!p) // assign same parameter state if no parameters were specified
      l2->kernel = kernel;
    return l2;
  }

  template <typename T, class Tstate>
  void convolution_module<T, Tstate>::load_x(idx<T> &weights) {
    if (!kernel.x.same_dim(weights)) {
      // if sizes are the same except for the feature size, load
      // into the corresponding slices with a warning
      // this allows to load grayscale pretrained weights only
      // in a grayscale + color net for example.
      idxdim d(kernel.x);
      d.setdim(0, weights.dim(0));
      if (d == weights.get_idxdim()) {
	cerr << "Warning: loading weights partly (the first " << d.dim(0)
	     << " features) from " << weights << " instead of entire weights ("
	     << kernel.x << ")." << endl;
	intg sz = std::min(kernel.x.dim(0), weights.dim(0));
	idx<T> slices = kernel.x.narrow(0, sz, 0);
	idx<T> w = weights.narrow(0, sz, 0);
	idx_copy(w, slices);
      } else
	eblthrow("expected same dimension weights but got " << kernel.x
		 << " and " << weights << " instead in " << this->name());
    } else
      idx_copy(weights, kernel.x);
  }

  template <typename T, class Tstate>
  std::string convolution_module<T, Tstate>::describe() {
    std::string desc;
    desc << "convolution module " << this->name() << " with " << kernel.x.dim(0)
	 << " kernels with size " << ker << ", stride " << stride
	 << " and table " << table << " (" << tablemax+1 << "->" << thickness
	 << ")";
    return desc;
  }

  template <typename T, class Tstate>
  void convolution_module<T, Tstate>::dump_fprop(Tstate &in, Tstate &out) {
    fprop(in, out);
    DUMP(in.x, this->name() << "_convolution_module_in.x");
    DUMP(kernel.x, this->name() << "_convolution_module_ker.x");
    DUMP(table, this->name() << "_convolution_module_table");
    DUMP(out.x, this->name() << "_convolution_module_out.x");
  }

  ////////////////////////////////////////////////////////////////
  // addc_module

  template <typename T, class Tstate>
  addc_module<T,Tstate>::addc_module(parameter<T,Tstate> *p, intg size,
				     const char *name_)
    : module_1_1<T,Tstate>(name_), bias(p, size) {
  }

  template <typename T, class Tstate>
  addc_module<T,Tstate>::~addc_module() {
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::fprop(Tstate& in, Tstate& out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x.spec); // use same dimensions as in
      d.setdim(0, bias.x.dim(0)); // except for the first one
      this->resize_output(in, out, &d); // resize iff necessary
    }
    // add each bias to entire slices cut from the first dimension
    idx_bloop3(inx, in.x, T, biasx, bias.x, T, outx, out.x, T) {
      idx_addc(inx, biasx.get(), outx);
    }
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::bprop(Tstate& in, Tstate& out) {
    if (&in != &out) idx_checknelems2_all(in.dx, out.dx);

    idx_bloop3(indx, in.dx, T, biasdx, bias.dx, T,
	       outdx, out.dx, T) {
      if (&in != &out) // only pass on info if necessary
	idx_add(outdx, indx, indx); // accumulate gradients to input
      idx_sumacc(outdx, biasdx); // accumulate gradients to weights
    }
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::bbprop(Tstate& in, Tstate& out) {
    if (&in != &out) idx_checknelems2_all(in.ddx, out.ddx);

    idx_bloop3(inddx, in.ddx, T, biasddx, bias.ddx, T,
	       outddx, out.ddx, T) {
      if (&in != &out) // only pass on info if necessary
	idx_add(outddx, inddx, inddx); // accumulate 2nd gradients to input
      idx_sumacc(outddx, biasddx); // accumulate 2nd gradients to weights
    }
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::forget(forget_param_linear& fp) {
    idx_clear(bias.x);
  }

  template <typename T, class Tstate>
  addc_module<T,Tstate>* addc_module<T,Tstate>::copy(parameter<T,Tstate> *p) {
    // new module (with its own local parameter buffers)
    addc_module<T,Tstate> *l2 =
      new addc_module<T, Tstate>(p, bias.x.dim(0), this->name());
    // assign same parameter state if no parameters were specified
    if (!p) l2->bias = bias;
    return l2;
  }

  template <typename T, class Tstate>
  void addc_module<T, Tstate>::load_x(idx<T> &weights) {
    if (!bias.x.same_dim(weights)) {
      // if sizes are the same except for the feature size, load
      // into the corresponding slices with a warning
      // this allows to load grayscale pretrained weights only
      // in a grayscale + color net for example.
      idxdim d(bias.x);
      d.setdim(0, weights.dim(0));
      if (d == weights.get_idxdim()) {
	cerr << "Warning: loading weights partly (the first " << d.dim(0)
	     << " features) from " << weights << " instead of entire weights ("
	     << bias.x << ")." << endl;
	intg sz = std::min(bias.x.dim(0), weights.dim(0));
	idx<T> slices = bias.x.narrow(0, sz, 0);
	idx<T> w = weights.narrow(0, sz, 0);
	idx_copy(w, slices);
      } else
      eblthrow("expected same dimension weights but got " << bias.x << " and "
	       << weights << " instead in " << this->name());
    } else
      idx_copy(weights, bias.x);
  }

  template <typename T, class Tstate>
  std::string addc_module<T, Tstate>::describe() {
    std::string desc;
    desc << "bias module " << this->name() << " with "
	 << bias.x.dim(0) << " biases";
    return desc;
  }

  template <typename T, class Tstate>
  void addc_module<T,Tstate>::dump_fprop(Tstate& in, Tstate& out) {
    fprop(in, out);
    DUMP(in.x, this->name() << "_addc_module_in.x");
    DUMP(bias.x, this->name() << "_addc_module_weights");
    DUMP(out.x, this->name() << "_addc_module_out.x");
  }

  ////////////////////////////////////////////////////////////////
  // power_module

  template <typename T, class Tstate>
  power_module<T,Tstate>::power_module(T p_
#ifdef __CUDA__
, bool use_gpu_, int gpu_id_
#endif
)
    : module_1_1<T,Tstate>("power"), p(p_) 
#ifdef __CUDA__
    ,use_gpu(use_gpu_), gpu_id(gpu_id_) 
#endif
{

#if __CUDA__
    // check precision to decide if we use TH or not
  float_precision = false;
  fstate_idx<T> temp;
  fstate_idx<float> *cont = dynamic_cast<fstate_idx<float>*>(&temp);
  if (cont) 
    float_precision = true;
#endif
  
  }

  template <typename T, class Tstate>
  power_module<T,Tstate>::~power_module() {
  }

  template <typename T, class Tstate>
  void power_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize iff necessary

#ifdef __CUDA__
    if(float_precision && use_gpu) {
      cuda_power(in.x, out.x, p, gpu_id);
      return;
    }
#endif

 // #ifdef __TH__
 //     th_pow(in.x, out.x, p);
 // #else
    idx_power(in.x, p, out.x);
 // #endif
  }

  template <typename T, class Tstate>
  void power_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions

    if (!tt.same_dim(in.x.get_idxdim())) { // resize temp buffer
      idxdim d(in.x);
      tt = idx<T>(d);
    }
    // compute derivative
 // #ifdef __TH__
 //     th_pow(in.x, tt, p - 1);
 // #else
    idx_power(in.x, p - 1, tt);
//#endif
    idx_mul(out.dx, tt, tt); //! tt = outdx*x^(p-1)
    idx_dotcacc(tt, p, in.dx);
  }

  template <typename T, class Tstate>
  void power_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions

    if (!tt.same_dim(in.x.get_idxdim())) { // resize temp buffer
      idxdim d(in.x);
      tt = idx<T>(d);
    }
    // compute 2nd derivative
// #ifdef __TH__
//     th_pow(in.x, tt, p - 1);
// #else
    idx_power(in.x, p - 1, tt);
//#endif
    idx_mul(tt, tt, tt); //! tt = (x^(p-1))^2
    idx_mul(out.ddx, tt, tt); //! tt = outddx*(x^(p-1))^2
    idx_dotcacc(tt, (p * p), in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // diff_module

  template <typename T, class Tstate>
  diff_module<T,Tstate>::diff_module() : module_2_1<T,Tstate>("diff") {
  }

  template <typename T, class Tstate>
  diff_module<T,Tstate>::~diff_module() {
  }

  template <typename T, class Tstate>
  void diff_module<T,Tstate>::fprop(Tstate &in1, Tstate &in2, Tstate &out) {
    this->resize_output(in1, in2, out); // resize iff necessary
    idx_sub(in1.x, in2.x, out.x);
  }

  template <typename T, class Tstate>
  void diff_module<T,Tstate>::bprop(Tstate &in1, Tstate &in2, Tstate &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.dx, in2.dx, out.dx);// must have same dimensions

    idx_add(out.dx, in1.dx); // derivative wrt in1
    idx_minus_acc(out.dx, in2.dx); // derivative wrt in2
  }

  template <typename T, class Tstate>
  void diff_module<T,Tstate>::bbprop(Tstate &in1, Tstate &in2, Tstate &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.ddx, in2.ddx, out.ddx);// must have same dimensions

    idx_add(out.ddx, in1.ddx); // derivative wrt in1
    idx_add(out.ddx, in2.ddx); // derivative wrt in2
  }

  ////////////////////////////////////////////////////////////////
  // mul_module

  template <typename T, class Tstate>
  mul_module<T,Tstate>::mul_module() : module_2_1<T,Tstate>("mul") {
  }

  template <typename T, class Tstate>
  mul_module<T,Tstate>::~mul_module() {
  }

  template <typename T, class Tstate>
  void mul_module<T,Tstate>::fprop(Tstate &in1, Tstate &in2, Tstate &out) {
    this->resize_output(in1, in2, out); // resize iff necessary
    idx_mul(in1.x, in2.x, out.x);
  }

  template <typename T, class Tstate>
  void mul_module<T,Tstate>::bprop(Tstate &in1, Tstate &in2, Tstate &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.dx, in2.dx, out.dx);// must have same dimensions

    if (!tmp.same_dim(in1.x.get_idxdim())) { // resize temp buffer
      idxdim d(in1.x);
      tmp = idx<T>(d);
    }
    idx_mul(out.dx, in2.x, tmp);
    idx_add(tmp, in1.dx);
    idx_mul(out.dx, in1.x, tmp);
    idx_add(tmp, in2.dx);
  }

  template <typename T, class Tstate>
  void mul_module<T,Tstate>::bbprop(Tstate &in1, Tstate &in2, Tstate &out) {
    state_idx_check_different3(in1, in2, out); // forbid same in and out
    idx_checknelems3_all(in1.ddx, in2.ddx, out.ddx);// must have same dimensions

    if (!tmp.same_dim(in1.x.get_idxdim())) { // resize temp buffer
      idxdim d(in1.x);
      tmp = idx<T>(d);
    }
    idx_mul(in2.x, in2.x, tmp);
    idx_mul(out.ddx, tmp, tmp);
    idx_add(tmp, in1.ddx);
    idx_mul(in1.x, in1.x, tmp);
    idx_mul(out.ddx, tmp, tmp);
    idx_add(tmp, in2.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // thres_module

  template <typename T, class Tstate>
  thres_module<T,Tstate>::thres_module(T thres_, T val_)
    : module_1_1<T,Tstate>("thres"), thres(thres_), val(val_) {
  }

  template <typename T, class Tstate>
  thres_module<T,Tstate>::~thres_module() {
  }

  template <typename T, class Tstate>
  void thres_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize iff necessary
    idx_aloop2(inx, in.x, T, outx, out.x, T) {
      if (*inx > thres)
	*outx = *inx;
      else
	*outx = val;
    }
  }

  template <typename T, class Tstate>
  void thres_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions

    idx_aloop3(inx, in.x, T, indx, in.dx, T, outdx, out.dx, T) {
      if (*inx > thres)
	*indx += *outdx;
    }
  }

  template <typename T, class Tstate>
  void thres_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions

    idx_add(out.ddx, in.ddx);
  }

  ////////////////////////////////////////////////////////////////
  // cutborder_module

  template <typename T, class Tstate>
  cutborder_module<T,Tstate>::cutborder_module(int nr_, int nc_)
    : module_1_1<T,Tstate>("cutborder"), nrow(nr_), ncol(nc_) {
  }

  template <typename T, class Tstate>
  cutborder_module<T,Tstate>::~cutborder_module() {
  }

  template <typename T, class Tstate>
  void cutborder_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    intg inr = in.x.dim(1);
    intg inc = in.x.dim(2);
    intg outr = inr - 2 * nrow;
    intg outc = inc - 2 * ncol;
    idxdim d(in.x.dim(0), outr, outc);
    this->resize_output(in, out, &d); // resize iff necessary
    out.clear();
    idx<T> tmp = in.x.narrow(1, outr, nrow);
    tmp = tmp.narrow(2, outc, ncol);
    idx_copy(tmp, out.x);
  }

  template <typename T, class Tstate>
  void cutborder_module<T,Tstate>::bprop(Tstate &in,
					 Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = out.x.dim(1);
    intg inc = out.x.dim(2);
    idx<T> tmp = in.dx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(out.dx, tmp);
  }

  template <typename T, class Tstate>
  void cutborder_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out

    intg inr = out.x.dim(1);
    intg inc = out.x.dim(2);
    idx<T> tmp = in.ddx.narrow(1, inr, nrow);
    tmp = tmp.narrow(2, inc, ncol);
    idx_add(out.ddx, tmp);
  }

  ////////////////////////////////////////////////////////////////
  // zpad_module

  template <typename T, class Tstate>
  zpad_module<T,Tstate>::zpad_module(const char *name_)
    : module_1_1<T,Tstate>(name_) {
    set_paddings(0, 0, 0, 0);
  }

  template <typename T, class Tstate>
  zpad_module<T,Tstate>::zpad_module(int nr, int nc)
    : module_1_1<T,Tstate>("zpad") {
    set_paddings(nr, nc, nr, nc);
  }

  template <typename T, class Tstate>
  zpad_module<T,Tstate>::zpad_module(int top, int left, int bottom, int right)
    : module_1_1<T,Tstate>("zpad") {
    set_paddings(top, left, bottom, right);
  }

  template <typename T, class Tstate>
  zpad_module<T,Tstate>::zpad_module(idxdim &kerdims, const char *name_)
    : module_1_1<T,Tstate>(name_) {
    set_kernel(kerdims);
  }

  template <typename T, class Tstate>
  zpad_module<T,Tstate>::zpad_module(midxdim &kerdims, const char *name_)
    : module_1_1<T,Tstate>(name_) {
    set_kernels(kerdims);
  }

  template <typename T, class Tstate>
  zpad_module<T,Tstate>::~zpad_module() {
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::fprop(mstate<Tstate> &in, mstate<Tstate> &out) {
    // check that in/out have at least 1 state and the same number of them.
    if (in.size() == 0) eblerror("input should have at least 1");
    out.resize(in);
    // run regular fprop on each states
    for (uint i = 0; i < in.size(); ++i) {
      Tstate &fin = in[i], &fout = out[i];
      if (i < pads.size()) pad = pads[i];
      fprop(fin, fout);
    }
    // remember number of input/outputs
    this->ninputs = in.size();
    this->noutputs = out.size();
  }
  
  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
    idx<T> input = in.x;
    idxdim d(input.dim(0),
	     input.dim(1) + top + bottom,
	     input.dim(2) + left + right);

    if (&in == &out) { // same buffers, use a temporary copy
      if (input.get_idxdim() != in.x.get_idxdim())
	input = idx<T>(in.x.get_idxdim());
      idx_copy(in.x, input); // only copy forward
    }
    this->resize_output(in, out, &d); // resize iff necessary
    out.clear();
    idx<T> tmp = out.x.narrow(1, input.dim(1), top);
    tmp = tmp.narrow(2, input.dim(2), left);
    idx_copy(input, tmp);
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::fprop(Tstate &in, idx<T> &out) {
    int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
    idx<T> input = in.x;
    idxdim d(input.dim(0),
	     input.dim(1) + top + bottom,
	     input.dim(2) + left + right);
    if (&in.x == &out) { // same buffers, use a temporary copy
      input = idx<T>(in.x.get_idxdim());
      idx_copy(in.x, input); // only copy forward
    }
    if (!out.same_dim(d)) // resize only when necessary
      out.resize(d);
    idx_clear(out);
    idx<T> tmp = out.narrow(1, input.dim(1), top);
    tmp = tmp.narrow(2, input.dim(2), left);
    idx_copy(input, tmp);
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::fprop(idx<T> &in, idx<T> &out) {
    int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
    idx<T> input = in;
    idxdim d(input.dim(0),
	     input.dim(1) + top + bottom,
	     input.dim(2) + left + right);
    if (&in == &out) { // same buffers, use a temporary copy
      input = idx<T>(in.get_idxdim());
      idx_copy(in, input); // only copy forward
    }
    if (!out.same_dim(d)) // resize only when necessary
      out.resize(d);
    idx_clear(out);
    idx<T> tmp = out.narrow(1, input.dim(1), top);
    tmp = tmp.narrow(2, input.dim(2), left);
    idx_copy(input, tmp);
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
    // if in and out are the same, we just want to crop the buffers
    // by the extra padding that was added by the fprop
    if (&in == &out) {
      // crop state
      Tstate tmp = in.narrow(1, out.x.dim(1) - top - bottom, top);
      tmp = tmp.narrow(2, out.x.dim(2) - left - right, left);
      in = tmp;
    } else { // different buffers, accumulate gradients to input
      idx<T> tmp = out.dx.narrow(1, in.x.dim(1), top);
      tmp = tmp.narrow(2, in.x.dim(2), left);
      idx_add(tmp, in.dx);
    }
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
    // if in and out are the same, we just want to crop the buffers
    // by the extra padding that was added by the fprop
    if (&in == &out) {
      // crop state
      Tstate tmp = in.narrow(1, out.x.dim(1) - top - bottom, top);
      tmp = tmp.narrow(2, out.x.dim(2) - left - right, left);
      in = tmp;
    } else { // different buffers, accumulate gradients to input
      idx<T> tmp = out.ddx.narrow(1, in.x.dim(1), top);
      tmp = tmp.narrow(2, in.x.dim(2), left);
      idx_add(tmp, in.ddx);
    }
  }

  template <typename T, class Tstate>
  idxdim zpad_module<T,Tstate>::get_paddings() {
    return pad;
  }

  template <typename T, class Tstate>
  idxdim zpad_module<T,Tstate>::get_paddings(idxdim &ker) {
    int top = (int) (floor(ker.dim(0) / (float) 2.0));
    int left = (int) (floor(ker.dim(1) / (float) 2.0));
    int bottom = top;
    int right = left;
    // remove 1 pixel on right and bottom borders if even.
    if (ker.dim(0) % 2 == 0) bottom -= 1;
    if (ker.dim(1) % 2 == 0) right -= 1;
    return idxdim(top, left, bottom, right);
  }

  template <typename T, class Tstate>
  midxdim zpad_module<T,Tstate>::get_paddings(midxdim &kers) {
    midxdim p;
    for (uint i = 0; i < kers.size(); ++i) {
      idxdim &ker = kers[i];
      ker = get_paddings(ker);
      p.push_back_new(ker);
    }
    return p;
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::set_paddings(int top, int left, int bottom,
					   int right) {
    pads.clear();
    pad = idxdim(top, left, bottom, right);
    pads.push_back_new(pad);
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::set_paddings(idxdim &pads_) {
    if (pads_.order() != 4)
      eblerror("expected a 4-dim idxdim but got: " << pads_);
    pads.clear();
    pads.push_back(pads_);
    pad = pads_;
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::set_kernel(idxdim &ker) {
    if (ker.maxdim() == 0) {
      eblwarn("no padding for kernel " << ker);
      return ;
    }
    idxdim p = get_paddings(ker);
    set_paddings(p);
  }

  template <typename T, class Tstate>
  void zpad_module<T,Tstate>::set_kernels(midxdim &kers) {
    intg mx = 0;
    for (uint i = 0; i < kers.size(); ++i) {
      idxdim &k = kers[i];
      if (k.maxdim() > mx) mx = k.maxdim();
    }
    if (mx == 0) {
      eblwarn("no padding for kernels " << kers);
      return ;
    }
    pads = get_paddings(kers);
    pad = pads[0];
  }

  template <typename T, class Tstate>
  fidxdim zpad_module<T,Tstate>::fprop_size(fidxdim &isize) {
    int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
    fidxdim osize = isize;
    osize.setdim(1, isize.dim(1) + top + bottom);
    osize.setdim(2, isize.dim(2) + left + right);
    //! Recompute the input size to be compliant with the output
    isize = bprop_size(osize);
    return osize;
  }

  template <typename T, class Tstate>
  fidxdim zpad_module<T, Tstate>::bprop_size(const fidxdim &osize) {
    int top = pad.dim(0), left = pad.dim(1);
    fidxdim isize = osize;
    isize.setoffset(1, osize.offset(1) - top);
    isize.setoffset(2, osize.offset(2) - left);
    return isize;
  }

  template <typename T, class Tstate>
  mfidxdim zpad_module<T, Tstate>::fprop_size(mfidxdim &isize) {
    mfidxdim osize;
    for (uint i = 0; i < isize.size(); ++i) {
      if (i < pads.size()) pad = pads[i];
      if (isize.exists(i)) {
	fidxdim s = fprop_size(isize[i]);
	osize.push_back(s);
      } else osize.push_back_empty();
    }
    //EDEBUG(this->name() << ": " << isize << " f-> " << osize);
    return osize;
  }

  template <typename T, class Tstate>
  mfidxdim zpad_module<T, Tstate>::bprop_size(mfidxdim &osize) {
    mfidxdim isize;
    for (uint i = 0; i < osize.size(); ++i) {
      if (i < pads.size()) pad = pads[i];
      if (osize.exists(i)) {
	fidxdim s = bprop_size(osize[i]);
	isize.push_back(s);
      } else isize.push_back_empty();
    }
    //EDEBUG(this->name() << ": " << osize << " -> " << isize);
    return isize;
  }

  template <typename T, class Tstate>
  std::string zpad_module<T, Tstate>::describe() {
    std::string desc;
    desc << "zpad module " << this->name() << " is padding with: "
	 << pads;
    return desc;
  }

  template <typename T, class Tstate>
  zpad_module<T,Tstate>* zpad_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    zpad_module<T,Tstate> *z = new zpad_module<T,Tstate>(this->name());
    z->pads = pads;
    return z;
  }

  ////////////////////////////////////////////////////////////////
  // mirrorpad_module

  template <typename T, class Tstate>
  mirrorpad_module<T,Tstate>::mirrorpad_module(int nr, int nc)
    : zpad_module<T,Tstate>(nr, nc) {
    this->_name = "mirrorpad";
  }

  template <typename T, class Tstate>
  mirrorpad_module<T,Tstate>::mirrorpad_module(idxdim &kernel)
    : zpad_module<T,Tstate>(kernel) {
    this->_name = "mirrorpad";
  }

  template <typename T, class Tstate>
  mirrorpad_module<T,Tstate>::~mirrorpad_module() {
  }

  template <typename T, class Tstate>
  void mirrorpad_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    int top = pad.dim(0), left = pad.dim(1), bottom = pad.dim(2),
      right = pad.dim(3);
    idx<T> input = in.x;
    idxdim d(input.dim(0), input.dim(1) + top + bottom,
	     input.dim(2) + left + right);
    if (&in == &out) { // same buffers, use a temporary copy
      cout << "TEMPORARY COPY!!!!!!!!!!!!!" << endl;
      // FIXME
      input = idx<T>(input.get_idxdim());
      idx_copy(input, input); // only copy forward
    }
    this->resize_output(in, out, &d); // resize iff necessary
    idx<T> tmp, tmp2;
    int i;
    tmp = out.x.narrow(1, input.dim(1), top);
    tmp = tmp.narrow(2, input.dim(2), left);
    idx_copy(input, tmp);
    // mirror border left
    for (i = std::max(0, (int) (left - input.dim(1) / 2)); i < left; ++i) {
      tmp2 = input.narrow(1, 1, left - i - 1);
      tmp = out.x.narrow(1, 1, i);
      tmp = tmp.narrow(2, input.dim(2), left);
      idx_copy(tmp2, tmp);
    }
    // mirror border right
    for (i = std::max(0, (int) (right - input.dim(1) / 2)); i < right; ++i) {
      tmp2 = input.narrow(1, 1, input.dim(1) - right - 1 + i);
      tmp = out.x.narrow(1, 1, out.x.dim(1) - 1 - i);
      tmp = tmp.narrow(2, input.dim(2), right);
      idx_copy(tmp2, tmp);
    }
    // mirror border top using out as input
    for (i = std::max(0, (int) (top - input.dim(2) / 2)); i < top; ++i) {
      tmp2 = out.x.narrow(2, 1, top + top - i - 1);
      tmp = out.x.narrow(2, 1, i);
      idx_copy(tmp2, tmp);
    }
    // mirror border bottom using out as input
    for (i = std::max(0, (int) (bottom - input.dim(2) / 2)); i < bottom; ++i) {
      tmp2 = out.x.narrow(2, 1, out.x.dim(2) - bottom * 2 - 1 + i);
      tmp = out.x.narrow(2, 1, out.x.dim(2) - 1 - i);
      idx_copy(tmp2, tmp);
    }
  }

  template <typename T, class Tstate>
  void mirrorpad_module<T,Tstate>::fprop(Tstate &in, idx<T> &out) {
    eblerror("not implemented");
  }

  template <typename T, class Tstate>
  mirrorpad_module<T,Tstate>* mirrorpad_module<T,Tstate>::
  copy(parameter<T,Tstate> *p) {
    int top = pad.dim(0), left = pad.dim(1);
    return new mirrorpad_module<T,Tstate>(top, left);
  }

  // fsum_module ///////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  fsum_module<T,Tstate>::fsum_module(bool div_, float split_)
    : module_1_1<T,Tstate>("fsum"), div(div_), split(split_) {
  }

  template <typename T, class Tstate>
  fsum_module<T,Tstate>::~fsum_module() {
  }

  template <typename T, class Tstate>
  void fsum_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize iff necessary
    T sum;
    uint i, ngroup, size = in.x.dim(0);
    uint nsplit = std::max((uint) 2, (uint) (in.x.dim(0) * split));
    idx<T> igroup, ogroup;
    idx_eloop2(inx2, in.x, T, outx2, out.x, T) {
      idx_eloop2(inx1, inx2, T, outx1, outx2, T) {
  	if (split != 1.0) { // sum in groups
  	  for (i = 0; i < size; ) {
  	    ngroup = std::min(nsplit, size - i);
  	    igroup = inx1.narrow(0, ngroup, i);
  	    ogroup = outx1.narrow(0, ngroup, i);
  	    sum = idx_sum(igroup);
  	    if (div) sum = sum / igroup.nelements();
  	    idx_fill(ogroup, sum);
  	    i += ngroup;
  	  }
  	} else { // no splitting
  	  sum = idx_sum(inx1);
  	  if (div) sum = sum / inx1.nelements();
  	  idx_fill(outx1, sum);
  	}
      }
    }
  }

  template <typename T, class Tstate>
  void fsum_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions

    T sum;
    uint i, ngroup, size = in.x.dim(0);
    uint nsplit = std::max((uint) 2, (uint) (in.x.dim(0) * split));
    idx<T> igroup, ogroup;
    idx_eloop2(indx2, in.dx, T, outdx2, out.dx, T) {
      idx_eloop2(indx1, indx2, T, outdx1, outdx2, T) {
  	if (split != 1.0) { // sum in groups
  	  for (i = 0; i < size; ) {
  	    ngroup = std::min(nsplit, size - i);
  	    igroup = indx1.narrow(0, ngroup, i);
  	    ogroup = outdx1.narrow(0, ngroup, i);
  	    sum = idx_sum(ogroup);
  	    if (div) sum = sum / igroup.nelements();
  	    idx_addc(igroup, sum, igroup);
  	    i += ngroup;
  	  }
  	} else { // no splitting
  	  sum = idx_sum(outdx1);
	  if (div) sum = sum / indx1.nelements();
  	  idx_addc(indx1, sum, indx1);
  	}
      }
    }
  }

  template <typename T, class Tstate>
  void fsum_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions

    T sum;
    uint i, ngroup, size = in.x.dim(0);
    uint nsplit = std::max((uint) 2, (uint) (in.x.dim(0) * split));
    idx<T> igroup, ogroup;
    idx_eloop2(inddx2, in.ddx, T, outddx2, out.ddx, T) {
      idx_eloop2(inddx1, inddx2, T, outddx1, outddx2, T) {
  	if (split != 1) { // sum in groups
  	  for (i = 0; i < size; ) {
  	    ngroup = std::min(nsplit, size - i);
  	    igroup = inddx1.narrow(0, ngroup, i);
  	    ogroup = outddx1.narrow(0, ngroup, i);
  	    sum = idx_sum(ogroup);
  	    if (div) sum = sum / igroup.nelements();
  	    idx_addc(igroup, sum, igroup);
  	    i += ngroup;
  	  }
  	} else { // no splitting
  	  sum = idx_sum(outddx1);
	  if (div) sum = sum / inddx1.nelements();
  	  idx_addc(inddx1, sum, inddx1);
  	}
      }
    }
  }

  ////////////////////////////////////////////////////////////////
  // binarize_module

  template <typename T, class Tstate>
  binarize_module<T,Tstate>::binarize_module(T threshold_, T false_value_,
				      T true_value_)
    : threshold(threshold_), false_value(false_value_), true_value(true_value_){
  }

  template <typename T, class Tstate>
  binarize_module<T,Tstate>::~binarize_module() {
  }

  template <typename T, class Tstate>
  void binarize_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize out iff necessary
    idx_aloop2(inx, in.x, T, outx, out.x, T) {
      if (*inx > threshold)
	*outx = true_value;
      else
	*outx = false_value;
    }
  }

  ////////////////////////////////////////////////////////////////
  // range_lut_module

  template <typename T, class Tstate>
  range_lut_module<T,Tstate>::range_lut_module(idx<T> *value_range_) {
    if (value_range_ == NULL)
      eblerror("expected non null range matrix");
    value_range = idx<T>(value_range_->get_idxdim());
    idx_copy(*value_range_, value_range);
  }

  template <typename T, class Tstate>
  range_lut_module<T,Tstate>::~range_lut_module() {
  }

  template <typename T, class Tstate>
  void range_lut_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize out iff necessary
    idx_aloop2(inx, in.x, T, outx, out.x, T) {
      //      cout << "v0: " << *inx;
      idx_bloop1(vr, value_range, T) {
	if (*inx < vr.get(1)) {
	  *outx = vr.get(0);
	  break ;
	}
      }
      //      cout << " v1: " << *outx << endl;
    }
  }

  ////////////////////////////////////////////////////////////////
  // diag_module

  template <typename T, class Tstate>
  diag_module<T, Tstate>::diag_module(parameter<T,Tstate> *p, intg thick,
				      const char *name_)
    : module_1_1<T,Tstate>(name_), coeff(p, thick) {
    // initialize coeffs to 1
    idx_fill(coeff.x, (T)1.0);
  }

  template <typename T, class Tstate>
  diag_module<T, Tstate>::~diag_module() {
  }

  template <typename T, class Tstate>
  void diag_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize out iff necessary
    idx_bloop3(c, coeff.x, T, i, in.x, T, o, out.x, T) {
      idx_dotc(i, c.get(), o);
    }
  }

  template <typename T, class Tstate>
  void diag_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    idx_bloop5(c, coeff.x, T, cd, coeff.dx, T, i, in.x, T, id, in.dx, T,
	       od, out.dx, T) {
      idx_dotcacc(od, c.get(), id); // bprop to input
      idx_dotacc(i, od, cd); // bprop to weights
    }
  }

  template <typename T, class Tstate>
  void diag_module<T, Tstate>::bbprop(Tstate &in, Tstate &out) {
    idx_bloop5(c, coeff.x, T, cdd, coeff.ddx, T, i, in.x, T, idd, in.ddx, T,
	       odd, out.ddx, T) {
      idx_dotcacc(odd, c.get() * c.get(), idd); // bprop to input
      idx_m2squdotm2acc(i, odd, cdd); // bprop to weights
    }
  }

  template <typename T, class Tstate>
  bool diag_module<T, Tstate>::resize_output(Tstate &in, Tstate &out) {
    // resize output based on input dimensions
    idxdim d(in.x); // use same dimensions as in
    d.setdim(0, coeff.x.dim(0)); // except for the first one
    return module_1_1<T,Tstate>::resize_output(in, out, &d);
  }

  template <typename T, class Tstate>
  void diag_module<T, Tstate>::load_x(idx<T> &weights) {
    if (!coeff.x.same_dim(weights)) {
      // if sizes are the same except for the feature size, load
      // into the corresponding slices with a warning
      // this allows to load grayscale pretrained weights only
      // in a grayscale + color net for example.
      idxdim d(coeff.x);
      d.setdim(0, weights.dim(0));
      if (d == weights.get_idxdim()) {
	cerr << "Warning: loading weights partly (the first " << d.dim(0)
	     << " features) from " << weights << " instead of entire weights ("
	     << coeff.x << ")." << endl;
	intg sz = std::min(coeff.x.dim(0), weights.dim(0));
	idx<T> slices = coeff.x.narrow(0, sz, 0);
	idx<T> w = weights.narrow(0, sz, 0);
	idx_copy(w, slices);
      } else
	eblthrow("expected same dimension weights but got " << coeff.x <<
		 " and " << weights << " instead in " << this->name());
    } else
    idx_copy(weights, coeff.x);
  }

  template <typename T, class Tstate>
  std::string diag_module<T, Tstate>::describe() {
    std::string desc;
    desc << "diag module " << this->name() << " with "
	 << coeff.x << " coefficients";
    return desc;
  }

  template <typename T, class Tstate>
  diag_module<T,Tstate>* diag_module<T,Tstate>::copy(parameter<T,Tstate> *p) {
    diag_module<T,Tstate>* d = new diag_module<T,Tstate>(p, coeff.x.dim(0));
    // assign same parameter state if no parameters were specified
    if (!p) d->coeff = coeff;
    return d;
  }

  ////////////////////////////////////////////////////////////////
  // copy_module

  template <typename T, class Tstate>
  copy_module<T, Tstate>::copy_module(const char *name_)
    : module_1_1<T,Tstate>(name_) {
  }

  template <typename T, class Tstate>
  copy_module<T, Tstate>::~copy_module() {
  }

  template <typename T, class Tstate>
  void copy_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize (iff necessary)
    idx_copy(in.x, out.x);
  }

  template <typename T, class Tstate>
  void copy_module<T, Tstate>::bprop(Tstate &in, Tstate &out) {
    idx_copy(out.dx, in.dx);
  }

  template <typename T, class Tstate>
  void copy_module<T, Tstate>::bbprop(Tstate &in, Tstate &out) {
    idx_copy(out.ddx, in.ddx);
  }

  template <typename T, class Tstate>
  std::string copy_module<T, Tstate>::describe() {
    std::string desc;
    desc << "copy module " << this->name();
    return desc;
  }

  ////////////////////////////////////////////////////////////////
  // back_module

#define BACK_MIN -10.0

  template <typename T, class Tstate>
  back_module<T, Tstate>::back_module(const char *name_)
    : module_1_1<T,Tstate>(name_), s0(NULL), s1(NULL), s2(NULL) {
  }

  template <typename T, class Tstate>
  back_module<T, Tstate>::~back_module() {
  }

  template <typename T, class Tstate>
  void back_module<T, Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize (iff necessary)
    // copy input to s0
    idx_copy(in.x, *s0);
    cout << "back: mins: so: " << idx_min(*s0) << " s1: " << idx_min(*s1) << " s2: "
	 << idx_min(*s2) << endl;
    cout << "back: maxs: so: " << idx_max(*s0) << " s1: " << idx_max(*s1) << " s2: "
	 << idx_max(*s2) << endl;
    // put max of all buffers in output
//     idx_aloop3(x0, *s0, T, x1, *s1, T, o, out.x, T) {
//       *o = std::max(*x0, *x1);
//     }
    // put max of all buffers in output
    idx_aloop4(x0, *s0, T, x1, *s1, T, x2, *s2, T, o, out.x, T) {
      *o = std::max(*x0, std::max(*x1, *x2));
    }
  }

  template <typename T, class Tstate>
  void back_module<T, Tstate>::bb(std::vector<bbox*> &boxes) {
    cout << "back: " << boxes.size() << " boxes" << endl;
    // shift internal buffers and clear first one
    idx_copy(*s1, *s2);
    idx_fill(*s1, (T)BACK_MIN);
    // copy all boxes features to s1
    int height = s0->dim(1);
    int width = s0->dim(2);
    for (uint i = 0; i < boxes.size(); ++i) {
      bbox &b = *(boxes[i]);
      // find box's location at this stage
      float rho = b.o.h0 / (float) b.oheight;
      float rwo = b.o.w0 / (float) b.owidth;
      int h0 = (int) (height * rho);
      int w0 = (int) (width * rwo);
      int h = pixel_size.dim(1);
      int w = pixel_size.dim(2);
      // cut bbox if outside of buffers
      if (h0 < 0) { h -= h0; h0 = 0; }
      if (w0 < 0) { w -= w0; w0 = 0; }
      if (h0 + h > height) h -= h0 + h - height;
      if (w0 + w > width) w -= w0 + w - width;
      // max-copy box features from s0 to s1
      idx<T> b1 = s1->narrow(1, h, h0);
      b1 = b1.narrow(2, w, w0);
      idx<T> b0 = s0->narrow(1, h, h0);
      b0 = b0.narrow(2, w, w0);
      idx_max(b0, b1);
    }
    // shift buffers for horizontal motion
    int wshift = (int) (.02 * width);
    cout << "back: shift by " << wshift << " pixels (width: "
	 << width << ")" << endl;
    idx<T> tmp(s1->get_idxdim());
    idx_fill(tmp, (T)BACK_MIN);
    idx<T> shifted = tmp.narrow(2, width - wshift, wshift);
    idx<T> original = s1->narrow(2, width - wshift, 0);
    idx_copy(original, shifted);
    idx_copy(tmp, *s1);
    // shift s2
    idx_fill(tmp, (T)BACK_MIN);
    shifted = tmp.narrow(2, width - wshift, wshift);
    original = s2->narrow(2, width - wshift, 0);
    idx_copy(original, shifted);
    idx_copy(tmp, *s2);
    // decay buffers
    //    idx_addc(*s1, (T) -0.2, *s1);
    //    idx_addc(*s2, (T) -0.2, *s2);
  }

  template <typename T, class Tstate>
  bool back_module<T, Tstate>::resize_output(Tstate &in, Tstate &out) {
    TIMING_RESIZING_ACCSTART(); // start accumulating resizing time
    // resize output based on input dimensions
    idxdim d(in.x); // use same dimensions as in
    if (out.x.get_idxdim() != d) { // resize only if necessary
      EDEBUG(this->name() << ": resizing output from " << out.x << " to " << d);
      out.resize(d);
    }
    if (!s0 || s0->get_idxdim() != d) {
      cout << "back: resizing internal buffers to " << d << endl;
      if (s0) s0->resize(d); else s0 = new idx<T>(d);
      if (s1) s1->resize(d); else s1 = new idx<T>(d);
      if (s2) s2->resize(d); else s2 = new idx<T>(d);
      idx_fill(*s0, (T)BACK_MIN);
      idx_fill(*s1, (T)BACK_MIN);
      idx_fill(*s2, (T)BACK_MIN);
    }
    TIMING_RESIZING_ACCSTOP(); // stop accumulating resizing time
    return true;
  }

  template <typename T, class Tstate>
  fidxdim back_module<T, Tstate>::bprop_size(const fidxdim &osize) {
    pixel_size = osize;
    cout << "back_module: 1 output pixel corresponds here to " << pixel_size
	 << endl;
    return osize;
  }

  template <typename T, class Tstate>
  std::string back_module<T, Tstate>::describe() {
    std::string desc;
    desc << "back module " << this->name();
    return desc;
  }

  ////////////////////////////////////////////////////////////////
  // printer_module

  template <typename T, class Tstate>
  printer_module<T,Tstate>::printer_module(const char* name_)
    : module_1_1<T,Tstate>(name_) {
  }

  template <typename T, class Tstate>
  printer_module<T,Tstate>::~printer_module() {
  }

  template <typename T, class Tstate>
  void printer_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    T max = idx_max(in.x);
    T min = idx_min(in.x);
    cout << "min: "<< min << "\tmax: "<< max <<endl;
    idx_copy(in.x, out.x);
  }

  template <typename T, class Tstate>
  void printer_module<T,Tstate>::bprop(Tstate &in,
					 Tstate &out) {
    T max = idx_max(out.dx);
    T min = idx_min(out.dx);
    cout << "min: "<< min << "\tmax: "<< max <<endl;
    idx_copy(out.dx, in.dx);
  }

  template <typename T, class Tstate>
  void printer_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    T max = idx_max(out.ddx);
    T min = idx_min(out.ddx);
    cout << "min: "<< min << "\tmax: "<< max <<endl;
    idx_copy(out.ddx, in.ddx);
  }


} // end namespace ebl
