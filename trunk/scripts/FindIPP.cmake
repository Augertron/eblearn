################################################################################
# Intel IPP
################################################################################

FIND_PATH(IPP_INCLUDE_DIR include PATHS
  "$ENV{ProgramFiles}/Intel/IPP/*.*/ia32/"
  "/opt/intel/ipp/*.*/ia32/"
  "/opt/intel/ipp/*.*/em64t/"
  "/usr/local/intel/ipp/*.*/ia32/")

FIND_PATH(IPP_LIBRARIES_DIR sharedlib PATHS
  "$ENV{ProgramFiles}/Intel/IPP/*.*/ia32/"
  "/opt/intel/ipp/*.*/ia32/"
  "/opt/intel/ipp/*.*/em64t/"
  "/usr/local/intel/ipp/*.*/ia32/")

IF (${IPP_INCLUDE_DIR})
  MESSAGE(STATUS "Found Intel IPP include: ${IPP_INCLUDE_DIR}")
ENDIF (${IPP_INCLUDE_DIR})
IF (${IPP_LIBRARIES_DIR})
  MESSAGE(STATUS "Found Intel IPP libraries: ${IPP_LIBRARIES_DIR}")
ENDIF (${IPP_LIBRARIES_DIR})

IF (${IPP_LIBRARIES_DIR} AND ${IPP_INCLUDE_DIR})
  SET(IPP_FOUND TRUE)
ELSE (${IPP_LIBRARIES_DIR} AND ${IPP_INCLUDE_DIR})
  MESSAGE("Warning: Intel IPP not found, install to speed up.")
ENDIF (${IPP_LIBRARIES_DIR} AND ${IPP_INCLUDE_DIR})


################################################################################
