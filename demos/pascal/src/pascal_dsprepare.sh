#!/bin/sh

################################
# pascal dataset compilation
################################

# directories
DATAROOT=/data
#DATAROOT=~/texieradata
PASCALROOT=$DATAROOT/pascal/VOCdevkit_trainval09/VOC2009/
ROOT=$DATAROOT/pascal/
OUT=$ROOT/ds/

# variables
H=96
W=96
NAME=allbi${H}x${W}
NAMEBG=${NAME}bg
MAX=50 # number of samples in validation set
DRAWS=5 # number of train/val sets to draw
PRECISION=float
PP=YpUV
KERNEL=9
RESIZE=bilinear
NBG=1
BGSCALES=1,2,4
BGDS=pascalbg${H}x${W}
OUTBG=$OUT/$BGDS
#MAXDATA="-maxdata 50"
#MAXPERCLASS="-maxperclass 25"

# create directories
mkdir $OUT 2> /dev/null > /dev/null
mkdir $OUTBG 2> /dev/null > /dev/null

# # extract background images at different scales
# ~/eblearn/bin/dscompiler $PASCALROOT -type pascalbg -precision $PRECISION \
#     -outdir $OUTBG/bg -scales $BGSCALES -dims ${H}x${W}x3 \
#     -maxperclass $NBG $MAXDATA \
#     -channels $PP -ignore_difficult -resize $RESIZE -kernelsz $KERNEL \
# #    -disp -sleep 1000

# # compile background dataset
# ~/eblearn/bin/dscompiler ${OUTBG} -type lush -precision $PRECISION \
#     -outdir ${OUT} -dname ${BGDS}_${NBG} $MAXDATA $MAXPERCLASS \
#     -dims ${H}x${W}x3
# #    -disp

# # compile regular dataset
# ~/eblearn/bin/dscompiler $PASCALROOT -type pascal -precision $PRECISION \
#     -outdir ${OUT} -channels $PP -dname $NAME $MAXDATA $MAXPERCLASS \
#     -ignore_difficult \
#     -resize $RESIZE -kernelsz $KERNEL -dims ${H}x${W}x3 # -disp -maxperclass 5

# # merge normal dataset with background dataset
# ~/eblearn/bin/dsmerge $OUT ${NAMEBG} ${NAME} ${BGDS}_$NBG

# # split dataset into training/validation
# ~/eblearn/bin/dssplit $OUT ${NAMEBG} ${NAMEBG}_val_${MAX}_ \
#     ${NAMEBG}_train_${MAX}_ -maxperclass ${MAX} -draws $DRAWS

# extract parts dataset
~/eblearn/bin/dscompiler $PASCALROOT -type pascalpart -precision $PRECISION \
    -outdir ${OUT} -channels $PP -dname $NAME $MAXDATA $MAXPERCLASS \
    -ignore_difficult \
    -resize $RESIZE -kernelsz $KERNEL -dims ${H}x${W}x3 \
    -disp \
    -exclude hand -exclude foot -usepose -mindims 32x32
