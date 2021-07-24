import cv2
import argparse as arg
import numpy as np
def readimage(dir):
    img=cv2.imread(dir)
    img_gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    img_gray=img_gray[:,420:1500]
    kernel1 = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]])
    laplace = cv2.filter2D(img_gray, -1, kernel1)
    gaussian = cv2.GaussianBlur(laplace, (5, 5), 0)
    _, thresh = cv2.threshold(gaussian, 0, 255, cv2.THRESH_OTSU)
    structure = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))
    morph = cv2.morphologyEx(thresh, cv2.MORPH_ELLIPSE, structure)
    def non_zero_mean(np_arr):
        exist = (np_arr != 0)
        num = np_arr.sum(axis=0)
        den = exist.sum(axis=0)
        return num / den
    res=cv2.bitwise_and(img_gray,img_gray,mask=morph)
    avg=[]
    for row in res:
        avg.append(non_zero_mean(row))
    #print('global:',np.average(img_gray))
    return(np.nanmean(avg))

def test():
    return 11;



#if __name__=='__main__':
#    parser=arg.ArgumentParser()
#    parser.add_argument('-d','--dir',help='输入图片地址')
#    args=parser.parse_args()
#    _main(args.dir)

def readpython(dir):
    img=cv2.imread(dir)
    img_gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    img_gray=img_gray[:,420:1500]
    kernel1 = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]])
    laplace = cv2.filter2D(img_gray, -1, kernel1)
    gaussian = cv2.GaussianBlur(laplace, (5, 5), 0)
    _, thresh = cv2.threshold(gaussian, 0, 255, cv2.THRESH_OTSU)
    structure = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))
    morph = cv2.morphologyEx(thresh, cv2.MORPH_ELLIPSE, structure)
    def non_zero_mean(np_arr):
        exist = (np_arr != 0)
        num = np_arr.sum(axis=0)
        den = exist.sum(axis=0)
        return num / den
    res=cv2.bitwise_and(img_gray,img_gray,mask=morph)
    avg=[]
    for row in res:
        avg.append(non_zero_mean(row))
    print('global:',np.average(img_gray))
    #print('palm:',np.nanmean(avg))
    return(np.nanmean(avg))
    #cv2.imshow('1',final)
    #cv2.waitKey(0)
