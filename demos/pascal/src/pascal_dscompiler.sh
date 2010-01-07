#!/bin/sh

# pascal dataset compilation
ROOT=/data/pascal/VOCdevkit/voc2009/
NAME=all09

~/eblearn/bin/dscompiler $ROOT -type pascal -precision float -outdir ${ROOT}/ds/ -channels YpUV -ignore_difficult -dname ${NAME} -maxperclass 5 -resize bilinear -disp
