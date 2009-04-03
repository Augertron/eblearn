################################################################################
#
# CMake configuration for the mnist demo project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

include_directories(include)
include_directories(${LIBEBLEARN_INCLUDE_DIR})
include_directories(${LIBIDX_INCLUDE_DIR})
include_directories(${EBLEARN_TOOLS_INCLUDE_DIR})

# compile executable
################################################################################
add_executable (posture
  src/main.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (posture eblearn idx eblearn_tools)
  
