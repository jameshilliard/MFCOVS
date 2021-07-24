# coding=UTF-8
from utils import *
import numpy as np
import cv2,time,math
'''
def OVS_GetROIImage(imgs,w,h):
    img_p,img=ImageProcess(imgs,w,h).apply()
    bw = img_p
    angle = -90
    M = cv2.getRotationMatrix2D((480 / 2, 480 / 2), angle, 1)
    img = cv2.warpAffine(img, M, (img.shape[0], img.shape[1]))
    N = cv2.getRotationMatrix2D((480 / 2, 480 / 2), angle, 1)
    bw = cv2.warpAffine(bw, N, (bw.shape[0], bw.shape[1]))

    # #### 找中心得 中心坐标（x，y）
    bw = bw.astype('uint8')
    distance = cv2.distanceTransform(bw, cv2.DIST_L2, 5, cv2.CV_32F)
    maxdist = 0
    # rows,cols = img.shape
    for i in range(distance.shape[0]):
        for j in range(distance.shape[1]):
            dist = distance[i][j]
            if maxdist < dist:
                xc = j
                yc = i
                maxdist = dist
    if maxdist > 0.1 * distance.shape[0]:
        pass
    else:
        #最大内切圆半径过小认为获取ROI区域失败
        return -101
    # ####

    # #### 找出轮廓形状，以坐标形式，取最长得轮廓以保证是最外面得那个,并绘制出轮廓。cv2.CHAIN_APPROX_SIMPLE
    contours, hierarchy = cv2.findContours(bw, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    contours = sorted(contours, key=len)
    cnt = contours[-1]
    #康托尔轮廓过小认为没有发现手掌
    if len(cnt)<0.04*480*480:
        return -100
    shapeF = cnt
    # ####
    print(shapeF[0][0][1])
    # #### regionLine矩阵前两位为坐标。第三位为到中心的距离.
    l = len(shapeF)
    regionLine = np.zeros((l, 3))
    distPeaks = np.zeros(l)
    for i in range(l):
        regionLine[i][0] = shapeF[i][0][0]
        regionLine[i][1] = shapeF[i][0][1]
        d = (shapeF[i][0][0] - xc) ** 2 + (shapeF[i][0][1] - yc) ** 2
        regionLine[i][2] = np.sqrt(d)
        distPeaks[i] = regionLine[i][2]
    # print(distPeaks)
    # #####

    # #### 平滑一下距离，低通滤波一下

    # ####
    distPeaks = list(distPeaks)
    # plt.show()
    # #### 查找峰值函数
    distPeaks = [-x for x in distPeaks]
    print(distPeaks)
    distPeaks = [x + 500 for x in distPeaks]
    print(distPeaks)
    ind_peaks, properties = signal.find_peaks(distPeaks, distance=40, height=15, width=25)
    print(ind_peaks)
    ind_peaks = list(ind_peaks)
    # ####
    ind = 0
    print('###############')
    # #### 峰值坐标
    coord = np.zeros((len(regionLine), 3))
    for i in range(len(ind_peaks)):
        coord[i][0] = regionLine[ind_peaks[i]][0]
        coord[i][1] = regionLine[ind_peaks[i]][1]
        cv2.circle(img, (int(coord[i][0]), int(coord[i][1])), 3, [0, 255, 0], -1)
        print((int(coord[i][0]), int(coord[i][1])))
    cv2.imshow('coords', img)
    # cv2.imwrite('333.png', img)
    print(ind_peaks)
    # ####
    n = len(ind_peaks)
    coordf = coord[0:len(ind_peaks), 0:2]
    for x in range(n - 1):
        for y in range(x + 1, n):
            if coordf[x][1] > coordf[y][1]:
                t0 = coordf[x][0]
                t1 = coordf[x][1]
                coordf[x][0] = coordf[y][0]
                coordf[x][1] = coordf[y][1]
                coordf[y][0] = t0
                coordf[y][1] = t1
                # coordf[x], coordf[y] = coordf[y], coordf[x]

    # cv2.circle() this line
    half_slide = maxdist * math.cos(math.pi / 4)
    (left, right, top, bottom) = ((xc - half_slide), (xc + half_slide), (yc - half_slide), (yc + half_slide))
    p1 = (int(left), int(top))
    p2 = (int(right), int(bottom))
    dstimg = img[int(top):int(bottom), int(left):int(right)]
    dstimg=cv2.resize(dstimg,(128,128))
    return dstimg
'''
palm=PalmVein()
def OVS_GetROIInfo(imgs):
    mask=ImageProcess(imgs).apply()
    imgs=cv2.cvtColor(ImageProcess(imgs).img,cv2.COLOR_BGR2GRAY)
    #划分手掌区域
    final=cv2.bitwise_and(imgs,imgs,mask=mask)
    _,thresh=cv2.threshold(final,10,255,cv2.THRESH_BINARY)
    #分割手掌轮廓
    contours, hierarchy = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    contours = sorted(contours, key=len)
    cnt = contours[-1]
    cx,cy=find_center(cnt)
    #如果亮度过低，返回-102
    if np.mean(final) <30:
        exit(-102)
    #如果亮度过高，返回-103
    elif np.mean(final)>70:
        exit(-103)
    score=cv2.Laplacian(imgs,cv2.CV_64F).var()
    return np.mean(final),(cx,cy),score
def OVS_GetROIImage(img1):
    roi=palm.get_roi(img1)
    return roi
def OVS_GetFeature(image,w,h):
    assert w==128 and h==128,'unvalid roi shape'
    _,template=palm.return_code_and_img(image)
    return template
def OVS_MatchVerify(code_acquired,code_given):
    assert len(code_acquired)==len(code_given),'Different code length'
    return cmpHash(code_acquired,code_acquired)
def OVS_MatchIdentify(code_acquired,db,level,timeout):
    temp_max=0
    temp_id='No match'
    start=time.time()
    for key in db.keys():
        db_code=db.get(key)
        for code in db_code:
            if cmpHash(code,code_acquired)>temp_max and cmpHash(code,code_acquired)>=level:
                temp_max=cmpHash(code,code_acquired)
                temp_id=key
            if time.time()-start>timeout:
                raise TimeoutError
        return temp_id
def OVS_CreateTemplate(roi_data):
    assert roi_data.shape==(128,128)
    code,_=palm.return_code_and_img(roi_data)
    return code,len(code)



if __name__=='__main__':
    palm = PalmVein()
    imgs=cv2.imread(r'a.png')
    a=OVS_GetROIImage(imgs)
    cv2.imshow('a',a)
    cv2.waitKey(0)
