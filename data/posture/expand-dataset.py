#!/usr/bin/python

import os
import Image
import re

def expandDataSet(dirpath):
    'Expand a set of images by rotating/scaling them'
    nbOfImgsGen = 0
    for imgclass in os.listdir(dirpath):
        if (imgclass[0] == '.'): continue
        print 'Expanding each file in ' + imgclass + ' by rotating/scaling it' 
        for imgname in os.listdir(os.path.join(dirpath, imgclass)):
            if (imgname[0] == '.'): continue
            imgpath = os.path.join(dirpath, imgclass, imgname)
            img = Image.open(imgpath)
            for rot in [5,10,15,-5,-10,-15]:
                out = img.rotate(rot)
                imgoutpath = os.path.splitext(imgpath)[0] + '_rot_'\
                    + str(rot) + '.ppm' 
                out.save(imgoutpath)
                nbOfImgsGen += 1
            for scale in [0.97, 0.94, 0.91, 0.87]:
                src = (0, 0, scale*img.size[0], scale*img.size[1])
                out = img.transform(img.size, Image.EXTENT, src, Image.BICUBIC)
                imgoutpath = os.path.splitext(imgpath)[0] + '_scale_'\
                    + str(scale) + '.ppm' 
                out.save(imgoutpath)
                nbOfImgsGen += 1
    print 'Generated ' + str(nbOfImgsGen) + ' new images !'

def cleanDataSet(dirpath):
    'Clean all generated images'
    genImgRegEx = re.compile('(.*_rot_.*)|(.*_scale_.*)')
    print 'Cleaning all previously generated images'
    for imgclass in os.listdir(dirpath):
        if (imgclass[0] == '.'): continue
        for imgname in os.listdir(os.path.join(dirpath, imgclass)):
            if (imgname[0] == '.'): continue
            imgpath = os.path.join(dirpath, imgclass, imgname)
            if genImgRegEx.match(imgname):
                os.remove(imgpath)

def makeBackgroundClass(dirpath):
    'Make a new dir containing background images'
    print 'Adding a new class containing backgrounds'
    iter = 0
    imgBlack = Image.new('RGB', (46,46))
    for imgclass in os.listdir(dirpath):
        if (imgclass[0] == '.'): continue
        for imgname in os.listdir(os.path.join(dirpath, imgclass)):
            "create one background image for each image in this class"
            if (imgname[0] == '.'): continue
            imgpath = os.path.join(dirpath, 'empty', 'empty') \
                + str(iter) + '.ppm'
            iter = iter + 1
            imgBlack.save(imgpath)
        return

if __name__ == '__main__':
    import sys
    print 'Syntax:\n expand-dataset.py -> generate data'
    print ' expand-dataset.py clean -> clean directory\n'
    if len(sys.argv) == 1:
        cleanDataSet('train')
        expandDataSet('train')
    elif sys.argv[1] == 'clean':
        cleanDataSet('train')
    elif sys.argv[1] == 'bgnd':
        makeBackgroundClass('train')
        makeBackgroundClass('test')

