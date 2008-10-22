/***************************************************************************
 *   Copyright (C) 2008 by Matt Grimes and Pierre Sermanet   *
 *   mkg@cs.nyu.edu, pierre.sermanet@gmail.com   *
 *   All rights reserved.
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

#ifndef DATASOURCE_HPP_
#define DATASOURCE_HPP_

#include "libidx.h"

namespace ebl {

template<typename T, typename L>
LabeledDataSource<T,L>::LabeledDataSource(Idx<T> *data, Idx<L> *labels)
	//:data(data),
	// labels(labels),
	:dataIter(*data, 0),
    labelsIter(*labels,0)
{
	this->data = data;
	this->labels = labels;
}

template<typename T, typename L>
int LabeledDataSource<T,L>::size() {
	return this->data->dim(0);
}

template<typename T, typename L>
void LabeledDataSource<T,L>::fprop(state_idx *state, Idx<L> *label) {
	//state->setndim(data.order()-1)
	//state.resize(data.dims()+1, data.dims()+data.order());
	state->resize(1, data->dim(1), data->dim(2)); // TODO: make this generic
	//label->resize();
	idx_copy(*dataIter, state->x);
	idx_copy(*labelsIter, *label);
}

template<typename T, typename L>
void LabeledDataSource<T,L>::next() {
	++dataIter;
	++labelsIter;

	if(!dataIter.notdone()) {
		dataIter = data->dim_begin(0);
		labelsIter = labels->dim_begin(0);
	}
}

template<typename T, typename L>
void LabeledDataSource<T,L>::seek_begin() {
  dataIter = data->dim_begin(0);
	labelsIter = labels->dim_begin(0);
}

////////////////////////////////////////////////////////////////

template<class I, class L>
MnistDataSource<I, L>::MnistDataSource(Idx<I> *inp, Idx<L> *lbl,
		intg w, intg h, double b, double c)
	: LabeledDataSource<I, L>(inp, lbl) {
  bias = b;
  coeff = c;
  width = w;
  height = h;
}

template<class I, class L>
void MnistDataSource<I, L>::fprop(state_idx *out, Idx<L> *label) {
  out->resize(1, height, width);
  intg ni = (this->data)->dim(1);
  intg nj = (this->data)->dim(2);
  intg di = 0.5 * (height - ni);
  intg dj = 0.5 * (width - nj);
  idx_fill(out->x, bias * coeff);
  Idx<double> tgt = (out->x).select(0, 0);
  tgt = tgt.narrow(0, ni, di);
  tgt = tgt.narrow(1, nj, dj);
  idx_copy(*(this->dataIter), tgt);
  idx_addc(out->x, bias, out->x);
  idx_dotc(out->x, coeff, out->x);
  label->set(this->labelsIter->get());
}

////////////////////////////////////////////////////////////////
/*
// TODO: implement seek
template<class I, class L>
DataSourceNarrow<I, L>::DataSourceNarrow(LabeledDataSource<I, L> *b,
		intg siz, intg off) {
  if ((siz + off) > b->size())
  	ylerror("illegal range for narrow-db");
  if ((off < 0) || (siz < 0))
    ylerror("offset and size of narrow-db must be positive");
  base = b;
  offset = off;
  size = siz;
  current = 0;
}

intg DataSourceNarrow<I, L>::size() {
	return size;
}

void fprop(state_idx &out, Idx<L> &label) {
  base->seek(offset + current);
  base->fprop(out,label);
}
*/

} // end namespace ebl

#endif /*DATASOURCE_HPP_*/
