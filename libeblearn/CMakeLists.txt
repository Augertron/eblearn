################################################################################
#
# CMake configuration for libeblearn project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

# set include directories
################################################################################
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(${LIBIDX_INCLUDE_DIR})
SET(LIBEBLEARN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

# compile library
################################################################################
ADD_LIBRARY (eblearn SHARED
		     src/Classifier2D.cpp  
		     src/DataSource.cpp  
		     src/Defines.cpp  
		     src/Gbl.cpp  
		     src/Ebl.cpp  
		     src/EblStates.cpp  
		     src/EblBasic.cpp  
		     src/EblTester.cpp
		     src/Image.cpp  
		     src/Ipp.cpp  
		     src/libeblearn.cpp  
		     src/Net.cpp  
		     src/Trainer.cpp
		     src/sdnn_modules.cpp
		     src/net_cscscfe.cpp
		     )

## link against libidx
TARGET_LINK_LIBRARIES(eblearn idx)