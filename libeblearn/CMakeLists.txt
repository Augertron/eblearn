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
SET(LIBEBLEARN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

# compile library
################################################################################
add_library (eblearn SHARED
		     src/Blas.cpp  
		     src/Classifier2D.cpp  
		     src/DataSource.cpp  
		     src/Defines.cpp  
		     src/Ebm.cpp  
		     src/Idx.cpp  
		     src/Image.cpp  
		     src/Ipp.cpp  
		     src/libeblearn.cpp  
		     src/Net.cpp  
		     src/Numerics.cpp  
		     src/Trainer.cpp
		     src/EbmTester.cpp
		     src/sdnn_modules.cpp
		     src/net_cscscfe.cpp
		     )

# check for external libraries
################################################################################
# CBLAS
################################################################################
FIND_PACKAGE(CBLAS)
IF(CBLAS_FOUND)
  INCLUDE_DIRECTORIES(${CBLAS_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(eblearn ${CBLAS_LIBRARIES})
  CONFIGURE_FILE("include/DummyBlas.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/DummyBlas.h")
ELSE(CBLAS_FOUND)
  MESSAGE(FATAL_ERROR "CBLAS library not found. Please specify library  location")
ENDIF(CBLAS_FOUND)

# IPP
################################################################################
