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

# find QT
###############################################################################
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

# find opencv
###############################################################################
FIND_PACKAGE(OpenCV)
IF (OpenCV_FOUND)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__OPENCV__")
  INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
  LINK_DIRECTORIES(${OpenCV_LIBRARY_DIRS})
ENDIF(OpenCV_FOUND)
  
# compile executable: objrec_train
################################################################################
add_executable (objrec_train
  src/train.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (objrec_train eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (objrec_train idxgui)
  target_link_libraries (objrec_train eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------

# copy conf and metaconf to bin
EXEC_PROGRAM("cp" ARGS "\"${CMAKE_CURRENT_SOURCE_DIR}/src/face_metaconf\""
		  "\"${LIBRARY_OUTPUT_PATH}\"")
EXEC_PROGRAM("cp" ARGS
  "\"${CMAKE_CURRENT_SOURCE_DIR}/src/face_dsprepare.sh\""
  "\"${LIBRARY_OUTPUT_PATH}\"")
		
# compile executable: objrec_detect
################################################################################
add_executable (objrec_detect
  src/detect.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (objrec_detect eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (objrec_detect idxgui)
  target_link_libraries (objrec_detect eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------
IF (${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(objrec_detect ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
ELSE(${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(objrec_detect ${Boost_FILESYSTEM_LIBRARY} 
    ${Boost_REGEX_LIBRARY})
ENDIF (${Boost_MINOR_VERSION} GREATER 34)
  
