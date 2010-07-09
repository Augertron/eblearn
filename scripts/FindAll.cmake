# determine machine architecture
################################################################################
IF (APPLE) # MAC OS
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__MAC__ -pthread")
  SET (MAC true)
  SET (OS_NAME "Mac")
ELSE (APPLE)
  IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__LINUX__ -pthread")
    SET (LINUX true)
    SET (OS_NAME "Linux")
  ELSE ("${CMAKE_SYSTEM}" MATCHES "Linux")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__WINDOWS__")    
    # avoid security improvements warnings
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_DEPRECATE")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
    SET (WINDOWS true)
    SET (OS_NAME "Windows")
  ENDIF("${CMAKE_SYSTEM}" MATCHES "Linux")
ENDIF (APPLE)

SET(BITSIZE "32")
SET(64BIT false) # default is 32 bits
IF (APPLE OR LINUX)
  EXEC_PROGRAM("uname -m" OUTPUT_VARIABLE ARCH_NAME)
  STRING(COMPARE EQUAL ${ARCH_NAME} "x86_64" 64BIT)
ELSE (APPLE OR LINUX)
  STRING(COMPARE EQUAL $ENV{PROCESSOR_ARCHITECTURE} "AMD64" 64BIT)
ENDIF (APPLE OR LINUX)
IF (64BIT)
  SET(BITSIZE "64")
ENDIF (64BIT)

MESSAGE(STATUS "Target OS is ${OS_NAME} (${BITSIZE} bits)")

MACRO(MAKE_WINDOWS_PATH pathname)
  # Enclose with UNESCAPED quotes.  This means we need to escape our
  # quotes once here, i.e. with \"
  SET(pathname "\"${pathname}\"")
ENDMACRO(MAKE_WINDOWS_PATH)

################################################################################
# custom dependencies with override
################################################################################
# Call custom dependencies, will override dependencies if overlap
################################################################################
FIND_PACKAGE(Custom)

# find boost
###############################################################################
IF ($ENV{NOBOOST})
  MESSAGE(STATUS "BOOST DISABLED by env variable $NOBOOST=1.")
ELSE ($ENV{NOBOOST})
  IF (NOT Boost_FOUND)
    FIND_PACKAGE(Boost COMPONENTS filesystem regex) 
    IF (Boost_FOUND)
      IF (${Boost_MINOR_VERSION} GREATER 34)
        FIND_PACKAGE(Boost COMPONENTS system filesystem regex)
      ENDIF(${Boost_MINOR_VERSION} GREATER 34)
    ENDIF (Boost_FOUND)
  ENDIF (NOT Boost_FOUND)
  IF (Boost_FOUND)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__BOOST__")
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
    MESSAGE(STATUS "Found Boost")
    MESSAGE(STATUS "Boost include directory: ${Boost_INCLUDE_DIRS}")
    MESSAGE(STATUS "Boost libraries directory: ${Boost_LIBRARY_DIRS}")
  ELSE(Boost_FOUND)
    MESSAGE("__ WARNING: Boost not found.")
  ENDIF(Boost_FOUND)
ENDIF ($ENV{NOBOOST})
  
# find opencv
###############################################################################
IF ($ENV{NOOPENCV})
  MESSAGE(STATUS "OPENCV DISABLED by env variable $NOOPENCV=1.")
ELSE ($ENV{NOOPENCV})
  FIND_PACKAGE(OpenCV)
  IF (OpenCV_FOUND)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__OPENCV__")
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    LINK_DIRECTORIES(${OpenCV_LIBRARY_DIRS})
    MESSAGE(STATUS "Found OpenCV")
    MESSAGE(STATUS "OpenCV include directory: ${OpenCV_INCLUDE_DIRS}")
    MESSAGE(STATUS "OpenCV libraries: ${OpenCV_LIBRARIES}")
  ELSE(OpenCV_FOUND)
    MESSAGE("__ WARNING: OpenCV not found.")
  ENDIF(OpenCV_FOUND)
ENDIF ($ENV{NOOPENCV})
  
# find XML++
##############################################################################
IF ($ENV{NOXML})
  MESSAGE(STATUS "XML DISABLED by env variable $NOXML=1.")
ELSE ($ENV{NOXML})
  FIND_PACKAGE(Xml++)
  IF (XML_FOUND)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__XML__")
    INCLUDE_DIRECTORIES(${XML_INCLUDE_DIRS})
  ENDIF (XML_FOUND)
ENDIF ($ENV{NOXML})

# find QT
##############################################################################
IF ($ENV{NOQT})
  MESSAGE(STATUS "QT DISABLED by env variable $NOQT=1.")
ELSE ($ENV{NOQT})
  SET(DESIRED_QT_VERSION 4.3)
  FIND_PACKAGE(Qt4)
  IF (NOT QT_FOUND)
    FIND_PACKAGE(Qt)
  ENDIF (NOT QT_FOUND)

#  SET(DESIRED_QT_VERSION 4.3)
  IF (NOT QT_FOUND)
    SET(QT_QMAKE_EXECUTABLE "/usr/local/pkg/qt/4.6.2/bin/qmake")
    IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
      SET(QT_QMAKE_EXECUTABLE "/usr/lib64/qt4/bin/qmake")
      IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
	SET(QT_QMAKE_EXECUTABLE "/usr/share/qt4/bin/qmake")
        IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
	  # still not found, use what's returned by 'which'
	  EXEC_PROGRAM("which" ARGS "qmake" OUTPUT_VARIABLE
	    QT_QMAKE_EXECUTABLE)
          IF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
	    # give up
	    SET(QT_QMAKE_EXECUTABLE "")
	  ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
	ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
      ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})
    ENDIF (NOT EXISTS ${QT_QMAKE_EXECUTABLE})

  FIND_PACKAGE(Qt4)
    #  INCLUDE(${QT_USE_FILE})
    SET(QT_FOUND TRUE)
    exec_program(${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_HEADERS"
      OUTPUT_VARIABLE QT_INCLUDE_DIR)
   SET(QT_QTGUI_INCLUDE_DIR ${QT_INCLUDE_DIR})
                               SET(QT_QTGUI_LIBRARY "${QT_LIBRARY_DIR}/libQtGui.so")
  ENDIF (NOT QT_FOUND)

  IF (QT_FOUND)
    MESSAGE(STATUS "Qt qmake: ${QT_QMAKE_EXECUTABLE}") 
    MESSAGE(STATUS "Qt moc: ${QT_MOC_EXECUTABLE}") 
    MESSAGE(STATUS "Qt lib dir: ${QT_LIBRARY_DIR}")  
    MESSAGE(STATUS "Qt include dir: ${QT_INCLUDE_DIR}")  
    IF (WINDOWS)
      # to use DLL instead of static libraries, tell code we are using DLLs
      #SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_DLL")
      # add dll paths
      
    ENDIF (WINDOWS)    
    IF (QT_MOC_EXECUTABLE 
        AND QT_QMAKE_EXECUTABLE AND QT_LIBRARY_DIR AND QT_INCLUDE_DIR)
      IF (EXISTS "${QT_INCLUDE_DIR}")
        include_directories(${QT_INCLUDE_DIR})
      ENDIF (EXISTS "${QT_INCLUDE_DIR}")
      IF (EXISTS "${QT_INCLUDE_DIR}/Qt")
        include_directories(${QT_INCLUDE_DIR}/Qt)
      ENDIF (EXISTS "${QT_INCLUDE_DIR}/Qt")
      IF (EXISTS "${QT_INCLUDE_DIR}/QtGui")
        include_directories(${QT_INCLUDE_DIR}/QtGui)
      ENDIF (EXISTS "${QT_INCLUDE_DIR}/QtGui")
      IF (EXISTS "${QT_INCLUDE_DIR}/QtCore")
        include_directories(${QT_INCLUDE_DIR}/QtCore)
      ENDIF (EXISTS "${QT_INCLUDE_DIR}/QtCore")
      IF (EXISTS "${QT_INCLUDE_DIR}/Headers")
        include_directories(${QT_INCLUDE_DIR}/Headers/)
      ENDIF (EXISTS "${QT_INCLUDE_DIR}/Headers")
      include_directories(${QT_QTGUI_INCLUDE_DIR})
      include_directories(${QT_QTCORE_INCLUDE_DIR})
      SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__GUI__")
      MESSAGE(STATUS "Found Qt")
    ELSEIF (QT_MOC_EXECUTABLE 
            AND QT_QMAKE_EXECUTABLE AND QT_LIBRARY_DIR AND QT_INCLUDE_DIR)
      MESSAGE("__ WARNING: Some Qt components are missing.") 
    ENDIF (QT_MOC_EXECUTABLE 
           AND QT_QMAKE_EXECUTABLE AND QT_LIBRARY_DIR AND QT_INCLUDE_DIR)
  ELSE (QT_FOUND)
    MESSAGE("__ WARNING: QT not found.")
  ENDIF (QT_FOUND)
ENDIF ($ENV{NOQT})

# find CBLAS
################################################################################
IF ($ENV{NOCBLAS})
  MESSAGE(STATUS "CBLAS DISABLED by env variable $NOCBLAS=1.")
ELSE ($ENV{NOCBLAS})
  FIND_PACKAGE(CBLAS)
  IF(CBLAS_FOUND)
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__CBLAS__")
    INCLUDE_DIRECTORIES(${CBLAS_INCLUDE_DIR})
  ELSE (CBLAS_FOUND)
    MESSAGE("__ WARNING: cblas not found, install to speed up.")
  ENDIF(CBLAS_FOUND)
ENDIF ($ENV{NOCBLAS})

# find CPPUnit
################################################################################
FIND_PACKAGE(CPPUNIT)
IF (CPPUNIT_FOUND)
  include_directories(${CPPUNIT_INCLUDE_DIR})
ENDIF (CPPUNIT_FOUND)

# find IPP
################################################################################
#FIND_PACKAGE(IPP)
IF ($ENV{NOIPP})
  MESSAGE(STATUS "IPP DISABLED by env variable $NOIPP=1.")
ELSE ($ENV{NOIPP})
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
  ELSE (IPP_FOUND)
    MESSAGE("__ WARNING: Intel IPP not found, install to speed up.")
  ENDIF (IPP_FOUND)
ENDIF ($ENV{NOIPP})

# find ImageMagick
################################################################################
# we can't use convert under Windows, so don't try
IF ($ENV{NOIMAGEIMAGICK})
  MESSAGE(STATUS "ImageMagick DISABLED by env variable $NOIMAGEMAGICK=1.")
ELSE ($ENV{NOIMAGEIMAGICK})
  FIND_PACKAGE(ImageMagick)
  IF (ImageMagick_FOUND)
  #  SET(IMAGEMAGICK_CONVERT_EXECUTABLE "\"${IMAGEMAGICK_CONVERT_EXECUTABLE}\"")
    MESSAGE(STATUS "ImageMagick convert: ${IMAGEMAGICK_CONVERT_EXECUTABLE}")
    MESSAGE(STATUS "ImageMagick Found.")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IMAGEMAGICK__")  
  ELSE (ImageMagick_FOUND)
    MESSAGE("__ WARNING: ImageMagick not found.")
  ENDIF (ImageMagick_FOUND)
ENDIF ($ENV{NOIMAGEIMAGICK})

# find Magick++
################################################################################
FIND_PACKAGE(Magick++)
IF (Magick++_FOUND)
  include_directories(${Magick++_INCLUDE_DIR})
  LINK_DIRECTORIES(${Magick++_LIBRARIES_DIR})
  MESSAGE(STATUS "Magick++ includes: ${Magick++_INCLUDE_DIR}")
  MESSAGE(STATUS "Magick++ library: ${Magick++_LIBRARY}")
  MESSAGE(STATUS "Magick++ Found.")
  INCLUDE_DIRECTORIES(${Magick++_INCLUDE_DIR})
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__MAGICKPP__")  
ELSE (Magick++_FOUND)
  # we don't care about this message under linux/mac if convert is found
  IF(WINDOWS OR NOT ImageMagick_FOUND) 
    MESSAGE("__ WARNING: Magick++ not found.")
  ENDIF(WINDOWS OR NOT ImageMagick_FOUND) 
ENDIF (Magick++_FOUND)
