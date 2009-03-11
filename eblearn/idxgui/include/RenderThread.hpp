/***************************************************************************
 *   Copyright (C) 2008 by Yann LeCun and Pierre Sermanet *
 *   yann@cs.nyu.edu, pierre.sermanet@gmail.com *
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

#ifndef RENDERTHREAD_HPP_
#define RENDERTHREAD_HPP_

using namespace std;

namespace ebl {
  
  template<class T>
  void RenderThread::draw_matrix(Idx<T> &im, unsigned int h0, unsigned int w0, 
				 T minv, T maxv, double zoomw, double zoomh) {
    Idx<ubyte> *uim = new Idx<ubyte>(grey_image_to_ubyte<T>(im, minv, maxv, 
							    zoomw, zoomh));
    // send image to main gui thread
    emit gui_drawImage(uim, h0, w0);
  }

  template<class T>
  void RenderThread::draw_matrix_frame(Idx<T> &im, ubyte r, ubyte g, ubyte b,
				       unsigned int h0, unsigned int w0, 
				       T minv, T maxv, 
				       double zoomw, double zoomh) {
    Idx<ubyte> uim = grey_image_to_ubyte<T>(im, minv, maxv, zoomw, zoomh);
    Idx<ubyte> tmp(uim.dim(0) + 2, uim.dim(1) + 2);
    Idx<ubyte> *fim = new Idx<ubyte>(tmp);
    Idx<ubyte> tmp2 = tmp.narrow(0, uim.dim(0), 1);
    tmp2 = tmp2.narrow(1, uim.dim(1), 1);
    idx_copy(uim, tmp2);
    tmp2 = tmp.narrow(0, 1, 0); idx_fill(tmp2, r);
    tmp2 = tmp.narrow(0, 1, tmp.dim(0) - 2); idx_fill(tmp2, r);
    tmp2 = tmp.narrow(1, 1, 0); idx_fill(tmp2, r);
    tmp2 = tmp.narrow(1, 1, tmp.dim(1) - 2); idx_fill(tmp2, r);
    // send image to main gui thread
    emit gui_drawImage(fim, h0, w0);
  }

  template<class T1, class T2>
  class ManipInfra {
  public:
    ManipInfra(RenderThread& (*pFun) (RenderThread&))
      : manipFun0(pFun), val1(0), val2(0), nval(0) {}
    ManipInfra(RenderThread& (*pFun) (RenderThread&, T1), T1 val1_)
      : manipFun1(pFun), val1(val1_), val2(0), nval(1) {}
    ManipInfra(RenderThread& (*pFun) (RenderThread&, T1, T2), 
	       T1 val1_, T2 val2_)
      : manipFun2(pFun), val1(val1_), val2(val2_), nval(2) {}

    void operator() (RenderThread& r) const {
      switch (nval) {
      case 0: manipFun0(r); break ;
      case 1: manipFun1(r, val1); break ;
      case 2: manipFun2(r, val1, val2); break ;
      default: ylerror("unknown mode");
      }
    }
  private:
    RenderThread& (*manipFun0) (RenderThread&);
    RenderThread& (*manipFun1) (RenderThread&, T1);
    RenderThread& (*manipFun2) (RenderThread&, T1, T2);
    T1 val1;
    T2 val2;
    int nval;
  };

  RenderThread& att(RenderThread& r, unsigned int h0, unsigned int w0);
  ManipInfra<unsigned int, unsigned int> at(unsigned int h0, unsigned int w0);

  RenderThread& fcout_and_gui(RenderThread& r);
  ManipInfra<int, int> cout_and_gui();

  RenderThread& fgui_only(RenderThread& r);
  ManipInfra<int, int> gui_only();

  template<class T1, class T2> 
  RenderThread& operator<<(RenderThread& r, const ManipInfra<T1, T2> &manip) {
    manip(r);
    return r;
  }

  template<class T> 
  RenderThread& operator<<(RenderThread& r, const T val) {
    ostringstream o;
    o << val;
    r.add_text(new std::string(o.str()));
    if (r.cout_output)
      cout << o.str();
    return r;
  }

} // end namespace ebl

#endif /* RENDERTHREAD_HPP_ */
