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

  map<string,string> pairtree::add(list<string> &subvar_,
				   map<string,string> &ivars) {
    list<string> subvar = subvar_;
    // the path leading to the current node 
    map<string,string> path;
    // use first string that is in vars as subvariable
    if (subvar.size() > 0) {
      subvariable = subvar.front();
      subvar.pop_front();
      while ((subvar.size() > 0) && (ivars.find(subvariable) == ivars.end())) {
	subvariable = subvar.front();
	subvar.pop_front();
      }
    }
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
	// get existing node
	if (subtree.find(ivars[subvariable]) == subtree.end()) {
	  // no node, add new one
	  pairtree t(subvariable, ivars[subvariable]);
	  path = t.add(subvar, tmp);
	  subtree[ivars[subvariable]] = t;
	} else {
	  path = subtree[ivars[subvariable]].add(subvar, tmp);
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

  varmaplist pairtree::flatten(varmaplist *flat, map<string,string> *path) {
    varmaplist flat2;
    map<string,string> path2;
    if (!flat)
      flat = &flat2;
    if (!path)
      path = &path2;

    for (map<string,pairtree>::iterator i = subtree.begin();
	 i != subtree.end(); ++i) {
      (*path)[subvariable] = i->first;
      i->second.flatten(flat, path);
    }
    if (subtree.size() == 0) { // leaf
      // merge path and vars
      map<string,string> all = vars;
      all.insert(path->begin(), path->end());
      // push this map of variables onto the list of maps
      flat->push_back(all);
    }
    return *flat;
  } 

  natural_varmap pairtree::best(const string &key, uint n, bool display) {
    natural_varmap flat = flatten(key);
    // keep only first n entries
    natural_varmap::iterator i = flat.begin();
    for (uint j = 0; j < n && i != flat.end(); j++, i++) ;
    flat.erase(i, flat.end());
    // display
    if (display && flat.size() > 0) {
      cout << "Best " << n << " results for \"" << key << "\":" << endl;
      pretty_flat(key, &flat);
    }
    return flat;
  }

  varmaplist pairtree::best(list<string> &keys, uint n, bool display) {
    varmaplist flat = flatten();
    flat.sort(map_natural_less(keys));
    // keep only first n entries
    varmaplist::iterator i = flat.begin();
    for (uint j = 0; j < n && i != flat.end(); j++, i++) ;
    flat.erase(i, flat.end());
    // display
    if (display && flat.size() > 0) {
      cout << "Best " << n << " results for keys";
      for (list<string>::iterator i = keys.begin(); i != keys.end(); ++i)
	cout << " \"" << *i << "\"";
      cout << ":" << endl;
      pretty_flat(&flat, &keys);
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

  string pairtree::flat_to_string(varmaplist *flat_, list<string> *keys) {
    ostringstream s;
    if (!flat_)
      return s.str();
    varmaplist flat = *flat_; // make a copy
    uint j = 1;
    for (varmaplist::iterator i = flat.begin(); i != flat.end(); ++i, ++j) {
      s << j << ":";
      // first display keys in order
      if (keys) {
	for (list<string>::iterator k = keys->begin(); k != keys->end(); ++k) {
	  map<string,string>::iterator key = i->find(*k);
	  if (key != i->end()) {
	    // display key
	    s << " " << key->first << ": " << key->second;
	    // remove key
	    i->erase(key);
	  }
	}
      }
      for (map<string,string>::iterator j = i->begin();
	   j != i->end(); ++j)
	s << " " << j->first << ": " << j->second;
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

  void pairtree::pretty_flat(varmaplist *flat, list<string> *keys) {
    varmaplist flat2;
    if (!flat) {
      flat2 = flatten();
      flat = &flat2;
    }
    cout << flat_to_string(flat, keys);
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
  
  bool pairtree::exists(const string &var) {
    for (map<string,string>::iterator i = vars.begin(); i != vars.end(); ++i) {
      if (i->first == var)
	return true;
    }
    for (map<string,pairtree>::iterator i = subtree.begin();
	 i != subtree.end(); ++i) {
      if (subvariable == var)
	return true;
      if (i->second.exists(var))
	return true;
    }
    return false;
  }
  
  map<string,pairtree,natural_less>& pairtree::get_subtree() {
    return subtree;
  }
  
  ////////////////////////////////////////////////////////////////
  // metaparser

  metaparser::metaparser() : separator(VALUE_SEPARATOR) {
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
      //      vars.clear();
      // initialize with keys of the hierarchy that have been seen before,
      // in case we have more variables
      // to add coming from a different line, this will be overriden
      // if new lines contains the hierarchy variables
      //vars.insert(keys.begin(), keys.end());
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

  int metaparser::get_max_iter() {
    if (!tree.exists("i"))
      return -1;
    return (int) tree.get_max_uint("i");
  }
  
  natural_varmap metaparser::best(const string &key, uint n, bool display) {
    return tree.best(key, n, display);
  }

  varmaplist metaparser::best(list<string> &keys, uint n, bool display) {
    return tree.best(keys, n, display);
  }

  void metaparser::pretty() {
    list<string> keys;
    keys.push_back("test_errors");
    keys.push_back("errors");
    keys.push_back("test_energy");
    keys.push_back("energy");
    //    tree.best(keys, 15, true);
    string gppparams = "set grid ytics;set ytics 5;set mytics 5;set grid mytics;set logscale y";
    write_plots(gppparams);
    //    tree.pretty();
  }
  
  // write plot files, using gpparams as additional gnuplot parameters
  void metaparser::write_plots(string &gpparams) {
    string colsep = "\t";
    string gnuplot_config1 = "clear ; set terminal pdf ; \n";
    string gnuplot_config2 = " set output \"";
    string gnuplot_config3 = ".pdf\" ;	plot ";
    // a map of file pointers for .plot and .p files
    map<string,ofstream*> plotfiles, pfiles; 

    // we assume that the tree has been extracted using those keys in that
    // order: job, i
    // loop on each job
    uint ijob = 0;
    for (map<string,pairtree>::iterator i = tree.get_subtree().begin();
	 i != tree.get_subtree().end(); ++i, ++ijob) {
      string job = i->first;
      // flatten remaining tree based on key "i"
      string ikey = "i";
      natural_varmap flat = i->second.flatten(ikey);
      // loop on all i
      for (natural_varmap::iterator j = flat.begin(); j != flat.end(); ++j) {
	// convert i to double
	double ival = string_to_double(j->first);
	// loop on all variables
	for (map<string,string>::iterator k = j->second.begin();
	     k != j->second.begin(); ++k) {
	  // check that p file is open
	  if (pfiles.find(k->first) == pfiles.end()) {
	    // not open, add file
	    ostringstream fname;
	    fname << k->first << ".p";
	    pfiles[k->first] = new ofstream(fname.str().c_str());
	    if (!pfiles[k->first]) {
	      cerr << "warning: failed to open " << fname.str() << endl;
	      continue ; // keep going
	    }
	  }
	  // check that plot file is open
	  if (plotfiles.find(k->first) == plotfiles.end()) {
	    // not open, add file
	    ostringstream fname;
	    fname << k->first << ".plot";
	    cout << "opening " << fname.str() << endl;
	    plotfiles[k->first] = new ofstream(fname.str().c_str());
	    if (!plotfiles[k->first]) {
	      cerr << "warning: failed to open " << fname.str() << endl;
	      continue ; // keep going
	    }
	  }
	  // for the first i only, add job plot description in p file
	  if (j == flat.begin()) {
	    *pfiles[k->first] << "\"" << k->first << ".plot\" using 1:"
			      << ijob + 2 << " title \""
			      << job << "\" with linespoints";
	  }
	  // add this value into plot file
	  ofstream &outp = *plotfiles[k->first];
	  // first add abscissa value i
	  outp << ival << colsep;
	  // then fill with empty values until reaching job's column
	  for (uint c = 0; c < ijob; ++c)
	    outp << "?" << colsep;
	  // finally add job's value
	  outp << k->second << endl;
	}
      }
    }
    // close and delete all file pointers
    for (map<string,ofstream*>::iterator i = pfiles.begin();
	 i != pfiles.end(); ++i)
      if (*i->second) {
	(*i->second).close();
	delete i->second;
      }
    for (map<string,ofstream*>::iterator i = plotfiles.begin();
	 i != plotfiles.end(); ++i)
      if (*i->second) {
	(*i->second).close();
	delete i->second;
      }
  }

  void metaparser::parse_logs(const string &root) {
    list<string> *fl = find_fullfiles(root, ".*[.]log");
    if (fl) {
      for (list<string>::iterator i = fl->begin(); i != fl->end(); ++i) {
	parse_log(*i);
      }
      delete fl;
    }
  }
  
} // end namespace ebl
