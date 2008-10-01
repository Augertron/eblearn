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
include_directories(${EBLEARN_TOOLS_INCLUDE_DIR})
include_directories(${EBLEARN_GUI_INCLUDE_DIR})

# do we have gui
###############################################################################
FIND_PACKAGE(Qt4)
IF (QT_FOUND) #-----------------------------------------------------------------
  include_directories (${QT_INCLUDE_DIR})
  include_directories (${QT_QTGUI_INCLUDE_DIR})
  include_directories (${QT_QTCORE_INCLUDE_DIR})
  # set __GUI__variable
  ##############################################################################
  SET(USE_EBLEARN_GUI 1)
  INCLUDE_DIRECTORIES(${EBLEARN_GUI_INCLUDE_DIR})
ENDIF (QT_FOUND) #--------------------------------------------------------------
CONFIGURE_FILE("include/EbmGuiTest.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/EbmGuiTest.h")

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
  target_link_libraries (eblearn_tester eblearn)
  target_link_libraries (eblearn_tester eblearn_tools)
  IF(QT_FOUND)
    target_link_libraries (eblearn_tester eblearn_gui)
  ENDIF(QT_FOUND)
  target_link_libraries (eblearn_tester ${CPPUNIT_LIBRARY})
  
  # write parameters for execution in run.init
  ################################################################################
  FILE(APPEND "${EXECUTABLE_OUTPUT_PATH}/run.init"
    "-data ${CMAKE_CURRENT_SOURCE_DIR}/../data\n")
  
ENDIF (CPPUNIT_FOUND) #---------------------------------------------------------
