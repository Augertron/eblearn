################################################################################
#
# CMake configuration for tools
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#   Soumith Chintala, soumith@gmail.com, New York University
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

# compile executable: train
################################################################################
# change target name if debugging	
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (TRAIN_BINARY_NAME train_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (TRAIN_BINARY_NAME train)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
 add_executable (${TRAIN_BINARY_NAME} src/train.cpp)
 # link executable with external libraries
 target_link_libraries (${TRAIN_BINARY_NAME} idx eblearn eblearntools ${GPROF_LIBRARIES})
 LINK_QT(${TRAIN_BINARY_NAME} idxgui)
 LINK_QT(${TRAIN_BINARY_NAME} eblearngui)
 
# compile executable: dsfprop
################################################################################
add_executable (dsfprop src/dsfprop.cpp)
# link executable with external libraries
target_link_libraries (dsfprop eblearn idx eblearntools)
LINK_QT(dsfprop idxgui)
LINK_QT(dsfprop eblearngui)
LINK_MAGICKPP(dsfprop)

# compile executable: imfprop
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (IMFPROP_BINARY_NAME imfprop_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (IMFPROP_BINARY_NAME imfprop)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
add_executable (${IMFPROP_BINARY_NAME} src/imfprop.cpp)
# link executable with external libraries
target_link_libraries (${IMFPROP_BINARY_NAME} eblearn idx eblearntools)
LINK_QT(${IMFPROP_BINARY_NAME} idxgui)
LINK_QT(${IMFPROP_BINARY_NAME} eblearngui)
LINK_MAGICKPP(${IMFPROP_BINARY_NAME})

# # compile executable: stdetect
# ################################################################################
# add_executable (stdetect src/stdetect.cpp)
# # link executable with external libraries
# target_link_libraries (stdetect eblearn idx eblearntools)
# LINK_QT(stdetect idxgui)
# LINK_QT(stdetect eblearngui)
# LINK_BOOST(stdetect system)
# LINK_BOOST(stdetect filesystem)
# LINK_BOOST(stdetect regex)
# LINK_MAGICKPP(stdetect)

# compile executable: detect
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DETECT_BINARY_NAME detect_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DETECT_BINARY_NAME detect)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
add_executable (${DETECT_BINARY_NAME} src/detect.cpp)
# link executable with external libraries
target_link_libraries (${DETECT_BINARY_NAME} eblearn idx eblearntools)
LINK_QT(${DETECT_BINARY_NAME} idxgui)
LINK_QT(${DETECT_BINARY_NAME} eblearngui)
LINK_BOOST(${DETECT_BINARY_NAME} system)
LINK_BOOST(${DETECT_BINARY_NAME} filesystem)
LINK_BOOST(${DETECT_BINARY_NAME} regex)
LINK_MAGICKPP(${DETECT_BINARY_NAME})
LINK_LUA(${DETECT_BINARY_NAME})

# compile executable: mpidetect
################################################################################
IF (MPI_FOUND AND Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)
IF (WINDOWS) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating mpidetect project under Windows")
ELSE (WINDOWS) # do not compile this project under windows
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (MPIDETECT_BINARY_NAME mpidetect_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (MPIDETECT_BINARY_NAME mpidetect)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${MPIDETECT_BINARY_NAME} src/mpidetect.cpp)
# link executable with external libraries
target_link_libraries (${MPIDETECT_BINARY_NAME} eblearn idx eblearntools)
LINK_QT(${MPIDETECT_BINARY_NAME} idxgui)
LINK_QT(${MPIDETECT_BINARY_NAME} eblearngui)
LINK_BOOST(${MPIDETECT_BINARY_NAME} system)
LINK_BOOST(${MPIDETECT_BINARY_NAME} filesystem)
LINK_BOOST(${MPIDETECT_BINARY_NAME} regex)
LINK_BOOST(${MPIDETECT_BINARY_NAME} serialization)
LINK_BOOST(${MPIDETECT_BINARY_NAME} mpi)
#LINK_MPI(${MPIDETECT_BINARY_NAME})

ENDIF (WINDOWS) # do not compile this project under windows
ELSE (MPI_FOUND AND Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)
  MESSAGE("__ WARNING: mpidetect will not be compiled because of missing MPI libraries (optional).")
ENDIF (MPI_FOUND AND Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)

# # compile executable: track
# ################################################################################
# add_executable (track
#   src/track.cpp
#   )

# # link executable with external libraries
# target_link_libraries (track eblearn idx eblearntools)
# LINK_QT(track idxgui)
# LINK_QT(track eblearngui)
# LINK_BOOST(track system)
# LINK_BOOST(track filesystem)
# LINK_BOOST(track regex)

# compile executable: dscompile
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DSCOMPILE_BINARY_NAME dscompile_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DSCOMPILE_BINARY_NAME dscompile)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${DSCOMPILE_BINARY_NAME} src/dscompile.cpp)
# link executable with external libraries
target_link_libraries (${DSCOMPILE_BINARY_NAME} idx eblearn eblearntools)
LINK_QT(${DSCOMPILE_BINARY_NAME} idxgui)
LINK_BOOST(${DSCOMPILE_BINARY_NAME} system)
LINK_BOOST(${DSCOMPILE_BINARY_NAME} filesystem)
LINK_BOOST(${DSCOMPILE_BINARY_NAME} regex)
LINK_MAGICKPP(${DSCOMPILE_BINARY_NAME})

# compile executable: dssplit
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DSSPLIT_BINARY_NAME dssplit_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DSSPLIT_BINARY_NAME dssplit)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${DSSPLIT_BINARY_NAME} src/dssplit.cpp)
# link executable with external libraries
target_link_libraries (${DSSPLIT_BINARY_NAME} eblearn idx eblearntools)
LINK_QT(${DSSPLIT_BINARY_NAME} idxgui)
LINK_QT(${DSSPLIT_BINARY_NAME} eblearngui)
LINK_BOOST(${DSSPLIT_BINARY_NAME} system)
LINK_BOOST(${DSSPLIT_BINARY_NAME} filesystem)
LINK_BOOST(${DSSPLIT_BINARY_NAME} regex)
LINK_MAGICKPP(${DSSPLIT_BINARY_NAME})

# compile executable: dsmerge
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DSMERGE_BINARY_NAME dsmerge_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DSMERGE_BINARY_NAME dsmerge)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${DSMERGE_BINARY_NAME} src/dsmerge.cpp)
# link executable with external libraries
target_link_libraries (${DSMERGE_BINARY_NAME} eblearn idx eblearntools)
LINK_QT(${DSMERGE_BINARY_NAME} idxgui)
LINK_QT(${DSMERGE_BINARY_NAME} eblearngui)
LINK_BOOST(${DSMERGE_BINARY_NAME} system)
LINK_BOOST(${DSMERGE_BINARY_NAME} filesystem)
LINK_BOOST(${DSMERGE_BINARY_NAME} regex)
LINK_MAGICKPP(${DSMERGE_BINARY_NAME})

# compile executable: dsdisplay
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DSDISPLAY_BINARY_NAME dsdisplay_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (DSDISPLAY_BINARY_NAME dsdisplay)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${DSDISPLAY_BINARY_NAME} src/dsdisplay.cpp)
# link executable with external libraries
target_link_libraries (${DSDISPLAY_BINARY_NAME} eblearn idx eblearntools)
LINK_QT(${DSDISPLAY_BINARY_NAME} idxgui)
LINK_QT(${DSDISPLAY_BINARY_NAME} eblearngui)
LINK_BOOST(${DSDISPLAY_BINARY_NAME} system)
LINK_BOOST(${DSDISPLAY_BINARY_NAME} filesystem)
LINK_BOOST(${DSDISPLAY_BINARY_NAME} regex)

# compile executable: matshow
################################################################################
# tell compiler to build WinMain application, not a console app
IF (WINDOWS AND NOT QT_FOUND) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating matshow project because of missing QT")
ELSE (WINDOWS AND NOT QT_FOUND) # do not compile this project under windows

IF (WINDOWS)
#  SET(WINTYPE WIN32)
#  MESSAGE(STATUS "Building matshow as a windowed application (not console)")
ENDIF (WINDOWS)

add_executable (matshow ${WINTYPE} src/matshow.cpp)
# link executable with external libraries
target_link_libraries (matshow idx eblearntools)
LINK_QT(matshow idxgui)
LINK_QT(matshow eblearngui)
LINK_MAGICKPP(matshow)

ENDIF (WINDOWS AND NOT QT_FOUND)

# compile executable: capture
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (CAPTURE_BINARY_NAME capture_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (CAPTURE_BINARY_NAME capture)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${CAPTURE_BINARY_NAME} src/capture.cpp)
# link executable with external libraries
target_link_libraries (${CAPTURE_BINARY_NAME} idx eblearntools)
LINK_QT(${CAPTURE_BINARY_NAME} idxgui)

# compile executable: metarun
################################################################################
IF (WINDOWS) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating metarun project under Windows")
ELSE (WINDOWS) # do not compile this project under windows
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (METARUN_BINARY_NAME metarun_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (METARUN_BINARY_NAME metarun)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${METARUN_BINARY_NAME}
  src/metarun.cpp
  )
# link executable with external libraries
target_link_libraries (${METARUN_BINARY_NAME} idx eblearntools)
#LINK_BOOST(${METARUN_BINARY_NAME} system)
#LINK_BOOST(${METARUN_BINARY_NAME} filesystem)
#LINK_BOOST(${METARUN_BINARY_NAME} regex)
ENDIF (WINDOWS) # do not compile this project under windows

# compile executable: metaparse
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (METAPARSE_BINARY_NAME metaparse_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (METAPARSE_BINARY_NAME metaparse)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${METAPARSE_BINARY_NAME} src/metaparse.cpp)
# link executable with external libraries
target_link_libraries (${METAPARSE_BINARY_NAME} eblearntools)
LINK_BOOST(${METAPARSE_BINARY_NAME} system)
LINK_BOOST(${METAPARSE_BINARY_NAME} filesystem)
LINK_BOOST(${METAPARSE_BINARY_NAME} regex)

# compile executable: maketable
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (MAKETABLE_BINARY_NAME maketable_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (MAKETABLE_BINARY_NAME maketable)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${MAKETABLE_BINARY_NAME} src/maketable.cpp)
# link executable with external libraries
target_link_libraries (${MAKETABLE_BINARY_NAME} idx eblearn eblearntools)

# compile executable: narrow
################################################################################
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (NARROW_BINARY_NAME narrow_debug)
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
   SET (NARROW_BINARY_NAME narrow)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (${NARROW_BINARY_NAME} src/narrow.cpp)
# link executable with external libraries
target_link_libraries (${NARROW_BINARY_NAME} idx)

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
