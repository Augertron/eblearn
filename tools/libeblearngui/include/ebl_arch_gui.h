/***************************************************************************
 *   Copyright (C) 2009 Pierre Sermanet   *
 *   pierre.sermanet@gmail.com   *
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

#ifndef EBL_ARCH_GUI_H_
#define EBL_ARCH_GUI_H_

#include "libidxgui.h"
#include "ebl_arch.h"

#define MAXWIDTH 1000

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // module_1_1_gui

  class module_1_1_gui {
  public:
    uint	 display_wid_fprop;
    uint	 display_wid_bprop;
    uint	 display_wid_bbprop;

    module_1_1_gui() {};
    virtual ~module_1_1_gui() {};

#define DISPLAY_PROTO_1_1(name)						\
    template<typename T, class Tstate>					\
      void name(module_1_1<T,Tstate> &m,				\
		Tstate &in, Tstate &out,				\
		unsigned int h0 = 0, unsigned int w0 = 0,		\
		double dzoom = 1.0, T vmin = 0, T vmax = 0,		\
		bool show_out = true,					\
		int wid = -1, const char *wname = NULL);
    
#define DISPLAY2_PROTO_1_1(name)					\
    template<typename T, class Tstate>					\
      void name(module_1_1<T,Tstate> &m,				\
		Tstate &in, Tstate &out,				\
		unsigned int &h0, unsigned int &w0,			\
		double dzoom = 1.0, T vmin = 0, T vmax = 0,		\
		bool show_out = true,					\
		int wid = -1, const char *wname = NULL);
    
    DISPLAY_PROTO_1_1(display_fprop)
    DISPLAY2_PROTO_1_1(display_fprop2)
    DISPLAY_PROTO_1_1(display_bprop)
    DISPLAY2_PROTO_1_1(display_bprop2)
    DISPLAY_PROTO_1_1(display_bbprop)
    DISPLAY2_PROTO_1_1(display_bbprop2)    
  };

  ////////////////////////////////////////////////////////////////
  // module_2_1_gui

  class module_2_1_gui {
  private:
    uint	 display_wid_fprop;

  public:
    module_2_1_gui() {};
    virtual ~module_2_1_gui() {};

#define DISPLAY_PROTO_2_1(name)						\
    template<typename T, class Tstate, class Tin2, class Tout>		\
      void name(module_2_1<T,Tstate,Tin2,Tout> &m,			\
		Tstate &in1, Tin2 &in2, Tout &out,			\
		unsigned int &h0, unsigned int &w0,			\
		double dzoom = 1.0, T vmin = 0, T vmax = 0,		\
		bool show_out = true,					\
		int wid = -1, const char *wname = NULL);

    DISPLAY_PROTO_2_1(display_fprop)
    DISPLAY_PROTO_2_1(display_bprop)
    DISPLAY_PROTO_2_1(display_bbprop)
  };

  ////////////////////////////////////////////////////////////////
  // layers_gui

  class layers_gui {
  public:
    layers_gui() {};
    virtual ~layers_gui() {};

#define DISPLAY_PROTO_LAYERSN(name)				\
    template<typename T, class Tstate>					\
      static void name(module_1_1_gui &g, layers<T,Tstate> &ln,		\
		       Tstate &in, Tstate &out,			\
		       unsigned int &h0, unsigned int &w0,		\
		       double dzoom = 1.0,				\
		       T vmin = 0, T vmax = 0,				\
		       bool show_out = false);

    DISPLAY_PROTO_LAYERSN(display_fprop)
    DISPLAY_PROTO_LAYERSN(display_bprop)
    DISPLAY_PROTO_LAYERSN(display_bbprop)
  };

  ////////////////////////////////////////////////////////////////
  // layers_2_gui

/*   class layers_2_gui { */
/*   public: */
/*     layers_2_gui() {}; */
/*     virtual ~layers_2_gui() {}; */

/* #define DISPLAY_PROTO_LAYERS2(name)				\ */
/*     template<typename T, class Tstate>							\ */
/*       static void name(module_1_1_gui &g,				\ */
/* 		       layers_2<T,Tstate> &ln,					\ */
/* 		       Tstate &in, Tstate &out,			\ */
/* 		       unsigned int &h0, unsigned int &w0,		\ */
/* 		       double dzoom = 1.0,				\ */
/* 		       T vmin = 0, T vmax = 0,				\ */
/* 		       bool show_out = false); */

/*     DISPLAY_PROTO_LAYERS2(display_fprop) */
/*     DISPLAY_PROTO_LAYERS2(display_bprop) */
/*     DISPLAY_PROTO_LAYERS2(display_bbprop) */
/*   }; */

  ////////////////////////////////////////////////////////////////
  // fc_ebm2_gui

  class fc_ebm2_gui {
  public:
    fc_ebm2_gui() {};
    virtual ~fc_ebm2_gui() {};
    
#define DISPLAY_PROTO_FCEBM2(name)					\
    template<typename T, class Tin1, class Tin2, class Ten>		\
      static void name(fc_ebm2<T, Tin1, Tin2, Ten> &fc,			\
		       Tin1 &i1, Tin2 &i2,				\
		       Ten &energy,					\
		       unsigned int &h0, unsigned int &w0,		\
		       double zoom, T vmin = 0, T vmax = 0,		\
		       bool show_out = true,				\
		       int wid = -1, const char *wname = NULL);

    DISPLAY_PROTO_FCEBM2(display_fprop)
    DISPLAY_PROTO_FCEBM2(display_bprop)
    DISPLAY_PROTO_FCEBM2(display_bbprop)
  };

} // namespace ebl {

#include "ebl_arch_gui.hpp"

#endif /* EBL_ARCH_GUI_H_ */
