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

FIND_PACKAGE(Qt4)
IF (QT_FOUND) #-----------------------------------------------------------------
  # add include directories
  ##############################################################################
  include_directories(${LIBIDXGUI_INCLUDE_DIR})
  include_directories(${QT_INCLUDE_DIR})
  include_directories(${QT_QTGUI_INCLUDE_DIR})
  include_directories(${QT_QTCORE_INCLUDE_DIR})
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GUI__")
ENDIF (QT_FOUND) #--------------------------------------------------------------

# compile library
################################################################################
ADD_LIBRARY (eblearn SHARED
		     src/ebl_arch.cpp  
		     src/ebl_states.cpp  
		     src/ebl_cost.cpp  
		     src/ebl_basic.cpp  
		     src/ebl_nonlinearity.cpp  
		     src/ebl_tester.cpp
		     src/ebl_layers.cpp  
		     src/ebl_logger.cpp  
		     src/ebl_machines.cpp  
		     src/ipp.cpp  
		     src/sdnn_modules.cpp
		     src/net_cscscfe.cpp
		     src/classifier2D.cpp  
		     src/datasource.cpp  
		     src/classifier_gen.cpp
		     )

# linking
################################################################################
TARGET_LINK_LIBRARIES(eblearn idx)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (eblearn idxgui)
ENDIF (QT_FOUND) #------------------------------------------------------------
