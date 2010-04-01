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
IF (QT_FOUND)
  include_directories(${LIBIDXGUI_INCLUDE_DIR})
  include_directories(${LIBEBLEARNGUI_INCLUDE_DIR})
ENDIF (QT_FOUND)

# copy scripts to bin
################################################################################
SET(FILE_TO_BIN 
  src/pascal_conf
  src/pascal_metaconf
  src/pascal_dsprepare.sh
)

FOREACH(NAME ${FILE_TO_BIN})
  EXEC_PROGRAM("cp" ARGS "\"${CMAKE_CURRENT_SOURCE_DIR}/${NAME}\""
		  "\"${LIBRARY_OUTPUT_PATH}\"" OUTPUT_VARIABLE OUT)
  MESSAGE(STATUS "Copied demos/pascal/${NAME} to bin directory.")
ENDFOREACH(NAME)

# compile executable: pascal_train
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

		
# compile executable: pascal_detect
################################################################################
add_executable (pascal_detect
  src/detect.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (pascal_detect eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (pascal_detect idxgui)
  target_link_libraries (pascal_detect eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------
IF (${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(pascal_detect ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
ELSE(${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(pascal_detect ${Boost_FILESYSTEM_LIBRARY} 
    ${Boost_REGEX_LIBRARY})
ENDIF (${Boost_MINOR_VERSION} GREATER 34)
  
