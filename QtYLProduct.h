#pragma once

#include <QtWidgets/QMainWindow>
#include <memory>
#include <QTimer>
#include <QImage>
#include <QSettings>
#include <QTextBrowser>
#include <QString>
#include <QLineEdit>
#include <thread>
#include <QThread>
#include <QPixmapCache>

#include "ui_QtYLProduct.h"
#include <opencv.hpp>

#include "ReadCamera.h"

#include "ovm_200_feature.h"
#include "ovm_200_match.h"
#include "ovm_200_defines.h"
#include "ovm_200_feature_dev.h"

using namespace std;
using namespace cv;
using namespace OVS;

class QtYLProduct : public QMainWindow
{
    Q_OBJECT

public:
    QtYLProduct(QWidget *parent = Q_NULLPTR);
    ~QtYLProduct();
private:
   shared_ptr<Ui::QtYLProductClass> p_ui;
   shared_ptr< FeatureDevelopTool> p_feature_develop;
   
   QThread* p_first_thread;
   ThreadImage* p_thread_preview;

   Mat picture;

   // 线程

   // 配置文件
   shared_ptr<QSettings> p_setting_config;

   // sn码
   bool b_sn_config;
   QString SN;
   // 产品型号
   bool b_product_version;
   QString product_version;
   // 硬件版本
   bool b_fireware_version;
   QString fireware_version;
   // 固件版本
   bool b_hardware_version;
   QString hardware_version;
   // 分辨率
   bool b_dpi;
   QString dpi;
   // 分辨率合格标志位阀值
   int dpi_mark_limit;
   QString dpi_mark;
   // 距离传感器
   bool b_distance_sensor;
   QString distance_sensor;
   // IR-LED合格标志位
   bool b_IRLED_mark;
   QString IRLED_mark;
   // current数值
   bool b_current_value;
   QString current_value;

public slots:
    void RecivePicture(QImage& img);
    // void RecivePicture();
    // void RecivePicture(Mat img);
private:
    // 启动
    void RunInit();
	// 结束
	void RunUnInit();
    // 退出
    void RunStop();
    QImage cvMat2QImage(const cv::Mat& mat);

    // 读取配置文件
    void ReadConfig();
    // 写入配置文件
    void WriteConfig();

    // 读取flash
    bool ReadFlash();
    // 写入flash
    bool WriteFlash();
	
    
    // log输出
    void Showlog(QString temp_str);

    // 测试
    void Protest();
};
