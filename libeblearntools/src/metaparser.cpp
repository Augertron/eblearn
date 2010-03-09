/***************************************************************************
 *   Copyright (C) 2009 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <map>
#include <algorithm>

#include "metaparser.h"
#include "libeblearntools.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // pairtree

  pairtree::pairtree(string &var, string &val)
    : variable(var), value(val), subvariable("") {
  }
  
  pairtree::pairtree()
    : variable(""), value(""), subvariable("") {
  }
  
  pairtree::~pairtree() {
  }

  map<string,string> pairtree::add(list<string> &subvar,
				   map<string,string> &ivars) {
    // the path leading to the current node 
    map<string,string> path;
    // use first string as subvariable
    if (subvar.size() > 0)
      subvariable = subvar.front();
    if (ivars.size() > 0) {
      // check if subvariable is in vars, if yes, this is a node, leaf otherwise
      if (ivars.find(subvariable) == ivars.end()) { // leaf
	// add all variables found in vars
	vars.insert(ivars.begin(), ivars.end());
      } else { // node
	// the var list without subvariable
	map<string,string> tmp(ivars);
	tmp.erase(subvariable);
	// subvariable list without current subvariable
	list<string> tmp2;
	if (subvar.size() > 0) {
	  tmp2 = subvar;
	  tmp2.pop_front();
	}
	// get existing node
	if (subtree.find(ivars[subvariable]) == subtree.end()) {
	  // no node, add new one
	  pairtree t(subvariable, ivars[subvariable]);
	  path = t.add(tmp2, tmp);
	  subtree[ivars[subvariable]] = t;
	} else {
	  path = subtree[ivars[subvariable]].add(tmp2, tmp);
	}
	// add current node pair to path
	path[subvariable] = ivars[subvariable];
      }
    }
    return path;
  }

  natural_varmap pairtree::flatten(const string &key, natural_varmap *flat,
				   map<string,string> *path) {
    natural_varmap flat2;
    map<string,string> path2;
    if (!flat)
      flat = &flat2;
    if (!path)
      path = &path2;

    for (map<string,pairtree>::iterator i = subtree.begin();
	 i != subtree.end(); ++i) {
      (*path)[subvariable] = i->first;
      i->second.flatten(key, flat, path);
    }
    if (subtree.size() == 0) { // leaf
      // merge path and vars
      map<string,string> all = vars;
      all.insert(path->begin(), path->end());
      // look for key
      map<string,string>::iterator k = all.find(key);
      if (k == all.end())
	return *flat;
      // key is found, extract key
      string kval = k->second;
      all.erase(k);
      (*flat)[kval] = all;
    }
    return *flat;
  } 

  natural_varmap pairtree::best(const string &key, uint n, bool display) {
    natural_varmap flat = flatten(key);
    natural_varmap::iterator i = flat.begin();
    for (uint j = 0; j < n; j++, i++);
    flat.erase(i, flat.end());
    if (display) {
      cout << "best results for \"" << key << "\":" << endl;
      pretty_flat(key, &flat);
    }
    return flat;
  }
 
  string& pairtree::get_variable() {
    return variable;
  }
  
  string& pairtree::get_value() {
    return value;
  }

  void pairtree::pretty(string offset) {
    string off = offset;
    off += "--";
    cout << off << " (" << variable << ", " << value << ")" << endl
	 << off << " vars: ";
    for (map<string,string>::iterator i = vars.begin(); i != vars.end(); ++i)
      cout << "(" << i->first << ", " << i->second << ") ";
    cout << endl << off << " subtree:" << endl;
    for (map<string,pairtree>::iterator i = subtree.begin();
	 i != subtree.end(); ++i) {
      cout << off << " (" << subvariable << ", " << i->first << ") ";
      cout << endl;
      i->second.pretty(off);
    }
  }
  
  string pairtree::flat_to_string(const string key, natural_varmap *flat) {
    ostringstream s;
    if (!flat)
      return s.str();
    for (natural_varmap::iterator i = flat->begin(); i != flat->end(); ++i) {
      s << i->first << ": ";
      for (map<string,string>::iterator j = i->second.begin();
	   j != i->second.end(); ++j)
	s << " (" << j->first << ", " << j->second << ") ";
      s << endl;
    }
    return s.str();
  }

  void pairtree::pretty_flat(const string key, natural_varmap *flat) {
    natural_varmap flat2;
    if (!flat) {
      flat2 = flatten(key);
      flat = &flat2;
    }
    cout << flat_to_string(key, flat);
  }

  uint pairtree::get_max_uint(const string &var) {
    uint max = 0;
    for (map<string,string>::iterator i = vars.begin(); i != vars.end(); ++i) {
      if (i->first == var)
	max = MAX(max, string_to_uint(i->second));
    }
    for (map<string,pairtree>::iterator i = subtree.begin();
	 i != subtree.end(); ++i) {
      if (subvariable == var)
	max = MAX(max, string_to_uint(i->first));
      max = MAX(max, i->second.get_max_uint(var));
    }
    return max;
  }
  
  ////////////////////////////////////////////////////////////////
  // metaparser

  metaparser::metaparser() : separator(VALUE_SEPARATOR) {
    hierarchy.push_back("name");
    hierarchy.push_back("job");
    hierarchy.push_back("i");
  }

  metaparser::~metaparser() {
  }

  bool metaparser::parse_log(const string &fname) {
    ifstream in(fname.c_str());
    string s, var, val;
    char separator = VALUE_SEPARATOR;
    string::size_type itok, stok;
    map<string,string> vars, keys;

    if (!in) {
      cerr << "warning: failed to open " << fname << endl;
      return false;
    }
    // parse all lines
    while (!in.eof()) {
      getline(in, s);
      istringstream iss(s, istringstream::in);
      // extract all variables for this line
      vars.clear();
      // initialize with keys of the hierarchy that have been seen before,
      // in case we have more variables
      // to add coming from a different line, this will be overriden
      // if new lines contains the hierarchy variables
      vars.insert(keys.begin(), keys.end());
      // loop over variable/value pairs
      itok = s.find(separator);
      while (itok != string::npos) { // get remaining values
	stok = s.find_last_of(' ', itok - 1); 
	var = s.substr(stok + 1, itok - stok - 1);
	stok = s.find_first_of(" \n\t\0", itok + 1);
	if (stok == string::npos)
	  stok = s.size();
	val = s.substr(itok + 1, stok - itok - 1);
	s = s.substr(stok);
	vars[var] = val;
	itok = s.find(separator);
	// if key, remember it for later
	if (find(hierarchy.begin(), hierarchy.end(), var) != hierarchy.end())
	  keys[var] = val;
      }
      // add variables to tree, and remember the path to the leaf
      tree.add(hierarchy, vars);
    }
    return true;
  }

  uint metaparser::get_max_iter() {
    return tree.get_max_uint("i");
  }
  
  natural_varmap metaparser::best(const string &key, uint n, bool display) {
    return tree.best(key, n, display);
  }
  
  // write plot files, using gpparams as additional gnuplot parameters
  bool metaparser::write_plots(string &gpparams) {
//     string gnuplot_column_separator = "\t";
//     string gnuplot_config1 = "clear ; \
// set terminal pdf ; \n";
//     string gnuplot_config2 = " set output \"";
//     string gnuplot_config3 = ".pdf\" ;	\
// plot ";	    
  
//     // loop on each plot
//     plots_t::iterator iplots = plots.begin();
//     for ( ; iplots != plots.end(); ++iplots) {
//       // open plot file
//       string plot_fname = iplots->first;
//       plot_fname += ".plot";
//       ofstream out(plot_fname.c_str());
//       if (!out)
// 	cerr << "warning: failed to open " << plot_fname << endl;
//       else {
// 	// open gnuplot p file
// 	string fname = iplots->first;
// 	fname += ".p";
// 	ofstream outp(fname.c_str());
// 	if (!outp)
// 	  cerr << "warning: failed to open " << fname << endl;
// 	else {
// 	  // initialize .p file
// 	  outp << gnuplot_config1 << gpparams << gnuplot_config2;
// 	  outp << iplots->first << gnuplot_config3;
// 	  // loop on each run to find maximum number of values and create
// 	  // the p files describing how to plot the data
// 	  map<string, map<double,string>, less<string> > &runs = iplots->second;
// 	  map<string, map<double,string>, less<string> >::iterator iruns =
// 	    runs.begin();
// 	  size_t max_nvalues = 0;
// 	  for (int r = 2; iruns != runs.end(); ++iruns, ++r) {
// 	    // find maximum number of values
// 	    max_nvalues = MAX(max_nvalues, iruns->second.size());
// 	    // add instruction to plot this run in the .p file
// 	    if (r > 2)
// 	      outp << ", ";
// 	    outp << "\"" << plot_fname << "\" using 1:" << r << " title \"";
// 	    outp << iruns->first << "\" with linespoints";
// 	  }
// 	  // loop on each run (1 run == 1 column)
// 	  uint pos = 0;
// 	  for (iruns = runs.begin(); iruns != runs.end(); ++iruns, ++pos) {
// 	    map<double,string> &values = iruns->second;
// 	    map<double,string>::iterator ival = values.begin();
// 	    // loop on each value and add 1 line per value
// 	    // with other columns with empty marker '?'
// 	    for ( ; ival != values.end(); ++ival) {
// 	      out << ival->first << gnuplot_column_separator;
// 	      for (uint j = 0; j < pos; ++j)
// 		out << "?"  << gnuplot_column_separator;
// 	      out << ival->second << gnuplot_column_separator;
// 	      for (uint j = pos + 1; j < runs.size(); ++j)
// 		out << "?"  << gnuplot_column_separator;
// 	      out << endl;
// 	    }
// 	  }
// 	  out << endl;
// 	}
// 	outp.close();
//       }
//       out.close();
//     }
    return true;
  }

  void metaparser::parse_logs(const string &root) {
    list<string> *fl = find_fullfiles(root, ".*[.]log");
    if (fl) {
      for (list<string>::iterator i = fl->begin(); i != fl->end(); ++i) {
	parse_log(*i);
      }
      delete fl;
      tree.pretty_flat("errors");
    }
  }
  
} // end namespace ebl
