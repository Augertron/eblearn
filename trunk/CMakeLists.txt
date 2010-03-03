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

endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

SET(EFENCE_FLAG "")
SET(CMAKE_CXX_FLAGS_DEBUG 
      "${EFENCE_FLAG} -g -Wall -ggdb -gdwarf-2 -Wunused-variable -Wno-long-long -Wno-unknown-pragmas -Wno-system-headers -pedantic -O0 -fsignaling-nans -fexceptions"
      CACHE STRING "Debug builds CMAKE CXX flags " FORCE )

# copy gdb config file into bin directory
EXEC_PROGRAM("cp" ARGS "\"${CMAKE_SOURCE_DIR}/.gdbinit\""
		  "\"${LIBRARY_OUTPUT_PATH}\"")
    
SET(LIBIDX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libidx/include)
SET(LIBEBLEARN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libeblearn/include)
SET(LIBEBLEARNTOOLS_INCLUDE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/libeblearntools/include)
SET(LIBIDXGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libidxgui/include)
#SET(LIBSPIDX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libspidx/include)

# find boost
###############################################################################
FIND_PACKAGE(Boost COMPONENTS filesystem regex)
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    FIND_PACKAGE(Boost COMPONENTS system filesystem regex)
  ENDIF(${Boost_MINOR_VERSION} GREATER 34)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__BOOST__")
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  ##LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)
  
# find opencv
###############################################################################
FIND_PACKAGE(OpenCV)
IF (OpenCV_FOUND)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__OPENCV__")
  INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
  LINK_DIRECTORIES(${OpenCV_LIBRARY_DIRS})
ENDIF(OpenCV_FOUND)
  
# find XML++
##############################################################################
FIND_PACKAGE(Xml++)
IF (XML_FOUND)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__XML__")
  INCLUDE_DIRECTORIES(${XML_INCLUDE_DIRS})
ENDIF (XML_FOUND)

# find QT
##############################################################################
FIND_PACKAGE(Qt4)
IF (QT_FOUND)
  include_directories(${LIBIDXGUI_INCLUDE_DIR})
  include_directories(${LIBEBLEARNGUI_INCLUDE_DIR})
  include_directories(${QT_INCLUDE_DIR})
  include_directories(${QT_QTGUI_INCLUDE_DIR})
  include_directories(${QT_QTCORE_INCLUDE_DIR})
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GUI__")
  MESSAGE(STATUS "Qt qmake: ${QT_QMAKE_EXECUTABLE}") 
  MESSAGE(STATUS "Qt lib dir: ${QT_LIBRARY_DIR}")  
  MESSAGE(STATUS "Qt include dir: ${QT_INCLUDE_DIR}")  
ENDIF (QT_FOUND)

# find CBLAS
################################################################################
FIND_PACKAGE(CBLAS)
IF(CBLAS_FOUND)
  INCLUDE_DIRECTORIES(${CBLAS_INCLUDE_DIR})
ENDIF(CBLAS_FOUND)

# find CPPUnit
################################################################################
FIND_PACKAGE(CPPUNIT)
IF (CPPUNIT_FOUND)
  include_directories(${CPPUNIT_INCLUDE_DIR})
ENDIF (CPPUNIT_FOUND)


MESSAGE(STATUS "Compilation flags: ${CMAKE_CXX_FLAGS}")

ADD_SUBDIRECTORY(libidx)
ADD_SUBDIRECTORY(libidxgui)
ADD_SUBDIRECTORY(libeblearn)
ADD_SUBDIRECTORY(libeblearngui)
ADD_SUBDIRECTORY(libeblearntools)
ADD_SUBDIRECTORY(tester)
ADD_SUBDIRECTORY(demos)

###ADD_SUBDIRECTORY(libspidx)
####ADD_SUBDIRECTORY(sptester)
