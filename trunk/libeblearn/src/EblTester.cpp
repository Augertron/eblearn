/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun, Pierre Sermanet, Koray Kavukcuoglu *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com, koray@cs.nyu.edu *
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

#include "EblTester.h"

using namespace std;

namespace ebl {

  ModuleTester::ModuleTester() {
    this->out = stdout;
    this->acc_thres = 1e-8;
    this->rrange = 2;
    this->jac_fprop = Idx<double>(1,1);
    this->jac_bprop = Idx<double>(1,1);
    this->jac_fprop_param = Idx<double>(1,1);
    this->jac_bprop_param = Idx<double>(1,1);

    kk = new Idx<double>(100,100);
    init_drand(time(NULL));
  }

  ModuleTester::ModuleTester(FILE* out, double thres, double rrange) {
    this->out = out;
    this->acc_thres = thres;
    this->rrange = rrange;
  }

  ModuleTester::ModuleTester(double thres, double rrange) {
    this->out = stdout;
    this->acc_thres = thres;
    this->rrange = rrange;
  }

  ModuleTester::~ModuleTester() {
    delete kk;
  }

  void ModuleTester::test_jacobian(module_1_1<state_idx,state_idx> *module, 
				   state_idx *in, state_idx *out)
  {
    forget_param_linear fp(2,0);

    // just to resize states
    module->fprop(in,out);
    // randomize parametes if there are any
    module->forget(fp);
    // clear all input and output
    in->clear();
    in->clear_dx();
    in->clear_ddx();
    out->clear();
    out->clear_dx();
    out->clear_ddx();

    // randomize input for fprop
    randomize_idx(in->x);

    get_jacobian_fprop(module,in,out,jac_fprop);
    get_jacobian_bprop(module,in,out,jac_bprop);

    std::cout <<"kk"<<std::endl;
    report_err(jac_fprop,jac_bprop,"jacobian input");
  }

  void ModuleTester::test_jacobian_param(parameter *p, 
					 module_1_1<state_idx,state_idx>
					 *module,
					 state_idx *in, state_idx *out)
  {
    forget_param_linear fp(2,0);

    // just to resize states
    module->fprop(in,out);
    // randomize parametes if there are any
    module->forget(fp);
    // clear all input and output
    in->clear();
    in->clear_dx();
    in->clear_ddx();
    out->clear();
    out->clear_dx();
    out->clear_ddx();

    // randomize input for fprop
    randomize_idx(p->x);
    randomize_idx(in->x);

    get_jacobian_fprop_param(p,module,in,out,jac_fprop_param);
    in->clear_dx();
    get_jacobian_bprop_param(p,module,in,out,jac_bprop_param);

    report_err(jac_fprop_param,jac_bprop_param,"jacobian param");
  }

  void ModuleTester::get_jacobian_fprop(module_1_1<state_idx,state_idx> 
					*module, 
					state_idx *in, state_idx *out,
					Idx<double>& jac)
  {
    state_idx sina = in->make_copy(); //x-small
    state_idx sinb = in->make_copy(); //x+small
    state_idx souta = out->make_copy(); //f(x-small)
    state_idx soutb = out->make_copy(); //f(x+small)
    double small = 1e-6;
    int cnt = 0;
    // clear out jacobian matrix
    jac.resize(in->size(),out->size());
    idx_clear(jac);
    {
      idx_aloop3(sx,in->x,double,sxa,sina.x,double,sxb,sinb.x,double){
	idx_copy(in->x,sina.x);
	idx_copy(in->x,sinb.x);
	// perturb
	*sxa = *sx - small;
	*sxb = *sx + small;
	module->fprop(&sina,&souta);
	module->fprop(&sinb,&soutb);
	idx_sub(soutb.x,souta.x,soutb.x);
	Idx<double> j = jac.select(0,cnt);
	idx_dotc(soutb.x,1.0/(2*small),j);
	cnt++;
      }
    }
  }

  void ModuleTester::get_jacobian_fprop_param(parameter *p, 
					      module_1_1<state_idx,state_idx> 
					      *module, 
					      state_idx *in, 
					      state_idx *out,Idx<double>& jac)
  {
    state_idx souta = out->make_copy(); //f(x-small)
    state_idx soutb = out->make_copy(); //f(x+small)
    double small = 1e-6;
    int cnt = 0;
    // clear out jacobian matrix
    idx_clear(jac);
    {
      idx_aloop1(px,p->x,double){
	// perturb
	*px = *px - small;
	module->fprop(in,&souta);
	*px = *px + 2*small;
	module->fprop(in,&soutb);
	*px = *px - small;
	idx_sub(soutb.x,souta.x,soutb.x);
	Idx<double> j = jac.select(0,cnt);
	idx_dotc(soutb.x,1.0/(2*small),j);
	cnt++;
      }
    }
  }

  void ModuleTester::get_jacobian_bprop(module_1_1<state_idx,state_idx> 
					*module, 
					state_idx *in, state_idx *out,
					Idx<double>& jac)
  {
    jac.resize(in->size(),out->size());
    idx_clear(jac);
    int cnt = 0;
    {
      idx_aloop1(dx,out->dx,double){
	idx_clear(out->dx);
	idx_clear(in->dx);
	*dx = 1.0;
	module->bprop(in,out);
	Idx<double> j = jac.select(1,cnt);
	idx_copy(in->dx,j);
	cnt++;
      }
    }
  }

  void ModuleTester::get_jacobian_bprop_param(parameter *p, 
					      module_1_1<state_idx,state_idx> 
					      *module, 
					      state_idx *in, state_idx *out,
					      Idx<double>& jac)
  {
  }

  void ModuleTester::report_err(Idx<double>& a, Idx<double>& b, const char* msg)
  {
    double maxdist;
    double totdist = idx_sqrdist(a,b);
    std::stringstream ss(std::stringstream::in |std::stringstream::out);

    // max distance
    idx_sub(a,b,a);
    idx_abs(a,a);
    maxdist = idx_max(a);

    // report results
    ss << "Max " << msg << " distance";
    fprintf(this->out,"%-40s = %-15g %15s\n",ss.str().c_str(),maxdist,
	    ((maxdist<this->acc_thres)?"OK":"NOT OK"));
    ss.str("");
    ss << "Total " << msg << "distance";
    fprintf(this->out,"%-40s = %-15g %15s\n",ss.str().c_str(),totdist,
	    ((totdist<this->acc_thres)?"OK":"NOT OK"));
    fflush(this->out);
  }

  void ModuleTester::randomize_idx(Idx<double>& m)
  {
    idx_aloop1(v,m,double) {
      *v = drand(this->rrange);
    }
  }

  double ModuleTester::get_acc_thres() const
  {
    return acc_thres;
  }

  void ModuleTester::set_acc_thres(double acc_thres)
  {
    this->acc_thres = acc_thres;
  }

  double ModuleTester::get_rrange() const
  {
    return rrange;
  }

  void ModuleTester::set_rrange(double rrange)
  {
    this->rrange = rrange;
  }

  FILE* ModuleTester::get_out() const
  {
    return out;
  }

  void ModuleTester::set_out(FILE* out)
  {
    this->out = out;
  }


  ////////////////////////////////////////////////////////////////

  void Jacobian_tester::test(module_1_1<state_idx, state_idx> *module){

    int insize = 16;
    state_idx *in = new state_idx(insize, 1, 1);
    state_idx *out = new state_idx(insize, 1, 1);

    //init
    dseed(2);  // 2 is chosen randomly... feel free to change it
    module->fprop(in, out); // used to resize the outputs
    { idx_bloop1( i, in->x, double)
	{ idx_bloop1 (ii, i, double)
	    { idx_bloop1( iii, ii, double)
		{ iii.set(drand(2)); }
	    }
	}
    }
    { idx_bloop1( o, out->x, double)
	{ idx_bloop1 (oo, o, double)
	    { idx_bloop1( ooo, oo, double)
		{ ooo.set(drand(2)); }
	    }
	}
    }


    // check the Jacobian
    int ndim_in = in->x.nelements();
    int ndim_out = in->x.nelements();
    // used to store the jacobian calculated via bprop
    Idx<double> jac_fprop(ndim_in, ndim_out); 
    //  used to store the jacobian calculated via prturbations
    Idx<double> jac_bprop(ndim_in, ndim_out); 

    // creation of jac_fprop
    module->fprop(in, out);
    int cnt = 0;
    { idx_bloop1(o, out->x, double)
	{ idx_bloop1(oo, o, double)
	    { idx_bloop1(ooo, oo, double)
		{
		  out->clear_dx();
		  in->clear_dx();
		  ooo.set(1);
		  module->bprop(in, out);
		  Idx<double> bla = jac_bprop.select(1, cnt);
		  idx_copy(in->dx, bla);
		  cnt++;
		}
	    }
	}
    }

    // creation of jac_bprop
    cnt = 0;
    double small = pow(10.0, -6);
    state_idx *in1 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *in2 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *out1 = new state_idx( 1, 1, 1);
    state_idx *out2 = new state_idx( 1, 1, 1);
    for(int d1 = 0; d1 < in->x.dim(0); d1++){
      for(int d2 = 0; d2 < in->x.dim(1); d2++){
	for(int d3 = 0; d3 < in->x.dim(2); d3++){
	  idx_copy(in->x, in1->x);
	  idx_copy(in->x, in2->x);
	  in1->x.set(in1->x.get( d1, d2, d3) + small, d1, d2, d3);
	  in2->x.set(in2->x.get( d1, d2, d3) - small, d1, d2, d3);
	  module->fprop(in1, out1);
	  module->fprop(in2, out2);
	  Idx<double> sub(new Srg<double>(), out1->x.spec);
	  Idx<double> dot(new Srg<double>(), out1->x.spec);
	  idx_sub(out1->x, out2->x, sub);
	  idx_dotc(sub, 0.5/small, dot);
	  Idx<double> bla2 = jac_fprop.select(0, cnt);
	  idx_copy(dot, bla2);
	  cnt++;
	}
      }
    }

    // comparison
    printf("Jacobian error: %8.7e \n", idx_sqrdist(jac_fprop, jac_bprop));
  }

  ////////////////////////////////////////////////////////////////////////

  void Bbprop_tester::test(module_1_1<state_idx, state_idx> *module){

    int insize = 16;
    state_idx *in = new state_idx(insize, 1, 1);
    state_idx *out = new state_idx(insize, 1, 1);

    //init
    dseed(2);  // 2 is chosen randomly... feel free to change it
    module->fprop(in, out); // used to resize the outputs
    { idx_bloop1( i, in->x, double)
	{ idx_bloop1 (ii, i, double)
	    { idx_bloop1( iii, ii, double)
		{ iii.set(drand(2)); }
	    }
	}
    }
    { idx_bloop1( o, out->x, double)
	{ idx_bloop1 (oo, o, double)
	    { idx_bloop1( ooo, oo, double)
		{ ooo.set(drand(2)); }
	    }
	}
    }

    module->fprop(in, out);
    module->bprop(in, out);
    module->bbprop(in, out);

    // used to store the bbprop calculated via perturbation
    Idx<double> bbprop_p(in->x.dim(0), in->x.dim(1), in->x.dim(2)); 

    // creation of bbprop_p
    int cnt = 0;
    double small = pow(10.0, -6);
    state_idx *in1 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *in2 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *out1 = new state_idx( 1, 1, 1);
    state_idx *out2 = new state_idx( 1, 1, 1);
    for(int d1 = 0; d1 < in->x.dim(0); d1++){
      for(int d2 = 0; d2 < in->x.dim(1); d2++){
	for(int d3 = 0; d3 < in->x.dim(2); d3++){
	  idx_copy(in->x, in1->x);
	  idx_copy(in->x, in2->x);
	  in1->x.set(in1->x.get( d1, d2, d3) + small, d1, d2, d3);
	  in2->x.set(in2->x.get( d1, d2, d3) - small, d1, d2, d3);
	  module->fprop(in1, out1);
	  module->fprop(in2, out2);
	  // here we calculate a in aX²+bX+c as a model for the 3 points 
	  // calculated via
	  // fprop(...), fprop(...+small) and fprop(...-small). the second 
	  // derivative is then 2*a
	  Idx<double> ad(new Srg<double>(), out1->x.spec);
	  Idx<double> sub(new Srg<double>(), out1->x.spec);
	  Idx<double> dot(new Srg<double>(), out1->x.spec);
	  Idx<double> dot2(new Srg<double>(), out1->x.spec);
	  idx_add(out1->x, out2->x, ad);
	  idx_dotc(out->x, (double)2, dot);
	  idx_sub(ad, dot, sub);
	  idx_dotc(sub, 1/small, dot2);
	  bbprop_p.set(dot2.get( d1, d2, d3), d1, d2, d3);
	  cnt++;
	}
      }
    }

    // comparison
    printf("bbprop error: %8.7e \n", idx_sqrdist(in->ddx, bbprop_p));
  }

  ////////////////////////////////////////////////////////////////////////

  void Bprop_tester::test(module_1_1<state_idx, state_idx> *module){

    int insize = 16;
    state_idx *in = new state_idx(insize, 1, 1);
    state_idx *out = new state_idx(insize, 1, 1);

    //init
    dseed(2);  // 2 is chosen randomly... feel free to change it
    module->fprop(in, out); // used to resize the outputs
    { idx_bloop1( i, in->x, double)
	{ idx_bloop1 (ii, i, double)
	    { idx_bloop1( iii, ii, double)
		{ iii.set(drand(2)); }
	    }
	}
    }
    { idx_bloop1( o, out->x, double)
	{ idx_bloop1 (oo, o, double)
	    { idx_bloop1( ooo, oo, double)
		{ ooo.set(drand(2)); }
	    }
	}
    }

    // used to store the bbprop calculated via perturbation
    Idx<double> bprop_p(in->x.dim(0), in->x.dim(1), in->x.dim(2)); 

    // creation of bprop_p
    int cnt = 0;
    double small = pow(10.0, -6);
    state_idx *in1 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *in2 = new state_idx(in->x.dim(0), in->x.dim(1), in->x.dim(2));
    state_idx *out1 = new state_idx( 1, 1, 1);
    state_idx *out2 = new state_idx( 1, 1, 1);
    for(int d1 = 0; d1 < in->x.dim(0); d1++){
      for(int d2 = 0; d2 < in->x.dim(1); d2++){
	for(int d3 = 0; d3 < in->x.dim(2); d3++){
	  idx_copy(in->x, in1->x);
	  idx_copy(in->x, in2->x);
	  in1->x.set(in1->x.get( d1, d2, d3) + small, d1, d2, d3);
	  in2->x.set(in2->x.get( d1, d2, d3) - small, d1, d2, d3);
	  module->fprop(in1, out1);
	  module->fprop(in2, out2);

	  Idx<double> sub(new Srg<double>(), out1->x.spec);
	  Idx<double> dot(new Srg<double>(), out1->x.spec);
	  idx_sub(out1->x, out2->x, sub);
	  idx_dotc(sub, 0.5/small, dot);
	  bprop_p.set(dot.get( d1, d2, d3), d1, d2, d3);
	  cnt++;
	}
      }
    }

    printf("Bprop error : %8.7e \n", idx_sqrdist(in->dx, bprop_p));
  }

} // end namespace ebl
