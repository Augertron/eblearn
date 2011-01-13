/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet *
 *   pierre.sermanet@gmail.com *
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

#include "mpijob.h"
#include "utils.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // mpijob manager

  mpijob_manager::mpijob_manager() {
#ifndef __MPI__
  eblerror("MPI was not found during compilation, install and recompile");
#else
  // boost::mpi::environment env(argc, argv);
  //  int rank = world.rank();    
#endif    
  }
  
  mpijob_manager::~mpijob_manager() {
  }
  
  void mpijob_manager::run() {
    if (rank == 0)
      run_master();
    else
      run_slave();
  }
  
  void mpijob_manager::run_master() {
    // prepare folders and files
    prepare();
    // run jobs and get their pid by forking
    for (uint i = 0; i < jobs.size() && i < max_jobs; ++i)
      jobs[i].run();
    // loop until all jobs are finished
    while (nrunning || unstarted > 0) {
      secsleep(swait);
      jobs_info();
      release_dead_children();
      // if there are jobs waiting to be started, start them if possible
      if (unstarted > 0 && ready_slots > 0) {
	for (vector<job>::iterator i = jobs.begin(); i != jobs.end(); ++i) {
	  if (!i->started() && ready_slots > 0) {
	    i->run();
	    ready_slots--;
	  }
	}
      }
      report();
    }
    last_report();
  }

  void mpijob_manager::run_slave() {
    while (1) {
      // wait for master command
      int cmd = 42;
      // execute command
      switch (cmd) {
      case -1: // stop
	return; break ;
      case 0: // run a new job
	break ;
      case 1: // tell master if we are available
	break ;
      case 2: // send master job's running time
	break ;
      default:
	cerr << "unknown master command: " << cmd << endl;
      }
    }
  }
  
} // namespace ebl
