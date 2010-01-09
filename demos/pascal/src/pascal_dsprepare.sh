#!/bin/sh

# pascal dataset compilation
#ROOT=/data/pascal/VOCdevkit/voc2009/
ROOT=~/blakeydata/pascal/VOC2009/
OUT=~/blakeydata/pascal/2009/
NAME=allbilinear
MAX=100
DRAWS=5

# compile dataset
~/eblearn/bin/dscompiler $ROOT -type pascal -precision float -outdir ${OUT} -channels YpUV -ignore_difficult -dname ${NAME} -resize bilinear -kernelsz 9 # -disp -maxperclass 5

# split dataset into training/validation
~/eblearn/bin/dssplit $OUT $NAME ${NAME}_val_${MAX} ${NAME}_train_${MAX} -maxperclass ${MAX} -draws $DRAWS
