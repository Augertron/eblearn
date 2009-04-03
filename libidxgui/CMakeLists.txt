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
SET(IDXGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

FIND_PACKAGE(Qt4)
IF (QT_FOUND) #-----------------------------------------------------------------

  # add include directories
  ##############################################################################
  include_directories (${QT_INCLUDE_DIR})
  include_directories (${QT_QTGUI_INCLUDE_DIR})
  include_directories (${QT_QTCORE_INCLUDE_DIR})

  # QT specific: produce moc files from headers
  ##############################################################################
  # TODO: make this automatic
  EXEC_PROGRAM("moc" ARGS "${CMAKE_CURRENT_SOURCE_DIR}/include/GuiThread.h >       ${CMAKE_CURRENT_SOURCE_DIR}/include/GuiThread.moc" OUTPUT_VARIABLE MOCOK)
  EXEC_PROGRAM("moc" ARGS "${CMAKE_CURRENT_SOURCE_DIR}/include/RenderThread.h > ${CMAKE_CURRENT_SOURCE_DIR}/include/RenderThread.moc" OUTPUT_VARIABLE MOCOK)
  EXEC_PROGRAM("moc" ARGS "${CMAKE_CURRENT_SOURCE_DIR}/include/Window.h > ${CMAKE_CURRENT_SOURCE_DIR}/include/Window.moc" OUTPUT_VARIABLE MOCOK)

  # compile library
  ##############################################################################
  add_library (idxgui SHARED
    src/GuiThread.cpp
    src/RenderThread.cpp
    src/Window.cpp
    )
  
  # link executable with external libraries
  ##############################################################################
  target_link_libraries (idxgui idx)
  target_link_libraries (idxgui ${QT_QTCORE_LIBRARY})
  target_link_libraries (idxgui ${QT_QTGUI_LIBRARY})
  
ENDIF (QT_FOUND) #--------------------------------------------------------------
