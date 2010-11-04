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

# compile library
################################################################################
add_library (eblearntools SHARED
  src/configuration.cpp
  src/gdb.cpp
  src/tools_utils.cpp
  src/opencv.cpp
  src/metaparser.cpp
  src/sort.cpp
  src/thread.cpp
  src/netconf.cpp
#  src/VFWCapture.cpp
  )
# link executable with external libraries
target_link_libraries (eblearntools eblearn idx)
LINK_OPENCV(eblearntools cv)
LINK_OPENCV(eblearntools highgui)
LINK_OPENCV(eblearntools cxcore)
LINK_BOOST(eblearntools system)
LINK_BOOST(eblearntools filesystem)
LINK_BOOST(eblearntools regex)
LINK_QT(eblearntools idxgui)
LINK_QT(eblearntools eblearngui)

# compile executable: dscompiler
################################################################################
add_executable (dscompiler
  src/dscompiler.cpp
  src/dataset.cpp
  src/xml_utils.cpp
  )
# link executable with external libraries
target_link_libraries (dscompiler idx eblearn eblearntools)
LINK_XML(dscompiler)
LINK_QT(dscompiler idxgui)
LINK_BOOST(dscompiler system)
LINK_BOOST(dscompiler filesystem)
LINK_BOOST(dscompiler regex)

# compile executable: dssplit
################################################################################
add_executable (dssplit
  src/dssplit.cpp
  src/dataset.cpp
  )
# link executable with external libraries
target_link_libraries (dssplit eblearn idx)
LINK_QT(dssplit idxgui)
LINK_QT(dssplit eblearngui)
LINK_BOOST(dssplit system)
LINK_BOOST(dssplit filesystem)
LINK_BOOST(dssplit regex)

# compile executable: dsmerge
################################################################################
add_executable (dsmerge
  src/dsmerge.cpp
  src/dataset.cpp
  )
# link executable with external libraries
target_link_libraries (dsmerge eblearn idx)
LINK_QT(dsmerge idxgui)
LINK_QT(dsmerge eblearngui)
LINK_BOOST(dsmerge system)
LINK_BOOST(dsmerge filesystem)
LINK_BOOST(dsmerge regex)

# compile executable: dsdisplay
################################################################################
add_executable (dsdisplay
  src/dsdisplay.cpp
  src/dataset.cpp
  )
# link executable with external libraries
target_link_libraries (dsdisplay eblearn idx)
LINK_QT(dsdisplay idxgui)
LINK_QT(dsdisplay eblearngui)
LINK_BOOST(dsdisplay system)
LINK_BOOST(dsdisplay filesystem)
LINK_BOOST(dsdisplay regex)

# compile executable: matshow
################################################################################
# tell compiler to build WinMain application, not a console app
IF (WINDOWS AND NOT QT_FOUND) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating matshow project because of missing QT")
ELSE (WINDOWS AND NOT QT_FOUND) # do not compile this project under windows

IF (WINDOWS)
  SET(WINTYPE WIN32)
  MESSAGE(STATUS "Building matshow as a windowed application (not console)")
ENDIF (WINDOWS)

add_executable (matshow ${WINTYPE}
  src/matshow.cpp
  )
# link executable with external libraries
target_link_libraries (matshow idx eblearntools)
LINK_QT(matshow idxgui)

ENDIF (WINDOWS AND NOT QT_FOUND)

# compile executable: capture
################################################################################
add_executable (capture
  src/capture.cpp
  )
# link executable with external libraries
target_link_libraries (capture idx eblearntools)
LINK_QT(capture idxgui)

# compile executable: metarun
################################################################################
IF (WINDOWS) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating metarun project under Windows")
ELSE (WINDOWS) # do not compile this project under windows
add_executable (metarun
  src/metarun.cpp
  src/metajobs.cpp
  )
# link executable with external libraries
target_link_libraries (metarun idx eblearntools)
LINK_BOOST(metarun system)
LINK_BOOST(metarun filesystem)
LINK_BOOST(metarun regex)
ENDIF (WINDOWS) # do not compile this project under windows

# compile executable: metaparse
################################################################################
add_executable (metaparse
  src/metaparser.cpp
  src/metaparse.cpp
  )
# link executable with external libraries
target_link_libraries (metaparse eblearntools)
LINK_BOOST(metaparse system)
LINK_BOOST(metaparse filesystem)
LINK_BOOST(metaparse regex)

# compile executable: maketable
################################################################################
add_executable (maketable
  src/maketable.cpp
  )
# link executable with external libraries
target_link_libraries (maketable idx eblearn)

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
