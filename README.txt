Eblearn Project
--------------------------------------------------------------------------------

Description:

  libeblearn: a machine learning C++ library (convolutional neural networks).
  libeblearn_tools: tools for libeblearn.
  eblearn_tester: unit-testing of libeblearn functionalities.

Installation:

1. Required external packages:
--------------------------------------------------------------------------------

*all packages*	     sudo apt-get install cmake g++ \
         	     	  	  	  libatlas-base-dev \
	       	 	  		  libboost-filesystem-dev \
					  libboost-regex-dev \
	       	     	  		  libqt4-core libqt4-dev libqt4-gui \
               	     	  		  libcppunit-dev

*details*

  compilation: 	     sudo apt-get install cmake g++
  libeblearn:        sudo apt-get install libatlas-base-dev 
  libeblearn-tools:  sudo apt-get install libboost-filesystem-dev 
  		     	  	  	  libboost-regex-dev
  libeblearn-gui:    sudo apt-get install libqt4-core libqt4-dev libqt4-gui 
  eblearn_tester:    sudo apt-get install libcppunit-dev

2. Compilation
--------------------------------------------------------------------------------
./build.sh

3. Execution
--------------------------------------------------------------------------------
./configure.sh
cd bin && ./eblearn_tester
