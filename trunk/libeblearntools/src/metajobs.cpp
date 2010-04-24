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
#include <unistd.h>
#include <iomanip>

#ifdef __BOOST__
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
using namespace boost::filesystem;
using namespace boost;
#endif

#include "metajobs.h"
#include "utils.h"
#include "tools_utils.h"
#include "metaparser.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // job

  job::job(configuration &conf_, const string &exe_, const string &oconffname) 
    : conf(conf_), exe(exe_), oconffname_(oconffname), classesname_(""), pid(-1) {
    // remove quotes around executable command if present
    if ((exe[0] == '"') && (exe[exe.size() - 1] == '"'))
      exe = exe.substr(1, exe.size() - 2);
  }

  job::~job() {
  }

  void job::run() {
    ostringstream cmd, log, errlog;
    log << "out_" << conf.get_name() << ".log";
    errlog << "out_" << conf.get_name() << ".errlog";
    // prepare command
    cmd << "cd " << outdir_ << " && echo \"job=" << conf.get_name()
	<< " classes=" << classesname_ << " config="
	<< confname_ << "\" > "
	<< log.str() << " && ((" << exe << " " << confname_
	<< " 3>&1 1>&2 2>&3 | tee /dev/tty | tee " << errlog.str()
	<< ") 3>&1 1>&2 2>&3) >> " << log.str() << " 2>&1 && exit 0";
    // fork job
    pid = fork();
    if (pid == -1)
      eblerror("fork failed");
    if (pid == 0) { // child code
      cout << "executing: " << cmd.str() << endl;
      // execl takes over this process (and its pid)
      execl("/bin/sh", "sh", "-c", cmd.str().c_str(), (char*)NULL);
    }
  }

  bool job::write() {
    ostringstream outdir, confname, cmd, classesname, tmp;
    // create directories 
    outdir.str("");
    outdir << conf.get_output_dir() << "/" << conf.get_name();
    outdir_ = outdir.str();
    mkdir_full(outdir.str().c_str());
    // copy classes file into directory
    if (conf.exists("train") && conf.exists("root")) {
      classesname << conf.get_string("root") << "/" << conf.get_string("train");
      classesname << "_" << CLASSES_NAME << MATRIX_EXTENSION;
      cmd.str("");
      cmd << "cp " << classesname.str() << " " << outdir.str() << "/";
      tmp << conf.get_name() << "_" << CLASSES_NAME << MATRIX_EXTENSION;
      classesname_ = tmp.str();
      cmd << classesname_;
      int res = std::system(cmd.str().c_str());
      if (res < 0)
	cerr << "warning: command failed: " << cmd.str() << endl;
      else
	cout << "copied class names file: " << cmd.str() << endl;
    }
    // create configuration file
    conf.set("job_name", conf.get_name().c_str());// add config name into config
    conf.resolve();
    confname.str("");
    confname << outdir.str() << "/" << conf.get_name() << ".conf";
    confname_ = confname.str();
    if (!conf.write(confname.str().c_str()))
      return false;
    // write conf in original metaconf filename
    confname.str(""); confname << outdir.str() << "/" << oconffname_;
    if (!conf.write(confname.str().c_str()))
      return false;
    return true;
  }

  bool job::alive() {
    // if job is alive, it will receive this harmless signal
    return (kill(pid, SIGCHLD) == 0); 
  }

  pid_t job::getpid() {
    return pid;
  }

  string& job::name() {
    return confname_;
  }
  
  string job::get_root() {
    string root = conf.get_output_dir();
    root += "/";
    root +=  conf.get_name();
    return root;
  }
  
  ////////////////////////////////////////////////////////////////
  // job manager

  job_manager::job_manager() : copy_path("") {
  }
  
  job_manager::~job_manager() {
  }

  bool job_manager::read_metaconf(const char *fname) {
    mconf_fullfname = fname;
    size_t pos = mconf_fullfname.find_last_of('/');
    mconf_fname = mconf_fullfname.substr(pos == string::npos ? 0 : pos);
    // read meta configuration
    if (!mconf.read(mconf_fullfname.c_str(), false))
      return false;
    // create job list from all possible configurations
    vector<configuration> &confs = mconf.configurations();
    vector<configuration>::iterator iconf = confs.begin();
    for ( ; iconf != confs.end(); ++iconf) {
      iconf->resolve();
      jobs.push_back(job(*iconf, mconf.get_string("meta_command"),
			 mconf_fname));
    }
    return true;
  }

  void job_manager::set_copy(const string &path) {
    if (path.size()) {
      copy_path = path;
      cout << "Enabling copy into jobs folders of: " << path << endl;
    }
  }

  void job_manager::run() {
    varmaplist best; // best results
    ostringstream cmd, jobs_info;
    int maxiter = -1, maxiter_tmp;
    metaparser p;

    // write job directories and files
    for (vector<job>::iterator i = jobs.begin(); i != jobs.end(); ++i) {
      // write conf
      i->write();
      // copy bins into jobs' root 
      if (copy_path.size()) {
	cout << "Copying " << copy_path << " to " << i->get_root() << endl;
	cmd.str("");
	cmd << "cp -R " << copy_path << " " << i->get_root();
	if (std::system(cmd.str().c_str()))
	  cerr << "warning: failed to execute: " << cmd.str() << endl;      
      }
    }
    // copy metaconf into jobs' root
    cmd.str("");
    cmd << "cp " << mconf_fullfname << " " << mconf.get_output_dir();
    if (std::system(cmd.str().c_str()))
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
    // run jobs and get their pid by forking
    for (vector<job>::iterator i = jobs.begin(); i != jobs.end(); ++i)
      i->run();
    // loop until all jobs are finished
    uint swait = 30; // default wait is 30 seconds
    if (mconf.exists("meta_watch_interval"))
      swait = mconf.get_uint("meta_watch_interval");
    uint nrunning = 1;
    while (nrunning) {
      sleep(swait);
      // check if each pid responds to a harmless signal
      nrunning = 0;
      for (vector<job>::iterator i = jobs.begin(); i != jobs.end(); ++i) {
	if (i->alive())
	  nrunning++;
      }
      int status = 0;
      waitpid(-1, &status, WNOHANG); // check children status
      cout << "Jobs alive: " << nrunning << " Iteration: " << maxiter << endl;
      // get jobs info for reporting
      jobs_info.str("");
      uint j = 1;
      for (vector<job>::iterator i = jobs.begin(); i != jobs.end(); ++i, ++j) {
      	jobs_info << j << ". pid: " << i->getpid() << ", name: " << i->name()
		  << ", status: " << (i->alive() ? "alive" : "dead") << endl;
      }
      // analyze outputs if requested
      if (mconf.exists_bool("meta_analyze")) {
	maxiter_tmp = p.get_max_common_iter(mconf.get_output_dir());
	if (maxiter_tmp != maxiter) { // iteration number has changed
	  maxiter = maxiter_tmp;
	  if (mconf.exists_bool("meta_send_email")) {
	    // send reports at certain iterations
	    if (mconf.exists("meta_email_iters")) {
	      // loop over set of iterations
	      list<uint> l =
		string_to_uintlist(mconf.get_string("meta_email_iters"));
	      for (list<uint>::iterator i = l.begin(); i != l.end(); ++i) {
		if (*i == maxiter) {
		  cout << "Reached iteration " << *i << endl;
		  // analyze 
		  best = p.analyze(mconf, mconf.get_output_dir(),
				   maxiter_tmp);
		  // send report
		  p.send_report(mconf, mconf.get_output_dir(), best, maxiter,
				mconf_fullfname, jobs_info.str(), nrunning);
		}
	      }
	    } else if (mconf.exists("meta_email_period") &&
		       (maxiter % mconf.get_uint("meta_email_period") == 0)) {
	      // analyze 
	      best = p.analyze(mconf, mconf.get_output_dir(),
			       maxiter_tmp);
	      // send report
	      p.send_report(mconf, mconf.get_output_dir(), best, maxiter,
			    mconf_fullfname, jobs_info.str(), nrunning);
	    }
	  }
	}
      }
    }
    cout << "All processes are finished. Exiting." << endl;
    // email last results before exiting
    if (mconf.exists_bool("meta_analyze"))
      best = p.analyze(mconf, mconf.get_output_dir(),
		       maxiter_tmp); // parse output and get best results
    // send report
    p.send_report(mconf, mconf.get_output_dir(), best, maxiter,
		  mconf_fullfname, jobs_info.str(), nrunning);
  }

} // namespace ebl
