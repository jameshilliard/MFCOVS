import numpy as np
import cv2 as cv

from get_data import get_pos_samples, get_neg_samples, read_pos_samples, read_neg_samples
from svm_train import svm_config, svm_train, svm_save, svm_load


# 计算hog特征
def computeHog(imgs, features):
    hog = cv.HOGDescriptor((64, 64), (16, 16), (4, 4), (8, 8), 9)
    count = 0
    for i in range(len(imgs)):
        '''
            if imgs[i].shape[1] >= wsize[1] and imgs[i].shape[0] >= wsize[0]:
            y = imgs[i].shape[0] - wsize[0]
            x = imgs[i].shape[1] - wsize[1]
            h = imgs[i].shape[0]
            w = imgs[i].shape[1]
           '''
        roi = cv.resize(imgs[i],(64,64))
        features.append(hog.compute(roi))
        #print(hog.compute(roi))
        count += 1

    print('count = ', count)
    return features


# 获取svm参数
def get_svm_detector(svm):
    sv = svm.getSupportVectors()
    rho, _, _ = svm.getDecisionFunction(0)
    sv = np.transpose(sv)
    return np.append(sv, [[-rho]], 0)


# 加载hardexample
def get_hard_samples(svm, hog_features, labels):
    hog = cv.HOGDescriptor((64, 64), (16, 16), (4, 4), (8, 8), 9)
    hard_examples = []
    hog.setSVMDetector(get_svm_detector(svm))
    negs, hardlabel = read_neg_samples(r'E:\work1002_svdd\feature-detection-master\orb_zls\nopalm')

    for i in range(len(negs)):
        rects, wei = hog.detectMultiScale(negs[i], 0, winStride=(8, 8), padding=(0, 0), scale=1.01)
        for (x, y, w, h) in rects:
            hardexample = negs[i][y: y + h, x: x + w]
            hard_examples.append(cv.resize(hardexample, (100, 100)))

    computeHog(hard_examples, hog_features)
    [labels.append(-1) for _ in range(len(hard_examples))]
    svm_train(svm, hog_features, labels)
    hog.setSVMDetector(get_svm_detector(svm))
    hog.save('myHogDector1.bin')
    # svm.train(np.array(hog_features),cv.ml.ROW_SAMPLE,np.array(labels))


# 获取所有的hog特征
def get_features(features, labels):
    pos_imgs, pos_labels = read_pos_samples(r'E:\work1002_svdd\testdata_')
    computeHog(pos_imgs, features)

    [labels.append(1) for _ in range(len(pos_imgs))]

    neg_imgs, neg_labels = read_neg_samples(r'E:\work1002_svdd\feature-detection-master\orb_zls\nopalm')
    computeHog(neg_imgs, features)

    [labels.append(-1) for _ in range(len(neg_imgs))]

    # print('feature_shape = ',np.shape(features))

    return features, labels


# hog训练
def hog_train(svm):
    features = []
    labels = []

    hog = cv.HOGDescriptor((64, 64), (16, 16), (4, 4), (8, 8), 9)

    # get hog features
    get_features(features, labels)

    # svm training
    print('svm training...')
    svm_train(svm, features, labels)
    print('svm training complete...')

    hog.setSVMDetector(get_svm_detector(svm))
    hog.save('myHogDector.bin')

    print('hard samples training...')
    get_hard_samples(svm, features, labels)
    print('hard samples complete...')


if __name__ == '__main__':
    # svm config
    svm = svm_config()

    # hog training
    hog_train(svm)
