/***************************************************************************
 *   Copyright (C) 2011 by Pierre Sermanet   *
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

#ifndef MPIJOB_H_
#define MPIJOB_H_

#ifdef __MPI__
#include <mpi.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/mpi.hpp>
#endif

#include "job.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // mpijob manager

  //! A class to manage jobs, that can be ran over a cluster using MPI.
  class EXPORT mpijob_manager : public job_manager {
  public:
    //! Constructor.
    mpijob_manager();

    //! Destructor.
    virtual ~mpijob_manager();

    //! Run all jobs.
    void run();

  protected:

    //! Run the master manager, which creates all initial files and
    //! configuration and controls jobs assignments to slaves managers.
    void run_master();

    //! Run a slave manager, which takes orders for the master manager.
    void run_slave();

    ////////////////////////////////////////////////////////////////
    // members
  protected:
#ifdef __MPI__
    //   boost::mpi::communicator world;
#endif
    int rank;
  };

} // end namespace ebl

#endif /* MPIJOB_H_ */
