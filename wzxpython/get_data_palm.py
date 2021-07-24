import cv2 as cv
import random
import glob
import os


# 加载负样本
def get_neg_samples(foldername, savePath):
    count = 0
    imgs = []
    labels = []
    filenames = glob.iglob(os.path.join(foldername, '*'))
    for filename in filenames:
        print('filename = ', filename)
        src = cv.imread(filename, 0)
        if ((src.cols >= 540) & (src.rows >= 540)):
            x = random.uniform(0, src.cols - 32)
            y = random.uniform(0, src.rows - 32)
            imgRoi = src(cv.Rect(x, y, 32, 32))
            imgs.append(imgRoi)
            saveName = savePath + 'neg' + str(count) + '.jpg'
            cv.imwrite(saveName, imgRoi)
            label = 'neg' + str(count) + '.jpg'
            labels.append(label)
            label = label + '\n'
            with open('negative.txt','w') as f:
                f.write(label)
            count += 1
    return imgs, labels


# 读取负样本
def read_neg_samples(foldername):
    imgs = []
    labels = []
    neg_count = 0
    filenames = glob.iglob(os.path.join(foldername, '*'))
    for filename in filenames:
        # print('filename = ',filename)
        src = cv.imread(filename, 0)
        # cv.imshow("src",src)
        # cv.waitKey(5)
        src = src[:, 420:1500]
        src = cv.resize(src, (100, 100))
        imgs.append(src)
        labels.append(-1)
        neg_count += 1

    # print ('neg_count = ',neg_count)
    return imgs, labels


# 加载正样本
def get_pos_samples(foldername, savePath):
    count = 0
    imgs = []
    labels = []
    filenames = glob.iglob(os.path.join(foldername, '*'))
    for filename in filenames:
        print('filename = ', filename)
        src = cv.imread(filename,0)
        imgRoi = src(cv.Rect(0, 0, 32, 32))
        imgs.append(imgRoi)
        saveName = savePath + 'pos' + str(count) + '.jpg'
        cv.imwrite(saveName, imgRoi)

        label = 'pos' + str(count) + '.jpg'
        labels.append(label)
        with open('positive.txt', 'w') as f:
            f.write(label)
        count += 1
    return imgs, labels


# 读取正样本
def read_pos_samples(foldername):
    imgs = []
    labels = []
    pos_count = 0
    filenames = glob.iglob(os.path.join(foldername, '*'))

    for filename in filenames:
        src = cv.imread(filename,0)
        src = src[:, 420:1500]
        src = cv.resize(src, (100, 100))
        imgs.append(src)
        labels.append(1)
        pos_count += 1

    return imgs, labels
