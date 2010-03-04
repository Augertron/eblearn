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
FIND_PACKAGE(Qt4)
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

