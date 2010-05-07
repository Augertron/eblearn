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

#ifndef NETCONF_HPP_
#define NETCONF_HPP_

namespace ebl {

  // select network based on configuration
  template <typename T>
  module_1_1<T>* create_network(parameter<T> &theparam,
				configuration &conf, uint noutputs) {
    string net_type = conf.get_string("net_type");
    if (!strcmp(net_type.c_str(), "cscscf")) {
      return (module_1_1<T>*) new lenet<T>
	(theparam, conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
	 conf.get_uint("net_c1h"), conf.get_uint("net_c1w"),
	 conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
	 conf.get_uint("net_c2h"), conf.get_uint("net_c2w"),
	 conf.get_uint("net_s2h"), conf.get_uint("net_s2w"),
	 conf.get_uint("net_full"), noutputs,
	 conf.get_bool("absnorm"), conf.get_bool("color"),
	 conf.get_bool("mirror"));
    } else if (!strcmp(net_type.c_str(), "cscsc")) {
      return (module_1_1<T>*) new lenet_cscsc<T>
	(theparam, conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
	 conf.get_uint("net_c1h"), conf.get_uint("net_c1w"),
	 conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
	 conf.get_uint("net_c2h"), conf.get_uint("net_c2w"),
	 conf.get_uint("net_s2h"), conf.get_uint("net_s2w"),
	 noutputs, conf.get_bool("absnorm"), conf.get_bool("color"),
	 conf.get_bool("mirror"));
    } else if (!strcmp(net_type.c_str(), "cscf")) {
      return (module_1_1<T>*) new lenet_cscf<T>
	(theparam, conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
	 conf.get_uint("net_c1h"), conf.get_uint("net_c1w"),
	 conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
	 conf.get_uint("net_c2h"), conf.get_uint("net_c2w"),
	 noutputs, conf.get_bool("absnorm"), conf.get_bool("color"),
	 conf.get_bool("mirror"));
    } else {
      cerr << "network type: " << net_type << endl;
      eblerror("unknown network type");
    }
    return NULL;
  }

} // end namespace ebl

#endif /* NETCONF_HPP_ */
