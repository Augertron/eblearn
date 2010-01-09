#!/bin/sh

# pascal dataset compilation
ROOT=/data/pascal/VOCdevkit/voc2009/
NAME=all09
MAX=100
DRAWS=5

# compile dataset
~/eblearn/bin/dscompiler $ROOT -type pascal -precision float -outdir ${ROOT}/ds/ -channels YpUV -ignore_difficult -dname ${NAME} -maxperclass 5 -resize bilinear  -kernelsz 9 # -disp

# split dataset into training/validation
~/eblearn/bin/dssplit $ROOT $NAME ${NAME}_val ${NAME}_train -maxperclass ${MAX} -draws $DRAWS
