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

  //! The following functions are really useful to estimate the relations
  //! between a module's input size and its output.
  // TODO : move these functions to a better place ?

  //! This method modifies i_size to be compliant with the architecture of 
  //! the module. It also returns the output size corresponding to the new
  //! input...
  template<class T>
  idxdim layers_n<T>::adapt_input_size(idxdim &i_size) {
    vector<module_1_1<state_idx, state_idx>*>::iterator iter;
    idxdim o_size(i_size.dim[0], i_size.dim[1]);
    idxdim kernel_size;
    intg stridei, stridej;

    //! Loop through all the layers of the module.
    //! For each convolution / subsampling layer, adapt the size accordingly
    for (iter = modules->begin(); iter != modules->end(); iter++) {
      string moduleType = typeid(**iter).name();
      if (moduleType.find("convolution") != string::npos) {
	// Select one kernel
	kernel_size = ( ((nn_layer_convolution*)(*iter))->
			convol.kernel.x[0].getidxdim(kernel_size) );
	// Extract its dimensions, update output size
	o_size.setdim(0, o_size.dim[0] - kernel_size.dim[0] + 1);
	o_size.setdim(1, o_size.dim[1] - kernel_size.dim[1] + 1);
      }
      if (moduleType.find("subsampling") != string::npos) {
	// Get dimensions of susampler
	stridei = ( ((nn_layer_subsampling*)(*iter))->
		    subsampler.stridei );
	stridej = ( ((nn_layer_subsampling*)(*iter))->
		    subsampler.stridej );
	// Update output size
	o_size.setdim(0, o_size.dim[0] / stridei);
	o_size.setdim(1, o_size.dim[1] / stridej);
      }
    }
    //! Recompute the input size to be compliant with the output
    i_size = get_input_size_from_output(o_size);
    return o_size;
  }

  //! This method computes the input size of the module for a given output
  //! size.
  template<class T>
  idxdim layers_n<T>::get_input_size_from_output(idxdim o_size) {
    vector<module_1_1<state_idx, state_idx>*>::reverse_iterator iter;
    idxdim i_size(o_size.dim[0], o_size.dim[1]);
    idxdim kernel_size;
    intg stridei, stridej;

    //! Loop through all the layers of the module, from the end to the beg.
    //! For each convolution / subsampling layer, adapt the size accordingly
    for (iter = modules->rbegin(); iter != modules->rend(); iter++) {
      string moduleType = typeid(**iter).name();
      if (moduleType.find("convolution") != string::npos) {
	// Select one kernel
	kernel_size = ( ((nn_layer_convolution*)(*iter))->
			convol.kernel.x[0].getidxdim(kernel_size) );
	// Extract its dimensions, update output size
	i_size.setdim(0, i_size.dim[0] + kernel_size.dim[0] - 1);
	i_size.setdim(1, i_size.dim[1] + kernel_size.dim[1] - 1);
      }
      if (moduleType.find("subsampling") != string::npos) {
	// Get dimensions of susampler
	stridei = ( ((nn_layer_subsampling*)(*iter))->
		    subsampler.stridei );
	stridej = ( ((nn_layer_subsampling*)(*iter))->
		    subsampler.stridej );
	// Update output size
	i_size.setdim(0, i_size.dim[0] * stridei);
	i_size.setdim(1, i_size.dim[1] * stridej);
      }
    }
    return i_size;
  }

  ////////////////////////////////////////////////////////////////////////

} // end namespace ebl
