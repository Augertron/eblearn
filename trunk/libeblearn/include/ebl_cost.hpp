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

  ////////////////////////////////////////////////////////////////
  // cost_module
  
  template<class T1, class T2>  
  cost_module<T1, T2>::cost_module(idx<T1> &targets_)
    : targets(targets_), in2(targets.select(0, 0)), energies(targets_.dim(0)) {
  }

  template<class T1, class T2>  
  cost_module<T1, T2>::~cost_module() {
  }

  ////////////////////////////////////////////////////////////////
  // euclidean_module

  template <class Tnet, class Tlabel>
  euclidean_module<Tnet, Tlabel>::euclidean_module(idx<Tnet> &targets_)
    : cost_module<Tnet, Tlabel>(targets_) {
  }

  template <class Tnet, class Tlabel>
  euclidean_module<Tnet, Tlabel>::~euclidean_module() {
  }

  template <class Tnet, class Tlabel>
  void euclidean_module<Tnet, Tlabel>::fprop(state_idx<Tnet> &in1,
					     Tlabel &label,
					     state_idx<Tnet> &energy) {
    idx<Tnet> target = targets.select(0, label);
    idx_copy(target, in2.x);
    // squared distance between in1 and target
    idx_sqrdist(in1.x, in2.x, energy.x);
    idx_dotc(energy.x, 0.5, energy.x); // multiply by .5
  }
  
  template <class Tnet, class Tlabel>
  void euclidean_module<Tnet, Tlabel>::bprop(state_idx<Tnet> &in1,
					     Tlabel &label,
					     state_idx<Tnet> &energy) {
    idx_checkorder1(energy.x, 0); // energy.x must have an order of 0
    idx_sub(in1.x, in2.x, in1.dx); // derivative with respect to in1
    idx_dotc(in1.dx, energy.dx.get(), in1.dx); // multiply by energy derivative
    idx_minus(in1.dx, in2.dx); // derivative with respect to in2
  }

  // mse has this funny property that the bbprop method mixes up the
  // the first derivative after with the second derivative before, and
  // vice versa. Only the first combination is used here.
  template <class Tnet, class Tlabel>
  void euclidean_module<Tnet, Tlabel>::bbprop(state_idx<Tnet> &in1,
					      Tlabel &label,
					      state_idx<Tnet> &energy) {
    idx_fill(in1.ddx, energy.dx.get());
  }

  template <class Tnet, class Tlabel>
  double euclidean_module<Tnet, Tlabel>::infer2(state_idx<Tnet> &i1,
						Tlabel &infered_label, 
						infer_param &ip, 
						Tlabel *label,
						state_idx<Tnet> *energy) {
    infered_label = 0;
    state_idx<Tnet> tmp;
    idx_bloop1(e, energies, Tnet) {
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

  template <class T>
  logadd_layer<T>::logadd_layer(intg thick, intg si, intg sj) {
    expdist = idx<T>(thick, si, sj);
    sumexp = idx<T>(thick);		// scaled partition function
  }

  template <class T>
  void logadd_layer<T>::fprop(state_idx<T> *in, state_idx<T> *out) {
    intg thick = in->x.dim(0);
    intg si = in->x.dim(1);
    intg sj = in->x.dim(2);
    expdist.resize(thick, si, sj);
    out->x.resize(thick);
    if (1 == (si * sj)) {
      // save time and precision if no replication
      idx<T> inx(in->x.select(2, 0));
      idx<T> m(inx.select(1, 0));
      idx<T> ed(expdist.select(2, 0));
      idx<T> ed1(ed.select(1, 0));
      idx_fill(ed1, 1.0);
      idx_fill(sumexp, 1.0);
      idx_copy(m, out->x);
    }	else {
      // spatially replicated
      // loop over output units
      { idx_bloop4(m, in->x, T, outx, out->x, T,
		   ed, expdist, T, sx, sumexp, T) {
	  // first compute smallest element of m
	  T mini = m.get(0, 0);
	  { idx_bloop1(m1, m, T) {
	      { idx_bloop1(m0, m1, T) {
		  if (m0.get() < mini)
		    mini = m0.get();
		}
	      }
	    }
	  }
	  // now do log-add, and save exponentials
	  T r = 0.0;
	  T w = 1 / (si * sj);
	  { idx_bloop2(m1, m, T, ed1, ed, T) {
	      { idx_bloop2(m0, m1, T, ed0, ed1, T) {
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

  template <class T>
  void logadd_layer<T>::bprop(state_idx<T> *in, state_idx<T> *out) {
    intg si = in->dx.dim(1);
    intg sj = in->dx.dim(2);
    if ((si * sj) == 1) {
      // save time and precision if no replication
      idx<T> indx(in->dx.select(2, 0));
      idx<T> m(indx.select(1, 0));
      idx_copy(out->dx, m);
    } else {
      // spatially replicated
      // loop over output units
      { idx_bloop4(m, in->dx, T, o, out->dx, T,
		   ed, expdist, T, sx, sumexp, T) {
	  { idx_bloop2(m1, m, T, ed1, ed, T) {
	      { idx_bloop2(m0, m1, T, ed0, ed1, T) {
		  m0.set(ed0.get() * o.get() / sx.get());
		}
	      }
	    }
	  }
	}
      }
    }
  }

  template <class T>
  void logadd_layer<T>::bbprop(state_idx<T> *in, state_idx<T> *out) {
    { idx_bloop2(o, out->ddx, T, i, in->ddx, T) {
  	idx_fill(i, o.get());
      }
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // distance_l2
  
  template <class T>
  distance_l2<T>::distance_l2() { 
  }
  
  template <class T>
  distance_l2<T>::~distance_l2() { 
  }
  
  template <class T>
  void distance_l2<T>::fprop(state_idx<T> &in1, state_idx<T> &in2,
			     state_idx<T> &energy) { 
    // squared distance between in1 and target
    idx_sqrdist(in1.x, in2.x, energy.x);
    idx_dotc(energy.x, 0.5, energy.x); // multiply by .5
  }
  
  template <class T>
  void distance_l2<T>::bprop(state_idx<T> &in1, state_idx<T> &in2,
			     state_idx<T> &energy) {
    idx_checkorder1(energy.x, 0); // energy.x must have an order of 0
    idxdim d(in1.x);
    if (!tmp.same_dim(d)) { // keep tmp buffer around to avoid allocations
      idx<T> tmp2(d);
      tmp = tmp2;
    }
    idx_sub(in1.x, in2.x, tmp);
    idx_dotc(tmp, energy.dx.get(), tmp); // multiply by energy derivative
    idx_add(in1.dx, tmp, in1.dx); // derivative with respect to in1
    idx_sub(in2.dx, tmp, in2.dx); // derivative with respect to in2
  }
  
  template <class T>
  void distance_l2<T>::bbprop(state_idx<T> &in1, state_idx<T> &in2,
			      state_idx<T> &energy) { 
    idx_addc(in1.ddx, energy.dx.get(), in1.ddx);
    idx_addc(in2.ddx, energy.dx.get(), in2.ddx);
  }
  
  template <class T>
  void distance_l2<T>::forget(forget_param_linear &fp) { 
    err_not_implemented(); }

  template <class T>
  void distance_l2<T>::infer2_copy(state_idx<T> &in1, state_idx<T> &in2,
				   state_idx<T> &energy) {
    idx_copy(in1.x, in2.x);
    idx_clear(energy.x);
  }
    
  ////////////////////////////////////////////////////////////////
  // penalty_l1

  template <class T>
  penalty_l1<T>::penalty_l1(T threshold_)
    : threshold(threshold_) { 
  }
  
  template <class T>
  penalty_l1<T>::~penalty_l1() { 
  }

  template <class T>
  void penalty_l1<T>::fprop(state_idx<T> &in, state_idx<T> &energy) { 
    idx_sumabs(in.x, energy.x);
  }
  
  template <class T>
  void penalty_l1<T>::bprop(state_idx<T> &in, state_idx<T> &energy) { 
    idx_thresdotc_acc(in.x, energy.dx.get(), threshold, in.dx);
  }
  
  template <class T>
  void penalty_l1<T>::bbprop(state_idx<T> &in, state_idx<T> &energy) { 
    idx_addc(in.ddx, energy.dx.get(), in.ddx);
  }
  
  template <class T>
  void penalty_l1<T>::forget(forget_param_linear &fp) { 
    err_not_implemented(); }
  
} // end namespace ebl
