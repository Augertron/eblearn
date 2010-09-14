#!/bin/sh

vgdir=~/visiongrader/
inria=/data/pedestrians/inria/INRIAPerson/Test/annotations/
#inria=${hostname}adata/pedestrians/test/inria/
#inria=/data/pedestrians/inria_caltech/annotations/set01/V000.vbb
#gtparser=caltech #inria
gtparser=inria

# # generate curves for inria dataset using visiongrader project
# # $1 should be the bounding box output file
# python $vgdir/main.py --input $1 --input_parser eblearn \
#     --groundtruth $inria  --groundtruth_parser $gtparser \
#     --comparator overlap50percent --det --saving-file $1.curve \
# #    --confidence_min -5 # --confidence_max 5

# # generate curves for inria dataset using visiongrader project
# # $1 should be the bounding box output file
# python $vgdir/main.py --input $1 --input_parser eblearn \
#     --groundtruth $inria  --groundtruth_parser $gtparser \
#     --comparator overlap50percent --det --saving-file $1.curve \
#     --confidence_min -5 # --confidence_max 5

# plot
python $vgdir/plotpickle.py --main_curve $1.curve \
    --xmin 0.003 --xmax 102 --ymin .03 --ymax 1.1 my_bbox/*.curve  \
    --xlegend "False positives per image" --ylegend "Miss rate"

# show db
python $vgdir/main.py --input $1 --input_parser eblearn \
    --groundtruth $inria --disp --groundtruth_parser inria \
    --images_path $inria
