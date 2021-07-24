#ifndef DBMATCHFORM_H
#define DBMATCHFORM_H

#include "Global.h"
#include "algorithm/include/aux_utils.h"

#include <QMap>
#include <QWidget>
#include <QSettings>
#include <QFileDialog>
#include <QButtonGroup>

enum E_PalmType
{
    MatchInforPalmL,
    MatchInforPalmR
};

struct MatchInfor
{

    int     Distance;     //图片距离
    int     times;        //次数
    QString UserID;        //用户ID
    QString FilePath;
    QString FileName;
    E_PalmType PalmType;      //0: 左手   1右手
};

namespace Ui {
class DBMatchForm;
}

class DBMatchForm : public QWidget
{
    Q_OBJECT

public:
    explicit DBMatchForm(QWidget *parent = 0);
    ~DBMatchForm();

private slots:
    void on_pushButtonBrowse_clicked();
    void on_pushButtonMatch_clicked();

private:
    bool                      bGOrIFlag;//true:Genuine     false:imposter
    QString                   FlifeUrl,IniFileName;
    QFileInfoList             AllUserInfoList;
    QList<QString>            UserIDList;
    QSettings                *pSetting;
    QButtonGroup             *pButtonGroup;
    AuxDev::StatusInfo        _StatusInfo;
    QMap<QString ,QList<MatchInfor>> MatchInforMap;//key:UserID

    Ui::DBMatchForm *ui;

    void ResetParam();
    void ReadAllFileFromFolder(QString FilePath);
    void ReadAuxDevStatusInfo(AuxDev::StatusInfo _StatusInfo,QString &str);
    void CreateTxtFile(QString FilePath,QString FileName);
//    void CreateTxtFile(QString FilePath,QString FileName,bool re);
    void SaveImage(const AuxDev::StatusInfo &Init,const AuxDev::StatusInfo&Match,
                   QString FilePath,QString FileName1,QString FileName2);
    void WriteTxtFileTimeData(QString FilePath,QString SaveFile,QString FileName,QString Info);
    void WriteTxtFileFARFRRData(QString FilePath,QString SaveFile,QString FileName,
                                QString Info,bool re);
    void WriteTxtFileIGData(QString FilePath,QString SaveFile,QString FileName,
                            QString Info,bool re);

    void DeleteTxtFile(QString FilePath,QString str);
    void DeleteTxtFileFromFile(QString FilePath);
//    void CreateNewFileForGenuine(QString FilePath,QString UserID,QString FileName);
//    bool CopyFileToPath(QString sourceDir ,QString toDir,QString FileName,
//                        bool coverFileIfExist);

    void GetFeatureAndGenuineMatch(QString FilePath,int DistanceL,int DistanceH);
    void GetFeatureAndImposterMatch(QString FilePath,int DistanceL,int DistanceH);
    void GetFeatureAndImposterMatchForSameUser();
    void GetFeatureAndImposterMatchForDiffUser();

    bool DBmatchVerify(const std::string &ovsTemplate, const std::string &feature,
                       MatchInfor InitialMatchInfor,MatchInfor MatchMatchInfor);
    void str2VtrTemplate(const std::string &str,Vtrf &vtr);

    inline void LogOut(QString str, int Level);//日志记录
};

#endif // DBMATCHFORM_H
