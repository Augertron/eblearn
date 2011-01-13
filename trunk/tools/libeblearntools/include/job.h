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

#ifndef JOB_H_
#define JOB_H_

#include "configuration.h"
#include "metaparser.h"
#include "utils.h"

#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#ifndef __WINDOWS__
#include <sys/wait.h>
#include <unistd.h>
#endif

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // job
  
  //! Jobs to be executed.
  class EXPORT job {
  public:
    job(configuration &conf, const string &exe, const string &oconffname);
    virtual ~job();

    //! Execute job (fork and call run_child()).
    virtual void run();

    //! Returns true if the job has been started, false otherwise.
    virtual bool started();

    //! Write job's files in configuration's output directory.
    virtual bool write();

    //! Return true if the process is alive.
    virtual bool alive();

    //! Return pid of this job.
    virtual int getpid();

    //! Return the name of this job (its configuration filename).
    virtual string &name();

    //! Return root directory of this job.
    virtual string get_root();

    //! Return job's running time in minutes.
    virtual double minutes();
    
  protected:

    //! Execute child's code.
    virtual void run_child();

    ////////////////////////////////////////////////////////////////
    // members
  protected:
    configuration	conf;
    string		exe;	//!< executable full path
    string		outdir_;	//!< job's output directory
    string		confname_;	//!< job's configuration filename
    string		oconffname_;	//!< job's original conf filename
    string              classesname_; //!< filename of classes matrix
    timer               t;
    bool                _started;
    int 		pid;	//!< pid of this job
  };

  ////////////////////////////////////////////////////////////////
  // job manager

  //! A class to manage jobs.
  class EXPORT job_manager {
  public:
    //! Constructor.
    job_manager();

    //! Destructor.
    virtual ~job_manager();

    //! Read meta configuration.
    //! @param tstamp An optional timestamp to be used for the job's name
    //!               instead of the current timestamp.
    virtual bool read_metaconf(const char *fname, const string *tstamp = NULL);

    //! Enable recursive copy of this path into jobs folders.
    virtual void set_copy(const string &path);
    
    //! Run all jobs.
    virtual void run();

  protected:

    //! Release child processes that have terminated from their zombie state.
    virtual void release_dead_children();
    
    //! Prepare all jobs (create folders and copy/create files).
    virtual void prepare();

    //! Gather and print information about jobs, such as number of jobs running,
    //! number to run left, min/max running time.
    virtual void jobs_info();

    //! Analyze and send a report.
    virtual void report();

    //! Print stopping message and send last report.
    virtual void last_report();

    ////////////////////////////////////////////////////////////////
    // members
  protected:
    meta_configuration	mconf;	   //!< Meta configuration
    string		mconf_fullfname;//!< Full filename of metaconf
    string		mconf_fname;	//!< Filename of metaconf
    vector<job>		jobs;	   //!< A vector of jobs to run
    string              copy_path; //!< Copy path to jobs folders.
    uint                max_jobs;  //!< Max number of jobs at the same time.
    metaparser          parser;
    int			maxiter;
    int			maxiter_tmp;
    double		mintime;
    double		maxtime;
    uint		nrunning;
    uint		unstarted;
    uint		ready_slots;
    ostringstream       infos;
    varmaplist          best; //!< best results
    uint                swait; //!< Waiting time when looping, in seconds.
  };

} // end namespace ebl

#endif /* JOB_H_ */
