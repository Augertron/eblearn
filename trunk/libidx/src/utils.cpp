/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

// tell header that we are in the libidx scope
#define LIBIDX

#include "utils.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <time.h>

#ifdef __WINDOWS__
#include <Windows.h>
#endif

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // directory utilities

  bool mkdir_full(string &dir) {
    return mkdir_full(dir.c_str());
  }
  
  bool mkdir_full(const char *dir) {
    string cmd = "mkdir -p ";
    cmd += dir;
    if (system(cmd.c_str()) < 0) {
      cerr << "warning: failed to create directory " << dir;
      return false;
    }
    return true;
  }

  ////////////////////////////////////////////////////////////////
  // timing utilities

  string tstamp() {
    ostringstream ts;
    static time_t t = time(NULL);
    static struct tm *lt = localtime(&t);
    ts << setw(2) << setfill('0') << lt->tm_year + 1900
       << setw(2) << setfill('0') << lt->tm_mon
       << setw(2) << setfill('0') << lt->tm_mday
       << "."
       << setw(2) << setfill('0') << lt->tm_hour
       << setw(2) << setfill('0') << lt->tm_min
       << setw(2) << setfill('0') << lt->tm_sec;
    return ts.str();
  }

  timer::timer() {
  }

  timer::~timer() {
  }

  void timer::start() {
#ifdef __WINDOWS__
    // TODO
#else // linux & mac
    gettimeofday(&t0, NULL);
#endif
  }

  void timer::restart() {
#ifdef __WINDOWS__
    // TODO
#else // linux & mac
    gettimeofday(&t0, NULL);
#endif
  }

  double timer::elapsed_minutes() {
#ifdef __WINDOWS__
    return 0; // TODO
#else // linux & mac
    gettimeofday(&t1, NULL);
    return (t1.tv_sec - t0.tv_sec) / (double) 60;
#endif
  }
  
  long timer::elapsed_seconds() {
#ifdef __WINDOWS__
    return 0; // TODO
#else // linux & mac
    gettimeofday(&t1, NULL);
    return t1.tv_sec - t0.tv_sec;
#endif
  }
  
  long timer::elapsed_milliseconds() {
#ifdef __WINDOWS__
    return 0; // TODO
#else // linux & mac
    gettimeofday(&t1, NULL);
    return (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_usec - t0.tv_usec) / 1000;
#endif
  }

  void millisleep(long millis) {
#ifdef __WINDOWS__
    Sleep(millis);
#else
    usleep(millis * 1000);
#endif
  } 
  
  void secsleep(long seconds) {
#ifdef __WINDOWS__
    Sleep(seconds * 1000);
#else
    usleep(seconds * 1000);
#endif
  } 
  
} // namespace ebl
