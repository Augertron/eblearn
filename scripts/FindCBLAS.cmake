# - Find CBLAS library
#
# This module finds an installed fortran library that implements the CBLAS 
# linear-algebra interface (see http://www.netlib.org/blas/), with CBLAS
# interface.
#
# This module sets the following variables:
#  CBLAS_FOUND - set to true if a library implementing the CBLAS interface
#    is found
#  CBLAS_LINKER_FLAGS - uncached list of required linker flags (excluding -l
#    and -L).
#  CBLAS_LIBRARIES - uncached list of libraries (using full path name) to 
#    link against to use CBLAS
#  CBLAS_INCLUDE_DIR - path to includes
#  CBLAS_INCLUDE_FILE - the file to be included to use CBLAS
#

INCLUDE(CheckFunctionExists)
INCLUDE(CheckIncludeFile)

MACRO(CHECK_ALL_LIBRARIES LIBRARIES _prefix _name _flags _list _include _search_include _check_function)
  # This macro checks for the existence of the combination of fortran libraries
  # given by _list.  If the combination is found, this macro checks (using the 
  # Check_Fortran_Function_Exists macro) whether can link against that library
  # combination using the name of a routine given by _name using the linker
  # flags given by _flags.  If the combination of libraries is found and passes
  # the link test, LIBRARIES is set to the list of complete library paths that
  # have been found.  Otherwise, LIBRARIES is set to FALSE.
  
  # N.B. _prefix is the prefix applied to the names of all cached variables that
  # are generated internally and marked advanced by this macro.

  SET(__list)
  FOREACH(_elem ${_list})
    IF(__list)
      SET(__list "${__list} - ${_elem}")
    ELSE(__list)
      SET(__list "${_elem}")
    ENDIF(__list)
  ENDFOREACH(_elem)
  MESSAGE(STATUS "Checking for [${__list}]")
  SET(_libraries_work TRUE)
  SET(${LIBRARIES})
  SET(_combined_name)
  SET(_paths /usr/local/include /usr/include /usr/local/atlas/include ENV)
  FOREACH(_library ${_list})
    SET(_combined_name ${_combined_name}_${_library})
MESSAGE("searching lib: ${_library}")
    # did we find all the libraries in the _list until now?
    # (we stop at the first unfound one)
    IF(_libraries_work)      
      IF(APPLE) 
        FIND_LIBRARY(${_prefix}_${_library}_LIBRARY
          NAMES ${_library}
          PATHS /usr/local/lib /usr/lib /usr/local/lib64 /usr/lib64 /usr/local/atlas/lib ENV 
          DYLD_LIBRARY_PATH 
          )
      ELSE(APPLE)
        FIND_LIBRARY(${_prefix}_${_library}_LIBRARY
          NAMES ${_library}
          PATHS /usr/local/lib /usr/lib /usr/local/lib64 /usr/lib64 /usr/local/lib/atlas  /usr/local/atlas/lib /usr/lib/atlas /usr/local/lib64/atlas /usr/lib64/atlas ENV 
          LD_LIBRARY_PATH 
          )
      ENDIF(APPLE)

      MARK_AS_ADVANCED(${_prefix}_${_library}_LIBRARY)
      IF(${_prefix}_${_library}_LIBRARY)
        GET_FILENAME_COMPONENT(_path ${${_prefix}_${_library}_LIBRARY} PATH)
        LIST(APPEND _paths ${_path}/../include ${_path}/../../include)
      ENDIF(${_prefix}_${_library}_LIBRARY)
      SET(${LIBRARIES} ${${LIBRARIES}} ${${_prefix}_${_library}_LIBRARY})
      SET(_libraries_work ${${_prefix}_${_library}_LIBRARY})
    ENDIF(_libraries_work)
  ENDFOREACH(_library ${_list})

  # Test include
  SET(_bug_search_include ${_search_include}) #CMAKE BUG!!! SHOULD NOT BE THAT
  IF(_bug_search_include)
    IF(${_prefix}${_combined_name}_INCLUDE)
      SET(${_prefix}${_combined_name}_INCLUDE ${${_prefix}${_combined_name}_INCLUDE}-NOTFOUND)
    ENDIF(${_prefix}${_combined_name}_INCLUDE)
    FIND_PATH(${_prefix}${_combined_name}_INCLUDE ${_include} ${_paths})
    MARK_AS_ADVANCED(${_prefix}${_combined_name}_INCLUDE)
    IF(${_prefix}${_combined_name}_INCLUDE)
      MESSAGE(STATUS "Checking for [${__list}] -- includes found: ${${_prefix}${_combined_name}_INCLUDE}")
      SET(${_prefix}_INCLUDE_DIR ${${_prefix}${_combined_name}_INCLUDE})
      SET(${_prefix}_INCLUDE_FILE ${_include})
    ELSE(${_prefix}${_combined_name}_INCLUDE)
      MESSAGE(STATUS "Checking for [${__list}] -- includes not found")
      SET(_libraries_work FALSE)
    ENDIF(${_prefix}${_combined_name}_INCLUDE)
  ELSE(_bug_search_include)
    SET(${_prefix}_INCLUDE_DIR)
    SET(${_prefix}_INCLUDE_FILE ${_include})
  ENDIF(_bug_search_include)

  IF(_libraries_work)
    # Test this combination of libraries.
    IF(_check_function)
      SET(CMAKE_REQUIRED_LIBRARIES ${_flags} ${${LIBRARIES}})
      CHECK_FUNCTION_EXISTS(${_name} ${_prefix}${_combined_name}_WORKS)
      SET(CMAKE_REQUIRED_LIBRARIES)
      MARK_AS_ADVANCED(${_prefix}${_combined_name}_WORKS)
      SET(_libraries_work ${${_prefix}${_combined_name}_WORKS})
    ENDIF(_check_function)
    IF(_libraries_work)
      MESSAGE(STATUS "Checking for [${__list}] -- libraries found: ${CBLAS_LIBRARIES}")
    ENDIF(_libraries_work)

  ENDIF(_libraries_work)
  

  IF(NOT _libraries_work)
    SET(${LIBRARIES} FALSE)
  ENDIF(NOT _libraries_work)

ENDMACRO(CHECK_ALL_LIBRARIES)

SET(CBLAS_LINKER_FLAGS)
SET(CBLAS_LIBRARIES)

# CBLAS in intel mkl library? (shared)
IF(NOT CBLAS_LIBRARIES)
  CHECK_ALL_LIBRARIES(
    CBLAS_LIBRARIES
    CBLAS
    cblas_sgemm
    ""
    "mkl;guide;pthread"
    "mkl_cblas.h"
    TRUE
    TRUE
    )
ENDIF(NOT CBLAS_LIBRARIES)

#CBLAS in intel mkl library? (static, 32bit)
IF(NOT CBLAS_LIBRARIES)
  CHECK_ALL_LIBRARIES(
    CBLAS_LIBRARIES
    CBLAS
    cblas_sgemm
    ""
    "mkl_ia32;guide;pthread"
    "mkl_cblas.h"
    TRUE
    TRUE
    )
ENDIF(NOT CBLAS_LIBRARIES)

#CBLAS in intel mkl library? (static, em64t 64bit)
IF(NOT CBLAS_LIBRARIES)
  CHECK_ALL_LIBRARIES(
    CBLAS_LIBRARIES
    CBLAS
    cblas_sgemm
    ""
    "mkl_em64t;guide;pthread"
    "mkl_cblas.h"
    ON
    TRUE
    )
ENDIF(NOT CBLAS_LIBRARIES)

IF(APPLE)
  SET(APPLE_FRAMEWORK_FOUND)
  # Apple CBLAS library?
  IF(NOT CBLAS_LIBRARIES)
    CHECK_ALL_LIBRARIES(
      CBLAS_LIBRARIES
      CBLAS
      cblas_dgemm
      ""
      "Accelerate"
      "Accelerate/Accelerate.h"
      FALSE
      TRUE
      )
    
  ENDIF(NOT CBLAS_LIBRARIES)
  
  IF( NOT CBLAS_LIBRARIES )
    CHECK_ALL_LIBRARIES(
      CBLAS_LIBRARIES
      CBLAS
      cblas_dgemm
      ""
      "vecLib"
      "vecLib/vecLib.h"
      FALSE
      TRUE
      )
  ENDIF( NOT CBLAS_LIBRARIES )
  IF(CBLAS_LIBRARIES)
    SET(APPLE_FRAMEWORK_FOUND TRUE)
  ENDIF(CBLAS_LIBRARIES)
ENDIF(APPLE)

IF(NOT CBLAS_LIBRARIES)
  # CBLAS in ATLAS library? (http://math-atlas.sourceforge.net/)
  CHECK_ALL_LIBRARIES(
    CBLAS_LIBRARIES
    CBLAS
    cblas_dgemm
    ""
    "cblas"
    "atlas/cblas.h"
    TRUE
    TRUE
    )
ENDIF(NOT CBLAS_LIBRARIES)

SET(MODULES cblas atlas lapack f77blas)
IF(NOT CBLAS_LIBRARIES)
  # CBLAS in ATLAS library? (http://math-atlas.sourceforge.net/)
  CHECK_ALL_LIBRARIES(
    CBLAS_LIBRARIES
    CBLAS
    cblas_dgemm
    ""
    "${MODULES}"
    "cblas.h"
    TRUE
    TRUE
    )
ENDIF(NOT CBLAS_LIBRARIES)

IF(CBLAS_LIBRARIES)
  SET(CBLAS_FOUND TRUE)
ELSE(CBLAS_LIBRARIES)
  SET(CBLAS_FOUND FALSE)
ENDIF(CBLAS_LIBRARIES)

IF(NOT CBLAS_FOUND AND CBLAS_FIND_REQUIRED)
  MESSAGE(FATAL_ERROR "CBLAS library not found. Please install cblas or atlas libraries as explained in README.txt")
ENDIF(NOT CBLAS_FOUND AND CBLAS_FIND_REQUIRED)

IF(NOT CBLAS_FIND_QUIETLY)
  IF(CBLAS_FOUND)
    MESSAGE(STATUS "Found CBlas library")
  ELSE(CBLAS_FOUND)
    MESSAGE(FATAL_ERROR "CBLAS library not found. Please install cblas or atlas libraries as explained in README.txt")
  ENDIF(CBLAS_FOUND)
ENDIF(NOT CBLAS_FIND_QUIETLY)
