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
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <limits>

#include "configuration.h"

using namespace std;

namespace ebl {

  string timestamp() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    strftime (buffer,80,"%Y%m%d.%H%M%S",timeinfo);
    string s = buffer;
    return s;
  }
  
  void remove_trailing_whitespaces(string &s) {
    string::size_type pos;
  
    pos = s.find_first_not_of(' ');
    if (pos == string::npos)
      s = "";
    else {
      s = s.substr(pos);
      pos = s.find_last_not_of(' ');
      s = s.erase(pos + 1);
    }
  }

  // open file fname and put variables assignments in smap.
  // e.g. " i = 42 # comment " will yield a entry in smap
  // with "i" as first value and "42" as second value.
  bool extract_variables(const char *fname, string_map_t &smap,
			 string_map_t *meta_smap = NULL) {
    string s0, s;
    char separator = '=';
    char comment1 = '#';
    char comment2 = ';';
    string::size_type pos;
    string name, value;

    ifstream in(fname);
    if (!in) {
      cerr << "error: failed to open " << fname << endl;
      return false;
    }
    while (!in.eof()) {
      name = "";
      value = "";
      getline(in, s0);
      // remove comments first
      s = s0;
      pos = s.find(comment1);
      if (pos != string::npos) // comment found, remove it
	s = s.erase(pos);
      pos = s.find(comment2);
      if (pos != string::npos) // comment found, remove it
	s = s.erase(pos);
      if (s.size() > 0) {
	// look for separator
	pos = s.find(separator);
	if (pos != string::npos) {
	  if (pos >= s.size() - 1) {
	    cerr << "warning: variable value cannot be empty,";
	    cerr << " ignoring this line: " << s0 << endl;
	    continue ;
	  }
	  // separate in 2
	  name = s.substr(0, pos);
	  value = s.substr(pos + 1);
	  // removing trailing whitespaces
	  remove_trailing_whitespaces(name);
	  remove_trailing_whitespaces(value);
	  // forbid spaces in names
	  pos = name.find(' ');
	  if (pos != string::npos) {
	    cerr << "warning: variable name cannot contain an empty space,";
	    cerr << " ignoring this line: " << s0 << endl;
	    continue ;
	  }
	  // forbid empty values
	  if (value.size() == 0) {
	    cerr << "warning: variable value cannot be empty,";
	    cerr << " ignoring this line: " << s0 << endl;
	    continue ;
	  }
	  // forbid duplicates
	  if (smap.find(name) != smap.end()) {
	    cerr << "warning: duplicate variable name \"" << name;
	    cerr << "\", ignoring new assignment: " << s0 << endl;
	    continue ;
	  }
	  // if variable name starts with "meta_" put it in the meta conf list
	  if (meta_smap && (name.compare(0, 5, "meta_") == 0))
	    (*meta_smap)[name] = value;
	  else {
	    // add variable to map
	    smap[name] = value;
	  }
	}
      }
    }
    in.close();
    return true;
  }

  void variables_to_variables_list(string_map_t &smap,
				   string_list_map_t &lmap) {
    string::size_type pos;
    string s, stmp;
    string_map_t::iterator smi = smap.begin();
    for ( ; smi != smap.end(); ++smi) {
      s = smi->second;
      vector<string> &vs = lmap[smi->first];
      // loop over list of elements
      pos = s.find_first_of(' ');
      while (pos != string::npos) {
	stmp = s.substr(0, pos);
	vs.push_back(stmp);
	s = s.substr(pos);
	remove_trailing_whitespaces(s);
	pos = s.find(' ');
      }
      vs.push_back(s);
    }
  }

  int config_combinations(string_list_map_t &lmap) {
    int combinations = 1;
    string_list_map_t::iterator lmi = lmap.begin();
    for ( ; lmi != lmap.end(); ++lmi) {
      combinations *= lmi->second.size();
    }
    return combinations;
  }

  // increment the conf_indices by 1 when there is room (based the size of the
  // lmap lists).
  bool config_indices_incr(vector<size_t> &conf_indices,
			   string_list_map_t &lmap) {
    // find first index that can be increased (less that list size)
    string_list_map_t::iterator lmi = lmap.begin();
    vector<size_t>::iterator ci_incr = conf_indices.begin();
    for ( ; lmi != lmap.end(); ++lmi, ++ci_incr) {
      if (*ci_incr < lmi->second.size() - 1) { // did not reach end of list yet
	// we found the first index that can be increased.
	// increase it and clear all preceding indices
	(*ci_incr)++; // increment index
	vector<size_t>::iterator ci_prev = conf_indices.begin();
	for ( ; ci_prev != ci_incr; ++ci_prev)
	  *ci_prev = 0;
	return true; // we incremented the configuration indices by 1, return
      }
    }
    return false;
  }

  string get_conf_name(vector<size_t> &conf_indices, string_list_map_t &lmap,
		       int combination_id) {
    string_list_map_t::iterator lmi = lmap.begin();
    vector<size_t>::iterator ci = conf_indices.begin();
    ostringstream name;
  
    name << "conf" << setfill('0') << setw(2) << combination_id;
    for ( ; lmi != lmap.end(); ++lmi, ++ci)
      if (lmi->second.size() > 1) {
	name << "_" << lmi->first << "_" << lmi->second[*ci];
      }
    return name.str();
  }

  void print_conf(vector<size_t> &conf_indices, string_list_map_t &lmap) {
    string_list_map_t::iterator lmi = lmap.begin();
    vector<size_t>::iterator ci = conf_indices.begin();
  
    for ( ; lmi != lmap.end(); ++lmi, ++ci)
      cout << lmi->first << " = " << lmi->second[*ci] << endl;
  }

  void print_string_map(string_map_t &smap) {
    string_map_t::iterator smi = smap.begin();
    for ( ; smi != smap.end(); ++smi)
      cout << smi->first << " : " << smi->second << endl;
  }

  void print_string_list_map(string_list_map_t &lmap) {
    cout << "___________________________________________________________" << endl;
    string_list_map_t::iterator lmi = lmap.begin();
    for ( ; lmi != lmap.end(); ++lmi) {
      cout << lmi->first << " : ";
      vector<string>::iterator vi = lmi->second.begin();
      for ( ; vi != lmi->second.end(); ++vi) {
	cout << *vi << ", ";
      }
      cout << endl;
    }
  }

  void assign_current_smap(string_map_t &new_smap, vector<size_t> &conf_indices,
			   string_list_map_t &lmap) {
    string_list_map_t::iterator lmi = lmap.begin();
    vector<size_t>::iterator ci = conf_indices.begin();
  
    for ( ; lmi != lmap.end(); ++lmi, ++ci)
      new_smap[lmi->first] = lmi->second[*ci];
  }

  ////////////////////////////////////////////////////////////////
  // configuration

  configuration::configuration() {
  }

  configuration::configuration(const char *filename) {
    read(filename);
  }

  configuration::configuration(string_map_t &smap_, string &name_,
			       string &output_dir_)
    : smap(smap_), name(name_), output_dir(output_dir_) {
  }

  configuration::~configuration() {
  }
  
  bool configuration::read(const char *fname) {
    // read file and extract all variables and values
    cout << "loading configuration file: " << fname << endl;
    if (!extract_variables(fname, smap))
      return false;
    cout << "loaded: " << endl;
    pretty();
    return true;
  }

  bool configuration::write(const char *fname) {
    ofstream of(fname);
    if (!of) {
      cerr << "error: failed to open " << fname << endl;
      return false;
    }
    string_map_t::iterator smi = smap.begin();
    for ( ; smi != smap.end(); ++smi)
      of << smi->first << " = " << smi->second << endl;
    of.close();
    return true;
  }
  
  const string &configuration::get_name() {
    return name;
  }

  const string &configuration::get_output_dir() {
    return output_dir;
  }

  const string &configuration::get_string(const char *varname) {
    if (smap.find(varname) == smap.end()) {
      cerr << "error: unknown variable: " << varname << endl;
      throw "unknown variable";
    }
    return smap[varname];
  }

  double configuration::get_double(const char *varname) {
    if (smap.find(varname) == smap.end()) {
      cerr << "error: unknown variable: " << varname << endl;
      throw "unknown variable";
    }
    istringstream iss(smap[varname], istringstream::in);
    // TODO: check double conversion validity with exceptions instead
    double d;
    d = numeric_limits<double>::max();
    iss >> d;
    if (d == numeric_limits<double>::max())
      throw "invalid conversion to double";
    return d;
  }

  uint configuration::get_uint(const char *varname) {
    if (smap.find(varname) == smap.end()) {
      cerr << "error: unknown variable: " << varname << endl;
      throw "unknown variable";
    }
    istringstream iss(smap[varname], istringstream::in);
    // TODO: check double conversion validity with exceptions instead
    uint d;
    d = numeric_limits<double>::max();
    iss >> d;
    if (d == numeric_limits<double>::max())
      throw "invalid conversion to double";
    return d;
  }

  void configuration::pretty() {
    cout << "_____________________ Configuration _____________________" << endl;
    print_string_map(smap);
    cout << "_________________________________________________________" << endl;
  }
  
  ////////////////////////////////////////////////////////////////
  // meta_configuration

  meta_configuration::meta_configuration() {
  }
  
  meta_configuration::~meta_configuration() {
  }
  
  bool meta_configuration::read(const char *fname) {
    cout << "Reading meta configuration file: " << fname << endl;
    // read file and extract all variables and values
    if (!extract_variables(fname, tmpsmap, &smap))
      return false;
    cout << "loaded: " << endl;
    pretty();
    // transpose values into list of values (a variable can be assigned a list
    // of values
    variables_to_variables_list(tmpsmap, lmap);
    // count number of possible configurations
    conf_combinations = config_combinations(lmap);

    // name of entire experiment
    name = timestamp();
    name += ".";
    if (smap.find("meta_name") != smap.end())
      name += smap["meta_name"];
    // name of output directory
    output_dir = "output"; // default name (other name optional)
    if (smap.find("meta_output_dir") != smap.end())
      output_dir = smap["meta_output_dir"];
    mkdir(output_dir.c_str(), 0700);
    output_dir += "/";
    output_dir += name;
    mkdir(output_dir.c_str(), 0700);
    cout << "Creating " << conf_combinations << " different combinations in ";
    cout << output_dir << endl;
    
    // create all possible configurations
    conf_indices.assign(lmap.size(), 0); // reset conf
    for (int i = 0; i < conf_combinations; ++i) {
      string conf_name = name;
      conf_name += "_";
      conf_name += get_conf_name(conf_indices, lmap, i);
      string_map_t new_smap;
      assign_current_smap(new_smap, conf_indices, lmap);
      configuration conf(new_smap, conf_name, output_dir);
      confs.push_back(conf);
      cout << "Creating " << conf_name << endl;
      config_indices_incr(conf_indices, lmap); // incr conf
    }
    return true;
  }

  vector<configuration>& meta_configuration::configurations() {
    return confs;
  }

  void meta_configuration::pretty() {
    cout << "__________________ Meta configuration ___________________" << endl;
    print_string_map(smap);
    cout << "_________________________________________________________" << endl;
  }

} // namespace ebl
