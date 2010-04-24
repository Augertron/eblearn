#ifndef OBJREC_HPP_
#define OBJREC_HPP_

// select network based on configuration
template <typename T>
module_1_1<T>* init_network(parameter<T> &theparam,
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

#endif /* OBJREC_HPP_ */
