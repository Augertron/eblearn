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
#include <iomanip>

#ifdef __WINDOWS__
#include <limits>
#endif

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

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
    string subval;
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
	subval = ivars[subvariable];
	// the var list without subvariable
	map<string,string> tmp(ivars);
	tmp.erase(subvariable);
	// subvariable list without current subvariable
	// get existing node
	bool found = false;
	map<string,pairtree,natural_less>::iterator p;
	for (p = subtree.begin(); p != subtree.end(); ++p) {
	  if (!strcmp(p->first.c_str(), subval.c_str())) {
	    found = true;
	    break ;
	  }}
	if (!found) {
	  // no node, add new one
	  pairtree t(subvariable, subval);
	  path = t.add(subvar, tmp);
	  subtree[subval] = t;
	} else {
	  path = p->second.add(subvar, tmp);
	}
	// add current node pair to path
	path[subvariable] = subval;
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

    for (map<string,pairtree,natural_less>::iterator i = subtree.begin();
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

    for (map<string,pairtree,natural_less>::iterator i = subtree.begin();
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
    for (map<string,pairtree,natural_less>::iterator i = subtree.begin();
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
      s << map_to_string2(i->second);
      s << endl;
      s << "________________________________________________________" << endl;
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
      s << endl;
      for (map<string,string>::iterator j = i->begin();
	   j != i->end(); ++j)
	s << j->first << ": " << j->second << endl;
      s << "________________________________________________________" << endl;
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
	max = std::max(max, string_to_uint(i->second));
    }
    for (map<string,pairtree,natural_less>::iterator i = subtree.begin();
	 i != subtree.end(); ++i) {
      if (subvariable == var)
	max = std::max(max, string_to_uint(i->first));
      max = std::max(max, i->second.get_max_uint(var));
    }
    return max;
  }
  
  bool pairtree::exists(const string &var) {
    for (map<string,string>::iterator i = vars.begin(); i != vars.end(); ++i) {
      if (i->first == var)
	return true;
    }
    for (map<string,pairtree,natural_less>::iterator i = subtree.begin();
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

  bool metaparser::parse_log(const string &fname, list<string> *sticky) {
    ifstream in(fname.c_str());
    string s, var, val;
    char separator = VALUE_SEPARATOR;
    string::size_type itok, stok;
    map<string,string> vars, stick;

    if (!in) {
      cerr << "warning: failed to open " << fname << endl;
      return false;
    }
    // parse all lines
    while (!in.eof()) {
      // extract all variables for this line
      getline(in, s);
      istringstream iss(s, istringstream::in);
      vars.clear(); // clear previous variables
      // keep sticky variables from previous lines in this new line
      // hierarchy keys are sticky by default, and additional sticky
      // variables are defined by 'sticky' list.
      vars.insert(stick.begin(), stick.end());
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
	// if a key, make it sticky
	if (find(hierarchy.begin(), hierarchy.end(), var) != hierarchy.end())
	  stick[var] = val;
	// if sticky, remember value
	if (sticky && find(sticky->begin(), sticky->end(), var)
	    != sticky->end())
	  stick[var] = val;
      }
      // add variables to tree, and remember the path to the leaf
      tree.add(hierarchy, vars);
    }
    in.close();
    return true;
  }

  int metaparser::get_max_iter() {
    if (!tree.exists("i"))
      return -1;
    return (int) tree.get_max_uint("i");
  }
  
  int metaparser::get_max_common_iter() {
    if (!tree.exists("i"))
      return -1;
    int minmax = (std::numeric_limits<int>::max)();
    // assuming that "job" is first level and "i" second one:
    for (map<string,pairtree,natural_less>::iterator i = 
	   tree.get_subtree().begin();
         i != tree.get_subtree().end(); ++i) {
      // for job i, find maximum i
      int max = 0;
      for (map<string,pairtree,natural_less>::iterator j = 
	     i->second.get_subtree().begin();
	   j != i->second.get_subtree().end(); ++j)
	max = std::max(max, string_to_int(j->first));
      // find minimum of the maximums
      minmax = MIN(minmax, max);
    }
    return minmax;
  }
  
  natural_varmap metaparser::best(const string &key, uint n, bool display) {
    return tree.best(key, n, display);
  }

  varmaplist metaparser::best(list<string> &keys, uint n, bool display) {
    return tree.best(keys, n, display);
  }

  void metaparser::process(const string &dir) {
    string confname, jobs_info;
    configuration conf;
    // find all configurations in non-sorted order, the meta conf
    // should be the first element.
    list<string> *confs = find_fullfiles(dir, ".*[.]conf", NULL, false);
    if (confs && confs->size() > 0) {
      confname = confs->front();
      delete confs;
      conf.read(confname.c_str(), true, false);
    } else {
      cerr << "warning: could not find a .conf file describing how to analyze "
	   << "this directory" << endl;
    }
    int iter = 0;
    varmaplist best = analyze(conf, dir, iter);
    send_report(conf, dir, best, iter, confname, jobs_info);
  }
  
  // write plot files, using gpparams as additional gnuplot parameters
  void metaparser::write_plots(const char *dir, const char *gpparams,
			       const char *gpterminal) {
    string colsep = "\t";
    string gnuplot_config1 = "clear ; set terminal ";
    gnuplot_config1 += gpterminal;
    gnuplot_config1 += "; \n";
    string gnuplot_config2 = " set output \"";
    string gnuplot_config3 = ".";
    gnuplot_config3 += gpterminal;
    gnuplot_config3 += "\" ;	plot ";
    // a map of file pointers for .plot and .p files
    map<string,ofstream*> plotfiles, pfiles;
    list<string> plist; // list p file names

    if (!tree.exists("job"))
      cerr << "warning: expected a \"job\" variable to differentiate each "
	   << "curve in the plots but not found." << endl;
    if (!tree.exists("i"))
      cerr << "warning: expected a \"i\" variable for the x-axis "
	   << "in the plots but not found." << endl;
    // we assume that the tree has been extracted using those keys in that
    // order: job, i
    // loop on each job
    uint ijob = 0;
    for (map<string,pairtree,natural_less>::iterator i = 
	   tree.get_subtree().begin();
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
	     k != j->second.end(); ++k) {
	  // try to convert value to double
	  istringstream iss(k->second);
	  double val;
	  iss >> val;
	  if (iss.fail())
	    continue ; // not a number, do not plot
	  // check that p file is open
	  if (pfiles.find(k->first) == pfiles.end()) {
	    // not open, add file
	    ostringstream fname;
	    if (dir)
	      fname << dir << "/";
	    fname << k->first << ".p";
	    ofstream *outp = new ofstream(fname.str().c_str());
	    if (!outp) {
	      cerr << "warning: failed to open " << fname.str() << endl;
	      continue ; // keep going
	    }
	    pfiles[k->first] = outp;
	    fname.str("");
	    fname << k->first << ".p";
	    plist.push_back(fname.str());
	    *outp << gnuplot_config1;
	    if (gpparams)
	      *outp << gpparams;
	    *outp << ";" << gnuplot_config2;
	    *outp << k->first << gnuplot_config3;
	  }
	  // check that plot file is open
	  if (plotfiles.find(k->first) == plotfiles.end()) {
	    // not open, add file
	    ostringstream fname;
	    if (dir)
	      fname << dir << "/";
	    fname << k->first << ".plot";
	    plotfiles[k->first] = new ofstream(fname.str().c_str());
	    if (!plotfiles[k->first]) {
	      cerr << "warning: failed to open " << fname.str() << endl;
	      continue ; // keep going
	    }
	  }
	  // for the first i only, add job plot description in p file
	  if (j == flat.begin()) {
	    ofstream &outp = *pfiles[k->first];
	    if (ijob > 0)
	      outp << ", ";
	    outp << "\"" << k->first << ".plot\" using 1:"
		 << ijob + 2 << " title \"" << job << "\" with linespoints";
	  }
	  // add this value into plot file
	  ofstream &outp = *plotfiles[k->first];
	  // first add abscissa value i
	  outp << ival << colsep;
	  // then fill with empty values until reaching job's column
	  for (uint c = 0; c < ijob; ++c)
	    outp << "?" << colsep;
	  // finally add job's value
	  outp << val << endl;
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
    // convert all plots to pdf using gnuplot
    ostringstream cmd;
    for (list<string>::iterator i = plist.begin(); i != plist.end(); ++i) {
      cout << "Creating plot from " << *i << endl;
      cmd.str("");
      cmd << "cd " << dir << " && cat " << *i << " | gnuplot && sleep .1";
      int ret = std::system(cmd.str().c_str());
      if (ret < 0)
	cerr << "warning: command failed" << endl;
    }
    if (plist.size() <= 0)
      cerr << "Warning: no plots created." << endl;
  }

  void metaparser::parse_logs(const string &root, list<string> *sticky) {
    cout << "Parsing all .log files recursively..." << endl;
    list<string> *fl = find_fullfiles(root, ".*[.]log");
    if (fl) {
      for (list<string>::iterator i = fl->begin(); i != fl->end(); ++i) {
	cout << "Parsing " << *i << endl;
	parse_log(*i, sticky);
      }
      delete fl;
    }
  }

  int metaparser::get_max_common_iter(const string &dir) {
    parse_logs(dir);
    return get_max_common_iter();
  }
  
  varmaplist metaparser::analyze(configuration &conf, const string &dir,
				int &maxiter) {
    list<string> sticky;
    varmaplist best;
    string gpparams = "";
    if (conf.exists("meta_gnuplot_params"))
      gpparams = conf.get_string("meta_gnuplot_params");
    if (conf.exists("meta_sticky_vars")) {
      sticky = string_to_stringlist(conf.get_string("meta_sticky_vars"));
      cout << "Sticky variables: " << stringlist_to_string(sticky) << endl;
    }
    parse_logs(dir, &sticky);
    if (conf.exists("meta_gnuplot_terminal"))
      write_plots(dir.c_str(), gpparams.c_str(),
		  conf.get_cstring("meta_gnuplot_terminal"));
    else
      write_plots(dir.c_str(), gpparams.c_str());
    maxiter = get_max_common_iter();
    if (!conf.exists("meta_minimize"))
      cerr << "Warning: meta_minimize not defined, not attempting to determine"
	   << " variables minimum." << endl;
    else {
      list<string> keys =
	string_to_stringlist(conf.get_string("meta_minimize"));
      best =
	tree.best(keys, std::max((uint) 1, conf.get_uint("meta_send_best")));
      ostringstream dirbest, tmpdir, cmd;
      string job;
      int ret;
    
      // save best weights
      dirbest << dir << "/best";
      cmd << "rm -Rf " << dirbest.str(); // remove previous best
      ret = std::system(cmd.str().c_str());
      mkdir_full(dirbest.str().c_str());
      uint j = 1;
      for (varmaplist::iterator i = best.begin(); i != best.end(); ++i, ++j) {
	tmpdir.str("");
	tmpdir << dirbest.str() << "/" << setfill('0') << setw(2) << j << "/";
	mkdir_full(tmpdir.str().c_str());
	// look for conf filename to save
	if (i->find("config") != i->end()) { // found config
	  cmd.str("");
	  cmd << "cp " << i->find("config")->second << " " << tmpdir.str();
	  ret = std::system(cmd.str().c_str());
	}
	// find job name
	if (i->find("job") == i->end()) // not found, continue
	  continue ; // can't do anything without job name
	else
	  job = i->find("job")->second;
	// look for classes filename to save
	if (i->find("classes") != i->end()) { // found classes
	  cmd.str("");
	  cmd << "cp " << dir << "/" << job << "/"
	      << i->find("classes")->second << " " << tmpdir.str();
	  ret = std::system(cmd.str().c_str());
	}
	// save out log
	cmd.str("");
	cmd << "cp " << dir << "/" << job << "/"
	    << "out_" << job << ".log " << tmpdir.str();
	ret = std::system(cmd.str().c_str());
	// look for weights filename to save
	if (i->find("saved") != i->end()) { // found weights
	  cmd.str("");
	  cmd << "cp " << dir << "/" << job << "/"
	      << i->find("saved")->second << " " << tmpdir.str();
	  ret = std::system(cmd.str().c_str());
	  // add weights filename into configuration
#ifdef __BOOST__
	  if ((i->find("config") != i->end()) &&
	      (i->find("saved") != i->end())) {
	    path p(i->find("config")->second);
	    cmd.str("");
	    cmd << "echo \"weights_file=" << i->find("saved")->second 
		<< " # variable added by metarun\n\" >> "
		<< tmpdir.str() << "/" << p.leaf();
	    ret = std::system(cmd.str().c_str());
	  }
#endif
	}
      }
      // tar all best files
      tar(dirbest.str(), dir);
    }
    return best;
  }
  
  void metaparser::send_report(configuration &conf, const string dir,
			       varmaplist &best, int maxiter,
			       string conf_fullfname, string jobs_info,
			       uint nrunning, double maxminutes,
			       double minminutes) {
    ostringstream cmd;
    string tmpfile = "report.tmp";
    int res;
 
    if (conf.exists_bool("meta_send_email")) {
      if (!conf.exists("meta_email")) {
	cerr << "undefined email address, not sending report." << endl;
	return ;
      }
      // write body of email
      cmd.str("");
      cmd << "rm -f " << tmpfile; // remove tmp file first
      res = std::system(cmd.str().c_str());
      // print summary infos
      cmd.str("");
      cmd << "echo \"Iteration: " << maxiter << endl;
      cmd << "Jobs running: " << nrunning << endl;
      cmd << "Min running time: " << minminutes << " mins ("
	  << minminutes / 60 << " hours) (" << minminutes / (60 * 24)
	  << " days)" << endl;
      cmd << "Max running time: " << maxminutes << " mins ("
	  << maxminutes / 60 << " hours) (" << minminutes / (60 * 24)
	  << " days)" << endl;
      cout << cmd.str();
      cmd << "\" >> " << tmpfile;
      res = std::system(cmd.str().c_str());
      // print best results
      if (best.size() > 0) {
	list<string> keys =
	  string_to_stringlist(conf.get_string("meta_minimize"));
	cmd.str("");
	cmd << "echo \"Best " << best.size() << " results at iteration " 
	    << maxiter << ":" << endl;
	cmd << pairtree::flat_to_string(&best, &keys) << "\"";
	res = std::system(cmd.str().c_str()); // print on screen
	cmd << " >> " << tmpfile;
	res = std::system(cmd.str().c_str());
      }
      // print err logs
      list<string> *errlogs = find_fullfiles(dir, ".*[.]errlog");
      if (errlogs) {
	cmd.str("");
	cmd << "echo \"Errors:\"";
	cmd << " >> " << tmpfile;
	res = std::system(cmd.str().c_str());
	for (list<string>::iterator ierr = errlogs->begin();
	     ierr != errlogs->end(); ++ierr) {
	  cmd.str("");
	  cmd << "echo \"\n" << *ierr << ":\"";
	  cmd << " >> " << tmpfile;
	  res = std::system(cmd.str().c_str());
	  cmd.str("");
	  cmd << "cat " << *ierr << " >> " << tmpfile;
	  res = std::system(cmd.str().c_str());
	}
      }
      // print jobs infos
      cmd.str("");
      cmd << "echo \"\nJobs running: " << nrunning << endl;
      cmd << jobs_info << endl;
      cout << cmd.str();
      cmd << "\" >> " << tmpfile;
      res = std::system(cmd.str().c_str());
      // print metaconf
      cmd.str("");
      cmd << "echo \"\nMeta Configuration:\"";
      cmd << " >> " << tmpfile;
      res = std::system(cmd.str().c_str());
      cmd.str("");
      cmd << "cat " << conf_fullfname << " >> " << tmpfile;
      res = std::system(cmd.str().c_str());
      // create command
      cmd.str("");
      cmd << "cat " << tmpfile << " | mutt " << conf.get_string("meta_email");
      // subject of email
      cmd << " -s \"MetaRun " << conf.get_name() << "\"";
      // attach files
      if (best.size() > 0)
	cmd << " -a " << dir << "/best.tgz"; 
      // attach logs
      if (tar_pattern(dir, dir, "logs.tgz", ".*[.]log"))
	cmd << " -a " << dir << "/logs.tgz";
      // attach plots
      list<string> *plots = find_fullfiles(dir, ".*[.]pdf");
      if (plots) {
	for (list<string>::iterator i = plots->begin(); i != plots->end(); ++i)
	  cmd << " -a " << *i;
	delete plots;
      }
      // send email
      cout << "Sending email report to " << conf.get_string("meta_email")
	   << ":" << endl;
      cout << cmd.str() << endl;
      res = std::system(cmd.str().c_str());
    }
  }
  
} // end namespace ebl
