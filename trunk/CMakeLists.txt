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

IF(COMMAND CMAKE_POLICY)
  #  CMAKE_POLICY(VERSION 2.6)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/scripts 
  ${CMAKE_BINARY_DIR}/cmake ${CMAKE_MODULE_PATH})

# All modes compilation flags
SET(CMAKE_CXX_FLAGS "-pthread")

# Release compilation flags
IF (CMAKE_BUILD_TYPE STREQUAL "Release")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} "
    CACHE STRING "Release builds CMAKE CXX flags " FORCE )
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")

# check for required and optional external packages
FIND_PACKAGE(All)

# Check that electric-fence and valgrind are installed for (debug mode only)
SET(EFENCE_FLAG "")
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
  FIND_PACKAGE(EFence)
  IF (NOT EFENCE_FOUND) 
    MESSAGE("*** Please install electric-fence to catch memory-related bugs in debug mode ***\n")
  ELSEIF (EFENCE_FOUND) 
    SET(EFENCE_FLAG "-lefence")
  ENDIF (NOT EFENCE_FOUND) 

  FIND_PACKAGE(Valgrind)
  IF (NOT VALGRIND_FOUND) 
    MESSAGE("*** Please install valgrind to catch memory-related bugs in debug mode ***")
    MESSAGE("*** Call 'valgrind ./myprogram' at least once before committing ***\n")
  ENDIF (NOT VALGRIND_FOUND) 

# Debug compilation flags
SET(EFENCE_FLAG "")
SET(CMAKE_CXX_FLAGS_DEBUG 
  "${EFENCE_FLAG} -D__DEBUG__ -g -Wall -ggdb -gdwarf-2 -Wunused-variable -Wno-long-long -Wno-unknown-pragmas -Wno-system-headers -pedantic -O0 -fsignaling-nans -fexceptions"
  CACHE STRING "Debug builds CMAKE CXX flags " FORCE )

# display debug flags
MESSAGE(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")

endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# display regular flags
MESSAGE(STATUS "Compilation flags: ${CMAKE_CXX_FLAGS}")

# copy gdb config file into bin directory
EXEC_PROGRAM("cp" ARGS "\"${CMAKE_SOURCE_DIR}/.gdbinit\""
		  "\"${LIBRARY_OUTPUT_PATH}\"" OUTPUT_VARIABLE OUT)
    
SET(LIBIDX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libidx/include)
SET(LIBEBLEARN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libeblearn/include)
SET(LIBEBLEARNTOOLS_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/libeblearntools/include)
SET(LIBIDXGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libidxgui/include)
#SET(LIBSPIDX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libspidx/include)

ADD_SUBDIRECTORY(libidx)
ADD_SUBDIRECTORY(libidxgui)
ADD_SUBDIRECTORY(libeblearn)
ADD_SUBDIRECTORY(libeblearngui)
ADD_SUBDIRECTORY(libeblearntools)
ADD_SUBDIRECTORY(tester)
ADD_SUBDIRECTORY(demos)

###ADD_SUBDIRECTORY(libspidx)
####ADD_SUBDIRECTORY(sptester)
