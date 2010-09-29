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
inria_root=$root/INRIAPerson/
dataroot_pos=$root/train/data_pos/
positive_root=$dataroot_pos/ped/
zoomedin_negative_root=$root/train/neg/zoomed_in/bg/
zoomedout_negative_root=$root/train/neg/zoomed_out/bg/
full_negative_root=$root/train/neg/bg_full/bg
out=$root/ds/
bin=${HOME}/eblearn/bin/

h=128 # 160 # 128 # target height
w=76 # 96 # 76 # target width
chans=1 # target color channels
maxval=200 # number of samples per class in validation set
draws=1 # number of train/val sets to draw
precision=float
pp=Yp # preprocessing
kernel=7 #9
resize=mean #bilinear
nbg=1 # maximum number of bg extracted per scale
# scales in bg images, in terms of factor of the target size, i.e. hxw * scale
bgscales=4,3
maxbg=1000 # initial number of negatives
max_zoomedin=1000 # initial number of zoomed in negatives
max_zoomedout=1000 # initial number of zoomed out negatives

# names
id=${resize}${h}x${w}_ker${kernel}
name=${dsname}_${id}
name_pos=${name}_pos
name_neg=${name}_neg
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
mkdir -p $positive_root
mkdir -p $zoomedin_negative_root
mkdir -p $full_negative_root

# stop if error
check_error() {
    if [ $1 -ne 0 ]
    then
	echo "Error, exiting."
	exit -1
    fi
}

###############################################################################
# one-time dataset preparations

# copy data from inria root to our directories
cp -R $inria_root/96X160H96/Train/pos/* $positive_root # the positive examples
cp -R $inria_root/Train/neg/* $full_negative_root # the full negative examples

# inria humans are centered as 96x40 in 160x96 images, crop tight
# around human for negative examples.
# cropping factors: 160x96 * 0.6x0.41 -> 96x39

# extract 'zoomed in' windows (zoom inside bounding box) 
# from all positive examples as negative examples
$bin/dscompiler $dataroot_pos -precision $precision \
    -outdir $zoomedin_negative_root -save mat -resize $resize \
    -bboxhfact .7 -bboxwfact .7 $debug
check_error $? 

# extract 'zoomed out' windows
# from all positive examples as negative examples
# (the original 160x96 images, which are zoomed out compared to the target
# positives 128x64)
# multiply width by .8333 so that ratio is 2:1
mkdir -p $zoomedout_negative_root
cp -R $inria_root/96X160H96/Train/pos/* $zoomedout_negative_root
# $bin/dscompiler $dataroot_pos -precision $precision \
#     -outdir $zoomedout_negative_root -save mat -resize $resize \
#     -dims ${h}x${w}x3 # -bboxwfact .83333 $debug
# check_error $? 

###############################################################################
# (repeatable) dataset compilations

# remove previous background extractions
rm -Rf $outbg

# extract background images at random scales and positions
$bin/dscompiler $full_negative_root/../ -type patch -precision $precision \
    -outdir $outbg/bg -scales $bgscales -dims ${h}x${w}x${chans} \
    -maxperclass $nbg -channels $pp -resize $resize -kernelsz $kernel \
    -maxdata $maxbg -nopadded $debug
check_error $? 

# compile background dataset
$bin/dscompiler ${outbg} -precision $precision \
    -outdir ${out} -dname ${bgds} -dims ${h}x${w}x${chans} $debug
check_error $? 

# delete temporary images
rm -Rf $outbg

# compile regular POSITIVE ONLY dataset
# crop inria so that the window height is 1.33333 the height of the pedestrians,
# i.e. H96 gives 128 window height, and cropping factor of .8
# then width target is 76, yielding cropping factor of .791
$bin/dscompiler ${dataroot_pos} -precision $precision -outdir ${out} \
    -channels $pp -dname ${name_pos} -resize $resize -kernelsz $kernel \
    -dims ${h}x${w}x${chans} \
    -bboxhfact .8 -bboxwfact .791 $debug
check_error $? 

# compile regular zoomed in negative dataset (no cropping)
$bin/dscompiler ${zoomedin_negative_root}/../ -precision $precision \
    -outdir ${out} -channels $pp -dname ${name_neg} -resize $resize \
    -kernelsz $kernel -dims ${h}x${w}x${chans} -maxdata $max_zoomedin \
    $debug
check_error $? 

# merge normal negative dataset with background dataset
$bin/dsmerge $out ${namebg} ${bgds} ${name_neg}
check_error $? 

if [ -d ${zoomedout_negative_root} ] ; then
    # compile regular zoomed out negative dataset (no cropping)
    $bin/dscompiler ${zoomedout_negative_root}/../ -precision $precision \
	-outdir ${out} -channels $pp -dname ${name_neg} -resize $resize \
	-kernelsz $kernel -dims ${h}x${w}x${chans} -maxdata $max_zoomedout \
	$debug
    check_error $? 
    
    # merge normal negative dataset with background dataset
    $bin/dsmerge $out ${namebg} ${namebg} ${name_neg}
    check_error $? 
fi

# merge normal positive and negative datasets
$bin/dsmerge $out ${namebg} ${name_pos} ${namebg}
check_error $? 

# split validation and training
$bin/dssplit $out ${namebg} \
    ${namebg}_val_ ${namebg}_train_ -maxperclass ${maxval} -draws ${draws}
check_error $? 

# print out information about extracted datasets to check that their are ok
$bin/dsdisplay $out/${namebg} -info
check_error $? 

