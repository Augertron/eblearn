################################################################################
#
# CMake configuration for the objrec demo project
#
# Author(s):
#   Pierre Sermanet, pierre.sermanet@gmail.com, New York University
#
################################################################################

include_directories(include)
include_directories(${LIBEBLEARN_INCLUDE_DIR})
include_directories(${LIBEBLEARNTOOLS_INCLUDE_DIR})
include_directories(${LIBIDX_INCLUDE_DIR})
IF (QT_FOUND)
  include_directories(${LIBIDXGUI_INCLUDE_DIR})
  include_directories(${LIBEBLEARNGUI_INCLUDE_DIR})
ENDIF (QT_FOUND)

# compile executable: objtrain
################################################################################
add_executable (objtrain
  src/train.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (objtrain eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (objtrain idxgui)
  target_link_libraries (objtrain eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------
		
# compile executable: objdetect
################################################################################
add_executable (objdetect
  src/detect.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (objdetect eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (objdetect idxgui)
  target_link_libraries (objdetect eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------
LINK_BOOST(objdetect)

# compile executable: mtdetect
################################################################################
add_executable (mtdetect
  src/mtdetect.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (mtdetect eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (mtdetect idxgui)
  target_link_libraries (mtdetect eblearngui)
ENDIF (QT_FOUND) #------------------------------------------------------------
LINK_BOOST(mtdetect)

# compile executable: mpidetect
################################################################################
add_executable (mpidetect
  src/mpidetect.cpp
  )
  
# link executable with external libraries
################################################################################
target_link_libraries (mpidetect eblearn idx eblearntools)
IF (QT_FOUND) #---------------------------------------------------------------
  target_link_libraries (mpidetect idxgui)
  target_link_libraries (mpidetect eblearngui)
  IF (MPI_FOUND)
    target_link_libraries (mpidetect mpi)
  ENDIF (MPI_FOUND)    
ENDIF (QT_FOUND) #------------------------------------------------------------
LINK_BOOST(mpidetect)

# # compile executable: objdetect
# ################################################################################
# add_executable (objtrack
#   src/track.cpp
#   )
  
# # link executable with external libraries
# ################################################################################
# target_link_libraries (objtrack eblearn idx eblearntools)
# IF (QT_FOUND) #---------------------------------------------------------------
#   target_link_libraries (objtrack idxgui)
#   target_link_libraries (objtrack eblearngui)
# ENDIF (QT_FOUND) #------------------------------------------------------------
# LINK_BOOST(objtrack)
