/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet  *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com   *
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

#ifndef TRAINER_HPP_
#define TRAINER_HPP_

namespace ebl {

  template <class T, class L> 
  void supervised::train(LabeledDataSource<T, L> *ds, classifier_meter *mtr) {
    ds->seek_begin();
    mtr->clear();
  }

  template <class T, class L> 
  void supervised::test(LabeledDataSource<T, L> *ds, classifier_meter *mtr) {
    ds->seek_begin();
    mtr->clear();
    for (int i = 0; i < ds->size(); ++i) {
      ds->fprop(*input, *desired);
      machine->fprop(input, output, desired, energy);
      mtr->update(age, output, desired->get(), energy);
      ds->next();
    }
  }

  template <class T, class L> 
  void supervised::test_sample(LabeledDataSource<T, L> *ds, 
			       classifier_meter *mtr, intg i) {
    /*  ds->seek(i);
	ds->fprop(input, desired);
	machine->fprop(input, output, desired, energy);
	mtr->test(output, desired, energy);
    */	err_not_implemented();
  }

////////////////////////////////////////////////////////////////////////////////

  template <class T, class L>  
  void supervised_gradient::train_online(LabeledDataSource<T, L> *ds, 
					 classifier_meter *mtr,
					 intg n, gd_param *gdp, 
					 double kappa = 0.0) {
    for (int i = 0; i < n; ++i) {
      ds->fprop(*input, *desired);
      machine->fprop(input, output, desired, energy);
      //   mtr->update(age, output, desired->get(), energy);
      param->clear_dx();
      machine->bprop(input, output, desired, energy);
      param->update_gd(*gdp);
      if (kappa != 0.0) {
    	err_not_implemented();
    	/*
	  if (kappa > 0.0)
	  param->update_xaverage(kappa);
	  else if (kappa < 0.0)
	  param->update_xaverage((-kappa) / (1 + age));
    	*/
      }
      age++;
      // info
      /*    cout << "age: " << age << " desired: " << (unsigned int) desired->get();
	    cout << " output_class: " << (unsigned int) output->output_class << " machine->mout->x ";
	    machine->mout->x.printElems();
	    cout << endl;
      */    // (print (==> ds tell) (desired) :machine:mout:x)
      ds->next();
    }
    // mtr->info(); // TODO
  }

  template <class T, class L>  
  void supervised_gradient::train(LabeledDataSource<T, L> *ds, 
				  classifier_meter *mtr, 
				  gd_param *gdp, double kappa) {
    ds->seek_begin();
    mtr->clear();
    this->train_online(ds, mtr, ds->size(), gdp, kappa);
  }

  template <class T, class L>  
  void supervised_gradient::compute_diaghessian(LabeledDataSource<T, L> *ds, 
						intg n, double mu) {
    param->clear_ddeltax();
    for (int i = 0; i < n; ++i) {
      ds->fprop(*input, *desired);
      machine->fprop(input, output, desired, energy);
      param->clear_dx();
      machine->bprop(input, output, desired, energy);
      param->clear_ddx();
      machine->bbprop(input, output, desired, energy);
      param->update_ddeltax((1 / (double) n), 1.0);
      ds->next();
    }
    param->compute_epsilons(mu);
    std::cout << "diaghessian inf: " << idx_min(param->epsilons);
    std::cout << " sup: " << idx_max(param->epsilons) << std::endl;
  }

  template <class T, class L>  
  void supervised_gradient::saliencies(LabeledDataSource<T, L> *ds, intg n) {
    err_not_implemented();
    /*
      param->clear_deltax();
      param->clear_ddeltax();
      for (int i = 0; i < n; ++i) {
      ds->fprop(input, desired);
      machine->fprop(input, output, desired, energy);
      param->clear_dx();
      param->update_deltax((1 / n), 1);
      machine->bprop(input, output, desired, energy);
      param->clear_ddx();
      machine->bbprop(input, output, desired, energy);
      param->update_ddeltax((1 / n), 1);
      ds->next();
      }
      param->saliencies();
      param->clear_deltax();
      param->clear_ddeltax();
    */
  }


} /* namespace ebl */
#endif /* TRAINER_HPP_ */
