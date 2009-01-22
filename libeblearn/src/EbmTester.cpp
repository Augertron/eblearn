/*
 * EbmTester.cpp
 *
 *  Created on: Sep 26, 2008
 *      Author: koray
 */

#include "EbmTester.h"

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
				       module_1_1<state_idx,state_idx>* module,
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

void ModuleTester::get_jacobian_fprop(module_1_1<state_idx,state_idx> *module, 
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

void ModuleTester::get_jacobian_bprop(module_1_1<state_idx,state_idx> *module, 
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
