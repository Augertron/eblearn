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

#ifndef STL_H_
#define STL_H_

#include "defines.h"

#ifdef __NOSTL__
#include <string.h>
#include <stdio.h>
#else // using STL
#include <string>
#include <iostream>
#include <sstream>
#endif

namespace ebl {

  //! Pre-declaration of classes.
  template <typename T> class idx;
  class eblexception;
  
}

namespace std {

  ////////////////////////////////////////////////////////////////
  // string

#ifdef __NOSTL__

  //! A string class.
  class EXPORT string {
  public:
    string();
    string(char *v);
    string(const char *v);
    virtual ~string();

    //! Returns a pointer to the const char *.
    const char *c_str() const;
    
    //! Returns size of the string.
    size_t size() const;
    
    //! Returns size of the string.
    size_t length() const;

    //! Clears the string.
    void clear();
    
    //! Returns true if resize was successful.
    inline bool resize(size_t newsize);
    
    string& operator+=(char *v);
    string& operator+=(const char *v);
    string& operator+=(const string &v);
    
    string& operator=(const char *v);
    string& operator=(char *v);
    
    string& operator<<(char *v);
    string& operator<<(const char *v);
    string& operator<<(const string &v);
    string& operator<<(void *v);
    string& operator<<(int v);
    string& operator<<(uint v);
    string& operator<<(char v);
    string& operator<<(unsigned char v);
    string& operator<<(float v);
    string& operator<<(double v);
    string& operator<<(long v); 
#ifndef __ANDROID__
    string& operator<<(size_t v);
#endif

  protected:
    char *s;
  };

#else
  
  //! Operator to concatenate elements into the string.
  template<class T>
    EXPORT string& operator<<(string& e, const T v);

  //! Operator to concatenate elements into the string.
  template<>
    EXPORT string& operator<<(string& e, void *v);
  template<>
    EXPORT string& operator<<(string& e, int v);
  template<>
    EXPORT string& operator<<(string& e, uint v);
  template<>
    EXPORT string& operator<<(string& e, char v);
  template<>
    EXPORT string& operator<<(string& e, unsigned char v);
  template<>
    EXPORT string& operator<<(string& e, float v);
  template<>
    EXPORT string& operator<<(string& e, double v);
  template<>
    EXPORT string& operator<<(string& e, long v); 
#ifndef __ANDROID__
  template<>
    EXPORT string& operator<<(string& e, size_t v);
#endif
    
#endif

  ////////////////////////////////////////////////////////////////
  // ostream
    
#ifdef __NOSTL__
  
#ifdef LIBIDX // we are inside the library
#define IDXEXPORT EXPORT
#else // we are outside
#define IDXEXPORT IMPORT
#endif

  class EXPORT ostream;

  // global variables declaration
  extern IDXEXPORT ostream cout;
  extern IDXEXPORT ostream cerr;
  enum t_endl { };
  extern IDXEXPORT t_endl endl;
  
  //! A stream class to output directly onto standard or error output.
  class EXPORT ostream {
  public:
    ostream(FILE *fp = stdout);
    virtual ~ostream();

    //! Operator to concatenate elements into the stream.
    template<class T> ostream& operator<<(const T &v);
    template<class T> ostream& operator<<(T &v);
    ostream& operator<<(const ebl::eblexception &v);
    ostream& operator<<(ebl::eblexception &v);
    ostream& operator<<(string &v);
    ostream& operator<<(const string &v);
    ostream& operator<<(void *v);
    ostream& operator<<(char *v);
    ostream& operator<<(const char *v);
    ostream& operator<<(t_endl v);
    ostream& operator<<(short v);
    ostream& operator<<(char v);
    ostream& operator<<(unsigned char v);
    ostream& operator<<(float v);
    ostream& operator<<(int v);
    ostream& operator<<(uint v);
    ostream& operator<<(double v);
    ostream& operator<<(long v);
#ifndef __ANDROID__
    ostream& operator<<(size_t v);
#endif
    
  private:
    FILE *fp;
#ifdef __ANDROID__
    string buf;
#endif
  };
  
#endif /* __NOSTL__ */

  ////////////////////////////////////////////////////////////////
  // vector

#ifdef __NOSTL__

  //! A vector class.
  template <typename T>
    class EXPORT vector : public ebl::idx<T> {
  public:
    vector();
    vector(size_t n, const T value = 0);
    virtual ~vector();

    //! Resize vector and push a new element at the end.
    //    void push_back(T &e);
    
    //! Resize vector and push a new element at the end.
    void push_back(T e);
    
    //! Returns size of the vector.
    size_t size() const;
    
    //! Returns size of the vector.
    size_t length() const;

    //! Returns true if empty, false otherwise.
    bool empty() const;

    //! Clears the vector and resizes it to zero-length.
    void clear();

    //! Return a reference to element at index i.
    T& operator[](size_t i);

      //! Return a reference to element at index i.
    const T& operator[](size_t i) const;
  };

#endif

  ////////////////////////////////////////////////////////////////
  // min/max

#ifdef __NOSTL__

  template <typename T>
    inline T min(const T &e1, const T &e2);
  
  template <typename T>
    inline T max(const T &e1, const T &e2);

#endif

} // end namespace std

namespace ebl {
  
  ////////////////////////////////////////////////////////////////
  // error reporting

  //! An exception class.
  class EXPORT eblexception : public std::string {
  public:
    eblexception();
    eblexception(const std::string &s);
    virtual ~eblexception();
  };

} // end namespace ebl

#include "stl.hpp"

#endif /* STL_H_ */