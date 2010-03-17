#!/bin/sh

################################################################################
# meta commands
################################################################################
meta_command="sh text_dsprepare.sh"
meta_name=textds
meta_email=pierre.sermanet@gmail.com

################################################################################
# text dataset compilation
################################################################################

# directories
dataroot=/data
#dataroot=~/texieradata
#dataroot=~/humairadata
#dataroot=~/blakeyadata
root=$dataroot/text/data
out=$dataroot/text/ds/
nopersons_root=$dataroot/nopersons/
false_positive_root=$dataroot/text/false_positives/

# variables
h=32 #48 64
w=${h}
max=300 # number of samples in test AND validation set
maxtest=0 # number of samples in the test set
draws=5 # number of train/val sets to draw
precision=float
pp=YpUV
kernel=7 #9
resize=mean #bilinear
nbg=2
bgscales=8,4,2,1

# names
id=${resize}${h}x${w}_ker${kernel}
name=text_${id}
namebg=${name}_bg
bgds=pascalbg_${id}
outbg=${out}/${bgds}
partsname=parts${name}
texts=text
fp_name=false_positives_${detector_name}
all_fp=${namebg}_${detector_name}

# debug variables
maxdata="-maxdata 50"
maxperclass="-maxperclass 25"
ddisplay="-disp -sleep 1000"

# create directories
mkdir -p $root
mkdir -p $out
mkdir -p $outbg
mkdir -p $nopersons_root
mkdir -p $false_positive_root
mkdir -p "$false_positive_root/bg/"

###############################################################################
# fetch datasets
###############################################################################

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

# # extract texts from pascal
# ~/eblearn/bin/dscompiler $pascalroot -type pascal -precision $precision \
#     -outdir $out -dims ${h}x${w}x3 \
#     -channels $pp -ignore_difficult -resize $resize -kernelsz $kernel \
#     -mindims 24x24 -include "head_Frontal" \
#     -useparts -dname ${namepheads_temp} -usepose \
#     $maxdata $maxperclass $ddisplay # debug

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

# # merge normal dataset with background dataset
# ~/eblearn/bin/dsmerge $out ${namebg} ${bgds}_$nbg ${name}

# # # merge pascal texts with regular dataset
# # ~/eblearn/bin/dsmerge $out ${namebgpheads} ${namebg} ${namepheads_temp}

# # split dataset into training and {validation/test}
# ~/eblearn/bin/dssplit $out ${namebg} \
#     ${namebg}_testval_${maxtest}_ \
#     ${namebg}_train_${maxtest}_ -maxperclass ${max} -draws $draws

# # split validation and test
# for i in `seq 1 ${draws}`
# do
# ~/eblearn/bin/dssplit $out ${namebg}_testval_${maxtest}_$i \
#     ${namebg}_test_${maxtest}_$i \
#     ${namebg}_val_${maxtest}_$i -maxperclass ${maxtest} -draws 1
# done

# # print out information about extracted datasets to check that their are ok
# ~/eblearn/bin/dsdisplay $out/${namebg} -info

###############################################################################
# false positive dataset compilations
###############################################################################

# # extract all pascal full images that do not contain texts
# ~/eblearn/bin/dscompiler $pascalroot -type pascalfull -precision $precision \
#     -outdir $nopersons_root_pascal -exclude "person" \
#     $maxdata $ddisplay # debug

# # generate false positives
# cd "${false_positive_root}/bg/" && \
# ~/eblearn/bin/objrec_detect \
#     ~/eblearn/demos/objrec/text/trained/${detector_name}.conf \
#     ${nopersons_root} \
# && cd -

# # compile false positive dataset
# ~/eblearn/bin/dscompiler ${false_positive_root} -type lush \
#     -precision ${precision} -input_precision ${precision} -outdir ${out} \
#     -dname ${fp_name} \
#     -dims ${h}x${w}x3 \
#     $maxdata $maxperclass $ddisplay # debug

# # merge normal dataset with background dataset
# ~/eblearn/bin/dsmerge $out ${all_fp} ${fp_name} ${namebg}

# # split dataset into training and {validation/test}
# ~/eblearn/bin/dssplit $out ${all_fp} \
#     ${all_fp}_testval_${maxtest}_ \
#     ${all_fp}_train_${maxtest}_ -maxperclass ${max} -draws $draws

# # split validation and test
# for i in `seq 1 ${draws}`
# do
# ~/eblearn/bin/dssplit $out ${all_fp}_testval_${maxtest}_$i \
#     ${all_fp}_test_${maxtest}_$i \
#     ${all_fp}_val_${maxtest}_$i -maxperclass ${maxtest} -draws 1
# done

# # print out information about extracted datasets to check that their are ok
# ~/eblearn/bin/dsdisplay $out/${all_fp} -info

# ###############################################################################
# # reporting
# ###############################################################################

# # email yourself the results
# here=`pwd`
# base="`basename ${here}`"
# tgz_name="logs_${base}.tgz"
# tar czvf ${tgz_name} out*.log
# cat $0 | mutt $meta_email -s "text dsprepare" -a ${tgz_name}

