# coding=UTF-8

import cv2
import math
import numpy as np
from scipy import signal
#图像预处理类
class ImageProcess:
    def __init__(self,img,w=1080,h=1920):
        self.img=img
        h1,h2=h//2-w//2,h//2+w//2
        self.img=self.img[:,h1:h2]
        self.img = cv2.resize(self.img, (540, 540),interpolation=cv2.INTER_CUBIC)
        self.img_gray=cv2.cvtColor(self.img,cv2.COLOR_BGR2GRAY)
        self.img=np.array(self.img,dtype=np.uint8)
    def apply(self):
        temp = self.img_gray.copy()
        kernel1 = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]])
        laplace = cv2.filter2D(temp, -1, kernel1)
        gaussian = cv2.GaussianBlur(laplace, (5, 5), 0)
        _, thresh = cv2.threshold(gaussian, 0, 255, cv2.THRESH_OTSU)
        structure = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))
        morph = cv2.morphologyEx(thresh, cv2.MORPH_ELLIPSE, structure)
        return morph
#字符串相似度
def cmpHash(hash1,hash2):
    n=0
    #hash长度不同则返回-1代表传参出错
    if len(hash1)!=len(hash2):
        return -1
    #遍历判断
    for i in range(len(hash1)):
        #不相等则n计数+1，n最终为相似度
        if hash1[i]!=hash2[i]:
            n=n+1
    return 1-(n/len(hash1))
#寻找康托尔集重心
def find_center(contour):
    M = cv2.moments(contour)
    cx = int(M['m10'] / M['m00'])  # 求x坐标
    cy = int(M['m01'] / M['m00'])  # 求y坐标
    return (cx,cy)
#获取ROI区域和提取特征并编码类
'''
def get_roi(img):
    #图像预处理
    img = img[:, 420:1500]
    #reshape图像到指定大小
    img = cv2.resize(img, (540, 540))
    #转为灰度图像
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img = np.array(img, dtype=np.uint8)
    temp = img_gray.copy()
    #建立拉普拉斯算子
    kernel1 = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]])
    #拉普拉斯滤波
    laplace = cv2.filter2D(temp, -1, kernel1)
    #高斯滤波去噪
    gaussian = cv2.GaussianBlur(laplace, (5, 5), 0)
    #分割手掌区域
    _, thresh = cv2.threshold(gaussian, 0, 255, cv2.THRESH_OTSU)
    #形态学处理
    structure = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (11, 11))
    morph = cv2.morphologyEx(thresh, cv2.MORPH_CLOSE, structure)
    #顺时针旋转图像
    bw = morph
    M=cv2.getRotationMatrix2D((540/2, 540/2),-90,1)
    img=cv2.warpAffine(img, M, (img.shape[0], img.shape[1]))
    N=cv2.getRotationMatrix2D((540/2, 540/2),-90,1)
    bw=cv2.warpAffine(bw, N, (bw.shape[0], bw.shape[1]))
    #寻找最大内切圆半径和中心坐标（x，y），并据此判断手掌是否在图像中出现，若出现是否在范围内
    bw=bw.astype('uint8')
    distance = cv2.distanceTransform(bw, cv2.DIST_L2, 5, cv2.CV_32F)
    maxdist = 0
    # rows,cols = img.shape
    for i in range(distance.shape[0]):
        for j in range(distance.shape[1]):
            dist = distance[i][j]
            if maxdist < dist:
                x = j
                y = i
                maxdist = dist
    if maxdist<0.1*distance.shape[0]:
        exit(-100)
    center_x = x
    center_y = y
    if center_x <135 or center_x >405 or center_y<135 or center_y>405:
        exit(-101)
    # #### 找出轮廓形状，以坐标形式，取最长得轮廓以保证是最外面得那个,并绘制出轮廓。cv2.CHAIN_APPROX_SIMPLE
    contours, hierarchy = cv2.findContours(bw, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    contours=sorted(contours, key=len)
    cnt = contours[-1]
    print('轮廓：')
    print(cnt.shape)
    shapeF = cnt
    #绘制contour范围图
    #cv2.drawContours(img, shapeF, -1, (0, 0, 255), 3)
    #cv2.imshow('contours', img)
    # ####
    # #### regionLine矩阵前两位为坐标。第三位为到中心的距离.
    l = len(shapeF)
    regionLine = np.zeros((l, 3))
    distPeaks = np.zeros(l)
    print(regionLine.shape)
    for i in range(l):
        regionLine[i][0] = shapeF[i][0][0]
        regionLine[i][1] = shapeF[i][0][1]
        d = (shapeF[i][0][0] - x)**2 + (shapeF[i][0][1] - y)**2
        regionLine[i][2] = np.sqrt(d)
        distPeaks[i] = regionLine[i][2]
    # print(distPeaks)
    # ####

    # ####
    distPeaks = list(distPeaks)
    print('juli:')
    print(distPeaks)
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
    coord = np.zeros((len(regionLine),3))
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
    coordf = np.zeros((n, 2))
    coordf = coord[0:len(ind_peaks), 0:2]

    print('坐标：')
    print(len(coordf))
    print(coordf)

    for x in range(n-1):
        for y in range(x+1, n):
            if coordf[x][1] > coordf[y][1] :
                t0 = coordf[x][0]
                t1 = coordf[x][1]
                coordf[x][0] = coordf[y][0]
                coordf[x][1] = coordf[y][1]
                coordf[y][0] = t0
                coordf[y][1] = t1
                #coordf[x], coordf[y] = coordf[y], coordf[x]
    print('排序')
    print(coordf)
    for i in range(3):
        cv2.circle(img, (int(coordf[i][0]), int(coordf[i][1])), 3, [255, 255, 255], -1)
    cv2.imshow('key', img)

    coordx = np.zeros((3, 2))
    coordx = coordf[:3]
    print('******************')
    print(coordx)
    for x in range(2):
        for y in range(x+1, 3):
            if coordx[x][0] > coordx[y][0]:
                t0 = coordf[x][0]
                t1 = coordf[x][1]
                coordf[x][0] = coordf[y][0]
                coordf[x][1] = coordf[y][1]
                coordf[y][0] = t0
                coordf[y][1] = t1
    print('排完序：')
    print(coordx)

    d = coordx[2][0] - coordx[0][0]
    yy = (coordx[2][1] - coordx[0][1])
    xx = (coordx[2][0] - coordx[0][0])
    print('angle')
    print(yy/xx)
    angle_ = math.atan(yy/xx)
    print('angle:')
    angle = angle_*180/np.pi
    print(angle)

    c_x = (coordx[0][0] + coordx[2][0])//2
    c_y = (coordx[0][1] + coordx[2][1])//2

    # 仿射变换
    M=cv2.getRotationMatrix2D((c_x, c_y),angle,1)
    img=cv2.warpAffine(img, M, (img.shape[0], img.shape[1]))

    # -------------

    cv2.imshow('rotate', img)
    #ROI位置参数
    r = 0.4
    #输出尺寸归一化后ROI区域信息
    # (center_x, center_y)
    #画框，显示图片
    #cv2.rectangle(img, (int((c_x-d/2)), int((c_y + r*d))), (int((c_x + d/2)), int((c_y + r*d + d))), (255, 0, 0), 2)
    #cv2.imshow('point', img)
    return cv2.resize(img[int(c_y + r*d):int(c_y + r*d + d),int(c_x-d/2):int(c_x + d/2)],(128,128),interpolation=cv2.INTER_LINEAR)
    '''
'''
def get_roi(img):
    # 图像预处理
    img = img[:, 420:1500]
    # reshape图像到指定大小
    img = cv2.resize(img, (540, 540))
    # 转为灰度图像
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    img = np.array(img, dtype=np.uint8)
    temp = img_gray.copy()
    # 建立拉普拉斯算子
    kernel1 = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]])
    # 拉普拉斯滤波
    laplace = cv2.filter2D(temp, -1, kernel1)
    # 高斯滤波去噪
    gaussian = cv2.GaussianBlur(laplace, (5, 5), 0)
    # 分割手掌区域
    _, thresh = cv2.threshold(gaussian, 0, 255, cv2.THRESH_OTSU)
    # 形态学处理
    structure = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (11, 11))
    morph = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, structure)
    # 顺时针旋转图像
    bw = morph
    M = cv2.getRotationMatrix2D((540 / 2, 540 / 2), -90, 1)
    img = cv2.warpAffine(img, M, (img.shape[0], img.shape[1]))
    N = cv2.getRotationMatrix2D((540 / 2, 540 / 2), -90, 1)
    bw = cv2.warpAffine(bw, N, (bw.shape[0], bw.shape[1]))
    # 寻找最大内切圆半径和中心坐标（x，y），并据此判断手掌是否在图像中出现，若出现是否在范围内
    bw = bw.astype('uint8')
    distance = cv2.distanceTransform(bw, cv2.DIST_L2, 5, cv2.CV_32F)
    maxdist = 0
    # rows,cols = img.shape
    for i in range(distance.shape[0]):
        for j in range(distance.shape[1]):
            dist = distance[i][j]
            if maxdist < dist:
                x = j
                y = i
                maxdist = dist
    if maxdist < 0.1 * distance.shape[0]:
        # exit(-100)
        pass
    center_x = x
    center_y = y
    # 如果手掌不在中心附近
    if center_x < 100 or center_x > 450 or center_y < 100 or center_y > 450:
        # exit(-101)
        pass
    # #### 找出轮廓形状，以坐标形式，取最长得轮廓以保证是最外面得那个,并绘制出轮廓。cv2.CHAIN_APPROX_SIMPLE
    contours, hierarchy = cv2.findContours(bw, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    contours = sorted(contours, key=len)
    cnt = contours[-1]
    print('轮廓：')
    print(cnt.shape)

    shapeF = cnt
    # 绘制contour范围图
    # cv2.drawContours(img, shapeF, -1, (0, 0, 255), 3)
    # cv2.imshow('contours', img)
    # ####
    # #### regionLine矩阵前两位为坐标。第三位为到中心的距离.
    l = len(shapeF)
    regionLine = np.zeros((l, 3))
    distPeaks = np.zeros(l)
    # print(regionLine.shape)
    for i in range(l):
        regionLine[i][0] = shapeF[i][0][0]
        regionLine[i][1] = shapeF[i][0][1]
        d = (shapeF[i][0][0] - x) ** 2 + (shapeF[i][0][1] - y) ** 2
        regionLine[i][2] = np.sqrt(d)
        distPeaks[i] = regionLine[i][2]
    # print(distPeaks)
    # ####

    # ####
    distPeaks = list(distPeaks)
    # print('juli:')
    # print(distPeaks)
    # #### 查找峰值函数
    distPeaks = [-x for x in distPeaks]
    # print(distPeaks)
    distPeaks = [x + 500 for x in distPeaks]
    # print(distPeaks)
    ind_peaks, properties = signal.find_peaks(distPeaks, distance=40, height=15, width=25)
    # print(ind_peaks)
    ind_peaks = list(ind_peaks)
    # ####
    ind = 0
    # print('###############')
    # #### 峰值坐标
    coord = np.zeros((len(regionLine), 3))
    for i in range(len(ind_peaks)):
        coord[i][0] = regionLine[ind_peaks[i]][0]
        coord[i][1] = regionLine[ind_peaks[i]][1]
        # cv2.circle(img, (int(coord[i][0]), int(coord[i][1])), 3, [0, 255, 0], -1)
        # print((int(coord[i][0]), int(coord[i][1])))
    # cv2.imshow('coords', img)
    # cv2.imwrite('333.png', img)
    # print(ind_peaks)
    # ####
    n = len(ind_peaks)
    # coordf = np.zeros((n, 2))
    coordf = coord[0:len(ind_peaks), 0:2]

    # print('坐标：')
    # print(len(coordf))
    # print(coordf)

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
    # print('排序')
    # print(coordf)
    # for i in range(3):
    # cv2.circle(img, (int(coordf[i][0]), int(coordf[i][1])), 3, [255, 255, 255], -1)
    # cv2.imshow('key', img)

    # coordx = np.zeros((3, 2))
    coordx = coordf[:3]
    # print('******************')
    # print(coordx)
    for x in range(2):
        for y in range(x + 1, 3):
            if coordx[x][0] > coordx[y][0]:
                t0 = coordf[x][0]
                t1 = coordf[x][1]
                coordf[x][0] = coordf[y][0]
                coordf[x][1] = coordf[y][1]
                coordf[y][0] = t0
                coordf[y][1] = t1
    # print('排完序：')
    # print(coordx)

    d = coordx[2][0] - coordx[0][0]
    yy = (coordx[2][1] - coordx[0][1])
    xx = (coordx[2][0] - coordx[0][0])
    # print('angle')
    # print(yy/xx)
    angle_ = math.atan(yy / xx)
    # print('angle:')
    angle = angle_ * 180 / np.pi
    # print(angle)

    c_x = (coordx[0][0] + coordx[2][0]) // 2
    c_y = (coordx[0][1] + coordx[2][1]) // 2

    # 仿射变换
    M = cv2.getRotationMatrix2D((c_x, c_y), angle, 1)
    img = cv2.warpAffine(img, M, (img.shape[0], img.shape[1]))

    # -------------

    # cv2.imshow('rotate', img)
    # ROI位置参数
    r = 0.4
    # 输出尺寸归一化后ROI区域信息
    # (center_x, center_y)
    # 画框，显示图片
    # cv2.rectangle(img, (int((c_x-d/2)), int((c_y + r*d))), (int((c_x + d/2)), int((c_y + r*d + d))), (255, 0, 0), 2)
    # cv2.imshow('point', img)
    return cv2.resize(img[int(c_y + r * d):int(c_y + r * d + d), int(c_x - d / 2):int(c_x + d / 2)], (128, 128),
                      interpolation=cv2.INTER_LINEAR)
    '''
class PalmVein:
    def getNBP(self,array):
        temp_list=[array[0,0],array[0,1],array[0,2],array[1,2],array[2,2],array[2,1],array[2,0],array[1,0]]
        comp=[]
        for i in range(7):
            if temp_list[i]>temp_list[i+1]:
                comp.append('1')
            else:
                comp.append('0')
        if temp_list[-1]>array[1][1]:
            comp.append('1')
        else:
            comp.append('0')
        return int(''.join(comp),base=2),''.join(comp)
    def process_NBP(self,array):
        w,h=array.shape[:2]
        res=np.zeros((w,h),dtype='uint8')
        array_paddle = np.pad(array, ((1, 1), (1, 1)), 'constant', constant_values=(0, 0))
        wn,hn=array_paddle.shape[:2]
        res_s=''
        for ww in range(1,w+1):
            for hh in range(1,h+1):
                num,string=self.getNBP(array_paddle[ww-1:ww+2,hh-1:hh+2])
                res[ww-1,hh-1]=num
                res_s+=string
        return res,res_s
    def get_roi(self,img):
        #图像预处理
        img = img[:, 420:1500]
        #reshape图像到指定大小
        img = cv2.resize(img, (540, 540))
        #转为灰度图像
        img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        img = np.array(img, dtype=np.uint8)
        temp = img_gray.copy()
        #建立拉普拉斯算子
        kernel1 = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]])
        #拉普拉斯滤波
        laplace = cv2.filter2D(temp, -1, kernel1)
        #高斯滤波去噪
        gaussian = cv2.GaussianBlur(laplace, (5, 5), 0)
        #分割手掌区域
        _, thresh = cv2.threshold(gaussian, 0, 255, cv2.THRESH_OTSU)
        #形态学处理
        structure = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (11, 11))
        morph = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, structure)
        #顺时针旋转图像
        bw = morph
        M=cv2.getRotationMatrix2D((540/2, 540/2),-90,1)
        img=cv2.warpAffine(img, M, (img.shape[0], img.shape[1]))
        N=cv2.getRotationMatrix2D((540/2, 540/2),-90,1)
        bw=cv2.warpAffine(bw, N, (bw.shape[0], bw.shape[1]))
        #寻找最大内切圆半径和中心坐标（x，y），并据此判断手掌是否在图像中出现，若出现是否在范围内
        bw=bw.astype('uint8')
        distance = cv2.distanceTransform(bw, cv2.DIST_L2, 5, cv2.CV_32F)
        maxdist = 0
        # rows,cols = img.shape
        for i in range(distance.shape[0]):
            for j in range(distance.shape[1]):
                dist = distance[i][j]
                if maxdist < dist:
                    x = j
                    y = i
                    maxdist = dist
        if maxdist<0.1*distance.shape[0]:
            #exit(-100)
            pass
        center_x = x
        center_y = y
        #如果手掌不在中心附近，返回-101
        if center_x <100 or center_x >450 or center_y<100 or center_y>450:
            #exit(-101)
            pass
        # #### 找出轮廓形状，以坐标形式，取最长得轮廓以保证是最外面得那个,并绘制出轮廓。cv2.CHAIN_APPROX_SIMPLE
        contours, hierarchy = cv2.findContours(bw, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        contours=sorted(contours, key=len)
        cnt = contours[-1]
        '''
        print('轮廓：')
        print(cnt.shape)
        '''
        shapeF = cnt
        #绘制contour范围图
        #cv2.drawContours(img, shapeF, -1, (0, 0, 255), 3)
        #cv2.imshow('contours', img)
        # ####
        # #### regionLine矩阵前两位为坐标。第三位为到中心的距离.
        l = len(shapeF)
        regionLine = np.zeros((l, 3))
        distPeaks = np.zeros(l)
        #print(regionLine.shape)
        for i in range(l):
            regionLine[i][0] = shapeF[i][0][0]
            regionLine[i][1] = shapeF[i][0][1]
            d = (shapeF[i][0][0] - x)**2 + (shapeF[i][0][1] - y)**2
            regionLine[i][2] = np.sqrt(d)
            distPeaks[i] = regionLine[i][2]
        # print(distPeaks)
        # ####

        # ####
        distPeaks = list(distPeaks)
        #print('juli:')
        #print(distPeaks)
        # #### 查找峰值函数
        distPeaks = [-x for x in distPeaks]
        #print(distPeaks)
        distPeaks = [x + 500 for x in distPeaks]
        #print(distPeaks)
        ind_peaks, properties = signal.find_peaks(distPeaks, distance=40, height=15, width=25)
        #print(ind_peaks)
        ind_peaks = list(ind_peaks)
        # ####
        ind = 0
        #print('###############')
        # #### 峰值坐标
        coord = np.zeros((len(regionLine),3))
        for i in range(len(ind_peaks)):
            coord[i][0] = regionLine[ind_peaks[i]][0]
            coord[i][1] = regionLine[ind_peaks[i]][1]
            #cv2.circle(img, (int(coord[i][0]), int(coord[i][1])), 3, [0, 255, 0], -1)
            #print((int(coord[i][0]), int(coord[i][1])))
        #cv2.imshow('coords', img)
        # cv2.imwrite('333.png', img)
        #print(ind_peaks)
        # ####
        n = len(ind_peaks)
        #coordf = np.zeros((n, 2))
        coordf = coord[0:len(ind_peaks), 0:2]

        #print('坐标：')
        #print(len(coordf))
        #print(coordf)

        for x in range(n-1):
            for y in range(x+1, n):
                if coordf[x][1] > coordf[y][1] :
                    t0 = coordf[x][0]
                    t1 = coordf[x][1]
                    coordf[x][0] = coordf[y][0]
                    coordf[x][1] = coordf[y][1]
                    coordf[y][0] = t0
                    coordf[y][1] = t1
                    #coordf[x], coordf[y] = coordf[y], coordf[x]
       # print('排序')
        #print(coordf)
        #for i in range(3):
            #cv2.circle(img, (int(coordf[i][0]), int(coordf[i][1])), 3, [255, 255, 255], -1)
        #cv2.imshow('key', img)

        #coordx = np.zeros((3, 2))
        coordx = coordf[:3]
        #print('******************')
        #print(coordx)
        for x in range(2):
            for y in range(x+1, 3):
                if coordx[x][0] > coordx[y][0]:
                    t0 = coordf[x][0]
                    t1 = coordf[x][1]
                    coordf[x][0] = coordf[y][0]
                    coordf[x][1] = coordf[y][1]
                    coordf[y][0] = t0
                    coordf[y][1] = t1
        #print('排完序：')
        #print(coordx)

        d = coordx[2][0] - coordx[0][0]
        yy = (coordx[2][1] - coordx[0][1])
        xx = (coordx[2][0] - coordx[0][0])
        #print('angle')
        #print(yy/xx)
        angle_ = math.atan(yy/xx)
        #print('angle:')
        angle = angle_*180/np.pi
        #print(angle)

        c_x = (coordx[0][0] + coordx[2][0])//2
        c_y = (coordx[0][1] + coordx[2][1])//2

        # 仿射变换
        M=cv2.getRotationMatrix2D((c_x, c_y),angle,1)
        img=cv2.warpAffine(img, M, (img.shape[0], img.shape[1]))

        # -------------

        #cv2.imshow('rotate', img)
        #ROI位置参数
        r = 0.4
        #输出尺寸归一化后ROI区域信息
        # (center_x, center_y)
        #画框，显示图片
        #cv2.rectangle(img, (int((c_x-d/2)), int((c_y + r*d))), (int((c_x + d/2)), int((c_y + r*d + d))), (255, 0, 0), 2)
        #cv2.imshow('point', img)
        return cv2.resize(img[int(c_y + r*d):int(c_y + r*d + d),int(c_x-d/2):int(c_x + d/2)],(128,128),interpolation=cv2.INTER_LINEAR)
    def neighbours(self,x, y, image) :
        #返回p1的8个邻域
        img = image
        x_1, y_1, x1, y1 = x - 1, y - 1, x + 1, y + 1
        return [img[x_1][y], img[x_1][y1], img[x][y1], img[x1][y1],  # P2,P3,P4,P5
                img[x1][y], img[x1][y_1], img[x][y_1], img[x_1][y_1]]  # P6,P7,P8,P9
    def transitions(self,neighbours) :
        # 大小比较变换
        n = neighbours + neighbours[0 :1]  # P2, P3, ... , P8, P9, P2
        return sum((n1, n2) == (0, 1) for n1, n2 in zip(n, n[1 :]))  # (P2,P3), (P3,P4), ... , (P8,P9), (P9,P2)
    def origin_LBP(self,img):
        h, w = img.shape
        dst = np.zeros((h,w),dtype=img.dtype)
        img=np.pad(img, ((1, 1), (1, 1)), 'constant', constant_values=(0, 0))
        start_index=1
        for i in range(start_index,h):
            for j in range(start_index,w):
                center = img[i][j]
                code = 0
    #             顺时针，左上角开始的8个像素点与中心点比较，大于等于的为1，小于的为0，最后组成8位2进制
                code |= (img[i-1][j-1] >= center) << (np.uint8)(7)
                code |= (img[i-1][j  ] >= center) << (np.uint8)(6)
                code |= (img[i-1][j+1] >= center) << (np.uint8)(5)
                code |= (img[i  ][j+1] >= center) << (np.uint8)(4)
                code |= (img[i+1][j+1] >= center) << (np.uint8)(3)
                code |= (img[i+1][j  ] >= center) << (np.uint8)(2)
                code |= (img[i+1][j-1] >= center) << (np.uint8)(1)
                code |= (img[i  ][j-1] >= center) << (np.uint8)(0)
                dst[i-start_index][j-start_index]= code
        return dst
    def multi_scale_block_LBP(self,img, scale):
        h, w = img.shape

        # cellSize表示一个cell大小
        cellSize = int(scale / 3)
        offset = int(cellSize / 2)
        cellImage = np.zeros((h - 2 * offset, w - 2 * offset), dtype=img.dtype)
        for i in range(offset, h - offset):
            for j in range(offset, w - offset):
                temp = 0
                for m in range(-offset, offset + 1):
                    for n in range(-offset, offset + 1):
                        temp += img[i + n, j + m]
                #             即取一个cell里所有像素的平均值
                temp /= (cellSize * cellSize)
                cellImage[i - offset, j - offset] = np.uint8(temp)
        #     再对平均后的像素做LBP特征处理
        cellImage = np.pad(cellImage, ((1, 1), (1, 1)), 'constant', constant_values=(0, 0))
        dst = self.origin_LBP(cellImage)
        return dst
    def Refine(self,image) :
        Image_Thinned = image.copy()
        changing1 = changing2 = 1  # 待移除的点
        while changing1 or changing2 :  # 迭代直到图像没有变化
            # Step 1
            changing1 = []
            [rows, columns]= Image_Thinned.shape
            for x in range(1, rows - 1) :
                for y in range(1, columns - 1) :
                    P2, P3, P4, P5, P6, P7, P8, P9 = n = self.neighbours(x, y, Image_Thinned)
                    if (Image_Thinned[x][y] == 1 and
                            2 <= sum(
                                n) <= 6 and  #  2<= N(P1) <= 6　　　保证不是孤立点
                            self.transitions(
                                n) == 1 and
                            P2 * P4 * P6 == 0 and  #  移除右下边界点
                            P4 * P6 * P8 == 0) :
                        changing1.append((x, y))
            for x, y in changing1 :
                Image_Thinned[x][y] = 0
            # Step 2
            changing2 = []

            for x in range(1, rows - 1) :
                for y in range(1, columns - 1) :
                    P2, P3, P4, P5, P6, P7, P8, P9 = n = self.neighbours(x, y, Image_Thinned)
                    if (Image_Thinned[x][y] == 1 and
                            2 <= sum(n) <= 6 and
                            self.transitions(n) == 1 and
                            P2 * P4 * P8 == 0 and  # 　　　移除左上边界点
                            P2 * P6 * P8 == 0) :
                        changing2.append((x, y))
            for x, y in changing2 :
                Image_Thinned[x][y] = 0
        return Image_Thinned
    def getGabor(self,img,filters):
        res = [] #滤波结果
        for i in range(len(filters)):
            accum = np.zeros_like(img)
            for kern in filters[i]:
                fimg = cv2.filter2D(img, cv2.CV_8UC1, kern)
                accum = np.maximum(accum, fimg, accum)
            res.append(np.asarray(accum))
        return res  #返回滤波结果,结果为24幅图，按照gabor角度排列
    def build_filters(self) :
        """ 返回一个多方向滤波核的list
        """
        filters = []
        ksize = 31  # gaborl尺度 这里是一个
        for theta in np.arange(0, np.pi, np.pi / 8) :  # gaborl方向 0 45 90 135 角度尺度的不同会导致滤波后图像不同

            params = {'ksize' : (ksize, ksize), 'sigma' : 3.3, 'theta' : theta, 'lambd' : 18.3,
                      'gamma' : 4.5, 'psi' : 0.89, 'ktype' : cv2.CV_32F}
            # gamma越大核函数图像越小，条纹数不变，sigma越大 条纹和图像都越大
            # psi这里接近0度以白条纹为中心，180度时以黑条纹为中心
            # theta代表条纹旋转角度
            # lambd为波长 波长越大 条纹越大
            kern = cv2.getGaborKernel(**params)  # 创建内核
            kern /= 1.5 * kern.sum()
            filters.append((kern, params))
        return filters
    def process(self,img, filters) :
        """ returns the img filtered by the filter list
        """
        accum = np.zeros_like(img)  # 初始化img一样大小的矩阵
        for kern, params in filters :
            fimg = cv2.filter2D(img, cv2.CV_8UC3, kern)  # 2D滤波函数  kern为其滤波模板
            np.maximum(accum, fimg, accum)  # 参数1与参数2逐位比较  取大者存入参数3  这里就是将纹理特征显化更加明显
        return accum
    def pooling(self,data, m, n, key='mean'):
        h,w = data.shape
        img_new = []
        for i in range(0,h,m):
            line = []
            for j in range(0,w,n):
                x = data[i:i+m,j:j+n]#选取池化区域
                if key == 'mean': #平均池化
                    line.append(np.sum(x[:,:]/(n*m)))
                elif key == 'max': #均值池化
                    line.append(np.max(x[:,:]))
                else:
                    return data
            img_new.append(line)
        return np.array(img_new,dtype='uint8')
    def return_bin(self,int1):
        #给出灰度值返回8位编码
        int1 = int(int1)
        str1 = bin(int1)[2:]
        return '0' * (8 - len(str1)) + str1
    def return_code_and_img(self,dstimg):
        #转灰度图
        dstimg = cv2.cvtColor(dstimg, cv2.COLOR_BGR2GRAY)
        #建立滤波器
        filters = self.build_filters()
        #滤波处理
        a = self.process(dstimg, filters)
        #clahe优化
        clahe1 = cv2.createCLAHE(clipLimit=10, tileGridSize=(8, 8))
        c = clahe1.apply(a)
        #高斯滤波
        filtered = cv2.GaussianBlur(c, (3, 3), 1)
        #二次clahe优化
        clahe2 = cv2.createCLAHE(clipLimit=6, tileGridSize=(4, 4))
        filtered = clahe2.apply(filtered)
        #缩小模板尺寸（通过调节池化范围大小）
        pool = self.pooling(filtered, 8, 8)
        #MB-LBP编码
        lbp_img = self.multi_scale_block_LBP(pool, 9)
        #细化结构
        final = self.Refine(lbp_img)

        '''
        LBP_string=''
        #遍历模板给出编码
        for xxx in range(pool.shape[0]):
            for yyy in range(pool.shape[1]):
                LBP_string += return_bin(pool[xxx, yyy])
        print(len(LBP_string), LBP_string)
        return LBP_string,final
        '''
        template,string=self.process_NBP(final)
        return string,template