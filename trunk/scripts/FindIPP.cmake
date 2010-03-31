################################################################################
# Intel IPP
################################################################################

FIND_PATH(IPP_INCLUDE_DIR "ipp.h" PATHS
  "$ENV{ProgramFiles}/Intel/IPP/*/ia32/include"
  "/opt/intel/ipp/*/ia32/include"
  "/opt/intel/ipp/*/em64t/include"
  "/usr/local/intel/ipp/*/ia32/include")

FIND_PATH(IPP_LIBRARIES_DIR "libguide.so" PATH
  "$ENV{ProgramFiles}/Intel/IPP/*.*/ia32/sharedlib"
  "/opt/intel/ipp/*/ia32/sharedlib"
  "/opt/intel/ipp/*/em64t/sharedlib"
  "/usr/local/intel/ipp/*/ia32/sharedlib")

IF (IPP_INCLUDE_DIR)
  MESSAGE(STATUS "Found Intel IPP include: ${IPP_INCLUDE_DIR}")
ENDIF (IPP_INCLUDE_DIR)
IF (IPP_LIBRARIES_DIR)
  MESSAGE(STATUS "Found Intel IPP libraries: ${IPP_LIBRARIES_DIR}")
ENDIF (IPP_LIBRARIES_DIR)

IF (IPP_LIBRARIES_DIR AND IPP_INCLUDE_DIR)
  SET(IPP_FOUND TRUE)
ELSE (IPP_LIBRARIES_DIR AND IPP_INCLUDE_DIR)
  MESSAGE("Warning: Intel IPP not found, install to speed up.")
ENDIF (IPP_LIBRARIES_DIR AND IPP_INCLUDE_DIR)


################################################################################
