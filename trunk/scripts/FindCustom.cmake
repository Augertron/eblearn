# Custom cmake configuration
# Put here paths to manually installed libraries if necessary
################################################################################

################################################################################
# Intel IPP
################################################################################
SET(IPP_FOUND FALSE)
IF (IPP_FOUND)
  INCLUDE_DIRECTORIES("/home/pierre/ipp/include/")
  LINK_DIRECTORIES("/home/pierre/ipp/sharedlib/")
  # 64 bit libraries
  #SET(IPP_LIBRARIES
  #  ippcoreem64t guide ippiem64t ippcvem64t ippsem64t ippccem64t pthread)
  # 32 bit libraries
  SET(IPP_LIBRARIES ippcore guide ippi ippcv ipps ippcc pthread)
  MESSAGE(STATUS "Found Intel IPP")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPP__")
ENDIF (IPP_FOUND)

################################################################################
