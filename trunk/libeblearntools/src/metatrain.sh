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
maxiteration=3
npasses=3
max_scale=4
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
negatives_root=$root/train/bg_full/
input_max=1000
# variables

metaconf0=$ebl/demos/pedestrians/daimler_det/${metaconf_name}
metaconf=${out}/${metaconf_name}

precision=float

# threshold will be decremented at each iter until -.95
threshold=-.5

# split ratio of validation over training
ds_split_ratio=".1"
draws=3

###############################################################################
# functions
###############################################################################

# stop if error
check_error() {
    if [ $1 -ne 0 ]
    then
	echo "Error, exiting."
	exit -1
    fi
}

# extract false positives
extract_falsepos() {
    # arguments ###############################################################
    bestconf=$1
    save_max=$2
    save_max_per_frame=$3
    bestout=$4 # directory of trained weights
    input_max=$5
    threshold=$6
    weights=$7
    negatives_root=$8 # directory where to find negative images
    eblearnbin=$9
    nthreads=${10}
    npasses=${11}
    max_scale=${12}
    tstamp=${13}
    name=${14}
    echo
    echo -e "Arguments:\nbestconf=${bestconf}\nsave_max=${save_max}"
    echo -e "save_max_per_frame=${save_max_per_frame}\nbestout=${bestout}\ninput_max=${input_max}"
    echo -e "threshold=${threshold}\nweights=${weights}\nnegatives_root=${negatives_root}"
    echo -e "eblearnbin=${eblearnbin}\nnthreads=${nthreads}\nnpasses=${npasses}"
    echo -e "max_scale=${max_scale}\ntstamp=${tstamp}\nname=${name}"
    echo
    # function body ###########################################################

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
    echo "input_max = ${input_max}" >> $bestconf
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
    echo "meta_name = ${name}" >> $bestconf
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
    check_error $? 
}

# add new data to existing sets
compile_data() {
    # arguments ###############################################################
    eblearnbin=$1
    falsepos_dir=$2
    precision=$3
    dataroot=$4
    h=$5
    w=$6
    chans=$7
    draws=$8
    traindsname=$9
    valdsname=${10}
    ds_split_ratio=${11}
    # function body ###########################################################

    # recompile data from last output directory which should contain 
    # all false positives
    # note: no need to preprocess, .mat should already be preprocessed
    ${eblearnbin}/dscompiler ${falsepos_dir} -precision ${precision} \
	-outdir ${dataroot} -forcelabel bg -dname allfp \
	-dims ${h}x${w}x${chans} \
	-image_pattern ".*[.]mat" -mindims ${h}x${w}x${chans}
    if [ $? -neq 0 ]; then exit -1; fi # stop if error

    # get dataset size
    dssize=`${eblearnbin}/dsdisplay ${dataroot}/allfp -size`
    echo "false_positives = ${dssize}"
    valsize=`echo "(${dssize} * ${ds_split_ratio})/1" | bc`
    echo "valsize = ${valsize}"
    
    # print out information about extracted dataset to check it is ok
    ${eblearnbin}/dsdisplay $dataroot/allfp -info
    check_error $? 
    
    # split dataset into training and validation
    ${eblearnbin}/dssplit ${dataroot} allfp \
	allfp_val_ allfp_train_ -maxperclass ${valsize} -draws $draws
    check_error $? 

    # merge new datasets into previous datasets: training
    for i in `seq 1 $draws`
    do
	${eblearnbin}/dsmerge ${dataroot} ${traindsname}_${i} \
	    allfp_train_${i} ${traindsname}_${i}
	check_error $? 
    done

    # merge new datasets into previous datasets: validation
    for i in `seq 1 $draws`
    do
	${eblearnbin}/dsmerge ${dataroot} ${valdsname}_${i} \
	    allfp_val_${i} ${valdsname}_${i}
	check_error $? 
    done
}

# retrain a trained network
retrain() {
    # arguments ###############################################################
    bestweights=$1
    eblearnbin=$2
    metaconf=$3
    name=$4
    tstamp=$5
    bestconf=$6
    # function body ###########################################################
    # retrain on old + new data
    echo "Retraining from best previous weights: ${bestweights}"
    # add last weights and activate retraining from those
    echo "meta_command = \"export LD_LIBRARY_PATH=${eblearnbin} && ${eblearnbin}/objtrain\"" >> $metaconf
    echo "retrain = 1" >> $metaconf
    echo "retrain_weights = ${bestweights}" >> $metaconf
    echo "meta_name = ${name}" >> $metaconf
    # send report at specific training iterations
    echo "meta_email_iters = 0,1,2,3,4,5,7,10,15,20,30,50,75,100,200" >> \
	$bestconf
    ${eblearnbin}/metarun $metaconf -tstamp ${tstamp}
    check_error $? 
}

print_step() {
    # arguments ###############################################################
    step=$1
    metaconf=$2
    lastname=$3
    lastdir=$4
    type=$5
    iter=$6
    maxiteration=$7
    # function body ###########################################################
    echo "_________________________________________________________________"
    echo "step ${step}: ${type} with metaconf ${metaconf}"
    echo "lastname: ${lastname}"
    echo "lastdir: ${lastdir}"
    echo "maxiteration: ${maxiteration} iter: ${iter}"
    echo "i=`expr ${maxiteration} - ${iter}`"
}   

metatrain() {
    # arguments ###############################################################
    minstep=$1
    maxiteration=$2
    out=$3
    eblearnbin=$4
    negatives_root=$5
    metaconf=$6
    metaconf0=$7
    meta_name=$8
    tstamp=$9
    save_max=${10}
    save_max_per_frame=${11}
    input_max=${12}
    threshold=${13}
    nthreads=${14}
    npasses=${15}
    max_scale=${16}
    precision=${17}
    dataroot=${18}
    h=${19}
    w=${20}
    chans=${21}
    draws=${22}
    traindsname=${23}
    valdsname=${24}
    ds_split_ratio=${25}
    echo
    echo -e "Arguments:\nminstep=${minstep}\nmaxiteration=${maxiteration}\nout=${out}"
    echo -e "eblearnbin=${eblearnbin}\nnegatives_root=${negatives_root}\nmetaconf=${metaconf}"
    echo -e "metaconf0=${metaconf0}\nmeta_name=${meta_name}\ntstamp=${tstamp}"
    echo -e "save_max=${save_max}\nsave_max_per_frame=${save_max_per_frame}"
    echo -e "input_max=${input_max}\nthreshold=${threshold}\nnthreads=${nthreads}"
    echo -e "npasses=${npasses}\nmax_scale=${max_scale}\nprecision=${precision}"
    echo -e "dataroot=${dataroot}\nh=${h}\nw=${w}\nchans=${chans}\ndraws=${draws}"
    echo -e "traindsname=${traindsname}\nvaldsname=${valdsname}"
    echo -e "ds_split_ratio=${ds_split_ratio}"
    echo
    # function body ###########################################################
    
    # create directories
    mkdir -p $out
    mkdir -p $eblearnbin
    mkdir -p $negatives_root

    # copy binaries
    cp $eblearnbin0/* $eblearnbin/
    # set path to libraries locally
    export LD_LIBRARY_PATH=${eblearnbin}

    # make a copy of meta conf and override its output dir
    cp $metaconf0 $metaconf
    echo "meta_output_dir = ${out}" >> $metaconf

    step=0
    # initial training
    lastname=${tstamp}.${meta_name}_${step}_training
    lastdir=${out}/${lastname}
    if [ $step -ge $minstep ]; then
	print_step $step $metaconf $lastname $lastdir "training" \
	    0 $maxiteration
	echo -n "meta_command = \"export LD_LIBRARY_PATH=" >> $metaconf
	echo "${eblearnbin} && ${eblearnbin}/objtrain\"" >> $metaconf
	echo "meta_name = ${meta_name}" >> $metaconf
	${eblearnbin}/metarun $metaconf -tstamp ${tstamp}
    fi
    step=`expr ${step} + 1` # increment step
    
    # looping on retraining on false positives
    for iter in `seq 1 ${maxiteration}`
      do	
        echo "_________________________________________________________________"
        echo "iteration ${iter}"
        # find path to latest metarun output: get directory with latest date
	bestout=${lastdir}/best/01/
        # find path to best conf (there should be only 1 conf per folder)
	bestconf=`ls ${bestout}/*.conf`
        # find path to best weights (there should be only 1 weights per folder)
	bestweights=`ls ${bestout}/*_net*.mat`

        # false positives
	name=${meta_name}_${step}_falsepos
	lastname=${tstamp}.${name}
	lastdir=${out}/${lastname}
	if [ $step -ge $minstep ]; then
	    print_step $step $bestconf $lastname $lastdir "false positives" \
		$iter $maxiteration
	    extract_falsepos $bestconf $save_max $save_max_per_frame $bestout \
		$input_max $threshold $bestweights $negatives_root $eblearnbin \
		$nthreads $npasses $max_scale $tstamp $name
	fi
	step=`expr ${step} + 1` # increment step
	
        # recompile data
	if [ $step -ge $minstep ]; then
	    print_step $step $metaconf $lastname $lastdir "data compilation" \
		$iter $maxiteration
	    compile_data $eblearnbin $lastdir $precision $dataroot \
		$h $w $chans $draws $traindsname $valdsname $ds_split_ratio
	fi
	step=`expr ${step} + 1` # increment step
	
        # retrain
	name=${meta_name}_${step}_retraining
	lastname=${tstamp}.${name}
	lastdir=${out}/${lastname}
	if [ $step -ge $minstep ]; then
	    print_step $step $metaconf $lastname $lastdir "retraining" \
		$iter $maxiteration
	    retrain $bestweights $eblearnbin $metaconf $name $tstamp \
		$bestconf
	fi
	step=`expr ${step} + 1` # increment step
    done
}

###############################################################################
# training
###############################################################################

# metatrain 4 $maxiteration $out $eblearnbin $negatives_root $metaconf $metaconf0 \
#     $meta_name $tstamp $save_max $save_max_per_frame $input_max $threshold \
#     $nthreads $npasses $max_scale $precision $dataroot $h $w $chans $draws \
#     $traindsname $valdsname $ds_split_ratio
