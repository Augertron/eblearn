/***************************************************************************
 *   Copyright (C) 2010 by Pierre Sermanet *
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

#ifndef DETECTION_THREAD_H_
#define DETECTION_THREAD_H_

// Windows does not know linux/mac mutexes, TODO: implement windows mutexes
#ifndef __WINDOWS__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "thread.h"
#include "netconf.h"
#include "configuration.h"

using namespace std;

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // A detection thread class

  template <typename Tnet>
  class detection_thread : public thread {
  public:
    detection_thread(configuration &conf, const char *name = "",
		     const char *arg2 = NULL);
    ~detection_thread();
    
    //! Execute the detection thread.
    virtual void execute();
    //! Return true if new data was copied to the thread, false otherwise.
    virtual bool set_data(idx<ubyte> &frame, string &frame_name);
    //! Return true if new data was copied from the thread, false otherwise.
    //! We get the frame back even though it was set via set_data,
    //! because we do not know which frame was actually used.
    //! (could use some kind of id, and remember frames to avoid copy).
    virtual bool get_data(vector<bbox*> &bboxes, idx<ubyte> &frame,
			  uint &total_saved, string &frame_name);
    //! Return true if the thread is available to process a new frame, false
    //! otherwise.
    virtual bool available();
    //! Set the directory where to write outputs.
    virtual void set_output_directory(string &out);
    
  private:
    //! Copy passed bounding boxes into bboxes class member
    //! (allocating new 'bbox' objects).
    void copy_bboxes(vector<bbox*> &bb);
    //! Turn 'out_updated' flag on, so that other threads know we just outputed
    //! new data.
    void set_out_updated();

    ////////////////////////////////////////////////////////////////
    // private members
  private:
    configuration		&conf;
    const char			*arg2;
    idx<ubyte>			 uframe;
    idx<Tnet>			 frame;
    pthread_mutex_t		 mutex_in;	// mutex for thread input
    pthread_mutex_t		 mutex_out;	// mutex for thread output
    vector<bbox*>		 bboxes;
    vector<bbox*>::iterator	 ibox;
    bool			 in_updated;	// thread input updated
    bool			 out_updated;	// thread output updated
    bool                         bavailable;    // thread is available
    string                       frame_name;    // name of current frame
    string                       outdir;        // output directory
    uint                         total_saved;
  };

} // end namespace ebl

#include "detection_thread.hpp"

#endif /* DETECTION_THREAD_H_ */

#endif /* __WINDOWS__ */
