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

#include "ebl_nonlinearity.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////

  stdsigmoid_module::stdsigmoid_module() {
  }
  stdsigmoid_module::~stdsigmoid_module() {
  }

  // standard sigmoid module
  void stdsigmoid_module::fprop(state_idx &in, state_idx &out) {
    idx_stdsigmoid(in.x, out.x);
  }

  void stdsigmoid_module::bprop(state_idx &in, state_idx &out) {
    idx_dstdsigmoid(in.x, in.dx);
    idx_mul(in.dx, out.dx, in.dx);
  }

  void stdsigmoid_module::bbprop(state_idx &in, state_idx &out) {
    idx_dstdsigmoid(in.x, in.ddx);
    idx_mul(in.ddx, in.ddx, in.ddx);
    idx_mul(in.ddx, out.ddx, in.ddx);
  }

  ////////////////////////////////////////////////////////////////

  // tanh module
  void tanh_module::fprop(state_idx &in, state_idx &out) {
    idx_tanh(in.x, out.x);
  }

  void tanh_module::bprop(state_idx &in, state_idx &out) {
    idx_dtanh(in.x, in.dx);
    idx_mul(in.dx, out.dx, in.dx);
  }

  void tanh_module::bbprop(state_idx &in, state_idx &out) {
    idx_dtanh(in.x, in.ddx);
    idx_mul(in.ddx, in.ddx, in.ddx);
    idx_mul(in.ddx, out.ddx, in.ddx);
  }

  void tanh_module::forget(forget_param_linear& fp) {
  }

  void tanh_module::normalize() {
  }

  ////////////////////////////////////////////////////////////////////////

  softmax::softmax(double b){
    beta = b;
  }

  void softmax::resize_nsame(state_idx &in, state_idx &out, int n){
    int nmax = in.x.order();
    if(n==0||n>nmax) {ylerror("illegal type")}
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

  void softmax::fprop( state_idx &in, state_idx &out){
    int n=in.x.order();
    if(n==0){
      idx<double> ib;
      ib.set(1);
      idx_copy(ib, out.x);
    }
    else {
      resize_nsame(in, out, n);
      if( n > 6) {ylerror("illegal type")}
      else{
	idx<double> pp(new Srg<double>(), in.x.spec);
	idx<double> dot(new Srg<double>(), in.x.spec);
	double mm = idx_max(in.x);
	idx_addc(in.x, -mm, pp);
	idx_dotc(pp, beta, dot);
	double out_sum = 0.0;
	double d = idx_sum(dot, &out_sum);
	idx_dotc(dot, (double)(1/d), out.x);
      }
    }
  }

  void softmax::bprop( state_idx &in, state_idx &out){
    int n = in.x.order();
    if( n == 0) return;
    if( n > 6 ) { ylerror("illegal type")}
    else{
      idx<double> pp(new Srg<double>(), out.dx.spec);
      idx<double> mul(new Srg<double>(), out.dx.spec);
      double dot = idx_dot(out.dx, out.x);
      idx_addc(out.dx, -dot, pp);
      idx_mul(out.x, pp, mul);
      idx_dotcacc(mul, beta, in.x);
    }
  }

  void softmax::bbprop( state_idx &in, state_idx &out){
    int n = in.x.order();
    if( n == 0) return;
    if( n > 6 ) { ylerror("illegal type")}
    else{
      idx<double> mul(new Srg<double>(), out.x.spec);
      idx<double> dot(new Srg<double>(), out.x.spec);
      idx<double> pp(new Srg<double>(), out.x.spec);
      idx<double> mul2(new Srg<double>(), out.x.spec);
      idx<double> pp2(new Srg<double>(), out.x.spec);
      idx<double> mul3(new Srg<double>(), out.x.spec);
      idx_mul(out.x, out.x, mul);
      idx_dotc(out.x, (double)-2, dot);
      idx_addc(dot, (double)1, pp);
      idx_mul(pp, out.ddx, mul2);
      idx_addc(mul2, idx_dot(out.ddx, mul), pp2);
      idx_mul(mul, pp2, mul3);

      idx_dotcacc(mul3, beta*beta, in.ddx);
    }
  }

} // end namespace ebl
