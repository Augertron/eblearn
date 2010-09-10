# Custom cmake configuration
# Put here paths to manually installed libraries if necessary
################################################################################

################################################################################
# Intel IPP
################################################################################
# SET(IPP_FOUND TRUE)
# SET(IPP_INCLUDE_DIR "/home/pierre/ipp/include/")
# SET(IPP_LIBRARIES_DIR "/home/pierre/ipp/sharedlib/")

################################################################################
#SET(Boost_FOUND TRUE)
#SET(Boost_INCLUDE_DIRS "$ENV{HOME}/installed/boost/include")
#SET(Boost_LIBRARY_DIRS "$ENV{HOME}/installed/boost/lib")
#SET(Boost_MINOR_VERSION 35)

 # SET(CPPUNIT_FOUND TRUE)
 # SET(CPPUNIT_INCLUDE_DIR "B:/windows/installed/cppunit-1.12.1/include")
 # SET(CPPUNIT_LIBRARY "B:/windows/installed/cppunit-1.12.1/lib/cppunitd.lib")
 # MESSAGE(STATUS "Found CPPUNIT")

#SET(MPI_FOUND TRUE)
#SET(MPI_INCLUDE_PATH "/share/apps/openmpi/1.3.3/intel/include/openmpi/ompi/mpi/cxx/")
SET(MPI_INCLUDE_PATH "/share/apps/openmpi/1.3.3/intel/include/")
SET(MPI_LIBRARIES "/share/apps/openmpi/1.3.3/intel/lib/")
#SET(MPI_LIBRARIES "/share/apps/openmpi/1.3.3/intel/lib/libmpi.so;/share/apps/openmpi/1.3.3/intel/lib/libmpi_cxx.so;/share/apps/openmpi/1.3.3/intel/lib/libmca_common_sm.so;/share/apps/openmpi/1.3.3/intel/lib/libmpi_f77.so;/share/apps/openmpi/1.3.3/intel/lib/libmpi_f90.so")
#SET(MPI_LIBRARIES "/share/apps/openmpi/1.3.3/intel/lib/libmpi_cxx.so;/share/apps/openmpi/1.3.3/intel/lib/libmpi.so")

LINK_DIRECTORIES("/share/apps/intel/cce/10.0.023/lib/")
#LINK_DIRECTORIES("/share/apps/intel/cce/10.0.023/lib/libirc.so")
#LINK_DIRECTORIES("/share/apps/intel/fce/10.0.023/lib/;/share/apps/intel/fce/10.0.023/lib/libirc.so")