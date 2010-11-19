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

  //! Get the parameters associated with module_name and variable id
  //! in configuration conf. Check the parameter exists,
  //! and return false missing, true otherwise.
  template <typename T>
  bool get_param(configuration &conf, const string &module_name,
		 const string &var_name, T &p, intg thickness, intg noutputs) {
    string pn = module_name; pn << "_" << var_name;
    // check that variable is present
    if (!conf.exists(pn)) {
      // not found
      cerr << "error: required parameter " << pn << " not found" << endl;
      return false;
    }
    std::string val_in = conf.get_string(pn);
    if (!val_in.compare("thickness"))
      p = (T) thickness; // special value
    else if (!val_in.compare("noutputs"))
      p = (T) noutputs; // special value
    else // get int value
      conf.get(p, pn);
    return true;
  }

  // select network based on configuration
  template <typename T, class Tstate>
  module_1_1<T,Tstate>* create_network(parameter<T, Tstate> &theparam,
				       configuration &conf, uint nout) {
    try {
      // if we don't find the generic architecture variable, try the old style
      // way with 'net-type'
      if (!conf.exists("arch"))
	return create_network_old(theparam, conf, nout);
      // else, use the arch list
      list<string> arch = string_to_stringlist(conf.get_string("arch"));
      uint arch_size = arch.size();
      layers<T,Tstate>* l = new layers<T,Tstate>(true); // true -> owns modules
      intg thick = -1; // to be updated by modules modifying the thickness
      cout << "Creating a network with " << nout << " outputs and "
	   << arch_size << " modules: " << conf.get_string("arch") << endl;
      // loop over each module
      for (uint i = 0; i < arch_size; ++i) {
	cout << i << ": ";
	// get first module name of the list and remove it from list
	string name = arch.front(); arch.pop_front();
	string type = strip_num(name);
	module_1_1<T,Tstate> *module = NULL;
	// switch on each possible type of module
	// convolution /////////////////////////////////////////////////////////
	if (!type.compare("conv")) {
	  intg kerh, kerw, strideh, stridew;
	  idx<intg> table(1, 1);
	  if (!get_param(conf, name, "kerh", kerh, thick, nout)) continue ;
	  if (!get_param(conf, name, "kerw", kerw, thick, nout)) continue ;
	  if (!get_param(conf, name, "strideh", strideh, thick, nout)) continue;
	  if (!get_param(conf, name, "stridew", stridew, thick, nout)) continue;
	  if (!load_table(conf, name, table, thick, nout)) continue ;
	  // update thickness
	  idx<intg> tblmax = table.select(1, 1);
	  thick = 1 + idx_max(tblmax);
	  // create module
	  module = (module_1_1<T,Tstate>*)
	    new convolution_module_replicable<T,Tstate>
	    (&theparam, kerh, kerw, strideh, stridew, table, name.c_str());
	}
	// convolution layer ///////////////////////////////////////////////////
	else if (!type.compare("convl")) {
	  intg kerh, kerw, strideh, stridew;
	  idx<intg> table(1, 1);
	  if (!get_param(conf, name, "kerh", kerh, thick, nout)) continue ;
	  if (!get_param(conf, name, "kerw", kerw, thick, nout)) continue ;
	  if (!get_param(conf, name, "strideh", strideh, thick, nout)) continue;
	  if (!get_param(conf, name, "stridew", stridew, thick, nout)) continue;
	  if (!load_table(conf, name, table, thick, nout)) continue ;
	  // update thickness
	  idx<intg> tblmax = table.select(1, 1);
	  thick = 1 + idx_max(tblmax);
	  // create module
	  module = (module_1_1<T,Tstate>*)
	    new convolution_layer<T,Tstate>
	    (&theparam, kerh, kerw, strideh, stridew, table,
	     true /* tanh */, name.c_str());
	}
	// subsampling ///////////////////////////////////////////////////////
	else if (!type.compare("subs")) {
	  intg kerh, kerw, strideh, stridew;
	  if (!get_param(conf, name, "kerh", kerh, thick, nout)) continue ;
	  if (!get_param(conf, name, "kerw", kerw, thick, nout)) continue ;
	  if (!get_param(conf, name, "strideh", strideh, thick, nout)) continue;
	  if (!get_param(conf, name, "stridew", stridew, thick, nout)) continue;
	  // create module
	  module = (module_1_1<T,Tstate>*)
	    new subsampling_module_replicable<T,Tstate>
	    (&theparam, strideh, stridew, kerh, kerw, thick, name.c_str());
	}
	// subsampling layer ///////////////////////////////////////////////////
	else if (!type.compare("subsl")) {
	  intg kerh, kerw, strideh, stridew;
	  if (!get_param(conf, name, "kerh", kerh, thick, nout)) continue ;
	  if (!get_param(conf, name, "kerw", kerw, thick, nout)) continue ;
	  if (!get_param(conf, name, "strideh", strideh, thick, nout)) continue;
	  if (!get_param(conf, name, "stridew", stridew, thick, nout)) continue;
	  // create module
	  module = (module_1_1<T,Tstate>*)
	    new subsampling_layer<T,Tstate>
	    (&theparam, strideh, stridew, kerh, kerw, thick, true,name.c_str());
	}
	// linear //////////////////////////////////////////////////////////////
	else if (!type.compare("linear")) {
	  intg in, out;
	  if (!get_param(conf, name, "in", in, thick, nout)) continue ;
	  if (!get_param(conf, name, "out", out, thick, nout)) continue ;
	  // create module
	  module = (module_1_1<T,Tstate>*)
	    new linear_module_replicable<T,Tstate>
	    (&theparam, in, out, name.c_str());
	}
	// addc //////////////////////////////////////////////////////////////
	else if (!type.compare("addc"))
	  module = (module_1_1<T,Tstate>*) new addc_module<T,Tstate>
	    (&theparam, thick, name.c_str());
	// diag //////////////////////////////////////////////////////////////
	else if (!type.compare("diag"))
	  module = (module_1_1<T,Tstate>*) new diag_module<T,Tstate>
	    (&theparam, thick, name.c_str());
	// wstd //////////////////////////////////////////////////////////////
	else if (!type.compare("wstd")) {
	  intg kerh, kerw;
	  if (!get_param(conf, name, "kerh", kerh, thick, nout)) continue ;
	  if (!get_param(conf, name, "kerw", kerw, thick, nout)) continue ;
	  module = (module_1_1<T,Tstate>*) new weighted_std_module<T,Tstate>
	    (kerh, kerw, thick, name.c_str(), conf.exists_true("mirror"),
	     true, false);
	}
	// tanh ///////////////////////////////////////////////////////////////
	else if (!type.compare("tanh"))
	  module = (module_1_1<T,Tstate>*) new tanh_module<T,Tstate>();
	// stdsig //////////////////////////////////////////////////////////////
	else if (!type.compare("stdsig"))
	  module = (module_1_1<T,Tstate>*) new stdsigmoid_module<T,Tstate>();
	// abs //////////////////////////////////////////////////////////////
	else if (!type.compare("abs"))
	  module = (module_1_1<T,Tstate>*) new abs_module<T,Tstate>();
	else
	  cout << "unknown module type " << type << endl;
	// add module to layers, if not null
	if (module) {
	  l->add_module(module);
	  cout << "Added " << module->describe() << endl;
	}
      }
      if (arch_size != l->size())
	eblerror("Some error occurred when loading modules");
      cout << "Loaded " << l->size() << " modules." << endl;
      return l;
    } eblcatch();
  }

  // select network based on configuration, using old-style variables
  template <typename T, class Tstate>
  module_1_1<T,Tstate>* create_network_old(parameter<T, Tstate> &theparam,
					   configuration &conf, uint noutputs) {
    string net_type = conf.get_string("net_type");
    // load custom tables if defined
    string mname;
    idx<intg> t0(1,1), t1(1,1), t2(1,1),
      *table0 = NULL, *table1 = NULL, *table2 = NULL;
    intg thick = -1;
    mname = "conv0";
    if (load_table(conf, mname, t0, thick, noutputs))
      table0 = &t0;
    mname = "conv1";
    if (load_table(conf, mname, t1, thick, noutputs))
      table1 = &t1;
    mname = "conv2";
    if (load_table(conf, mname, t2, thick, noutputs))
      table2 = &t2;
    // create networks
    // cscscf ////////////////////////////////////////////////////////////////
    if (!strcmp(net_type.c_str(), "cscscf")) {
      return (module_1_1<T,Tstate>*) new lenet<T,Tstate>
	(theparam, conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
	 conf.get_uint("net_c1h"), conf.get_uint("net_c1w"),
	 conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
	 conf.get_uint("net_c2h"), conf.get_uint("net_c2w"),
	 conf.get_uint("net_s2h"), conf.get_uint("net_s2w"),
	 conf.get_uint("net_full"), noutputs,
	 conf.get_bool("absnorm"), conf.get_bool("color"),
	 conf.get_bool("mirror"), conf.get_bool("use_tanh"),
	 conf.exists_true("use_shrink"), conf.exists_true("use_diag"),
	 table0, table1, table2);
    // cscsc ////////////////////////////////////////////////////////////////
    } else if (!strcmp(net_type.c_str(), "cscsc")) {
      return (module_1_1<T,Tstate>*) new lenet_cscsc<T,Tstate>
	(theparam, conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
	 conf.get_uint("net_c1h"), conf.get_uint("net_c1w"),
	 conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
	 conf.get_uint("net_c2h"), conf.get_uint("net_c2w"),
	 conf.get_uint("net_s2h"), conf.get_uint("net_s2w"),
	 noutputs, conf.get_bool("absnorm"), conf.get_bool("color"),
	 conf.get_bool("mirror"), conf.get_bool("use_tanh"),
	 conf.exists_true("use_shrink"), conf.exists_true("use_diag"),
	 conf.exists_true("norm_pos"), table0, table1, table2);
    // cscf ////////////////////////////////////////////////////////////////
    } else if (!strcmp(net_type.c_str(), "cscf")) {
      return (module_1_1<T,Tstate>*) new lenet_cscf<T,Tstate>
	(theparam, conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
	 conf.get_uint("net_c1h"), conf.get_uint("net_c1w"),
	 conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
	 conf.get_uint("net_c2h"), conf.get_uint("net_c2w"),
	 noutputs, conf.get_bool("absnorm"), conf.get_bool("color"),
	 conf.get_bool("mirror"), conf.get_bool("use_tanh"),
	 conf.exists_true("use_shrink"), conf.exists_true("use_diag"), 
	 table0, table1);
    // cscc ////////////////////////////////////////////////////////////////
    } else if (!strcmp(net_type.c_str(), "cscc")) {
      if (!table0 || !table1 || !table2) 
	eblerror("undefined connection tables");
      return (module_1_1<T,Tstate>*) new net_cscc<T,Tstate>
	(theparam, conf.get_uint("net_ih"), conf.get_uint("net_iw"), 
	 conf.get_uint("net_c1h"), conf.get_uint("net_c1w"), *table0,
	 conf.get_uint("net_s1h"), conf.get_uint("net_s1w"),
	 conf.get_uint("net_c2h"), conf.get_uint("net_c2w"), *table1,
	 *table2, noutputs, conf.get_bool("absnorm"),
	 conf.get_bool("mirror"), conf.get_bool("use_tanh"),
	 conf.exists_true("use_shrink"), conf.exists_true("use_diag"));
    } else {
      cerr << "network type: " << net_type << endl;
      eblerror("unknown network type");
    }
    return NULL;
  }
  
  //! Try to cast module and load it with weights matrix found in filename.
  template <class Tmodule, typename T, class Tstate>
  bool load_module(configuration &conf, module_1_1<T,Tstate> &m,
		   const string &module_name, const string &type) {
    if (!dynamic_cast<Tmodule*>(&m))
      eblerror("cannot cast module " << module_name << " (\"" << m.name() 
	       << "\") into a " << type << " type");
    string name = module_name; name << "_weights";
    if (!conf.exists(name))
      return false; // do nothing if variable not found
    string filename = conf.get_string(name.c_str());
    idx<T> w = load_matrix<T>(filename);
    m.load_x(w);
    cout << "Loaded weights " << w << " into " << module_name << " (\""
	 << m.name() << "\") from " << filename << endl;
    return true;
  }

  // select network based on configuration
  template <typename T, class Tstate>
  void manually_load_network(layers<T,Tstate> &l, configuration &conf) {
    list<string> arch = string_to_stringlist(conf.get_string("arch"));
    uint arch_size = arch.size();
    cout << "Loading network manually using module list: "
	 << conf.get_string("arch") << endl;
    uint n = 0;
    // loop over each module
    for (uint i = 0; i < arch_size; ++i) {
      // get first module name of the list and remove it from list
      string name = arch.front(); arch.pop_front();
      string type = strip_num(name);
      module_1_1<T,Tstate> *m = (*l.modules)[i];
      // switch on each possible type of module
      if (!type.compare("conv"))
	n += load_module<convolution_module_replicable<T,Tstate>,T,Tstate>
	  (conf, *m, name, type);
      else if (!type.compare("addc"))
	n += load_module<addc_module<T,Tstate>,T,Tstate>(conf, *m, name, type);
      else if (!type.compare("diag"))
	n += load_module<diag_module<T,Tstate>,T,Tstate>(conf, *m, name, type);
      }
    cout << "Loaded " << n << " weights." << endl;
  }
  
} // end namespace ebl

#endif /* NETCONF_HPP_ */
