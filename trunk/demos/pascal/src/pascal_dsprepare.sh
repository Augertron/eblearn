#!/bin/sh

################################
# pascal dataset compilation
################################

# directories
#DATAROOT=/data
DATAROOT=~/texieradata
PASCALROOT=$DATAROOT/pascal/VOCdevkit_trainval09/VOC2009/
ROOT=$DATAROOT/pascal/
OUT=$ROOT/ds/

# variables
H=96
W=${H}
NAME=all${RESIZE}${H}x${W}_ker${KERNEL}
NAMEBG=${NAME}bg
MAX=50 # number of samples in validation set
DRAWS=5 # number of train/val sets to draw
PRECISION=float
PP=YpUV
KERNEL=9
RESIZE=gaussian #bilinear
NBG=1
BGSCALES=1,2,4
BGDS=pascalbg${RESIZE}${H}x${W}_ker${KERNEL}
OUTBG=$OUT/$BGDS

# debug variables
MAXDATA="-maxdata 50"
MAXPERCLASS="-maxperclass 25"
DDISPLAY="-disp -sleep 1000"

# create directories
mkdir $OUT 2> /dev/null > /dev/null
mkdir $OUTBG 2> /dev/null > /dev/null

# get pascal dataset
#wget http://pascallin.ecs.soton.ac.uk/challenges/VOC/voc2009/VOCtrainval_11-May-2009.tar $R3D
#tar xvf "${PASCALROOT0}/VOCtrainval_11-May-2009.tar" -C $PASCALROOT0/
#mv $PASCALROOT0/VOCdevkit $PASCALROOT0/VOCdevkit_trainval09

# extract background images at different scales
~/eblearn/bin/dscompiler $PASCALROOT -type pascalbg -precision $PRECISION \
    -outdir $OUTBG/bg -scales $BGSCALES -dims ${H}x${W}x3 \
    -maxperclass $NBG \
    -channels $PP -ignore_difficult -resize $RESIZE -kernelsz $KERNEL \
    $MAXDATA $DDISPLAY # debug

# compile background dataset
~/eblearn/bin/dscompiler ${OUTBG} -type lush -precision $PRECISION \
    -outdir ${OUT} -dname ${BGDS}_${NBG} $MAXDATA $MAXPERCLASS \
    -dims ${H}x${W}x3 \
    $MAXDATA $MAXPERCLASS $DDISPLAY # debug

# compile regular dataset
~/eblearn/bin/dscompiler $PASCALROOT -type pascal -precision $PRECISION \
    -outdir ${OUT} -channels $PP -dname $NAME -ignore_difficult \
    -resize $RESIZE -kernelsz $KERNEL -dims ${H}x${W}x3 \
    $MAXDATA $MAXPERCLASS $DDISPLAY # debug

# merge normal dataset with background dataset
~/eblearn/bin/dsmerge $OUT ${NAMEBG} ${NAME} ${BGDS}_$NBG

# split dataset into training/validation
~/eblearn/bin/dssplit $OUT ${NAMEBG} ${NAMEBG}_val_${MAX}_ \
    ${NAMEBG}_train_${MAX}_ -maxperclass ${MAX} -draws $DRAWS

# extract parts dataset
~/eblearn/bin/dscompiler $PASCALROOT -type pascalpart -precision $PRECISION \
    -outdir ${OUT} -channels $PP -dname $NAME -ignore_difficult \
    -resize $RESIZE -kernelsz $KERNEL -dims ${H}x${W}x3 \
    -exclude hand -exclude foot -usepose -mindims 32x32 \
    $MAXDATA $MAXPERCLASS $DDISPLAY # debug
