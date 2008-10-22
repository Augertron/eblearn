################################################################################
#
# CMake configuration for eblearn_gui project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

include_directories (include)
include_directories(${LIBEBLEARN_INCLUDE_DIR})
include_directories(${LIBIDX_INCLUDE_DIR})
SET(EBLEARN_GUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

FIND_PACKAGE(Qt4)
IF (QT_FOUND) #-----------------------------------------------------------------

  SET(USE_EBLEARN_GUI 1 PARENT_SCOPE)

  # add include directories
  ################################################################################
  include_directories (${QT_INCLUDE_DIR})
  include_directories (${QT_QTGUI_INCLUDE_DIR})
  include_directories (${QT_QTCORE_INCLUDE_DIR})
  
  # compile library
  ################################################################################
  add_library (eblearn_gui SHARED
    src/moc_ebm_gui.cpp
    src/ebm_gui.cpp
    )
  
  # link executable with external libraries
  ################################################################################
  target_link_libraries (eblearn_gui eblearn idx)
  target_link_libraries (eblearn_gui ${QT_QTCORE_LIBRARY})
  target_link_libraries (eblearn_gui ${QT_QTGUI_LIBRARY})
  
ENDIF (QT_FOUND) #--------------------------------------------------------------
