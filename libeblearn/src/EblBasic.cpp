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

#include "EblBasic.h"
#include "IdxIO.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // linear_module

  linear_module::linear_module(parameter *p, intg in, intg out)
  {
    w = new state_idx(p, out, in);
  }

  linear_module::~linear_module()
  {
    delete w;
  }

  void linear_module::fprop(state_idx *in, state_idx *out)
  {
    //	intg instride = 0;
    //	intg outstride = 0;
    //	if (in->x.order() == 1)
    //	{
    //		instride = in->x.mod(0);
    //	}
    //	else if (in->x.contiguousp())
    //	{
    //		instride = 1;
    //	}
    //	if (out->x.order() == 1)
    //	{
    //		outstride = out->x.mod(0);
    //		out->resize(w->x.dim(0));
    //	}
    //	else if (out->x.contiguousp())
    //	{
    //		outstride = 1;
    //	}
    //	if ( (instride == 0)||(outstride==0))
    //	{
    //		ylerror("linear_module::fprop: state must 1D or contiguous");
    //	}
    //	else
    //	{
    //		if (out->x.nelements() != w->x.dim(0))
    //		{
    //			ylerror("linear_module::fprop: output has wrong size");
    //		}
    //		else
    //		{
    //			idx_m2dotm1(w->x, in->x, out->x);
    //		}
    //	}
    Idx<double> inx = in->x.view_as_order(1);
    out->resize(w->x.dim(0));
    idx_m2dotm1(w->x, inx, out->x);
  }

  void linear_module::bprop(state_idx *in, state_idx *out)
  {
    Idx<double> inx = in->x.view_as_order(1);
    Idx<double> indx = in->dx.view_as_order(1);
    intg instride = 0;
    intg outstride = 0;
    if (inx.order() == 1)
      {
	instride = inx.mod(0);
      }
    else if (inx.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("linear_module::fprop: state must 1D or contiguous");
      }
    else
      {
	if (out->x.nelements() != w->x.dim(0))
	  {
	    ylerror("linear_module::fprop: output has wrong size");
	  }
	else
	  {
	    Idx<double> twx(w->x.transpose(0, 1));
	    idx_m1extm1(out->dx, inx, w->dx);
	    idx_m2dotm1(twx, out->dx, indx);
	  }
      }
  }

  void linear_module::bbprop(state_idx *in, state_idx *out)
  {
    Idx<double> inx = in->x.view_as_order(1);
    Idx<double> inddx = in->ddx.view_as_order(1);
    intg instride = 0;
    intg outstride = 0;
    if (inx.order() == 1)
      {
	instride = inx.mod(0);
      }
    else if (inx.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("linear_module::fprop: state must 1D or contiguous");
      }
    else
      {
	if (out->x.nelements() != w->x.dim(0))
	  {
	    ylerror("linear_module::fprop: output has wrong size");
	  }
	else
	  {
	    Idx<double> twx = w->x.transpose(0, 1);
	    idx_m1squextm1(out->ddx, inx, w->ddx);
	    idx_m2squdotm1(twx, out->ddx, inddx);
	  }
      }
  }

  void linear_module::forget(forget_param_linear &fp)
  {
    double fanin = w->x.dim(1);
    double z = fp.value / pow(fanin, fp.exponent);
    if(!drand_ini) printf("You have not initialized random sequence. \
Please call init_drand() before using this function !\n");
    idx_aloop1(lx,w->x,double)
      {	*lx = drand(z);}
  }

  void linear_module::normalize()
  {
    norm_columns(w->x);
  }

  ////////////////////////////////////////////////////////////////
  // linear_module 

  linear_module_dim0::linear_module_dim0(parameter *p, intg in, intg out)
    : linear_module(p, in, out) {
  }

  void linear_module_dim0::fprop(state_idx *in, state_idx *out)
  {
    //	intg instride = 0;
    //	intg outstride = 0;
    //	if (in->x.order() == 1)
    //	{
    //		instride = in->x.mod(0);
    //	}
    //	else if (in->x.contiguousp())
    //	{
    //		instride = 1;
    //	}
    //	if (out->x.order() == 1)
    //	{
    //		outstride = out->x.mod(0);
    //		out->resize(w->x.dim(0));
    //	}
    //	else if (out->x.contiguousp())
    //	{
    //		outstride = 1;
    //	}
    //	if ( (instride == 0)||(outstride==0))
    //	{
    //		ylerror("linear_module_dim0::fprop: state must 1D or contiguous");
    //	}
    //	else
    //	{
    //		if (out->x.nelements() != w->x.dim(0))
    //		{
    //			ylerror("linear_module_dim0::fprop: output has wrong size");
    //		}
    //		else
    //		{
    //			idx_m2dotm1(w->x, in->x, out->x);
    //		}
    //	}

    // TODO: resize out?

    // check that input and output have at most 4 dimensions
    if ((in->x.order() > 4) || (out->x.order() > 4))
      ylerror("linear_module_dim0: currently only 4-order idx are supported");
    // see input and output as idx of order 4
    Idx<double> inx = in->x.view_as_order(4);
    Idx<double> outx = out->x.view_as_order(4);
    // loop over last 3 dimensions and call linear combination on first dim
    { idx_eloop2(linx,inx,double, loutx,outx,double) {
	idx_eloop2(llinx,linx,double, lloutx,loutx,double) {
	  idx_eloop2(lllinx,llinx,double, llloutx,lloutx,double) {
	    // multiply weight matrix by input
	    idx_m2dotm1(w->x, lllinx, llloutx);
	  }
	}
      }}
  }

  void linear_module_dim0::bprop(state_idx *in, state_idx *out)
  {
    state_idx in1(in); // TODO: temporary, find a cleaner solution
    intg instride = 0;
    intg outstride = 0;
    if (in1.x.order() == 1)
      {
	instride = in1.x.mod(0);
      }
    else if (in1.x.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("linear_module_dim0::fprop: state must 1D or contiguous");
      }
    else
      {
	if (out->x.nelements() != w->x.dim(0))
	  {
	    ylerror("linear_module_dim0::fprop: output has wrong size");
	  }
	else
	  {
	    // see input and output as idx of order 4
	    Idx<double> inx = in->x.view_as_order(4);
	    Idx<double> outx = out->x.view_as_order(4);
	    Idx<double> indx = in->dx.view_as_order(4);
	    Idx<double> outdx = out->dx.view_as_order(4);
	    Idx<double> twx(w->x.transpose(0, 1));
	    // loop over last 3 dimensions and bprop on first dim
	    { idx_eloop4(linx,inx,double, lindx,indx,double, 
			 loutx,outx,double, loutdx,outdx,double) {
		idx_eloop4(llinx,linx,double, llindx,lindx,double,
			   lloutx,loutx,double, lloutdx,loutdx,double) {
		  idx_eloop4(lllinx,llinx,double, lllindx,llindx,double,
			     llloutx,lloutx,double, llloutdx,lloutdx,double) {
		    idx_m1extm1(llloutdx, lllinx, w->dx);
		    idx_m2dotm1(twx, llloutdx, lllindx);
		  }
		}
	      }}
	  }
      }
  }

  void linear_module_dim0::bbprop(state_idx *in, state_idx *out)
  {
    state_idx in1(in); // TODO: temporary, find a cleaner solution
    intg instride = 0;
    intg outstride = 0;
    if (in1.x.order() == 1)
      {
	instride = in1.x.mod(0);
      }
    else if (in1.x.contiguousp())
      {
	instride = 1;
      }
    if (out->x.order() == 1)
      {
	outstride = out->x.mod(0);
      }
    else if (out->x.contiguousp())
      {
	outstride = 1;
      }
    if ( (instride == 0)||(outstride==0))
      {
	ylerror("linear_module_dim0::fprop: state must 1D or contiguous");
      }
    else
      {
	if (out->x.nelements() != w->x.dim(0))
	  {
	    ylerror("linear_module_dim0::fprop: output has wrong size");
	  }
	else
	  {

	    // see input and output as idx of order 4
	    Idx<double> inx = in->x.view_as_order(4);
	    Idx<double> inddx = in->ddx.view_as_order(4);
	    Idx<double> outddx = out->ddx.view_as_order(4);
	    Idx<double> twx(w->x.transpose(0, 1));
	    // loop over last 3 dimensions and bprop on first dim
	    { idx_eloop3(linx,inx,double, linddx,inddx,double, 
			 loutddx,outddx,double) {
		idx_eloop3(llinx,linx,double, llinddx,linddx,double,
			   lloutddx,loutddx,double) {
		  idx_eloop3(lllinx,llinx,double, lllinddx,llinddx,double,
			     llloutddx,lloutddx,double) {
		    idx_m1squextm1(llloutddx, lllinx, w->ddx);
		    idx_m2squdotm1(twx, llloutddx, lllinddx);
		  }
		}
	      }}

	  }
      }
  }

  ////////////////////////////////////////////////////////////////
  // addc_module

  addc_module::addc_module(parameter *p, intg size)
  {
    bias = new state_idx(p,size);
  }

  addc_module::~addc_module()
  {
    delete bias;
  }

  void addc_module::fprop(state_idx* in, state_idx* out)
  {
    out->resize1(0, bias->x.dim(0));
    idx_bloop3(inx, in->x, double, biasx, bias->x, double, outx, out->x, double)
      {
	idx_addc(inx, biasx.get(), outx);
      }
  }

  void addc_module::bprop(state_idx* in, state_idx* out)
  {
    idx_bloop3(indx, in->dx, double, biasdx, bias->dx, double, 
	       outdx, out->dx, double)
      {
	idx_copy(outdx, indx); // TODO: needed?
	idx_sumacc(outdx, biasdx);
      }
  }

  void addc_module::bbprop(state_idx* in, state_idx* out)
  {
    idx_bloop3(inddx, in->ddx, double, biasddx, bias->ddx, double, 
	       outddx, out->ddx, double)
      {
	idx_copy(outddx, inddx); // TODO: needed?
	idx_sumacc(outddx, biasddx);
      }
  }

  void addc_module::forget(forget_param_linear& fp)
  {
    idx_clear(bias->x);
  }

  void addc_module::normalize()
  {
  }

} // end namespace ebl
