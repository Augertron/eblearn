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

#include "EblLayers.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // nn_layer_full

  nn_layer_full::nn_layer_full(parameter *p, intg indim0, intg noutputs)
  {
    linear = new linear_module_replicable(p, indim0, noutputs);
    adder = new addc_module(p, noutputs);
    // the order of sum is not yet known and this is just an internal buffer
    // that does not need to be save in the parameter, so we allocate it later
    sum = NULL; 
    sigmoid = new tanh_module();
  }

  nn_layer_full::~nn_layer_full()
  {
    delete sigmoid;
    if (sum) delete sum;
    delete adder;
    delete linear;
  }

  void nn_layer_full::fprop(state_idx *in, state_idx *out)
  {
    // resize output and sum
    IdxDim d(in->x.spec); // use same dimensions as in
    d.setdim(0, adder->bias->x.dim(0)); // except for the first one
    out->resize(d);
    if (!sum) sum = new state_idx(d); // we now know the order of sum
    else sum->resize(d);

    // fprop
    linear->fprop(in, sum);
    adder->fprop(sum, sum);
    sigmoid->fprop(sum, out);
  }

  void nn_layer_full::bprop(state_idx *in, state_idx *out)
  {
    sigmoid->bprop(sum, out);
    adder->bprop(sum, sum);
    linear->bprop(in, sum);
  }

  void nn_layer_full::bbprop(state_idx *in, state_idx *out)
  {
    sigmoid->bbprop(sum, out);
    adder->bbprop(sum, sum);
    linear->bbprop(in, sum);
  }

  void nn_layer_full::forget(forget_param_linear &fp)
  {
    linear->forget(fp);
    adder->forget(fp);
  }

  ////////////////////////////////////////////////////////////////
  // nn_layer_convolution

  nn_layer_convolution::nn_layer_convolution(parameter *p, 
					     intg kerneli, intg kernelj, 
					     intg ri, intg rj, 
					     Idx<intg> *tbl, intg thick)
  {
    convol = new convolution_module_2D_replicable(p, kerneli, kernelj, ri, rj, 
						  tbl, thick);
    adder = new addc_module(p, thick);
    sum = NULL;
    sigmoid = new tanh_module();
  }

  nn_layer_convolution::~nn_layer_convolution()
  {
    delete convol;
    delete sigmoid;
    if (sum) delete sum;
    delete adder;
  }

  void nn_layer_convolution::fprop(state_idx *in, state_idx *out)
  {
    // 1. resize sum
    IdxDim d(in->x.spec); // use same dimensions as in
    d.setdim(0, convol->thickness); // except for the first one
    if (!sum) sum = new state_idx(d);
    else sum->resize(d);

    // 2. fprop
    convol->fprop(in, sum);
    out->resize_as(*sum); // resize output
    adder->fprop(sum, sum);
    sigmoid->fprop(sum, out);
  }

  void nn_layer_convolution::bprop(state_idx *in, state_idx *out)
  {
    sigmoid->bprop(sum, out);
    adder->bprop(sum, sum);
    convol->bprop(in, sum);
  }

  void nn_layer_convolution::bbprop(state_idx *in, state_idx *out)
  {
    sigmoid->bbprop(sum, out);
    adder->bbprop(sum, sum);
    convol->bbprop(in, sum);
  }

  void nn_layer_convolution::forget(forget_param_linear &fp)
  {
    convol->forget(fp);
    adder->forget(fp);
  }

  ////////////////////////////////////////////////////////////////
  // nn_layer_subsampling

  nn_layer_subsampling::nn_layer_subsampling(parameter *p,
					     intg stridei, intg stridej,
					     intg subi, intg subj, 
					     intg thick)
  {
    subsampler = new subsampling_module_2D_replicable(p, stridei, stridej, 
						      subi, subj, thick);
    adder = new addc_module(p, thick);
    sum = NULL;
    sigmoid = new tanh_module();
  }

  nn_layer_subsampling::~nn_layer_subsampling()
  {
    delete subsampler;
    delete sigmoid;
    if (sum) delete sum;
    delete adder;
  }

  void nn_layer_subsampling::fprop(state_idx *in, state_idx *out)
  {
    // 1. resize sum
    IdxDim d(in->x.spec); // use same dimensions as in
    d.setdim(0, subsampler->thickness); // except for the first one
    if (!sum) sum = new state_idx(d);
    else sum->resize(d);

    // 2. fprop
    subsampler->fprop(in, sum);
    out->resize_as(*sum); // resize output
    adder->fprop(sum, sum);
    sigmoid->fprop(sum, out);
  }

  void nn_layer_subsampling::bprop(state_idx *in, state_idx *out)
  {
    sigmoid->bprop(sum, out);
    adder->bprop(sum, sum);
    subsampler->bprop(in, sum);
  }

  void nn_layer_subsampling::bbprop(state_idx *in, state_idx *out)
  {
    sigmoid->bbprop(sum, out);
    adder->bbprop(sum, sum);
    subsampler->bbprop(in, sum);
  }

  void nn_layer_subsampling::forget(forget_param_linear &fp)
  {
    subsampler->forget(fp);
    adder->forget(fp);
  }

} // end namespace ebl
