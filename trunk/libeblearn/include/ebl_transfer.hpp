/***************************************************************************
 *   Copyright (C) 2009 by Yann LeCun and Pierre Sermanet *
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

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // weighted_std_module

  template <class T>
  weighted_std_module<T>::weighted_std_module(uint kernelh, uint kernelw,
					      int nf, bool mirror_,
					      bool threshold_,
					      bool global_norm_)
    : mirror(mirror_),
      convmean(true),
      convvar(true),
      sqrtmod((T) .5), // square root module
      invmod(-1), // inverse module
      sqmod(2), // square module
      // by default, pass 0 for threshold and values
      // but every fprop updates these values
      thres((T) 1.0, (T) 1.0), // threshold module
      // create internal states, explanations are in fprop
      inmean(1, 1, 1),
      inzmean(1, 1, 1),
      inzmeansq(1, 1, 1),
      invar(1, 1, 1),
      instd(1, 1, 1),
      thstd(1, 1, 1),
      invstd(1, 1, 1),
      threshold(threshold_),
      global_norm(global_norm_),
      nfeatures(nf)
  {
    //! create little objects to do math
    //! zero pad borders
    //! convolution module to calculate the weighted sums for mean and std
    //! augment with fsum, since actually we take a cube over all features
    //! that is why I normalize the kernel using all kernel, not one by one.
    
    // create weighting kernel
    w = create_gaussian_kernel<T>(kernelh, kernelw);
    // prepare convolutions and their kernels
    idx<intg> table = one2one_table(nfeatures);
    convolution_module<T> *conv1 =
      new convolution_module<T>(&param, w.dim(0), w.dim(1), 1, 1, table);
    convolution_module<T> *conv2 =
      new convolution_module<T>(&param, w.dim(0), w.dim(1), 1, 1, table);
    idx_bloop1(kx, conv1->kernel.x, T)
      idx_copy(w, kx);
    //! normalize the kernels
    idx_dotc(conv1->kernel.x, 1/idx_sum(conv1->kernel.x), conv1->kernel.x);
    idx_copy(conv1->kernel.x, conv2->kernel.x);
    // create modules
    module_1_1<T> *padding1;
    if (mirror) // switch between zero and mirror padding
      padding1 = new mirrorpad_module<T>((w.dim(0) - 1)/2, (w.dim(1) - 1)/2);
    else
      padding1 = new zpad_module<T>(w);
    convmean.add_module(padding1, new state_idx<T>(1, 1, 1));
    convmean.add_module(conv1, new state_idx<T>(1, 1, 1));
    //! feature sum module to sum along features 
    //! this might be implemented by making the table in above conv module
    //! all to all connection, but that would be very inefficient
    convmean.add_last_module(new fsum_module<T>);
    // convvar
    module_1_1<T> *padding2;
    if (mirror) // switch between zero and mirror padding
      padding2 = new mirrorpad_module<T>((w.dim(0) - 1)/2, (w.dim(1) - 1)/2);
    else
      padding2 = new zpad_module<T>(w);
    convvar.add_module(padding2, new state_idx<T>(1, 1, 1));
    convvar.add_module(conv2, new state_idx<T>(1, 1, 1));
    //! feature sum module to sum along features 
    //! this might be implemented by making the table in above conv module
    //! all to all connection, but that would be very inefficient
    convvar.add_last_module(new fsum_module<T>);
  }
  
  template <class T>
  weighted_std_module<T>::~weighted_std_module() {
  }
      
  //! {<code>
  //!   v_ijk = x_ijk - sum_ipq ( w_pq * x_i,j+p,k+q )
  //!   s_jk = ( sum_ipq (w_pq * (x_i,j+p,k+q)^2 ) ) ^(1/2)
  //!   y_ijk = v_ijk  / max(s_jk, <s>_jk)
  //! </code>}
  template <class T>
  void weighted_std_module<T>::fprop(state_idx<T> &in, state_idx<T> &out) {
    if (global_norm) // global normalization
      idx_std_normalize(in.x, in.x);
    T a = (T) 1e-5; // avoid divisions by zero
    //! sum_j (w_j * in_j)
    convmean.fprop(in, inmean);
    //! in - mean
    difmod.fprop(in, inmean, inzmean);
    //! (in - inmean)^2
    idx_addc(inzmean.x, a, inzmean.x); // TODO: temporary
    sqmod.fprop(inzmean, inzmeansq);
    //! sum_j (w_j (in - mean)^2)
    convvar.fprop(inzmeansq, invar);
    //! sqrt(sum_j (w_j (in - mean)^2))
    idx_addc(invar.x, a, invar.x); // TODO: temporary
    sqrtmod.fprop(invar, instd);
    if (threshold) { // don't update threshold for inputs
      //! update the threshold values in thres
      T mm = (T) (idx_sum(instd.x) / (T) instd.size());
      thres.thres = mm;
      thres.val = mm;
    }
    //! std(std<mean(std)) = mean(std)
    thres.fprop(instd, thstd);
    //! 1/std
    invmod.fprop(thstd, invstd);
    //! out = (in-mean)/std
    mcw.fprop(inzmean, invstd, out);
  }

  template <class T>
  void weighted_std_module<T>::bprop(state_idx<T> &in, state_idx<T> &out) {
    //! clear derivatives from conv module
    param.clear_dx();
    inmean.clear_dx();
    inzmean.clear_dx();
    inzmeansq.clear_dx();
    invar.clear_dx();
    instd.clear_dx();
    thstd.clear_dx();
    invstd.clear_dx();
    //! out = (in-mean)/std
    mcw.bprop(inzmean, invstd, out);
    //! 1/std
    invmod.bprop(thstd, invstd);
    //! std(std<mean(std)) = mean(std)
    thres.bprop(instd, thstd);
    //! sqrt(sum_j (w_j (in^2 - mean^2)))
    sqrtmod.bprop(invar, instd);
    //! sum_j (w_j (in^2 - mean^2))
    convvar.bprop(inzmeansq, invar);
    //! (in - mean)^2
    sqmod.bprop(inzmean, inzmeansq);
    //! in - mean
    difmod.bprop(in, inmean, inzmean);
    //! sum_j (w_j * in_j)
    convmean.bprop(in, inmean);
  }

  template <class T>
  void weighted_std_module<T>::bbprop(state_idx<T> &in, state_idx<T> &out) {
    param.clear_ddx();
    inmean.clear_ddx();
    inzmean.clear_ddx();
    inzmeansq.clear_ddx();
    invar.clear_ddx();
    instd.clear_ddx();
    thstd.clear_ddx();
    invstd.clear_ddx();
    //! out = (in-mean)/std
    mcw.bbprop(inzmean, invstd, out);
    //! 1/std
    invmod.bbprop(thstd, invstd);
    //! std(std<mean(std)) = mean(std)
    thres.bbprop(instd, thstd);
    //! sqrt(sum_j (w_j (in^2 - mean^2)))
    sqrtmod.bbprop(invar, instd);
    //! sum_j (w_j (in^2 - mean^2))
    convvar.bbprop(inzmeansq, invar);
    //! (in - mean)^2
    sqmod.bbprop(inzmean, inzmeansq);
    //! in - mean
    difmod.bbprop(in, inmean, inzmean);
    //! sum_j (w_j * in_j)
    convmean.bbprop(in, inmean);
  }

  template <class T>
  weighted_std_module<T>* weighted_std_module<T>::copy() {
    return new weighted_std_module<T>(w.dim(0), w.dim(1), nfeatures, mirror,
				      threshold, global_norm);
  }
  
  ////////////////////////////////////////////////////////////////
  // abs_module

  template <class T>
  abs_module<T>::abs_module(double thres) {
    threshold = thres;
  }

  template <class T>
  abs_module<T>::~abs_module() {
  }

  template <class T>
  void abs_module<T>::fprop(state_idx<T>& in, state_idx<T>& out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x.spec); // use same dimensions as in
      out.resize(d);
    }
    idx_abs(in.x, out.x);
  }

  template <class T>
  void abs_module<T>::bprop(state_idx<T>& in, state_idx<T>& out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions
    
    idx_aloop3(inx, in.x, T, indx, in.dx, T, outdx, out.dx, T) {
      if (*inx > threshold)
	*indx = *indx + *outdx;
      else if (*inx < -threshold)
      	*indx = *indx - *outdx;
    }
  }

  template <class T>
  void abs_module<T>::bbprop(state_idx<T>& in, state_idx<T>& out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions
    
    idx_add(in.ddx, out.ddx, in.ddx);
  }
  
  template <class T>
  abs_module<T>* abs_module<T>::copy() {
    return new abs_module<T>();
  }

  //////////////////////////////////////////////////////////////////
  // smooth_shrink_module

  template <class T>
  smooth_shrink_module<T>::smooth_shrink_module(parameter<T> *p, intg nf,
						T bt, T bs)
    : beta(p,nf), bias(p,nf), ebb(1), ebx(1,1,1), tin(1,1,1), absmod(0.0),
      default_beta(bt), default_bias(bs) {
    idx_fill(beta.x, bt);
    idx_fill(bias.x, bs);
  }
  
  template <class T>
  smooth_shrink_module<T>::~smooth_shrink_module(){
  }

  template <class T>
  void smooth_shrink_module<T>::fprop(state_idx<T>& in, state_idx<T>& out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x.spec); // use same dimensions as in
      out.resize(d);
    }
    absmod.fprop(in,tin);
    // failsafe
    { idx_aloop1(x, in.x, T) {
      if (*x > 20)
	*x = 20;
      }}
    ebb.resize(bias.x.dim(0));
    ebx.resize_as(in);
    
    idx_mul(beta.x, bias.x, ebb.x);
    idx_exp(ebb.x);

    idx_bloop5(inx, tin.x, T, outx, out.x, T, ebbx, ebb.x, T,
	       betax, beta.x, T, biasx, bias.x, T) {
      idx_dotc(inx,betax.get(),outx);
      idx_exp(outx);
      idx_addc(outx, ebbx.get()-1, outx);
      idx_log(outx);
      idx_dotc(outx, 1/betax.get(), outx);
      idx_addc(outx, -biasx.get(), outx);
    }
    idx_aloop2(x, in.x, T, y, out.x, T) {
      if (abs(*x) > 20)
	*y = *x;
      if(*x < 0.0) {
	*y = -(*y);
      }
    }
  }
  
  template <class T>
  void smooth_shrink_module<T>::bprop(state_idx<T>& in, state_idx<T>& out) {
    absmod.fprop(in,tin);
    // failsafe
    { idx_aloop1(x, in.x, T) {
	if (*x > 20)
	  *x = 20;
      }}
    tin.clear_dx();
    beta.clear_dx();
    bias.clear_dx();

    // bb = exp (beta .* bias)
    idx_mul(beta.x, bias.x, ebb.x);
    idx_exp(ebb.x);
    intg nf = bias.x.dim(0);
    
    idx<T> ttx(ebx.x[0].spec);
    idx<T> tty(ebx.x[0].spec);
    for (intg i=0; i< nf; i++) {
      // ebx = exp(beta * x)
      idx<T> ebxxi = ebx.x[i];
      idx<T> ebxdxi = ebx.dx[i];
      idx<T> ebxddxi = ebx.ddx[i];
      idx<T> tinxi = tin.x[i];
      idx<T> tindxi = tin.dx[i];
      idx<T> outdxi = out.dx[i];

      idx_dotc(tinxi,beta.x[i].get(),ebxxi);
      idx_exp(ebxxi);

      // ebdx = exp(beta*x) + exp(beta*bias) -1
      idx_addc(ebxxi,ebb.x[i].get()-1,ebxdxi);
      // ebddx = exp (beta*x)/ (exp(beta*x) + exp(beta*bias)-1)
      idx_div(ebxxi,ebxdxi,ebxddxi);

      // df/dx
      idx_mul(ebxddxi,outdxi,tindxi);
      
      //cout << tinxi.get(0,0) << tindxi.get(0,0) << endl;

      // ebddx = 1/ebdx
      idx_inv(ebxdxi,ebxddxi);

      // df/dbias
      idx_dotc(ebxddxi,ebb.x[i].get(),ttx);
      idx_addc(ttx,(T)-1.0,ttx);
      bias.dx[i].set(idx_dot(outdxi,ttx));
      
      // df/dbeta
      idx_mul(tinxi,ebxxi,ttx);
      idx_addc(ttx, bias.x[i].get() * ebb.x[i].get(),ttx);
      idx_mul(ttx,ebxddxi,ttx);
      idx_dotc(ttx, 1/beta.x[i].get(),ttx);
      idx_log(ebxdxi);
      idx_dotc(ebxdxi,-1/(beta.x[i].get()*beta.x[i].get()),tty);
      idx_add(ttx,tty,ttx);
      beta.dx[i].set(idx_dot(outdxi,ttx));
    }
    idx_add(in.dx,tin.dx,in.dx);
  }
  
  template <class T>
  void smooth_shrink_module<T>::bbprop(state_idx<T>& in, state_idx<T>& out) {
    absmod.fprop(in,tin);
    // failsafe
    { idx_aloop1(x, in.x, T) {
	if (*x > 20)
	  *x = 20;
      }}
    tin.clear_ddx();
    beta.clear_ddx();
    bias.clear_ddx();

    // bb = exp (beta .* bias)
    idx_mul(beta.x, bias.x, ebb.x);
    idx_exp(ebb.x);
    intg nf = bias.x.dim(0);
    
    idx<T> ttx(ebx.x[0].spec);
    idx<T> tty(ebx.x[0].spec);
    for (intg i=0; i< nf; i++) {
      // ebx = exp(beta * x)
      idx<T> ebxxi = ebx.x[i];
      idx<T> ebxdxi = ebx.dx[i];
      idx<T> ebxddxi = ebx.ddx[i];
      idx<T> tinxi = tin.x[i];
      idx<T> tindxi = tin.ddx[i];
      idx<T> outdxi = out.ddx[i];

      idx_dotc(tinxi,beta.x[i].get(),ebxxi);
      idx_exp(ebxxi);

      // ebdx = exp(beta*x) + exp(beta*bias) -1
      idx_addc(ebxxi,ebb.x[i].get()-1,ebxdxi);
      // ebddx = exp (beta*x)/ (exp(beta*x) + exp(beta*bias)-1)
      idx_div(ebxxi,ebxdxi,ebxddxi);

      // df/dx
      idx_mul(ebxddxi,ebxddxi,ebxddxi);
      idx_mul(ebxddxi,outdxi,tindxi);
      
      //cout << tinxi.get(0,0) << tindxi.get(0,0) << endl;

      // ebddx = 1/ebdx
      idx_inv(ebxdxi,ebxddxi);

      // df/dbias
      idx_dotc(ebxddxi,ebb.x[i].get(),ttx);
      idx_addc(ttx,(T)-1.0,ttx);
      idx_mul(ttx,ttx,ttx);
      bias.ddx[i].set(idx_dot(outdxi,ttx));
      
      // df/dbeta
      idx_mul(tinxi,ebxxi,ttx);
      idx_addc(ttx, bias.x[i].get() * ebb.x[i].get(),ttx);
      idx_mul(ttx,ebxddxi,ttx);
      idx_dotc(ttx, 1/beta.x[i].get(),ttx);
      idx_log(ebxdxi);
      idx_dotc(ebxdxi,-1/(beta.x[i].get()*beta.x[i].get()),tty);
      idx_add(ttx,tty,ttx);
      idx_mul(ttx,ttx,ttx);
      beta.ddx[i].set(idx_dot(outdxi,ttx));
    }
    idx_add(in.ddx,tin.ddx,in.ddx);
  }
  
  template <class T>
  smooth_shrink_module<T>* smooth_shrink_module<T>::copy() {
    smooth_shrink_module<T>* s2 =
      new smooth_shrink_module<T>(NULL, beta.x.dim(0),
				  default_beta, default_bias);
    // copy data
    idx_copy(beta.x, s2->beta.x);
    idx_copy(bias.x, s2->bias.x);
    return s2;
  }

} // end namespace ebl
