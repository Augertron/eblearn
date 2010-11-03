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

#ifndef METAJOBS_H_
#define METAJOBS_H_

#include "configuration.h"
#include "metaparser.h"
#include "utils.h"

#include <sstream>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // job
  
  //! Jobs to be executed.
  class job {
  public:
    job(configuration &conf, const string &exe, const string &oconffname);
    virtual ~job();

    //! Execute job
    void run();

    //! Write job's files in configuration's output directory.
    bool write();

    //! Return true if the process is alive.
    bool alive();

    //! Return pid of this job.
    pid_t getpid();

    //! Return the name of this job (its configuration filename).
    string &name();

    //! Return root directory of this job.
    string get_root();

    //! Return job's running time in minutes.
    double minutes();
    
    ////////////////////////////////////////////////////////////////
    // members
  private:
    configuration	conf;
    string		exe;	//!< executable full path
    string		outdir_;	//!< job's output directory
    string		confname_;	//!< job's configuration filename
    string		oconffname_;	//!< job's original conf filename
    pid_t		pid;	//!< pid of this job
    string              classesname_; //!< filename of classes matrix
    timer               t;
  };

  ////////////////////////////////////////////////////////////////
  // job manager

  //! A class to manage jobs.
  class job_manager {
  public:
    //! Constructor.
    job_manager();

    //! Destructor.
    virtual ~job_manager();

    //! Read meta configuration.
    //! @param tstamp An optional timestamp to be used for the job's name
    //!               instead of the current timestamp.
    bool read_metaconf(const char *fname, const string *tstamp = NULL);

    //! Enable recursive copy of this path into jobs folders.
    void set_copy(const string &path);
    
    //! Run all jobs.
    void run();

    ////////////////////////////////////////////////////////////////
    // members
  private:
    meta_configuration	mconf; //!< Meta configuration
    string		mconf_fullfname;	//!< Full filename of metaconf
    string		mconf_fname;	//!< Filename of metaconf
    vector<job>		jobs; //!< A vector of jobs to run
    string              copy_path; //!< Copy path to jobs folders.
  };

} // end namespace ebl

#endif /* METAJOBS_H_ */