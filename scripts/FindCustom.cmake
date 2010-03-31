# Custom cmake configuration
# Put here paths to manually installed libraries if necessary
################################################################################

################################################################################
# Intel IPP
################################################################################
SET(IPP_FOUND FALSE)
IF (IPP_FOUND)
  INCLUDE_DIRECTORIES("/opt/intel/ipp/current/em64t/include")
  LINK_DIRECTORIES("/opt/intel/ipp/current/em64t/sharedlib")
  MESSAGE(STATUS "Found Intel IPP")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPP__")
ENDIF (IPP_FOUND)

################################################################################
