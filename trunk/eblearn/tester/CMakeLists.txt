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
  ################################################################################
  add_executable (eblearn_tester
    src/BlasTest.cpp
    src/Classifier2DTest.cpp
    src/DataSourceTest.cpp
    src/EbmTest.cpp
    src/IdxIOTest.cpp
    src/IdxIteratorsTest.cpp
    src/IdxTest.cpp
    src/ImageTest.cpp
    src/NetTest.cpp
    src/testeblearn.cpp
    src/test_Idx.cpp
    src/test_IdxIter.cpp
    )
  
  # link executable with external libraries
  ################################################################################
  target_link_libraries (eblearn_tester eblearn idx)
  target_link_libraries (eblearn_tester eblearn_tools)
  target_link_libraries (eblearn_tester ${CPPUNIT_LIBRARY})
  
  # write parameters for execution in run.init
  ################################################################################
  FILE(APPEND "${EXECUTABLE_OUTPUT_PATH}/run.init"
    "-data ${CMAKE_CURRENT_SOURCE_DIR}/../data\n")
  
ENDIF (CPPUNIT_FOUND) #---------------------------------------------------------
