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

#ifndef NETCONF_H_
#define NETCONF_H_

#include "libeblearn.h"
#include "configuration.h"

namespace ebl {

  //! Create a new network based on a configuration.
  //! The configuration should at least contain these variables:
  //! 'net_type' which can contain so far 'cscscf', 'cscsc', 'cscf', etc.
  //! Other variables used are convolution and subsampling kernel sizes
  //! such as 'net_c1h', 'net_c1w', 'net_s1h', etc.
  //! See netconf.hpp for more details.
  //! 'in' and 'out' are used for memory optimization if not null, otherwise
  //! independent buffers are used in between each module (required for
  //! learning).
  //! \param in The input buffer for memory optimization.
  //! \param out The output buffer for memory optimization.
  template <typename T, class Tstate = bbstate_idx<T> >
    module_1_1<T,Tstate>* create_network(parameter<T, Tstate> &theparam,
					 configuration &conf, uint noutputs,
					 Tstate *in = NULL,
					 Tstate *out = NULL);

} // end namespace ebl

#include "netconf.hpp"

#endif /* NETCONF_H_ */
