#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "no video path." << endl;
        return 0;
    }

    VideoCapture cap(argv[1]);
    if (!cap.isOpened())
    {
        cerr << "cannot open video file: " << argv[1] << endl;
        return 0;
    }

    bool ok = true;
    int k = 0;
    while (ok)
    {
        Mat raw;
        cap >> raw;
        if (raw.cols == 0)
        {
            cout << "no frame" << endl;
            break;
        }
        k++;
        cout << k << endl;

        Mat res;
        resize(raw, res, Size(640, 360));
        Mat gray;
        cvtColor(res, gray, COLOR_BGR2GRAY);
        Ptr<CLAHE> clahe = createCLAHE(20, Size(128, 128));
        Mat enh;
        clahe->apply(gray, enh);

        vector<KeyPoint> kps;
        FAST(enh, kps, 27);
        cout << kps.size() << endl;
        for(const auto& kp : kps)
        {
            circle(enh, kp.pt, 3, Scalar::all(255), -1);
        }

        imshow("frame", enh);
        int key = waitKey(30);
        if(key == 27)
            break;
    }
}