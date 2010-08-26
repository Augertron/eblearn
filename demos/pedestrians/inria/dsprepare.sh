#!/bin/sh

dsname=inria
################################################################################
# meta commands
meta_command="sh dsprepare.sh"
meta_name=${dsname}_dsprepare
meta_send_email=1 # emailing results or not
meta_email=${myemail} # email to use (use environment variable "myemail")

################################################################################
# ped dataset compilation
machine=${HOSTNAME}a
root=~/${machine}data/ped/${dsname}/
dataroot=$root/train/data/
positive_root=$dataroot/ped/
negative_root=$dataroot/bg/
full_negative_root=$root/train/bg_full
out=$root/ds/
bin=${HOME}/eblearn/bin/

h=80 # target height
w=40 # target width
chans=1 # target color channels
maxval=200 # number of samples per class in validation set
draws=1 # number of train/val sets to draw
precision=float
pp=Yp # preprocessing
kernel=7 #9
resize=mean #bilinear
nbg=1 # maximum number of bg extracted per scale
# scales in bg images, in terms of factor of the target size, i.e. hxw * scale
bgscales=6,3,1
maxbg=3000 # initial number of negatives

# names
id=${resize}${h}x${w}_ker${kernel}
name=${dsname}_${id}
namebg=${name}_bg
bgds=nopersons_${id}
outbg=${out}/${bgds}

# debug variables
maxdata="-maxdata 50"
maxperclass="-maxperclass 25"
ddisplay="-disp -sleep 10000"
debug= #"$maxdata $maxperclass $ddisplay"

# create directories
mkdir -p $out
mkdir -p $outbg

###############################################################################
# one-time dataset preparations

# inria humans are centered as 96x40 in 160x96 images, crop tight
# around human for negative examples.
# cropping factors: 0.6x0.41

# # extract 'inside' windows (zoom inside bounding box) 
# # from all positive examples as negative examples
# $bin/dscompiler $dataroot -precision $precision \
#     -outdir $full_negative_root/bg/inside -save mat -resize $resize \
#     -bboxhfact .6 -bboxwfact .41 $debug

# # copy 'inside' negatives to initial bg directory
# cp -R $full_negative_root/bg/inside/* $negative_root

###############################################################################
# (repeatable) dataset compilations

# remove previous background extractions
rm -Rf $outbg

# extract background images at random scales and positions
$bin/dscompiler $full_negative_root -type patch -precision $precision \
    -outdir $outbg/bg -scales $bgscales -dims ${h}x${w}x${chans} \
    -maxperclass $nbg -channels $pp -resize $resize -kernelsz $kernel \
    -maxdata $maxbg -nopadded $debug

# compile background dataset
$bin/dscompiler ${outbg} -precision $precision \
    -outdir ${out} -dname ${bgds} -dims ${h}x${w}x${chans} $debug

# compile regular dataset
# crop inria so that the window height is 1.3 the height of the pedestrians,
# i.e. H96 gives 125 window height, and cropping factor of .78
# then width target is 62.4, yielding cropping factor of .65
$bin/dscompiler $dataroot -precision $precision -outdir ${out} -channels $pp \
    -dname $name -resize $resize -kernelsz $kernel -dims ${h}x${w}x${chans} \
    -bboxhfact .78 -bboxwfact .65 $debug

# delete temporary images
rm -Rf $outbg

# merge normal dataset with background dataset
$bin/dsmerge $out ${namebg} ${bgds} ${name}

# split validation and training
$bin/dssplit $out ${namebg} \
    ${namebg}_val_ ${namebg}_train_ -maxperclass ${maxval} -draws ${draws}

# print out information about extracted datasets to check that their are ok
$bin/dsdisplay $out/${namebg} -info

