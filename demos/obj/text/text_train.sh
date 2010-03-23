#!/bin/sh

################################################################################
# meta commands
################################################################################

# required variables
meta_command="sh text_train.sh"

# optional meta variables ######################################################

# directory where to write outputs of all processes
meta_output_dir=${out}
# name of this meta job
meta_name=text_train_texier
# emailing results or not
meta_send_email=1
# email to use
meta_email=pierre.sermanet@gmail.com
# send email with this frequency (if email_iters not defined)
meta_email_period=1

# interval in seconds to analyze processes output, and to check who is alive.
meta_watch_interval=30
# variables to minimize, process and iteration with lowest value will
# be used to report best weights, or start consequent training
meta_minimize=i
# send n best answers that minimize meta_minimize's value
meta_send_best=15

################################################################################
# variables
################################################################################

# directories
xpname=${meta_name}_`date +"%Y%m%d.%H%M%S"`
root=~/texieradata/text/
root2=~/texieradata/
dataroot=$root/ds
out=$root/out/$xpname/
eblearnbin0=~/eblearn/bin/
eblearnbin=${out}/bin/
nopersons_root=$root2/nopersons/
false_positive_root=$root2/false_positives/$xpname/

# variables

metaconf0=${eblearnbin}/text_meta.conf
metaconf=${out}/text_meta.conf

# maximum number of retraining iterations
maxiteration=20
# threshold will be decremented at each iter until -.95
threshold=.9

# network input size and precision
h=32
w=${h}
precision=float

# split ratio of validation over training
ds_split_ratio=".1"
draws=5

# name of datasets
traindsnamea=text_mean32x32_ker7_bg_train
valdsname=text_mean32x32_ker7_bg_val

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
echo "________________________________________________________________________"
echo "initial training from metaconf: ${metaconf}"
${eblearnbin}/metarun $metaconf

# looping on retraining on false positives
echo "________________________________________________________________________"
echo "retraining loop"
for iter in `seq 1 ${maxiteration}`
  do

# find path to latest metarun output: get directory with latest date
  lastout=`ls -dt1 ${out}/*/ | head -1`
  bestout=${lastout}/best/01/
# find path to best conf (there should be only 1 conf per folder)
  bestconf=`ls ${bestout}/*.conf`
# find path to best weights (there should be only 1 weights per folder)
  bestweights=`ls ${bestout}/*_net*.mat`

  echo "___________Retraining iteration: ${iter}___________"
  echo "Using best conf of previous training: ${bestconf}"
  echo "i=`expr ${maxiteration} - ${iter}`"

#if [ $iter != 1 ]; then  

# extract false positives: first add new variables to best conf
# activate retraining
  echo "retrain = 1" >> $bestconf
# force saving detections
  echo "save_detections = 1" >> $bestconf
# do not save video
  echo "save_video = 0" >> $bestconf
  echo "save_max = 25000" >> $bestconf
  echo "save_max_per_frame = 10" >> $bestconf
# add directory where to find trained files
  echo "root2 = ${bestout}" >> $bestconf
# limit input size 
  echo "input_max = 900" >> $bestconf
# decrement threshold, capping at -.95
  threshold=`echo "thr=${threshold} - .2; if (thr < -.95){ thr = -.95;}; print thr" | bc`
  echo "threshold = ${threshold}" >> $bestconf
# set weights to retrain: same as this conf
  echo "retrain_weights = \${weights}" >> $bestconf
# add subdirectories of retraining dir
  echo "retrain_dir = ${nopersons_root}/\${retrain_dir_id}/" >> $bestconf
  echo -n "retrain_dir_id = " >> $bestconf
  for idir in `seq 1 2`
    do
    echo -n "${idir} " >> $bestconf
  done
# override train command by detect command
  echo >> $bestconf
  echo "meta_command = ${eblearnbin}/objdetect" >> $bestconf
  echo "meta_name = ${meta_name}_false_positives" >> $bestconf
# start parallelized extraction
  ${eblearnbin}/metarun $bestconf

# find path to latest metarun output: get directory with latest date
  lastout=`ls -dt1 ${out}/*/ | head -1`

# recompile data from last output directory which should contain 
# all false positives
  ${eblearnbin}/dscompiler ${lastout} -precision ${precision} \
      -outdir ${dataroot} -forcelabel bg -dname allfp -dims ${h}x${w}x3 \
      -image_pattern ".*[.]mat" -mindims ${h}x${w}x3 

# get dataset size
  dssize=`${eblearnbin}/dsdisplay ${dataroot}/allfp -size`
  echo "false_positives = ${dssize}"
  valsize=`echo "(${dssize} * ${ds_split_ratio})/1" | bc`
  echo "valsize = ${valsize}"
  
# print out information about extracted dataset to check it is ok
  ${eblearnbin}/dsdisplay $dataroot/allfp -info
    
# split dataset into training and validation
  ${eblearnbin}/dssplit ${dataroot} allfp \
      allfp_val_ allfp_train_ -maxperclass ${valsize} -draws $draws

# merge new datasets into previous datasets: training
  for i in `seq 1 $draws`
  do
      ${eblearnbin}/dsmerge ${dataroot} ${traindsname}_${i} \
	  allfp_train_${i} ${traindsname}_${i}
  done

# merge new datasets into previous datasets: validation
  for i in `seq 1 $draws`
  do
      ${eblearnbin}/dsmerge ${dataroot} ${valdsname}_${i} \
	  allfp_val_${i} ${valdsname}_${i}
  done

#fi  

# retrain on old + new data
  echo "Retraining from best previous weights: ${bestweights}"
# add last weights and activate retraining from those
  echo "meta_command = ${eblearnbin}/objtrain" >> $metaconf
  echo "retrain = 1" >> $metaconf
  echo "retrain_weights = ${bestweights}" >> $metaconf
  echo "meta_name = ${meta_name}_retrain_${iter}" >> $metaconf
  ${eblearnbin}/metarun $metaconf
    
done