#!/bin/sh

vgdir=~/visiongrader/
inria=/data/pedestrians/INRIAPerson/Test/annotations/
#inria=${hostname}adata/pedestrians/test/inria/

# generate curves for inria dataset using visiongrader project
# $1 should be the bounding box output file
python $vgdir/main.py --input $1 --input_parser eblearn \
    --groundtruth $inria  --groundtruth_parser inria \
    --comparator overlap50percent --det --saving-file inria.curve \
    --confidence_min -5 --confidence_max 5

# plot
python $vgdir/plotpickle.py --main_curve inria.curve \
    --xmin 0.003 --xmax 102 --ymin .03 --ymax 1.1 *.curve \
    --xlegend "False positives per image" --ylegend "Miss rate"