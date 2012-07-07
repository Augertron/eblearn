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
  bool get_param2(configuration &conf, const string &module_name,
		  const string &var_name, T &p, intg thickness,
		  intg noutputs) {
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

  //! Get the parameters associated with module_name and variable id
  //! in configuration conf. Check the parameter exists,
  //! and return false missing, true otherwise.
  template <typename T>
  bool get_param(configuration &conf, const string &module_name,
		 const string &var_name, T &p, bool optional = false) {
    string pn = module_name; pn << "_" << var_name;
    // check that variable is present
    if (!conf.exists(pn)) {
      // not found
      if (!optional)
	cerr << "error: required parameter " << pn << " not found" << endl;
      return false;
    }
    std::string val_in = conf.get_string(pn);
    conf.get(p, pn);
    return true;
  }

  // select network based on configuration
  template <typename T, class Tstate>
  module_1_1<T,Tstate>*
  create_network(parameter<T, Tstate> &theparam, configuration &conf,
		 intg &thick, int nout,
		 const char *varname, int tid, bool isbranch, bool narrow,
		 intg narrow_dim, intg narrow_size, intg narrow_offset,
		 vector<layers<T,Tstate>*> *branches,
		 vector<intg> *branches_thick,
		 map<string,module_1_1<T,Tstate>*> *shared_,
		 map<string,module_1_1<T,Tstate>*> *loaded_) {
    // if we don't find the generic architecture variable, try the old style
    // way with 'net-type'
    // if (!conf.exists(varname))
    // 	return create_network_old(theparam, conf, nout);
    // else, use the arch list
    map<string,module_1_1<T,Tstate>*> *shared = shared_;
    map<string,module_1_1<T,Tstate>*> *loaded = loaded_;
    if (!shared) shared = new map<string,module_1_1<T,Tstate>*>;
    if (!loaded) loaded = new map<string,module_1_1<T,Tstate>*>;
    list<string> arch = string_to_stringlist(conf.get_string(varname));
    uint arch_size = arch.size();
    layers<T,Tstate>* l =
      new layers<T,Tstate>(true, varname, isbranch, narrow, narrow_dim,
				  narrow_size, narrow_offset);
    // remember thickness output of branches
    if (!branches_thick)
      branches_thick = new vector<intg>;
    // keep list of existing branches so far
    if (!branches)
      branches = new vector<layers<T,Tstate>*>;
    // info
    cout << "Creating a network with " << nout << " outputs and "
	 << arch_size << " modules (input thickness is " << thick
	 << "): " << conf.get_string(varname) << endl;
    try {
      // loop over each module
      for (uint i = 0; i < arch_size; ++i) {
	cout << varname << " " << i << ": ";
	// get first module name of the list and remove it from list
	string name = arch.front(); arch.pop_front();
	int errid = 0; // id of error thrown by create_module
	module_1_1<T,Tstate> *module = NULL;
	try {
	  module = create_module<T,Tstate>
	    (name, theparam, conf, nout, thick, *shared, *loaded,
	     branches, branches_thick, tid);
	} catch (int err) { errid = err; }
	// add module to layers, if not null
	if (module) {
	  // add the module
	  l->add_module(module);
	  cout << "Added " << module->describe() << " (#params "
	       << theparam.x.nelements() << ", thickness " << thick
	       << ")" << endl;
	} else {
	  switch (errid) {
	  case 1: eblwarn("ignoring module " << name);
	    arch_size--; // decrease expected size of architecture
	    break ;
	  default: eblerror("failed to load module " << name); break ;
	  }
	}
      }
      if (isbranch) // remember last thickness for this branch
	branches_thick->push_back(thick);
      if (arch_size != l->size())
	eblerror("Some error occurred when loading modules, expected to load "
		 << arch_size << " modules but only " << l->size()
		 << " were successfully loaded");
      cout << varname << ": loaded " << l->size() << " modules." << endl;
    } eblcatcherror();
    if (!shared_) delete shared; // shared was allocated here, we can delete it
    if (!loaded_) delete loaded; // loaded was allocated here, we can delete it
    return l;
  }

  // select network based on configuration
  template <typename T, class Tstate>
  module_1_1<T,Tstate>*
  create_module(const string &name, parameter<T, Tstate> &theparam,
		configuration &conf, int &nout, intg &thick,
		map<string,module_1_1<T,Tstate>*> &shared,
		map<string,module_1_1<T,Tstate>*> &loaded,
		vector<layers<T,Tstate>*> *branches,
		vector<intg> *branches_thick,
                int tid) {
#ifdef __CUDA__
    // set some cuda variables
    bool use_gpu = conf.exists_bool("use_gpu");
    int gpu_id = -1;
    int num_devices = eblcuda_count_devices();    
    if (tid != -1 && num_devices > 0) {
      gpu_id = tid % num_devices;
      //cout << "Thread " << tid <<":\tUsing GPU:" << gpu_id << endl;
    }
    if (conf.exists("gpu_id"))
      gpu_id = conf.get_uint("gpu_id");
#endif
    string type = strip_last_num(name);
    module_1_1<T,Tstate> *module = NULL;
    // switch on each possible type of module
    // shared //////////////////////////////////////////////////////////////////
    // first check if the module we're loading is shared
    string sshared; bool bshared = false, bshared_exists = false;
    if (get_param(conf, name, "shared", sshared, true))
      bshared = (bool) string_to_int(sshared);
    // check if we already have it in stock
    typename map<string,module_1_1<T,Tstate>*>::iterator i =
      shared.find(name);
    if (i != shared.end()) bshared_exists = true; // module already allocated
    // merge ///////////////////////////////////////////////////////////////
    if (!type.compare("merge")) {
      string type, strbranches;
      if (!get_param(conf, name, "type", type)) return NULL;
      // switch on merging type
      if (!type.compare("concat")) { // concatenate
	intg concat_dim;
	string sstates;
	vector<vector<uint> > states;
	if (!get_param(conf, name, "dim", concat_dim)) return NULL;
	if (get_param(conf, name, "states", sstates, true)) {
	  vector<string> s = string_to_stringvector(sstates, ';');
	  for (uint i = 0; i < s.size(); ++i) {
	    vector<uint> v = string_to_uintvector(s[i]);
	    states.push_back(v);
	  }
	}
	// create module
	if (states.size() > 0)
	  module = (module_1_1<T,Tstate>*)
	    new merge_module<T,Tstate>(states, concat_dim, name.c_str());
	else eblerror("expected a _states id list to be concatenated");
	// update thickness after merging if specified by hand
	get_param(conf, name, "thickness", thick, true);
      } else if (!type.compare("flat") 
		 || !type.compare("mflat")
		 || !type.compare("linear")) { // flatten
	string strides, ins, sscales;
	mfidxdim bstride, scales;
	if (!get_param(conf, name, "ins", ins)) return NULL;
	if (!get_param(conf, name, "strides", strides)) return NULL;
	if (get_param(conf, name, "scales", sscales, true))
	  scales = string_to_fidxdimvector(sscales.c_str());

	midxdim bin = string_to_idxdimvector(ins.c_str());
	bstride = string_to_fidxdimvector(strides.c_str());
	mfidxdim *pscales = scales.size() > 0 ? &scales : NULL;
	if (!type.compare("linear")) {
	  intg lout;
	  if (!get_param2(conf, name, "out", lout, thick, nout)) return NULL;
#ifdef __CUDA__
          bool use_gpu_m = use_gpu;
          int gpu_id_m = gpu_id;
          get_param(conf, name, "use_gpu", use_gpu_m, true);
          get_param(conf, name, "gpu_id", gpu_id_m, true);
          if (use_gpu_m)
	  module = (module_1_1<T,Tstate>*)
	    new cuda_linear_merge_module<T,Tstate>(bshared_exists? NULL : &theparam,
					      lout, bin, bstride, 
					      name.c_str(), pscales, gpu_id_m);
          else
#endif
	  module = (module_1_1<T,Tstate>*)
	    new linear_merge_module<T,Tstate>(bshared_exists? NULL : &theparam,
					      lout, bin, bstride, 
					      name.c_str(), pscales);
	  thick = lout; // update thickness
	} else {
	  module = (module_1_1<T,Tstate>*)
	    new flat_merge_module<T,Tstate>(bin, bstride, name.c_str(),
					    pscales);
	}
      } else eblerror("unknown merge_type " << type);
    }
    // branch //////////////////////////////////////////////////////////////
    else if (!type.compare("branch")) {
      layers<T,Tstate> *branch = NULL;
      string type;
      bool narrow = false;
      intg narrow_dim, narrow_size, narrow_offset;
      if (!get_param(conf, name, "type", type)) return NULL;
      // get narrow parameters
      if (!type.compare("narrow")) { // narrow input
	narrow = true;
	if (!get_param(conf, name, "narrow_dim", narrow_dim)) return NULL;
	if (!get_param(conf, name, "narrow_size", narrow_size)) return NULL;
	if (!get_param(conf, name, "narrow_offset", narrow_offset)) return NULL;
      }
      cout << "Creating branch " << name;
      if (narrow)
	cout << ", narrow dim: " << narrow_dim << ", size: "
	     << narrow_size << ", offset: " << narrow_offset;
      cout << endl;
      // add branch
      branch = (layers<T,Tstate>*) create_network<T,Tstate>
	(theparam, conf, thick, nout, name.c_str(), tid, true,
	 narrow, narrow_dim, narrow_size, narrow_offset, branches,
	 branches_thick, &shared, &loaded);
      branches->push_back(branch);
      module = (module_1_1<T,Tstate>*) branch;
    }
    // narrow //////////////////////////////////////////////////////////////////
    else if (!type.compare("narrow")) {
      intg dim, size;
      vector<intg> offsets;
      string soff;
      bool narrow_states = false;
      if (!get_param(conf, name, "dim", dim)) return NULL;
      if (!get_param(conf, name, "size", size)) return NULL;
      if (!get_param(conf, name, "offset", soff)) return NULL;
      get_param(conf, name, "narrow_states", narrow_states, true);
      offsets = string_to_intgvector(soff.c_str());
      module = new narrow_module<T,Tstate>(dim, size, offsets, narrow_states,
					   name.c_str());
    }
    // table //////////////////////////////////////////////////////////////////
    else if (!type.compare("table")) {
      vector<intg> tbl;
      string sin;
      intg total = -1;
      if (!get_param(conf, name, "in", sin)) return NULL;
      if (!get_param(conf, name, "total", total)) return NULL;
      tbl = string_to_intgvector(sin.c_str());
      module = new table_module<T,Tstate>(tbl, total, name.c_str());
    }
    // interlace ///////////////////////////////////////////////////////////////
    else if (!type.compare("interlace")) {
      uint stride = 0;
      if (!get_param(conf, name, "stride", stride)) return NULL;
      module = new interlace_module<T,Tstate>(stride, name.c_str());
    }
    // preprocessing //////////////////////////////////////////////////////
    else if (!type.compare("rgb_to_ypuv") || !type.compare("rgb_to_ynuv")
	     || !type.compare("rgb_to_yp") || !type.compare("rgb_to_yn")
	     || !type.compare("y_to_yp")) {
      // get parameters for normalization
      string skernel; idxdim kernel;
      bool mirror = DEFAULT_PP_MIRROR, globn = true;
      t_norm mode = WSTD_NORM;
      double eps = NORM_EPSILON, eps2 = 0;
      if (get_param(conf, name, "kernel", skernel))
	kernel = string_to_idxdim(skernel);
      get_param(conf, name, "mirror", mirror, true);
      get_param(conf, name, "epsilon", eps, true);
      get_param(conf, name, "epsilon2", eps2, true);
      get_param(conf, name, "global_norm", globn, true);
      // create modules
      if (!type.compare("rgb_to_ypuv") || !type.compare("rgb_to_ynuv")) {
	module = (module_1_1<T,Tstate>*)
	  new rgb_to_ynuv_module<T,Tstate>(kernel, mirror, mode, globn, 
					   eps, eps2);
      } else if (!type.compare("rgb_to_yp") || !type.compare("rgb_to_yn")) {
	module = (module_1_1<T,Tstate>*)
	  new rgb_to_yn_module<T,Tstate>(kernel, mirror, mode, globn, 
					 eps, eps2);
      } else if (!type.compare("y_to_yp")) {
	module = (module_1_1<T,Tstate>*)
	  new y_to_yp_module<T,Tstate>(kernel, mirror, mode, globn, eps, eps2);
      }
    } else if (!type.compare("rgb_to_yuv"))
      module = (module_1_1<T,Tstate>*) new rgb_to_yuv_module<T,Tstate>();
    else if (!type.compare("rgb_to_y"))
      module = (module_1_1<T,Tstate>*) new rgb_to_y_module<T,Tstate>();
    else if (!type.compare("mschan")) {
      string snstates;
      if (!get_param(conf, name, "nstates", snstates)) return NULL;
      uint nstates = string_to_uint(snstates);
      module = (module_1_1<T,Tstate>*)
	new mschan_module<T,Tstate>(nstates, name.c_str());
    }
    // ms ////////////////////////////////////////////////////////////////
    else if (!type.compare("ms") || !type.compare("msc")) {
      string spipe;
      spipe << name << "_pipe";
      std::vector<module_1_1<T,Tstate>*> pipes;
      // loop while pipes exist
      vector<string> matches = conf.get_all_strings(spipe);
      intg thick2 = thick;
      for (uint i = 0; i < matches.size(); ++i) {
	thick2 = thick;
	string sp = matches[i];
	if (conf.exists(sp)) {
	  module_1_1<T,Tstate>* m =
	    create_network<T,Tstate>(theparam, conf, thick2, nout, sp.c_str(),
                                     tid, false, 0,0,0,0, branches, 
                                     branches_thick,
				     &shared, &loaded);
	  // check the module was created
	  if (!m) {
	    cerr << "expected a module in " << spipe << endl;
	    return NULL;
	  }
	  // add it
	  pipes.push_back(m);
	} else {
	  cout << "adding empty pipe (just passing data along) from variable "
	       << sp << endl;
	  pipes.push_back(NULL);
	}
      }
      thick = thick2;
      if (pipes.size() == 0) {
	eblwarn("no pipes found in module " << name.c_str()
		<< ", ignoring it");
	throw 1; // ignore this module
      }
      // get switching parameter
      string sswitch;
      midxdim switches;
      if (get_param(conf, name, "switch", sswitch, true))
	switches = string_to_idxdimvector(sswitch.c_str());
      // ms
      if (!type.compare("ms")) {
	bool replicate_inputs = false;
	get_param(conf, name, "replicate_inputs", replicate_inputs, true);
	ms_module<T,Tstate> *ms =
	  new ms_module<T,Tstate>(pipes, replicate_inputs, name.c_str());
	ms->set_switch(switches);
	module = (module_1_1<T,Tstate>*) ms;
      } else if (!type.compare("msc")) { // msc
	uint nsize = 0, nsize2 = 0, stride = 1;
	if (!get_param(conf, name, "nsize", nsize)) return NULL;
	get_param(conf, name, "nsize2", nsize2, true);
	get_param(conf, name, "stride", stride, true);
	msc_module<T,Tstate> *msc = new msc_module<T,Tstate>
	  (pipes, nsize, stride, nsize2, name.c_str());
	msc->set_switch(switches);
	module = (module_1_1<T,Tstate>*) msc;
      }
      EDEBUG("type: " << type << " " << module->describe());
    }
    // zpad /////////////////////////////////////////////////////////
    else if (!type.compare("zpad")) {
      string szpad;
      midxdim dims;
      if (get_param(conf, name, "dims", szpad))
	dims = string_to_idxdimvector(szpad.c_str());
      module = (module_1_1<T,Tstate>*)
	new zpad_module<T,Tstate>(dims, name.c_str());
    }
    // jitter //////////////////////////////////////////////////////////////////
    else if (!type.compare("jitter")) {
      jitter_module<T,Tstate> *j = new jitter_module<T,Tstate>(name.c_str());
      module = (module_1_1<T,Tstate>*) j;
      string str, srot, ssc, ssh, sel, spad;
      if (get_param(conf, name, "translations", str, true)) {
	vector<int> tr = string_to_intvector(str.c_str());
	j->set_translations(tr);
      }
      if (get_param(conf, name, "rotations", srot, true)) {
	vector<float> rot = string_to_floatvector(srot.c_str());
	j->set_rotations(rot);
      }
      if (get_param(conf, name, "scalings", ssc, true)) {
	vector<float> sc = string_to_floatvector(ssc.c_str());
	j->set_scalings(sc);
      }
      if (get_param(conf, name, "shears", ssh, true)) {
	vector<float> sh = string_to_floatvector(ssh.c_str());
	j->set_shears(sh);
      }
      if (get_param(conf, name, "elastic", sel, true)) {
	vector<float> el = string_to_floatvector(sel.c_str());
	j->set_elastics(el);
      }
      if (get_param(conf, name, "padding", spad, true)) {
	vector<uint> sp = string_to_uintvector(spad.c_str());
	j->set_padding(sp);
      }
    }
    // resizepp /////////////////////////////////////////////////////////
    else if (!type.compare("resizepp")) {
      string pps;
      // first get the preprocessing module
      if (!get_param(conf, name, "pp", pps)) return NULL;
      string pps_type = strip_last_num(pps);
      module_1_1<T,Tstate> *pp =
	create_module<T,Tstate>(pps, theparam, conf, nout, thick, shared,
				loaded, branches, branches_thick);
      if (!pp) {
	cerr << "expected a preprocessing module in " << name << endl;
	return NULL;
      }
      string szpad, ssize, sfovea, smode;
      idxdim zpad, size;
      uint mode = MEAN_RESIZE;
      bool preserve_ratio = true;
      get_param(conf, name, "preserve_ratio", preserve_ratio, true);
      if (get_param(conf, name, "mode", smode, true))
	mode = get_resize_type(smode.c_str());
      if (get_param(conf, name, "zpad", szpad, true))
	zpad = string_to_idxdim(szpad);
      if (get_param(conf, name, "size", ssize, true)) {
	size = string_to_idxdim(ssize);
	module = (module_1_1<T,Tstate>*)
	  new resizepp_module<T,Tstate>(size, mode, pp, true, &zpad,
					preserve_ratio, name.c_str());
      } else if (get_param(conf, name, "fovea", sfovea, true)) {
        //TODO: might have to add fovea_scale_size
	vector<double> fovea = string_to_doublevector(sfovea);
	module = (module_1_1<T,Tstate>*)
	  new fovea_module<T,Tstate>(fovea, false, mode, pp, true,&zpad);
      } else
	module = (module_1_1<T,Tstate>*)
	  new resizepp_module<T,Tstate>(mode, pp, true, &zpad,
					preserve_ratio, name.c_str());
    }
    // resize /////////////////////////////////////////////////////////
    else if (!type.compare("resize")) {
      double resizeh, resizew;
      string szpad, smod, smode;
      idxdim pad;
      bool preserve_ratio = true;
      uint mode = MEAN_RESIZE;
      if (!get_param(conf, name, "hratio", resizeh)) return NULL;
      if (!get_param(conf, name, "wratio", resizew)) return NULL;
      get_param(conf, name, "preserve_ratio", preserve_ratio);
      if (get_param(conf, name, "mode", smode, true))
	mode = get_resize_type(smode.c_str());
      if (get_param(conf, name, "zpad", szpad, true))
	pad = string_to_idxdim(szpad, 'x');
      // resize as a module's outputs
      if (get_param(conf, name, "as_module", smod, true)) {
	string ssize;
	if (!get_param(conf, name, "add", ssize)) return NULL;
	idxdim size = string_to_idxdim(ssize);
	// find module by name
	typename map<string,module_1_1<T,Tstate>*>::iterator i =
	  loaded.find(smod);
	if (i != loaded.end()) {
	  cout << "resizing as found module " << i->second->name() << endl;
	  module = (module_1_1<T,Tstate>*)
	    new resize_module<T,Tstate>(i->second, size, mode, &pad,
					name.c_str());
	} else
	  eblerror("resizing as module " << smod << ", module not found");
      } else 
	module = (module_1_1<T,Tstate>*)
	  new resize_module<T,Tstate>(resizeh, resizew, mode, &pad,
				      preserve_ratio, name.c_str());
    }
    // resize /////////////////////////////////////////////////////////
    else if (!type.compare("lpyramid")) {
      uint nscales = 0;
      string pp, skernels, sscalings, szpad;
      bool globnorm = true, locnorm = true, locnorm2 = false,
	color_lnorm = false, cnorm_across = true;
      midxdim zpads;
      double eps = NORM_EPSILON, eps2 = 0;
      if (!get_param(conf, name, "nscales", nscales)) return NULL;
      get_param(conf, name, "pp", pp, true);
      if (!get_param(conf, name, "kernels", skernels)) return NULL;
      midxdim kernels = string_to_idxdimvector(skernels.c_str());
      get_param(conf, name, "globalnorm", globnorm, true);
      get_param(conf, name, "localnorm", locnorm, true);
      get_param(conf, name, "localnorm2", locnorm2, true);
      get_param(conf, name, "cnorm_across", cnorm_across, true);
      get_param(conf, name, "color_lnorm", color_lnorm, true);
      get_param(conf, name, "epsilon", eps, true);
      get_param(conf, name, "epsilon2", eps2, true);
      if (get_param(conf, name, "zpad", szpad, true))
	zpads = string_to_idxdimvector(szpad.c_str());

      vector<float> scalings;
      if (get_param(conf, name, "scalings", sscalings, true))
	scalings = string_to_floatvector(sscalings.c_str());
      // create module
      module = (module_1_1<T,Tstate>*)
	create_preprocessing<T,Tstate>(pp.c_str(), kernels, zpads, "bilinear",
				       true, nscales, NULL, NULL, globnorm,
				       locnorm, locnorm2, color_lnorm,
				       cnorm_across, 1.0, 1.0,
				       scalings.size() > 0 ? &scalings : NULL,
				       name.c_str(), eps, eps2);
    }
    // convolution /////////////////////////////////////////////////////////
    else if (!type.compare("conv") || !type.compare("convl")) {
      idxdim kernel, stride;
      string skernel, sstride;
      idx<intg> table(1, 1);
      if (get_param(conf, name, "kernel", skernel, true))
	kernel = string_to_idxdim(skernel);
      if (get_param(conf, name, "stride", sstride, true))
	stride = string_to_idxdim(sstride);
      if (!load_table(conf, name, table, thick, nout)) return NULL;
      // update thickness
      idx<intg> tblmax = table.select(1, 1);
      thick = 1 + idx_max(tblmax);
      bool crop = true;
      // create module
      if (!type.compare("conv")) { // conv module 
#ifdef __CUDA__
        bool use_gpu_m = use_gpu;
        int gpu_id_m = gpu_id;
        get_param(conf, name, "use_gpu", use_gpu_m, true);
        get_param(conf, name, "gpu_id", gpu_id_m, true);
        if (use_gpu_m)
          module = (module_1_1<T,Tstate>*)
            new cuda_convolution_module<T,Tstate>
            (bshared_exists? NULL : &theparam, kernel, stride, table,
             name.c_str(), crop, gpu_id_m);
        else
#endif
          module = (module_1_1<T,Tstate>*)
            //	  new convolution_module_replicable<T,Tstate>
            new convolution_module<T,Tstate>
            (bshared_exists? NULL : &theparam, kernel, stride, table,
             name.c_str(), crop);
      }
      else if (!type.compare("convl")) // conv layer
	module = (module_1_1<T,Tstate>*)
	  new convolution_layer<T,Tstate>
	  (bshared_exists? NULL : &theparam, kernel, stride, table,
	   true /* tanh */, name.c_str());
    }
    // subsampling ///////////////////////////////////////////////////////
    else if (!type.compare("subs") || !type.compare("subsl")
	     || !type.compare("maxss")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      // create module
      if (!type.compare("subs")) // subsampling module
	module = (module_1_1<T,Tstate>*)
	  new subsampling_module_replicable<T,Tstate>
	  (bshared_exists? NULL : &theparam, thick, kernel, stride,
	   name.c_str());
      else if (!type.compare("subsl"))
	module = (module_1_1<T,Tstate>*)
	  new subsampling_layer<T,Tstate>
	  (bshared_exists? NULL : &theparam, thick, kernel, stride, true,
	   name.c_str());
      else if (!type.compare("maxss"))
	module = (module_1_1<T,Tstate>*)
	  new maxss_module<T,Tstate>(thick, kernel, stride, name.c_str());
    }
    // subsampling ///////////////////////////////////////////////////////
    else if (!type.compare("avg_pyramid")) {
      string sstride;
      if (!get_param(conf, name, "strides", sstride)) return NULL;
      midxdim strides = string_to_idxdimvector(sstride.c_str());
      module = (module_1_1<T,Tstate>*)
	new average_pyramid_module<T,Tstate>
	(bshared_exists? NULL : &theparam, thick, strides, name.c_str());
    }
    // wavg_pooling ////////////////////////////////////////////////////////////
    else if (!type.compare("wavgpool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      module = (module_1_1<T,Tstate>*)
	new wavg_pooling_module<T,Tstate>(thick, kernel, stride, name.c_str());
    }
    // l1pooling ///////////////////////////////////////////////////////////////
    else if (!type.compare("l1pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 1, name.c_str());
    }
    // l2pooling ///////////////////////////////////////////////////////////////
    else if (!type.compare("l2pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      bool crop = true;
#ifdef __CUDA__
      bool use_gpu_m = use_gpu;
      int gpu_id_m = gpu_id;
      get_param(conf, name, "use_gpu", use_gpu_m, true);
      get_param(conf, name, "gpu_id", gpu_id_m, true);
      if (use_gpu_m)
        module = (module_1_1<T,Tstate>*)
          new cuda_lppooling_module<T,Tstate>(th, kernel, stride, 
                                              2, name.c_str(), crop, 
                                              gpu_id);
      else
#endif
        module = (module_1_1<T,Tstate>*)
          new lppooling_module<T,Tstate>(th, kernel, stride, 2, name.c_str());
    }
    // l4pooling ///////////////////////////////////////////////////////////////
    else if (!type.compare("l4pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 4, name.c_str());
    }
    // l6pooling ///////////////////////////////////////////////////////////////
    else if (!type.compare("l6pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 6, name.c_str());
    }
    // l8pooling ///////////////////////////////////////////////////////////////
    else if (!type.compare("l8pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 8, name.c_str());
    }
    // l10pooling //////////////////////////////////////////////////////////////
    else if (!type.compare("l10pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 10, name.c_str());
    }
    // l12pooling //////////////////////////////////////////////////////////////
    else if (!type.compare("l12pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 12, name.c_str());
    }
    // l14pooling //////////////////////////////////////////////////////////////
    else if (!type.compare("l14pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 14, name.c_str());
    }
    // l16pooling //////////////////////////////////////////////////////////////
    else if (!type.compare("l16pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 16, name.c_str());
    }
    // l32pooling //////////////////////////////////////////////////////////////
    else if (!type.compare("l32pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 32, name.c_str());
    }
    // l64pooling //////////////////////////////////////////////////////////////
    else if (!type.compare("l64pool")) {
      string skernel, sstride;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, 64, name.c_str());
    }
    // lppooling ///////////////////////////////////////////////////////////////
    else if (!type.compare("lppool")) {
      string skernel, sstride;
      uint pool_power;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      if (!get_param(conf, name, "stride", sstride)) return NULL;
      if (!get_param(conf, name, "power", pool_power)) return NULL;
      idxdim kernel = string_to_idxdim(skernel);
      idxdim stride = string_to_idxdim(sstride);
      intg th = thick;
      get_param(conf, name, "thickness", th, true);
      module = (module_1_1<T,Tstate>*)
	new lppooling_module<T,Tstate>(th, kernel, stride, pool_power,
				       name.c_str());
    }
    // linear //////////////////////////////////////////////////////////////////
    else if (!type.compare("linear") || !type.compare("linear_replicable")) {
      intg lin, lout;
      if (!get_param2(conf, name, "in", lin, thick, nout)) return NULL;
      if (!get_param2(conf, name, "out", lout, thick, nout)) return NULL;
      // create module
      if (!type.compare("linear"))
	module = (module_1_1<T,Tstate>*) new linear_module<T,Tstate>
	  (bshared_exists? NULL : &theparam, lin, lout, name.c_str());
      else
	module = (module_1_1<T,Tstate>*) new linear_module_replicable<T,Tstate>
	  (bshared_exists? NULL : &theparam, lin, lout, name.c_str());
      thick = lout; // update thickness
    }
    // addc ////////////////////////////////////////////////////////////////////
    else if (!type.compare("addc")) {
#ifdef __CUDA__
      bool use_gpu_m = use_gpu;
      int gpu_id_m = gpu_id;
      get_param(conf, name, "use_gpu", use_gpu_m, true);
      get_param(conf, name, "gpu_id", gpu_id_m, true);
      if (use_gpu_m)
      module = (module_1_1<T,Tstate>*) new cuda_addc_module<T,Tstate>
	(bshared_exists? NULL : &theparam, thick, name.c_str(), gpu_id_m);
      else
#endif
        module = (module_1_1<T,Tstate>*) new addc_module<T,Tstate>
          (bshared_exists? NULL : &theparam, thick, name.c_str());
    }
    // diag ////////////////////////////////////////////////////////////////////
    else if (!type.compare("diag"))
      module = (module_1_1<T,Tstate>*) new diag_module<T,Tstate>
	(bshared_exists? NULL : &theparam, thick, name.c_str());
    // copy ////////////////////////////////////////////////////////////////////
    else if (!type.compare("copy"))
      module = (module_1_1<T,Tstate>*) new copy_module<T,Tstate>
	(name.c_str());
    // printer /////////////////////////////////////////////////////////////////
    else if (!type.compare("printer"))
      module = (module_1_1<T,Tstate>*) new printer_module<T,Tstate>
	(name.c_str());
    // normalization ///////////////////////////////////////////////////////////
    else if (!type.compare("wstd") || !type.compare("cnorm")
	     || !type.compare("snorm") || !type.compare("dnorm")) {
      intg wthick = thick;
      string skernel;
      bool learn = false, learn_mean = false, fsum_div = false;
      double cgauss = 2.0, epsilon = NORM_EPSILON, epsilon2 = 0;
      float fsum_split = 1.0;
      if (!get_param(conf, name, "kernel", skernel)) return NULL;
      idxdim ker = string_to_idxdim(skernel);
      // set optional number of features (default is 'thick')
      get_param(conf, name, "features", wthick, true);
      get_param(conf, name, "learn", learn, true);
      get_param(conf, name, "learn_mean", learn_mean, true);
      get_param(conf, name, "gaussian_coeff", cgauss, true);
      get_param(conf, name, "fsum_div", fsum_div, true);
      get_param(conf, name, "fsum_split", fsum_split, true);
      get_param(conf, name, "epsilon", epsilon, true);
      get_param(conf, name, "epsilon2", epsilon2, true);
      // normalization modules
      if (!type.compare("wstd") || !type.compare("cnorm")) {
#ifdef __CUDA__
      bool use_gpu_m = use_gpu;
      int gpu_id_m = gpu_id;
      get_param(conf, name, "use_gpu", use_gpu_m, true);
      get_param(conf, name, "gpu_id", gpu_id_m, true);
      if (use_gpu_m)
	module = (module_1_1<T,Tstate>*) new cuda_contrast_norm_module<T,Tstate>
	  (ker, wthick, conf.exists_true("mirror"), true, false,
	   learn ? &theparam : NULL, name.c_str(), true, learn_mean, cgauss,
	   fsum_div, fsum_split, epsilon, epsilon2, gpu_id_m);
      else
#endif
	module = (module_1_1<T,Tstate>*) new contrast_norm_module<T,Tstate>
	  (ker, wthick, conf.exists_true("mirror"), true, false,
	   learn ? &theparam : NULL, name.c_str(), true, learn_mean, cgauss,
	   fsum_div, fsum_split, epsilon, epsilon2);
      }
      else if (!type.compare("snorm")) {
#ifdef __CUDA__
      bool use_gpu_m = use_gpu;
      int gpu_id_m = gpu_id;
      get_param(conf, name, "use_gpu", use_gpu_m, true);
      get_param(conf, name, "gpu_id", gpu_id_m, true);
      if (use_gpu_m)
	module = (module_1_1<T,Tstate>*) new cuda_subtractive_norm_module<T,Tstate>
	  (ker, wthick, conf.exists_true("mirror"), false,
	   learn ? &theparam : NULL, name.c_str(), true, cgauss,
	   fsum_div, fsum_split, gpu_id_m);
      else
#endif
	module = (module_1_1<T,Tstate>*) new subtractive_norm_module<T,Tstate>
	  (ker, wthick, conf.exists_true("mirror"), false,
	   learn ? &theparam : NULL, name.c_str(), true, cgauss,
	   fsum_div, fsum_split);
      }
      else if (!type.compare("dnorm")) {
#ifdef __CUDA__
      bool use_gpu_m = use_gpu;
      int gpu_id_m = gpu_id;
      get_param(conf, name, "use_gpu", use_gpu_m, true);
      get_param(conf, name, "gpu_id", gpu_id_m, true);
      if (use_gpu_m)
	module = (module_1_1<T,Tstate>*) new cuda_divisive_norm_module<T,Tstate>
	  (ker, wthick, conf.exists_true("mirror"), true,
	   learn ? &theparam : NULL, name.c_str(), true, cgauss, fsum_div,
	   fsum_split, epsilon, epsilon2, gpu_id_m);
      else
#endif
	module = (module_1_1<T,Tstate>*) new divisive_norm_module<T,Tstate>
	  (ker, wthick, conf.exists_true("mirror"), true,
	   learn ? &theparam : NULL, name.c_str(), true, cgauss, fsum_div,
	   fsum_split, epsilon, epsilon2);
      }
    }
    // smooth shrink ///////////////////////////////////////////////////////////
    else if (!type.compare("sshrink")) {
      string sbias, sbeta;
      T beta = (T) 10, bias = (T) .3;
      if (get_param(conf, name, "beta", sbeta, true))
	beta = (T) string_to_double(sbeta);
      if (get_param(conf, name, "bias", bias, true))
	bias = (T) string_to_double(sbias);
      module = (module_1_1<T,Tstate>*) new smooth_shrink_module<T,Tstate>
	(bshared_exists? NULL : &theparam, thick, beta, bias);
    }
    // linear shrink ///////////////////////////////////////////////////////////
    else if (!type.compare("lshrink")) {
      string sbias;
      T bias = 0;
      if (get_param(conf, name, "bias", sbias, true))
	bias = (T) string_to_double(sbias);
      module = (module_1_1<T,Tstate>*) new linear_shrink_module<T,Tstate>
	(bshared_exists? NULL : &theparam, thick, bias);
    }
    // linear shrink ///////////////////////////////////////////////////////////
    else if (!type.compare("tshrink")) {
      bool diags = false;
      get_param(conf, name, "coefficients", diags, true);
      module = (module_1_1<T,Tstate>*) new tanh_shrink_module<T,Tstate>
	(bshared_exists? NULL : &theparam, thick, diags);
      // tanh ///////////////////////////////////////////////////////////////
    } else if (!type.compare("tanh")) {
#ifdef __CUDA__
      bool use_gpu_m = use_gpu;
      int gpu_id_m = gpu_id;
      get_param(conf, name, "use_gpu", use_gpu_m, true);
      get_param(conf, name, "gpu_id", gpu_id_m, true);
      if (use_gpu_m)
        module = (module_1_1<T,Tstate>*) new cuda_tanh_module<T,Tstate>(gpu_id_m);
      else
#endif
        module = (module_1_1<T,Tstate>*) new tanh_module<T,Tstate>();
    }
    // stdsig //////////////////////////////////////////////////////////////
    else if (!type.compare("stdsig"))
      module = (module_1_1<T,Tstate>*) new stdsigmoid_module<T,Tstate>();
    // abs //////////////////////////////////////////////////////////////
    else if (!type.compare("abs"))
      module = (module_1_1<T,Tstate>*) new abs_module<T,Tstate>();
    // abs //////////////////////////////////////////////////////////////
    else if (!type.compare("back"))
      module = (module_1_1<T,Tstate>*) new back_module<T,Tstate>();
    // abs //////////////////////////////////////////////////////////////
    else if (!type.compare("lua")) {
      string script;
      if (!get_param(conf, name, "script", script)) return NULL;
      module = (module_1_1<T,Tstate>*) new lua_module<T,Tstate>(script.c_str());
    } else
      cout << "unknown module type " << type << endl;
    // check if the module we're loading is shared
    if (module && bshared) { // this module is shared with others
      // check if we already have it in stock
      typename map<string,module_1_1<T,Tstate>*>::iterator i =
	shared.find(name);
      if (i != shared.end()) { // already exist
	delete module;
	module = i->second->copy(); // load a shared copy instead
	cout << "Loaded a shared copy of " << name << ". ";
      }
      else // we don't have it, add it
	shared[name] = module; // save this copy for future sharing
    }
    // add an ebm1 wrapper around this module if requested
    string sebm;
    if (get_param(conf, name, "energy", sebm, true)) {
      // create penalty module
      ebm_1<T,Tstate> *e = create_ebm1<T,Tstate>(sebm, conf);
      if (!e) eblerror("failed to create ebm1 from " << sebm);
      // create hybrid penalty / module_1_1
      module = new ebm_module_1_1<T,Tstate>(module, e, sebm.c_str());
    }
    // add this module to map of loaded modules
    loaded[name] = module;
    return module;
  }

  // select network based on configuration
  template <typename T, class Tstate>
  ebm_1<T,Tstate>* create_ebm1(const string &name, configuration &conf) {
    string type = strip_last_num(name);
    ebm_1<T,Tstate> *ebm = NULL;
    // switch on each possible type of module
    if (!type.compare("l1penalty")) {
      T threshold = 0, coeff = 1;
      get_param(conf, name, "threshold", threshold, true);
      get_param(conf, name, "coeff", coeff, true);
      ebm = (ebm_1<T,Tstate>*) new l1_penalty<T,Tstate>(threshold, coeff);
    }
    else cout << "unknown ebm1 type " << type << endl;
    return ebm;
  }

  template <typename T, typename Tds1, typename Tds2, class Tstate>
  answer_module<T,Tds1,Tds2,Tstate>*
  create_answer(configuration &conf, uint noutputs,
		const char *varname) {
    string name = conf.get_string(varname);
    string type = strip_last_num(name);
    answer_module<T,Tds1,Tds2,Tstate> *module = NULL;
    // loop on possible answer modules /////////////////////////////////////////
    if (!type.compare("class_answer")) {
      string kerd_name;
      uint tconf = confidence_max;
      bool binary = false, btanh = false;
      float factor = 1.0;
      int force = -1, single = -1;
      idxdim kerd;
      double sigma_scale = 3;
      get_param(conf, name, "single_output", single, true);
      get_param(conf, name, "factor", factor, true);
      get_param(conf, name, "binary", binary, true);
      get_param(conf, name, "confidence", tconf, true);
      get_param(conf, name, "tanh", btanh, true);
      get_param(conf, name, "force_class", force, true);
      get_param(conf, name, "sigma_scale", sigma_scale, true);
      if (get_param(conf, name, "kernel", kerd_name, true))
	kerd = string_to_idxdim(kerd_name);
      module = new class_answer<T,Tds1,Tds2,Tstate>
	(noutputs, factor, binary, (t_confidence) tconf, btanh, name.c_str(), 
	 force, single, &kerd, sigma_scale);
      //////////////////////////////////////////////////////////////////////////
    } else if (!type.compare("vote_answer")) {
      string factor_name, binary_name, tconf_name, tanh_name;
      t_confidence tconf = confidence_max;
      bool binary = false, btanh = false;
      float factor = 1.0;
      if (get_param(conf, name, "factor", factor_name, true))
	factor = string_to_float(factor_name);
      if (get_param(conf, name, "binary", binary_name, true))
	binary = (bool) string_to_uint(binary_name);
      if (get_param(conf, name, "confidence", tconf_name, true))
	tconf = (t_confidence) string_to_uint(tconf_name);
      if (get_param(conf, name, "tanh", tanh_name, true))
	btanh = (bool) string_to_uint(tanh_name);
      module = new vote_answer<T,Tds1,Tds2,Tstate>
	(noutputs, factor, binary, tconf, btanh, name.c_str());
      //////////////////////////////////////////////////////////////////////////
    } else if (!type.compare("regression_answer")) {
      string threshold_name;
      float64 threshold = 0.0;
      if (get_param(conf, name, "threshold", threshold_name, true))
	threshold = (float64) string_to_double(threshold_name);
      module = new regression_answer<T,Tds1,Tds2,Tstate>
	(noutputs, threshold, name.c_str());
      //////////////////////////////////////////////////////////////////////////
    } else if (!type.compare("scaler_answer")) {
      string negative_name, raw_name, threshold_name, spatial_name;
      bool raw_conf = false, spatial = false;
      float threshold = 0.0;
      if (!get_param(conf, name, "negative", negative_name)) return NULL;
      if (get_param(conf, name, "rawconf", raw_name, true))
	raw_conf = (bool) string_to_uint(raw_name);
      if (get_param(conf, name, "threshold", threshold_name, true))
	threshold = (float) string_to_float(threshold_name);
      if (get_param(conf, name, "spatial", spatial_name, true))
	spatial = (bool) string_to_uint(spatial_name);
      module = new scaler_answer<T,Tds1,Tds2,Tstate>
	(1, 0, raw_conf, threshold, spatial, name.c_str());
      //////////////////////////////////////////////////////////////////////////
    } else if (!type.compare("scalerclass_answer")) {
      string factor_name, binary_name, tconf_name, tanh_name,
	jsize_name, joff_name, mgauss_name, pconf_name, pbconf_name,
	coeffs_name, biases_name;
      t_confidence tconf = confidence_max;
      bool binary = false, btanh = false,
	predict_conf = false, predict_bconf = false;
      float factor = 1.0, mgauss = 1.5;
      uint jsize = 1, joff = 0;
      idx<T> coeffs, biases;
      bool coeffs_set = false, biases_set = false;
      if (get_param(conf, name, "factor", factor_name, true))
	factor = string_to_float(factor_name);
      if (get_param(conf, name, "binary", binary_name, true))
	binary = (bool) string_to_uint(binary_name);
      if (get_param(conf, name, "confidence", tconf_name, true))
	tconf = (t_confidence) string_to_uint(tconf_name);
      if (get_param(conf, name, "tanh", tanh_name, true))
	btanh = (bool) string_to_uint(tanh_name);
      if (get_param(conf, name, "jsize", jsize_name, true))
	jsize = string_to_uint(jsize_name);
      if (get_param(conf, name, "joffset", joff_name, true))
	joff = string_to_uint(joff_name);
      if (get_param(conf, name, "mgauss", mgauss_name, true))
	mgauss = string_to_float(mgauss_name);
      if (get_param(conf, name, "predict_conf", pconf_name, true))
	predict_conf = (bool) string_to_uint(pconf_name);
      if (get_param(conf, name, "predict_bconf", pbconf_name, true))
	predict_bconf = (bool) string_to_uint(pbconf_name);
      if (get_param(conf, name, "coeffs", coeffs_name, true)) {
	coeffs = string_to_idx<T>(coeffs_name.c_str());
	coeffs_set = true;
      }
      if (get_param(conf, name, "biases", biases_name, true)) {
	biases = string_to_idx<T>(biases_name.c_str());
	biases_set = true;
      }
      module =
	new scalerclass_answer<T,Tds1,Tds2,Tstate>
	(noutputs, factor, binary, tconf, btanh, jsize, joff, mgauss,
	 predict_conf, predict_bconf, biases_set ? &biases : NULL,
	 coeffs_set ? &coeffs : NULL, name.c_str());
      //////////////////////////////////////////////////////////////////////////
    } else
      cerr << "unknown answer type " << type << endl;
    return module;
  }

  template <typename T, typename Tds1, typename Tds2, class Tstate>
  trainable_module<T,Tds1,Tds2,Tstate>*
  create_trainer(configuration &conf, labeled_datasource<T,Tds1,Tds2> &ds,
		 module_1_1<T,Tstate> &net,
		 answer_module<T,Tds1,Tds2,Tstate> &answer,
		 const char *varname) {
    string name = conf.get_string(varname);
    string type = strip_last_num(name);
    trainable_module<T,Tds1,Tds2,Tstate> *module = NULL;
    // switch on each possible type of trainer module
    if (!type.compare("trainable_module")) {
      ebm_2<Tstate> *energy = NULL;
      string energy_name, switcher;
      if (!get_param(conf, name, "energy", energy_name)) return NULL;
      string energy_type = strip_last_num(energy_name);
      get_param(conf, name, "switcher", switcher, true);

      // loop on possible energy modules ///////////////////////////////////////
      if (!energy_type.compare("l2_energy")) {
	energy = new l2_energy<T,Tstate>(energy_name.c_str());
      } else if (!energy_type.compare("scalerclass_energy")) {
	string tanh_name, jsize_name, jselection_name, dist_name, scale_name,
	  pconf_name, pbconf_name, coeffs_name, biases_name;
	bool apply_tanh = false, predict_conf = false, predict_bconf = false;
	uint jsize = 1, jselection = 0;
	float dist_coeff = 1.0, scale_coeff = 1.0;
	idx<T> coeffs, biases;
	bool coeffs_set = false, biases_set = false;
	if (get_param(conf, energy_name, "tanh", tanh_name, true))
	  apply_tanh = (bool) string_to_uint(tanh_name);
	if (get_param(conf, energy_name, "jsize", jsize_name, true))
	  jsize = string_to_uint(jsize_name);
	if (get_param(conf, energy_name, "jselection", jselection_name, true))
	  jselection = string_to_uint(jselection_name);
	if (get_param(conf, energy_name, "distcoeff", dist_name, true))
	  dist_coeff = string_to_float(dist_name);
	if (get_param(conf, energy_name, "scalecoeff", scale_name, true))
	  scale_coeff = string_to_float(scale_name);
	if (get_param(conf, energy_name, "predict_conf", pconf_name, true))
	  predict_conf = (bool) string_to_uint(pconf_name);
	if (get_param(conf, energy_name, "predict_bconf", pbconf_name, true))
	  predict_bconf = (bool) string_to_uint(pbconf_name);
	if (get_param(conf, energy_name, "coeffs", coeffs_name, true)) {
	  coeffs = string_to_idx<T>(coeffs_name.c_str());
	  coeffs_set = true;
	}
	if (get_param(conf, energy_name, "biases", biases_name, true)) {
	  biases = string_to_idx<T>(biases_name.c_str());
	  biases_set = true;
	}
	energy =
	  new scalerclass_energy<T,Tstate>(apply_tanh, jsize, jselection,
					   dist_coeff, scale_coeff,
					   predict_conf, predict_bconf,
					   biases_set ? &biases : NULL,
					   coeffs_set ? &coeffs : NULL,
					   energy_name.c_str());
      } else if (!energy_type.compare("scaler_energy")) {
	energy = new scaler_energy<T,Tstate>(energy_name.c_str());
      } else
	eblerror("unknown energy type " << energy_type);

      // allocate trainer module
      module = new trainable_module<T,Tds1,Tds2,Tstate>
	(*energy, net, NULL, &answer, name.c_str(), switcher.c_str());
    }
    if (!module)
      eblerror("no trainer module found");
    return module;
  }

  template <typename T, class Tstate>
  resizepp_module<T,Tstate>*
  create_preprocessing(uint height, uint width, const char *ppchan,
		       idxdim &kersz, const char *resize_method,
		       bool keep_aspect_ratio, int lpyramid,
		       vector<double> *fovea, midxdim *fovea_scale_size,
                       bool globnorm, bool locnorm, bool locnorm2,
		       bool color_lnorm, bool cnorm_across,
		       double hscale, double wscale, vector<float> *scalings,
		       const char *name, double epsilon, double epsilon2) {
    midxdim kers;
    kers.push_back(kersz);
    return create_preprocessing<T,Tstate>
      (height, width, ppchan, kers, resize_method, keep_aspect_ratio, lpyramid,
       fovea, fovea_scale_size, globnorm, locnorm, locnorm2, color_lnorm,
       cnorm_across, hscale, wscale, scalings, name, epsilon, epsilon2);
  }

  template <typename T, class Tstate>
  resizepp_module<T,Tstate>*
  create_preprocessing(midxdim &dims, const char *ppchan,
		       midxdim &kers, midxdim &zpads, const char *resize_method,
		       bool keep_aspect_ratio, int lpyramid,
		       vector<double> *fovea, midxdim *fovea_scale_size,
                       bool globnorm, bool locnorm, bool locnorm2,
		       bool color_lnorm, bool cnorm_across, double hscale,
		       double wscale, vector<float> *scalings,
		       const char *name_, double e, double e2) {
    module_1_1<T,Tstate> *chanmodule = NULL;
    resizepp_module<T,Tstate> *ppmodule = NULL;
    if (kers.size() == 0) eblerror("expected at least 1 ker dims");
    idxdim kersz = kers[0];
    // set name of preprocessing
    string name;
    if (name_) name << name_ << "_";
    if (dims.size() == 0) eblerror("expected at least 1 idxdim in dims");
    idxdim d = dims[0];
    int height = d.dim(0), width = d.dim(1);
    name << kersz << "_" << resize_method << height << "x" << width;
    if (!keep_aspect_ratio) name << "_noaspratio";
    if (!globnorm) name << "_nognorm";
    // set default min/max val for display
    T minval = (T) -2, maxval = (T) 2;
    t_norm tn = WSTD_NORM; bool mir = true;
    // create channel preprocessing module
    if (!strcmp(ppchan, "YpUV") || !strcmp(ppchan, "YnUV")) {
      chanmodule = 
	new rgb_to_ynuv_module<T,Tstate>(kersz, mir, tn, globnorm, e, e2);
    } else if (!strcmp(ppchan, "Yp") || !strcmp(ppchan, "Yn")) {
      chanmodule = 
	new rgb_to_yn_module<T,Tstate>(kersz, mir, tn, globnorm, e, e2);
    } else if (!strcmp(ppchan, "YnUVn")) {
      chanmodule = 
	new rgb_to_ynuvn_module<T,Tstate>(kersz, mir, tn,globnorm, e, e2);
    } else if (!strcmp(ppchan, "YnUnVn")) {
      chanmodule = 
	new rgb_to_ynunvn_module<T,Tstate>(kersz, mir,tn,globnorm, e, e2);
    } else if (!strcmp(ppchan, "YUVn")) {
      chanmodule = 
	new rgb_to_yuvn_module<T,Tstate>(kersz, mir, tn, globnorm, e, e2);
    } else if (!strcmp(ppchan, "RGBn")) {
      chanmodule = 
	new rgb_to_rgbn_module<T,Tstate>(kersz, mir, tn, globnorm, e, e2);
    } else if (!strcmp(ppchan, "YUV")) {
      chanmodule = new rgb_to_yuv_module<T,Tstate>(globnorm);
    } else if (!strcmp(ppchan, "HSV")) {
      eblerror("HSV pp module not implemented");
    } else if (!strcmp(ppchan, "RGB")) {
      // no preprocessing module, just set min/max val for display
      minval = (T) 0;
      maxval = (T) 255;
    } else eblerror("undefined channel preprocessing " << ppchan);
    // initialize resizing method
    uint resiz = get_resize_type(resize_method);
    // create resizing module
    // fovea resize
    if (fovea && fovea->size() > 0) {
      if (!fovea_scale_size || fovea_scale_size->size() != fovea->size())
	eblerror("expected same number of parameters in fovea and "
		 << "fovea_scale_size");
      ppmodule = new fovea_module<T,Tstate>(*fovea, *fovea_scale_size, d, true,
                                            resiz, chanmodule);
      name << "_fovea" << fovea->size();
    } else if (lpyramid > 0) { // laplacian pyramid resize
      laplacian_pyramid_module<T,Tstate> *pyr =
	new laplacian_pyramid_module<T,Tstate>
	(lpyramid, kers, dims, resiz, chanmodule, false, NULL, globnorm,
	 locnorm, locnorm2, color_lnorm, cnorm_across, keep_aspect_ratio);
      if (scalings) pyr->set_scalings(*scalings);
      ppmodule = pyr;
      if (!locnorm) name << "_nolnorm";
      if (!locnorm2) name << "_nolnorm2";
      if (color_lnorm) {
	name << "_colorlnorm";
	if (cnorm_across) name << "across";
      }
      name << "_lpyramid" << lpyramid;
    } else // regular resize
      ppmodule = new resizepp_module<T,Tstate>(d, resiz, chanmodule, true,
					       NULL, keep_aspect_ratio);
    ppmodule->set_scale_factor(hscale, wscale);
    ppmodule->set_display_range(minval, maxval);
    ppmodule->set_name(name.c_str());
    ppmodule->set_zpad(zpads);
    return ppmodule;
  }

  template <typename T, class Tstate>
  resizepp_module<T,Tstate>*
  create_preprocessing(const char *ppchan,
		       midxdim &kers, midxdim &zpads, const char *resize_method,
		       bool keep_aspect_ratio, int lpyramid,
		       vector<double> *fovea, midxdim *fovea_scale_size,
                       bool globnorm, bool locnorm, bool locnorm2,
		       bool color_lnorm, bool cnorm_across, double hscale,
		       double wscale, vector<float> *scalings,
		       const char *name, double epsilon, double epsilon2) {
    midxdim d;
    d.push_back(idxdim(0, 0));
    return create_preprocessing<T,Tstate>
      (d, ppchan, kers, zpads, resize_method, keep_aspect_ratio, lpyramid,
       fovea, fovea_scale_size, globnorm, locnorm, locnorm2, color_lnorm,
       cnorm_across, hscale, wscale, scalings, name, epsilon, epsilon2);
  }

  // select network based on configuration, using old-style variables
  template <typename T, class Tstate>
  module_1_1<T,Tstate>* create_network_old(parameter<T, Tstate> &theparam,
					   configuration &conf, int noutputs) {
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
    if (!conf.exists(name)) return false; // do nothing if variable not found
    vector<string> filenames = string_to_stringvector(conf.get_string(name.c_str()));
    idx<T> w = load_matrix<T>(filenames, 0);
    m.load_x(w);
    cout << "Loaded weights " << w << " into " << module_name << " from "
	 << filenames << " (dims " << w << " min " << idx_min(w) << " max "
	 << idx_max(w) << " mean " << idx_mean(w) << ")" << endl;
    return true;
  }

  // select network based on configuration
  template <typename T, class Tstate>
  uint manually_load_network(layers<T,Tstate> &l, configuration &conf,
			     const char *varname) {
    list<string> arch = string_to_stringlist(conf.get_string(varname));
    uint arch_size = arch.size();
    cout << "Loading network manually using module list: "
	 << conf.get_string(varname) << endl;
    uint n = 0;
    // loop over each module
    for (uint i = 0; i < arch_size; ++i) {
      // get first module name of the list and remove it from list
      string name = arch.front(); arch.pop_front();
      string type = strip_last_num(name);
      module_1_1<T,Tstate> *m = l.modules[i];
      // switch on each possible type of module
      if (!type.compare("conv"))
	n += load_module<convolution_module<T,Tstate>,T,Tstate>
	  (conf, *m, name, type);
      else if (!type.compare("addc"))
	n += load_module<addc_module<T,Tstate>,T,Tstate>(conf, *m, name, type);
      else if (!type.compare("linear"))
	n += load_module<linear_module<T,Tstate>,T,Tstate>(conf, *m, name,type);
      else if (!type.compare("diag"))
	n += load_module<diag_module<T,Tstate>,T,Tstate>(conf, *m, name, type);
      else if (!type.compare("ms")) {
	ms_module<T,Tstate> *msm = dynamic_cast<ms_module<T,Tstate>*>(m);
	if (!msm)
	  eblerror("expected a ms module while trying to load module "
		   << name << " but found: "<< typeid(m).name());
	for (uint pi = 0; pi < msm->npipes(); ++pi) {
	  module_1_1<T,Tstate> *pipe = msm->get_pipe(pi);
	  if (!pipe) continue ;
	  if (!dynamic_cast<layers<T,Tstate>*>(pipe))
	    eblerror("expected a layers module in pipes[" << pi << "] while "
		     << "trying to load module " << pipe->name()
		     << " but found: " << typeid(pipe).name());
	  n += manually_load_network(*((layers<T,Tstate>*)pipe), conf,
				     pipe->name());
	}
      }
      else if (!type.compare("branch")) {
	if (!dynamic_cast<layers<T,Tstate>*>(m))
	  eblerror("expected a layers module with type branch while trying "
		   << "to load module " << name << " but found: "
		   << typeid(m).name());
	n += manually_load_network(*((layers<T,Tstate>*)m), conf,
				   name.c_str());
      }
    }
    if (!l.is_branch())
      cout << "Loaded " << n << " weights." << endl;
    return n;
  }

} // end namespace ebl

#endif /* NETCONF_HPP_ */
