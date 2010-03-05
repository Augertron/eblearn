#!/bin/sh

################################################################################
# meta commands
################################################################################
# the command to run
meta_command="sh pascal_dsprepare.sh"
# name of this meta job
meta_name=pascalds
# maximum number of cpus to use at the same time
meta_max_cpus=8
# directory where to write outputs of all processes
meta_output_dir=${root}/out/
# emailing results or not
meta_send_email=1
# email to use
meta_email=pierre.sermanet@gmail.com
# interval in seconds to analyze processes output, and to check who is alive.
meta_watch_interval=5

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
max=10 # number of samples in validation set
draws=5 # number of train/val sets to draw
precision=float
pp=YpUV
kernel=7 9
resize=mean bilinear
nbg=2
bgscales=6,4,2,1
bboxfact=1.2
easy=0 1
occluded=${easy}
truncated=${easy}
difficult=0

# names
id=${resize}${h}x${w}_ker${kernel}_diff${difficult}trunc${truncated}occl${occluded}
name=all_${id}
namebg=${name}_bg
bgds=pascalbg_${id}
outbg=${out}/${bgds}
partsname=parts${name}

# debug variables
# maxdata="-maxdata 50"
# maxperclass="-maxperclass 25"
# ddisplay="-disp -sleep 1000"

# create directories
mkdir $out 2> /dev/null > /dev/null
mkdir $outbg 2> /dev/null > /dev/null

# ignore flags
if [ $difficult -eq 0 ]
then
    diff_cmd="-ignore_difficult"
fi
if [ $occluded -eq 0 ]
then
    occl_cmd="-ignore_occluded"
fi
if [ $truncated -eq 0 ]
then
    trunc_cmd="-ignore_truncated"
fi

# get pascal dataset
#wget http://pascallin.ecs.soton.ac.uk/challenges/VOC/voc2009/VOCtrainval_11-May-2009.tar $r3d
#tar xvf "${pascalroot0}/voctrainval_11-may-2009.tar" -c $pascalroot0/
#mv $pascalroot0/vocdevkit $pascalroot0/vocdevkit_trainval09

# # extract background images at different scales
# ~/eblearn/bin/dscompiler $pascalroot -type pascalbg -precision $precision \
#     -outdir $outbg/bg -scales $bgscales -dims ${h}x${w}x3 \
#     -maxperclass $nbg \
#     -channels $pp -resize $resize -kernelsz $kernel \
#     $maxdata $ddisplay # debug

# # compile background dataset
# ~/eblearn/bin/dscompiler ${outbg} -type lush -precision $precision \
#     -outdir ${out} -dname ${bgds}_${nbg} $maxdata $maxperclass \
#     -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# # delete temporary images
# rm -Rf $outbg

# # compile regular dataset
# ~/eblearn/bin/dscompiler $pascalroot -type pascal -precision $precision \
#     -outdir ${out} -channels $pp -dname $name $diff_cmd $occl_cmd $trunc_cmd \
#     -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 -bboxfact $bboxfact \
#     $maxdata $maxperclass $ddisplay # debug

# merge normal dataset with background dataset
~/eblearn/bin/dsmerge $out ${namebg} ${name} ${bgds}_$nbg

# split dataset into training/validation
~/eblearn/bin/dssplit $out ${namebg} ${namebg}_val_${max}_ \
    ${namebg}_train_${max}_ -maxperclass ${max} -draws $draws

# extract parts dataset
~/eblearn/bin/dscompiler $pascalroot -type pascal -precision $precision \
    -outdir ${out} -channels $pp -dname $partsname \
    $diff_cmd $occl_cmd $trunc_cmd \
    -resize $resize -kernelsz $kernel -dims ${h}x${w}x3 \
    -useparts -partsonly -bboxfact $bboxfact \
    $maxdata $maxperclass $ddisplay # debug
 #-usepose -mindims 16x16 

# print out information about extracted datasets to check that their are ok
~/eblearn/bin/dsdisplay ${out}/${namebg} -info
~/eblearn/bin/dsdisplay ${out}/${partsname} -info

# email yourself the results
here=`pwd`
base="`basename ${here}`"
tgz_name="logs_${base}.tgz"
tar czvf ${tgz_name} out*.log
cat $0 | mutt $meta_email -s "pascal dsprepare" -a ${tgz_name}

