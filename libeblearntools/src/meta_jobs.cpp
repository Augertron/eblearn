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
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

#include "meta_jobs.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // job

  job::job(configuration &conf_, const string &exe_, const string &oconffname) 
    : conf(conf_), exe(exe_), oconffname_(oconffname) {
    // remove quotes around executable command if present
    if ((exe[0] == '"') && (exe[exe.size() - 1] == '"'))
      exe = exe.substr(1, exe.size() - 2);
  }

  job::~job() {
  }

  void job::run() {
    ostringstream cmd;
    cmd << "cd " << outdir_ << " && ((" << exe << " " << confname_;
    cmd << " 3>&1 1>&2 2>&3 | tee /dev/tty) 3>&1 1>&2 2>&3) > ";
    //    cmd << outdir_ << "/";
    cmd << "out_" << conf.get_name() << ".log 2>&1 &";
    cout << "executing: " << cmd.str() << endl;
    if (system(cmd.str().c_str()))
      cerr << "error executing: " << cmd.str() << endl;
  }

  bool job::write() {
    ostringstream outdir, confname, cmd, classesname;
    // create directories 
    outdir.str("");
    outdir << conf.get_output_dir() << "/" << conf.get_name();
    outdir_ = outdir.str();
    cmd << "mkdir -p " << outdir.str();
    int res; res = system(cmd.str().c_str());
    // copy classes file into directory
    if (conf.exists("train") && conf.exists("root")) {
      classesname << conf.get_string("root") << "/" << conf.get_string("train");
      classesname << "_" << CLASSES_NAME << MATRIX_EXTENSION;
      cmd.str("");
      cmd << "cp " << classesname.str() << " " << outdir.str() << "/";
      cmd << conf.get_name() << "_" << CLASSES_NAME << MATRIX_EXTENSION;
      res = system(cmd.str().c_str());
      cout << "copying class names file: " << cmd.str() << endl;
    }
    // create configuration file
    conf.set("job_name", conf.get_name().c_str()); // add config name into config
    conf.resolve();
    confname.str(""); confname << outdir.str() << "/" << conf.get_name() << ".conf";
    confname_ = confname.str();
    if (!conf.write(confname.str().c_str()))
      return false;
    // write conf in original metaconf filename
    confname.str(""); confname << outdir.str() << "/" << oconffname_;
    if (!conf.write(confname.str().c_str()))
      return false;
    return true;
  }

  ////////////////////////////////////////////////////////////////
  // job manager

  job_manager::job_manager() {
  }
  
  job_manager::~job_manager() {
  }

  bool job_manager::read_metaconf(const char *fname) {
    mconf_fname = fname;
    size_t pos = mconf_fname.find_last_of('/');
    string name = mconf_fname.substr(pos == string::npos ? 0 : pos);
    // read meta configuration
    if (!mconf.read(fname, false))
      return false;
    // create job list from all possible configurations
    vector<configuration> &confs = mconf.configurations();
    vector<configuration>::iterator iconf = confs.begin();
    for ( ; iconf != confs.end(); ++iconf) {
      iconf->resolve();
      jobs.push_back(job(*iconf, mconf.get_string("meta_command"), name));
    }
    return true;
  }

  void job_manager::run() {
    ostringstream cmd;
    // write job directories and files
    for (vector<job>::iterator i = jobs.begin(); i != jobs.end(); ++i) {
      // write conf
      i->write();
    }
    // copy metaconf into jobs' root
    cmd.str(""); cmd << "cp " << mconf_fname << " " << mconf.get_output_dir();
    if (system(cmd.str().c_str()))
      cerr << "warning: failed to execute: " << cmd.str() << endl;
    // create gnuplot param file in jobs' root
    try {
      if (mconf.exists("meta_gnuplot_params")) {
	string params = mconf.get_string("meta_gnuplot_params");
	ostringstream gpp;
	gpp << mconf.get_output_dir() << "/" << "gnuplot_params.txt";
	ofstream of(gpp.str().c_str());
	if (!of) {
	  cerr << "warning: failed to write gnuplot parameters to ";
	  cerr << gpp.str() << endl;
	} else {
	  of << params;
	  of.close();
	}
      }
    } catch (const char *s) { cerr << s << endl; }
    // run jobs
    for (vector<job>::iterator i = jobs.begin(); i != jobs.end(); ++i)
      i->run();
  }

} // namespace ebl
