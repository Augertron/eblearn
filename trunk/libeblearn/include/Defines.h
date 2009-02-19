/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef Defines_H
#define Defines_H

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#ifndef NULL
#define NULL (void*)0
#endif

// #define DEBUG_ON

#ifdef DEBUG_ON
#define DEBUG(s,d) fprintf(stderr,s,d)
#else
#define DEBUG(s,d)
#endif

// TODO: should be changed to throwing
// an exception or something.
// void ylerror(const char *s);
#define ylerror(s) {						\
    std::cerr << "\033[1;31mException:\033[0m " << s;		\
    std::cerr << ", in " << __FUNCTION__ << " at " << __FILE__;	\
    std::cerr << ":" << __LINE__ << std::endl;			\
    abort();							\
  }

#define err_not_implemented() {						\
    ylerror("member function not implemented for this class"); }

// not used right now
#define ITER(x) x##__iter

//! see numerics.h for description
extern bool drand_ini;

#define check_drand_ini() {					      \
    if (!drand_ini) printf("You have not initialized random sequence. \
Please call init_drand(time(NULL)) before using this function !\n"); }

/* namespace ebl { */

/* // intg is used for array indexing, hence should be */
/* // defined as long if you want very large arrays */
/* // on 64 bit machines. */
/* typedef long intg; */
/* typedef unsigned char ubyte; */

/* } // end namespace ebl */


#endif
