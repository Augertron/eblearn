################################################################################
#
# CMake configuration for the objrec demo project
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

# compile executable: objtrain
################################################################################
add_executable (objtrain
  src/train.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (objtrain eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (objtrain idxgui)
  target_link_libraries (objtrain eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------

# copy conf and metaconf to bin
EXEC_PROGRAM("cp" ARGS "\"${CMAKE_CURRENT_SOURCE_DIR}/face/face_meta.conf\""
		  "\"${LIBRARY_OUTPUT_PATH}\"")
EXEC_PROGRAM("cp" ARGS
  "\"${CMAKE_CURRENT_SOURCE_DIR}/face/face_dsprepare.sh\""
  "\"${LIBRARY_OUTPUT_PATH}\"")
		
# compile executable: objdetect
################################################################################
add_executable (objdetect
  src/detect.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (objdetect eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (objdetect idxgui)
  target_link_libraries (objdetect eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------
IF (${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(objdetect ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
ELSE(${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(objdetect ${Boost_FILESYSTEM_LIBRARY} 
    ${Boost_REGEX_LIBRARY})
ENDIF (${Boost_MINOR_VERSION} GREATER 34)
