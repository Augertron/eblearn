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
#include <stdio.h>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>

#include "meta_jobs.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // job

  job::job(configuration &conf_) : conf(conf_) {
    // create directories 
    mkdir(conf.get_output_dir().c_str(), 0700); // create output_dir
    string job_dir = conf.get_output_dir();
    job_dir += "/";
    job_dir += conf.get_name();
    mkdir(job_dir.c_str(), 0700); // create job_dir
    // create configuration file
    string conf_dir = job_dir;
    conf_dir += "/";
    conf_dir += "config";
    conf.write(conf_dir.c_str());
  }

  job::~job() {
  }

  ////////////////////////////////////////////////////////////////
  // job manager

  job_manager::job_manager() {
  }
  
  job_manager::~job_manager() {
  }

  bool job_manager::read_metaconf(const char *fname) {
    // read meta configuration
    if (!mconf.read(fname))
      return false;
    // create job list from all possible configurations
    vector<configuration> &confs = mconf.configurations();
    vector<configuration>::iterator iconf = confs.begin();
    for ( ; iconf != confs.end(); ++iconf) {
      jobs.push_back(*iconf);
    }
    return true;
  }

} // namespace ebl
