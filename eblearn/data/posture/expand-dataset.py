
import os
import Image
 
imgsdict = {}

dirpath = 'train/stand'
for imgname in os.listdir(dirpath):
    #if not imgname[0].isdigit(): continue
    imgpath = os.path.join(dirpath, imgname)
    img = Image.open(imgpath)
    imgsdict[imgname] = img
    print img.format, img.size, img.mode
    img.show()

    

