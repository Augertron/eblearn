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

#include "configuration.h"
#include "sort.h"

#define VALUE_SEPARATOR '='

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // iteration

  typedef map<string,map<string,string>,natural_less> natural_varmap;
  typedef list<map<string,string> > varmaplist;
  
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

    //! Empty constructor, should be used for the unique root only.
    pairtree();
      
    //! Destructor.
    virtual ~pairtree();

    map<string,string> add(list<string> &subvar, map<string,string> &ivars);

    //! Return a flat representation of the tree, using the variable name
    //! key as key to represent each group of variables.
    //! E.g. if we have a tree with nodes "name" and "i", and leaves
    //! contain "error", "success" and we choose "error" as our key,
    //! this will return a map with all error values paired with
    //! all var/val pairs of "name", "i" and "success".
    natural_varmap flatten(const string &key, natural_varmap *flat = NULL,
			   map<string,string> *path = NULL);

    //! Return a flat representation of the tree, as a list of all possible
    //! branches (each branch is a map of var/val pairs).
    varmaplist flatten(varmaplist *flat = NULL,
		       map<string,string> *path = NULL);

    //! Return the n best values (minimized) of key.
    //! \param display If true, pretty best answers.
    natural_varmap best(const string &key, uint n, bool display = false);
    
    //! Return the n best sets of variables that minimize the key in the order
    //! of their list.
    //! \param display If true, pretty best answers.
    varmaplist best(list<string> &keys, uint n, bool display = false);

    //! Returns the variable name associated with this pair.
    string& get_variable();

    //! Returns the value associated with this pair.
    string& get_value();

    //! Pretty this tree, with a string offset beforehand.
    void pretty(string offset = "");

    //! Returns a string representation of this tree flattened using key.
    //! If flat is not NULL, pretty this flat, otherwise generate
    //! a flat representation of the current tree.
    static string flat_to_string(const string key, natural_varmap *flat =NULL);

    //! Returns a string representation of this tree flattened using key.
    //! If flat is not NULL, pretty this flat, otherwise generate
    //! a flat representation of the current tree.
    //! \param keys If not null, display keys first.
    static string flat_to_string(varmaplist *flat =NULL,
				 list<string> *keys = NULL);

    //! Pretty this tree, flattened using key.
    //! If flat is not NULL, pretty this flat, otherwise generate
    //! a flat representation of the current tree.
    void pretty_flat(const string key, natural_varmap *flat = NULL);

    //! Pretty this tree, flattened.
    //! If flat is not NULL, pretty this flat, otherwise generate
    //! a flat representation of the current tree.
    //! \param keys If not null, display keys first.
    void pretty_flat(varmaplist *flat = NULL, list<string> *keys = NULL);

    //! Return the maximum uint value of variable var.
    uint get_max_uint(const string &var);

    //! Return true if variable var exists in the tree.
    bool exists(const string &var);

    //! Return sub tree.
    map<string,pairtree,natural_less>& get_subtree();

    ////////////////////////////////////////////////////////////////
    // members
  private:
    string			variable;	//!< The variable.
    string			value;	//!< The value.
    string			subvariable;	//!< variable name of subtree.
    map<string, pairtree, natural_less>	subtree;//!< Subtree of the subvariable.
    map<string, string>		vars;//!< Map of leaf variables and their value.
  };
  
  ////////////////////////////////////////////////////////////////
  // metaparser
  
  //! A parser that can analyze the output of multiple jobs (usually used in
  //! conjunction with metarun).
  class metaparser {
  public:
    //! Constructor, initialize the hierarchy. This hierarchy defines
    //! groups of variables, e.g. we want to have a first level of variables
    //! for each value of variable "name", then a sub level for each value
    //! of variable "i".
    metaparser();

    //! Destructor.
    virtual ~metaparser();

    //! Parse all files in root matching the .log extension.
    void parse_logs(const string &root);

    //! Write text files parsable by plotting tools such as gnuplot,
    //! and generate pdf plots with gnuplot into directory dir.
    //! \param gpparams Extra plot configurations parameters.
    void write_plots(const char *dir = NULL, const char *gpparams = NULL);

    //! Return the n best values (minimized) of key.
    natural_varmap best(const string &key, uint n, bool display = false);

    //! Return the n best values (minimized) of key.
    varmaplist best(list<string> &keys, uint n, bool display = false);

    //! Return the maximum iteration number, i.e. the maximum value found
    //! for variable "i", -1 if nothing is found.
    int get_max_iter();
    
    //! Parse, analyze and report.
    void process(const string &dir);

    //! Analyze log files and return the best set of variables.
    //! \param maxiter Set this to the maximum iteration number found.
    varmaplist analyze(configuration &conf, const string &dir, int &maxiter);
    
    //! Send an email reporting the status of the runs.
    void send_report(configuration &conf, const string &dir,
		     varmaplist &best, int iteration,
		     string &conf_fullfname, uint nrunning = 0);

    ////////////////////////////////////////////////////////////////
    // internal methods
  private:
    bool parse_log(const string &fname);
    
    ////////////////////////////////////////////////////////////////
    // members
  private:
    pairtree		tree;	        //!< A tree of var/val pairs.
    char		separator;      //!< token separating var/val
    map<string,string>	curpath;	//!< Current path to pairtree leaf.
    list<string>	hierarchy;	//!< List of vars forming the hierarchy.
  };

} // end namespace ebl

#endif /* METAPLOT_H_ */
