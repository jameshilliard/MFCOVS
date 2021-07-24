#include "ReadCamera.h"

ThreadImage::ThreadImage()
	:b_thread_status(true)
{
	p_feature_dev = make_shared<FeatureDevelopTool>();
	
}

ThreadImage::~ThreadImage()
{

}

void ThreadImage::MainWindowDisplay()
{
	while (b_thread_status)
	{
		//qDebug() << "*******************first therad" <<QThread::currentThread();
		Mat mat_src_image;

		OVS::Image temp_image_full;
		string temp_feature;
		OVS::Information temp_info;
		// ÏÔÊ¾Í¼Ïñ
		p_feature_dev->getPreviewData1080p(mat_src_image);
		if (mat_src_image.empty())
		{
			qDebug() << "image empty";
			continue;
		}
		else {
			qDebug() << "image nomral";
		}

		Scalar color = Scalar(140.0f, 199.0f, 0.0f);
		line(mat_src_image, Point(760, 540), Point(1160, 540), color, 1, LINE_AA);
		line(mat_src_image, Point(960, 340), Point(960, 740), color, 1, LINE_AA);
		Rect rect(10, 70, 1900, 940);
		//Rect rect(50, 50, 1820, 980);
		Mat display_show;
		mat_src_image(rect).copyTo(display_show);
		
		QImage global_image = QImage((const unsigned char*)display_show.data,
			display_show.cols, display_show.rows, QImage::Format_RGB888).rgbSwapped();
		
			
		emit SendPicture(global_image);
		// emit SendPicture();
		// emit SendPicture(cvMat2QImage(mat_src_image));
		
		Sleep(1);
	}
}

void ThreadImage::ThreadStart()
{
	b_thread_status = true;
}

void ThreadImage::ThreadStop()
{
	b_thread_status = false;
}

QImage ThreadImage::cvMat2QImage(const cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1
	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar* pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar* pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3
	else if (mat.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar* pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		qDebug() << "CV_8UC4";
		// Copy input Mat
		const uchar* pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		qDebug() << "ERROR: Mat could not be converted to QImage.";
		return QImage();
	}
}
