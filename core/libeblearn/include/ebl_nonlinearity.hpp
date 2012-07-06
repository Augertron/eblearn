/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
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

  template <typename T, class Tstate>
  stdsigmoid_module<T,Tstate>::stdsigmoid_module()
  : module_1_1<T,Tstate>("stdsigmoid") {
  }

  template <typename T, class Tstate>
  stdsigmoid_module<T,Tstate>::~stdsigmoid_module() {
  }

  // standard sigmoid module
  template <typename T, class Tstate>
  void stdsigmoid_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize iff necessary
    this->resize_output(in, tmp); // resize iff necessary
    idx_stdsigmoid(in.x, out.x);
  }

  template <typename T, class Tstate>
  void stdsigmoid_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    idx_dstdsigmoid(in.x, tmp);
    idx_mulacc(tmp, out.dx, in.dx);
  }

  template <typename T, class Tstate>
  void stdsigmoid_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    idx_dstdsigmoid(in.x, tmp);
    idx_mul(tmp, tmp, tmp);
    idx_mulacc(tmp, out.ddx, in.ddx);
  }

  template <typename T, class Tstate>
  stdsigmoid_module<T,Tstate>* stdsigmoid_module<T,Tstate>::copy() {
    return new stdsigmoid_module<T,Tstate>();
  }
  
  ////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  tanh_module<T,Tstate>::tanh_module() : module_1_1<T,Tstate>("tanh") {}

  template <typename T, class Tstate>
  tanh_module<T,Tstate>::~tanh_module() {}

  // tanh module
  template <typename T, class Tstate>
  void tanh_module<T,Tstate>::fprop(Tstate &in, Tstate &out) {
    this->resize_output(in, out); // resize iff necessary
    this->resize_output(in, tmp); // resize iff necessary
    idx_tanh(in.x, out.x);
  }

  template <typename T, class Tstate>
  void tanh_module<T,Tstate>::bprop(Tstate &in, Tstate &out) {
    idx_dtanh(in.x, tmp);
    idx_mulacc(tmp, out.dx, in.dx);
  }

  template <typename T, class Tstate>
  void tanh_module<T,Tstate>::bbprop(Tstate &in, Tstate &out) {
    idx_dtanh(in.x, tmp);
    idx_mul(tmp, tmp, tmp);
    idx_mulacc(tmp, out.ddx, in.ddx);
  }

  template <typename T, class Tstate>
  tanh_module<T,Tstate>* tanh_module<T,Tstate>::copy() {
    return new tanh_module<T,Tstate>();
  }
  
  ////////////////////////////////////////////////////////////////////////

  template <typename T, class Tstate>
  softmax<T,Tstate>::softmax(double b) : module_1_1<T,Tstate>("softmax") {
    beta = b;
  }

  template <typename T, class Tstate>
  void softmax<T,Tstate>::resize_nsame(Tstate &in, Tstate &out, int n){
    int nmax = in.x.order();
    if(n==0||n>nmax) {eblerror("illegal type")}
    else{
      switch(n){
      case 1: out.resize(in.x.dim(0));
	break;
      case 2: out.resize(in.x.dim(0), in.x.dim(1));
	break;
      case 3: out.resize(in.x.dim(0), in.x.dim(1), in.x.dim(2));
	break;
      case 4: out.resize(in.x.dim(0), in.x.dim(1), in.x.dim(2), 
			  in.x.dim(3));
	break;
      case 5: out.resize(in.x.dim(0), in.x.dim(1), in.x.dim(2), 
			  in.x.dim(3), in.x.dim(4));
	break;
      case 6: out.resize(in.x.dim(0), in.x.dim(1), in.x.dim(2), 
			  in.x.dim(3), in.x.dim(4), in.x.dim(5));
	break;
      }
    }
  }

  template <typename T, class Tstate>
  void softmax<T,Tstate>::fprop(Tstate &in, Tstate &out){
    int n=in.x.order();
    if(n==0){
      idx<double> ib;
      ib.set(1);
      idx_copy(ib, out.x);
    }
    else {
      resize_nsame(in, out, n);
      if( n > 6) {eblerror("illegal type")}
      else{
	idx<double> pp(new srg<double>(), in.x.spec);
	idx<double> dot(new srg<double>(), in.x.spec);
	double mm = idx_max(in.x);
	idx_addc(in.x, -mm, pp);
	idx_dotc(pp, beta, dot);
	double out_sum = 0.0;
	double d = idx_sum(dot, &out_sum);
	idx_dotc(dot, (double)(1/d), out.x);
      }
    }
  }

  template <typename T, class Tstate>
  void softmax<T,Tstate>::bprop(Tstate &in, Tstate &out){
    int n = in.x.order();
    if( n == 0) return;
    if( n > 6 ) { eblerror("illegal type")}
    else{
      idx<double> pp(new srg<double>(), out.dx.spec);
      idx<double> mul(new srg<double>(), out.dx.spec);
      double dot = idx_dot(out.dx, out.x);
      idx_addc(out.dx, -dot, pp);
      idx_mul(out.x, pp, mul);
      idx_dotcacc(mul, beta, in.x);
    }
  }

  template <typename T, class Tstate>
  void softmax<T,Tstate>::bbprop(Tstate &in, Tstate &out){
    int n = in.x.order();
    if( n == 0) return;
    if( n > 6 ) { eblerror("illegal type")}
    else{
      idx<double> mul(new srg<double>(), out.x.spec);
      idx<double> dot(new srg<double>(), out.x.spec);
      idx<double> pp(new srg<double>(), out.x.spec);
      idx<double> mul2(new srg<double>(), out.x.spec);
      idx<double> pp2(new srg<double>(), out.x.spec);
      idx<double> mul3(new srg<double>(), out.x.spec);
      idx_mul(out.x, out.x, mul);
      idx_dotc(out.x, (double)-2, dot);
      idx_addc(dot, (double)1, pp);
      idx_mul(pp, out.ddx, mul2);
      idx_addc(mul2, idx_dot(out.ddx, mul), pp2);
      idx_mul(mul, pp2, mul3);

      idx_dotcacc(mul3, beta*beta, in.ddx);
    }
  }

  ////////////////////////////////////////////////////////////////
  // abs_module

  template <typename T, class Tstate>
  abs_module<T,Tstate>::abs_module(double thres) : module_1_1<T,Tstate>("abs") {
    threshold = thres;
  }

  template <typename T, class Tstate>
  abs_module<T,Tstate>::~abs_module() {
  }

  template <typename T, class Tstate>
  void abs_module<T,Tstate>::fprop(Tstate& in, Tstate& out) {
    this->resize_output(in, out); // resize iff necessary
    idx_abs(in.x, out.x);
  }

  template <typename T, class Tstate>
  void abs_module<T,Tstate>::bprop(Tstate& in, Tstate& out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.dx, out.dx); // must have same dimensions
    
    idx_aloopf3(inx, in.x, T, indx, in.dx, T, outdx, out.dx, T, {
	if (*inx > threshold)
	  *indx = *indx + *outdx;
	else if (*inx < -threshold)
	  *indx = *indx - *outdx;
      });
  }

  template <typename T, class Tstate>
  void abs_module<T,Tstate>::bbprop(Tstate& in, Tstate& out) {
    state_idx_check_different(in, out); // forbid same in and out
    idx_checknelems2_all(in.ddx, out.ddx); // must have same dimensions
    
    idx_add(in.ddx, out.ddx, in.ddx);
  }
  
  template <typename T, class Tstate>
  abs_module<T,Tstate>* abs_module<T,Tstate>::copy() {
    return new abs_module<T,Tstate>();
  }

  //////////////////////////////////////////////////////////////////
  // linear_shrink_module

  template <typename T, class Tstate>
  linear_shrink_module<T,Tstate>::linear_shrink_module(parameter<T,Tstate> *p,
						       intg nf, T bs)
    : module_1_1<T,Tstate>("linear_shrink"), bias(p,nf), default_bias(bs) {
    idx_fill(bias.x, bs);
  }
  
  template <typename T, class Tstate>
  linear_shrink_module<T,Tstate>::~linear_shrink_module(){
  }

  template <typename T, class Tstate>
  void linear_shrink_module<T,Tstate>::fprop(Tstate& in, Tstate& out) {
    if (&in != &out) eblerror("in and out should be different buffers");
    this->resize_output(in, out); // resize iff necessary

    idx_bloop3(inx, in.x, T, outx, out.x, T, biasx, bias.x, T) {
      T b = biasx.get();
      idx_aloopf2(i, inx, T, o, outx, T, {
	  if (*i > b) *o = *i - b;
	  else if (*i < -b) *o = *i + b;
	  else *o = 0; });
    }
  }
  
  template <typename T, class Tstate>
  void linear_shrink_module<T,Tstate>::bprop(Tstate& in, Tstate& out) {
    idx_bloop5(inx, in.x, T, indx, in.dx, T, outdx, out.dx, T, 
	       biasx, bias.x, T, biasdx, bias.dx, T) {
      T b = biasx.get();
      idx_aloopf3(i, inx, T, id, indx, T, od, outdx, T, {
	  if (*i > b) {
	    *id += *od;
	    biasdx.set(biasdx.get() - *od);
	  } else if (*i < -b) {
	    *id += *od;
	    biasdx.set(biasdx.get() - *od);
	  }});
    }
  }
  
  template <typename T, class Tstate>
  void linear_shrink_module<T,Tstate>::bbprop(Tstate& in, Tstate& out){    
    idx_bloop5(inx, in.x, T, inddx, in.ddx, T, outddx, out.ddx, T, 
	       biasx, bias.x, T, biasddx, bias.ddx, T) {
      T b = biasx.get();
      idx_aloopf3(i, inx, T, idd, inddx, T, odd, outddx, T, {
	  if (*i > b) {
	    *idd += *odd;
	    biasddx.set(biasddx.get() - *odd);
	  } else if (*i < -b) {
	    *idd += *odd;
	    biasddx.set(biasddx.get() - *odd);
	  }});
    }
  }
  
  template <typename T, class Tstate>
  linear_shrink_module<T,Tstate>* linear_shrink_module<T,Tstate>::copy() {
    linear_shrink_module<T,Tstate>* s2 =
      new linear_shrink_module<T,Tstate>(NULL, bias.x.dim(0), default_bias);
    // assign same parameter state
    s2->bias = bias;
    return s2;
  }

  template <typename T, class Tstate>
  std::string linear_shrink_module<T,Tstate>::describe() {
    std::string desc;
    desc << "linear_shrink module " << this->name() << " with biases: " 
	 << bias.x << " min: " << idx_min(bias.x) 
	 << " max: " << idx_max(bias.x);
    return desc;
  }

  //////////////////////////////////////////////////////////////////
  // smooth_shrink_module

  template <typename T, class Tstate>
  smooth_shrink_module<T,Tstate>::smooth_shrink_module(parameter<T,Tstate> *p,
						       intg nf, T bt, T bs)
    : module_1_1<T,Tstate>("smooth_shrink"), 
      beta(p,nf), bias(p,nf), ebb(1), ebx(1,1,1), tin(1,1,1), absmod(0.0),
      default_beta(bt), default_bias(bs) {
    idx_fill(beta.x, bt);
    idx_fill(bias.x, bs);
  }
  
  template <typename T, class Tstate>
  smooth_shrink_module<T,Tstate>::~smooth_shrink_module(){
  }

  template <typename T, class Tstate>
  void smooth_shrink_module<T,Tstate>::fprop(Tstate& in, Tstate& out) {
    if (&in != &out) { // resize only when input and output are different
      idxdim d(in.x.spec); // use same dimensions as in
      out.resize(d);
    } else
      eblerror("in and out should be different buffers");
    absmod.fprop(in,tin);
    // failsafe
    idx_aloopf1(x, in.x, T, {
	if (*x > 20)
	  *x = 20;
      });
    ebb.resize(bias.x.dim(0));
    ebx.resize(in.x.get_idxdim());
    
    idx_mul(beta.x, bias.x, ebb.x);
    idx_exp(ebb.x);

    idx_bloop5(inx, tin.x, T, outx, out.x, T, ebbx, ebb.x, T,
	       betax, beta.x, T, biasx, bias.x, T) {
      idx_dotc(inx, betax.get(), outx);
      idx_exp(outx);
      idx_addc(outx, ebbx.get()-1, outx);
      idx_log(outx);
      idx_dotc(outx, 1/betax.get(), outx);
      idx_addc(outx, -biasx.get(), outx);
    }
    idx_aloopf2(x, in.x, T, y, out.x, T, {
	if (abs((int)*x) > 20)
	  *y = *x;
	if(*x < 0.0) {
	  *y = -(*y);
	}
      });
  }
  
  template <typename T, class Tstate>
  void smooth_shrink_module<T,Tstate>::bprop(Tstate& in, Tstate& out) {
    absmod.fprop(in,tin);
    // failsafe
    idx_aloopf1(x, in.x, T, {
	if (*x > 20)
	  *x = 20;
      });
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
      beta.dx[i].set((T)idx_dot(outdxi,ttx));
    }
    idx_add(in.dx,tin.dx,in.dx);
  }
  
  template <typename T, class Tstate>
  void smooth_shrink_module<T,Tstate>::bbprop(Tstate& in, Tstate& out){    
    absmod.fprop(in,tin);
    // failsafe
    idx_aloopf1(x, in.x, T, {
	if (*x > 20)
	  *x = 20;
      });
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
      bias.ddx[i].set((T)idx_dot(outdxi,ttx));
      
      // df/dbeta
      idx_mul(tinxi,ebxxi,ttx);
      idx_addc(ttx, bias.x[i].get() * ebb.x[i].get(),ttx);
      idx_mul(ttx,ebxddxi,ttx);
      idx_dotc(ttx, 1/beta.x[i].get(),ttx);
      idx_log(ebxdxi);
      idx_dotc(ebxdxi,-1/(beta.x[i].get()*beta.x[i].get()),tty);
      idx_add(ttx,tty,ttx);
      idx_mul(ttx,ttx,ttx);
      beta.ddx[i].set((T)idx_dot(outdxi,ttx));
    }
    idx_add(in.ddx,tin.ddx,in.ddx);
  }
  
  template <typename T, class Tstate>
  smooth_shrink_module<T,Tstate>* smooth_shrink_module<T,Tstate>::copy() {
    smooth_shrink_module<T,Tstate>* s2 =
      new smooth_shrink_module<T,Tstate>(NULL, beta.x.dim(0),
					 default_beta, default_bias);
    // assign same parameter state
    s2->beta = beta;
    s2->bias = bias;
    return s2;
  }

  //////////////////////////////////////////////////////////////////
  // tanh_shrink_module

  template <typename T, class Tstate>
  tanh_shrink_module<T,Tstate>::
  tanh_shrink_module(parameter<T,Tstate> *p, intg nf, bool diags_)
    : module_1_1<T,Tstate>("tanh_shrink"),
      nfeatures(nf), alpha(NULL), beta(NULL), diags(diags_) {
    if (diags) {
      alpha = new diag_module<T,Tstate>(p, nf);
      beta = new diag_module<T,Tstate>(p, nf);
    }
  }
  
  template <typename T, class Tstate>
  tanh_shrink_module<T,Tstate>::~tanh_shrink_module() {
    if (alpha) delete alpha;
    if (beta) delete beta;
  }

  template <typename T, class Tstate>
  void tanh_shrink_module<T,Tstate>::fprop(Tstate& in, Tstate& out) {
    if (&in != &out) { // resize only when input and output are different
      this->resize_output(in, out); // resize iff necessary
    } else eblerror("in and out should be different buffers");
    // fprop
    if (diags) { // use coefficients
      // x * alpha
      alpha->fprop(in, abuf);
      // tanh(x * alpha)
      mtanh.fprop(abuf, tbuf);
      // (x * alpha) - tanh(x * alpha)
      difmod.fprop(in, tbuf, bbuf);
      // beta * ((x * alpha) - tanh(x * alpha))
      beta->fprop(bbuf, out);
    } else { // no coefficients
      // tanh(x)
      mtanh.fprop(in, tbuf);
      // x - tanh(x)
      difmod.fprop(in, tbuf, out);
    }
  }
  
  template <typename T, class Tstate>
  void tanh_shrink_module<T,Tstate>::bprop(Tstate& in, Tstate& out) {
    // clear derivatives
    tbuf.clear_dx();
    // bprop
    if (diags) { // use coefficients
      // clear derivatives
      abuf.clear_dx();
      bbuf.clear_dx();
      // bprop
      beta->bprop(bbuf, out);
      difmod.bprop(in, tbuf, bbuf);
      mtanh.bprop(abuf, tbuf);
      alpha->bprop(in, abuf);
    } else { // no coefficients
      difmod.bprop(in, tbuf, out);
      mtanh.bprop(in, tbuf);
    }
  }
  
  template <typename T, class Tstate>
  void tanh_shrink_module<T,Tstate>::bbprop(Tstate& in, Tstate& out) {
    tbuf.clear_ddx();
    // bbprop
    if (diags) { // use coefficients
      // clear derivatives
      abuf.clear_ddx();
      bbuf.clear_ddx();
      // bprop
      beta->bbprop(bbuf, out);
      difmod.bbprop(in, tbuf, bbuf);
      mtanh.bbprop(abuf, tbuf);
      alpha->bbprop(in, abuf);
    } else { // no coefficients
      difmod.bbprop(in, tbuf, out);
      mtanh.bbprop(in, tbuf);
    }
  }
  
  template <typename T, class Tstate>
  tanh_shrink_module<T,Tstate>* tanh_shrink_module<T,Tstate>::copy() {
    tanh_shrink_module<T,Tstate>* s2 =
      new tanh_shrink_module<T,Tstate>(NULL, nfeatures);
    // assign same parameter state
    if (s2->alpha) delete s2->alpha;
    if (s2->beta) delete s2->beta;
    s2->alpha = alpha->copy();
    s2->beta = beta->copy();
    return s2;
  }

  template <typename T, class Tstate>
  std::string tanh_shrink_module<T,Tstate>::describe() {
    std::string desc;
    desc << "tanh_shrink module " << this->name() 
	 << (diags ? " with" : " without") << " scaling coefficients";
    return desc;
  }

} // end namespace ebl
