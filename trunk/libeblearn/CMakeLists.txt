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
		     src/ebl_arch.cpp  
		     src/ebl_states.cpp  
#		     src/ebl_cost.cpp  
#		     src/ebl_codec.cpp  
		     src/ebl_basic.cpp  
#		     src/ebl_transfer.cpp  
#		     src/ebl_nonlinearity.cpp  
#		     src/ebl_tester.cpp
#		     src/ebl_layers.cpp  
		     src/ebl_logger.cpp  
#		     src/ebl_machines.cpp  
		     src/ipp.cpp  
#		     src/sdnn_modules.cpp
#		     src/net_cscscfe.cpp
#		     src/detector.cpp  
#		     src/datasource.cpp
		     )

# linking
################################################################################
TARGET_LINK_LIBRARIES(eblearn idx)
