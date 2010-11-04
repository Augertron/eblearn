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
target_link_libraries (objtrain eblearn idx eblearntools)
LINK_QT(objtrain idxgui)
LINK_QT(objtrain eblearngui)
		
# compile executable: objdetect
################################################################################
add_executable (objdetect
  src/detect.cpp
  )
# link executable with external libraries
target_link_libraries (objdetect eblearn idx eblearntools)
LINK_QT(objdetect idxgui)
LINK_QT(objdetect eblearngui)
LINK_BOOST(objdetect system)
LINK_BOOST(objdetect filesystem)
LINK_BOOST(objdetect regex)

# compile executable: mtdetect
################################################################################
IF (WINDOWS) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating mtdetect project under Windows")
ELSE (WINDOWS)
add_executable (mtdetect
  src/mtdetect.cpp
  )  
# link executable with external libraries
target_link_libraries (mtdetect eblearn idx eblearntools)
LINK_QT(mtdetect idxgui)
LINK_QT(mtdetect eblearngui)
LINK_BOOST(mtdetect system)
LINK_BOOST(mtdetect filesystem)
LINK_BOOST(mtdetect regex)
ENDIF (WINDOWS) # do not compile this project under windows

# compile executable: mpidetect
################################################################################
IF (MPI_FOUND AND Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)
IF (WINDOWS) # do not compile this project under windows
  MESSAGE("__ WARNING: Not generating mpidetect project under Windows")
ELSE (WINDOWS) # do not compile this project under windows
  
add_executable (mpidetect
  src/mpidetect.cpp
  )  
# link executable with external libraries
target_link_libraries (mpidetect eblearn idx eblearntools)
LINK_QT(mpidetect idxgui)
LINK_QT(mpidetect eblearngui)
LINK_BOOST(mpidetect system)
LINK_BOOST(mpidetect filesystem)
LINK_BOOST(mpidetect regex)
LINK_BOOST(mpidetect serialization)
LINK_BOOST(mpidetect mpi)
IF (MPI_FOUND)
  target_link_libraries (mpidetect pmpich++) # mpi_cxx)
ENDIF (MPI_FOUND)    

ENDIF (WINDOWS) # do not compile this project under windows
ELSE (MPI_FOUND AND Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)
  MESSAGE("__ WARNING: mpidetect will not be compiled because of missing MPI libraries (optional).")
ENDIF (MPI_FOUND AND Boost_SERIALIZATION_FOUND AND Boost_MPI_FOUND)

# # compile executable: objdetect
# ################################################################################
# add_executable (objtrack
#   src/track.cpp
#   )
  
# # link executable with external libraries
# target_link_libraries (objtrack eblearn idx eblearntools)
# LINK_QT(objtrack idxgui)
# LINK_QT(objtrack eblearngui)
# LINK_BOOST(objtrack system)
# LINK_BOOST(objtrack filesystem)
# LINK_BOOST(objtrack regex)
