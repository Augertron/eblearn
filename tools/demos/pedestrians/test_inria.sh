#!/bin/sh

# expected arguments:
newcurve=$1 # the eblearn bbox output file
dir=$2 # the directory containing other curves (pickle files)

vgdir=~/visiongrader/
inria=/data/pedestrians/inria/INRIAPerson/Test/annotations/
#inria=/data/pedestrians/INRIAPerson/Test/annotations/
#inria=${hostname}adata/pedestrians/test/inria/
#inria=/data/pedestrians/inria_caltech/annotations/set01/V000.vbb
#gtparser=caltech #inria
gtparser=inria

# approximation of curve to avoid computing all possible thresholds
sampling="--sampling 50"
xmin=0.003
xmax=102
ymin=0.03
ymax=1.1
gt_whratio="--gt_whratio .43"
#hratio="--hratio 1.145"
#wratio="--wratio 1.055"
#whratio="--whratio .43"
#hratio="--hratio 1.01"
#whratio="--whratio .43"
comp_threshold=.5
extract_caltech=0
show_caltech_db=0
 
################################################################################
# generate caltech algos curves

caltech=$vgdir/res/
test -d $caltech
if [ $? -eq 0 ] ; then
if [ $extract_caltech -eq 1 ] ; then
    for i in `ls -d $caltech/*/`; do
	algodir=$i/set01/V000/
	algo=`basename $i`
	echo "_________________________________________________________________"
	echo "Processing algorithm $algo"
	python $vgdir/main.py --input $algodir --input_parser caltech \
    	    --groundtruth $inria  --groundtruth_parser $gtparser $gt_whratio \
    	    $sampling --xmin $xmin --xmax $xmax --ymin $ymin --ymax $ymax \
    	    --comparator overlap50percent \
	    --comparator_param ${comp_threshold} \
	    --det --saving-file $dir/$algo.pickle \
    	    --show-no-curve --confidence_min -5
	if [ $? -ne 0 ] ; then
	    echo "Error, stopping."
	    exit -1
	fi
	if [ $show_caltech_db -eq 1 ] ; then
            # show db
	    echo "_____________________________________________________________"
	    echo "Showing images for algo ${algodir}"
	    python $vgdir/main.py --input $algodir --input_parser caltech \
		--groundtruth $inria --groundtruth_parser $gtparser --disp \
		$gt_whratio --images_path $inria \
		--comparator overlap50percent \
		--comparator_param ${comp_threshold}
	fi
    done
fi
fi

################################################################################

# generate curves for inria dataset using visiongrader project
# $1 should be the bounding box output file
python $vgdir/main.py --input $newcurve --input_parser eblearn \
    --groundtruth $inria  --groundtruth_parser $gtparser \
    $gt_whratio $whratio $hratio $wratio \
    $sampling --xmin $xmin --xmax $xmax --ymin $ymin --ymax $ymax \
    --comparator overlap50percent --comparator_param ${comp_threshold} \
    --det --saving-file $newcurve.pickle \
    --show-no-curve

# plot
python $vgdir/plotpickle.py --main_curve $newcurve.pickle \
    --xmin $xmin --xmax $xmax --ymin $ymin --ymax $ymax $dir/*.pickle  \
    --xlegend "False positives per image" --ylegend "Miss rate" \
    --grid_major --grid_minor

# show db
python $vgdir/main.py --input $newcurve --input_parser eblearn --disp \
    --groundtruth $inria --groundtruth_parser inria \
    $gt_whratio $whratio $hratio $wratio \
    --images_path $inria \
    --comparator overlap50percent --comparator_param ${comp_threshold}
    

################################################################################