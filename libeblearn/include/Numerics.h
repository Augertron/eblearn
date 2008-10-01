/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun   *
 *   yann@cs.nyu.edu   *
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

#ifndef Numerics_H
#define Numerics_H

#include <math.h>

namespace ebl {

//! derivative of tanh
double dtanh(double x);

//! "standard" sigmoid, used in Lush.
//! Rational polynomial for computing y = 1.71593428*tanh(0.66666666*x)
double stdsigmoid(double x);
//! derivative of standard digmoid.
double dstdsigmoid(double x);


////////////////////////////////////////////////////////////////

//! flag used to know if dseed has been already called
//! This flag is raised only when init_rand is called, not
//! when dseed is called.
//! At the moment only the forget functions use this flag (see ebm)
extern bool drand_ini;

//! initializes drand by calling dseed, and raises drand_ini
void init_drand(int x);

//! sets the seed of the random number generator.
//! This MUST be called at least once before
//! the random number generator is used. Otherwise
//! calls to drand() and dgauss() always return the
//! same number.
void dseed(int x);

//! random number generator. Return a random number
//! drawn from a uniform distribution over [0,1].
double drand(void);

//! random number generator. Return a random number
//! drawn from a uniform distribution over [-v,+v].
double drand(double v);

//! random number generator. Return a random number
//! drawn from a uniform distribution over [v0,v1].
double drand(double v0, double v1);

//! draw a random number from a quasi-Gaussian
//! distribution with mean 0 and variance 1.
double dgauss(void);

//! draw a random number from a quasi-Gaussian
//! distribution with mean 0 and variance sigma.
double dgauss(double sigma);

//! draw a random number from a quasi-Gaussian
//! distribution with mean m and variance sigma.
double dgauss(double m, double sigma);

} // end namespace ebl

#endif
