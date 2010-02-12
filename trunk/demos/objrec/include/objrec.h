#ifndef OBJREC_H_
#define OBJREC_H_

#include <fenv.h>
#include "libeblearn.h"
#include "libeblearntools.h"

#ifdef __GUI__
#include "libeblearngui.h"
#endif

using namespace std;
using namespace ebl; // all eblearn objects are under the ebl namespace

// select network based on configuration
template <typename T>
module_1_1<T>* init_network(parameter<T> &theparam,
			    configuration &conf, uint noutputs);

#include "objrec.hpp"

#endif /* OBJREC_H_ */
