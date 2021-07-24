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

   // �߳�

   // �����ļ�
   shared_ptr<QSettings> p_setting_config;

   // sn��
   bool b_sn_config;
   QString SN;
   // ��Ʒ�ͺ�
   bool b_product_version;
   QString product_version;
   // Ӳ���汾
   bool b_fireware_version;
   QString fireware_version;
   // �̼��汾
   bool b_hardware_version;
   QString hardware_version;
   // �ֱ���
   bool b_dpi;
   QString dpi;
   // �ֱ��ʺϸ��־λ��ֵ
   int dpi_mark_limit;
   QString dpi_mark;
   // ���봫����
   bool b_distance_sensor;
   QString distance_sensor;
   // IR-LED�ϸ��־λ
   bool b_IRLED_mark;
   QString IRLED_mark;
   // current��ֵ
   bool b_current_value;
   QString current_value;

public slots:
    void RecivePicture(QImage& img);
    // void RecivePicture();
    // void RecivePicture(Mat img);
private:
    // ����
    void RunInit();
	// ����
	void RunUnInit();
    // �˳�
    void RunStop();
    QImage cvMat2QImage(const cv::Mat& mat);

    // ��ȡ�����ļ�
    void ReadConfig();
    // д�������ļ�
    void WriteConfig();

    // ��ȡflash
    bool ReadFlash();
    // д��flash
    bool WriteFlash();
	
    
    // log���
    void Showlog(QString temp_str);

    // ����
    void Protest();
};
