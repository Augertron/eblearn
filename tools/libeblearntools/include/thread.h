/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
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

#ifndef THREAD_H_
#define THREAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "utils.h"

#ifndef __WINDOWS__
#include <pthread.h>

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // A thread class
  class thread {
  public:
    thread(const char *name = "Thread");
    virtual ~thread();
    //! Start the thread.
    int start();

    //! Tell the thread to stop working. Once thread is done,
    //! finished() will return true.
    //! \param wait If true, wait until finished() is true.
    void stop(bool wait = false);

    //! Return true if thread has finished executing.
    bool finished();

    //! Return name of this thread.
    string& name();

  protected:
    void run();
    static void* entrypoint(void *pthis);
    virtual void execute() = 0;

  protected:    
    bool 		_stop;
    string      	_name;  //! Name of this thread.
  private:
    pthread_t 		threadptr;
    bool 		_finished;
    pthread_mutex_t 	mutex1;
  };

} // end namespace ebl

#endif /* THREAD_H_ */

#endif /* __WINDOWS__ */