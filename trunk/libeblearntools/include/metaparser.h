/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet   *
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

#ifndef METAPLOT_H_
#define METAPLOT_H_

#include <sstream>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <map>
#include <list>

#define VALUE_SEPARATOR '='

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // iteration

  //! A class representing a tree hierarchy of variable/value pairs.
  //! A pair tree is defined by a variable/value pair, and a subtree
  //! corresponding to 1 subvariable only (could be extended to multiple
  //! variables). The subtree is a map of pairtrees for each value of this
  //! subvariable.
  class pairtree {
  public:
    //! Constructor.
    //! \param var A string containing a variable name.
    //! \param val A string containing a value.
    pairtree(string &var, string &val);

    pairtree();
      
    //! Destructor.
    virtual ~pairtree();

    void add(list<string> &subvar, map<string,string> &ivars);
    
    //! Returns the variable name associated with this pair.
    string& get_variable();

    //! Returns the value associated with this pair.
    string& get_value();

    //! Pretty this tree, with a string offset beforehand.
    void pretty(string offset = "");

    ////////////////////////////////////////////////////////////////
    // members
  private:
    string			variable;	//!< The variable.
    string			value;	//!< The value.
    string			subvariable;	//!< variable name of subtree.
    map<string, pairtree>	subtree;	//!< Subtree of the subvariable.
    map<string, string>		vars;//!< Map of leaf variables and their value.
  };
  
  /* //////////////////////////////////////////////////////////////// */
  /* // metaparser */
  
  /* //! A parser that can analyze the output of multiple jobs (usually used in */
  /* //! conjunction with metarun). */
  /* class metaparser { */
  /* public: */
  /*   metaparser(); */
  /*   virtual ~metaparser(); */

  /*   //! Parse */
  /*   void run(); */

  /*   //! Add a new iteration to our list and return a pointer to it. */
  /*   //! \param iter The iteration number. */
  /*   iteration* new_iteration(double iter); */

  /*   //! Write text files parsable by plotting tools such as gnuplot, */
  /*   //! using iteration */
  /*   bool write_plots(); */

  /*   //////////////////////////////////////////////////////////////// */
  /*   // internal methods */
  /* private: */
  /*   bool parse_log(const string &fname); */
    
  /*   //////////////////////////////////////////////////////////////// */
  /*   // members */
  /* private: */
  /*   list<iteration> iterations; //!< a list of all iterations */
  /*   static char separator = VALUE_SEPARATOR; //! token to separate var/val */
  /* }; */

} // end namespace ebl

#endif /* METAPLOT_H_ */
