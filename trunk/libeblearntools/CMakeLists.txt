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
  )

IF (OpenCV_FOUND)
  target_link_libraries (eblearntools cv highgui cxcore)
ENDIF (OpenCV_FOUND)
# link executable with external libraries
target_link_libraries (eblearntools eblearn idx)
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(eblearntools ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
  ELSE(${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(eblearntools ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ENDIF (${Boost_MINOR_VERSION} GREATER 34)
ENDIF (Boost_FOUND)
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
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(dscompiler ${Boost_SYSTEM_LIBRARY}
      ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
  ELSE(${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(dscompiler ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ENDIF (${Boost_MINOR_VERSION} GREATER 34)
ENDIF (Boost_FOUND)

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
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(dssplit ${Boost_SYSTEM_LIBRARY}
      ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
  ELSE(${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(dssplit ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ENDIF (${Boost_MINOR_VERSION} GREATER 34)
ENDIF (Boost_FOUND)

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
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(dsmerge ${Boost_SYSTEM_LIBRARY}
      ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
  ELSE(${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(dsmerge ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ENDIF (${Boost_MINOR_VERSION} GREATER 34)
ENDIF (Boost_FOUND)

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
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(dsdisplay ${Boost_SYSTEM_LIBRARY}
      ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
  ELSE(${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(dsdisplay ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ENDIF (${Boost_MINOR_VERSION} GREATER 34)
ENDIF (Boost_FOUND)

# compile executable: matdisplay
################################################################################
add_executable (matdisplay
  src/matdisplay.cpp
  )

# link executable with external libraries
target_link_libraries (matdisplay idx)
IF (QT_FOUND)
  target_link_libraries (matdisplay idxgui)
ENDIF (QT_FOUND)

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
add_executable (metarun
  src/metarun.cpp
  src/metajobs.cpp
  )

# link executable with external libraries
target_link_libraries (metarun idx eblearntools)
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(metarun ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
  ELSE(${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(metarun ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ENDIF (${Boost_MINOR_VERSION} GREATER 34)
ENDIF (Boost_FOUND)

# compile executable: metaparse
################################################################################
add_executable (metaparse
  src/metaparser.cpp
  src/metaparse.cpp
  )

# link executable with external libraries
target_link_libraries (metaparse eblearntools)
IF (Boost_FOUND)
  IF (${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(metaparse ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
  ELSE(${Boost_MINOR_VERSION} GREATER 34)
    TARGET_LINK_LIBRARIES(metaparse ${Boost_FILESYSTEM_LIBRARY} 
      ${Boost_REGEX_LIBRARY})
  ENDIF (${Boost_MINOR_VERSION} GREATER 34)
ENDIF (Boost_FOUND)

EXEC_PROGRAM("cp" ARGS "\"${CMAKE_CURRENT_SOURCE_DIR}/src/meta_plot.sh\""
		  "\"${LIBRARY_OUTPUT_PATH}\"")

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
