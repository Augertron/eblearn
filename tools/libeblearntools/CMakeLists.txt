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
#  src/VFWCapture.cpp
  )

IF (OpenCV_FOUND)
  target_link_libraries (eblearntools cv highgui cxcore)
ENDIF (OpenCV_FOUND)
# link executable with external libraries
target_link_libraries (eblearntools eblearn idx)
LINK_BOOST(eblearntools)
IF (QT_FOUND)
  target_link_libraries (eblearntools idxgui eblearngui)
ENDIF (QT_FOUND)

# IF (NOT Boost_FOUND)
#   MESSAGE(STATUS "___WARNING___: libeblearntools is compiled but missing capabilities because of missing Boost libraries.")
# ENDIF (NOT Boost_FOUND) #-------------------------------------------------------

# compile executable: dscompiler
################################################################################
add_executable (dscompiler
  src/dscompiler.cpp
  src/dataset.cpp
  src/xml_utils.cpp
  )

# link executable with external libraries
target_link_libraries (dscompiler idx eblearn eblearntools)
IF (QT_FOUND)
  target_link_libraries (dscompiler idxgui)
ENDIF (QT_FOUND)
IF (XML_FOUND)
  target_link_libraries (dscompiler ${XML_LIBRARY_FLAGS})
ENDIF (XML_FOUND)
LINK_BOOST(dscompiler)

# compile executable: dssplit
################################################################################
add_executable (dssplit
  src/dssplit.cpp
  src/dataset.cpp
  )

# link executable with external libraries
target_link_libraries (dssplit eblearn idx)
IF (QT_FOUND)
  target_link_libraries (dssplit idxgui eblearngui)
ENDIF (QT_FOUND)
LINK_BOOST(dssplit)

# compile executable: dsmerge
################################################################################
add_executable (dsmerge
  src/dsmerge.cpp
  src/dataset.cpp
  )

# link executable with external libraries
target_link_libraries (dsmerge eblearn idx)
IF (QT_FOUND)
  target_link_libraries (dsmerge idxgui eblearngui)
ENDIF (QT_FOUND)
LINK_BOOST(dsmerge)

# compile executable: dsdisplay
################################################################################
add_executable (dsdisplay
  src/dsdisplay.cpp
  src/dataset.cpp
  )

# link executable with external libraries
target_link_libraries (dsdisplay eblearn idx)
IF (QT_FOUND)
  target_link_libraries (dsdisplay idxgui eblearngui)
ENDIF (QT_FOUND)
LINK_BOOST(dsdisplay)

IF (WINDOWS AND NOT QT_FOUND) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating matshow project because of missing QT")
ELSE (WINDOWS AND NOT QT_FOUND) # do not compile this project under windows
# compile executable: matshow
################################################################################
# tell compiler to build WinMain application, not a console app
IF (WINDOWS)
  SET(WINTYPE WIN32)
  MESSAGE(STATUS "Building matshow as a windowed application (not console)")
ENDIF (WINDOWS)

add_executable (matshow ${WINTYPE}
  src/matshow.cpp
  )

# link executable with external libraries
target_link_libraries (matshow idx)
target_link_libraries (matshow eblearntools)
IF (QT_FOUND)
  target_link_libraries (matshow idxgui)
ENDIF (QT_FOUND)

ENDIF (WINDOWS AND NOT QT_FOUND)

# compile executable: capture
################################################################################
add_executable (capture
  src/capture.cpp
  )

# link executable with external libraries
target_link_libraries (capture idx eblearntools)
IF (QT_FOUND)
  target_link_libraries (capture idxgui)
ENDIF (QT_FOUND)


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
LINK_BOOST(metarun)
ENDIF (WINDOWS) # do not compile this project under windows

# compile executable: metaparse
################################################################################
add_executable (metaparse
  src/metaparser.cpp
  src/metaparse.cpp
  )

# link executable with external libraries
target_link_libraries (metaparse eblearntools)
LINK_BOOST(metaparse)

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
