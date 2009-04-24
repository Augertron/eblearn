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

FIND_PACKAGE(Qt4)
IF (QT_FOUND) #-----------------------------------------------------------------

  # add include directories
  ##############################################################################
  include_directories (${QT_INCLUDE_DIR})
  include_directories (${QT_QTGUI_INCLUDE_DIR})
  include_directories (${QT_QTCORE_INCLUDE_DIR})

  # QT specific: produce moc files from headers
  ##############################################################################
#  IF (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/gui_thread.moc")
    EXEC_PROGRAM("moc" ARGS "${CMAKE_CURRENT_SOURCE_DIR}/include/gui_thread.h > ${CMAKE_CURRENT_SOURCE_DIR}/include/gui_thread.moc" OUTPUT_VARIABLE MOCOK)
#   ENDIF (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/gui_thread.moc")
#   ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/include/gui_thread.moc" COMMAND "moc" "${CMAKE_CURRENT_SOURCE_DIR}/include/gui_thread.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/gui_thread.moc" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/gui_thread.h")

#   IF (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/idxgui.moc")
    EXEC_PROGRAM("moc" ARGS "${CMAKE_CURRENT_SOURCE_DIR}/include/idxgui.h > ${CMAKE_CURRENT_SOURCE_DIR}/include/idxgui.moc" OUTPUT_VARIABLE MOCOK)
#   ENDIF (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/idxgui.moc")
#   ADD_CUSTOM_COMMAND(
#     OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/idxgui.h
#     COMMAND "moc" ${CMAKE_CURRENT_SOURCE_DIR}/include/idxgui.h ${CMAKE_CURRENT_SOURCE_DIR}/include/idxgui.moc 
#     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/idxgui.moc)

#   IF (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/window.moc")
    EXEC_PROGRAM("moc" ARGS "${CMAKE_CURRENT_SOURCE_DIR}/include/window.h > ${CMAKE_CURRENT_SOURCE_DIR}/include/window.moc" OUTPUT_VARIABLE MOCOK)
#   ENDIF (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/window.moc")
#   ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/include/window.moc" COMMAND "moc" "${CMAKE_CURRENT_SOURCE_DIR}/include/window.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/window.moc" DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/window.h")

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
