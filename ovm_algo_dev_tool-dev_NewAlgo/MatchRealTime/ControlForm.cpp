#include "Log.h"
#include "DevHandler.h"
#include "MatchRealTime/ControlForm.h"
#include "MatchRealTime/ThreadHandler.h"
#include "ui_ControlForm.h"
#include "algorithmhandler.h"

#include <QMutex>
#include <fstream>
#include <iostream>
#include <QDateTime>
#include <QMetaType>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

OVSSTATUS        ControlForm::_ovsStatus;
OVSImage         ControlForm::_ovsImage;
OVSInformation   ControlForm::_ovsInfo;
std::string      ControlForm::_ovsFeature;

const int conMatchTimeout      = 5000;//5000ms
const int confeatureNum        = 5;//一个模板中包含的feature数量
const int conExposureDefault   = 28;//曝光值
const int conLedCurrentDefault = 0; //电流值

const int timerInterval       = 10;//30ms timeEvent
const int conImageWidth       = 540;
const int conImageHeight      = 540;
const int conCellFeatureBytes = 576;
const QString RunDataEnroll   = "RunData/Enroll";
const QString RunDataEnroll2  = "RunData/Enroll2";
const QString RunDataMatch    = "RunData/Match";
const QString RunDataMatchI   = "Imposter";
const QString RunDataMatchG   = "Genuine";

#ifdef DIFFMAP
const QString strTemplateType1 = "中远中中近远近";//使用 _DataMapVtrf
const QString strTemplateType2 = "中远中中近远-近";//使用 _DataMapVtrf + _DataMapVtrf1
const QString strTemplateType3 = "中远中中近-远近";//使用 _DataMapVtrf + _DataMapVtrf1
const QString strTemplateType4 = "中远中中近-远-近";//使用 _DataMapVtrf +_DataMapVtrf1 + _DataMapVtrf2
const QString strTemplateType5 = "中远中-中近-远近";//使用 _DataMapVtrf +_DataMapVtrf1 + _DataMapVtrf2
const QString strTemplateType6 = "中远中-中近-远-近";//使用 _DataMapVtrf+_DataMapVtrf1 + _DataMapVtrf2 +_DataMapVtrf3
#endif

#ifdef e_M2M1M_H_LDIFFSORT
const QString strTemplateType1 = "中远中中近-远-近";//使用 _DataMapVtrf
const QString strTemplateType2 = "中远中中近-近-远";//使用 _DataMapVtrf + _DataMapVtrf1
const QString strTemplateType3 = "近-远-中远中中近";//使用 _DataMapVtrf + _DataMapVtrf1
const QString strTemplateType4 = "近-中远中中近-远";//使用 _DataMapVtrf +_DataMapVtrf1 + _DataMapVtrf2
const QString strTemplateType5 = "远-近-中远中中近";//使用 _DataMapVtrf +_DataMapVtrf1 + _DataMapVtrf2
const QString strTemplateType6 = "远-中远中中近-近";//使用 _DataMapVtrf+_DataMapVtrf1 + _DataMapVtrf2 +_DataMapVtrf3
#endif

ControlForm::ControlForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlForm)
  ,_imageWidget(new imageWidget)
{
    ui->setupUi(this);

    qRegisterMetaType<OVSInformation>("OVSInformation");
    qRegisterMetaType<AuxDev::StatusInfo>("AuxDev::StatusInfo");

    bAddPalmR             = false;
    bStartCoutTime        = false;
    pMatchTimeout         = NULL;
    TableViewSelRow       = -1;
    iRevExecuteResultCout = 0;
    g_sdkStatus           = E_SDK_STATUS::e_suspend;
    iTemplateType         = TEMPLATE_TYPE::e_M2M1M_H_L;
    NewMatch_Status       = MATCH_STATUS::e_MatchSucc;
    LogOut("ControlForm Start",LOG_DEBUG);
    qRegisterMetaType<OVSInformation>("OVSInformation");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_imageWidget);
    layout->setMargin(0);
    ui->widgetImage->setLayout(layout);
    ui->widgetImage->resize(conImageWidth,conImageHeight);

    connect(this,&ControlForm::sigSendImage,_imageWidget,&imageWidget::slotRevImage);
    connect(ThreadAlgo::instance(),&ThreadAlgo::sigExecuteResult,
            this,&ControlForm::slotRevExecuteResult,Qt::QueuedConnection);
    connect(ThreadAlgo::instance(),&ThreadAlgo::sigFeatureResult,
            this,&ControlForm::slotRevFeatureResult,Qt::QueuedConnection);

    //单击选择一行
    ui->tableViewUsetList->setSelectionBehavior(QAbstractItemView::SelectRows);
    //设置只能选择一行，不能多行选中
    ui->tableViewUsetList->setSelectionMode(QAbstractItemView::SingleSelection);
    //设置每行内容不可更改
    ui->tableViewUsetList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置隔一行变一颜色，即：一灰一白
    _itemModel = new QStandardItemModel();
    QStringList strHeader;
    strHeader << "序号"
              << "姓名"
              << "电话"
              << "左手特征"
              << "右手特征";
    //添加表头数据
    _itemModel->setHorizontalHeaderLabels(strHeader);
    //设置列数
    _itemModel->setColumnCount(strHeader.size());
    //设置行数
    _itemModel->setRowCount(20);
    //隐藏列表头
    ui->tableViewUsetList->verticalHeader()->hide();
    //setModel
    ui->tableViewUsetList->setModel(_itemModel);
    //表头设置最后一列数据填充整个控件
    ui->tableViewUsetList->horizontalHeader()->setStretchLastSection(true);
    //表中选中某行，表头不加粗
    ui->tableViewUsetList->horizontalHeader()->setHighlightSections(false);
    //表头内容均分填充整个控件
//    ui->tableViewUsetList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewUsetList->setShowGrid(false);
    ui->tableViewUsetList->horizontalHeader()->resizeSection(0,50);
    ui->tableViewUsetList->horizontalHeader()->resizeSection(1,80);
    ui->tableViewUsetList->horizontalHeader()->resizeSection(2,180);
    ui->tableViewUsetList->horizontalHeader()->resizeSection(3,70);
    ui->tableViewUsetList->horizontalHeader()->resizeSection(4,60);

    ui->textBrowserLog->document()->setMaximumBlockCount(50);

    ui->comboBoxTemplateType->addItem(strTemplateType1);
    ui->comboBoxTemplateType->addItem(strTemplateType2);
    ui->comboBoxTemplateType->addItem(strTemplateType3);
    ui->comboBoxTemplateType->addItem(strTemplateType4);
    ui->comboBoxTemplateType->addItem(strTemplateType5);
    ui->comboBoxTemplateType->addItem(strTemplateType6);

    ui->labelDisInfo->setText("");
    ui->lineEditTempNum->setText("1");
    ui->checkBoxShowProgressPic->setCheckState(Qt::CheckState::Checked);
    ui->labelMatchState->setText("提示：未开始比对过程");
    ui->labelMatchState->setStyleSheet("color: rgb(255,0，0);");
    ui->label_exposureValue->setText(QString::number(conExposureDefault));
    ui->label_currentValue->setText( QString::number(conLedCurrentDefault));

    _pButtonGroup = new QButtonGroup;
    _pButtonGroup->setExclusive(true);
    _pButtonGroup->addButton(ui->radioButtonMatchL);
    _pButtonGroup->addButton(ui->radioButtonMatchR);
    ui->radioButtonMatchL->setChecked(true);

    _DataList.clear();
    SqlHandler::instance()->loadData(_DataList);
    //更新内存数据
    AddTo_DataMap(_DataList);
    //更新表格数据
    InitTableView();

    iCount            = 0;//比对计数器
    fTimerTotal1      = 0.0;
    iEnrollTempNum    = 1;
    iconCurrentValue  = conLedCurrentDefault;
    iconExposureValue = conExposureDefault;

    ui->lineEditTempNum->setVisible(false);
    ui->groupBoxDevHardware->setVisible(false);
    ui->pushButton_startMatch->setVisible(false);

    UpdateButtonEnable(true);

    pMatchTimeout = new QTimer(this);
    connect(pMatchTimeout,SIGNAL(timeout()),this,SLOT(slot_Timerout()));
//    qDebug()<<"ControlForm currentThreadId = "<<QThread::currentThreadId();
}

ControlForm::~ControlForm()
{
    if(pMatchTimeout != NULL)
    {
        pMatchTimeout->stop();
        delete pMatchTimeout;
        pMatchTimeout = NULL;
    }

    delete _pButtonGroup;
    delete _imageWidget;
    delete ui;
}

//设备初始化
int ControlForm::InitFeatureSDK()
{
    int ret = false;

    Global::instance()->InitGlobalParam();


    ret = DevHandler::instance()->init();
    if(!ret)
        LogOut("dev init fail",LOG_DEBUG);

#if defined(__linux__)      //add for windows
    g_cvCap = cv::VideoCapture(g_moduleVideoNum,cv::CAP_V4L);
#else
    g_cvCap = cv::VideoCapture(g_moduleVideoNum + cv::CAP_DSHOW);
#endif

    g_cvCap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    g_cvCap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    if(g_cvCap.isOpened()){
        LogOut("cap opened succ.",LOG_DEBUG);
    }else{
        LogOut("cap opened failed.",LOG_DEBUG);
        return ERR_SDK_INIT_FAIL;
    }
    //
    DevHandler::instance()->setExposure(conExposureDefault);
    DevHandler::instance()->setLEDCurrent(conLedCurrentDefault);

    ThreadHandler::instance();

    return true;
}

int ControlForm::UninitFeatureSDK()
{
    g_cvCap.release();
    //
    if(false == DevHandler::instance()->unInit()){
        return ERR_SDK_UNINIT_FAIL;
    }

    return 0;
}

void ControlForm::LogOut(QString str, int Level)
{
    switch (Level)
    {
    case LOG_DEBUG:
        Log::instance()->getLogPoint(LOG_MODULE_WIDGET)->debug(str);
        break;
    case LOG_INFO:
        Log::instance()->getLogPoint(LOG_MODULE_WIDGET)->info(str);
        break;
    case LOG_WARING:
        Log::instance()->getLogPoint(LOG_MODULE_WIDGET)->warn(str);
        break;
    case LOG_ERROR:
        Log::instance()->getLogPoint(LOG_MODULE_WIDGET)->error(str);
        break;
    default:
        break;
    }
}

void ControlForm::timerEvent(QTimerEvent *e)
{
#if 0
    if((g_sdkStatus == E_SDK_STATUS::e_enroll || (g_sdkStatus == E_SDK_STATUS::e_match))
           && (false == bStartCoutTime) && (NewMatch_Status = MATCH_STATUS::e_WaitPalm))
    {
        if((dis > g_iDisSensorLow) && (dis < g_iDisSensorHigh))
        {
            bStartCoutTime = true;

            qDebug()<<"计时开始：";
            msTimer.restart();
        }
    }

#endif


#if 1
    qDebug()<<"3333333333333";
    Global::instance()->getPreviewImageDetail(_ovsImage,_ovsFeature,_ovsInfo);
//    qDebug()<<"44444444444444";

    previewHandler();
//    qDebug()<<"88888888888888";
#endif

}

void ControlForm::previewHandler()
{
    int dis;
    QImage image;

#if 1
    qDebug()<<"_imageWidget->g_sensorDis = "<<g_sensorDis<<",bUpdateImage = "<<g_bUpdateImage<<"NewMatch_Status = "<<NewMatch_Status;

    dis = g_sensorDis;
    if(false == g_bUpdateImage)
    {
        if((dis < g_iDisSensorLow) || (dis > g_iDisSensorHigh))
        {
            g_bUpdateImage = true;
            ui->labelWidgetImage_2->setText("请放手");
            ui->labelWidgetImage_2->setStyleSheet("color: rgb(255,0，0);");  
            ShowFeaturePic(false);
        }

        qDebug()<<"12121212";
        return;
    }
    else
    {
        if((dis < g_iDisSensorLow) || (dis > g_iDisSensorHigh))
            ShowFeaturePic(false);

        if(0 !=_ovsImage.dataVtr.size())
            image = QImage(&_ovsImage.dataVtr[0],_ovsImage.width,
                    _ovsImage.height,QImage::Format_RGB888);

        emit sigSendImage(image,_ovsInfo);

        if(g_sdkStatus == E_SDK_STATUS::e_enroll)
        {
            QString str = "当前手掌距离:" + QString::number(g_sensorDis);
            ui->labelWidgetImage_2->setText(str);
        }

        if((dis > g_iDisSensorLow) && (dis < g_iDisSensorHigh))
        {
            if(NewMatch_Status == MATCH_STATUS::e_MatchSucc)
            {
                qDebug()<<"    ";
                qDebug()<<"    ";
                qDebug()<<"    ";
                qDebug()<<"计时开始：";
                if(g_sdkStatus == E_SDK_STATUS::e_match)
                {
                    qDebug()<<"0 计时开始：";
                    if(pMatchTimeout->isActive())
                        pMatchTimeout->stop();

                    pMatchTimeout->start(conMatchTimeout);
                }

                //iMatchImgNum比较图像计数，判断时第几个图像
                iRevExecuteResultCout      = 0;
                g_ovsInfo.iMatchImgNumSucc = 0;
                g_ovsInfo.iMatchImgNumFail = 0;

                msTimer.restart();
//                NewMatch_Status = MATCH_STATUS::e_WaitPalm;

                QString path,path1;
                path            = QDir::currentPath() + "/" + RunDataMatch;
                path1           = path + "/" + "Match-MatchUser";
                NewMatch_Status = MATCH_STATUS::e_StartMatch;

                qDebug()<<"previewHandler del  dis = "<<dis;
                //
//                DeleteFile(path,true);//删除/RunData/Match 下所有文件
//                DeleteFile(path1,true);//删除/RunData/Match/Match-MatchUser 下所有文件

            }
        }
        else
        {
            if(g_sdkStatus == E_SDK_STATUS::e_match)
            {
                //手离开距离传感器范围，开始重新计时
                bStartCoutTime   = false;
                NewMatch_Status  = MATCH_STATUS::e_MatchSucc;

                return;
            }
        }
    }
#endif

    ovsFeatureHandler();
}

void ControlForm::ovsFeatureHandler()
{
    int ret = -1;

    qDebug()<<"ovsFeatureHandler _ovsInfo.is_feature_ok = "<<_ovsInfo.is_feature_ok;
    if(_ovsInfo.is_feature_ok)
    {
#ifdef DebugInfo
        qDebug()<<"ControlForm is_feature_ok _ovsFeature.size() = "<<_ovsFeature.size()<<",_ovsInfo.is_feature_ok = "<<_ovsInfo.is_feature_ok<<",g_sdkStatus = "<<g_sdkStatus;
#endif
//        ThreadHandler::instance()->ThreadStop();

        std::string UserID;
        if(g_sdkStatus == E_SDK_STATUS::e_enroll) //注册流程
        {
            if(ui->checkBoxSaveImage->isChecked())
                StartSaveEnrollInfoToFile();
            else
                StartSaveEnrollInfo();
        }
        else if(g_sdkStatus == E_SDK_STATUS::e_match)
        {
            int hanming,iPalmType;
            ret = MatchIdentify(MatchMode::e_left,_ovsFeature,UserID,hanming,iPalmType);

            if(OVSSTATUS::SUCCESS == ret)
            {
//                if(pMatchTimeout)
                {
                    ListTimeoutMat.clear();
                    ListTimeoutInfo.clear();
                    pMatchTimeout->stop();
                }

                qDebug()<<"pMatchTimeout->isActive() = "<<pMatchTimeout->isActive();
//                float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;
//                qDebug()<<"比对成功 UserID = "<<QString::fromStdString(UserID) <<
//                          ",iMatchImgNumSucc = "<<g_ovsInfo.iMatchImgNumSucc
//                       <<",iMatchImgNumFail = "<<g_ovsInfo.iMatchImgNumFail<<"， fTimerEnreoll = "<<fTimerEnreoll;
                //查找phone对应的姓名
                DataList::iterator it;
                QString userName;
                for(it = _DataList.begin();it != _DataList.end();it++)
                {
                    _PairData = *it;
                    QString phone = _PairData.first;
                    if(QString::compare(phone,QString::fromStdString(UserID))== 0)
                    {
                        std::list<std::string> ListStr;
                        std::list<std::string>::iterator itm;

                        ListStr  = _PairData.second;
                        itm      = ListStr.begin();
                        userName = QString::fromStdString(*itm);
                    }
                }
                qDebug()<<"比对成功 UserID = "<<QString::fromStdString(UserID) <<
                          ",iMatchImgNumSucc = "<<g_ovsInfo.iMatchImgNumSucc
                       <<",iMatchImgNumFail = "<<g_ovsInfo.iMatchImgNumFail;

                QString strPath = QDir::currentPath() + "/" + RunDataMatch + "/" +
                                  "Match-MatchUser";
                ReadAllFileFromFolder(strPath);

                bool    iPalm = ui->radioButtonMatchL->isChecked();
                QString Phone = ui->lineEditPhoneNumMatch->text();

                if(0 == QString::compare(Phone,QString::fromStdString(UserID)) &&
                   (((true == iPalm) && (1 == iPalmType)) || ((iPalm == false) && (2 == iPalmType))))
                {
                    QString str  = "Name:" + userName + "-Phone:" + QString::fromStdString(UserID) + " 认证成功";
                    ui->labelWidgetImage_2->setText(str);
                    ui->labelWidgetImage_2->setStyleSheet("color: rgb(0,0,255);");
                }
                else
                {
                    QString str  = "Name:" + userName + "-Phone:" + QString::fromStdString(UserID) + " 认证失败";
                    ui->labelWidgetImage_2->setText(str);
                    ui->labelWidgetImage_2->setStyleSheet("color: rgb(255,0,255);");
                }

                iCount++;
                g_bUpdateImage   = false;
                bStartCoutTime   = false;
                NewMatch_Status  = MATCH_STATUS::e_MatchSucc;

                AlgorithmHandler::instance()->ClearStatusInfo();
//                ThreadHandler::instance()->ThreadStopTest();

                if(MATCH_SDK_STATUS::e_1v1 == g_matchType)
                    on_pushButtonStopMatch_clicked();

//                SaveInfo(QString::fromStdString(UserID),hanming,fTimerEnreoll,iPalmType);
                qDebug()<<"认证成功 iCount = "<<iCount;
            }
            else
            {
                QString strImg,strfile,qPhone,FilePathData;

                qPhone = ui->lineEditPhoneNumMatch->text();
                //保存信息至txt文件
                if(qPhone.isEmpty())
                {
                    FilePathData = QDir::currentPath() + "/" + RunDataMatchG;
                    strfile =  FilePathData + "/" + "GenuineInfo.txt";
                    strImg = "null-score:" + QString::number(hanming) + "-false" + "\n";
                }
                else
                {
                    FilePathData = QDir::currentPath() + "/" + RunDataMatchI;
                    strfile =  FilePathData + "/" + "ImposterInfo.txt";
                    strImg = "null-Match:" + qPhone + "-score:" + QString::number(hanming) + "-false" + "\n";
                }
                QFile file(strfile);
                file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
                QByteArray strByte = strImg.toLatin1();
                file.write(strByte);
                file.close();
            }

            qDebug()<<"**************完成一次成功比对 match ret = "<<ret;
        }
    }
}

//储存特征信息
bool ControlForm::StartSaveEnrollInfo()
{
#ifdef DebugInfo
    qDebug()<<"注册->添加新用户 _DataList.size = "<<_DataList.size();
#endif
    std::list<string> ListStr;

    g_sdkStatus           = E_SDK_STATUS::e_suspend;
    bStartCoutTime        = false;
    iRevExecuteResultCout = 0;

    killTimer(_timerId);

    float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;

    QString qname  = ui->lineEdit_id->text();
    QString qPhone = ui->lineEditPhoneNum->text();
    qDebug()<<"注册成功 fTimerEnreoll = "<<fTimerEnreoll<<",qPhone = "<<qPhone<<",bAddPalmR = "<<bAddPalmR;

    ui->lineEdit_id->clear();
    ui->lineEditPhoneNum->clear();

    ListStr.push_back(qname.toStdString());

    if(!bAddPalmR)
    {
        //把特征数据放入左手
        ListStr.push_back(_ovsFeature);
        ListStr.push_back("");

//        //仅用于TEST 保存的 .template 是否是一致的
//        QString str1Raw = QDir::currentPath() + "/" + RunDataEnroll + "/saveTemp.template";
//        SaveTemplate(_ovsFeature,str1Raw.toStdString());


        _DataList.append(qMakePair(qPhone,ListStr));
        AddTo_DataMap(_DataList);

        QMap<std::string,QList<Vtrf>>::iterator it;

        it = _DataMapVtrf.find(qPhone.toStdString());
        if(it != _DataMapVtrf.end())
        {
            qDebug()<<"StartSaveEnrollInfo it.key() = "<<QString::fromStdString(it.key());

            QList<Vtrf> TmpList = it.value();
            qDebug()<<"StartSaveEnrollInfo TmpList.size() = "<<TmpList.size();

        }
    }
    else
    {
        //把特征数据放入右手
        ListStr.push_back("");
        ListStr.push_back(_ovsFeature);

        UpdateTo_DataMap(qPhone,"",_ovsFeature);
    }

    string nameStr = qname.toStdString();

    qDebug()<<"StartSaveEnrollInfo before _DataList.size() = "<<_DataList.size();

    if(!bAddPalmR)
    {
        QString str1Raw = QDir::currentPath() + "/" + RunDataEnroll + "/Enroll-" + qPhone + "/" + "L-Raw" + "-" + + ".template";
        SaveTemplate(_ovsFeature,str1Raw.toStdString());
        AddTableView1Row(qname,qPhone,true,false);
        SqlHandler::instance()->addData(qPhone.toStdString(),nameStr,_ovsFeature,"");

        qDebug()<<"str1Raw = "<<str1Raw;
    }
    else
    {
        QString str1Raw = QDir::currentPath() + "/" + RunDataEnroll + "/Enroll-" + qPhone + "/" + "R-Raw" + "-" + + ".template";
        SaveTemplate(_ovsFeature,str1Raw.toStdString());
        UpdateTableView1Row(qname,qPhone,true,true);
        SqlHandler::instance()->UpdateData(qPhone.toStdString(),nameStr,false,_ovsFeature);
    }

    qDebug()<<"StartSaveEnrollInfo end _DataList.size() = "<<_DataList.size();
    QMap<std::string,QList<Vtrf>>::iterator it;
    for(it = _DataMapVtrf.begin();it != _DataMapVtrf.end();it++)
    {
        qDebug()<<"StartSaveEnrollInfo phone = "<<QString::fromStdString(it.key())<<",it.value().size() = "<<it.value().size();
    }

    bAddPalmR = false;

    return true;
}

bool ControlForm::StartSaveEnrollInfoToFile()
{
    bool bhasPalmL,bhasPalmR;
    QString qname  = ui->lineEdit_id->text();
    QString qPhone = ui->lineEditPhoneNum->text();

    bhasPalmL   = false;
    bhasPalmR   = false;

    if(qPhone.contains("L") || qPhone.contains("l") ||
       qPhone.contains("R") || qPhone.contains("r"))
    {
        int iIndex = qPhone.lastIndexOf("-");
        qPhone = qPhone.left(iIndex);
    }

    QString FilePath = QDir::currentPath() + "/" + RunDataEnroll2 + "/" +
                      "Enroll-" + qPhone;

    QDir dir(FilePath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList AllUserInfoList = dir.entryInfoList();

    for (int i = 0; i < AllUserInfoList.size(); ++i)
    {
        QFileInfo fileInfo  = AllUserInfoList.at(i);
        QString strUserInfo = fileInfo.fileName();

        if(strUserInfo.contains("-L-") || strUserInfo.contains("-l-"))
            bhasPalmL = true;

        if(strUserInfo.contains("-R-") || strUserInfo.contains("-r-"))
            bhasPalmR = true;
    }
    qDebug()<<"bhasPalmL = "<<bhasPalmL<<",bhasPalmR = "<<bhasPalmR;

    if(bhasPalmL && !bhasPalmR)
    {
        killTimer(_timerId);

        QMessageBox::warning(this,"warning",qPhone + "-人员没有右手掌特征信息");
        on_pushButtonStartEnroll_clicked();
    }
    else if(!bhasPalmL && bhasPalmR)
        QMessageBox::warning(this,"warning",qPhone + "-人员没有左手掌特征信息");
    else if(bhasPalmL && bhasPalmR)
    {
        qDebug()<<"StartSaveEnrollInfoToFile killTimer";
        killTimer(_timerId);
        g_sdkStatus = E_SDK_STATUS::e_suspend;
        QMessageBox::warning(this,"warning",qPhone + "-人员掌特征信息完整");
    }

    return true;
}

OVSSTATUS ControlForm::MatchIdentify(const MatchMode emm, const string &feature, string &userID,
                                     int &hanming, int &iPalmType)
{
    userID.clear();

    if(matchIdentify(emm,feature,userID,hanming,iPalmType))
        return OVSSTATUS::SUCCESS;
    else
        return OVSSTATUS::ERR_MATCH_FAILED;

}

bool ControlForm::matchIdentify(const MatchMode emm, const string &feature,
                                string &userID,int &hanming,int &iPalmType)
{
    if(feature.empty()){
        return false;
    }

    g_eSDKStatus = MATCH_SDK_STATUS::e_1vN;
    g_i1V1Times = 0;

    static MatchMode sMatchMode = MatchMode::e_left_right;
    if(sMatchMode != emm)
    {
        sMatchMode = emm;
        g_map1vNTimes.clear();
    }

    userID.clear();

    bool bRe = false;
    string tmpUserID;

#ifdef e_M2M1M_H_LDIFFSORT
    if(iTemplateType == e_M2M1M_H_L)
    {
        bRe = matchIdentifyDetail(_DataMapVtrf,feature,tmpUserID,iPalmType,hanming);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }
    }
#endif


#ifdef DIFFMAP
    if(iTemplateType == e_M2M1MHL)
        bRe = matchIdentifyDetail(_DataMapVtrf,feature,tmpUserID,"0",5);
    else if(iTemplateType == e_M2M1MH_L)
    {
        bRe = matchIdentifyDetail(_DataMapVtrf,feature,tmpUserID,"0",4);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }

        bRe = matchIdentifyDetail(_DataMapVtrf1,feature,tmpUserID,"1",1);
    }
    else if(iTemplateType == e_M2M1M_HL)
    {
        bRe = matchIdentifyDetail(_DataMapVtrf,feature,tmpUserID,"0",3);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }

        bRe = matchIdentifyDetail(_DataMapVtrf1,feature,tmpUserID,"1",2);
    }
    else if(iTemplateType == e_M2M1M_H_L)
    {
        bRe = matchIdentifyDetail(_DataMapVtrf,feature,tmpUserID,"0",3);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }

        bRe = matchIdentifyDetail(_DataMapVtrf1,feature,tmpUserID,"1",1);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }

        bRe = matchIdentifyDetail(_DataMapVtrf2,feature,tmpUserID,"2",1);
    }
    else if(iTemplateType == e_M2M1_M_HL)
    {
        bRe = matchIdentifyDetail(_DataMapVtrf,feature,tmpUserID,"0",2);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }

        bRe = matchIdentifyDetail(_DataMapVtrf1,feature,tmpUserID,"1",1);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }

        bRe = matchIdentifyDetail(_DataMapVtrf2,feature,tmpUserID,"2",2);
    }
    else if(iTemplateType == e_M2M1_M_H_L)
    {
        bRe = matchIdentifyDetail(_DataMapVtrf,feature,tmpUserID,"0",2);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }

        bRe = matchIdentifyDetail(_DataMapVtrf1,feature,tmpUserID,"1",1);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }

        bRe = matchIdentifyDetail(_DataMapVtrf2,feature,tmpUserID,"2",1);
        if(bRe == true)
        {
            userID = tmpUserID;
            return bRe;
        }

        bRe = matchIdentifyDetail(_DataMapVtrf3,feature,tmpUserID,"3",1);

    }
#endif

    userID = tmpUserID;

    return bRe;
}


bool ControlForm::matchIdentifyDetail(const QMap<string, string> &_DataMap, const string &sFeature, string &userID)
{
    if(_DataMap.size() < 1){
        return false;
    }

    //
    int iMatchNum = 0;
    float fTimerTotal =0.0;
    QMap<string, string>::const_iterator it;

    qDebug()<<"_DataMap.size() = "<<_DataMap.size();

    QElapsedTimer msTimer;
    msTimer.start();
    for(it = _DataMap.begin();it != _DataMap.end();it++)
    {
#if 0 //近中远各一个模板 比对方法
        std::string strTemple = it.value();
        qDebug()<<"strTemple.size() = "<<strTemple.size();

        for(int i = 0;i < 3;i++)
        {
            std::string strTempleMatch = strTemple.substr(i * 2304, 2304);
            qDebug()<<"strTempleMatch.size() = "<<strTempleMatch.size()<<",i = "<<i;

            if(matchVerify(strTempleMatch,sFeature))
            {
                userID = it.key();

                return true;
            }
        }

#elif 1 //近1中3远1  五个模板比对 方法
        std::string strTemple = it.value();
//        qDebug()<<"strTemple.size() = "<<strTemple.size();

        for(int i = 0;i < 5;i++)
        {
            QElapsedTimer msTimer1;
            msTimer1.start();
            iMatchNum++;
            std::string strTempleMatch = strTemple.substr(i * 2304, 2304);
//            qDebug()<<"strTempleMatch.size() = "<<strTempleMatch.size()<<",i = "<<i;
            float fTimerEnreoll = (double)msTimer1.nsecsElapsed()/(double)1000000;
            fTimerTotal = fTimerTotal + fTimerEnreoll;


            if(matchVerify(strTempleMatch,sFeature))
            {
                userID = it.key();
                qDebug()<<"第几个模板比对成功："<<i;
                return true;
            }
        }

#elif 0 //一个模板比对 方法
        if(matchVerify(it.value(),sFeature))
        {
            userID = it.key();

            return true;
        }
#endif
    }
    float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;
    qDebug()<<"**************1w个手掌 fTimerEnreoll = "<<fTimerEnreoll<<",iMatchNum ="
           <<iMatchNum<<"fTimerTotal = "<<fTimerTotal<<",fTimerTotal1 = "<<fTimerTotal1;
    fTimerTotal1 = 0.0;


    return false;
}

//int iPalmType: 0(未成功识别)  1(左右)  2(右手)
bool ControlForm::matchIdentifyDetail(const QMap<string, QList<Vtrf>> _DataMapVtrf, const string &sFeature,
                                      string &userID, int &iPalmType, int &succHanming)
{
    if(_DataMapVtrf.size() < 1){
        return false;
    }

    iPalmType = 0;
    bool bMatchVerifyL = false;
    bool bMatchVerifyR = false;

    QMap<string, QList<Vtrf>>::const_iterator it;
    //sFeature转换成 Vtrf ;
    Vtrf vtrsFeature;
    Global::instance()->str2VtrTemplate(sFeature,vtrsFeature);

//    QElapsedTimer msTimer;
//    msTimer.start();

    int failHanming = -1;
    _MatchSuccMap.clear();
    for(it = _DataMapVtrf.begin();it != _DataMapVtrf.end();it++)
    {
#if 1 //近1中3远1  confeatureNum个模板比对 方法
        QList<Vtrf> VtrfList = it.value();
        qDebug()<<"左手 VtrfList.size() = "<<VtrfList.size() <<",it.key() = "
                <<QString::fromStdString(it.key());

        bMatchVerifyL = matchVerify(succHanming,failHanming,VtrfList.at(0),vtrsFeature);
        if(bMatchVerifyL)
        {
            iPalmType = 1;
            userID = it.key();
            qDebug()<<" 左手比对成功：" <<"-" <<"userID = "<<QString::fromStdString(userID);
            float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;

            MatchSuccScore MatchSuccScoreTmp;

            _mutex.lock();
            MatchSuccScoreTmp.RawMat    = g_matVtr[0];
//            MatchSuccScoreTmp.RawMat    = g_matRaw;
            _mutex.unlock();
//            g_matVtr.clear();

            qDebug()<<"左手比对成功： RawMat.empty = "<<MatchSuccScoreTmp.RawMat.empty();
            MatchSuccScoreTmp.hanming   = succHanming;
            MatchSuccScoreTmp.iPalmType = 1;
            _MatchSuccMap.insert(QString::fromStdString(userID),MatchSuccScoreTmp);
//            SaveInfo(QString::fromStdString(userID),succHanming,1,true);
//            return true;
        }
        else
        {
            qDebug()<<"左手比对失败";
            cv::Mat MatTmp;
            userID = it.key();
            SaveInfo(QString::fromStdString(userID),failHanming,1,false,MatTmp);
        }

//        qDebug()<<"右手 VtrfList.size() = "<<VtrfList.size();
        if(VtrfList.size() > 1)
        {
            bMatchVerifyR = matchVerify(succHanming,failHanming,VtrfList.at(1),vtrsFeature);
            if(bMatchVerifyR)
            {
                iPalmType = 2;
                userID = it.key();
                qDebug()<<"右手比对成功：" <<"-" <<"userID = "<<QString::fromStdString(userID);
                float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;

                MatchSuccScore MatchSuccScoreTmp;

                _mutex.lock();
                MatchSuccScoreTmp.RawMat    = g_matVtr[0];
//                MatchSuccScoreTmp.RawMat    = g_matRaw;
                _mutex.unlock();
//                g_matVtr.clear();

                MatchSuccScoreTmp.hanming   = succHanming;
                MatchSuccScoreTmp.iPalmType = 2;
                _MatchSuccMap.insert(QString::fromStdString(userID),MatchSuccScoreTmp);
//                SaveInfo(QString::fromStdString(userID),succHanming,2,true);
//                return true;
            }
            else
            {
                qDebug()<<"右手比对失败";
                cv::Mat MatTmp;
                userID = it.key();
                SaveInfo(QString::fromStdString(userID),failHanming,2,false,MatTmp);
            }
        }
#endif
    }
    g_matVtr.clear();

    qDebug()<<"_MatchSuccMap.size() = "<<_MatchSuccMap.size();
    if(_MatchSuccMap.size() > 0)
    {
        QMap<QString,MatchSuccScore>::iterator it;

        for(it = _MatchSuccMap.begin();it != _MatchSuccMap.end();it++)
        {
            QString userPhone = it.key();
            MatchSuccScore MatchSuccScoreTmp = it.value();

            userID = userPhone.toStdString();

            qDebug()<<"_MatchSuccMap.size() > 0  RawMat.empty() = "<<MatchSuccScoreTmp.RawMat.empty();
            SaveInfo(userPhone,MatchSuccScoreTmp.hanming,MatchSuccScoreTmp.iPalmType,true,MatchSuccScoreTmp.RawMat);
        }

        for(int i = 0;i < ListTimeoutInfo.size();i++)
        {
            QString FileName,FilePath,SaveFileName,qPhone,strImg,strfile,FilePathData;
            qPhone       = ui->lineEditPhoneNumMatch->text();
            SaveFileName = "Match-";
            FileName     = SaveFileName + qPhone;
            FilePath     = QDir::currentPath() + "/" +RunDataMatch + "/" + FileName;
            FilePathData = FilePath + "/" + RunDataMatchG;
            strfile      = FilePathData + "/" + "GenuineInfo.txt";

            strImg = ListTimeoutInfo.at(i);
            //保存信息至txt文件
            QFile file(strfile);
            file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
            strImg = strImg + "-" + "false" + "\n";
            QByteArray strByte = strImg.toLatin1();
            file.write(strByte);
            file.close();
        }

        return true;
    }

//    float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;
//    qDebug()<<"**************1w个手掌 fTimerEnreoll = "<<fTimerEnreoll;

    return false;
}

bool ControlForm::matchVerify(int &succHanming, int &failHanming, const Vtrf &ovsTemplate, const Vtrf &feature)
{
    bool  bRe     = false;
    float hamming = 0.0f;

#if 1
    bRe = AlgoMatch::instance()->computeFeature(
                    ovsTemplate,
                    feature,
                    succHanming,failHanming);

//    succHanming = hamming;
#else

    bRe = AlgoMatch::instance()->computeFeature(
                    vtrTemplate,
                    vtrFeature,
                    conCellFeatureBytes,
                    hamming);
#endif

    if(bRe)
        return true;
    else
        return false;
}

bool ControlForm::matchVerify(const string &ovsTemplate, const string &feature)
{
    QElapsedTimer msTimer2;
    msTimer2.start();
    const string &sTemplate = ovsTemplate;
    const string &sFeature = feature;

    Vtrf vtrTemplate;
    Global::instance()->str2VtrTemplate(sTemplate,vtrTemplate);

    Vtrf vtrFeature;
    Global::instance()->str2VtrTemplate(sFeature,vtrFeature);

    float hamming = 0.0f;

    bool bRe;
    float fTimerEnreoll = (double)msTimer2.nsecsElapsed()/(double)1000000;
    fTimerTotal1 = fTimerTotal1 + fTimerEnreoll;

#if 1
    bRe = AlgoMatch::instance()->computeFeature(
                    vtrTemplate.data(),
                    vtrFeature.data(),
                    conCellFeatureBytes,
                    hamming);
#else

    bRe = AlgoMatch::instance()->computeFeature(
                    vtrTemplate,
                    vtrFeature,
                    conCellFeatureBytes,
                    hamming);
#endif
//    qDebug()<<"matchVerify bRe = "<<bRe;

    if(bRe)
    {
        if(g_eSDKStatus == MATCH_SDK_STATUS::e_1v1)
        {
            g_i1V1Times++;

            if(g_i1V1Times >= g_eMatchLevel)
            {
                //                logde<<"0104-sdk match,1v1 times:"<<g_i1V1Times;

                g_i1V1Times = 0;
                g_eSDKStatus = MATCH_SDK_STATUS::e_null;

                return true;
            }else
                return false;
        }else if(g_eSDKStatus == MATCH_SDK_STATUS::e_1vN)
            return true;
        else
            return false;
    }else
        return false;

}

//void ControlForm::str2VtrTemplate(const string &str, Vtrf &vtr)
//{
//    if(str.empty()){
//        return;
//    }

//    vtr.clear();
//    vtr.resize(str.size());

//    memcpy(vtr.data(),str.data(),str.size());
//}

//初始化TableView
void ControlForm::InitTableView()
{
    int iRow,i = 0;
    QString name,phone;
    std::string PalmL,PalmR;
    std::list<std::string> ListStr;
    std::list<std::string>::iterator itm;
    DataList::iterator it;

    iRow = _DataList.size();
    _itemModel->setRowCount(iRow);
    for(it = _DataList.begin();it != _DataList.end();it++)
    {
        _PairData = *it;

        phone = _PairData.first;

        ListStr = _PairData.second;
        itm = ListStr.begin();
        name  = QString::fromStdString(*itm);
        itm++;
        PalmL = (*itm);
        itm++;
        PalmR = (*itm);
#ifdef DebugInfo
//        qDebug()<<"updateTableView name = "<<name<<",ListStr.size()= "<<ListStr.size()<<",PalmL = "<<PalmL.size()<<",PalmR = "<<PalmR.size();
#endif

        QStandardItem *idItem     = new QStandardItem(QString::number(i + 1));
        QStandardItem *nameItem   = new QStandardItem(name);
        QStandardItem *phoneItem  = new QStandardItem(phone);

        QStandardItem *LeftPlam,*RightPlam;
        if(PalmL.size() > 0)
            LeftPlam  = new QStandardItem("EXIST");
        else
            LeftPlam  = new QStandardItem("NULL");

        if(PalmR.size() > 0)
            RightPlam  = new QStandardItem("EXIST");
        else
            RightPlam  = new QStandardItem("NULL");

        _itemModel->setItem(i,0,idItem);
        _itemModel->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));
        _itemModel->setItem(i,1,nameItem);
        _itemModel->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));
        _itemModel->setItem(i,2,phoneItem);
        _itemModel->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));
        _itemModel->setItem(i,3,LeftPlam);
        _itemModel->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));
        _itemModel->setItem(i,4,RightPlam);
        _itemModel->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));

        i++;
    }
}

//TableView添加新的一行
void ControlForm::AddTableView1Row(QString name, QString phone, bool PalmL, bool PalmR)
{
    int iNowCount = 0;

    iNowCount = ui->tableViewUsetList->model()->rowCount();
    _itemModel->setRowCount(iNowCount + 1);

    QString str = QString::number(iNowCount + 1);
    QStandardItem *idItem     = new QStandardItem(str);
    QStandardItem *nameItem   = new QStandardItem(name);
    QStandardItem *phoneItem  = new QStandardItem(phone);

    QStandardItem *LeftPlam,*RightPlam;

    if(PalmL == true)
        LeftPlam  = new QStandardItem("EXIST");
    else
        LeftPlam  = new QStandardItem("NULL");

    if(PalmR == true)
        RightPlam  = new QStandardItem("EXIST");
    else
        RightPlam  = new QStandardItem("NULL");

    _itemModel->setItem(iNowCount,0,idItem);
    _itemModel->item(iNowCount,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
    _itemModel->setItem(iNowCount,1,nameItem);
    _itemModel->item(iNowCount,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
    _itemModel->setItem(iNowCount,2,phoneItem);
    _itemModel->item(iNowCount,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
    _itemModel->setItem(iNowCount,3,LeftPlam);
    _itemModel->item(iNowCount,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
    _itemModel->setItem(iNowCount,4,RightPlam);
    _itemModel->item(iNowCount,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
}

void ControlForm::UpdateTableView1Row(QString name, QString phone, bool PalmL, bool PalmR)
{
    int iNowCount = 0;

    iNowCount = TableViewSelRow;

    QString str = QString::number(iNowCount + 1);
    QStandardItem *idItem     = new QStandardItem(str);
    QStandardItem *nameItem   = new QStandardItem(name);
    QStandardItem *phoneItem  = new QStandardItem(phone);

    QStandardItem *LeftPlam,*RightPlam;

    if(PalmL == true)
        LeftPlam  = new QStandardItem("EXIST");
    else
        LeftPlam  = new QStandardItem("NULL");

    if(PalmR == true)
        RightPlam  = new QStandardItem("EXIST");
    else
        RightPlam  = new QStandardItem("NULL");

    _itemModel->setItem(iNowCount,0,idItem);
    _itemModel->item(iNowCount,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
    _itemModel->setItem(iNowCount,1,nameItem);
    _itemModel->item(iNowCount,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
    _itemModel->setItem(iNowCount,2,phoneItem);
    _itemModel->item(iNowCount,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
    _itemModel->setItem(iNowCount,3,LeftPlam);
    _itemModel->item(iNowCount,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
    _itemModel->setItem(iNowCount,4,RightPlam);
    _itemModel->item(iNowCount,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    _itemModel->item(iNowCount, 0)->setForeground(QBrush(QColor(0, 0, 0)));
}

//更新TableView信息
void ControlForm::UpdateTableView()
{
    _DataList.clear();
    SqlHandler::instance()->loadData(_DataList);

    int iRow,i = 0;
    QString name,phone;
    std::list<std::string> ListStr;
    DataList::iterator it;
    iRow = _DataList.size();
    _itemModel->setRowCount(iRow);
    for(it = _DataList.begin();it != _DataList.end();it++)
    {
        std::string PalmL,PalmR;

        _PairData = *it;
        phone = _PairData.first;

        std::list<std::string>::iterator itm;
        ListStr = _PairData.second;
        itm = ListStr.begin();
        name  = QString::fromStdString(*itm);
        itm++;
        PalmL = (*itm);
        itm++;
        PalmR = (*itm);
#ifdef DebugInfo
//        qDebug()<<"updateTableView name = "<<name<<",ListStr.size()= "<<ListStr.size()<<",PalmL = "<<PalmL.size()<<",PalmR = "<<PalmR.size();
#endif

        QStandardItem *idItem     = new QStandardItem(QString::number(i + 1));
        QStandardItem *nameItem   = new QStandardItem(name);
        QStandardItem *phoneItem  = new QStandardItem(phone);

        QStandardItem *LeftPlam,*RightPlam;
        if(PalmL.size() > 0)
            LeftPlam  = new QStandardItem("EXIST");
        else
            LeftPlam  = new QStandardItem("NULL");

        if(PalmR.size() > 0)
            RightPlam  = new QStandardItem("EXIST");
        else
            RightPlam  = new QStandardItem("NULL");

        _itemModel->setItem(i,0,idItem);
        _itemModel->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));
        _itemModel->setItem(i,1,nameItem);
        _itemModel->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));
        _itemModel->setItem(i,2,phoneItem);
        _itemModel->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));
        _itemModel->setItem(i,3,LeftPlam);
        _itemModel->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));
        _itemModel->setItem(i,4,RightPlam);
        _itemModel->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        _itemModel->item(i, 0)->setForeground(QBrush(QColor(0, 0, 0)));

        i++;
    }
}

void ControlForm::UpdateTo_DataMap(const QString phone, const std::string featureL, const std::string featureR)
{
    QMap<std::string,QList<Vtrf>>::iterator it;

    it = _DataMapVtrf.find(phone.toStdString());

    if(it != _DataMapVtrf.end())
    {
        qDebug()<<"UpdateTo_DataMap it.key() = "<<QString::fromStdString(it.key());

        Vtrf VtrfPalmL,VtrfPalmR;
        QList<Vtrf> TmpList = it.value();

        VtrfPalmL = it.value().at(0);
        TmpList.clear();
        Global::instance()->str2VtrTemplate(featureR,VtrfPalmR);

        TmpList.append(VtrfPalmL);
        TmpList.append(VtrfPalmR);

        _DataMapVtrf.remove(phone.toStdString());
        _DataMapVtrf.insert(phone.toStdString(),TmpList);

        qDebug()<<"UpdateTo_DataMap TmpList.size() = "<<TmpList.size();

    }
    else
        qDebug()<<"未找到 :"<<phone;
}

//更新内存数据
void ControlForm::AddTo_DataMap(DataList _DataList)
{
    DataList::iterator it;

    for(it = _DataList.begin();it != _DataList.end();it++)
    {
        QString name;
        std::string phone,PalmL,PalmR;
        std::list<string> ListStr;

        _PairData = *it;
        phone = _PairData.first.toStdString();

        std::list<std::string>::iterator itm;
        ListStr = _PairData.second;
        itm = ListStr.begin();
        name  = QString::fromStdString(*itm);
        itm++;
        PalmL = (*itm);
        itm++;
        PalmR = (*itm);

//        //仅用于TEST 保存的 .template 是否是一致的
//        if(QString::compare(QString::fromStdString(phone),"15066850733") == 0)
//        {
//            QString str1Raw = QDir::currentPath() + "/" + RunDataEnroll + "/111.template";
//            SaveTemplate(PalmL,str1Raw.toStdString());
//        }
        //        if(ComboBoxCurrentIndex == 0)//中远中中近-远-近
        //        {
#ifdef e_M2M1M_H_LDIFFSORT
        QList<Vtrf> VtrfList;

        std::string strTempleMatch = PalmL;
        Vtrf vtrFeature;
        Global::instance()->str2VtrTemplate(strTempleMatch,vtrFeature);
        VtrfList.append(vtrFeature);

        qDebug()<<"PalmL vtrFeature.size() = "<<vtrFeature.size();

        if(PalmR.size() > 0)
        {
            strTempleMatch = PalmR;
            Global::instance()->str2VtrTemplate(strTempleMatch,vtrFeature);
            VtrfList.append(vtrFeature);
        }


        _DataMapVtrf.insert(phone,VtrfList);
        //        }
#endif


#ifdef DIFFMAP
        //feature中含有五个模板
        if(ComboBoxCurrentIndex == 0)//中远中中近远近
        {
            QList<Vtrf> VtrfList;
            for(int i = 0;i < confeatureNum;i++)
            {
                std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                Vtrf vtrFeature;
                str2VtrTemplate(strTempleMatch,vtrFeature);
                VtrfList.append(vtrFeature);
            }
            _DataMapVtrf.insert(phone,VtrfList);
        }
        else if(ComboBoxCurrentIndex == 1)//中远中中近远-近
        {
            QList<Vtrf> VtrfList,VtrfList1;
            for(int i = 0;i < confeatureNum;i++)
            {
                if(i <= 3)
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList.append(vtrFeature);
                }
                else
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList1.append(vtrFeature);
                }
            }
            _DataMapVtrf.insert(phone,VtrfList);
            _DataMapVtrf1.insert(phone,VtrfList1);
        }
        else if(ComboBoxCurrentIndex == 2)//中远中中近-远近
        {
            QList<Vtrf> VtrfList,VtrfList1;
            for(int i = 0;i < confeatureNum;i++)
            {
                if(i <= 2)
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList.append(vtrFeature);
                }
                else
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList1.append(vtrFeature);
                }
            }
            _DataMapVtrf.insert(phone,VtrfList);
            _DataMapVtrf1.insert(phone,VtrfList1);
        }
        else if(ComboBoxCurrentIndex == 3)//中远中中近-远-近
        {
            QList<Vtrf> VtrfList,VtrfList1,VtrfList2;
            for(int i = 0;i < confeatureNum;i++)
            {
                if(i <= 2)
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList.append(vtrFeature);
                }
                else if(i == 3)
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList1.append(vtrFeature);
                }
                else
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList2.append(vtrFeature);
                }
            }
            _DataMapVtrf.insert(phone,VtrfList);
            _DataMapVtrf1.insert(phone,VtrfList1);
            _DataMapVtrf2.insert(phone,VtrfList2);
        }
        else if(ComboBoxCurrentIndex == 4)//中远中-中近-远近
        {
            QList<Vtrf> VtrfList,VtrfList1,VtrfList2;
            for(int i = 0;i < confeatureNum;i++)
            {
                if(i <= 1)
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList.append(vtrFeature);
                }
                else if(i == 2)
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList1.append(vtrFeature);
                }
                else
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList2.append(vtrFeature);
                }
            }
            _DataMapVtrf.insert(phone,VtrfList);
            _DataMapVtrf1.insert(phone,VtrfList1);
            _DataMapVtrf2.insert(phone,VtrfList2);
        }
        else if(ComboBoxCurrentIndex == 5)//中远中-中近-远-近
        {
            QList<Vtrf> VtrfList,VtrfList1,VtrfList2,VtrfList3;
            for(int i = 0;i < confeatureNum;i++)
            {
                if(i <= 1)
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList.append(vtrFeature);
                }
                else if(i == 2)
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList1.append(vtrFeature);
                }
                else if(i == 3)
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList2.append(vtrFeature);
                }
                else
                {
                    std::string strTempleMatch = PalmL.substr(i * 2304, 2304);

                    Vtrf vtrFeature;
                    str2VtrTemplate(strTempleMatch,vtrFeature);
                    VtrfList3.append(vtrFeature);
                }
            }
            _DataMapVtrf.insert(phone,VtrfList);
            _DataMapVtrf1.insert(phone,VtrfList1);
            _DataMapVtrf2.insert(phone,VtrfList2);
            _DataMapVtrf3.insert(phone,VtrfList3);
        }
#endif

#ifdef DebugInfo
//        qDebug()<<"AddUserToSDK phone = "<< QString::fromStdString(phone)<<",PalmL.size = "<<PalmL.size()<<",PalmR.size() = "<<PalmR.size();
#endif
    }
}



void ControlForm::ClearTo_DataMap()
{
    _DataMapVtrf.clear();
    _DataMapVtrf1.clear();
    _DataMapVtrf2.clear();
    _DataMapVtrf3.clear();
}

//为1：1 比对前，做准备
bool ControlForm::ReadyForMatchMode()
{
    QAbstractItemModel *Imodel=ui->tableViewUsetList->model();
    //用户左手特征数据
    QModelIndex Iindex = Imodel->index(TableViewSelRow,3);
    QVariant datatemp  = Imodel->data(Iindex);
    QString featureL   = datatemp.toString();
    //用户右手特征数据
    Iindex            = Imodel->index(TableViewSelRow,4);
    datatemp          = Imodel->data(Iindex);
    QString featureR = datatemp.toString();

    qDebug()<<"ReadyForMatchMode featureL = "<<featureL<<",featureR = "<<featureR;

    //    //"1:1 开始左手match"
    //    if((ui->radioButtonL11->isChecked()) && (!ui->radioButtonR11->isChecked()) && (!ui->radioButtonLR11->isChecked()))
    //    {
    if(QString::compare(featureL,"NULL",Qt::CaseInsensitive) == 0)
    {
        QMessageBox::warning(this,"warning","该人员没有左手掌特征信息");
        return false;
    }
    //    }
    //    else if((!ui->radioButtonL11->isChecked()) && (ui->radioButtonR11->isChecked()) && (!ui->radioButtonLR11->isChecked()))//"1:1 开始左手match"
    //    {
    if(QString::compare(featureR,"NULL",Qt::CaseInsensitive) == 0)
    {
        QMessageBox::warning(this,"warning","该人员没有右手掌特征信息");
        return false;
    }
    //    }

    return true;
}

//读取AuxDev::StatusInfo数据
void ControlForm::ReadAuxDevStatusInfo(AuxDev::StatusInfo _StatusInfo, OVSInformation ImgInfo, QString &str, bool bSQLSaveImg)
{
    QString str1,str2,str3,str4,str5,str6;
    if(true == _StatusInfo.a1_palm.success)
        str1 = "TRUE";
    else
        str1 = "FALSE";

    if(true == _StatusInfo.a2_finger.success)
        str2 = "TRUE";
    else
        str2 = "FALSE";

    if(true == _StatusInfo.a3_roi.success)
        str3 = "TRUE";
    else
        str3 = "FALSE";

    if(true == _StatusInfo.a4_enhance.success)
        str4 = "TRUE";
    else
        str4 = "FALSE";

    if(true == _StatusInfo.a5_fcompute.success)
        str5 = "TRUE";
    else
        str5 = "FALSE";

    if(true == _StatusInfo.a6_1fetturesucc.success)
        str6 = "TRUE";
    else
        str6 = "FALSE";

    QString srOut1 = "a1_palm         success:" + str1 + " | " + "time_consume:" + QString::number(_StatusInfo.a1_palm.time_consume,'g',6) + "\n";
    QString srOut2 = "a2_finger       success:" + str2 + " | " + "time_consume:" + QString::number(_StatusInfo.a2_finger.time_consume,'g',6) + "\n";
    QString srOut3 = "a3_roi          success:" + str3 + " | " + "time_consume:" + QString::number(_StatusInfo.a3_roi.time_consume,'g',6) + "\n" ;
    QString srOut4 = "a4_enhance      success:" + str4 + " | " + "time_consume:" + QString::number(_StatusInfo.a4_enhance.time_consume,'g',6) + "\n" ;
    QString srOut5 = "a5_fcompute     success:" + str5 + " | " + "time_consume:" + QString::number(_StatusInfo.a5_fcompute.time_consume,'g',6) + "\n" ;
    QString srOut6 = "a6_1fetturesucc success:" + str6 + " | " + "time_consume:" + QString::number(_StatusInfo.a6_1fetturesucc.time_consume,'g',6) + "\n";
    QString srOut7 = "palm_cx:" + QString::number(ImgInfo.palm_cx) + " | " + "palm_cy:" + QString::number(ImgInfo.palm_cy); + "\n";

    //    str = "<u>" + srOut1 + "</u>"  + "\n" + srOut2 + srOut3 + srOut4 + srOut5 + srOut6;
    str = srOut1 + srOut2 + srOut3 + srOut4 + srOut5 + srOut6 + srOut7;

    QString SaveFileName;
    if(g_sdkStatus == E_SDK_STATUS::e_enroll)
    {
        SaveFileName = "Enroll-";
        SaveImage(_StatusInfo,ImgInfo,SaveFileName,bSQLSaveImg);
        WriteTxtFile(str,bSQLSaveImg);
    }

    if((g_sdkStatus == E_SDK_STATUS::e_match) && (true == g_bUpdateImage))
    {
        SaveFileName = "Match-";
        SaveImage(_StatusInfo,ImgInfo,SaveFileName,bSQLSaveImg);
        WriteTxtFile(str,bSQLSaveImg);
    }
}

void ControlForm::AddOneUserDBRealTImeButton(const QModelIndex &index, QAbstractItemModel *Imodel)
{
    //用户id序号
    QModelIndex Iindex = Imodel->index(index.row(),0);
    QVariant datatemp  = Imodel->data(Iindex);
    QString ID         = datatemp.toString();

    if(ID.size() <= 0)
        return;

    //记录选中行的人员姓名
    Iindex = Imodel->index(index.row(),1);
    datatemp = Imodel->data(Iindex);
    QString qsname     = datatemp.toString();

    Iindex = Imodel->index(index.row(),2);
    datatemp = Imodel->data(Iindex);
    QString qsPhoneNum = datatemp.toString();

    ui->lineEdit_id->setText(qsname);
    ui->lineEditPhoneNum->setText(qsPhoneNum);

    if(qsname.isEmpty())
    {
        QMessageBox::warning(this,"warning","姓名不能为空。");
        return;
    }

    if(qsname.contains(" "))
    {
        QMessageBox::warning(this,"warning","姓名不能包含空格.");
        return;
    }

    if(qsPhoneNum.isEmpty())
    {
        QMessageBox::warning(this,"warning","手机号不能为空");
        return;
    }

    if(qsPhoneNum.contains(" "))
    {
        QMessageBox::warning(this,"warning","手机号不能包含空格.");
        return;
    }

    ui->labelLResult->setText("未完成");
    ui->labelMResult->setText("未完成");
    ui->labelHResult->setText("未完成");
    ui->labelMLResult->setText("未完成");
    ui->labelMHResult->setText("未完成");
    ui->labelLResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelMResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelHResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelMLResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelMHResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelDisInfo->setText("近：40-60,\n"
                              "中：60-68,68-76,76-85,\n"
                              "远：85-115");

    qDebug()<<"EnrollFinishStopEnroll bAddPalmR = true";
    bAddPalmR   = true;
    g_sdkStatus = E_SDK_STATUS::e_enroll;
    ThreadHandler::instance()->ThreadStart();


    _timerId              = startTimer(timerInterval);
    iRevExecuteResultCout = 0;
}

void ControlForm::UpdateButtonEnable(bool bEnable)
{
    ui->pushButtonStartInit->setEnabled(bEnable);
    ui->pushButtonCancelInit->setEnabled(!bEnable);
    ui->pushButtonStartEnroll->setEnabled(!bEnable);
    ui->pushButtonStopEnroll->setEnabled(!bEnable);
    ui->pushButton_startMatchCon->setEnabled(!bEnable);
    ui->pushButtonStopMatch->setEnabled(!bEnable);
}

void ControlForm::EnrollFinishStopEnroll()
{
    qDebug()<<"EnrollFinishStopEnroll bAddPalmR = false";
    iRevExecuteResultCout = 0;

    float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;

    ui->labelLResult->setText("");
    ui->labelMResult->setText("");
    ui->labelHResult->setText("");
    ui->labelMLResult->setText("");
    ui->labelMHResult->setText("");
    ui->labelDisInfo->setText("");
    ui->labelWidgetImage_2->setText("");

    ThreadHandler::instance()->ThreadStop();
    //清空上次注册未成功时遗留的数据
    ThreadAlgo::instance()->ClearEnrollData();
}

void ControlForm::ShowFeaturePic(bool bisShow)
{
    if(bisShow)
    {    QImage img;
        QImage scaledimg;
        img = QImage(m_StatusInfo.image_roi.data,128,
                     128,QImage::Format_Grayscale8);
        scaledimg = img.scaled(128,128,Qt::KeepAspectRatio);
        ui->labelPicROI->setPixmap(QPixmap::fromImage(scaledimg));

        img = QImage(m_StatusInfo.image_wnd.data,m_StatusInfo.image_wnd.rows,
                     m_StatusInfo.image_wnd.cols,QImage::Format_RGB888);
        scaledimg = img.scaled(ui->labelPicWND->width(),ui->labelPicWND->height(),Qt::IgnoreAspectRatio);
        ui->labelPicWND->setPixmap(QPixmap::fromImage(scaledimg));
    }
    else
    {
        ui->labelPicROI->clear();
        ui->labelPicWND->clear();
    }

}

//iPalmType:   1(左手)   2(右手)    0(无效)
void ControlForm::SaveInfo(const QString phone, const int hanming,
                           const int iPalmType, bool bSucc, cv::Mat &_MatchSuccMat)
{
    QDateTime da_time;
    QString FileName,FilePath,FilePathData,SaveFileName,qPhone,strImg,time_str,strfile;

    qPhone       = ui->lineEditPhoneNumMatch->text();

    SaveFileName = "Match-";
    FileName     = SaveFileName + qPhone;
    FilePath     = QDir::currentPath() + "/" +RunDataMatch + "/" + FileName;
    time_str     = da_time.currentDateTime().toString("yyyy-MM-dd HH:mm:ss ");

    QString strTempPalm,strMatchPalm;
    if(1 == iPalmType)
        strTempPalm = "L";
    else if(2 == iPalmType)
        strTempPalm = "R";

    qDebug()<<"QString::compare(qPhone,phone,Qt::CaseInsensitive) = "
           <<QString::compare(qPhone,phone,Qt::CaseInsensitive)<<",qPhone = "<<qPhone<<",phone = "<<phone;

    bool bMatchPalmL = ui->radioButtonMatchL->isChecked();
    bool bMatchPalmR = ui->radioButtonMatchR->isChecked();

    if(bMatchPalmL)
        strMatchPalm = "L";
    else
        strMatchPalm = "R";

    if((QString::compare(qPhone,phone,Qt::CaseInsensitive) == 0))//同一人
    {
        QString strResult;

        if((QString::compare(strTempPalm,strMatchPalm,Qt::CaseInsensitive) == 0))//同一张手
        {
            FilePathData = FilePath + "/" + RunDataMatchG;
            strfile = FilePathData + "/" + "GenuineInfo.txt";
            strImg  = FilePathData + "/" + "[time]:" + time_str + "-[Match]:" + qPhone + "-[MPalm]:" + strMatchPalm + "-[Temp]:" +
                    phone + "-[TPalm]:" + strTempPalm + "-[score]:" + QString::number(hanming);
        }
        else//不同张手
        {
            FilePathData = FilePath + "/" + RunDataMatchI;
            strfile = FilePathData + "/" + "ImposterInfo.txt";
            strImg  = FilePathData + "/" + "[time]:" + time_str + "-[Match]:" + qPhone + "-[MPalm]:" + strMatchPalm + "-[Temp]:" +
                    phone + "-[TPalm]:" + strTempPalm + "-[score]:" + QString::number(hanming);
        }

        qDebug()<<"bSucc = "<<bSucc<<",strImg = "<<strImg;
        if(true == bSucc)//识别成功
        {
            strResult = "true";
            strImg = strImg + ".png";
            //保存图像
            if(!bSucc)
            {
                _mutex.lock();
                cv::imwrite(strImg.toStdString(),g_matRaw);
                _mutex.unlock();
            }
            else
            {
                _mutex.lock();
                if(!_MatchSuccMat.empty())
                    cv::imwrite(strImg.toStdString(),_MatchSuccMat);
                else
                    qDebug()<<"imwrite 图像为空";

                _mutex.unlock();
            }
        }
        else
            strResult = "false";

        if((!bSucc) && ((QString::compare(strTempPalm,strMatchPalm,Qt::CaseInsensitive) == 0)))//识别不成功且不是同一个人的同一张手，保存至暂存区
            ListTimeoutInfo.append(strImg);
        else
        {
            //保存信息至txt文件
            QFile file(strfile);
            file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
            strImg = strImg + "-" + strResult + "\n";
            QByteArray strByte = strImg.toLatin1();
            file.write(strByte);
            file.close();
        }

//        ListTimeoutInfo.append(strImg);
    }
    else//不同人
    {
        QString strResult;

        FilePathData = FilePath + "/" + RunDataMatchI;
        strfile = FilePathData + "/" + "ImposterInfo.txt";
        strImg  = FilePathData + "/" + "[time]:" + time_str + "-[Match]:" + qPhone + "-[MPalm]:" + strMatchPalm + "-[Temp]:" + phone +
                "-[TPalm]:" + strTempPalm + "-[score]:" + QString::number(hanming);

        if(true == bSucc)
        {
            strResult = "true";
            strImg = strImg + ".png";
            //保存图像
            _mutex.lock();
            cv::imwrite(strImg.toStdString(),g_matRaw);
            _mutex.unlock();
        }
        else
            strResult = "false";

        //保存信息至txt文件
        QFile file(strfile);
        file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
        strImg = strImg + "-" + strResult + "\n";
        QByteArray strByte = strImg.toLatin1();
        file.write(strByte);
        file.close();
    }
}

void ControlForm::SaveTemplate(const string &str, string file)
{
    Vtrf VtrfTmp;

    Global::instance()->str2VtrTemplate(str,VtrfTmp);

    std::ofstream ofs(file);
    int n = 0;
    for (auto v : VtrfTmp)
    {
        ofs << v << " ";
        ++n;
        if (n % (2 + 128) == 0)
            ofs << endl;
    }
}

//保存图像
void ControlForm::SaveImage(const AuxDev::StatusInfo &Init, OVSInformation ImgInfo, QString SaveFileName, bool bSQLSaveImg)
{
    QString FileName,FilePath,FilePathData,qPhone,qPhoneTmp;

    qPhone       = ui->lineEditPhoneNum->text();
    FilePath     = QDir::currentPath();
    qPhoneTmp    = qPhone;
    if(g_sdkStatus == E_SDK_STATUS::e_enroll)
    {
        if(ui->checkBoxSaveImage->isChecked())
        {
            if(qPhone.contains("L") || qPhone.contains("l") ||
               qPhone.contains("R") || qPhone.contains("r"))
            {
                int iIndex = qPhone.lastIndexOf("-");
                qPhone = qPhone.left(iIndex);
            }
        }

        FileName     = SaveFileName + qPhone;
        qDebug()<<"SaveImage FileName = "<<FileName;

        if(ui->checkBoxSaveImage->isChecked())
        {
            FilePathData = FilePath + "/" + RunDataEnroll2;
            CreateFolder(FilePath,RunDataEnroll2);
        }
        else
        {
            FilePathData = FilePath + "/" + RunDataEnroll;
            CreateFolder(FilePath,RunDataEnroll);
        }

    }
    else if(g_sdkStatus == E_SDK_STATUS::e_match)
    {
        FileName     = SaveFileName + "MatchUser";
        FilePathData = FilePath + "/" + RunDataMatch;
        CreateFolder(FilePath,RunDataMatch);
    }

    CreateFolder(FilePathData,FileName);

    iRevExecuteResultCout++;

    QString str1Roi,str1Wnd,str1Raw;
    if(g_sdkStatus == E_SDK_STATUS::e_enroll)
    {
        if(!ui->checkBoxSaveImage->isChecked())
        {
            if(!bAddPalmR)
            {
                str1Roi = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-L-Roi" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) +".png";
                str1Wnd = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-L-Wnd" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) + ".png";
                str1Raw = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-L-Raw" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) + ".png";
            }
            else
            {
                str1Roi = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-R-Roi" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) +".png";
                str1Wnd = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-R-Wnd" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) + ".png";
                str1Raw = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-R-Raw" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) + ".png";
            }
        }
        else
        {
            if(qPhoneTmp.contains("-l") || qPhoneTmp.contains("-L"))
            {
                str1Roi = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-L-Roi" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) +".png";
                str1Wnd = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-L-Wnd" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) + ".png";
                str1Raw = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-L-Raw" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) + ".png";
            }
            else
            {
                str1Roi = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-R-Roi" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) +".png";
                str1Wnd = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-R-Wnd" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) + ".png";
                str1Raw = FilePathData + "/" + FileName + "/" + QString::number(iRevExecuteResultCout) + "-R-Raw" +
                        "-" + QString::number(bSQLSaveImg) + "-" + QString::number(ImgInfo.iDistance) + ".png";
            }
        }
    }
//    else if(g_sdkStatus == E_SDK_STATUS::e_match)
//    {
//        str1Roi = FilePathData + "/" + FileName + "/" +
//                QString::number(iRevExecuteResultCout) + "-Roi" + "-" +".png";
//        str1Wnd = FilePathData + "/" + FileName + "/" +
//                QString::number(iRevExecuteResultCout) + "-Wnd" + "-" + ".png";
//        str1Raw = FilePathData + "/" + FileName + "/" +
//                QString::number(iRevExecuteResultCout) + "-Raw" + "-" + ".png";
//    }

    if(g_sdkStatus == E_SDK_STATUS::e_enroll)
    {
        cv::imwrite(str1Raw.toStdString(),Init.image_raw);

        if(!Init.image_roi.empty())
            cv::imwrite(str1Roi.toStdString(),Init.image_roi);
        if(!Init.image_wnd.empty())
            cv::imwrite(str1Wnd.toStdString(),Init.image_wnd);
    }
//    else if(g_sdkStatus == E_SDK_STATUS::e_match)
//    {
//        cv::imwrite(str1Raw.toStdString(),Init.image_raw);
//    }
}

//创建文件夹
bool ControlForm::CreateFolder(QString FilePath, QString FileName)
{
    int ret = 0;
    QString NewFilePath;

    NewFilePath = FilePath + "/" + FileName;

    QDir dir;
    if(!dir.exists(FilePath))
        ret = dir.mkdir(FilePath);

    if(!dir.exists(NewFilePath))
        ret = dir.mkdir(NewFilePath);

    return ret;
    //    qDebug()<<"ControlForm::CreateTxtFile NewFilePath = "<<NewFilePath<<",ret = "<<ret ;
}

//读取文件夹下所有文件
bool ControlForm::ReadAllFileFromFolder(QString FilePath)
{
    QDir dir(FilePath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList AllUserInfoList = dir.entryInfoList();
    //    std::cout << "     Bytes Filename" << std::endl;
//    qDebug()<<"ReadAllFileFromFolder AllUserInfoList.size() = "<<AllUserInfoList.size();
    for (int i = 0; i < AllUserInfoList.size(); ++i)
    {
        QFileInfo fileInfo  = AllUserInfoList.at(i);
        QString strUserInfo = fileInfo.fileName();
//        qDebug()<<"ControlForm::ReadAllFileFromFolder strUserInfo = "<<strUserInfo;

//        QString ChangeFilePath = FilePath + "/" + strUserInfo;
        ChangeFileName(FilePath,strUserInfo);

//        if(false == ret)
//            return false;
    }

    return true;
}

//改变文件名称
bool ControlForm::ChangeFileName(QString FilePath, QString OldFileName)
{
//    qDebug()<<"ChangeFileName FilePath = "<<FilePath;

    QString ChangeFilePath = FilePath + "/" + OldFileName;
    QFile file(ChangeFilePath);

    QStringList qStringList = OldFileName.split("-");

//    qDebug()<<"11 qStringList.size() = "<<qStringList.size()<<",OldFileName = "<<OldFileName;
    for (int i = 0; i < qStringList.size(); ++i)
    {
        QString str = qStringList.at(0);
        if((str.toInt() != g_ovsInfo.iMatchImgNumSucc))
            return false;
        else
        {
            QStringList qStringListTmp = OldFileName.split(".");

            QString NewFileName = qStringListTmp.at(0) + QString::number(1) + "." + qStringListTmp.at(1);
            bool ret = file.rename(FilePath + "/" + NewFileName);

//            qDebug()<<"ret = "<<ret<<",NewFileName = "<<NewFileName;
            return true;
        }
    }

    return false;
}

//将注册信息写入txt文件
bool ControlForm::WriteTxtFile(const QString &Info, bool bSQLSaveImg)
{
    QString   FilePath,Filename,time_str,str;
    QDateTime da_time;
    QString SaveInfo = Info;

    if(g_sdkStatus == E_SDK_STATUS::e_enroll)
    {
        if(ui->checkBoxSaveImage->isChecked())
        {
            Filename   = ui->lineEditPhoneNum->text();
            int iIndex = Filename.lastIndexOf("-");
            Filename   = Filename.left(iIndex);
            FilePath   = QDir::currentPath() + "/" + RunDataEnroll2;
        }

        else
        {
            Filename = ui->lineEditPhoneNum->text();
            FilePath = QDir::currentPath() + "/" + RunDataEnroll;
        }
    }
    else if(g_sdkStatus == E_SDK_STATUS::e_match)
    {
        Filename = "MatchUser";
        FilePath = QDir::currentPath() + "/" + RunDataMatch;
    }

    str      = FilePath + "/" + Filename + ".txt";
    time_str = da_time.currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz ");

    QFile file(str);
    //写入内容,这里需要转码，否则报错。
    if(g_sdkStatus == E_SDK_STATUS::e_match)
    {
        //存在打开，不存在创建
        file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);

        QString strTmp;
        strTmp   = time_str + "--" + QString::number(bSQLSaveImg) + "\n";
        SaveInfo = strTmp + SaveInfo;
        SaveInfo = SaveInfo + "\n\n";
    }
    else if(g_sdkStatus == E_SDK_STATUS::e_enroll)
    {
        //存在打开，不存在创建
        file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);

        QString strTmp;
        strTmp   = time_str + "-" + QString::number(iRevExecuteResultCout) +
                              "-" + "\n";
        SaveInfo = strTmp + SaveInfo;
        SaveInfo = SaveInfo + "\n\n";
    }

    QByteArray strByte = SaveInfo.toLatin1();
    //写入QByteArray格式字符串
    file.write(strByte);
    file.close();

    return true;
}

//bool bAllFile:   true:删除所有文件夹    false:删除某个文件夹
bool ControlForm::DeleteFile(QString Path, bool bAllFile)
{
    QString FilePatTmp;
    FilePatTmp = Path;

    QDir dir(Path);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    if(true == bAllFile)
    {
        QFileInfoList AllUserInfoList = dir.entryInfoList();

//        qDebug()<<"1111 Path = "<<Path<<",AllUserInfoList.size() = "<<AllUserInfoList.size();

        for (int i = 0; i < AllUserInfoList.size(); ++i)
        {
            QFileInfo fileInfo  = AllUserInfoList.at(i);
            QString strUserInfo = fileInfo.fileName();

//            qDebug()<<"strUserInfo  = "<<strUserInfo;
            FilePatTmp = FilePatTmp + "/" + strUserInfo;
//            qDebug()<<"FilePatTmp = "<<FilePatTmp;
            QFileInfo FileInfo(FilePatTmp);

            if (FileInfo.isFile())//如果是文件
            {
                QFile::remove(FilePatTmp);

                FilePatTmp.clear();
                FilePatTmp = Path;
            }
        }
    }
    else
    {
        QString strUserInfo = TableViewSelPhone + ".txt";
        FilePatTmp = FilePatTmp + "/" + strUserInfo;

        QFileInfo FileInfo(FilePatTmp);
        if (FileInfo.isFile())//如果是文件
            QFile::remove(FilePatTmp);
    }

    return true;
}

//删除文件夹
bool ControlForm::DeleteFolder(QString path)
{
    QDir dir(path);

    if(!dir.exists()){
        return true;
    }

    return dir.removeRecursively(); // 删除文件夹
}

//1:近模板  2:中近模板  3:中模板  4:中远模板  5:远模板
void ControlForm::slotRevFeatureResult(int iWhichFeature)
{
    switch (iWhichFeature) {
    case 1:
        ui->labelLResult->setText("已完成");
        ui->labelLResult->setStyleSheet("color: rgb(0,255,0);");
        break;
    case 2:
        ui->labelMLResult->setText("已完成");
        ui->labelMLResult->setStyleSheet("color: rgb(0,255,0);");
        break;
    case 3:
        ui->labelMResult->setText("已完成");
        ui->labelMResult->setStyleSheet("color: rgb(0,255,0);");
        break;
    case 4:
        ui->labelMHResult->setText("已完成");
        ui->labelMHResult->setStyleSheet("color: rgb(0,255,0);");
        break;
    case 5:
        ui->labelHResult->setText("已完成");
        ui->labelHResult->setStyleSheet("color: rgb(0,255,0);");
        break;
    case 6:
        EnrollFinishStopEnroll();
        break;
    default:
        break;
    }
}

//此处输出txt，图像，打印日志消耗100ms
void ControlForm::slotRevExecuteResult(const AuxDev::StatusInfo _StatusInfo, OVSInformation ImgInfo, bool bSQLSaveImg)
{
//        qDebug()<<"recv sigExecuteResult";
    m_StatusInfo = _StatusInfo;

    if(ui->checkBoxShowProgressPic->isChecked() && ImgInfo.is_feature_ok)
        ShowFeaturePic(true);

    QString str,time_str;
    QDateTime da_time;

    time_str = da_time.currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz ");

    ReadAuxDevStatusInfo(_StatusInfo,ImgInfo,str,bSQLSaveImg);

    ui->textBrowserLog->append(time_str);
    ui->textBrowserLog->append(str);
    ui->textBrowserLog->append("");
}

//比对超时定时器
void ControlForm::slot_Timerout()
{
    qDebug()<<"slot_Timerout ListTimeoutMat.size() = "<<ListTimeoutMat.size()<<",ListTimeoutInfo.size() = "<<ListTimeoutInfo.size();

    g_bUpdateImage   = false;
    bStartCoutTime   = false;
    NewMatch_Status  = MATCH_STATUS::e_MatchSucc;

    if(pMatchTimeout->isActive())
        pMatchTimeout->stop();

    QDateTime da_time;
    QString FileName,FilePath,SaveFileName,qPhone,strImg,time_str,strfile,FilePathData;

    qPhone       = ui->lineEditPhoneNumMatch->text();
    SaveFileName = "Match-";
    FileName     = SaveFileName + qPhone;
    FilePath     = QDir::currentPath() + "/" +RunDataMatch + "/" + FileName;
    time_str     = da_time.currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz ");

    QString info = QString::number(conMatchTimeout / 1000) + "s比对超时" + ",等待存储数据";
    QMessageBox::warning(this,"warning",info);

    if(ListTimeoutMat.size() == ListTimeoutInfo.size())
    {
        qDebug()<<"超时 ListTimeoutMat.size() == ListTimeoutInfo.size()";
    }
    else
    {

        qDebug()<<"超时 ListTimeoutMat.size() < ListTimeoutInfo.size()";

        int j = 0;
        for(int i = 0;i < ListTimeoutInfo.size();i++)
        {
            strImg = ListTimeoutInfo.at(i);

            //                qDebug()<<"strImg = "<<strImg<<",strImg.contains(RunDataMatchI) = "<<strImg.contains(RunDataMatchI)<<",i = "<<i;
            //创建超时文件夹
            QString TimeOutFile = "TimeOut-" + time_str;
            FilePathData = FilePath + "/" + RunDataMatchG + "/" + TimeOutFile;
            QDir dir;
            if(!dir.exists(FilePathData))
                dir.mkdir(FilePathData);

            if(!strImg.contains(RunDataMatchI))//同一个人不同手，属于imposter
            {
                strImg.replace(QString("\n"), QString(""));
                strfile = strImg + "-" + QString::number(j) +".png";
                int iIndex = strfile.indexOf(RunDataMatchG) + sizeof("Genuine");
                strfile.insert(iIndex,(TimeOutFile + "/"));
                cv::imwrite(strfile.toStdString(),ListTimeoutMat.at(j));
//                qDebug()<<"strfile = "<<strfile<<"Genuine index = "<<strfile.indexOf(RunDataMatchG)<<",bWrite = "<<bWrite;
                j++;
            }

            //保存信息至txt文件
            strfile = FilePathData + "/" + "GenuineInfo.txt";
            QFile file(strfile);
            file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
            strImg = strImg + "-" + "false" + "\n";
            QByteArray strByte = strImg.toLatin1();
            file.write(strByte);
            file.close();
        }

        qDebug()<<"j = "<<j;
    }


//    //保存图像
//    for(int i = 0;i < ListTimeoutMat.size();i++)
//    {
//        strImg  = FilePathData + "/" + time_str + "-Match:" + qPhone + "-Temp:" + qPhone +
//                "-Palm:" + "NULL" + "-score:-1"+ "-" + QString::number(i) + ".png";
//        cv::imwrite(strImg.toStdString(),ListTimeoutMat.at(i));
//    }

//    //保存信息至txt文件
//    QFile file(strfile);
//    file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
//    for(int i = 0;i < ListTimeoutInfo.size();i++)
//    {
//        strImg = ListTimeoutInfo.at(0);
//        QByteArray strByte = strImg.toLatin1();
//        file.write(strByte);
//    }
//    file.close();

    QMessageBox mess(QMessageBox::Information, "提醒:","数据存储完毕");
    QPushButton *button1      = (mess.addButton(tr("继续比对"), QMessageBox::AcceptRole));
    QPushButton *buttonCancel = (mess.addButton(tr("取消比对"), QMessageBox::RejectRole));
    mess.exec();

    ListTimeoutMat.clear();
    ListTimeoutInfo.clear();

    if (mess.clickedButton() == button1)//点击添加按钮
    {
        if(pMatchTimeout->isActive())
            pMatchTimeout->stop();

        ListTimeoutMat.clear();
        ListTimeoutInfo.clear();

    }
    else
    {
       on_pushButtonStopMatch_clicked();
    }

}

void ControlForm::on_pushButtonStartInit_clicked()
{
    int ret = InitFeatureSDK();

    if(true == ret)
    {
        ui->labelDevInitState->setText("设备初始化成功");
        ui->labelDevInitState->setStyleSheet("color: rgb(0,255,0);");

        UpdateButtonEnable(false);
    }
    else
    {
        ui->labelDevInitState->setText("设备初始化失败");
        ui->labelDevInitState->setStyleSheet("color: rgb(255,0,0);");
    }
}

//人员列表界面点击->提取选中某人
void ControlForm::on_tableViewUsetList_clicked(const QModelIndex &index)
{
    qDebug()<<"g_sdkStatus = "<<g_sdkStatus;
    if(E_SDK_STATUS::e_suspend != g_sdkStatus)
        return;

#ifdef DebugInfo
    qDebug()<<"PeopleListForm index.row() = "<<index.row()<<",index.column() = "<<index.column();
#endif
    QAbstractItemModel *Imodel=ui->tableViewUsetList->model();
    //用户id序号
    QModelIndex Iindex = Imodel->index(index.row(),0);
    QVariant datatemp  = Imodel->data(Iindex);
    QString ID         = datatemp.toString();

    if(ID.size() <= 0)
        return;

    //记录选中行的人员姓名
    Iindex = Imodel->index(index.row(),1);
    datatemp = Imodel->data(Iindex);
    QString qsName = datatemp.toString();
    TableViewSelRow = index.row();
    TableViewSelName = qsName;
    //记录选中行的电话号码
    Iindex = Imodel->index(index.row(),2);
    datatemp = Imodel->data(Iindex);
    QString qsPhone = datatemp.toString();
    TableViewSelPhone = qsPhone;

    ui->lineEditPhoneNumMatch->setText(TableViewSelPhone);

    qDebug()<<"on_tableViewUsetList_clicked TableViewSelPhone = "<<TableViewSelPhone;
}

void ControlForm::on_tableViewUsetList_doubleClicked(const QModelIndex &index)
{
    qDebug()<<"doubleClicked g_sdkStatus = "<<g_sdkStatus;
    if(E_SDK_STATUS::e_suspend != g_sdkStatus)
    {
        QMessageBox box(QMessageBox::Warning,"INFO","请先停止比对或者录入过程");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }

#ifdef DebugInfo
    qDebug()<<"doubleClicked PeopleListForm index.row() = "<<index.row()<<",index.column() = "<<index.column();
#endif
    QAbstractItemModel *Imodel=ui->tableViewUsetList->model();
    //用户id序号
    QModelIndex Iindex = Imodel->index(index.row(),0);
    QVariant datatemp  = Imodel->data(Iindex);
    QString ID         = datatemp.toString();

    if(ID.size() <= 0)
        return;

    //记录选中行的人员姓名
    Iindex = Imodel->index(index.row(),1);
    datatemp = Imodel->data(Iindex);
    QString qsName = datatemp.toString();
    TableViewSelRow = index.row();
    TableViewSelName = qsName;
    //记录选中行的电话号码
    Iindex = Imodel->index(index.row(),2);
    datatemp = Imodel->data(Iindex);
    QString qsPhone = datatemp.toString();
    TableViewSelPhone = qsPhone;

    qDebug()<<"on_tableViewUsetList_doubleClicked TableViewSelPhone = "<<TableViewSelPhone;

    Iindex = Imodel->index(index.row(),index.column());
    datatemp = Imodel->data(Iindex);
    QString palmFeature = datatemp.toString();

//    logde<<"palmFeature.size() = "<<palmFeature.size();

    if(palmFeature.compare("NULL") == 0)//特征信息不存在->开始添加DB
    {
        QMessageBox mess(QMessageBox::Information, tr("提醒"), tr("是否开始添加用户特征信息"));
        QPushButton *button1      = (mess.addButton(tr("添加"), QMessageBox::AcceptRole));
        QPushButton *buttonCancel = (mess.addButton(tr("取消"), QMessageBox::RejectRole));
        mess.exec();

        if (mess.clickedButton() == button1)//点击添加按钮
        {
            AddOneUserDBRealTImeButton(index,Imodel);
        }
    }

}

void ControlForm::on_pushButtonStopMatch_clicked()
{
    iCount              = 0;
    g_sdkStatus         = E_SDK_STATUS::e_suspend;
//    TableViewSelRow     = -1;
    float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;

    killTimer(_timerId);
    ThreadHandler::instance()->ThreadStop();

    if(pMatchTimeout->isActive())
    {
        ListTimeoutMat.clear();
        ListTimeoutInfo.clear();

        pMatchTimeout->stop();
    }

    ui->labelMatchState->setText("提示：未开始比对过程");
    ui->labelMatchState->setStyleSheet("color: rgb(2550,，0);");
}


//开始录入
void ControlForm::on_pushButtonStartEnroll_clicked()
{
    QString qsname     = ui->lineEdit_id->text();
    QString qsPhoneNum = ui->lineEditPhoneNum->text();

    if(qsname.isEmpty())
    {
        QMessageBox::warning(this,"warning","姓名不能为空。");
        return;
    }

    if(qsname.contains(" "))
    {
        QMessageBox::warning(this,"warning","姓名不能包含空格.");
        return;
    }

    if(qsPhoneNum.isEmpty())
    {
        QMessageBox::warning(this,"warning","手机号不能为空");
        return;
    }

    if(qsPhoneNum.contains(" "))
    {
        QMessageBox::warning(this,"warning","手机号不能包含空格.");
        return;
    }

    if(ui->checkBoxSaveImage->isChecked())
    {
        if(!(qsPhoneNum.contains("L") || qsPhoneNum.contains("l") ||
             qsPhoneNum.contains("R") || qsPhoneNum.contains("r")))
        {
            QMessageBox::warning(this,"warning","需要输入左右手掌信息.");
            return;
        }

        qDebug()<<"qsPhoneNum.split().size() = "<<qsPhoneNum.split("-").size();
        if(3 != qsPhoneNum.split("-").size())
        {
            QMessageBox::warning(this,"warning","输入格式不正确.");
            return;
        }
    }

    ui->labelLResult->setText("未完成");
    ui->labelMResult->setText("未完成");
    ui->labelHResult->setText("未完成");
    ui->labelMLResult->setText("未完成");
    ui->labelMHResult->setText("未完成");
    ui->labelLResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelMResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelHResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelMLResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelMHResult->setStyleSheet("color: rgb(255,0,0);");
    ui->labelDisInfo->setText("近：40-60,\n"
                              "中：60-68,68-76,76-85,\n"
                              "远：85-115");

    g_sdkStatus = E_SDK_STATUS::e_enroll;
    ThreadHandler::instance()->ThreadStart();


    _timerId              = startTimer(timerInterval);
    iRevExecuteResultCout = 0;
}

void ControlForm::on_pushButtonStopEnroll_clicked()
{
    bAddPalmR             = false;
    g_sdkStatus           = E_SDK_STATUS::e_suspend;
    iRevExecuteResultCout = 0;

    float fTimerEnreoll = (double)msTimer.nsecsElapsed()/(double)1000000;

    ui->labelLResult->setText("");
    ui->labelMResult->setText("");
    ui->labelHResult->setText("");
    ui->labelMLResult->setText("");
    ui->labelMHResult->setText("");
    ui->labelDisInfo->setText("");
    ui->labelWidgetImage_2->setText("");

    killTimer(_timerId);
    ThreadHandler::instance()->ThreadStop();

    //清空上次注册未成功时遗留的数据
    ThreadAlgo::instance()->ClearEnrollData();
}

void ControlForm::on_pushButton_startMatchCon_clicked()
{
    qDebug()<<"startMatchCon _DataMapVtrf.size = "<<_DataMapVtrf.size();
    QString FilePath,FileName,qsPhoneNum;

    qsPhoneNum = ui->lineEditPhoneNumMatch->text();
    if(qsPhoneNum.isEmpty())
    {
        QMessageBox::warning(this,"warning","手机号不能为空");
        return;
    }

    auto it = _DataMapVtrf.find(qsPhoneNum.toStdString());
    if(it == _DataMapVtrf.end())
    {
        QMessageBox::warning(this,"warning","数据库中此手机号不存在");
        return;
    }

    iCount      = 0;//比对计数器
    g_sdkStatus = E_SDK_STATUS::e_match;
    g_matchType = MATCH_SDK_STATUS::e_1vN;

    ThreadHandler::instance()->ThreadStart();
    _timerId   = startTimer(timerInterval);

    ComboBoxCurrentIndex = ui->comboBoxTemplateType->currentIndex();
    ComboBoxCurrentText = ui->comboBoxTemplateType->currentText();

    FileName     = "Match-" + qsPhoneNum;
    FilePath     = QDir::currentPath() + "/" +RunDataMatch + "/" + FileName;

    qDebug()<<"FilePath = "<< FilePath;
    CreateFolder(FilePath,RunDataMatchI);
    CreateFolder(FilePath,RunDataMatchG);

    if(pMatchTimeout->isActive())
        pMatchTimeout->stop();

    _MatchSuccMap.clear();
    ListTimeoutMat.clear();
    ListTimeoutInfo.clear();

    pMatchTimeout->setSingleShot(true);

#ifdef e_M2M1M_H_LDIFFSORT
    if(0 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_M2M1M_H_L;
    else if(1 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_M2M1M_L_H;
    else if(2 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_L_H_M2M1M;
    else if(3 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_L_M2M1M_H;
    else if(4 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_H_L_M2M1M;
    else if(5 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_H_M2M1M_L;
#endif


#ifdef DIFFMAP
    if(0 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_M2M1MHL;
    else if(1 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_M2M1MH_L;
    else if(2 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_M2M1M_HL;
    else if(3 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_M2M1M_H_L;
    else if(4 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_M2M1_M_HL;
    else if(5 == ComboBoxCurrentIndex)
        iTemplateType = TEMPLATE_TYPE::e_M2M1_M_H_L;

#endif

//    ClearTo_DataMap();
//    //更新内存数据
//    qDebug()<<"before startMatchCon _DataList.size = "<<_DataList.size()
//            <<",_DataMapVtrf.size() = "<<_DataMapVtrf.size();

//    QMap<std::string,QList<Vtrf>>::iterator it;
//    for(it = _DataMapVtrf.begin();it != _DataMapVtrf.end();it++)
//    {
//        qDebug()<<"before phone = "<<QString::fromStdString(it.key())<<",it.value().size() = "<<it.value().size();
//    }

//    AddTo_DataMap(_DataList);

//    qDebug()<<"after startMatchCon _DataList.size = "<<_DataList.size()
//            <<",_DataMapVtrf.size() = "<<_DataMapVtrf.size();

//    for(it = _DataMapVtrf.begin();it != _DataMapVtrf.end();it++)
//    {
//        qDebug()<<"after phone = "<<QString::fromStdString(it.key())<<",it.value().size() = "<<it.value().size();
//    }

    ui->labelMatchState->setText("提示：已开始比对过程");
    ui->labelMatchState->setStyleSheet("color: rgb(0,255，0);");
}

//1:1比对
void ControlForm::on_pushButton_startMatch_clicked()
{

    if(TableViewSelRow < 0)
    {
        QMessageBox::warning(this,"warning","1:1比对需要选中某人");
        return;
    }

//    ReadyForMatchModeResult = ReadyForMatchMode();
//    if(ReadyForMatchModeResult != true)
//        return;

    qDebug()<<"startMatchCon _DataMapVtrf.size = "<<_DataMapVtrf.size();

    g_sdkStatus = E_SDK_STATUS::e_match;
    g_matchType = MATCH_SDK_STATUS::e_1v1;

    ThreadHandler::instance()->ThreadStart();
    _timerId   = startTimer(timerInterval);

}

void ControlForm::on_pushButtonDeleteUser_clicked()
{
    if(g_sdkStatus != E_SDK_STATUS::e_suspend)
    {
        QMessageBox box(QMessageBox::Warning,"INFO","请先停止比对或者录入过程");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }

    if(TableViewSelRow < 0)
    {
        QMessageBox box(QMessageBox::Warning,"INFO","未选中某个人员");
        box.setStandardButtons(QMessageBox::Ok);
        box.exec();

        return;
    }

    QString str = "是否删除" + TableViewSelName + "用户信息";
    QMessageBox mess(QMessageBox::Information, tr("提醒"), str);
    QPushButton *button1      = (mess.addButton(tr("删除"), QMessageBox::ApplyRole));
    QPushButton *buttonCancel = (mess.addButton(tr("取消"), QMessageBox::RejectRole));
    mess.exec();
    if (mess.clickedButton() == button1)//点击删除按钮
    {
        //删除该人员录入时存储的图像
        QString Path1 = QDir::currentPath() + "/" + RunDataEnroll;
        QString Path2 = QDir::currentPath() + "/" + RunDataEnroll + "/" + "Enroll-" + TableViewSelPhone;
        DeleteFile(Path1,false);
        int ret = DeleteFolder(Path2);

        qDebug()<<"删除该人员录入时存储的图像 ret = "<<ret;

        //查找数据库中是否存在该人员
        DataList::iterator iter;
        for(iter = _DataList.begin();iter != _DataList.end();iter++)
        {
            _PairData = *iter;

            QString phoneTmp = _PairData.first;
            qDebug()<<"删除用户 _PairData.first = "<<phoneTmp<<",TableViewSelPhone = "<<TableViewSelPhone;

            if(QString::compare(TableViewSelPhone,phoneTmp) == 0)
            {
                _DataList.erase(iter);
                qDebug()<<"find phone:"<<TableViewSelPhone;
                break;
            }
        }

        //删除_DataMapVtrf中的数据
        auto it = _DataMapVtrf.find(TableViewSelPhone.toStdString());
        if(it != _DataMapVtrf.end())
        {
            qDebug()<<"del _DataMapVtrf it.key() = "<<QString::fromStdString(it.key());
            _DataMapVtrf.remove(TableViewSelPhone.toStdString());
        }


        TableViewSelRow = -1;
        SqlHandler::instance()->delUserInfo(TableViewSelPhone);
        UpdateTableView();
        //        on_pushButtonCancleUsreSelect_clicked();
    }


}

void ControlForm::on_pushButtonDeleteUserAll_clicked()
{
    int iNowCount = 0;

    iNowCount = ui->tableViewUsetList->model()->rowCount();

    //    if(iNowCount <= 0)
    //    {
    //        QMessageBox box(QMessageBox::Warning,"INFO","没有可删除的人员信息");
    //        box.setStandardButtons(QMessageBox::Ok);
    //        box.exec();

    //        return;
    //    }

    if(g_sdkStatus != E_SDK_STATUS::e_suspend)
    {
        QMessageBox box(QMessageBox::Warning,"INFO","请先停止比对或者录入过程");
        box.setStandardButtons (QMessageBox::Ok);
        box.setButtonText (QMessageBox::Ok,QString("确 定"));
        box.exec ();
        return;
    }

    QMessageBox mess(QMessageBox::Information, tr("提醒"), "是否删除所有用户");
    QPushButton *button1      = (mess.addButton(tr("删除"), QMessageBox::ApplyRole));
    QPushButton *buttonCancel = (mess.addButton(tr("取消"), QMessageBox::RejectRole));
    mess.exec();

    //    on_pushButtonCancleUsreSelect_clicked();
    if (mess.clickedButton() == button1)//点击删除按钮
    {
        _DataList.clear();
        //删除程序运行目录下所有注册的保存的图像
        QString strPath = QDir::currentPath()  + "/" + RunDataEnroll;
        QStringList mImgNames;
        QDir dir(strPath);
        if (!dir.exists())
            mImgNames = QStringList("");
        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::Name);
        mImgNames = dir.entryList();
        for (int i = 0; i < mImgNames.size(); ++i)
        {
            QString str = strPath + "/" + mImgNames[i];
            DeleteFolder(str);
        }

        DeleteFile(strPath,true);

        SqlHandler::instance()->delUserInfoAll();
        UpdateTableView();
    }
    else
        return;
}

void ControlForm::on_pushButton_exposure_add_clicked()
{
    iconExposureValue = iconExposureValue + 1;
    ui->label_exposureValue->setText(QString::number(iconExposureValue));
}

void ControlForm::on_pushButton_exposure_sub_clicked()
{
    iconExposureValue = iconExposureValue - 1;
    ui->label_exposureValue->setText(QString::number(iconExposureValue));
}

void ControlForm::on_pushButton_current_add_clicked()
{
    iconCurrentValue = iconCurrentValue + 1;
    ui->label_exposureValue->setText(QString::number(iconCurrentValue));
}

void ControlForm::on_pushButton_current_sub_clicked()
{
    iconCurrentValue = iconCurrentValue - 1;
    ui->label_exposureValue->setText(QString::number(iconCurrentValue));
}

void ControlForm::on_pushButtonExposureConfirm_clicked()
{
    DevHandler::instance()->setExposure(iconExposureValue);
}

void ControlForm::on_pushButtonCurrentConfirm_clicked()
{
    DevHandler::instance()->setLEDCurrent(iconCurrentValue);
}


