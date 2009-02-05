#!/bin/sh

USE_IPP=IPPOFF
IPP_DIR=
LIBS=""
LIBS_IPP="-lippiemerged -lippimerged -lippcore -lippsmerged"
MNIST=""

echo "libeblearn configuration."
echo "1. Use proprietary Intel Integrated Primitives (IPP) libraries? (y/N)"
echo -n "\t"
  read ans
  case $ans in
    [yY]) echo "Ok, what is your IPP directory? (Example: /opt/intel/ipp)"
    echo -n "\t"
    USE_IPP=USE_IPP
    LIBS="$LIBS $LIBS_IPP"
    read IPP_DIR
    test -d $IPP_DIR
    if [ $? -eq 1 ] 
      then echo -n "Error, not a valid path: "
           echo $IPP_DIR
           exit 1
      fi
    echo -n "\tOk, using IPP from directory: "
    echo $IPP_DIR
    ;;
    *) echo "\tNot using IPP libraries."
  esac

echo "2. Use MNIST handwritten digit database? (y/N)"
echo "   (download at http://yann.lecun.com/exdb/mnist/)"
echo -n "\t"
  read ans
  case $ans in
    [yY]) echo "\tOk, what is your MNIST directory? (Example: ~/mnist)"
    echo -n "\t"
    read MNIST
    test -d $MNIST
    if [ $? -eq 1 ] 
      then echo -n "Error, not a valid path: "
           echo $MNIST
           exit 1
      fi
    echo -n "\tOk, using MNIST from directory: "
    echo $MNIST
    ;;
    *) echo "\tNot using MNIST."
  esac

#ouputing arguments to main() for runtime
mkdir bin 2> /dev/null
echo "\n-mnist $MNIST" >> run.init

echo "Configuration done."
