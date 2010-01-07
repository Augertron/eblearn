################################################################################
#
# CMake configuration for the pascal demo project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

include_directories(include)
include_directories(${LIBEBLEARN_INCLUDE_DIR})
include_directories(${LIBEBLEARNTOOLS_INCLUDE_DIR})
include_directories(${LIBIDX_INCLUDE_DIR})

FIND_PACKAGE(Qt4)
IF (QT_FOUND) #-----------------------------------------------------------------
  # add include directories
  ##############################################################################
  include_directories(${LIBIDXGUI_INCLUDE_DIR})
  include_directories(${LIBEBLEARNGUI_INCLUDE_DIR})
  include_directories(${QT_INCLUDE_DIR})
  include_directories(${QT_QTGUI_INCLUDE_DIR})
  include_directories(${QT_QTCORE_INCLUDE_DIR})
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GUI__")
ENDIF (QT_FOUND) #--------------------------------------------------------------

# compile executable
################################################################################
add_executable (pascal_train
  src/train.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (pascal_train eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (pascal_train idxgui)
  target_link_libraries (pascal_train eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------

# copy conf and metaconf to bin
EXEC_PROGRAM("cp" ARGS "\"${CMAKE_CURRENT_SOURCE_DIR}/src/pascal_conf\""
		  "\"${LIBRARY_OUTPUT_PATH}\"")		
EXEC_PROGRAM("cp" ARGS "\"${CMAKE_CURRENT_SOURCE_DIR}/src/pascal_metaconf\""
		  "\"${LIBRARY_OUTPUT_PATH}\"")
EXEC_PROGRAM("cp" ARGS
  "\"${CMAKE_CURRENT_SOURCE_DIR}/src/pascal_dscompiler.sh\""
  "\"${LIBRARY_OUTPUT_PATH}\"")
EXEC_PROGRAM("cp" ARGS
  "\"${CMAKE_CURRENT_SOURCE_DIR}/src/pascal_dssplit.sh\""
  "\"${LIBRARY_OUTPUT_PATH}\"")
		
# compile executable
################################################################################
add_executable (pascal_reco
  src/reco.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (pascal_reco eblearn idx)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (pascal_reco idxgui)
  target_link_libraries (pascal_reco eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------
  
