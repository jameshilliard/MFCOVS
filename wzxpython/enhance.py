import cv2
import numpy as np
from scipy import signal


# Diffience of Gaussian
def gaussConv(I, size, sigma):
    # 卷积核的高和宽
    H, W = size
    # 构造水平方向上非归一化的高斯卷积核
    xr, xc = np.mgrid[0:1, 0:W]
    xc -= np.int((W - 1) / 2)
    xk = np.exp(-np.power(xc, 2.0) / (2.0 * pow(sigma, 2)))
    # I 与 xk 卷积
    I_xk = signal.convolve2d(I, xk, 'same', 'symm')
    # 构造垂直方向上的非归一化的高斯卷积核
    yr, yc = np.mgrid[0:H, 0:1]
    yr -= np.int((H - 1) / 2)
    yk = np.exp(-np.power(yr, 2.0) / (2.0 * pow(sigma, 2.0)))
    # I_xk 与 yk 卷积
    I_xk_yk = signal.convolve2d(I_xk, yk, 'same', 'symm')
    I_xk_yk *= 1.0 / (2 * np.pi * pow(sigma, 2.0))
    return I_xk_yk


def DoG(I, size, sigma, k=1.4):
    # 标准差为 sigma 的非归一化的高斯卷积
    Is = gaussConv(I, size, sigma)
    # 标准差为 k*sigma 的非归一化高斯卷积
    Isk = gaussConv(I, size, k * sigma)
    # 两个高斯卷积的差分
    doG = Is - Isk
    doG /= (pow(sigma, 2.0) * (k - 1))
    return doG


# Image enhancement
def enhance(roi):
    # Judge the depth of input roi, if not grayscale image, convert it into grayscale.
    if len(roi.shape) == 3:
        roi = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)
    # Create CLAHE optimizer
    clahe = cv2.createCLAHE(4, (8, 8))
    # Do diffience of Gaussian
    gauss = DoG(roi, (11, 11), 2.8)
    # Normalize
    gauss = (gauss - np.min(gauss)) * 255 / (np.max(gauss) - np.min(gauss))
    gauss = gauss.astype(np.uint8)
    # cv2.imshow('img1', gauss)
    # Do CLAHE optimize
    gauss = clahe.apply(gauss)
    gauss = gauss.astype(np.uint8)
    return gauss


def main(roi):
    return enhance(roi)


if __name__ == '__main__':
    # You can call main function to enhance roi
    # Example:
    # Please input the ROI into main function
    roi = cv2.imread('path to your roi')
    enhanced = main(roi)
