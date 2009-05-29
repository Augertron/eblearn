################################################################################
#
# CMake configuration for libeblearn_tools project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################


# add include directories
################################################################################
include_directories (include)
include_directories (${LIBIDX_INCLUDE_DIR})
include_directories (${LIBIDXGUI_INCLUDE_DIR})
include_directories (${LIBEBLEARN_INCLUDE_DIR})
include_directories (${LIBEBLEARNGUI_INCLUDE_DIR})
SET(EBLEARN_TOOLS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

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
  
# find QT
##############################################################################
FIND_PACKAGE(Qt4)
IF (QT_FOUND) #-----------------------------------------------------------------
  include_directories(${LIBIDXGUI_INCLUDE_DIR})
  include_directories(${QT_INCLUDE_DIR})
  include_directories(${QT_QTGUI_INCLUDE_DIR})
  include_directories(${QT_QTCORE_INCLUDE_DIR})
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GUI__")
ENDIF (QT_FOUND) #--------------------------------------------------------------

# compile library
################################################################################
add_library (eblearntools SHARED
  src/dataset_generation.cpp
  )

# link executable with external libraries
target_link_libraries (eblearntools eblearn idx)
IF (${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(eblearntools ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
ELSE(${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(eblearntools ${Boost_FILESYSTEM_LIBRARY} 
    ${Boost_REGEX_LIBRARY})
ENDIF (${Boost_MINOR_VERSION} GREATER 34)

IF (NOT Boost_FOUND)
  MESSAGE(STATUS "___WARNING___: libeblearntools is compiled but missing capabilities because of missing Boost libraries.")
ENDIF (NOT Boost_FOUND) #-------------------------------------------------------

# compile executable: dataset_compiler
################################################################################
add_executable (dataset_compiler
  src/dataset_compiler.cpp
  )

# link executable with external libraries
target_link_libraries (dataset_compiler eblearn idx eblearntools)
IF (QT_FOUND)
  target_link_libraries (dataset_compiler idxgui eblearngui)
ENDIF (QT_FOUND)

# compile executable: dataset_display
################################################################################
add_executable (dataset_display
  src/dataset_display.cpp
  )

# link executable with external libraries
target_link_libraries (dataset_display eblearn idx)
IF (QT_FOUND)
  target_link_libraries (dataset_display idxgui eblearngui)
ENDIF (QT_FOUND)

# compile executable: meta_trainer
################################################################################
add_executable (meta_trainer
  src/meta_trainer.cpp
  )

# link executable with external libraries
IF (${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(meta_trainer ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
ELSE(${Boost_MINOR_VERSION} GREATER 34)
  TARGET_LINK_LIBRARIES(meta_trainer ${Boost_FILESYSTEM_LIBRARY} 
    ${Boost_REGEX_LIBRARY})
ENDIF (${Boost_MINOR_VERSION} GREATER 34)


# compile executable: videoffinder
################################################################################
# IF (OpenCV_FOUND) #-------------------------------------------------------------
# add_executable (videoffinder
#   src/video_features_finder.cpp
#   src/similar_patches.cpp
#   )

# # link executable with external libraries
# ################################################################################
# target_link_libraries (videoffinder eblearn idx)
# IF (QT_FOUND)
#   target_link_libraries (videoffinder idxgui)
# ENDIF (QT_FOUND)
# IF (OpenCV_FOUND)
#   target_link_libraries (videoffinder cv highgui cxcore)
# ENDIF (OpenCV_FOUND)

# ELSE (OpenCV_FOUND)
#   MESSAGE(STATUS "___WARNING___: videoffinder won't be compiled because of missing OpenCV libraries.")
# ENDIF (OpenCV_FOUND) #----------------------------------------------------------
