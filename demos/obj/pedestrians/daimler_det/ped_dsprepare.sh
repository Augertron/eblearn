#!/bin/sh

################################################################################
# meta commands
################################################################################
meta_command="sh ped_dsprepare.sh"
meta_name=pedds_daimer_det
# emailing results or not
meta_send_email=1
# email to use
meta_email=pierre.sermanet@gmail.com

################################################################################
# ped dataset compilation
################################################################################

machine=banquoa
# directories
dataroot=~/${machine}data/ped/daimler_detection/
train_root=$dataroot/train/data/
out=$dataroot/ds/
nopersons_root=$dataroot/train/bg_full/
bin=${HOME}/eblearn/bin/

# target size
h=96
w=48
# number of samples in validation set
maxval=2000
draws=5 # number of train/val sets to draw
precision=float
pp=Yp
kernel=7 #9
resize=mean #bilinear
# maximum number of bg extracted per image and per scale
nbg=1
# scales in bg images, in terms of factor of the target size, i.e. hxw * scale
bgscales=8,6,4
# initial non-pedestrians should be 15660 to compare with Enzweiler/Gravila
maxbg=15660

# names
id=${resize}${h}x${w}_ker${kernel}
name=ped_daimlerdet_${id}
namebg=${name}_bg
bgds=nopersons_${id}
outbg=${out}/${bgds}

# debug variables
# maxdata="-maxdata 50"
# maxperclass="-maxperclass 25"
#ddisplay="-disp -sleep 1000"

# create directories
mkdir -p $root
mkdir -p $out
mkdir -p $outbg
mkdir -p $nopersons_root

###############################################################################
# dataset compilations
###############################################################################

# extract background images at random scales and positions
$bin/dscompiler $nopersons_root -type patch -precision $precision \
    -outdir $outbg/bg -scales $bgscales -dims ${h}x${w}x3 \
    -maxperclass $nbg -channels $pp -resize $resize -kernelsz $kernel \
    -maxdata $maxbg -nopadded \
    $ddisplay # debug

# compile background dataset
$bin/dscompiler ${outbg} -precision $precision \
    -outdir ${out} -dname ${bgds} -dims ${h}x${w}x3 \
    # $maxdata $maxperclass $ddisplay # debug

# compile regular dataset
$bin/dscompiler $root -precision $precision -outdir ${out} -channels $pp \
    -dname $name -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
#    $maxdata $maxperclass $ddisplay # debug

# merge normal dataset with background dataset
$bin/dsmerge $out ${namebg} ${bgds} ${name}

# split validation and training
$bin/dssplit $out ${namebg} \
    ${namebg}_val_ ${namebg}_train_ -maxperclass ${maxval} -draws ${draws}

# print out information about extracted datasets to check that their are ok
$bin/dsdisplay $out/${namebg} -info
