#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ovm_200_match.h"
#include "ovm_200_feature.h"
#include "ovm_200_feature_dev.h"
#include "logger.h"
#include "sqlhandler.h"

#include <qdebug.h>
#include <QVBoxLayout>
#include <qmessagebox.h>

const int conImageWidth = 540;
const int conImageHeight = 540;

const int conITimerInterval = 10;
//
const int conTemplateContentSize = 5;
const int conCellTemplateBytes = 2304;
const int conCellTemplateFloatSize = 576;
//

//#define ENABLE_GET_PREVIEW_CALL_BACK

OVS::STATUS      MainWindow::_ovsStatus;
OVS::Image       MainWindow::_ovsImage;
OVS::Information MainWindow::_ovsInfo;
std::string      MainWindow::_ovsFeature;

void callBackNotify(MainWindow *pMW){
    logde<<__func__;
    static MainWindow * spMW = nullptr;
    if(pMW != nullptr){
        spMW = pMW;
        logde<<"set main window.";
        return;
    }

    if(spMW == nullptr){
        logde<<"sp mw is null.";
        return;
    }

    if(OVS::SUCCESS == spMW->_ovsStatus){
        spMW->previewHandler();
    }else if(OVS::STATUS::ERR_PROCESS_NOT_START == spMW->_ovsStatus){
        logde<<"process not start.";
    }else if(OVS::STATUS::ERR_FUNC_MODE_CALL_BACK == spMW->_ovsStatus){
        logde<<"func mod call back.";
    }else if(OVS:: ERR_PREVIEW_IS_EMPTY == spMW->_ovsStatus){
        logde<<"preview is empty.";
    }else{
        logde<<"null.";
    }

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  ,_imageWidget(new ImageWidget)
  ,_softRunType(e_null)
  ,_timer(new QTimer)
{
    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_imageWidget);
    layout->setMargin(0);
    ui->widget->setLayout(layout);
    ui->widget->resize(conImageWidth,
                       conImageHeight);

    ui->pushButton_abortEnroll->setEnabled(false);
    ui->pushButton_abortMatch->setEnabled(false);
    ui->progressBar->setValue(0);
    ui->label_stage->setStyleSheet("background-color:yellow;");

    ui->label_stage->hide();

    connect(this,&MainWindow::sigSendImage,
            _imageWidget,&ImageWidget::slotRevImage);
    connect(_timer,&QTimer::timeout,
            this,&MainWindow::slotTimerOut);

    std::string devId;
    OVS::STATUS  st =  OVS::InitFeatureSDK();
    if(st == OVS::SUCCESS){
        logde<<"demo,init sdk succ";
    }else if(st == OVS::ERR_NO_DEVICE){
        logde<<"demo,init sdk,no device.";
        _timer->stop();
        exit(0);
    }else{
        logde<<"init sdk failed.";
        exit(0);
    }
    //
    OVS::InitMatchSDK(OVS::MatchLevel::lv_3);
    //data
    SqlHandler::instance()->loadData(_stdMap);

    updateIdList();

    //
    readFlashData();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::slotTimerOut()
{
    _ovsStatus = OVS::GetPreviewImage(_ovsImage,_ovsFeature,_ovsInfo);

    previewHandler();
}

void MainWindow::on_pushButton_startEnroll_clicked()
{
    QString qsId = ui->lineEdit_id->text();
    if(qsId.isEmpty()){
        QMessageBox::warning(this,"warning","id is empty.");
        return;
    }

    auto it = _stdMap.find(qsId.toStdString());
    if(it != _stdMap.end()){
        QMessageBox::warning(this,"warning","id was exist.");
        return;
    }
    //
    _softRunType = e_enroll;

    QString id = ui->lineEdit_id->text();
    if(id.isEmpty() || id.isNull()){
        QMessageBox::warning(this,"warning","id is wrong.");
        return;
    }

#ifdef ENABLE_GET_PREVIEW_CALL_BACK
    OVS::STATUS re = OVS::StartFeatureForEnroll(MainWindow::sGetPreview);
#else
    OVS::STATUS re = OVS::StartEnrollMode(nullptr,nullptr);
#endif

    if(re == OVS::STATUS::SUCCESS){

#ifndef ENABLE_GET_PREVIEW_CALL_BACK
        _timer->start(conITimerInterval);
#endif
        ui->pushButton_startEnroll->setEnabled(false);
        ui->pushButton_abortEnroll->setEnabled(true);
    }else if(re == OVS::ERR_NO_DEVICE){
        logde<<"demo,no device...";
        _timer->stop();
    }else{
        qDebug()<<"start failed,"<<re;
    }
}

void MainWindow::on_pushButton_abortEnroll_clicked()
{
    _softRunType = e_null;

    OVS::AbortCapture();

    _timer->stop();

    QImage image;
    emit sigSendImage(image);

    ui->pushButton_startEnroll->setEnabled(true);
    ui->pushButton_abortEnroll->setEnabled(false);

    ui->progressBar->setValue(0);
}

void MainWindow::ovsInfoHandler()
{
    if(E_RUN_TYPE::e_enroll == _softRunType){

        if((_ovsInfo.enroll_stage != _ovsPreInfo.enroll_stage)
                || (_ovsInfo.enroll_progress != _ovsPreInfo.enroll_progress)){
            _ovsPreInfo = _ovsInfo;
        }else{
            return;
        }

        if(_ovsInfo.enroll_stage == 1){
            ui->label_stage->setText("录入阶段一");
        }else if(_ovsInfo.enroll_stage == 2){
            if(_ovsInfo.enroll_progress == 0){
                ui->label_stage->setText("录入阶段二,请移开手掌重新放入");
            }else{
                ui->label_stage->setText("录入阶段二");
            }
        }else if(_ovsInfo.enroll_stage == 3){
            if(_ovsInfo.enroll_progress == 0){
                ui->label_stage->setText("录入阶段三,请移开手掌重新放入");
            }else{
                ui->label_stage->setText("录入阶段三");
            }
            ui->progressBar->setValue(_ovsInfo.enroll_progress);

            if(_ovsInfo.enroll_progress == 100){
                QString id = ui->lineEdit_id->text();
                ui->listWidget_user->addItem(id);

                QMessageBox::information(this,"information","恭喜,录入成功.");
            }
        }

        ui->progressBar->setValue(_ovsInfo.enroll_progress);
    }
}

void MainWindow::ovsFeatureHandler()
{
    if(_ovsInfo.is_feature_ok){
        if(E_RUN_TYPE::e_enroll == _softRunType){
            logde<<"feature size:"<<_ovsFeature.size();

            //display template

#if 0
            vtrArray vaTemplate;
            str2VvtrTemplate(_ovsFeature,vaTemplate);

            for(int i = 0; i < vaTemplate.size(); i++){
                Vtrf vtrfTemplate = vaTemplate.at(i);
                for(int j = 0; j < vtrfTemplate.size();j++){
                    logde<<"demo,template data:"
                        <<i<<"|"
                        <<j<<"|"<<vtrfTemplate.at(j);
                }
            }
#endif
            //

            toolReset();
            //
            QString id = ui->lineEdit_id->text();
            ui->lineEdit_id->clear();

            string idStr = id.toStdString();
            _stdMap.insert(make_pair(idStr,_ovsFeature));

            SqlHandler::instance()->addData(
                        id.toStdString(),
                        _ovsFeature);

            updateIdList();
        }else if(e_match == _softRunType){
            //display feature
            //            Vtrf vtrfFeature;
            //            str2VtrfFeature(_ovsFeature,vtrfFeature);
            //            for(auto& item:vtrfFeature){
            //                logde<<"demo,feature item:"<<item;
            //            }

            //1vN
            string sTargetID;
            int re = OVS::MatchIdentify(
                        OVS::MatchMode::e_left_right,
                        _ovsFeature,sTargetID);

            if(OVS::SUCCESS == re){
                logde<<"1vN succ,id:"<<sTargetID;
            }else{
                logde<<"1vN failed.re:"<<re;
            }


#if 0
            for(auto item:_stdMap){
                string id = (string)item.first;

                string ovsTemplate = (string)item.second;


                //1v1
                int re = OVS::MatchVerify(ovsTemplate,
                                          _ovsFeature);


                if(OVS::SUCCESS == re){
                    logde<<"match succ,-----------------id:"<<id;

                    //                    QMessageBox::warning(this,"warning","match succ.");
                    //                    on_pushButton_abortMatch_clicked();

                    //                    continue;
                    return;
                }else if(OVS::ERR_INVALID_PARAM == re){
                    //                                        logde<<"err invalid param.";
                    continue;
                }else if(OVS::ERR_MATCH_FAILED == re){
                    //                                        logde<<"err match failed.";
                    continue;
                }
            }
#endif
            //            QMessageBox::warning(this,"warning","match failed.");
        }

    }else{
        //        logde<<" feature no ok.----------------------";
    }
}
void MainWindow::str2VvtrTemplate(const string &srcStr,
                                  vvtrf &featureVtr)
{
    if(srcStr.size() < conCellTemplateBytes){
        return;
    }

    featureVtr.clear();
    //item
    Vtrf vtrfTmp;
    for(int i = 0; i < conTemplateContentSize; i++){
        string str = srcStr.substr(0 + i * conCellTemplateBytes ,
                                   conCellTemplateBytes);

        vtrfTmp.clear();
        vtrfTmp.resize(conCellTemplateBytes);

        memcpy(vtrfTmp.data(),str.data(),str.size());

        featureVtr.push_back(vtrfTmp);
    }
}
void MainWindow::str2VtrfFeature(const string &str, Vtrf &vtrf)
{
    int byteLength = str.size();

    vtrf.clear();
    vtrf.resize(byteLength / sizeof(float));

    memcpy(vtrf.data(),str.data(),str.size());
}
void MainWindow::updateIdList()
{
    logde<<"_stdmap size:"<<_stdMap.size();

    ui->listWidget_user->clear();

    for(auto it:_stdMap){
        string sId = it.first;
        ui->listWidget_user->addItem(QString::fromStdString(sId));
        int iRe = OVS::AddUser(sId,it.second,string());
        if(iRe != OVS::SUCCESS){
            logde<<"add user failed.iRe:"<<iRe;
            logde<<"id:"<<sId;
        }
    }
}

void MainWindow::sGetPreview(OVS::STATUS st,OVS::Image& image,
                             std::string& feature,
                             OVS::Information& info)

{
    //    logde<<__func__;

    _ovsStatus = st;
    _ovsImage = image;
    _ovsFeature = feature;
    _ovsInfo = info;

    callBackNotify(nullptr);
}

void MainWindow::previewHandler()
{
    QImage image;
    if(_ovsStatus == OVS::SUCCESS){;
        if(0 !=_ovsImage.dataVtr.size()){

#if 1
            image = QImage(&_ovsImage.dataVtr[0],
                    _ovsImage.width,
                    _ovsImage.height,
                    QImage::Format_RGB888);
#endif

        }else{
            //                        logde<<"image is null.";
        }
#if 0
        image = QImage(_ovsImage.data,
                _ovsImage.width,
                _ovsImage.height,
                QImage::Format_RGB888);
#endif

        emit sigSendImage(image);

        ovsInfoHandler();
        ovsFeatureHandler();
    }else if(_ovsStatus == OVS::ERR_NO_DEVICE){
        _ovsImage.dataVtr.clear();
        logde<<"demo no device.";
        _timer->stop();
        return ;
    }else if(_ovsStatus == OVS::ERR_PREVIEW_IS_EMPTY){
        _ovsImage.dataVtr.clear();
        //        qDebug()<<"preview is empty.";
        return ;
    }else if(_ovsStatus == OVS::ERR_ENROLL_PROCESS_NOT_START){
        _ovsImage.dataVtr.clear();
        qDebug()<<"unstart enroll process.";
        return ;
    }else {
        _ovsImage.dataVtr.clear();
        qDebug()<<"get preview re:"<<_ovsStatus;
        qDebug()<<"get preview image,failed.";
        return;
    }
}

void MainWindow::on_pushButton_startMatch_clicked()
{


#ifdef ENABLE_GET_PREVIEW_CALL_BACK
    int re = OVS::StartFeatureForMatch(MainWindow::sGetPreview);
#else
    int re = OVS::StartMatchMode(nullptr,nullptr);
    _timer->start(conITimerInterval);
#endif

    if(re == OVS::SUCCESS){
        logde<<"start feature for match succ.";

        ui->pushButton_startMatch->setEnabled(false);
        ui->pushButton_abortMatch->setEnabled(true);

        _softRunType = e_match;
    }else if(re == OVS::ERR_NO_DEVICE){
        logde<<"demo,no device.";
    }else{
        logde<<"start feature for match failed.";
        exit(0);
    }
}

void MainWindow::on_pushButton_abortMatch_clicked()
{
    ui->pushButton_startMatch->setEnabled(true);
    ui->pushButton_abortMatch->setEnabled(false);

    _softRunType = e_null;

    OVS::AbortCapture();

    _timer->stop();

    QImage image;
    emit sigSendImage(image);

}

void MainWindow::toolReset()
{
    _softRunType = e_null;

    _timer->stop();

    QImage image;
    emit sigSendImage(image);

    ui->pushButton_startEnroll->setEnabled(true);
    ui->pushButton_abortEnroll->setEnabled(false);

    ui->progressBar->setValue(0);
}

void MainWindow::on_pushButton_write2Flash_clicked()
{
    FeatureDevelopTool *fdt = FeatureDevelopTool::instance();

    QString qsDevName = ui->lineEdit_devName->text();
    string sDevName = qsDevName.toStdString();
    fdt->setDevName(sDevName);

    QString qsSn = ui->lineEdit_sn->text();
    string sSn = qsSn.toStdString();
    fdt->setDevSN(sSn);

    QString qsFwv = ui->lineEdit_firewareVer->text();
    string sFwv = qsFwv.toStdString();
    fdt->setDevFireWareVer(sFwv);

    QString qsHwv = ui->lineEdit_hardwareVer->text();
    string sHwv = qsHwv.toStdString();
    fdt->setDevHardWareVer(sHwv);

    QString qsSwv = ui->lineEdit_softwareVer->text();
    string sSwv = qsSwv.toStdString();
    fdt->setDevSoftWareVer(sSwv);

    FeatureDevelopTool::instance()->write2Flash();
}

void MainWindow::on_pushButton_readFromFlash_clicked()
{
    FeatureDevelopTool::instance()->readFromFlash();
    readFlashData();
}

void MainWindow::readFlashData()
{
    FeatureDevelopTool *fdt = FeatureDevelopTool::instance();

    string sDevName;
    fdt->getDevName(sDevName);

    string sSn;
    fdt->getDevSN(sSn);

    string sFwv;
    fdt->getDevFireWareVer(sFwv);

    string sHwv;
    fdt->getDevHardWareVer(sHwv);

    string sSwv;
    fdt->getDevSoftWareVer(sSwv);

    ui->lineEdit_devName->setText(QString::fromStdString(sDevName));
    ui->lineEdit_sn->setText(QString::fromStdString(sSn));
    ui->lineEdit_firewareVer->setText(QString::fromStdString(sFwv));
    ui->lineEdit_hardwareVer->setText(QString::fromStdString(sHwv));
    ui->lineEdit_softwareVer->setText(QString::fromStdString(sSwv));

}
