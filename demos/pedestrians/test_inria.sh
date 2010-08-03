#!/bin/sh

# generate curves for inria dataset using visiongrader project
# $1 should be the bounding box output file
python ~/visiongrader/main.py --input $1 --input_parser eblearn --groundtruth ${hostname}adata/pedestrians/test/inria/ --groundtruth_parser INRIA --comparator overlap50percent --det
