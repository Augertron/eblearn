/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun, Pierre Sermanet, Koray Kavukcuoglu *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com, koray@cs.nyu.edu *
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

#ifndef EBLTESTER_H_
#define EBLTESTER_H_

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <ostream>

#include "Idx.h"
#include "Blas.h"
#include "EblStates.h"
#include "EblBasic.h"
#include "EblArch.h"

namespace ebl {

  /*
   * Tests any module that is descendant of module_1_1. (includes layers_n)
   * bprop to input and to parameters are validated with finite difference
   * approximations.
   *
   * This class is not meant to replace the testing framework for eblearn 
   * developers but it should be useful for eblearn users that create new 
   * modules using this library as a binary reference
   */
  class ModuleTester {
  public:

    /*
     * Standard constructor
     */
    ModuleTester();
    /*
     * out    : reference to ostream to push results
     * thres  : threshold to decide bprop and finite diff jacobian are same or 
     *          different this might be tricky because if the modules 
     *          accumulates derivatives, then the difference will inevitably 
     *          grow, so user has to have a sense of what is valid 
     *          default : 1e-8
     * rrange : range that is used to randomize the input state and parameter 
     *          objects. if this value is very small, then the jacobians will 
     *          always be very similar
     */
    ModuleTester(FILE* out, double thres, double rrange);
    ModuleTester(double thres, double rrange);

    virtual ~ModuleTester();

    /*
     * test the derivative of output wrt input
     * module : pointer to a module_1_1
     * in     : pointer to state_idx
     * out    : pointer to state_idx
     */
    void test_jacobian(module_1_1<state_idx,state_idx> *module, state_idx *in, 
		       state_idx *out);
    /*
     * test the derivative of output wrt parameters of the module
     * p      : pointer to parameter object that was used to allocate the 
     *          parameters of module. this parameter object should contain the 
     *          parameter of only this module
     * module : pointer to a module_1_1
     * in     : pointer to state_idx
     * out    : pointer to state_idx
     */
    void test_jacobian_param(parameter *p, 
			     module_1_1<state_idx,state_idx>* module, 
			     state_idx *in, state_idx *out);

    /*
     * get accuracy threshold
     */
    double get_acc_thres() const;
    /*
     * set accuracy threshold
     */
    void set_acc_thres(double acc_thres);
    /*
     * get random range
     */
    double get_rrange() const;
    /*
     * set random range
     */
    void set_rrange(double rrange);
    /*
     * get ostream used
     */
    FILE* get_out() const;
    /*
     * set ostream
     */
    void set_out(FILE* out);

  private:
    // accuracy threshold
    double acc_thres;
    // ranfom initialization range
    double rrange;
    // output stream
    FILE* out;
    // jacobian from fprop
    Idx<double> jac_fprop;
    // jacobian from bprop;
    Idx<double> jac_bprop;
    // jacobian from fprop wrt param
    Idx<double> jac_fprop_param;
    // jacobian from bprop wrt param
    Idx<double> jac_bprop_param;

    Idx<double>* kk;

    /*
     * get jacobian using 1st order central finite differnce approximation for 
     * derivative of output wrt input
     * module : pointer to a module_1_1
     * in     : pointer to state_idx
     * out    : pointer to state_idx
     */
    void get_jacobian_fprop(module_1_1<state_idx,state_idx> *module, 
			    state_idx *in, state_idx *out,Idx<double>& jac);
    /*
     * get jacobian using 1st order central finite differnce approximation for 
     * derivative of output wrt parameter
     * p      : pointer to parameter object that was used to allocate the 
     *          parameters of module. this parameter object should contain the 
     *          parameter of only this module
     * module : pointer to a module_1_1
     * in     : pointer to state_idx
     * out    : pointer to state_idx
     */
    void get_jacobian_fprop_param(parameter *p, 
				  module_1_1<state_idx,state_idx> *module, 
				  state_idx *in, state_idx *out,
				  Idx<double>& jac);
    /*
     * get jacobian using bprop for derivative of output wrt input
     * module : pointer to a module_1_1
     * in     : pointer to state_idx
     * out    : pointer to state_idx
     */
    void get_jacobian_bprop(module_1_1<state_idx,state_idx> *module, 
			    state_idx *in, state_idx *out,Idx<double>& jac);
    /*
     * get jacobian using bprop for derivative of output wrt parameter
     * p      : pointer to parameter object that was used to allocate the 
     *          parameters of module. this parameter object should contain the 
     *          parameter of only this module
     * module : pointer to a module_1_1
     * in     : pointer to state_idx
     * out    : pointer to state_idx
     */
    void get_jacobian_bprop_param(parameter *p, 
				  module_1_1<state_idx,state_idx> *module, 
				  state_idx *in, state_idx *out,
				  Idx<double>& jac);
    /*
     * a : first Idx to compare
     * b : second Idx to compare
     * c : message to use
     */
    void report_err(Idx<double>& a, Idx<double>& b, const char* msg);
    /*
     * assigns random numbers to every element of give idx
     */
    void randomize_idx(Idx<double>& m);
  };

  ////////////////////////////////////////////////////////////////
  //! Generic Jacobian tester

  class Jacobian_tester {

  public:
    Jacobian_tester() {
    }
    ;
    ~Jacobian_tester() {
    }
    ;

    // this function take any module_1_1 with a fprop et bprop implemented,
    // and tests if the jacobian is correct (by pertubation) 
    // (on a state_idx with 3 dimensions)
    void test(module_1_1<state_idx, state_idx> *module);

  };

  ////////////////////////////////////////////////////////////////
  //! Generic BBprop tester tester

  class Bbprop_tester {

  public:
    Bbprop_tester() {
    }
    ;
    ~Bbprop_tester() {
    }
    ;

    // this function take any module_1_1 with a fprop et bbprop implemented, 
    // and tests if the Bbprop is correct (by pertubation) 
    // (on a state_idx with 3 dimensions)
    void test(module_1_1<state_idx, state_idx> *module);

  };

  ////////////////////////////////////////////////////////////////
  //! Generic Bprop tester tester

  class Bprop_tester {

  public:
    Bprop_tester() {
    }
    ;
    ~Bprop_tester() {
    }
    ;

    // this function take any module_1_1 with a fprop et bprop implemented, 
    // and tests if the bprop is correct (by pertubation) 
    // (on a state_idx with 3 dimensions)
    void test(module_1_1<state_idx, state_idx> *module);

  };

} // namespace ebl {

#endif /* EBLTESTER_H_ */
