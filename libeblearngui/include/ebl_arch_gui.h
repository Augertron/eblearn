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

namespace ebl {

  ////////////////////////////////////////////////////////////////
  // module_1_1_gui

  class module_1_1_gui {
  public:
    int	 display_wid_fprop;
    int	 display_wid_bprop;

    module_1_1_gui() {};
    virtual ~module_1_1_gui() {};

    template<class Tin, class Tout>
      void display_fprop(module_1_1<Tin, Tout> &m, Tin &in, Tout &out,
			 unsigned int h0 = 0, unsigned int w0 = 0, 
			 double dzoom = 1.0, double vmin=0.0, double vmax=0.0,
			 bool show_out = true,
			 int wid = -1, const char *wname = NULL);
    
    template<class Tin, class Tout>
      void display_fprop2(module_1_1<Tin, Tout> &m, Tin &in, Tout &out,
			  unsigned int &h0, unsigned int &w0, 
			  double dzoom = 1.0, double vmin=0.0, double vmax=0.0,
			  bool show_out = true,
			  int wid = -1, const char *wname = NULL);

    template<class Tin, class Tout>
      void display_bprop(module_1_1<Tin, Tout> &m, Tin &in, Tout &out,
			 unsigned int h0 = 0, unsigned int w0 = 0, 
			 double dzoom = 1.0, double vmin=0.0, double vmax=0.0,
			 bool show_in = true,
			 int wid = -1, const char *wname = NULL);

    template<class Tin, class Tout>
      void display_bprop2(module_1_1<Tin, Tout> &m, Tin &in, Tout &out,
			  unsigned int &h0, unsigned int &w0, 
			  double dzoom = 1.0, double vmin=0.0, double vmax=0.0,
			  bool show_int = true,
			  int wid = -1, const char *wname = NULL);
    
  };

  ////////////////////////////////////////////////////////////////
  // module_2_1_gui

  class module_2_1_gui {
  private:
    unsigned int	 display_wid_fprop;

  public:
    module_2_1_gui() {};
    virtual ~module_2_1_gui() {};

    template<class Tin1, class Tin2, class Tout>
      void display_fprop(module_2_1<Tin1, Tin2, Tout> &m, 
			 Tin1 &in1, Tin2 &in2, Tout &out,
			 unsigned int &h0, unsigned int &w0, 
			 double dzoom = 1.0, double vmin=0.0, double vmax=0.0,
			 bool show_out = true,
			 int wid = -1, const char *wname = NULL);
    
    template<class Tin1, class Tin2, class Tout>
      void display_bprop(module_2_1<Tin1, Tin2, Tout> &m, 
			 Tin1 &in1, Tin2 &in2, Tout &out,
			 unsigned int &h0, unsigned int &w0, 
			 double dzoom = 1.0, double vmin=0.0, double vmax=0.0,
			 bool show_out = true,
			 int wid = -1, const char *wname = NULL);
  };

  ////////////////////////////////////////////////////////////////
  // layers_n_gui

  class layers_n_gui {
  public:
    layers_n_gui() {};
    virtual ~layers_n_gui() {};
    
    template<class T> 
      static void display_fprop(module_1_1_gui &g, layers_n<T> &ln, 
				T &in, T &out,
				unsigned int &h0, unsigned int &w0, 
				double dzoom = 1.0,
				double vmin=0.0, double vmax=0.0,
				bool show_out = false);

    template<class T> 
      static void display_bprop(module_1_1_gui &g, layers_n<T> &ln, 
				T &in, T &out,
				unsigned int &h0, unsigned int &w0, 
				double dzoom = 1.0,
				double vmin=0.0, double vmax=0.0,
				bool show_out = false);
  };

  ////////////////////////////////////////////////////////////////
  // fc_ebm2_gui

  class fc_ebm2_gui {
  public:
    fc_ebm2_gui() {};
    virtual ~fc_ebm2_gui() {};
    
    template<class Tin1, class Tin2, class Tout>
      static void display_fprop(fc_ebm2<Tin1, Tin2, Tout> &fc,
				Tin1 &i1, Tin2 &i2, 
				state_idx &energy, 
				unsigned int &h0, unsigned int &w0,
				double zoom, double vmin=0.0, double vmax=0.0,
				bool show_out = true,
				int wid = -1, const char *wname = NULL);
    
    template<class Tin1, class Tin2, class Tout>
      static void display_bprop(fc_ebm2<Tin1, Tin2, Tout> &fc,
				Tin1 &i1, Tin2 &i2, 
				state_idx &energy, 
				unsigned int &h0, unsigned int &w0,
				double zoom, double vmin=0.0, double vmax=0.0,
				bool show_out = true,
				int wid = -1, const char *wname = NULL);
  };

} // namespace ebl {

#include "ebl_arch_gui.hpp"

#endif /* EBL_ARCH_GUI_H_ */
