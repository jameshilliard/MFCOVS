#include "QtYLProduct.h"

QtYLProduct::QtYLProduct(QWidget* parent)
	: QMainWindow(parent)
{
	p_ui = make_shared<Ui::QtYLProductClass>();
	p_ui->setupUi(this);
	qRegisterMetaType<QImage>("QImage&");
	qRegisterMetaType<Mat>("Mat&");
	// 参数1 信号的发送者 参数二 发送的信号（函数的地址）
	// 参数3 信号的接受者 参数4 处理的槽函数
	connect(p_ui->button_start, &QPushButton::clicked, this, &QtYLProduct::RunInit);
	connect(p_ui->button_end, &QPushButton::clicked, this, &QtYLProduct::RunUnInit);
	connect(p_ui->button_stop, &QPushButton::clicked, this, &QtYLProduct::RunStop);

	p_first_thread = new QThread();
	p_thread_preview = new ThreadImage();
	p_thread_preview->moveToThread(p_first_thread);
	connect(p_first_thread, &QThread::started, p_thread_preview, &ThreadImage::MainWindowDisplay);

	//connect(p_thread_preview, SIGNAL(ThreadImage::SendPicture(QImage)), this, SLOT(QtYLProduct::RecivePicture(QImage)));
	connect(p_thread_preview, &ThreadImage::SendPicture, this, &QtYLProduct::RecivePicture);
	// connect(p_thread_preview, SIGNAL(ThreadImage::SendPicture(Mat)), this, SLOT(QtYLProduct::RecivePicture(Mat)), Qt::DirectConnection);
	QPixmapCache::setCacheLimit(1);
}

QtYLProduct::~QtYLProduct()
{
	delete p_thread_preview;
}

void QtYLProduct::RunInit()
{
	int result_ok = InitFeatureSDK();
	StartMatchMode(nullptr, nullptr);

	p_feature_develop = make_shared<FeatureDevelopTool>();

	if (result_ok == SUCCESS)
	{
		Showlog("Init sdk success!");
		p_first_thread->start();

	}
	else if (result_ok == ERR_NO_DEVICE)
	{
		Showlog("no device.");
		return;
	}
	else
	{
		Showlog("init sdk failed.");
		return;
	}

	// qDebug() << "main id" << QThread::currentThread();

	// 读取配置文件
	ReadConfig();

	// 读取flash
	ReadFlash();
}


void QtYLProduct::RecivePicture(QImage& img)
{
	if (img.isNull())
	{
		return;
	}
	QPixmapCache::clear();
	p_ui->label_preview->setPixmap(QPixmap::fromImage(img));
}
//
//void QtYLProduct::RecivePicture()
//{
//	qDebug() << "***********RecivePicture";
//	if (p_thread_preview->mat_src_image.empty())
//	{
//		return;
//	}
//	p_ui->label_preview->setPixmap(QPixmap::fromImage(cvMat2QImage(p_thread_preview->mat_src_image)));
//}
//
//void QtYLProduct::RecivePicture(Mat img)
//{
//	qDebug() << "RecivePicture";
//	p_ui->label_preview->setPixmap(QPixmap::fromImage(cvMat2QImage(p_thread_preview->mat_src_image));
//}

//void QtYLProduct::DisplayFrame()
//{
//	/*QImage temp_img = QImage((const unsigned char*)mat_src_image.data,
//		mat_src_image.cols, mat_src_image.rows, QImage::Format_RGB888).rgbSwapped();
//	emit SendPicture(temp_img);*/
//}

void QtYLProduct::ReadConfig()
{
	p_setting_config = make_shared<QSettings>("config.ini", QSettings::IniFormat);

	b_sn_config = p_setting_config->value("Control/b_sn").toBool();
	b_product_version = p_setting_config->value("Control/b_product_version").toBool();
	b_fireware_version = p_setting_config->value("Control/b_fireware_version").toBool();
	b_hardware_version = p_setting_config->value("Control/b_hardware_version").toBool();
	b_dpi = p_setting_config->value("Control/b_dpi").toBool();
	b_distance_sensor = p_setting_config->value("Control/b_distance_sensor").toBool();
	b_IRLED_mark = p_setting_config->value("Control/b_IRLED_mark").toBool();
	b_current_value = p_setting_config->value("Control/b_current_value").toBool();

	SN = p_setting_config->value("Value/sn").toString();
	product_version = p_setting_config->value("Value/product_version").toString();
	fireware_version = p_setting_config->value("Value/fireware_version").toString();
	hardware_version = p_setting_config->value("Value/hardware_version").toString();
	dpi = p_setting_config->value("Value/dpi").toString();
	dpi_mark_limit = p_setting_config->value("Value/dpi_mark_limit").toInt();
	distance_sensor = p_setting_config->value("Value/distance_sensor").toString();
	IRLED_mark = p_setting_config->value("Value/IRLED_mark").toString();
	current_value = p_setting_config->value("Value/current_value").toString();
}



void QtYLProduct::WriteConfig()
{
	p_setting_config = make_shared<QSettings>("config.ini", QSettings::IniFormat);

	p_setting_config->beginGroup(tr("Control"));
	p_setting_config->setValue(tr("b_sn"), tr("true"));
	p_setting_config->setValue(tr("b_product_version"), tr("true"));
	p_setting_config->setValue(tr("b_fireware_version"), tr("true"));
	p_setting_config->setValue(tr("b_hardware_version"), tr("true"));
	p_setting_config->setValue(tr("b_dpi"), tr("true"));
	p_setting_config->setValue(tr("b_distance_sensor"), tr("true"));
	p_setting_config->setValue(tr("b_IRLED_mark"), tr("true"));
	p_setting_config->setValue(tr("b_current_value"), tr("true"));
	p_setting_config->endGroup();

	p_setting_config->beginGroup(tr("Value"));
	p_setting_config->setValue(tr("sn"), tr(""));
	p_setting_config->setValue(tr("product_version"), tr(""));
	p_setting_config->setValue(tr("fireware_version"), tr(""));
	p_setting_config->setValue(tr("hardware_version"), tr(""));
	p_setting_config->setValue(tr("dpi"), tr(""));
	p_setting_config->setValue(tr("dpi_mark_limit"), tr(""));
	p_setting_config->setValue(tr("distance_sensor"), tr(""));
	p_setting_config->setValue(tr("IRLED_mark"), tr(""));
	p_setting_config->setValue(tr("current_value"), tr(""));
	p_setting_config->endGroup();
}


bool QtYLProduct::ReadFlash()
{
	bool temp = p_feature_develop->readFromFlash();
	if (!temp)
	{
		Showlog("read flash false");
		return false;
	}

	if (b_sn_config)
	{
		string temp_sn = "";
		p_feature_develop->getDevSN(temp_sn);
		SN = QString::fromStdString(temp_sn);
		Showlog("SN: " + SN);
		p_ui->lineEdit_sn->setText(SN);
		
	}
	else
	{
		Showlog("SN: " + SN + "read only");
		p_ui->lineEdit_sn->setText(SN);
		p_ui->lineEdit_sn->setReadOnly(true);
	}

	if (b_product_version)
	{
		string temp_product_version = "";
		p_feature_develop->getProductVersion(temp_product_version);
		product_version = QString::fromStdString(temp_product_version);
		Showlog("product_version: " + product_version);
		p_ui->lineEdit_product_version->setText(product_version);
	}
	else
	{
		
		Showlog("product_version: " + product_version + "read only");
		p_ui->lineEdit_product_version->setText(product_version);
		p_ui->lineEdit_product_version->setReadOnly(true);
	}

	if (b_fireware_version)
	{
		string temp_fireware_version = "";
		p_feature_develop->getDevFireWareVer(temp_fireware_version);
		fireware_version = QString::fromStdString(temp_fireware_version);
		Showlog("fireware_version: " + fireware_version);
		p_ui->lineEdit_fireware_version->setText(fireware_version);
		
	}
	else
	{
		Showlog("fireware_version: " + fireware_version );
		p_ui->lineEdit_fireware_version->setText(fireware_version);
		p_ui->lineEdit_fireware_version->setReadOnly(true);
	}

	if (b_hardware_version)
	{
		string temp_hardware_version = "";
		p_feature_develop->getDevHardWareVer(temp_hardware_version);
		hardware_version = QString::fromStdString(temp_hardware_version);
		Showlog("hardware_version: " + hardware_version);
		p_ui->lineEdit_hardware_version->setText(hardware_version);
		
	}
	else
	{
		Showlog("hardware_version: " + hardware_version + "read only");
		p_ui->lineEdit_hardware_version->setText(hardware_version);
		p_ui->lineEdit_hardware_version->setReadOnly(true);
	}

	if (b_dpi)
	{
		string temp_dpi = "";
		p_feature_develop->getDpi(temp_dpi);
		dpi = QString::fromStdString(temp_dpi);
		Showlog("dpi: " + dpi);
		p_ui->lineEdit_dpi->setText(dpi);
	}
	else
	{
		Showlog("dpi: " + dpi + "read only");
		p_ui->lineEdit_dpi->setText(dpi);
		p_ui->lineEdit_dpi->setReadOnly(true);
	}

	/*if (!b_distance_sensor)
	{
		p_feature_develop->getDisSensor(distance_sensor.toStdString());
		Showlog("distance_sensor: " + distance_sensor + "read only");
		p_ui->lineEdit_distance_sensor->setText(distance_sensor);
		p_ui->lineEdit_distance_sensor->setReadOnly(true);
	}
	else
	{
		p_feature_develop->getDisSensor(distance_sensor.toStdString());
		Showlog("distance_sensor: " + distance_sensor);
		p_ui->lineEdit_distance_sensor->setText(distance_sensor);
	}*/

	if (b_IRLED_mark)
	{
		string temp_irled = "";
		p_feature_develop->getIRLEDMarkOfConformity(temp_irled);
		IRLED_mark = QString::fromStdString(temp_irled);
		Showlog("IRLED_mark: " + IRLED_mark);
		p_ui->radioButton_true->setChecked(true);
	}
	else
	{
		Showlog("IRLED_mark: " + IRLED_mark);
		p_ui->radioButton_true->setChecked(true);
	}

	if (b_current_value)
	{
		string temp_current_value = "";
		p_feature_develop->getCurrent(temp_current_value);
		current_value = QString::fromStdString(temp_current_value);
		Showlog("current_value: " + current_value);
		p_ui->lineEdit_current_value->setText(current_value);
	}
	else
	{
		Showlog("current_value: " + current_value + "read only");
		p_ui->lineEdit_current_value->setText(current_value);
		p_ui->lineEdit_current_value->setReadOnly(true);
	}

	Showlog("read flash success");
	return true;
}


bool QtYLProduct::WriteFlash()
{
	// 控件赋值
	SN = p_ui->lineEdit_sn->text();
	string temp_sn = SN.toStdString();
	p_feature_develop->setDevSN(temp_sn);
	Showlog("SN write success");


	product_version = p_ui->lineEdit_product_version->text();
	string temp_product_version = product_version.toStdString();
	p_feature_develop->setProductVersion(temp_product_version);
	Showlog("product_version write success");


	fireware_version = p_ui->lineEdit_fireware_version->text();
	string temp_fireware_version = fireware_version.toStdString();
	p_feature_develop->setDevFireWareVer(temp_fireware_version);
	Showlog("fireware_version write success");

	
	hardware_version = p_ui->lineEdit_hardware_version->text();
	string temp_hardware_version = hardware_version.toStdString();
	p_feature_develop->setDevHardWareVer(temp_hardware_version);
	Showlog("hardware_version write success");


	dpi = p_ui->lineEdit_dpi->text();
	string temp_dpi = dpi.toStdString();
	p_feature_develop->setDpi(temp_dpi);

	if (dpi.toInt() >= dpi_mark_limit)
	{
		dpi_mark = "true";
		p_feature_develop->setDpiMarkOfConformity(dpi_mark.toStdString());
	}
	else
	{
		dpi_mark = "false";
		p_feature_develop->setDpiMarkOfConformity(dpi_mark.toStdString());
	}
	Showlog("dpi write success!");
	

	//if (b_distance_sensor)
	//{
	//	// 电机采集距离



	//	p_feature_develop->setDevHardWareVer(distance_sensor.toStdString());
	//	Showlog("distance_sensor write success");
	//}


	//if (p_ui->radioButton_false->isChecked())
	//{
	//	IRLED_mark = "true";
	//	p_feature_develop->setIRLEDMarkOfConformity(IRLED_mark.toStdString());
	//}
	//else if (p_ui->radioButton_false->isChecked())
	//{
	//	IRLED_mark = "false";
	//	p_feature_develop->setIRLEDMarkOfConformity(IRLED_mark.toStdString());
	//}
	// Showlog("IRLED_mark write success");
	
	current_value = p_ui->lineEdit_current_value->text();
	string temp_current_value = current_value.toStdString();
	p_feature_develop->setCurrent(temp_current_value);
	Showlog("current_value write success");
	

	if (!p_feature_develop->write2Flash())
	{
		Showlog("write flash false");
		return false;
	}
	Showlog("write flash success!");
	return true;
}

void QtYLProduct::RunUnInit()
{
	WriteFlash();
	int reasult = UninitFeatureSDK();
	p_thread_preview->ThreadStop();
}

void QtYLProduct::RunStop()
{
	AbortCapture();
	int reasult = UninitFeatureSDK();
	p_thread_preview->ThreadStop();
}

QImage QtYLProduct::cvMat2QImage(const cv::Mat& mat)
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

void QtYLProduct::Showlog(QString temp_str)
{
	p_ui->textBrowser->moveCursor(QTextCursor::End);
	p_ui->textBrowser->append(temp_str);
}

//
//QT_NAMESPACE::QImage QtYLProduct::Mat2QImage(cv::Mat& cvImg)
//{
//	QImage qImg;
//	if (cvImg.channels() == 3)                             //3 channels color image
//	{
//
//		cv::cvtColor(cvImg, cvImg, CV_BGR2RGB);
//		cv::cvtColor(cvImg, cvImg, CV_BGR2RGB);
//		qImg = QImage((const unsigned char*)(cvImg.data),
//			cvImg.cols, cvImg.rows,
//			cvImg.cols * cvImg.channels(),
//			QImage::Format_RGB888);
//	}
//	else if (cvImg.channels() == 1)                    //grayscale image
//	{
//		qImg = QImage((const unsigned char*)(cvImg.data),
//			cvImg.cols, cvImg.rows,
//			cvImg.cols * cvImg.channels(),
//			QImage::Format_Indexed8);
//	}
//	else
//	{
//		qImg = QImage((const unsigned char*)(cvImg.data),
//			cvImg.cols, cvImg.rows,
//			cvImg.cols * cvImg.channels(),
//			QImage::Format_RGB888);
//	}
//
//	return qImg;
//}

void QtYLProduct::Protest()
{

}