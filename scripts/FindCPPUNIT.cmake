# Find CppUnit Library
################################################################
FIND_PATH(CPPUNIT_INCLUDE_DIR cppunit/extensions/HelperMacros.h 
  /usr/include
  /usr/local/include
  /sw/include
  /sw/local/include
  /opt/local/include
  )

FIND_LIBRARY(CPPUNIT_LIBRARY 
  NAMES cppunit 
  PATH 
  /usr/lib 
  /usr/local/lib
  /sw/lib
  /sw/local/lib
  /opt/local/lib
  )
IF (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY)
  SET(CPPUNIT_FOUND TRUE)
ENDIF (CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY)

IF (CPPUNIT_FOUND)
  MESSAGE(STATUS "Found CPPUnit: ${CPPUNIT_LIBRARY}")
ELSE (CPPUNIT_FOUND)
  MESSAGE("ERROR:   Could not find CPPUnit library.")
ENDIF (CPPUNIT_FOUND)

MARK_AS_ADVANCED(CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARY)
