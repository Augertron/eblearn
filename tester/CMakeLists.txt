################################################################################
#
# CMake configuration for eblearn_tester project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

# add include directories
################################################################################
include_directories (include)
include_directories(${LIBEBLEARN_INCLUDE_DIR})
include_directories(${LIBIDX_INCLUDE_DIR})
include_directories(${LIBEBLEARN_TOOLS_INCLUDE_DIR})
IF (QT_FOUND)
  include_directories(${LIBIDXGUI_INCLUDE_DIR})
  include_directories(${LIBEBLEARNGUI_INCLUDE_DIR})
ENDIF (QT_FOUND)

IF (CPPUNIT_FOUND) # compile only if cppunit is present
  
  # compile executable
  ##############################################################################
  add_executable (tester
    src/idxops_test.cpp
    src/detector_test.cpp
    src/ClusterTest.cpp
    src/DataSourceTest.cpp
    src/ebl_basic_test.cpp
    src/ebl_preprocessing_test.cpp
    src/idxIO_test.cpp
    src/IdxIteratorsTest.cpp
    src/IdxTest.cpp
    src/image_test.cpp
    src/NetTest.cpp
    src/main.cpp
    src/MyBriefTestProgressListener.cpp
    src/MyTextOutputter.cpp
    )

  # link executable with external libraries
  ##############################################################################
  IF (NOT WINDOWS)
    target_link_libraries (tester dl)
  ENDIF (NOT WINDOWS)
  target_link_libraries (tester eblearn idx)
  target_link_libraries (tester eblearntools)
  target_link_libraries (tester ${CPPUNIT_LIBRARY})
  IF (QT_FOUND) #---------------------------------------------------------------
    target_link_libraries (tester idxgui)
    target_link_libraries (tester eblearngui)
  ENDIF (QT_FOUND) #------------------------------------------------------------
  
  # write parameters for execution in run.init
  ##############################################################################
  IF (EXISTS "${EXECUTABLE_OUTPUT_PATH}/../run.init")
    FILE(READ "${EXECUTABLE_OUTPUT_PATH}/../run.init" TEMP1)
    STRING(REGEX MATCH "[-]data .*[.][.]/data" MATCHED "${TEMP1}")
    IF (MATCHED)
      STRING(REGEX REPLACE "[-]data .*[.][.]/data" 
	"-data ${CMAKE_CURRENT_SOURCE_DIR}/../data" TEMP2 "${TEMP1}")
      FILE(WRITE "${EXECUTABLE_OUTPUT_PATH}/../run.init" "${TEMP2}") 
    ELSE (MATCHED)
      SET(TEMP2 "-data ${CMAKE_CURRENT_SOURCE_DIR}/../data")
      FILE(APPEND "${EXECUTABLE_OUTPUT_PATH}/../run.init" "${TEMP2}") 
    ENDIF (MATCHED)
  ELSE (EXISTS "${EXECUTABLE_OUTPUT_PATH}/../run.init")
    SET(TEMP2 "-data ${CMAKE_CURRENT_SOURCE_DIR}/../data")
    FILE(WRITE "${EXECUTABLE_OUTPUT_PATH}/../run.init" "${TEMP2}") 
  ENDIF (EXISTS "${EXECUTABLE_OUTPUT_PATH}/../run.init")
  IF (WINDOWS)
    IF (MNIST_PATH)
      MESSAGE(STATUS "Using MNIST path: ${MNIST_PATH}")
      SET (TEMP2 "${TEMP2}\n-mnist ${MNIST_PATH}")
    ELSE (MNIST_PATH)
      MESSAGE("__ Warning: MNIST location unknown, set MNIST_PATH variable to your MNIST path.")
    ENDIF (MNIST_PATH)
    FILE(WRITE "${EXECUTABLE_OUTPUT_PATH}/run.init" "${TEMP2}")
  ENDIF (WINDOWS)

ELSE (CPPUNIT_FOUND)
  MESSAGE("Error: CppUnit required to build tester")
  
ENDIF (CPPUNIT_FOUND) #---------------------------------------------------------
 
