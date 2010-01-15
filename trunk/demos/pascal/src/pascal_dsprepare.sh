#!/bin/sh

# pascal dataset compilation
ROOT=/data/pascal/VOCdevkit/voc2009/
OUT=/data/pascal/tmp/
#ROOT=~/blakeydata/pascal/VOC2009/
#OUT=$ROOT
NAME=allbilinear
MAX=100
DRAWS=5
H=32
W=32
PRECISION=float
PP=YpUV
KERNEL=9
RESIZE=bilinear

# compile dataset
# ~/eblearn/bin/dscompiler $ROOT -type pascal -precision $PRECISION \
#     -outdir ${OUT} -channels $PP -ignore_difficult -dname $NAME \
#     -resize $RESIZE -kernelsz $KERNEL -dims ${H}x${W}x3 # -disp -maxperclass 5

# # split dataset into training/validation
# ~/eblearn/bin/dssplit $OUT $NAME ${NAME}_val_${MAX} ${NAME}_train_${MAX} \
#     -maxperclass ${MAX} -draws $DRAWS

# extract background images at different scales
~/eblearn/bin/dscompiler $ROOT -type pascalbg -precision $PRECISION \
    -outdir $OUT -scales 2,4 -dims ${H}x${W}x3 -maxperclass 10 \
    -channels $PP -ignore_difficult -resize $RESIZE -kernelsz $KERNEL \
    -disp -sleep 1000
    