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
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    FIND_PACKAGE(Boost COMPONENTS system filesystem regex)
  ENDIF(${Boost_MINOR_VERSION} GREATER 34)
  SET (CMAKE_CXX_FLAGS "-D__BOOST__")
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  ##LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
  IF (${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(eblearn_tools ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ELSE(${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(eblearn_tools ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ENDIF (${Boost_MINOR_VERSION} GREATER 34)
ENDIF(Boost_FOUND)
  
# link executable with external libraries
################################################################################
target_link_libraries (eblearn_tools eblearn idx)
