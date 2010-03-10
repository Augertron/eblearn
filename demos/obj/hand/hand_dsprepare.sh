#!/bin/sh

################################################################################
# meta commands
################################################################################
meta_command="sh hand_dsprepare.sh"
meta_name=handds
meta_email=pierre.sermanet@gmail.com

################################################################################
# face dataset compilation
################################################################################

# directories
#dataroot=/data
#dataroot=~/texieradata
dataroot=~/humairadata
#dataroot=~/blakeyadata
pascal=$dataroot/pascal
pascalroot=$pascal/VOCdevkit_trainval09/VOC2009/
root=$dataroot/hand/data
out=$dataroot/face/ds/
nopersons_root=$out/nopersons/
nopersons_root_pascal=$nopersons_root/pascal/
detector_name=20100211.025504.face_conf00_color_0_eta_.00001_resize_mean
false_positive_root=$dataroot/face/false_positives/$detector_name/

# variables
h=48 64
w=${h}
max=10 # number of samples in test AND validation set
maxtest=0 # number of samples in the test set
draws=2 # number of train/val sets to draw
precision=float
pp=YpUV
kernel=7 #9
resize=mean #bilinear
nbg=2
bgscales=8,4,2,1

# names
id=${resize}${h}x${w}_ker${kernel}
name=hand_${id}
namebg=${name}_bg
bgds=pascalbg_${id}
outbg=${out}/${bgds}
partsname=parts${name}
faces=face

# debug variables
# maxdata="-maxdata 50"
# maxperclass="-maxperclass 25"
# ddisplay="-disp -sleep 1000"

# create directories
mkdir -p $pascalroot
mkdir -p $root
mkdir -p $out
mkdir -p $outbg
mkdir -p $nopersons_root
mkdir -p $nopersons_root_pascal

###############################################################################
# fetch datasets
###############################################################################

# # crop hands
# cd $root
# # crop pictures to center well on the face
# for fname in `find . -name "*.JPG"`
# do
#     echo "cropping $fname"
#     convert -crop 1200x1200+434+0 $fname $fname
# done

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

# # # extract faces from pascal
# # ~/eblearn/bin/dscompiler $pascalroot -type pascal -precision $precision \
# #     -outdir $out -dims ${h}x${w}x3 \
# #     -channels $pp -ignore_difficult -resize $resize -kernelsz $kernel \
# #     -mindims 24x24 -include "head_Frontal" \
# #     -useparts -dname ${namepheads_temp} -usepose \
# #     $maxdata $maxperclass $ddisplay # debug

# # compile background dataset
# ~/eblearn/bin/dscompiler ${outbg} -type lush -precision $precision \
#     -outdir ${out} -dname ${bgds}_${nbg} \
#     -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# compile regular dataset
~/eblearn/bin/dscompiler $root -precision $precision \
    -outdir ${out} -channels $pp -dname $name \
    -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
    $maxdata $maxperclass $ddisplay # debug

# merge normal dataset with background dataset
~/eblearn/bin/dsmerge $out ${namebg} ${bgds}_$nbg ${name}

# # # merge pascal faces with regular dataset
# # ~/eblearn/bin/dsmerge $out ${namebgpheads} ${namebg} ${namepheads_temp}

# split dataset into training and {validation/test}
~/eblearn/bin/dssplit $out ${namebg} \
    ${namebg}_testval_${maxtest}_ \
    ${namebg}_train_${maxtest}_ -maxperclass ${max} -draws $draws

# split validation and test
for i in `seq 1 ${draws}`
do
~/eblearn/bin/dssplit $out ${namebg}_testval_${maxtest}_$i \
    ${namebg}_val_${maxtest}_$i \
    ${namebg}_test_${maxtest}_$i -maxperclass ${maxtest} -draws 1
done

# # print out information about extracted datasets to check that their are ok
# ~/eblearn/bin/dsdisplay $out/${namebg} -info

###############################################################################
# false positive dataset compilations
###############################################################################

# # extract all pascal full images that do not contain faces
# ~/eblearn/bin/dscompiler $pascalroot -type pascalfull -precision $precision \
#     -outdir $nopersons_root_pascal -exclude "person" \
#     $maxdata $ddisplay # debug

# # compile false positive dataset
# ~/eblearn/bin/dscompiler ${false_positive_root} -type lush \
#     -precision $precision -outdir ${out} \
#     -dname false_positives_${detector_namebgds} \
#     -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# ###############################################################################
# # reporting
# ###############################################################################

# # email yourself the results
# here=`pwd`
# base="`basename ${here}`"
# tgz_name="logs_${base}.tgz"
# tar czvf ${tgz_name} out*.log
# cat $0 | mutt $meta_email -s "face dsprepare" -a ${tgz_name}

