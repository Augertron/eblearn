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

#include "stl.h"
#include <stdio.h>
#include <stdlib.h>

namespace std {
  
  ////////////////////////////////////////////////////////////////
  // string

#ifdef __NOSTL__
  
  string::string() : s(NULL) {
  }

  string::string(char *v) : s(NULL) {
    *this = v;
  }

  string::string(const char *v) : s(NULL) {
    *this = v;
  }

  string::~string() {
    if (s)
      free(s);
  }

  const char* string::c_str() const {
    return s;
  }
    
  size_t string::size() const {
    if (!s)
      return 0;
    return strlen(s);
  }

  size_t string::length() const {
    if (!s)
      return 0;
    return strlen(s);
  }

  void string::clear() {
    if (s) {
      free(s);
      s = NULL;
    }
  }

  bool string::resize(size_t newsize) {
    if (newsize == 0)
      return true;
    if (newsize == size())
      return true;
    void *ns;
    if (!s)
      ns = malloc(newsize);
    else
      ns = realloc((void*) s, newsize);
    if (!ns)
      return false;
    s = (char *) ns;
    return true;
  }

  string& string::operator=(const char *v) {
    if (!v)
      return *this;
    size_t lv = strlen(v);
    if (lv == 0)
      return *this;
    if (!resize(lv + 1))
      eblthrow("failed to resize string");
    strcpy(s, v);
    return *this;
  }
  
  string& string::operator=(char *v) {
    if (!v)
      return *this;
    size_t lv = strlen(v);
    if (lv == 0)
      return *this;
    if (!resize(lv + 1))
      eblthrow("failed to resize string");
    strcpy(s, v);
    return *this;
  }
  
  string& string::operator<<(const string &v) {
    *this << v.s;
    return *this;
  }
  
  string& string::operator+=(const char *v) {
    *this << v;
    return *this;
  }
  
  string& string::operator+=(char *v) {
    *this << v;
    return *this;
  }
  
  string& string::operator+=(const string &v) {
    *this << v.s;
    return *this;
  }
  
  string& string::operator<<(char *v) {
    if (v == NULL)
      return *this;
    size_t lv = strlen(v), le = size();
    if (lv == 0)
      return *this;
    lv += le + 1;
    if (!resize(lv))
      eblthrow("failed to resize string");
    strcpy(s + le, v);
    return *this;
  }

  string& string::operator<<(const char *v) {
    if (v == NULL)
      return *this;
    size_t lv = strlen(v), le = size();
    if (lv == 0)
      return *this;
    lv += le + 1;
    if (!resize(lv))
      eblthrow("failed to resize string");
    strcpy(s + le, v);
    return *this;
  }

#define CONCAT_SPRINTF(type, specifier, type_cast)	  \
  string& string::operator<<(type v) {			  \
    char buf[64];					  \
    sprintf(buf, specifier, (type_cast) v);		  \
    *this << buf;					  \
    return *this;					  \
  }

  CONCAT_SPRINTF(void*, "%p", void*);
  CONCAT_SPRINTF(int, "%d", int);
  CONCAT_SPRINTF(uint, "%u", uint);
  CONCAT_SPRINTF(char, "%c", char);
  CONCAT_SPRINTF(float, "%f", float);
  CONCAT_SPRINTF(double, "%f", double);
  CONCAT_SPRINTF(long, "%ld", long);
  CONCAT_SPRINTF(unsigned char, "%c", unsigned char);
#ifndef __ANDROID__
  CONCAT_SPRINTF(size_t, "%u", uint);
#endif
  
#else // extending STL

#define CONCAT_SPRINTF(type, specifier, type_cast)	  \
  template <>						  \
  string& operator<<(string &e, type v) {		  \
    char buf[64];					  \
    sprintf(buf, specifier, (type_cast) v);		  \
    e << buf;						  \
    return e;						  \
  }

  CONCAT_SPRINTF(void*, "%p", void*);
  CONCAT_SPRINTF(int, "%d", int);
  CONCAT_SPRINTF(uint, "%u", uint);
  CONCAT_SPRINTF(char, "%c", char);
  CONCAT_SPRINTF(float, "%f", float);
  CONCAT_SPRINTF(double, "%f", double);
  CONCAT_SPRINTF(long, "%ld", long);
  CONCAT_SPRINTF(unsigned char, "%c", unsigned char);
#ifndef __ANDROID__
  CONCAT_SPRINTF(size_t, "%u", uint);
#endif
  
#endif

  ////////////////////////////////////////////////////////////////
  // ostream

#ifdef __ANDROID__

#define ANDROID_OUT(v)				\
  if (fp == stderr) {				\
    LOGE(buf); buf.clear();			\
  } else if (fp == stdout) {			\
    LOGI(buf); buf.clear();			\
  } else     

#define ANDROID_ACC(v)				\
  if (fp == stderr || fp == stdout)		\
    buf << v;					\
  else

#endif	
  
#ifdef __NOSTL__
  
  // global variables

  ostream cout;
  ostream cerr(stderr);
  t_endl endl;
  
  ostream::ostream(FILE *f) {
    fp = f;
  }

  ostream::~ostream() {
  }

  ostream& ostream::operator<<(const ebl::eblexception &v) {
    ANDROID_ACC(v)
    fprintf(fp, "%s", v.c_str());
    return *this;
  }
  
  ostream& ostream::operator<<(ebl::eblexception &v) {
    ANDROID_ACC(v)
    fprintf(fp, "%s", v.c_str());
    return *this;
  }
  
  ostream& ostream::operator<<(const string &v) {
    ANDROID_ACC(v)
    fprintf(fp, "%s", v.c_str());
    return *this;
  }
  
  ostream& ostream::operator<<(string &v) {
    ANDROID_ACC(v)
    fprintf(fp, "%s", v.c_str());
    return *this;
  }
  
  ostream& ostream::operator<<(char *v) {
    ANDROID_ACC(v)
    fprintf(fp, "%s", v);
    return *this;
  }

  ostream& ostream::operator<<(const char *v) {
    ANDROID_ACC(v)
    fprintf(fp, "%s", v);
    return *this;
  }

  ostream& ostream::operator<<(t_endl v) {
    ANDROID_OUT(v)
    fprintf(fp, "\n");
    return *this;
  }
  
#define CONCAT_FPRINTF(type, specifier, type_cast)		  \
  ostream& ostream::operator<<(type v) {			  \
    ANDROID_ACC(v)						  \
    fprintf(fp, specifier, (type_cast) v);			  \
    return *this;						  \
  }

  CONCAT_FPRINTF(void*, "%p", void*);
  CONCAT_FPRINTF(int, "%d", int);
  CONCAT_FPRINTF(uint, "%u", uint);
  CONCAT_FPRINTF(char, "%c", char);
  CONCAT_FPRINTF(float, "%f", float);
  CONCAT_FPRINTF(double, "%f", double);
  CONCAT_FPRINTF(short, "%d", int);
  CONCAT_FPRINTF(long, "%ld", long);
  CONCAT_FPRINTF(unsigned char, "%c", unsigned char);
#ifndef __ANDROID__
  CONCAT_FPRINTF(size_t, "%u", uint);
#endif
  
#endif /* __NOSTL__ */

} // namespace std

namespace ebl {
    
  ////////////////////////////////////////////////////////////////
  // error reporting

  eblexception::eblexception() {
  }

  eblexception::~eblexception() {
  }

  eblexception::eblexception(const std::string &s) {
    *this << s;
  }

} // namespace ebl

