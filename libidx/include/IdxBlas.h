/* #undef APPLE_FRAMEWORK_FOUND */

// Apple framework calls are mode into a C++ header
// On custom Atlas installations extern C is not included
// On package instalaltions from distributions, extern C is
// included, but another one does not hurt :)
#ifndef APPLE_FRAMEWORK_FOUND
extern "C" {
#endif

#include "cblas.h"

#ifndef APPLE_FRAMEWORK_FOUND
}
#endif
