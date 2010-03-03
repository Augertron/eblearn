################################################################################
#
# CMake configuration for idxgui library
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

include_directories (include)
include_directories(${LIBIDX_INCLUDE_DIR})
SET(LIBIDXGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

IF (QT_FOUND) #-----------------------------------------------------------------

  # QT specific: produce moc files from headers
  ##############################################################################
  include_directories(${CMAKE_CURRENT_BINARY_DIR})

  SET_SOURCE_FILES_PROPERTIES(src/gui_thread.cpp PROPERTIES 
    OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/moc_gui_thread.cxx)
  SET_SOURCE_FILES_PROPERTIES(src/idxgui.cpp PROPERTIES 
    OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/moc_idxgui.cxx)
  SET_SOURCE_FILES_PROPERTIES(src/window.cpp PROPERTIES 
    OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/moc_window.cxx)

  QT_WRAP_CPP(idxgui IDXGUI_SRC_MOC
    include/gui_thread.h
    include/idxgui.h
    include/window.h)

  # compile library
  ##############################################################################
  add_library (idxgui SHARED
    src/gui_thread.cpp
    src/idxgui.cpp
    src/window.cpp
    src/gui.cpp
    src/scroll_box0.cpp
    src/scroll_box.cpp
   )
  
  # link executable with external libraries
  ##############################################################################
  target_link_libraries (idxgui idx)
  target_link_libraries (idxgui ${QT_QTCORE_LIBRARY})
  target_link_libraries (idxgui ${QT_QTGUI_LIBRARY})
  
ELSE (QT_FOUND) #---------------------------------------------------------------
  MESSAGE(STATUS "___WARNING___: libidxgui won't be compiled because of missing QT libraries.")
ENDIF (QT_FOUND) #--------------------------------------------------------------
