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

#include <jni.h>
#include <android/bitmap.h>

#include "libidx.h"
#include "libeblearn.h"

using namespace ebl;

extern "C" {

  typedef float t_net;
  
  JNIEXPORT jint JNICALL
  Java_com_eblearn_eblearn_detect(JNIEnv* env, jobject obj, jobject bitmap,
				  jobject fd_sys1, jlong off1,
				  jobject fd_sys2, jlong off2,
				  jobject fd_sys3, jlong off3, jobject jbb,
				  jfloat threshold) {
    ////////////////////////////////////////////////////////////////////////////
    // read weight matrix from resources
    idx<t_net> weights;
    idx<ubyte> classnames;
    //    configuration conf;
    
    if (!fd_sys1 || !fd_sys1 || !fd_sys1) {
      eblprint( "null file descriptor: " << fd_sys1 << " " << fd_sys2
                << " " << fd_sys3 << std::endl);
      return -1;
    }
    jclass fdClass = env->FindClass("java/io/FileDescriptor");
    if (fdClass) { 
      jfieldID fid = env->GetFieldID(fdClass, "descriptor", "I");
      if (fid) {
	jint fd1 = env->GetIntField(fd_sys1, fid),
	  fd2 = env->GetIntField(fd_sys2, fid),
	  fd3 = env->GetIntField(fd_sys3, fid);
	int dupfd1 = dup(fd1), dupfd2 = dup(fd2), dupfd3 = dup(fd3);
	FILE* fp;
	// weights
	fp = fdopen(fd1, "rb"); 
	if (!fp) {
	  eblwarn("Could not open resource file descriptor " << fp << std::endl);
	  return -1; }
	fseek(fp, off1, SEEK_SET);
	weights = load_matrix<t_net>(fp);
	eblprint( "Loaded weight matrix: " << weights
	     << " (min: " << idx_min(weights)
                  << ", max: " << idx_max(weights) << ")" << std::endl);
	// class names
	fp = fdopen(fd2, "rb"); 
	if (!fp) {
	  eblwarn( "Could not open resource file descriptor " << fp << std::endl);
	  return -1; }
	fseek(fp, off2, SEEK_SET);
	classnames = load_matrix<ubyte>(fp);
	eblprint( "Loaded class names matrix: " << classnames << std::endl);
	// // configuration
	// fp = fdopen(fd3, "rb"); 
	// if (!fp) { LOGI("Could not open resource file descriptor " << fp);
	//   return -1; }
	// fseek(fp, off3, SEEK_SET);
	// weights = load_matrix<float>(fp);
	// LOGI("Loaded weight matrix: " << weights);
      } 
    }
    ////////////////////////////////////////////////////////////////////////////
    // read image
    AndroidBitmapInfo info;
    void *pixels;
    int ret;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
      eblwarn("AndroidBitmap_getInfo() failed ! error=" << ret << std::endl);
      return -1; }
    eblprint( "Image height " << info.height << " width " << info.width
              << " format: " << info.format << " stride: "
              << info.stride << std::endl);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
      LOGE("Bitmap format is not ARGB_8888 !");
      return -1;
    }
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
      eblwarn("AndroidBitmap_lockPixels() failed ! error=" << ret << std::endl);
      return -1; }
    typedef ubyte intype;
    idx<ubyte> im(info.height, info.width, 3);
    ubyte *ptr = (ubyte*)im.idx_ptr();
    intype *p = (intype*) pixels, v;
    for (size_t i = 0; i < info.height * info.width; ++i) {
      *(ptr++) = *(p++); // r
      *(ptr++) = *(p++); // g
      *(ptr++) = *(p++); // b
      p++; // a
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    eblprint( "Created idx image " << im << " (range: " << (int) idx_min(im)
              << ", " << (int) idx_max(im) << ")" << std::endl);
    // string fname = "/sdcard/nens.mat";
    // im = load_matrix<ubyte>(fname);
    //      eblerror("failed to load " << fname);
    
    ////////////////////////////////////////////////////////////////////////////
    // start detection
    uint norm_size = 7, input_max = 300;
    float scaling = 1.4, min_scale = 1.0, max_scale = 2.0;
    float bbhfactor = 1, bbwfactor = 1, bbh_overlap = .67, bbw_overlap = 0;
    float hzpad = .5, wzpad = .5;
    uint net_ih = 32, net_iw = 32;
    uint net_c1h = 5, net_c1w = 5;
    uint net_s1h = 2, net_s1w = 2;
    uint net_c2h = 5, net_c2w = 5;
    uint net_s2h = 2, net_s2w = 2;
    uint noutputs = classnames.dim(0);
    bool absnorm = true, color = false,  use_tanh = true,
      use_shrink = false, pruning = true;

    // load classes
    std::vector<std::string> sclasses;
    idx_bloop1(uu, classnames, ubyte) {
      sclasses.push_back((const char *) uu.idx_ptr());
    }

    // create answer module
    bool binary = false, btanh = false;
    float factor = 1.0;
    int force = -1, single = -1;
    idxdim kerd;
    double sigma_scale = 3;
    t_confidence tconf = confidence_max;
    answer_module<t_net, t_net, t_net> *ans = 
      new class_answer<t_net,t_net,t_net>
      (noutputs, factor, binary, (t_confidence) tconf, btanh, "class_answer", 
       force, single, &kerd, sigma_scale);

    // create preprocessing
    module_1_1<t_net> *chanmodule = NULL;
    idxdim norm_dim(norm_size,norm_size);
    bool globn = true; // global normalization
    bool mirror = DEFAULT_PP_MIRROR;
    t_norm mode = WSTD_NORM;
    double eps = NORM_EPSILON, eps2 = 0;
    if (im.dim(2) == 1) {// grayscale input
      chanmodule = new y_to_yp_module<t_net>(norm_dim, mirror, 
					      mode, globn, eps, eps2);
      eblprint("Detected Grayscale Input. Setting preprocessing to y_to_yn"
               << std::endl);
    }
    else if (im.dim(2) == 3) { // color input
      chanmodule = new rgb_to_yn_module<t_net>(norm_dim, mirror, mode, 
						globn, eps, eps2);
      eblprint("Detected Color Input. Setting preprocessing to rgb_to_yn"
               << std::endl);
    }
    else {
      eblwarn("image format not supported: " << im << std::endl);
      return -1;
    }
    module_1_1<t_net> *ppmodule = NULL;
    idxdim target_size(net_ih, net_iw);
    uint resize_type = MEAN_RESIZE;
    bool keep_aspect_ratio = true;
    ppmodule = (module_1_1<t_net>*)
      new resizepp_module<t_net>(target_size, resize_type, chanmodule, true,
                                 NULL, keep_aspect_ratio);

    // create network    
    parameter<t_net> theparam;
    // build net
    module_1_1<t_net>
      *lenet = (module_1_1<t_net>*)new lenet_cscsc<t_net>(theparam, net_ih, net_iw,
                                                        net_c1h, net_c1w, net_s1h,
                                                        net_s1w, net_c2h,
                                                        net_c2w, net_s2h, net_s2w,
                                                        noutputs, absnorm, color,
                                                        mirror, use_tanh,
                                                        use_shrink);
    // load net
    theparam.load_x(weights);

    layers<t_net>* net = new layers<t_net>(true, "face detector");
    net->add_module(ppmodule);
    net->add_module(lenet);
    // build detector
    detector<t_net> detect(*((module_1_1<t_net>*)net), sclasses, NULL, NULL);
    detect.set_resolutions(scaling, max_scale, min_scale);
    detect.set_max_resolution(input_max);
    // TODO: change pruning to nms calls
    //detect.set_pruning(pruning);
    //detect.set_bbox_factors(bbhfactor, bbwfactor);
    //detect.set_bbox_overlaps(bbh_overlap, bbw_overlap);
    detect.set_zpads(hzpad, wzpad);
    // detect.set_silent();
    // detection
    eblprint( "Detection threshold: " << threshold << std::endl);
    detect.set_outputs_threshold(threshold, -1);
    // vector<bbox*> &bb = detect.fprop(im);
    eblprint( "Detection started!" << std::endl);
    bboxes bb = detect.fprop(im);
    
    eblprint( "Detection finished" << std::endl);
    eblprint(bb.pretty_short(sclasses) << std::endl);    

    jclass cls = env->GetObjectClass(jbb);
    jmethodID mid = env->GetMethodID(cls, "add", "(FIIII)V");
    if (mid == 0) {
      eblerror("failed to get add method from JNI");
      return -1;
    }
    bbox *b;
    for (size_t i = 0; i < bb.size(); ++i) {
      b = &bb[i];
      env->CallVoidMethod(jbb, mid, b->confidence, b->h0, b->w0, b->height,
			  b->width);
    }
    
    return (jint) bb.size();
  }

}

