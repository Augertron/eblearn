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

#include "EblCost.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // euclidean_module
  
  euclidean_module::euclidean_module(Idx<double> &targets_)
    : cost_module<state_idx,int>(targets_) {
  }

  euclidean_module::~euclidean_module() {
  }

  void euclidean_module::fprop(state_idx &in1, int &label, state_idx &energy) {
    Idx<double> target = targets.select(0, label);
    idx_copy(target, in2.x);
    // squared distance between in1 and target
    idx_sqrdist(in1.x, in2.x, energy.x);
    idx_dotc(energy.x, 0.5, energy.x); // multiply by .5
  }
  
  void euclidean_module::bprop(state_idx &in1, int &label, state_idx &energy) {
    idx_checkorder1(energy.x, 0); // energy.x must have an order of 0
    idx_sub(in1.x, in2.x, in1.dx); // derivative with respect to in1
    idx_dotc(in1.dx, energy.dx.get(), in1.dx); // multiply by energy derivative
    idx_minus(in1.dx, in2.dx); // derivative with respect to in2
  }

  // mse has this funny property that the bbprop method mixes up the
  // the first derivative after with the second derivative before, and
  // vice versa. Only the first combination is used here.
  void euclidean_module::bbprop(state_idx &in1, int &label, state_idx &energy) {
    idx_fill(in1.ddx, energy.dx.get());
  }

  double euclidean_module::infer2(state_idx &i1, int &infered_label, 
				  infer_param &ip, 
				  int *label, state_idx *energy) {
    infered_label = 0;
    state_idx tmp;
    idx_bloop1(e, energies, double) {
      fprop(i1, infered_label, tmp);
      idx_copy(tmp.x, e);
      infered_label++;
    }
    // TODO: use logadd_layer like in gblearn2 on energies?
    if (label && energy) // if groundtruth is passed, fill in its energy
      energy->x.set(energies.get(*label)); 
    infered_label = idx_indexmin(energies);
    return 0.0;
  }

  ////////////////////////////////////////////////////////////////////////
  // logadd_layer

  logadd_layer::logadd_layer(intg thick, intg si, intg sj) {
    expdist = Idx<double>(thick, si, sj);
    sumexp = Idx<double>(thick);		// scaled partition function
  }

  void logadd_layer::fprop(state_idx *in, state_idx *out) {
    intg thick = in->x.dim(0);
    intg si = in->x.dim(1);
    intg sj = in->x.dim(2);
    expdist.resize(thick, si, sj);
    out->x.resize(thick);
    if (1 == (si * sj)) {
      // save time and precision if no replication
      Idx<double> inx(in->x.select(2, 0));
      Idx<double> m(inx.select(1, 0));
      Idx<double> ed(expdist.select(2, 0));
      Idx<double> ed1(ed.select(1, 0));
      idx_fill(ed1, 1.0);
      idx_fill(sumexp, 1.0);
      idx_copy(m, out->x);
    }	else {
      // spatially replicated
      // loop over output units
      { idx_bloop4(m, in->x, double, outx, out->x, double,
		   ed, expdist, double, sx, sumexp, double) {
	  // first compute smallest element of m
	  double mini = m.get(0, 0);
	  { idx_bloop1(m1, m, double) {
	      { idx_bloop1(m0, m1, double) {
		  if (m0.get() < mini)
		    mini = m0.get();
		}
	      }
	    }
	  }
	  // now do log-add, and save exponentials
	  double r = 0.0;
	  double w = 1 / (si * sj);
	  { idx_bloop2(m1, m, double, ed1, ed, double) {
	      { idx_bloop2(m0, m1, double, ed0, ed1, double) {
		  ed0.set(w * exp(mini - m0.get()));
		  r += ed0.get();
		}
	      }
	    }
	  }
	  sx.set(r);
	  // put result in output
	  outx.set(mini - log(r));
	}
      }
    }
  }

  void logadd_layer::bprop(state_idx *in, state_idx *out) {
    intg si = in->dx.dim(1);
    intg sj = in->dx.dim(2);
    if ((si * sj) == 1) {
      // save time and precision if no replication
      Idx<double> indx(in->dx.select(2, 0));
      Idx<double> m(indx.select(1, 0));
      idx_copy(out->dx, m);
    } else {
      // spatially replicated
      // loop over output units
      { idx_bloop4(m, in->dx, double, o, out->dx, double,
		   ed, expdist, double, sx, sumexp, double) {
	  { idx_bloop2(m1, m, double, ed1, ed, double) {
	      { idx_bloop2(m0, m1, double, ed0, ed1, double) {
		  m0.set(ed0.get() * o.get() / sx.get());
		}
	      }
	    }
	  }
	}
      }
    }
  }

  void logadd_layer::bbprop(state_idx *in, state_idx *out) {
    { idx_bloop2(o, out->ddx, double, i, in->ddx, double) {
  	idx_fill(i, o.get());
      }
    }
  }

} // end namespace ebl
