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
include_directories(${EBLEARN_TOOLS_INCLUDE_DIR})

# check for external libraries
################################################################################
# CPPUnit
################################################################################
FIND_PACKAGE(CPPUNIT)

IF (CPPUNIT_FOUND) #------------------------------------------------------------
  include_directories(${CPPUNIT_INCLUDE_DIR})
  
  # compile executable
  ##############################################################################
  add_executable (eblearn_tester
    src/BlasTest.cpp
    src/Classifier2DTest.cpp
    src/ClusterTest.cpp
    src/DataSourceTest.cpp
    src/EbmTest.cpp
    src/IdxIOTest.cpp
    src/IdxIteratorsTest.cpp
    src/IdxTest.cpp
    src/ImageTest.cpp
    src/NetTest.cpp
    src/main.cpp
    src/MyBriefTestProgressListener.cpp
    src/MyTextOutputter.cpp
    )

  # compilation flags
  ##############################################################################
  FIND_PACKAGE(Boost COMPONENTS filesystem regex)
  IF (Boost_FOUND)
     IF (${Boost_MINOR_VERSION} GREATER 34)
        FIND_PACKAGE(Boost COMPONENTS system filesystem regex)
     ENDIF(${Boost_MINOR_VERSION} GREATER 34)
     SET (CMAKE_CXX_FLAGS "-D__BOOST__")
  ENDIF (Boost_FOUND)
  
  # link executable with external libraries
  ##############################################################################
  target_link_libraries (eblearn_tester eblearn idx)
  target_link_libraries (eblearn_tester eblearn_tools)
  target_link_libraries (eblearn_tester ${CPPUNIT_LIBRARY})
  
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

ELSE (CPPUNIT_FOUND)
  MESSAGE("Error: CppUnit required to build eblearn_tester")
  
ENDIF (CPPUNIT_FOUND) #---------------------------------------------------------
 
