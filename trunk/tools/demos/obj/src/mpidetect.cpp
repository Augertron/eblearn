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

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include "libeblearn.h"
#include "libeblearntools.h"
#include <stdio.h>

#ifdef __MPI__
#include <mpi.h>
#endif

#ifndef __WINDOWS__
#include <fenv.h>
#endif

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

#ifdef __DEBUGMEM__
  INIT_DEBUGMEM()
#endif

typedef float t_net; // network precision

//////////////////////////////////////////////////////////////////////////
// serialization functions

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/mpi.hpp>

//namespace mpi = boost::mpi;

namespace boost {
  namespace serialization {
    
    template<class Archive>
    void serialize(Archive & ar, idx<ubyte>& mat, const unsigned int version) {
      intg d1, d2, d3;
      if (Archive::is_saving::value) { // saving to stream
	if (!mat.contiguousp())
	  eblerror("expected contiguous idx for serialization");
	if (mat.order() != 3)
	  eblerror("no support for idx order != 3 for now, got: " << mat);

	d1 = mat.dim(0); 
	d2 = mat.dim(1); 
	d3 = mat.dim(2); 
	
	ar & d1;
	ar & d2;
	ar & d3;
	idx_aloop1(m, mat, ubyte) {
	  ar & *m;
	}
      } else { // loading from stream
	ar & d1;
	ar & d2;
	ar & d3;
	idx<ubyte> m(d1, d2, d3);
	ubyte b;
	idx_aloop1(mm, m, ubyte) {
	  ar & b; // get ubyte
	  *mm = b;
	}
	mat = m; // assign new idx
      }
    }
    
    template<class Archive>
    void serialize(Archive & ar, bbox& b, const unsigned int version) {
      ar & b.class_id;
      ar & b.confidence;
      ar & b.h0;
      ar & b.w0;
      ar & b.height;
      ar & b.width;
      // we dont' really care about other members here
    }
    // template<class Archive>
    // void serialize(Archive & ar, bbox*& b, const unsigned int version) {
    //   if (b == NULL)
    // 	b = new bbox();
    //   ar & b->class_id;
    //   ar & b->confidence;
    //   ar & b->h0;
    //   ar & b->w0;
    //   ar & b->height;
    //   ar & b->width;
    //   // we dont' really care about other members here
    // }
    
  } // namespace serialization
} // namespace boost

//////////////////////////////////////////////////////////////////////////
// comm functions

enum tag { cmd_finished = 0, cmd_get_data = 1 };

// blocking-ask if thread 'rank' has finished.
bool ask_thread_finished(boost::mpi::communicator &world, int rank) {
  bool finished = 0;
  world.send(rank, cmd_finished); // send command (no data)
  world.recv(rank, cmd_finished, finished); // get bool
  return finished;
}

// non-blocking check if question was asked, and blocking-answer it.
void answer_thread_finished(boost::mpi::communicator &world, int rank,
			    bool finished) {
  // check if we received a question, non blocking
  boost::mpi::request r = world.irecv(rank, cmd_finished);
  // check if the comm went through
  if (!r.test())
    return ; // no cmd received
  // we received the command, send the answer
  world.send(rank, cmd_finished, finished);
}

// blocking-ask for new data, return false if no new data is available.
bool ask_get_data(boost::mpi::communicator &world, int rank, vector<bbox*> &bb,
		  idx<ubyte> &frame, uint &total_saved, string &frame_name,
		  uint &frame_id) {
  bool new_data = 0;
  world.send(rank, cmd_get_data);
  world.recv(rank, cmd_get_data, new_data);
  if (!new_data)
    return false; // no new data, stop here
  // get the new data
  world.recv(rank, cmd_get_data, bb);
  world.recv(rank, cmd_get_data, frame);
  world.recv(rank, cmd_get_data, total_saved);
  world.recv(rank, cmd_get_data, frame_name);
  world.recv(rank, cmd_get_data, frame_id);
  return true;
}

// non-blocking check if question was asked, then blocking send data.
void answer_get_data(boost::mpi::communicator &world, int rank,
		     detection_thread<t_net> &dt) {
  // data variables
  idx<ubyte> frame;
  string frame_name;
  uint frame_id;
  vector<bbox*> bb;
  uint total_saved;
  // check if we received a question, non blocking
  boost::mpi::request r = world.irecv(rank, cmd_get_data);
  // check if the comm went through
  if (!r.test())
    return ; // no cmd received
  // we got the question, answer it
  bool new_data = dt.get_data(bb, frame, total_saved, frame_name, frame_id);
  world.send(rank, cmd_get_data, new_data);
  if (!new_data)
    return ; // no new data, stop here
  // send the new data
  world.send(rank, cmd_get_data, bb);
  world.send(rank, cmd_get_data, frame);
  world.send(rank, cmd_get_data, total_saved);
  world.send(rank, cmd_get_data, frame_name);
  world.send(rank, cmd_get_data, frame_id);
}

// blocking-ask to send data.
void ask_set_data(boost::mpi::communicator &world, int rank, idx<ubyte> &frame,
		  string &frame_name, uint frame_id) {
}

// non-blocking check if question was asked, then blocking get data into thread.
// return true if data was retrieved.
bool answer_set_data(boost::mpi::communicator &world, int rank, idx<ubyte> &frame,
		     string &frame_name, uint frame_id) {
  return true;
}

// blocking-ask if thread is available for new data
bool ask_available(boost::mpi::communicator &world, int rank) {
  return true;
}

// non-blocking check if question was asked, and blocking-answer it.
void answer_ask_available(boost::mpi::communicator &world, int rank, bool available) {
}

// blocking-ask the thread to stop
void ask_ask_stop(boost::mpi::communicator &world, int rank) {
}

// non-blocking check if command was sent, return true if thread was asked to
// stop.
bool check_ask_stop(boost::mpi::communicator &world) {
}

//////////////////////////////////////////////////////////////////////////
// main thread

void main_thread(int argc, char **argv, configuration &conf,
		 boost::mpi::communicator &world) {
  // config
  ostream &mout = cout;
  ostream &merr = cerr;
  uint          ipp_cores		  = 1;
  if (conf.exists("ipp_cores")) ipp_cores = conf.get_uint("ipp_cores");
  ipp_init(ipp_cores);		// limit IPP (if available) to 1 core
  bool		save_video		  = conf.exists_true("save_video");
  string	cam_type		  = conf.get_string("camera");
  int		height			  = conf.get_int("input_height");
  int		width			  = conf.get_int("input_width");
  bool          input_random		  = conf.exists_true("input_random");
  uint          npasses			  = 1;
  bool              silent        = conf.exists_true("silent");
  if (conf.exists("input_npasses"))
    npasses = conf.get_uint("input_npasses");
  // outputs
  string outdir = "out_";
  outdir += tstamp();
  outdir += "/";
  cout << "Saving outputs to " << outdir << endl;
  string viddir;
  if (save_video) {
    viddir << outdir << "video/";
    mkdir_full(viddir);
  }
  // save conf to output dir
  string cname = outdir;
  cname << filename(argv[1]);
  if (conf.write(cname.c_str()))
    mout << "Wrote configuration to " << cname << endl;
  // bbox saving
  bboxes boxes(conf.exists("bbox_saving") ?
	       (t_bbox_saving) conf.get_int("bbox_saving") : bbox_all,
	       &outdir, mout, merr);

  int nprocs = world.size();
  int nthreads = nprocs - 1;
  mout << "Thread M: Initialized " << nthreads << " detection threads." << endl;
    
  // initialize camera (opencv, directory, shmem or video)
  idx<ubyte> frame;
  camera<ubyte> *cam = NULL, *cam2 = NULL;
  if (!strcmp(cam_type.c_str(), "directory")) {
    if (argc >= 3) // read input dir from command line
      cam = new camera_directory<ubyte>(argv[2], height, width,
					input_random, npasses, mout, merr);
    else if (conf.exists("input_dir")) // read input dir from conf
      cam = new camera_directory<ubyte>(conf.get_cstring("input_dir"), 
					height, width, input_random,
					npasses, mout, merr);
    else eblerror("expected 2nd argument");
  } else if (!strcmp(cam_type.c_str(), "opencv"))
    cam = new camera_opencv<ubyte>(-1, height, width);
#ifdef __LINUX__
  else if (!strcmp(cam_type.c_str(), "v4l2"))
    cam = new camera_v4l2<ubyte>(conf.get_cstring("device"),
				 height, width);
#endif
  else if (!strcmp(cam_type.c_str(), "shmem"))
    cam = new camera_shmem<ubyte>("shared-mem", height, width);
  else if (!strcmp(cam_type.c_str(), "video")) {
    if (argc >= 3)
      cam = new camera_video<ubyte>
	(argv[2], height, width, conf.get_uint("input_video_sstep"),
	 conf.get_uint("input_video_max_duration"));
    else eblerror("expected 2nd argument");
  } else eblerror("unknown camera type, set \"camera\" in your .conf");

  // answer variables & initializations
  vector<bbox*> bb;

  // gui
#ifdef __GUI__
  bool display	     = conf.exists_bool("display");
  bool show_parts        = conf.exists_true("show_parts");
  // mindisplay     = conf.exists_bool("minimal_display");
  // display_sleep  = conf.get_uint("display_sleep");
  // display_states = conf.exists_bool("display_states");
  uint qstep1 = 0, qheight1 = 0, qwidth1 = 0,
    qheight2 = 0, qwidth2 = 0, qstep2 = 0;
  if (conf.exists_bool("queue1")) {
    qstep1 = conf.get_uint("qstep1");
    qheight1 = conf.get_uint("qheight1");
    qwidth1 = conf.get_uint("qwidth1"); }
  if (conf.exists_bool("queue2")) {
    qstep2 = conf.get_uint("qstep2");
    qheight2 = conf.get_uint("qheight2");
    qwidth2 = conf.get_uint("qwidth2"); }
  // wid_states  = display_states ? new_window("network states"):0;
  // night_mode();
  uint wid  = display ? new_window("eblearn object recognition") : 0;
  night_mode();
#endif  
  // timing variables
  timer tpass, toverall, tstop;
  uint cnt = 0;
  mout << "i=" << cnt << endl;
  bool stop = false, finished = false, updated = false;
  idx<ubyte> detframe; // frame returned by detection thread
  idx<uint> total_saved(nthreads);
  idx_clear(total_saved);
  
  // loop
  toverall.start();
  while (!finished) {
    // check for results and send new image for each thread
    uint i = 0;
    finished = true;
    for (int rank = 1; rank < nprocs; ++rank) {
      // do nothing if thread is finished already
      if (ask_thread_finished(world, rank))
	continue ;
      finished = false; // a thread is not finished
      string processed_fname;
      uint processed_id = 0;
      // retrieve new data if present
      updated = ask_get_data(world, rank, bb, detframe, 
			     *(total_saved.idx_ptr() + rank - 1),
			     processed_fname, processed_id);
      // save bounding boxes
      if (updated) {
	updated = false;
	boxes.new_group(&processed_fname, processed_id);
	boxes.add(bb);
	cnt++;
	// display processed frame
// #ifdef __GUI__
// 	if (display) {
// 	  select_window(wid);
// 	  disable_window_updates();
// 	  clear_resize_window();
// 	  detector_gui<t_net>::
// 	    display_minimal(detframe, bb, 
// 			    ((*ithreads)->pdetect ?
// 			     (*ithreads)->pdetect->labels : classes),
// 			    0, 0, 1, 0, 255,wid, show_parts);
// 	  enable_window_updates();
// 	  if (save_video && display) {
// 	    string fname;
// 	    fname << viddir << processed_fname;
// 	    save_window(fname.c_str());
// 	    if (!silent) mout << "saved " << fname << endl;
// 	  }
// 	}
// #endif
	  // output info
	if (!silent) {
	  mout << "total_saved=" << idx_sum(total_saved);
	  if (conf.exists("save_max"))
	    mout << " / " << conf.get_uint("save_max");
	  mout << endl;
	  mout << " remaining=" << (cam->size() - cnt);
	  mout << " elapsed=" << toverall.elapsed();
	  if (cam->size() > 0) {
	    mout << " ETA=" << toverall.
	      elapsed((long)((cam->size() - cnt) * 
			     (toverall.elapsed_seconds() 
			      /(float)std::max((uint)1,cnt))));
	  }
	  if (conf.exists("save_max")) {
	    uint total = idx_sum(total_saved);
	    mout << " save_max_ETA="
		 << toverall.
	      elapsed((long)((conf.get_uint("save_max") - total)
			     * (toverall.elapsed_seconds() 
				/ (float)std::max((uint)1,total))));
	  }
	  mout << endl;
	}
	mout << "i=" << cnt << " processing: " << tpass.elapsed_ms()
	     << " fps: " << cam->fps() << endl;
      }
      // check if ready
      if (ask_available(world, rank)) {
	if (stop)
	  ask_ask_stop(world, rank); // stop but let thread finish
	else {
	  // grab a new frame if available
	  if (cam->empty()) {
	    stop = true;
	    tstop.start(); // start countdown timer
	    ask_ask_stop(world, rank); // ask this thread to stop
	    millisleep(50);
	  } else {
	    // if the pre-camera is defined use it until empty
	    if (cam2 && !cam2->empty())
	      frame = cam2->grab();
	    else // empty pre-camera, use regular camera
	      frame = cam->grab();
	    // send new frame to this thread
	    string frame_name = cam->frame_name();
	    ask_set_data(world, rank, frame, frame_name, cnt);
	    // we just sent a new frame
	    tpass.restart();
	  }
	}
      }
    }
    // sleep display
    // 	if (display_sleep > 0) {
    // 	  mout << "sleeping for " << display_sleep << "ms." << endl;
    // 	  millisleep(display_sleep);
    // 	}
    if (conf.exists("save_max") && !stop &&
	idx_sum(total_saved) > conf.get_uint("save_max")) {
      mout << "Reached max number of detections, exiting." << endl;
      stop = true; // limit number of detection saves
      tstop.start(); // start countdown timer
    }
    // sleep a bit between each iteration
    millisleep(5);
    // check if stop countdown reached 0
    if (stop && tstop.elapsed_minutes() >= 5) {
      cerr << "threads did not all return 5 min after request, stopping"
	   << endl;
      break ; // program too long to stop, force exit
    }
  }
  // saving boxes
  boxes.save();
  mout << "Execution time: " << toverall.elapsed() << endl;
  if (save_video)
    cam->stop_recording(conf.exists_bool("use_original_fps") ?
			cam->fps() : conf.get_uint("save_video_fps"),
			outdir.c_str());
  // free variables
  if (cam) delete cam;
  mout << "Detection finished." << endl;
}

//////////////////////////////////////////////////////////////////////////
// child_thread

void child_thread(configuration &conf, boost::mpi::communicator &world) {
  // data variables
  idx<ubyte> frame;
  string frame_name;
  uint frame_id;
  vector<bbox*> bb;
  uint total_saved;
  
  // start thread
  detection_thread<t_net> dt(conf);
  dt.start();
  
  // loop to receive/send messages
  while (1) {
    // check for new commands (non blocking) and answer them (blocking)
    // answer if thread is finished
    answer_thread_finished(world, 0, dt.finished());
    // send data back to main thread
    answer_get_data(world, 0, dt);
    // receive new data
    if (answer_set_data(world, 0, frame, frame_name, frame_id)) {
      // try to set data until successful (meaning we got the mutex lock)
      while (!dt.set_data(frame, frame_name, frame_id))
	millisleep(5);
    }
    // answer if thread is available to process new data
    answer_ask_available(world, 0, dt.available());
    // check if we were asked to stop
    if (check_ask_stop(world)) {
      dt.ask_stop();
      return ; // quit loop
    }
    // sleep a little bit.
    millisleep(20);
  }
}

//////////////////////////////////////////////////////////////////////////
// main

#ifdef __GUI__
MAIN_QTHREAD(int, argc, char **, argv) { // macro to enable multithreaded gui
#else
int main(int argc, char **argv) { // regular main without gui
#endif
#ifndef __MPI__
  eblerror("MPI was not found during compilation, install and recompile");
#else
  try {
    // check input parameters
    if ((argc != 2) && (argc != 3) ) {
      cerr << "warning: wrong number of parameters." << endl;
      cerr << "usage: mtdetect <config file> [directory or file]" << endl;
      //	return -1;
    }
#ifdef __LINUX__
    feenableexcept(FE_DIVBYZERO | FE_INVALID); // enable float exceptions
#endif
    // MPI vars
    boost::mpi::environment env(argc, argv);
    boost::mpi::communicator world;
    int myid = world.rank();    
    
    // load configuration
    configuration	conf(argv[1]);
    if (!conf.exists("root2")) {
      string dir = dirname(argv[1]);
      cout << "Looking for trained files in: " << dir << endl;
      conf.set("root2", dir.c_str());
      conf.resolve();
    }
    uint              ipp_cores     = 1;
    if (conf.exists("ipp_cores")) ipp_cores = conf.get_uint("ipp_cores");
    ipp_init(ipp_cores); // limit IPP (if available) to 1 core
  
    // At this point, all programs are running equivalently, the rank
    // distinguishes the roles of the programs in the SPMD model,
    // with rank 0 often used specially...    
    if (myid == 0)
      main_thread(argc, argv, conf, world);
    else
      child_thread(conf, world);
  } eblcatch();
#endif
  return 0;
}
