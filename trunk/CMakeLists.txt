################################################################################
#
# CMake configuration for eblearn project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

PROJECT(eblearn)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)

#IF(COMMAND CMAKE_POLICY)
#  CMAKE_POLICY(VERSION 2.6)
#ENDIF(COMMAND CMAKE_POLICY)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/scripts 
		      ${CMAKE_BINARY_DIR}/cmake ${CMAKE_MODULE_PATH})


SET(LIBEBLEARN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libeblearn/include)
SET(EBLEARN_TOOLS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eblearn/tools/include)
SET(EBLEARN_GUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eblearn/gui/include)
ADD_SUBDIRECTORY(libeblearn)
ADD_SUBDIRECTORY(eblearn)
