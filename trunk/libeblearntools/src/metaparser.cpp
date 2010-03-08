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
#include "libidx.h"
#include <algorithm>

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

#include "libeblearntools.h"
#include "metaparser.h"

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

  void pairtree::add(list<string> &subvar, map<string,string> &ivars) {
    if (subvar.size() > 0)
      subvariable = subvar.front();
    if (ivars.size() > 0) {
      // check if subvariable is in vars, if yes, this is a node, leaf otherwise
      if (ivars.find(subvariable) == ivars.end()) { // leaf
	vars = ivars; // add all variables found in vars
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
	pairtree t(subvariable, ivars[subvariable]);
	t.add(tmp2, tmp);
	subtree[ivars[subvariable]] = t;
      }
    }
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

// ////////////////////////////////////////////////////////////////
// // metaparser

// iteration* metaparse::new_iteration(double iter) {
//   iterations.push_back(iter);
//   return &iterations.back();
// }

// bool metaparse::parse_log(const string &fname) {
//   ifstream in(fname.c_str());
//   string s, var, val;
//   char separator = VALUE_SEPARATOR;
//   double iter;
//   float f;
//   string::size_type itok, stok;
//   iteration *i = NULL;
//   map<string,string> vars;

//   if (!in) {
//     cerr << "warning: failed to open " << fname << endl;
//     return false;
//   }
//   // parse all lines
//   while (!in.eof()) {
//     getline(in, s);
//     istringstream iss(s, istringstream::in);
//     // extract all variables for this line
//     vars.clear();
//     itok = s.find(separator);
//     while (itok != string::npos) { // get remaining values
//       stok = s.find_last_of(' ', itok - 1); 
//       var = s.substr(stok + 1, itok - stok - 1);
//       stok = s.find_first_of(' ', itok + 1);
//       val = s.substr(itok + 1, stok - itok - 1);
//       s = s.substr(stok + 1);
//       vars[var] = val;
//       itok = s.find(separator);
//     }
    
//   }
//   return true;
// }


// // write plot files, using gpparams as additional gnuplot parameters
//   bool metaparser::write_plots(plots_t &plots, string &gpparams) {
//   string gnuplot_column_separator = "\t";
//   string gnuplot_config1 = "clear ; \
// set terminal pdf ; \n";
//   string gnuplot_config2 = " set output \"";
//   string gnuplot_config3 = ".pdf\" ;	\
// plot ";	    
  
//   // loop on each plot
//   plots_t::iterator iplots = plots.begin();
//   for ( ; iplots != plots.end(); ++iplots) {
//     // open plot file
//     string plot_fname = iplots->first;
//     plot_fname += ".plot";
//     ofstream out(plot_fname.c_str());
//     if (!out)
//       cerr << "warning: failed to open " << plot_fname << endl;
//     else {
//       // open gnuplot p file
//       string fname = iplots->first;
//       fname += ".p";
//       ofstream outp(fname.c_str());
//       if (!outp)
// 	cerr << "warning: failed to open " << fname << endl;
//       else {
// 	// initialize .p file
// 	outp << gnuplot_config1 << gpparams << gnuplot_config2;
// 	outp << iplots->first << gnuplot_config3;
// 	// loop on each run to find maximum number of values and create
// 	// the p files describing how to plot the data
// 	map<string, map<double,string>, less<string> > &runs = iplots->second;
// 	map<string, map<double,string>, less<string> >::iterator iruns =
// 	  runs.begin();
// 	size_t max_nvalues = 0;
// 	for (int r = 2; iruns != runs.end(); ++iruns, ++r) {
// 	  // find maximum number of values
// 	  max_nvalues = MAX(max_nvalues, iruns->second.size());
// 	  // add instruction to plot this run in the .p file
// 	  if (r > 2)
// 	    outp << ", ";
// 	  outp << "\"" << plot_fname << "\" using 1:" << r << " title \"";
// 	  outp << iruns->first << "\" with linespoints";
// 	}
// 	// loop on each run (1 run == 1 column)
// 	uint pos = 0;
// 	for (iruns = runs.begin(); iruns != runs.end(); ++iruns, ++pos) {
// 	  map<double,string> &values = iruns->second;
// 	  map<double,string>::iterator ival = values.begin();
// 	  // loop on each value and add 1 line per value
// 	  // with other columns with empty marker '?'
// 	  for ( ; ival != values.end(); ++ival) {
// 	    out << ival->first << gnuplot_column_separator;
// 	    for (uint j = 0; j < pos; ++j)
// 	      out << "?"  << gnuplot_column_separator;
// 	    out << ival->second << gnuplot_column_separator;
// 	    for (uint j = pos + 1; j < runs.size(); ++j)
// 	      out << "?"  << gnuplot_column_separator;
// 	    out << endl;
// 	  }
// 	}
// 	out << endl;
//       }
//       outp.close();
//     }
//     out.close();
//   }
//   return true;
// }

// // recursively explore all subdirectories of root and add plots when .log
// // are found.
// bool find_logs(const path &root, plots_t &plots) {
//   // explore root
//   cmatch what;
//   regex hidden_dir(".svn");
//   regex output_log(".*[.]log");
//   directory_iterator end_itr; // default construction yields past-the-end
//   for (directory_iterator itr(root); itr != end_itr; itr++) {
//     if (!regex_match(itr->leaf().c_str(), what, hidden_dir)) {      
//       // recursively loop into subdirectories
//       if (is_directory(itr->status()))
// 	find_logs(itr->path(), plots);
//       else {
// 	// look for output.log
// 	if (regex_match(itr->leaf().c_str(), what, output_log))
// 	  parse_output_log(itr->path().string(), root.leaf(), plots);
//       }
//     }
//   }
//   return true;
// }

// int main(int argc, char **argv) {
//   cout << "________________________________Meta Plotter";
//   cout << "________________________________" << endl;
//   // parse arguments
//   if (!parse_args(argc, argv)) {
//     print_usage();
//     return -1;
//   }
//   string root = argv[1];
//   string gpparams;

//   // for each variable, for each logfile, the values of a plot in a vector
//   plots_t plots;
//   // check for gnuplot parameters in gnuplot_params.txt
//   ostringstream p;
//   p << root << "/" << "gnuplot_params.txt";
//   ifstream in(p.str().c_str());
//   if (in) {
//     getline(in, gpparams);
//     in.close();
//   }
 
// #ifdef __BOOST__
//     // check root directory
//     path rd(root);
//     if (!exists(rd)) {
//       cerr << "cannot find path: " << rd.string() << endl;
//       eblerror("path does not exist");
//       return -1;
//     }
//     find_logs(rd, plots);
//     write_plots(plots, gpparams);
// #endif 
//   return 0;
// }

  } // end namespace ebl

using namespace ebl;
  int main(int argc, char **argv) {
    list<string> l;
    string var = "root", val = "rootval";
    l.push_back("name");
    l.push_back("iter");
    map<string,string> m;
    pairtree t(var, val);
    m["name"] = "train"; m["iter"] = "1"; m["toto"] = "42"; m["tata"] = "44";
    t.add(l, m);
    m["name"] = "train"; m["iter"] = "2"; m["toto"] = "42"; m["tata"] = "44";
    t.add(l, m);
    m["name"] = "train"; m["iter"] = "3"; m["toto"] = "42"; m["tata"] = "44";
    t.add(l, m);
    m["name"] = "test"; m["iter"] = "4"; m["toto"] = "42"; m["tata"] = "44";
    t.add(l, m);
    m["name"] = "test"; m["iter"] = "5"; m["toto"] = "42"; m["tata"] = "44";
    t.add(l, m);
    m["name"] = "train"; m["iter"] = "6"; m["toto"] = "42"; m["tata"] = "44";
    t.add(l, m);
    m["name"] = "train"; m["iter"] = "7"; m["toto"] = "42"; m["tata"] = "44";
    t.add(l, m);
    t.pretty();
    return 0;
  }
