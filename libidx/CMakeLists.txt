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
include_directories (include)
SET(LIBIDX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

# compile library
################################################################################
add_library (idx SHARED
		     src/Blas.cpp  
		     src/LibidxDefines.cpp  
		     src/Idx.cpp  
		     src/Image.cpp  
		     src/Numerics.cpp  
		     src/libidx.cpp  
		     )

# check for external libraries
################################################################################
# CBLAS
################################################################################
FIND_PACKAGE(CBLAS)
IF(CBLAS_FOUND)
  INCLUDE_DIRECTORIES(${CBLAS_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(idx ${CBLAS_LIBRARIES})
  CONFIGURE_FILE("include/IdxBlas.h.in" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/IdxBlas.h")
ELSE(CBLAS_FOUND)
  MESSAGE(FATAL_ERROR 
    "CBLAS library not found. Please specify library  location")
ENDIF(CBLAS_FOUND)

