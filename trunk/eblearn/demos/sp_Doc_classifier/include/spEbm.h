/***************************************************************************
 *   Copyright (C) 2008 by Cyril Poulet   *
 *   cyril.poulet@centraliens.net   *
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

#ifndef SPEBM_H_
#define SPEBM_H_

#include "libeblearn.h"
#include "libspidx.h"

namespace ebl {

//! class that stores a vector/tensor state
class state_spidx: public state_idx {
public:


	//! state itself
	spIdx<double> x;
	//! gradient of loss with respect to state
	spIdx<double> dx;
	//! diag hessian of loss with respect to state
	spIdx<double> ddx;


	//! Constructs a state_idx of order 0
	state_spidx();

	//! Constructor. A state_idx can have up to 8 dimensions.
	state_spidx(intg s0, intg s1 = -1, intg s2 = -1, intg s3 = -1, intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 = -1);

	//! this appends the state_idx into the same Srg as the
	//! state_idx passed as argument. This is useful for
	//! allocating multiple state_idx inside a parameter.
	//! This replaces the Lush function alloc_state_idx.
	state_spidx(parameter *st, intg Nelem, intg s0 = -1, intg s1 = -1, intg s2 = -1, intg s3 = -1, intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 = -1);

	virtual ~state_spidx();

	//! clear x
	void clear();

	//! clear gradients dx
	void clear_dx();

	//! clear diag hessians ddx
	void clear_ddx();

	//! return number of elements
	intg nelements();

	//! return footprint in storages
	intg footprint(){ ylerror("Not implemented for state_spidx"); return 0;}

	//! same as footprint
	intg size(){ ylerror("Not implemented for state_spidx"); return 0;}

	//! update with gradient descent
	void update_gd(gd_param &arg);

	//! resize. The order cannot be changed with this.
	void resize(intg s0 = -1, intg s1 = -1, intg s2 = -1, intg s3 = -1,
			intg s4 = -1, intg s5 = -1, intg s6 = -1, intg s7 = -1);

	void resizeAs(state_spidx &s);

	void resize(const intg* dimsBegin, const intg* dimsEnd){ ylerror("Not implemented for state_spidx");}

	//! make a new copy of self
	state_spidx make_spcopy();
	state_idx make_copy(){ylerror("Make_copy not implemented for state_spidx, use make_spcopy instead"); state_idx bla; return bla;}
};

////////////////////////////////////////////////////////////////
// linear module
// It's different from f_layer in that it is
// not spatially replicable and does not operate
// on 3D state_idx.

class sp_linear_module: public module_1_1<state_idx, state_idx> {
public:
	state_idx *w;
	Idx<intg> *table;

	virtual ~sp_linear_module();
	sp_linear_module(parameter *p, Idx<intg>* connection_table, intg in, intg out);
	void fprop(state_spidx *in, state_spidx *out);
	void bprop(state_spidx *in, state_spidx *out);
	void bbprop(state_spidx *in, state_spidx *out);
	void forget(forget_param_linear &fp);
	void normalize();
};


class sp_logsoftmax_module : public module_1_1< state_idx, state_idx > {
public:
	Idx<ubyte> classindex2label; //! a vector that maps output unit index to a label
	double beta;

	sp_logsoftmax_module(double b, Idx<ubyte> *classes);
	~sp_logsoftmax_module(){}

	void fprop(state_spidx *in, state_spidx *out);
	void bprop(state_spidx *in, state_spidx *out);

	void calc_energy(state_spidx *out, Idx<ubyte> *desired, state_idx *energy);
	void calc_max(state_spidx *out, class_state *output);
};


}
#endif /* SPEBM_H_ */
