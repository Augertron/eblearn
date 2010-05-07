#!/bin/sh

################################################################################
# meta commands
################################################################################
meta_command="sh ped_dsprepare.sh"
meta_name=pedds
# emailing results or not
meta_send_email=1
# email to use
meta_email=pierre.sermanet@gmail.com

################################################################################
# ped dataset compilation
################################################################################

machine=juno
# directories
#dataroot=/data
#dataroot=~/texieradata
dataroot=~/${machine}adata
pascal=$dataroot/pascal
pascalroot=$pascal/VOCdevkit_trainval09/VOC2009/
root=$dataroot/ped/data
out=$dataroot/ped/ds/
nopersons_root=$dataroot/nopersons/

# variables
h=80 #48 64
w=32
maxval=5000 # number of samples in validation set
draws=5 # number of train/val sets to draw
precision=float
pp=YpUV
kernel=7 #9
resize=mean #bilinear
nbg=2
bgscales=8,4,2,1

# names
id=${resize}${h}x${w}_ker${kernel}
name=ped_${id}
namebg=${name}_bg
#bgds=pascalbg_${id}
bgds=pascalbg_bilinear32x32_ker7_2
outbg=${out}/${bgds}

# debug variables
# maxdata="-maxdata 50"
# maxperclass="-maxperclass 25"
# ddisplay="-disp -sleep 1000"

# create directories
mkdir -p $root
mkdir -p $out
mkdir -p $outbg
mkdir -p $nopersons_root

###############################################################################
# dataset compilations
###############################################################################

# # extract background images at different scales from all images parts that
# # don't contain persons
# ~/eblearn/bin/dscompiler $pascalroot -type pascalbg -precision $precision \
#     -outdir $outbg/bg -scales $bgscales -dims ${h}x${w}x3 \
#     -maxperclass $nbg $maxdata -include "person" \
#     -channels $pp -resize $resize -kernelsz $kernel \
#     $maxdata $ddisplay # debug

# # compile background dataset
# ~/eblearn/bin/dscompiler ${outbg} -type lush -precision $precision \
#     -outdir ${out} -dname ${bgds}_${nbg} \
#     -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# compile regular dataset
~/eblearn/bin/dscompiler $root -precision $precision \
    -outdir ${out} -channels $pp -dname $name \
    -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
#    $maxdata $maxperclass $ddisplay # debug

# merge normal dataset with background dataset
~/eblearn/bin/dsmerge $out ${namebg} ${bgds} ${name}

# split validation and training
for i in `seq 1 ${draws}`
do
~/eblearn/bin/dssplit $out ${namebg}_val_$i \
    ${namebg}_val_$i ${namebg}_train_$i -maxperclass ${maxval} -draws 1
done

# print out information about extracted datasets to check that their are ok
~/eblearn/bin/dsdisplay $out/${namebg} -info
