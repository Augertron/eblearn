################################################################################
#
# CMake configuration for libeblearn_tools project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

# compile library
################################################################################
add_library (eblearn_tools SHARED
		src/DataTools.cpp
		)

# add include directories
################################################################################
include_directories (include)
include_directories (${LIBIDX_INCLUDE_DIR})
include_directories (${LIBEBLEARN_INCLUDE_DIR})
SET(EBLEARN_TOOLS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
# find boost
###############################################################################
FIND_PACKAGE(Boost COMPONENTS filesystem regex)
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(eblearn_tools ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
ENDIF(Boost_FOUND)
  
# link executable with external libraries
################################################################################
target_link_libraries (eblearn_tools eblearn idx)
