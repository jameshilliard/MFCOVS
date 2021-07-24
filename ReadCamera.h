#pragma once
#include <QThread>
#include <QObject>
#include <memory>
#include <opencv.hpp>
#include <dshow.h>
#include <QImage>
#include "ovm_200_feature.h"
#include "ovm_200_match.h"
#include "ovm_200_defines.h"
#include "ovm_200_feature_dev.h"
#include <QDebug>
#include <QMetaType>     

Q_DECLARE_METATYPE(QImage)

using namespace std;
using namespace cv;
using namespace OVS;

class ThreadImage : public QObject
{
	Q_OBJECT
public:
	ThreadImage();
	~ThreadImage();
signals:
	void SendPicture(QImage& img);
	// void SendPicture();
	// void SendPicture(Mat img);
public slots:
	void MainWindowDisplay();

public:
	void ThreadStart();
	void ThreadStop();

	QImage cvMat2QImage(const cv::Mat& mat);

	shared_ptr<FeatureDevelopTool> p_feature_dev;
	bool b_thread_status;
};