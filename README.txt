Eblearn Project
--------------------------------------------------------------------------------

Description:

  libeblearn: a machine learning C++ library (convolutional neural networks).
  libeblearn_tools: tools for libeblearn.
  eblearn_tester: unit-testing of libeblearn functionalities.

Installation and compilation:

*** LINUX (UBUNTU) ***
--------------------------------------------------------------------------------

    1. Required external packages:

       *all packages* 
	   	     sudo apt-get install subversion cmake g++ \
         	     	  	  	  libatlas-base-dev \
	       	 	  		  libboost-filesystem-dev \
					  libboost-regex-dev \
	       	     	  		  libqt4-core libqt4-dev libqt4-gui \
               	     	  		  libcppunit-dev imagemagick
       *for developers only*
       	    	     sudo apt-get install valgrind electric-fence
       *details*

       compilation:  sudo apt-get install cmake g++ subversion
         libeblearn: sudo apt-get install libatlas-base-dev imagemagick
   libeblearn-tools: sudo apt-get install libboost-filesystem-dev \
  		     	  	  	  libboost-regex-dev
     libeblearn-gui: sudo apt-get install libqt4-core libqt4-dev libqt4-gui \
     eblearn_tester: sudo apt-get install libcppunit-dev
          debugging: sudo apt-get install valgrind electric-fence
					      
    2. Compilation
       ./configure
       make
       (or 'make debug' to compile with debug information for gdb)
       (or 'make prj=idxgui' if you just want to compile the library
       libidxgui and its dependencies or 'make debug prj=tester' to
       compile the tester only in debug mode)  

    3. Execution
       cd bin
       ./tester
       ./mnist "your_mnist_directory"

*** MAC OS X *** 
--------------------------------------------------------------------------------
(by George Williams from 
    http://movement.nyu.edu/internal/Main/EblearnMacosNotes)

    These are some notes that should help you get eblearn to compile
 and run on macos. They are based on revision 115 of the subversion
 source repository of the eblearn source.
    
Instructions

    * Install cmake
          o I installed from the src tarball (cmake-2.6.2.tar.gz)
          which you can get here at
          http://www.cmake.org/cmake/resources/software.html.

          o Consult the README.txt in the top level directory and note
          that the steps to compile and install are "./bootstrap;
          make; sudo make install".

    * Install libatlas
          o I installed from the src tarball (Atlas-C++-0.6.1.tar.gz)
          which you can get here at
          http://sourceforge.net/projects/worldforge/

          o The standard "configure/make/sudo make install" will do. 

    * Install cppunit
          o I installed from the src tarball (cppunit-1.12.1.tar.gz)
          which you can get here at
          http://apps.sourceforge.net/mediawiki/cppunit/.

          o The standard "configure/make/sudo make install" will do. 

    * Install mnist databases
          o You get these here http://yann.lecun.com/exdb/mnist/.
          o You will need to unpack the data
          o You will need the full path to the data directory later on 
    * Install boost
          o You should be able to install from the most recent boost
          version. I found a "macports" version, which is much easier
          to install.

          o Macports install - If you don't have macports, use the
          tarball instructions here:
          http://www.macports.org/install.php. I installed from the
          tarball MacPorts-1.7.0.tar.gz.

          o Macports boost - cd into the macports bin directory and
          type ">> sudo ./ports install boost". This may take a
          while. 

    * Download and adjust eblearn build
          o Download from the subversion repository: ">> svn export
          https://eblearn.svn.sourceforge.net/svnroot/eblearn/trunk
          eblearn"

          o Modify the build to find boost headers. Locate the file
          "eblearn/scripts/FindBoost.cmake" and insert the line at the
          top of the file "SET(BOOST_INCLUDE_DIR
          /opt/local/include)". Assuming you did not change the
          default install directory of macports, then
          /opt/local/include will contain the macports versions of
          packages.

    * Build eblearn
          o At this point, you can use the standard instructions to
          build and configure eblearn

          o cd into the toplevel eblearn directory, and type
          ">>./clean.sh" (just in case you did a previous build with
          this src tree.)

          o Then type ">>./configure.sh". You'll need the full path to
          your local mnist data dir. 

    * Run unit tester
          o cd into "bin" and type "./tester".
          o all the implemented unit tests should pass 
