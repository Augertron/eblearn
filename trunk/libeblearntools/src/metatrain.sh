#!/bin/sh

h=96
w=48
chans=1
traindsname=ped_daimlerdet_mean${h}x${w}_ker7_bg_train
valdsname=ped_daimlerdet_mean${h}x${w}_ker7_bg_val

ebl=$HOME/eblearn/
machine=${HOSTNAME}a
eblearnbin0=$ebl/bin/
metaconf_name=daimlerdet_meta.conf
# max number of false positives to extract per iteration
save_max=15660
# max number of false positives to extract per full image
save_max_per_frame=10
# number of threads to use duing false positive extraction
nthreads=5
# maximum number of retraining iterations
maxiteration=10

################################################################################
# meta commands
################################################################################

# required variables
meta_command="sh train.sh"

# optional meta variables ######################################################

# directory where to write outputs of all processes
meta_output_dir=${out}
# name of this meta job
meta_name=daimlerdet_${machine}
# emailing results or not
meta_send_email=1
# email to use (use environment variable "myemail")
meta_email=${myemail}
# send email with this frequency (if email_iters not defined)
meta_email_period=1

# interval in seconds to analyze processes output, and to check who is alive.
meta_watch_interval=120
# variables to minimize, process and iteration with lowest value will
# be used to report best weights, or start consequent training
meta_minimize=i
# send n best answers that minimize meta_minimize's value
meta_send_best=5

################################################################################
# variables
################################################################################

# directories
#tstamp=`date +"%Y%m%d.%H%M%S"`
tstamp=20100727.073128
xpname=${meta_name}_${tstamp}
root=~/${machine}data/ped/daimler_detection/
dataroot=$root/ds/
out=$root/out/$xpname/
eblearnbin=${out}/bin/
nopersons_root=$root/train/bg_full/

# variables

metaconf0=$ebl/demos/pedestrians/daimler_det/${metaconf_name}
metaconf=${out}/${metaconf_name}

precision=float

# threshold will be decremented at each iter until -.95
threshold=-.5

# split ratio of validation over training
ds_split_ratio=".1"
draws=3

# create directories
mkdir -p $out
mkdir -p $eblearnbin
mkdir -p $nopersons_root

# copy binaries
cp $eblearnbin0/* $eblearnbin/
# set path to libraries locally
export LD_LIBRARY_PATH=${eblearnbin}

# make a copy of meta conf and override its output dir
cp $metaconf0 $metaconf
echo "meta_output_dir = ${out}" >> $metaconf

###############################################################################
# functions
###############################################################################

touch $out/${tstamp}.${meta_name}_falsepos_3

# extract false positives
extract_falsepos() {
# function arguments
    bestconf=$1
    save_max=$2
    save_max_per_frame=$3
    bestout=$4 # directory of trained weights
    input_max=$5
    threshold=$6
    weights=$7
    negatives_root=$8 # directory where to find negative images
    eblearnbin=$9
    nthreads=$10
    npasses=$11
    max_scale=$12
    tstamp=$13
    iter=$14
    falsepos_dir=$15

# add new variables to best conf
# force saving detections
    echo "save_detections = 1" >> $bestconf
# do not save video
    echo "save_video = 0" >> $bestconf
    echo "display = 0" >> $bestconf
    echo "save_max = ${save_max}" >> $bestconf
    echo "save_max_per_frame = ${save_max_per_frame}" >> $bestconf
# add directory where to find trained files
    echo "root2 = ${bestout}" >> $bestconf
# limit input size 
    echo "input_max = 1000" >> $bestconf
# decrement threshold, capping at -.95
    threshold=`echo "thr=${threshold} - .2; if (thr < -.95){ thr = -.95;}; print thr" | bc`
    echo "threshold = ${threshold}" >> $bestconf
# set weights to retrain: same as this conf
    echo "retrain_weights = \${weights}" >> $bestconf
# set where to find full images
    echo "input_dir = ${negatives_root}/" >> $bestconf
# send report every 1000 frames processed
    echo "meta_email_iters = " >> $bestconf
    echo "meta_email_period = 1000" >> $bestconf
# override train command by detect command
    echo >> $bestconf
    echo "meta_command = \"export LD_LIBRARY_PATH=${eblearnbin} && ${eblearnbin}/mtdetect\"" >> $bestconf
    echo "meta_name = ${meta_name}_falsepos_${iter}" >> $bestconf
# set multi threads for detection
    echo "nthreads = ${nthreads}" >> $bestconf
# pass n times on the dataset to make sure we reach the desired amount of fp
    echo "input_npasses = ${npasses}" >> $bestconf
# randomize list of files to extract fp from
    echo "input_random = 1" >> $bestconf
# keep all detected bbox even overlapping ones
    echo "pruning = 0" >> $bestconf
# oversample to n times input (capped by input_max)
    echo "max_scale = ${max_scale}" >> $bestconf
# start parallelized extraction
    ${eblearnbin}/metarun $bestconf -tstamp ${tstamp}
    if [ $? -neq 0 ]; then exit -1; fi # stop if error
}

compile_data() {
    # arguments
    eblearnbin=$1
    falsepos_dir=$2
    precision=$3
    dataroot=$4
    h=$5
    w=$6
    chans=$7
    draws=$8
    traindsname=$9
    valdsname=$10

# recompile data from last output directory which should contain 
# all false positives
# note: no need to preprocess, .mat should already be preprocessed
  ${eblearnbin}/dscompiler ${falsepos_dir} -precision ${precision} \
      -outdir ${dataroot} -forcelabel bg -dname allfp -dims ${h}x${w}x${chans} \
      -image_pattern ".*[.]mat" -mindims ${h}x${w}x${chans}
  if [ $? -neq 0 ]; then exit -1; fi # stop if error

# get dataset size
  dssize=`${eblearnbin}/dsdisplay ${dataroot}/allfp -size`
  echo "false_positives = ${dssize}"
  valsize=`echo "(${dssize} * ${ds_split_ratio})/1" | bc`
  echo "valsize = ${valsize}"
  
# print out information about extracted dataset to check it is ok
  ${eblearnbin}/dsdisplay $dataroot/allfp -info
  if [ $? -neq 0 ]; then exit -1; fi # stop if error
    
# split dataset into training and validation
  ${eblearnbin}/dssplit ${dataroot} allfp \
      allfp_val_ allfp_train_ -maxperclass ${valsize} -draws $draws
  if [ $? -neq 0 ]; then exit -1; fi # stop if error

# merge new datasets into previous datasets: training
  for i in `seq 1 $draws`
  do
      ${eblearnbin}/dsmerge ${dataroot} ${traindsname}_${i} \
	  allfp_train_${i} ${traindsname}_${i}
      if [ $? -neq 0 ]; then exit -1; fi # stop if error
  done

# merge new datasets into previous datasets: validation
  for i in `seq 1 $draws`
  do
      ${eblearnbin}/dsmerge ${dataroot} ${valdsname}_${i} \
	  allfp_val_${i} ${valdsname}_${i}
      if [ $? -neq 0 ]; then exit -1; fi # stop if error
  done
}

# retrain on old + new data
  echo "Retraining from best previous weights: ${bestweights}"
# add last weights and activate retraining from those
  echo "meta_command = \"export LD_LIBRARY_PATH=${eblearnbin} && ${eblearnbin}/objtrain\"" >> $metaconf
  echo "retrain = 1" >> $metaconf
  echo "retrain_weights = ${bestweights}" >> $metaconf
  echo "meta_name = ${meta_name}_retraining_${iter}" >> $metaconf
# send report at specific training iterations
  echo "meta_email_iters = 0,1,2,3,4,5,7,10,15,20,30,50,75,100,200" >> $bestconf
  ${eblearnbin}/metarun $metaconf -tstamp ${tstamp}
    
done

###############################################################################
# training
###############################################################################

# initial training
echo "________________________________________________________________________"
echo "initial training from metaconf: ${metaconf}"
echo "meta_command = \"export LD_LIBRARY_PATH=${eblearnbin} && ${eblearnbin}/objtrain\"" >> $metaconf
echo "meta_name = ${meta_name}" >> $metaconf
#${eblearnbin}/metarun $metaconf -tstamp ${tstamp}
touch $out/${tstamp}.${meta_name}_retraining_2

# looping on retraining on false positives
echo "________________________________________________________________________"
echo "retraining loop"
for iter in `seq 3 ${maxiteration}`
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

  if [ $? -neq 0 ]; then exit -1; fi # stop if error
extract_falsepos(bestconf, save_max, save_max_per_frame, 
    bestout=$4 # directory of trained weights
    input_max=$5
    threshold=$6
    weights=$7
    negatives_root=$8 # directory where to find negative images
    eblearnbin=$9
    nthreads=$10
    npasses=$11
    max_scale=$12
    tstamp=$13
    iter=$14
    falsepos_dir=$15
