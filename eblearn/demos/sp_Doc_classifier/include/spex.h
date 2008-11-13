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

#ifndef SPEX_H_
#define SPEX_H_

#include "gzstream.h"
#include "libspidx.h"
#include "libeblearn.h"
#include "spDataSource.h"
#include "spEbm.h"

void load(const char *fname, spIdx<double> &xp, Idx<ubyte> &yp);

class spnet {

protected :
	state_spidx *inter;

public :
	sp_linear_module mylinmodule;
	sp_logsoftmax_module mysoftmodule;

	spnet(parameter *p, Idx<intg>* connection_table, intg in, intg out, double beta, Idx<ubyte> *classes);
	~spnet();

	void fprop(state_spidx *in, state_spidx *out, Idx<ubyte> *label, class_state *output, state_idx *energy);
	void bprop(state_spidx *in, state_spidx *out);
	void forget(forget_param_linear forgetparam);

};

class sptrainer{

public:
	int nclasses;
	intg age;
	Idx<ubyte> *labels, *label, doclabels;
	spIdx<double> docs;
	Idx<intg> *myconnections;

	spLabeledDataSource<double, ubyte> *mydatasource;
	parameter *myparam;
	gd_param *gdp;
	spnet *mynet;
	classifier_meter* trainmeter;

	state_spidx *in, *out;
	class_state *output;
	state_idx *energy;

public:
	sptrainer(const string fname);
	~sptrainer();

	void train(int npass);
	void train_online();
	void test();
};
#endif /* SPEX_H_ */
