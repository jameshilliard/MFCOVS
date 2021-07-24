#ifndef CONTROLFORM_H
#define CONTROLFORM_H

#include "Global.h"
#include "sqlhandler.h"
#include "imageWidget.h"
#include "algorithm/include/aux_utils.h"

#include <QMap>
#include <QTimer>
#include <QWidget>
#include <QButtonGroup>
#include <QStandardItemModel>

//#define DIFFMAP
#define e_M2M1M_H_LDIFFSORT

//typedef vector<float> Vtrf;
//typedef vector<vector<float> > vvtrf;

enum MATCH_STATUS{
    e_Init,
    e_WaitPalm,
    e_StartMatch,
    e_MatchSucc
};

#ifdef e_M2M1M_H_LDIFFSORT
enum TEMPLATE_TYPE{
    e_M2M1M_H_L,//中远中中近-远-近
    e_M2M1M_L_H,//中远中中近-近-远
    e_L_H_M2M1M,//近-远-中远中中近
    e_L_M2M1M_H,//近-中远中中近-远
    e_H_L_M2M1M,//远-近-中远中中近
    e_H_M2M1M_L//远-中远中中近-近
};
#endif

#ifdef DIFFMAP
enum TEMPLATE_TYPE{
    e_M2M1MHL,//中远中中近远近
    e_M2M1MH_L,//中远中中近远-近
    e_M2M1M_HL,//中远中中近-远近
    e_M2M1M_H_L,//中远中中近-远-近
    e_M2M1_M_HL,//中远中-中近-远近
    e_M2M1_M_H_L//中远中-中近-远-近
};
#endif

struct MatchSuccScore
{
    cv::Mat RawMat;
    int hanming;
    int iPalmType;
};

namespace Ui {
class ControlForm;
}

class ControlForm : public QWidget
{
    Q_OBJECT
public:
    explicit ControlForm(QWidget *parent = 0);
    ~ControlForm();

    void previewHandler();
    void ovsFeatureHandler();

signals:
    void sigSendImage(const QImage&,const OVSInformation&);

public slots:
    void slotRevFeatureResult(int iWhichFeature);
    void slotRevExecuteResult(const AuxDev::StatusInfo _StatusInfo,OVSInformation ImgInfo,bool bSQLSaveImg);

private slots:
    void slot_Timerout();
    void on_pushButtonStartInit_clicked();
    void on_pushButtonStopMatch_clicked();
    void on_pushButtonStopEnroll_clicked();
    void on_pushButtonDeleteUser_clicked();
    void on_pushButtonStartEnroll_clicked();
    void on_pushButton_startMatch_clicked();
    void on_pushButton_current_add_clicked();
    void on_pushButton_current_sub_clicked();
    void on_pushButtonDeleteUserAll_clicked();
    void on_pushButton_exposure_add_clicked();
    void on_pushButton_exposure_sub_clicked();
    void on_pushButton_startMatchCon_clicked();
    void on_pushButtonCurrentConfirm_clicked();
    void on_pushButtonExposureConfirm_clicked();
    void on_tableViewUsetList_clicked(const QModelIndex &index);
    void on_tableViewUsetList_doubleClicked(const QModelIndex &index);

protected:
    void timerEvent(QTimerEvent *e);

private:
    float fTimerTotal1;

    int iCount;
    int _timerId;
    int iEnrollTempNum;
    Ui::ControlForm *ui;
    QStandardItemModel *_itemModel;
    QButtonGroup       *_pButtonGroup;

    bool                  bAddPalmR;//添加右手掌静脉信息标志位
    bool                  bStartCoutTime;//注册阶段 计时器(计算一次注册所需时间)是否启动标志位
    int                   iconCurrentValue;
    int                   iconExposureValue;
    int                   iRevExecuteResultCout;//feature提取成功计数器，用于SaveImage()中的图片名称
    int                   TableViewSelRow;//点击TableView时，获取点中行行数
    QString               TableViewSelName;//点击TableView时，获取点中行user name
    QString               TableViewSelPhone;//点击TableView时，获取点中行user phone
    int                   ComboBoxCurrentIndex;
    QString               ComboBoxCurrentText;

    MATCH_STATUS          NewMatch_Status;  //判断当前运行状态，用于删除上次/RunData/Match中所有文件
    TEMPLATE_TYPE         iTemplateType;
    imageWidget          *_imageWidget;
    QTimer               *pMatchTimeout;
    QElapsedTimer         msTimer;     //一次完整时间
    QElapsedTimer         msTimer1;
    static OVSImage       _ovsImage;
    static OVSSTATUS      _ovsStatus;
    static std::string    _ovsFeature;
    static OVSInformation _ovsInfo;
    AuxDev::StatusInfo    m_StatusInfo;
    PairData     _PairData;
    DataList     _DataList;
    QMap<std::string,std::string> _DataMap;//DB数据存入map，用于后续match
    QMap<std::string,QList<Vtrf>> _DataMapVtrf;//DB数据存入map，用于后续match
    QMap<std::string,QList<Vtrf>> _DataMapVtrf1;//DB数据存入map，用于后续match
    QMap<std::string,QList<Vtrf>> _DataMapVtrf2;//DB数据存入map，用于后续match
    QMap<std::string,QList<Vtrf>> _DataMapVtrf3;//DB数据存入map，用于后续match
    QMap<QString,MatchSuccScore>  _MatchSuccMap;

    int InitFeatureSDK();
    int UninitFeatureSDK();

    bool ReadyForMatchMode();
    bool StartSaveEnrollInfo();
    bool StartSaveEnrollInfoToFile();
    bool WriteTxtFile(const QString &Info, bool bSQLSaveImg);
    bool DeleteFile(QString Path,bool bAllFile);
    bool DeleteFolder(QString Path);
    bool CreateFolder(QString FilePath,QString FileName);
    bool ReadAllFileFromFolder(QString FilePath);
    bool ChangeFileName(QString FilePath,QString FileName);

    void InitTableView();
    void UpdateTableView();
    void AddTo_DataMap(DataList);
    void UpdateTo_DataMap(const QString phone,const std::string featureL,const std::string featureR);
    void ClearTo_DataMap();
    void SaveInfo(const QString phone,const int hanming,const int iPalmType,bool bSucc,cv::Mat &_MatchSuccMat);
    void SaveTemplate(const std::string &str, string file);
    void SaveImage(const AuxDev::StatusInfo &Init,OVSInformation ImgInfo,QString FileName, bool bSQLSaveImg);
    void AddTableView1Row(QString name,QString phone,bool PalmL,bool PalmR);
    void UpdateTableView1Row(QString name,QString phone,bool PalmL,bool PalmR);
    void ReadAuxDevStatusInfo(AuxDev::StatusInfo  _StatusInfo,OVSInformation ImgInfo,QString &str,bool bSQLSaveImg);
    void AddOneUserDBRealTImeButton(const QModelIndex &index,QAbstractItemModel *Imodel);
    void UpdateButtonEnable(bool bEnable);

    void EnrollFinishStopEnroll();

    //match使用函数
    void ShowFeaturePic(bool bisShow);
//    void str2VtrTemplate(const string& str,Vtrf&vtr);
    bool matchVerify(int &succHanming,int &failHanming,const Vtrf& ovsTemplate,   const Vtrf &feature);
    bool matchVerify(const string& ovsTemplate, const string &feature);
    bool matchIdentify(const MatchMode emm,const std::string &feature,
                       std::string &userID,int &hanming,int &isPalmL);
    bool matchIdentifyDetail(const QMap<std::string,std::string>& _DataMap,
                             const string &sFeature, string &userID);
    bool matchIdentifyDetail(const QMap<std::string,QList<Vtrf>> _DataMapVtrf,
                             const string &sFeature, string &userID,int &isPalmL,int &hanming);
    OVSSTATUS MatchIdentify(const MatchMode emm,const std::string &feature,
                            std::string &userID,int &hanming,int &isPalmL);

    inline void LogOut(QString str, int Level);//日志记录
};

#endif // CONTROLFORM_H
