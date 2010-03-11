#!/bin/sh

################################################################################
# meta commands
################################################################################

# required variables
meta_command="sh face_train.sh"

# optional meta variables ######################################################

# directory where to write outputs of all processes
meta_output_dir=${out}
# name of this meta job
meta_name=faceds
# emailing results or not
meta_send_email=1
# email to use
meta_email=pierre.sermanet@gmail.com

################################################################################
# variables
################################################################################

# directories
dataroot=~/humairadata/face/ds
out=${dataroot}/../out/face_train_`date +"%Y%m%d.%H%M%S"`
eblearnbin0=~/eblearn/bin/
eblearnbin=${eblearnbin0}/bin/
nopersons_root=$dataroot/nopersons/
detector_name=20100223.002945.face_conf00_eta_.00001
false_positive_root=$dataroot/face/false_positives/$detector_name/

# variables

# maximum number of retraining iterations
maxiteration=15
# stop if reached that error rate
target_error_rate=0.0001

# create directories
mkdir -p $out
mkdir -p $eblearnbin
mkdir -p $nopersons_root
mkdir -p $false_positive_root
mkdir -p "$false_positive_root/bg/"

# copy binaries
cp $eblearnbin0/* $eblearnbin/

###############################################################################
# training
###############################################################################

# initial training
${eblearnbin}/metarun ${eblearnbin}/face_meta.conf

# looping on retraining on false positives
for iter in `seq 1 .. ${maxiteration}`
do
    
# extract false positives
    cd "${false_positive_root}/bg/" && \
	${eblearnbin}/obdetect \
	~/eblearn/demos/objrec/face/trained/${detector_name}.conf \
	${nopersons_root} \
	&& cd -
    
# recompile data
    ${eblearnbin}/dscompiler ${false_positive_root} -type lush \
	-precision ${precision} -input_precision ${precision} -outdir ${out} \
	-dname ${fp_name} \
	-dims ${h}x${w}x3 \
	$maxdata $maxperclass $ddisplay # debug

# merge normal dataset with background dataset
    ${eblearnbin}/dsmerge $out ${all_fp} ${fp_name} ${namebg}

# split dataset into training and {validation/test}
    ${eblearnbin}/dssplit $out ${all_fp} \
	${all_fp}_testval_${maxtest}_ \
	${all_fp}_train_${maxtest}_ -maxperclass ${max} -draws $draws

# split validation and test
    for i in `seq 1 ${draws}`
    do
	${eblearnbin}/dssplit $out ${all_fp}_testval_${maxtest}_$i \
	    ${all_fp}_test_${maxtest}_$i \
	    ${all_fp}_val_${maxtest}_$i -maxperclass ${maxtest} -draws 1
    done

# print out information about extracted datasets to check that their are ok
    ${eblearnbin}/dsdisplay $out/${all_fp} -info

# retrain on old + new data

# check if we reached the target error rate
    
done