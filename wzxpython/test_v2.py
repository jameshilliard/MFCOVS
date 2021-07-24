import numpy as np
import cv2 as cv
from nms import py_cpu_nms
from imutils.object_detection import non_max_suppression

import glob,os,types
import time
hog = cv.HOGDescriptor((64, 64), (16, 16), (4, 4), (8, 8), 9)
hog.load('myHogDector1.bin')



# **** 视频测试 begin

'''


# cap=cv.VideoCapture(r'E:\work1002_svdd\svm-wangzhixin\zls6.mp4')
cap=cv.VideoCapture(r'E:\work1021_palm\testvideo\zhangjunqiang1.avi')
while 1:
    ret,img=cap.read()
    img=cv.cvtColor(img,cv.COLOR_BGR2GRAY)
    img=cv.resize(img[:,420:1500],(100,100))
    #M=cv.getRotationMatrix2D((540/2, 540/2),-90,1)
    #img=cv.warpAffine(img, M, (img.shape[0], img.shape[1]))
    cv.imshow('src', img)
    #cv.waitKey(0)

    rects, scores = hog.detectMultiScale(img, winStride=(4, 4), padding=(0, 0), scale=1.1)
    sc = [score[0] for score in scores]
    sc = np.array(sc)

    # 转换下输出格式(x,y,w,h) -> (x1,y1,x2,y2)
    for i in range(len(rects)):
        r = rects[i]
        rects[i][2] = r[0] + r[2]
        rects[i][3] = r[1] + r[3]
    #print(rects.shape)
    feature_params = dict( maxCorners = 4,
                           qualityLevel = 0.3,
                           minDistance = 7,
                           blockSize = 7 )
    points = cv.goodFeaturesToTrack(img, mask = None, **feature_params)
    pick = []

    # 非极大值移植
    #print('rects_len', len(rects))
    pick = non_max_suppression(rects,
                               probs=sc,
                               overlapThresh=0.1)
    #print('pick_len = ', len(pick))
    img_gray = img.copy()
    img = cv.cvtColor(img, cv.COLOR_GRAY2BGR)
    print(pick)
    # 画出矩形框

    # for pt in pick:
    #     x, y = pt.ravel()
    #     # cv.circle(img, (x, y), 3, 255, -1)
    #     cv.rectangle(img, (x, y), (xx, yy), (0, 0, 255), 1)

    for (x, y, xx, yy) in pick:
        cv.circle(img, ((x+xx)//2, (y+yy)//2),2,(0, 0, 255),-1)
        cv.rectangle(img, (x, y), (xx, yy), (0, 0, 255), 1)

    cv.imshow('a', img)
    cv.waitKey(1)

'''

# **** 多张图片测试 begin

count =0
count1=0
pictures = []
# foldername = 'E:/work1002_svdd/feature-detection-master/orb_zls/nopalm'
# foldername = 'E:/work1002_svdd/svm-wangzhixin/bmp'
# 输入文件夹路径
foldername = 'E:/work1021_palm/roi_false'
# foldername = 'E:\work1002_svdd\sample'
filenames = glob.iglob(os.path.join(foldername, '*.bmp'))
for filename in filenames:
    img = cv.imread(filename)
    count = count + 1
    print('count:', count)
    print(filename)
    img = cv.resize(img[:, 420:1500], (100, 100))
    rects, scores = hog.detectMultiScale(img, winStride=(4, 4), padding=(0, 0), scale=1.05)
    if np.array(rects).size != 0:
        print(filename)
        print(rects)
    else:
        print('No Palm')
        pictures.append(filename)
        count1=count1+1
print('no palm:',count1)
print(pictures)
# **** 多张图片测试 end


# **** 单张测试 begin
'''


# img = cv.imread(r'E:\work1002_svdd\feature-detection-master\orb_zls\nopalm\41.jpg')
img = cv.imread(r'E:\work1002_svdd\sample\4_R_13969721290_1_27_0_5_.bmp')
# img = cv.imread(r'D:\projects\palm-detection\scripts\0\1873_R_13708942163_1_1E_0_3_.bmp',0)
time1 = time.time()
img=cv.resize(img[:,420:1500],(100,100))
# cv.imshow('src', img)
#cv.waitKey(0)

# 多尺度目标检测（检测输入图像中不同大小的目标）
rects, scores = hog.detectMultiScale(img, winStride=(4, 4), padding=(0, 0), scale=1.05)
print('rects')
print(rects)
time2 = time.time()
print('time:', + time2-time1)
print('scores')
print(scores)

sc = [score[0] for score in scores]
sc = np.array(sc)

# 转换下输出格式(x,y,w,h) -> (x1,y1,x2,y2)
for i in range(len(rects)):
    r = rects[i]
    rects[i][2] = r[0] + r[2]
    rects[i][3] = r[1] + r[3]

pick = []
# 非极大值移植
print('rects_len', len(rects))
pick = non_max_suppression(rects, probs=sc, overlapThresh=0.3)
print('pick_len = ', len(pick))

# 画出矩形框
for (x, y, xx, yy) in pick:
    #if xx-x <=100:
    # cv.circle(img, (x+(xx-x)//2, y+(yy-y)//2),2,(0, 0, 255),-1)
    cv.rectangle(img, (x,y), (xx, yy), (0,0,255), 1)
    cv.imwrite('palm.png', img)

cv.imshow('a', img)
cv.waitKey(0)
'''
# **** 单张测试 end




