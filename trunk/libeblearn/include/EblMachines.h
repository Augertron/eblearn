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

#ifndef EBLMACHINES_H_
#define EBLMACHINES_H_

#include "Defines.h"
#include "Gbl.h"
#include "Idx.h"
#include "Blas.h"
#include "EblStates.h"
#include "EblBasic.h"
#include "EblArch.h"
#include "EblNonLinearity.h"
#include "EblLayers.h"

namespace ebl {

  ////////////////////////////////////////////////////////////////
  //! Standard LeNet5-type architecture without the final e-dist RBF layer.
  class nn_machine_cscscf : public layers_n<state_idx> {
  public:
    //! Empty constructor, awaiting for initialization by the user via the 
    //! init() function.
    nn_machine_cscscf();
    //! Complete constructor, calls the init() function.
    //! See the init() description for complete arguments description.
    nn_machine_cscscf(parameter *prm, intg ini, intg inj, intg ki0, intg kj0, 
		      Idx<intg> *tbl0, intg si0, intg sj0, intg ki1, intg kj1, 
		      Idx<intg> *tbl1, intg si1, intg sj1, intg ki2, intg kj2, 
		      Idx<intg> *tbl2, intg outthick);
    virtual ~nn_machine_cscscf();

    //! The init function creates the machine by stacking the modules in this
    //! order (c-s-c-s-c-f): nn_layer_convolution, nn_layer_subsampling, 
    //! nn_layer_convolution, nn_layer_subsampling, nn_layer_convolution, 
    //! nn_layer_full.
    //! <ini> <inj>: expected max size of input for preallocation of internal 
    //! states
    //! <ki0> <kj0>: kernel size for first convolutional layer
    //! <tbl0>: table of connections between input anf feature maps for first 
    //! layer
    //! <si0> <sj0>: subsampling for first layer
    //! <ki1> <kj1> <tbl1> <si1> <sj1>: same for next 2 layers
    //! <ki2> <kj2> <tbl2>: same for last convolution layer
    //! <outthick>: number of outputs.
    //! <prm> an idx1-ddparam in which the parameters will be allocated.
    void init(parameter *prm, intg ini, intg inj, intg ki0, intg kj0, 
	      Idx<intg> *tbl0, intg si0, intg sj0, intg ki1, intg kj1, 
	      Idx<intg> *tbl1, intg si1, intg sj1, intg ki2, intg kj2, 
	      Idx<intg> *tbl2, intg outthick);
  };

  ////////////////////////////////////////////////////////////////
  //! create a new instance of net-cscscf implementing a LeNet-5 type
  //! convolutional neural net. This network has regular sigmoid 
  //! units on the output, not an extra RBF layer as described 
  //! in the Proc. IEEE paper. The network has 6 feature
  //! maps at the first layer and 16 feature maps at the second layer
  //! with a connection matrix between feature maps as described in 
  //! the paper.
  //! Arguments:
  //! {<pre>
  //!  <image-height> <image-width>: height and width of input image
  //!  <ki0> <kj0>: height and with of convolutional kernel, first layer.
  //!  <si0> <sj0>: subsampling ratio of subsampling layer, second layer.
  //!  <ki1> <kj1>: height and with of convolutional kernel, third layer.
  //!  <si1> <sj1>: subsampling ratio of subsampling layer, fourth layer.
  //!  <hid>: number of hidden units, fifth layer
  //!  <output-size>: number of output units
  //!  <net-param>: idx1-ddparam that will hold the trainable parameters
  //!               of the network
  //! </pre>}
  //! example
  //! {<code>
  //!  (setq p (new idx1-ddparam 0 0.1 0.02 0.02 80000))
  //!  (setq z (new-lenet5 32 32 5 5 2 2 5 5 2 2 120 10 p))
  //! </code>}
  class lenet5 : public nn_machine_cscscf {
  public:
    Idx<intg> table0;	
    Idx<intg> table1;	
    Idx<intg> table2;	
		
    lenet5(parameter *net_param, intg image_height, intg image_width,
	   intg ki0, intg kj0, intg si0, intg sj0,
	   intg ki1, intg kj1, intg si1, intg sj1,
	   intg hid, intg output_size);
    virtual ~lenet5() {}
  };

  ////////////////////////////////////////////////////////////////
  //! Lenet7, similar to lenet5 with different neural connections.
  class lenet7 : public nn_machine_cscscf {
  public:
    Idx<intg> table0;	
    Idx<intg> table1;	
    Idx<intg> table2;	
		
    lenet7(parameter *net_param, intg image_height, intg image_width);
    virtual ~lenet7() {}
  };

  ////////////////////////////////////////////////////////////////

  //! a module that takes an idx3 as input, runs it through
  //! a machine, and runs the output of the machine through
  //! a cost function whose second output is the desired label
  //! stored in an idx0 of int.
  class idx3_supervised_module {
  public:
    nn_machine_cscscf	*machine;
    state_idx		*mout;
    edist_cost 		*cost;
    max_classer 	*classifier;

    idx3_supervised_module(nn_machine_cscscf *m, edist_cost *c, 
			   max_classer *cl);
    virtual ~idx3_supervised_module();

    void fprop(state_idx *input, class_state *output, 
	       Idx<ubyte> *desired, state_idx *energy);
    void use(state_idx *input, class_state *output);
    void bprop(state_idx *input, class_state *output, 
	       Idx<ubyte> *desired, state_idx *energy);
    void bbprop(state_idx *input, class_state *output, 
  		Idx<ubyte> *desired, state_idx *energy);
  };

  ////////////////////////////////////////////////////////////////

} // namespace ebl {

#endif /* EBLMACHINES_H_ */
