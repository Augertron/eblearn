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

  job::job(configuration &conf_, const string &exe_) : conf(conf_), exe(exe_) {
  }

  job::~job() {
  }

  void job::run() {
    ostringstream cmd;
    cmd << "cd " << outdir << " && " << exe << " " << outdir << "/config";
    cmd << " > " << outdir << "/out_" << conf.get_name() << ".log &";
    cout << "executing: " << cmd.str() << endl;
    if (!system(cmd.str().c_str()))
      cerr << "error executing: " << cmd.str() << endl;
  }

  bool job::write() {
    // create directories 
    mkdir(conf.get_output_dir().c_str(), 0700); // create output_dir
    outdir = conf.get_output_dir();
    outdir += "/";
    outdir += conf.get_name();
    mkdir(outdir.c_str(), 0700); // create job_dir
    // create configuration file
    string conf_dir = outdir;
    conf_dir += "/";
    conf_dir += "config";
    if (!conf.write(conf_dir.c_str()))
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
    // read meta configuration
    mconf_fname = fname;
    if (!mconf.read(fname))
      return false;
    // create job list from all possible configurations
    vector<configuration> &confs = mconf.configurations();
    vector<configuration>::iterator iconf = confs.begin();
    for ( ; iconf != confs.end(); ++iconf) {
      jobs.push_back(job(*iconf, mconf.get_string("meta_command")));
    }
    return true;
  }

  void job_manager::run() {
    // write job directories and files
    for (vector<job>::iterator i = jobs.begin(); i != jobs.end(); ++i)
      i->write();
    // copy metaconf into jobs' root
    ostringstream cmd;
    cmd << "cp " << mconf_fname << " " << mconf.get_output_dir();
    if (!system(cmd.str().c_str()))
      cerr << "warning: failed to execute: " << cmd.str() << endl;
    // run jobs
    for (vector<job>::iterator i = jobs.begin(); i != jobs.end(); ++i)
      i->run();
  }

} // namespace ebl
