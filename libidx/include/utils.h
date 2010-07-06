/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef UTILS_H_
#define UTILS_H_

#ifdef __WINDOWS__
#include <time.h>
#else // linux & mac
#include <sys/time.h>
#include <unistd.h>
#endif

#include <string>
#include <stdio.h>
#include "defines.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // directory utilities

  //! Create specified directory and all its parents if they do not exists.
  //! Return false upon failure.
  EXPORT bool mkdir_full(const char *dir);
  
  //! Create specified directory and all its parents if they do not exists.
  //! Return false upon failure.
  EXPORT bool mkdir_full(string &dir);

  ////////////////////////////////////////////////////////////////
  // timing utilities

  //! Return a string containing a timestamp of localtime in the following
  //! format: "<year><month><day>.<hour><minutes><seconds>".
  EXPORT string tstamp();

  //! A timer class.
  class EXPORT timer {
  public:
    timer();
    virtual ~timer();
    //! Start timer.
    void start();
    //! Restart timer.
    void restart();
    //! Return elapsed time in minutes since start() or restart().
    double elapsed_minutes();
    //! Return elapsed time in seconds since start() or restart().
    long elapsed_seconds();
    //! Return elapsed time in milliseconds since start() or restart().
    long elapsed_milliseconds();
  private:
#ifdef __WINDOWS__
#else // linux & mac
    struct timeval t0, t1;
#endif
  };

  //! Sleep for 'millis' milliseconds.
  EXPORT void millisleep(long millis);
  
  //! Sleep for 'seconds' seconds.
  EXPORT void secsleep(long seconds);
  
} // end namespace ebl

#endif /* UTILS_ */
