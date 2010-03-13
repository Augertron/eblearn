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
xpname=face_train_20100312.194954 # face_train_`date +"%Y%m%d.%H%M%S"`
root=~/budadata/face/
root2=~/budcdata/face/
dataroot=$root/ds
out=$root/out/$xpname/
eblearnbin0=~/eblearn/bin/
eblearnbin=${out}/bin/
nopersons_root=$root2/nopersons/
false_positive_root=$root2/false_positives/$xpname/

# variables

metaconf0=${eblearnbin}/face_meta.conf
metaconf=/tmp/face_meta.conf
# maximum number of retraining iterations
maxiteration=10
# stop if reached that error rate
# target_error_rate=0.0001
h=32
w=${h}
precision=float
# split percentage between train and validation
ds_split_percent=10

# create directories
mkdir -p $out
mkdir -p $eblearnbin
mkdir -p $nopersons_root
mkdir -p $false_positive_root
mkdir -p "$false_positive_root/bg/"

# copy binaries
cp $eblearnbin0/* $eblearnbin/

# make a copy of meta conf and override its output dir
cp $metaconf0 $metaconf
echo "meta_output_dir = ${out}" >> $metaconf

###############################################################################
# training
###############################################################################

# initial training
#${eblearnbin}/metarun $metaconf
echo "toto"
# looping on retraining on false positives
for iter in `seq 1 ${maxiteration}`
  do
  
# find path to latest metarun output: get directory with latest date
  lastout=`ls -dt1 ${out}/*/ | head -1`
  bestout=${lastout}/best/01/
# find path to best conf (there should be only 1 conf per folder)
  bestconf=`ls ${bestout}/*.conf`

  echo "___________Retraining iteration: ${iter}___________"
  echo "Using best conf of previous training: ${bestconf}"

# extract false positives: first add new variables to best conf
# activate retraining
  echo "retrain = 1" >> $bestconf
# force saving detections
  echo "save_detections = 1" >> $bestconf
# do not save video
  echo "save_video = 0" >> $bestconf
# add directory where to find trained files
  echo "root2 = ${bestout}" >> $bestconf
# add subdirectories of retraining dir
  echo "retrain_dir = ${nopersons_root}/${retrain_dir_id}" >> $bestconf
  echo -n "retrain_dir_id = " >> $bestconf
  for idir in `seq 1 8`
    do
    echo -n "${idir} " >> $bestconf
  done
# override train command by detect command
  echo >> $bestconf
  echo "meta_command = objdetect" >> $bestconf
# start parallelized extraction
  ${eblearnbin}/metarun $bestconf
  
# find path to latest metarun output: get directory with latest date
  lastout=`ls -dt1 ${out}/*/ | head -1`

# # recompile data from last output directory which should contain 
# # all false positives
#   ${eblearnbin}/dscompiler ${lastout} -type lush \
#       -precision ${precision} -input_precision ${precision} -outdir ${lastout} \
#       -dname allfp -dims ${h}x${w}x3

# # get dataset size
#   dssize=`${eblearnbin}/dsdisplay -size -info`
#   valsize=$dssize percentage

# # split dataset into training and {validation/test}
#   ${eblearnbin}/dssplit $out ${all_fp} \
#       ${all_fp}_testval_${maxtest}_ \
#       ${all_fp}_train_${maxtest}_ -maxperclass ${max} -draws $draws

# # split validation and test
#     for i in `seq 1 ${draws}`
#     do
# 	${eblearnbin}/dssplit $out ${all_fp}_testval_${maxtest}_$i \
# 	    ${all_fp}_test_${maxtest}_$i \
# 	    ${all_fp}_val_${maxtest}_$i -maxperclass ${maxtest} -draws 1
#     done

# # merge previous datasets with new datasets
#     ${eblearnbin}/dsmerge ${lastout}/allfp  ${all_fp} ${fp_name} ${namebg}

# # print out information about extracted datasets to check that their are ok
#     ${eblearnbin}/dsdisplay $out/${all_fp} -info

# retrain on old + new data

# check if we reached the target error rate
    
done