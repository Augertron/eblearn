#!/bin/sh

# pascal dataset compilation
ROOT=~/blakeydata/pascal/2009/
NAME=all
MAX=100

~/eblearn/bin/dssplit $ROOT $NAME ${NAME}_test ${NAME}_train -maxperclass ${MAX} -draws 5

