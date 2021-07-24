import cv2
from enhance import enhance


def get_kp(img,mode='sift'):
    sift = cv2.xfeatures2d.SIFT_create()
    orb=cv2.ORB_create()
    if mode=='sift':
        kp1, des = sift.detectAndCompute(img, None)
    else:
        kp1,des=orb.detectAndCompute(img,None)
    return kp1,des
def match_roi(des1,des2,ratio=0.8):
    matcher = cv2.BFMatcher()
    raw_matches = matcher.knnMatch(des1, des2, k=2)
    good_matches = []
    for m1, m2 in raw_matches:
        #  如果最接近和次接近的比值大于一个既定的值，那么我们保留这个最接近的值，认为它和其匹配的点为good_match
        if m1.distance < ratio * m2.distance:
            good_matches.append([m1])
    #matches = cv2.drawMatchesKnn(img1, kp1, img2, kp2, good_matches, None, flags=2)
    #cv2.imshow('matches',matches)
    #cv2.waitKey(0)
    return len(good_matches),len(raw_matches)
def main(dataset,collected):
    kp1,des1=get_kp(collected)
    kp2,des2=get_kp(dataset)
    good1,total1=match_roi(des1,des2)
    good2,total2=match_roi(des2,des1)
    return 200*good1/(total1+total2)


if __name__ == '__main__':
#We modified the process of feature points extraction and feature matches.
#1.Now we use diffience of Gaussians to enhance the original roi, then we normalized the roi, finally we use CLAHE to
#get the proper constast.
#2.After we enhance the roi, we use SIFT to extract feature points, acquired its position and describe matrix.
#3.When we want to regist a palm, we extract and enhance its roi and save it into database. The size of image you want
#to save should be (128,128).
#4.When we want to make comparsion between database and collected palm, we could call main function, which inputs are
#database image and collected image. Then we could get a comparision result(If they are not same,the result should be
# under 30).
#Example
    #Use the enhanced image to find feature points and make matches
    dataset=cv2.imread(r'D:\work\palm_vein\image\roi\roi_2119_R_15898714612_1_29_0_9_.bmp')
    collected=cv2.imread(r'D:\work\palm_vein\image\roi\roi_2117_R_15898714612_1_30_0_7_.bmp')
    dataset=enhance(dataset)
    collected=enhance(collected)
    score=main(dataset,collected)
    #We set the threshold as 30, if the score >=30, then we regard two rois are the same.
    print(score)

