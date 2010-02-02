#!/bin/sh

################################################################################
# meta commands
################################################################################
meta_command="sh pascal_dsprepare.sh"
meta_name=pascalds

################################################################################
# pascal dataset compilation
################################################################################

# directories
#dataroot=/data
#dataroot=~/texieradata
dataroot=~/humairadata
pascalroot=$dataroot/pascal/VOCdevkit_trainval09/VOC2009/
root=$dataroot/pascal/
out=$root/ds/

# variables
h=96
w=${h}
max=50 # number of samples in validation set
draws=5 # number of train/val sets to draw
precision=float
pp=YpUV
kernel=7 9
resize=gaussian bilinear
nbg=2
bgscales=8,6,4,2,1

# names
id=${resize}${h}x${w}_ker${kernel}
name=all_$id
namebg=${name}_bg
bgds=pascalbg_$id
outbg=$out/$bgds
partsname=parts${name}

# debug variables
# maxdata="-maxdata 50"
# maxperclass="-maxperclass 25"
# ddisplay="-disp -sleep 1000"

# create directories
mkdir $out 2> /dev/null > /dev/null
mkdir $outbg 2> /dev/null > /dev/null

# get pascal dataset
#wget http://pascallin.ecs.soton.ac.uk/challenges/voc/voc2009/voctrainval_11-may-2009.tar $r3d
#tar xvf "${pascalroot0}/voctrainval_11-may-2009.tar" -c $pascalroot0/
#mv $pascalroot0/vocdevkit $pascalroot0/vocdevkit_trainval09

# extract background images at different scales
~/eblearn/bin/dscompiler $pascalroot -type pascalbg -precision $precision \
    -outdir $outbg/bg -scales $bgscales -dims ${h}x${w}x3 \
    -maxperclass $nbg \
    -channels $pp -ignore_difficult -resize $resize -kernelsz $kernel \
    $maxdata $ddisplay # debug

# compile background dataset
~/eblearn/bin/dscompiler ${outbg} -type lush -precision $precision \
    -outdir ${out} -dname ${bgds}_${nbg} $maxdata $maxperclass \
    -dims ${h}x${w}x3 \
    $maxdata $maxperclass $ddisplay # debug

# compile regular dataset
~/eblearn/bin/dscompiler $pascalroot -type pascal -precision $precision \
    -outdir ${out} -channels $pp -dname $name -ignore_difficult \
    -resize $resize -kernelsz $kernel -dims ${h}x${w}x3  \
    $maxdata $maxperclass $ddisplay # debug

# merge normal dataset with background dataset
~/eblearn/bin/dsmerge $out ${namebg} ${name} ${bgds}_$nbg

# split dataset into training/validation
~/eblearn/bin/dssplit $out ${namebg} ${namebg}_val_${max}_ \
    ${namebg}_train_${max}_ -maxperclass ${max} -draws $draws

# extract parts dataset
~/eblearn/bin/dscompiler $pascalroot -type pascal -precision $precision \
    -outdir ${out} -channels $pp -dname $partsname -ignore_difficult \
    -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
    -useparts -partsonly \
    $maxdata $maxperclass $ddisplay # debug
 #-usepose -mindims 16x16 

# print out information about extracted datasets to check that their are ok
~/eblearn/bin/dsdisplay ${out}/${namebg} -info
~/eblearn/bin/dsdisplay ${out}/${partsname} -info
