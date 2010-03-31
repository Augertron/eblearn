# determine machine architecture
###############################################################################
EXEC_PROGRAM("uname -m" OUTPUT_VARIABLE ARCH_NAME)
MESSAGE(STATUS "Target architecture is ${ARCH_NAME}")
STRING(COMPARE EQUAL ${ARCH_NAME} "x86_64" 64BIT)

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
  
# find XML++
##############################################################################
FIND_PACKAGE(Xml++)
IF (XML_FOUND)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__XML__")
  INCLUDE_DIRECTORIES(${XML_INCLUDE_DIRS})
ENDIF (XML_FOUND)

# find QT
##############################################################################
SET(DESIRED_QT_VERSION 4.3)
INCLUDE(FindQt)
SET(DESIRED_QT_VERSION 4.3)
IF (QT4_INSTALLED)
  SET(QT_QMAKE_EXECUTABLE "/usr/local/pkg/qt/4.6.2/bin/qmake")
  IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
    SET(QT_QMAKE_EXECUTABLE "/usr/lib64/qt4/bin/qmake")
    IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
      SET(QT_QMAKE_EXECUTABLE "/usr/share/qt4/bin/qmake")
    ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
  ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})

  INCLUDE(FindQt4)
#  INCLUDE(${QT_USE_FILE})
  SET(QT_FOUND TRUE)
  exec_program(${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_HEADERS"
        OUTPUT_VARIABLE QT_INCLUDE_DIR)
  SET(QT_QTGUI_INCLUDE_DIR ${QT_INCLUDE_DIR})
  SET(QT_QTGUI_LIBRARY "${QT_LIBRARY_DIR}/libQtGui.so")
ENDIF (QT4_INSTALLED)

IF (QT_FOUND)
  include_directories(${QT_INCLUDE_DIR})
  include_directories(${QT_QTGUI_INCLUDE_DIR})
  include_directories(${QT_QTCORE_INCLUDE_DIR})
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GUI__")
  MESSAGE(STATUS "Qt qmake: ${QT_QMAKE_EXECUTABLE}") 
  MESSAGE(STATUS "Qt lib dir: ${QT_LIBRARY_DIR}")  
  MESSAGE(STATUS "Qt include dir: ${QT_INCLUDE_DIR}")  
ENDIF (QT_FOUND)

# find CBLAS
################################################################################
FIND_PACKAGE(CBLAS)
IF(CBLAS_FOUND)
  INCLUDE_DIRECTORIES(${CBLAS_INCLUDE_DIR})
ENDIF(CBLAS_FOUND)

# find CPPUnit
################################################################################
FIND_PACKAGE(CPPUNIT)
IF (CPPUNIT_FOUND)
  include_directories(${CPPUNIT_INCLUDE_DIR})
ENDIF (CPPUNIT_FOUND)

# find IPP
################################################################################
FIND_PACKAGE(IPP)
IF (IPP_FOUND)
  include_directories(${IPP_INCLUDE_DIR})
  LINK_DIRECTORIES(${IPP_LIBRARIES_DIR})
  IF (64BIT) # 64 bit libraries
    SET(IPP_LIBRARIES
      ippcoreem64t guide ippiem64t ippcvem64t ippsem64t ippccem64t pthread)
    MESSAGE(STATUS "Found 64bit Intel IPP")
  ELSE (64BIT) # 32 bit libraries
    SET(IPP_LIBRARIES ippcore guide ippi ippcv ipps ippcc pthread)
    MESSAGE(STATUS "Found 32bit Intel IPP")
  ENDIF (64BIT)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPP__")  
ENDIF (IPP_FOUND)

################################################################################
# LAST CALL: custom dependencies with possible override
################################################################################
# Call custom dependencies, will override previous dependencies if overlap
################################################################################
FIND_PACKAGE(Custom)

