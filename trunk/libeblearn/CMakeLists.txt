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
  include_directories(${EBLEARN_GUI_INCLUDE_DIR})
  include_directories(${QT_INCLUDE_DIR})
  include_directories(${QT_QTGUI_INCLUDE_DIR})
  include_directories(${QT_QTCORE_INCLUDE_DIR})
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GUI__")
ENDIF (QT_FOUND) #--------------------------------------------------------------

# compile library
################################################################################
ADD_LIBRARY (eblearn SHARED
		     src/Defines.cpp  
		     src/Ebl.cpp  
		     src/EblArch.cpp  
		     src/EblStates.cpp  
		     src/EblCost.cpp  
		     src/EblBasic.cpp  
		     src/EblNonLinearity.cpp  
		     src/EblTester.cpp
		     src/EblLayers.cpp  
		     src/EblLogger.cpp  
		     src/EblMachines.cpp  
		     src/Ipp.cpp  
		     src/libeblearn.cpp  
		     src/Net.cpp  
		     src/Trainer.cpp
		     src/sdnn_modules.cpp
		     src/net_cscscfe.cpp
		     src/Classifier2D.cpp  
		     src/DataSource.cpp  
		     src/Gbl.cpp  
		     )

## link against libidx
TARGET_LINK_LIBRARIES(eblearn idx)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (eblearn eblearn_gui)
ENDIF (QT_FOUND) #------------------------------------------------------------
