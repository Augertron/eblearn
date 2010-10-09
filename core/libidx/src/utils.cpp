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

#ifndef __NOSTL__
#include <sstream>
#include <iostream>
#include <iomanip>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __WINDOWS__
#include <Windows.h>
#include <direct.h>
#endif

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // directory utilities

#define FILELEN 512
  
  bool mkdir_full(string &dir) {
    return mkdir_full(dir.c_str());
  }
  
  bool mkdir_full(const char *dir) {
#ifdef __WINDOWS__
    if (!_mkdir(dir))
      return true;
    return false;
#else
    string cmd = "mkdir -p ";
    cmd += dir;
    if (system(cmd.c_str()) < 0) {
      cerr << "warning: failed to create directory " << dir;
      return false;
    }
    return true;
#endif
  }

  bool dir_exists(const char *s) {
#ifndef __WINDOWS__
    struct stat buf;
    if (stat(s,&buf)==0)
      if (buf.st_mode & S_IFDIR)
	return true;
#else
    char *last;
    char buffer[FILELEN];
    struct _stat buf;
    if ((s[0]=='/' || s[0]=='\\') && 
	(s[1]=='/' || s[1]=='\\') && !s[2]) 
      return true;
    if (strlen(s) > sizeof(buffer) - 4)
      eblerror("Filename too long");
    strcpy(buffer,s);
    last = buffer + strlen(buffer) - 1;
    if (*last=='/' || *last=='\\' || *last==':')
      strcat(last,".");
    if (_stat(buffer,&buf)==0)
      if (buf.st_mode & S_IFDIR)
	return true;
#endif
    return false;
  }

  bool file_exists(const char *s) {
#ifndef __WINDOWS__
    struct stat buf;
    if (stat(s,&buf)==-1)
      return false;
    if (buf.st_mode & S_IFREG) 
      return false;
#else
    struct _stat buf;
    if (_stat(s,&buf)==-1)
      return false;
    if (buf.st_mode & S_IFREG) 
      return false;
#endif
    return true;
  }

  ////////////////////////////////////////////////////////////////
  // timing utilities

  string tstamp() {
    static time_t t = time(NULL);
    static struct tm *lt = localtime(&t);
#ifdef __NOSTL__
    string ts;
    ts << lt->tm_year + 1900 << lt->tm_mon << lt->tm_mday
       << "." << lt->tm_hour << lt->tm_min << lt->tm_sec;
    return ts;
#else
    ostringstream ts;
    ts << setw(2) << setfill('0') << lt->tm_year + 1900
       << setw(2) << setfill('0') << lt->tm_mon
       << setw(2) << setfill('0') << lt->tm_mday
       << "."
       << setw(2) << setfill('0') << lt->tm_hour
       << setw(2) << setfill('0') << lt->tm_min
       << setw(2) << setfill('0') << lt->tm_sec;
    return ts.str();
#endif
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

  void timer::pretty_elapsed() {
    pretty_secs(elapsed_seconds());
  }

  string timer::elapsed() {
    return elapsed(elapsed_seconds());
  }
  
  string timer::elapsed_ms() {
    return elapsed_ms(elapsed_milliseconds());
  }
  
  void timer::pretty_secs(long seconds) {
    cout << elapsed(seconds);
  }
  
  // second equivalences
#define SECMIN 60
#define SECHOUR 3600
#define SECDAY 86400
#define SECWEEK 604800
#define SECMONTH 18144000

  std::string timer::elapsed(long seconds) {
    std::string sout;
    long div, mod;
    bool pretty = false;
    div = seconds / SECMONTH; mod = seconds % SECMONTH;
    if (div > 0 || pretty) {
      sout << (int) div << "m ";
      pretty = true;
    }
    div = mod / SECWEEK; mod = mod % SECWEEK;
    if (div > 0 || pretty) {
      sout << (int) div << "w ";
      pretty = true;
    }
    div = mod / SECDAY; mod = mod % SECDAY;
    if (div > 0 || pretty) {
      sout << (int) div << "d ";
      pretty = true;
    }
    div = mod / SECHOUR; mod = mod % SECHOUR;
    if (div > 0 || pretty) {
      sout << (int) div << "h ";
      pretty = true;
    }
    div = mod / SECMIN; mod = mod % SECMIN;
    if (div > 0 || pretty) {
      sout << (int) div << "m ";
      pretty = true;
    }
    sout << (int) mod << "s";
    return sout;
  }

  // second equivalences
#define MSMIN 60000
#define MSHOUR 3600000
#define MSDAY 86400000
#define MSWEEK 604800000
#define MSMONTH 18144000000ULL

  std::string timer::elapsed_ms(long milliseconds) {
    std::string sout;
    long div, mod;
    bool pretty = false;
    div = milliseconds / MSMONTH; mod = milliseconds % MSMONTH;
    if (div > 0 || pretty) {
      sout << (int) div << "m ";
      pretty = true;
    }
    div = mod / MSWEEK; mod = mod % MSWEEK;
    if (div > 0 || pretty) {
      sout << (int) div << "w ";
      pretty = true;
    }
    div = mod / MSDAY; mod = mod % MSDAY; 
    if (div > 0 || pretty) {
      sout << (int) div << "d ";
      pretty = true;
    }
    div = mod / MSHOUR; mod = mod % MSHOUR;
    if (div > 0 || pretty) {
      sout << (int) div << "h ";
      pretty = true;
    }
    div = mod / MSMIN; mod = mod % MSMIN;
    if (div > 0 || pretty) {
      sout << (int) div << "m ";
      pretty = true;
    }
    div = mod / 1000; mod = mod % 1000;
    if (div > 0 || pretty) {
      sout << (int) div << "s ";
      pretty = true;
    }
    sout << (int) mod << "ms";
    return sout;
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
