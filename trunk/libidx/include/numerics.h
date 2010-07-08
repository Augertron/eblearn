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

#ifndef Numerics_H
#define Numerics_H

#include "defines.h"
#include <cmath>

namespace ebl {

  //! derivative of tanh
  EXPORT double dtanh(double x);

  //! "standard" sigmoid, used in Lush.
  //! Rational polynomial for computing y = 1.71593428*tanh(0.66666666*x)
  EXPORT float stdsigmoid(float x);
  //! derivative of standard digmoid.
  EXPORT float dstdsigmoid(float x);

  //! "standard" sigmoid, used in Lush.
  //! Rational polynomial for computing y = 1.71593428*tanh(0.66666666*x)
  EXPORT double stdsigmoid(double x);
  //! derivative of standard digmoid.
  EXPORT double dstdsigmoid(double x);

  ////////////////////////////////////////////////////////////////

  //! flag used to know if dseed has been already called
  //! This flag is raised only when init_rand is called, not
  //! when dseed is called.
  //! At the moment only the forget functions use this flag (see ebm)
#ifdef LIBIDX // declared from inside this library
  extern EXPORT bool drand_ini;
#else // declared from outside
  extern IMPORT bool drand_ini;
#endif

  //! initializes drand by calling dseed, and raises drand_ini
  EXPORT void init_drand(int x);

  //! initializes drand by calling dseed with a random seed (time(NULL), 
  //! and raises drand_ini.
  EXPORT void dynamic_init_drand();

  //! initializes drand by calling dseed with a fixed seed (0), 
  //! and raises drand_ini.
  EXPORT void fixed_init_drand();

  //! sets the seed of the random number generator.
  //! This MUST be called at least once before
  //! the random number generator is used. Otherwise
  //! calls to drand() and dgauss() always return the
  //! same number.
  EXPORT void dseed(int x);

  //! random number generator. Return a random number
  //! drawn from a uniform distribution over [0,1].
  EXPORT double drand(void);

  //! random number generator. Return a random number
  //! drawn from a uniform distribution over [-v,+v].
  EXPORT double drand(double v);

  //! random number generator. Return a random number
  //! drawn from a uniform distribution over [v0,v1].
  EXPORT double drand(double v0, double v1);

  //! draw a random number from a quasi-Gaussian
  //! distribution with mean 0 and variance 1.
  EXPORT double dgauss(void);

  //! draw a random number from a quasi-Gaussian
  //! distribution with mean 0 and variance sigma.
  EXPORT double dgauss(double sigma);

  //! draw a random number from a quasi-Gaussian
  //! distribution with mean m and variance sigma.
  EXPORT double dgauss(double m, double sigma);

  //! n choose k (k must be <= n)
  EXPORT int choose(int n, int k);

  } // end namespace ebl

#endif
