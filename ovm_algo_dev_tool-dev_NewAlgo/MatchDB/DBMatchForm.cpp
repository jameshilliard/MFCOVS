#include "Log.h"
#include "DBMatchForm.h"
#include "ui_DBMatchForm.h"
#include "algorithmhandler.h"

#include <QStandardPaths>

const int conCellFeatureBytes = 576;

DBMatchForm::DBMatchForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DBMatchForm)
{
    ui->setupUi(this);

    pButtonGroup = new QButtonGroup;
    pButtonGroup->setExclusive(true);
    pButtonGroup->addButton(ui->radioButtonG);
    pButtonGroup->addButton(ui->radioButtonI);
    ui->radioButtonI->setChecked(true);

}

DBMatchForm::~DBMatchForm()
{
    delete ui;
}

void DBMatchForm::on_pushButtonBrowse_clicked()
{
    QString str = "选择图片路径";

    LogOut(str,LOG_DEBUG);
    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    FlifeUrl = QFileDialog::getExistingDirectory(this, "选择文件",
                                                 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)); //选择路径:默认桌面

    qDebug()<<"on_pushButtonBrowse_clicked FlifeUrl = "<<FlifeUrl;
    ui->lineEditBrowse->setText(FlifeUrl);

    ReadAllFileFromFolder(FlifeUrl);

    //    DeleteTxtFile(FlifeUrl,"/Success");
    //    DeleteTxtFile(FlifeUrl,"/Fail");
    //    DeleteTxtFile(FlifeUrl,"/RunData");
    DeleteTxtFile(FlifeUrl,"/Imposter");
    DeleteTxtFile(FlifeUrl,"/Genuine");
    DeleteTxtFileFromFile(FlifeUrl + "/Genuine");
    DeleteTxtFileFromFile(FlifeUrl + "/Imposter");

    CreateTxtFile(FlifeUrl,  "/Imposter");
    CreateTxtFile(FlifeUrl,  "/Genuine");
    CreateTxtFile(FlifeUrl + "/Genuine", "/Fail");
    CreateTxtFile(FlifeUrl + "/Genuine", "/RunData");
    CreateTxtFile(FlifeUrl + "/Genuine", "/Success");
    CreateTxtFile(FlifeUrl + "/Imposter","/Fail");
    CreateTxtFile(FlifeUrl + "/Imposter","/RunData");
    CreateTxtFile(FlifeUrl + "/Imposter","/Success");
}

void DBMatchForm::on_pushButtonMatch_clicked()
{
    bool bGenuine  = ui->radioButtonG->isChecked();
    bool bImposter = ui->radioButtonI->isChecked();

    memset(&g_eMatchResultCount,0,sizeof(MatchResultCount));

    qDebug()<<"on_pushButtonMatch_clicked bGenuine = "<<bGenuine<<",bImposter = "<<bImposter;
    if(true == bGenuine)
    {
        bGOrIFlag = true;
        GetFeatureAndGenuineMatch(FlifeUrl,0,10);
    }

    if(true == bImposter)
    {
        bGOrIFlag = false;
        GetFeatureAndImposterMatch(FlifeUrl,0,10);
    }

}

//读取文件夹中所有内容
void DBMatchForm::ReadAllFileFromFolder(QString FilePath)
{
    QDir dir(FilePath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    AllUserInfoList = dir.entryInfoList();
    //    std::cout << "     Bytes Filename" << std::endl;
    qDebug()<<"ReadAllFileFromFolder AllUserInfoList.size() = "<<AllUserInfoList.size();
    for (int i = 0; i < AllUserInfoList.size(); ++i)
    {
        //        std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
        //                                .arg(fileInfo.fileName()));
        //        std::cout << std::endl;
        QFileInfo fileInfo  = AllUserInfoList.at(i);
        QString strUserInfo = fileInfo.fileName();

        QStringList strOneUserInfoList = strUserInfo.split("_");

        if(strOneUserInfoList.size() != 5)
        {
            QString str = "格式不符合要求,文件名：" + strUserInfo;
            LogOut("str",LOG_DEBUG);
        }

        MatchInfor MatchInforTmp;

        MatchInforTmp.FileName = strUserInfo;
        MatchInforTmp.FilePath = FilePath + "/" + strUserInfo;
        MatchInforTmp.UserID   = strOneUserInfoList.at(1);
        MatchInforTmp.Distance = QString(strOneUserInfoList.at(3)).toInt();
        MatchInforTmp.times    = QString(strOneUserInfoList.at(4)).toInt();

        if(QString::compare(strOneUserInfoList.at(2),"L",Qt::CaseInsensitive) == 0)
            MatchInforTmp.PalmType = E_PalmType::MatchInforPalmL;
        else
            MatchInforTmp.PalmType = E_PalmType::MatchInforPalmR;

        //判断MatchInforMap中是否已经含有相同的UserID，如果有相同的UserID，则把原来的值取出，在append新值，再放入
        QList<MatchInfor> NewMatchInforList;
        if(MatchInforMap.contains(MatchInforTmp.UserID))
        {
            QList<MatchInfor> OldMatchInforList  = MatchInforMap[MatchInforTmp.UserID];

            NewMatchInforList = OldMatchInforList;
            NewMatchInforList.append(MatchInforTmp);
            MatchInforMap.insert(MatchInforTmp.UserID,NewMatchInforList);
        }
        else
        {
            NewMatchInforList.append(MatchInforTmp);
            MatchInforMap.insert(MatchInforTmp.UserID,NewMatchInforList);
        }

        //        CreateNewFileForGenuine(FilePath,MatchInforTmp.UserID,strUserInfo);
    }


    qDebug()<<"ReadAllFileFromFolder MatchInforMap.size() = "<<MatchInforMap.size();
}

//读取AuxDev::StatusInfo数据
void DBMatchForm::ReadAuxDevStatusInfo(AuxDev::StatusInfo _StatusInfo, QString &str)
{
    //    qDebug()<<"InitialPic a1_palm.time_consume = "<<_StatusInfo.a1_palm.time_consume<<",a1_palm.success = "<<_StatusInfo.a1_palm.success;
    //    qDebug()<<"InitialPic a2_finger.time_consume = "<<_StatusInfo.a2_finger.time_consume<<",a2_finger.success = "<<_StatusInfo.a2_finger.success;
    //    qDebug()<<"InitialPic a5_fcompute.time_consume = "<<_StatusInfo.a5_fcompute.time_consume<<",a5_fcompute.success = "<<_StatusInfo.a5_fcompute.success;

    QString str1,str2,str3,str4,str5;
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

    QString srOut1 = "a1_palm     success:" + str1 + " | " + "time_consume:" + QString::number(_StatusInfo.a1_palm.time_consume,'g',6) + "\n";
    QString srOut2 = "a2_finger   success:" + str2 + " | " + "time_consume:" + QString::number(_StatusInfo.a2_finger.time_consume,'g',6) + "\n";
    QString srOut3 = "a3_roi      success:" + str3 + " | " + "time_consume:" + QString::number(_StatusInfo.a3_roi.time_consume,'g',6) + "\n" ;
    QString srOut4 = "a4_enhance  success:" + str4 + " | " + "time_consume:" + QString::number(_StatusInfo.a4_enhance.time_consume,'g',6) + "\n" ;
    QString srOut5 = "a5_fcompute success:" + str5 + " | " + "time_consume:" + QString::number(_StatusInfo.a5_fcompute.time_consume,'g',6) + "\n" ;


    str = srOut1 + srOut2 + srOut3 + srOut4 + srOut5;
    //    qDebug()<<"srOut = "<<srOut;

}

void DBMatchForm::CreateTxtFile(QString FilePath, QString FileName)
{
    QString NewFilePath;

    NewFilePath = FilePath + "/" + FileName;

    QDir dir;
    if(!dir.exists(NewFilePath))
        dir.mkdir(NewFilePath);
}

#if 0
void DBMatchForm::CreateTxtFile(QString FilePath, QString FileName, bool re)
{
    bool exist;
    QString NewFilePath,NewFile;

    if(true == re)
        NewFilePath = FilePath + "/Success";
    else
        NewFilePath = FilePath + "/Fail";

    QDir dir;
    if(!dir.exists(NewFilePath))
        dir.mkdir(NewFilePath);
}
#endif

void DBMatchForm::SaveImage(const AuxDev::StatusInfo &Init, const AuxDev::StatusInfo &Match,
                            QString FilePath, QString FileName1, QString FileName2)
{
    QString str1Roi = FilePath + "/" + FileName1 + "Roi" + ".png";
    QString str1Wnd = FilePath + "/" + FileName1 + "Wnd" + ".png";
    QString str2Roi = FilePath + "/" + FileName2 + "Roi"  + ".png";
    QString str2Wnd = FilePath + "/" + FileName2 + "Wnd"  + ".png";

    //    qDebug()<<"SaveImage str1 = "<<str1;

    cv::imwrite(str1Roi.toStdString(),Init.image_roi);
    cv::imwrite(str1Wnd.toStdString(),Init.image_wnd);
    cv::imwrite(str2Roi.toStdString(),Init.image_roi);
    cv::imwrite(str2Wnd.toStdString(),Init.image_wnd);
}

//输出比较基准图像和将要对比图像的所耗费时间，记录至文档
void DBMatchForm::WriteTxtFileTimeData(QString FilePath, QString SaveFile,
                                   QString FileName, QString Info)
{
    QString str,NewFilePath;

    NewFilePath = FilePath + "/" + SaveFile;

    //    qDebug()<<"FileName before = "<<FileName;

    QStringList FileNameList = FileName.split(".");
    FileName     = FileNameList.at(0);

    FileNameList = FileName.split("_");
    FileName     = FileNameList.at(1) + "_" + FileNameList.at(2);

    //    qDebug()<<"FileName after1 = "<<FileName;

    str = NewFilePath + "/" + FileName + ".txt";

    QFile file(str);
    //存在打开，不存在创建
    file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
    //写入内容,这里需要转码，否则报错。
    Info = Info + "\n";
    QByteArray strByte = Info.toLatin1();
    //写入QByteArray格式字符串
    file.write(strByte);

    file.close();
}

//输出Genuine、Imposter  match成功或者失败信息至txt文档
void DBMatchForm::WriteTxtFileFARFRRData(QString FilePath, QString SaveFile, QString FileName, QString Info, bool re)
{
    QString str,NewFilePath;

    if(true == re)
        NewFilePath = FilePath + "/" + SaveFile;
    else
        NewFilePath = FilePath + "/" + SaveFile;

    //    qDebug()<<"FileName before = "<<FileName;

    QStringList FileNameList = FileName.split(".");
    FileName     = FileNameList.at(0);

    FileNameList = FileName.split("_");
    FileName     = FileNameList.at(1) + "_" + FileNameList.at(2);

    //    qDebug()<<"FileName after1 = "<<FileName;

    str = NewFilePath + "/" + FileName + ".txt";

    QFile file(str);
    //存在打开，不存在创建
    file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
    //写入内容,这里需要转码，否则报错。
    Info = Info + "\n";
    QByteArray strByte = Info.toLatin1();
    //写入QByteArray格式字符串
    file.write(strByte);

    file.close();
}

//输出Genuine、Imposter  中一次个步骤atch时间至RunData文件夹
void DBMatchForm::WriteTxtFileIGData(QString FilePath, QString SaveFile, QString FileName, QString Info, bool re)
{
    QString str;
    QString NewFilePath;

    if(true == re)
        NewFilePath = FilePath + "/Genuine/RunData";
    else
        NewFilePath = FilePath + "/Imposter/RunData";

    QDir dir;
    if(!dir.exists(NewFilePath))
        dir.mkdir(NewFilePath);


    QStringList FileNameList = FileName.split(".");
    FileName     = FileNameList.at(0);

    str = NewFilePath + "/" + FileName + ".txt";

    QFile file(str);
    //存在打开，不存在创建
    file.open(QIODevice::ReadWrite | QIODevice::Text/* | QIODevice::Append*/);


    QByteArray Qbyte = file.readAll();
//    qDebug()<<"WriteTxtFileIGData str = "<<str<<",Qbyte = "<<Qbyte.size();
    if(Qbyte.size() <= 0)
    {
        //写入内容,这里需要转码，否则报错。
        Info = Info + "\n";
        QByteArray strByte = Info.toLatin1();
        //写入QByteArray格式字符串
        file.write(strByte);
    }

    file.close();
}


//删除文件
void DBMatchForm::DeleteTxtFile(QString FilePath, QString str)
{
    QString FilePatTmp;

    FilePatTmp = FilePath + str;
    QDir dir(FilePatTmp);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    AllUserInfoList = dir.entryInfoList();

    //    qDebug()<<"DeleteTxtFile AllUserInfoList.size() = "<<AllUserInfoList.size();
    for (int i = 0; i < AllUserInfoList.size(); ++i)
    {
        QFileInfo fileInfo  = AllUserInfoList.at(i);
        QString strUserInfo = fileInfo.fileName();

        //        qDebug()<<"strUserInfo  = "<<strUserInfo;
        FilePatTmp = FilePatTmp + "/" + strUserInfo;
        //        qDebug()<<"FilePatTmp = "<<FilePatTmp;
        QFileInfo FileInfo(FilePatTmp);

        if (FileInfo.isFile())//如果是文件
        {
            QFile::remove(FilePatTmp);

            FilePatTmp.clear();
            FilePatTmp = FilePath + str;
        }
    }
}

//删除文件夹中的文件
void DBMatchForm::DeleteTxtFileFromFile(QString mFolderPath)
{
    // 获取所有文件夹名
    QStringList mImgNames;

    QDir dir(mFolderPath);
    mFolderPath = dir.fromNativeSeparators(mFolderPath);//  "\\"转为"/"
    if (!dir.exists())
        mImgNames = QStringList("");
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    mImgNames = dir.entryList();
    for (int i = 0; i < mImgNames.size(); ++i)
    {
        //        qDebug() << "entryList: " << i << "-" << mFolderPath + "/" + mImgNames[i];
        DeleteTxtFile(mFolderPath + "/",mImgNames[i]);
    }
}

//void DBMatchForm::CreateNewFileForGenuine(QString FilePath, QString UserID, QString FileName)
//{
//    QString NewFilePath = FilePath + "/" + UserID;
//    QDir dir;
//    if(!dir.exists(NewFilePath)){
//        dir.mkdir(NewFilePath);
//        UserIDList.append(UserID);
//    }

//    qDebug()<<"CreateNewFileForGenuine UserIDList.size() = = "<<UserIDList.size();
//    //    CopyFileToPath(FilePath,NewFilePath,FileName,false);
//}

//bool DBMatchForm::CopyFileToPath(QString sourceDir, QString toDir, QString FileName, bool coverFileIfExist)
//{
//    sourceDir = sourceDir + "/" + FileName;
//    toDir     = toDir     + "/" + FileName;

//    qDebug()<<"sourceDir = "<<sourceDir<<",toDir = "<<toDir<<",FileName = "<<FileName;
//    if (sourceDir == toDir)
//        return true;

//    if (!QFile::exists(sourceDir))
//        return false;

//    if(!QFile::copy(sourceDir, toDir))
//        return false;

//    return true;
//}

//相同userid  相同手掌类型的比较
void DBMatchForm::GetFeatureAndGenuineMatch(QString FilePath, int DistanceL, int DistanceH)
{
    int iTimes = 0;
    QList<QString>                   UserIDListTmp;
    QMap<QString ,QList<MatchInfor>>::iterator itm;
    QElapsedTimer msTimerExe1;     // 定义对象

    UserIDListTmp    = UserIDList;

    qDebug()<<"GetFeatureAndGenuineMatch MatchInforMap.size() = "<<MatchInforMap.size();

    for(itm = MatchInforMap.begin();itm != MatchInforMap.end();itm++)
    {
        MatchInfor        stMatchInfor,stMatchInforInitial;
        QList<MatchInfor> MatchInforList;
        QList<MatchInfor>::iterator it;

        MatchInforList = (*itm);
        //        qDebug()<<"init strUserID = "<<itm.key();

        for(int i = MatchInforList.size();i > 1;i--)
        {
            QString str;
            std::string   InitalFeature;
            PalmImageInfo InitalpalmImageInfo;
            AuxDev::StatusInfo InitStatusInfo;
            it = MatchInforList.begin();
            stMatchInforInitial = (*it);
            //            qDebug()<<"GetFeatureAndGenuineMatch stMatchInforInitial.UserID   = "<<stMatchInforInitial.UserID;
            //            qDebug()<<"GetFeatureAndGenuineMatch stMatchInforInitial.PalmType = "<<stMatchInforInitial.PalmType;
            //            qDebug()<<"GetFeatureAndGenuineMatch stMatchInforInitial.FilePath = "<<stMatchInforInitial.FilePath;

            //提取比较基准图像并计算feature提取时间
            cv::Mat InitialPic = cv::imread(stMatchInforInitial.FilePath.toStdString());
            msTimerExe1.start();
            AlgorithmHandler::instance()->execute(InitialPic,InitalpalmImageInfo,InitalFeature,_StatusInfo);
            float fTimerExe1 = (double)msTimerExe1.nsecsElapsed()/(double)1000000;

            //提取比较基准图像，并读取该图像提取feature期间每个步骤的时间和图像
            InitStatusInfo = _StatusInfo;
            ReadAuxDevStatusInfo(_StatusInfo,str);
            WriteTxtFileIGData(FlifeUrl,"RunData",stMatchInforInitial.FileName,str,true);

            MatchInforList.erase(it);
            for(it = MatchInforList.begin();it != MatchInforList.end();it++)
            {
                std::string   MatchFeature;
                PalmImageInfo MatchpalmImageInfo;
                stMatchInfor = (*it);

                //                qDebug()<<"IPalmType = "<<stMatchInforInitial.PalmType<<",MPalmType = "<<stMatchInfor.PalmType;
                if(stMatchInforInitial.PalmType != stMatchInfor.PalmType)//不是同一类型手掌不比较
                    continue;

                //提取将要比对的图像并计算提取时间
                cv::Mat MatchPic = cv::imread(stMatchInfor.FilePath.toStdString());
                msTimerExe1.start();
                AlgorithmHandler::instance()->execute(MatchPic,MatchpalmImageInfo,MatchFeature,_StatusInfo);
                float fTimerExe2 = (double)msTimerExe1.nsecsElapsed()/(double)1000000;
                //                qDebug()<<"stMatchInforInitial.FileName = "<<stMatchInforInitial.FileName<<",stMatchInfor.FileName = "<<stMatchInfor.FileName;

                //提取比较基准图像和将要比对的图像所需match的时间
                msTimerExe1.start();
                int ret = DBmatchVerify(InitalFeature,MatchFeature,stMatchInforInitial,stMatchInfor);
                float fTimerExe3 = (double)msTimerExe1.nsecsElapsed()/(double)1000000;

                //输出比较基准图像和将要对比图像的所耗费时间，记录至文档
                QString initStr  = stMatchInforInitial.FileName;
                QString matchStr = stMatchInfor.FileName;
                QString LogOutInit  = initStr + " and " + matchStr + "  " + "Init:" + QString::number(fTimerExe1,'g',6) + " | " +
                        "Match:" + QString::number(fTimerExe2,'g',6) + " | " +"Compare:" + QString::number(fTimerExe3,'g',6);
                WriteTxtFileTimeData(FlifeUrl,"Genuine",stMatchInforInitial.FileName,LogOutInit);
                //                qDebug()<<"fTimerExe1 = "<<fTimerExe1<<",fTimerExe2 = "<<fTimerExe2<<",fTimerExe3 = "<<fTimerExe3;

                //创建GenuineMatch 比对失败的文档，并把对比双方的图像输出至此文档
                QStringList FileNameList = stMatchInforInitial.FileName.split(".");
                QString str  = FileNameList.at(0);
                FileNameList = str.split("_");
                str          = FileNameList.at(1) + "_" + FileNameList.at(2);
                QString FilePathStr = FlifeUrl + "/Genuine/" + str;
                CreateTxtFile(FlifeUrl + "/Genuine",str);

                if(false == ret)
                    SaveImage(InitStatusInfo,_StatusInfo,FilePathStr,initStr,matchStr);

                iTimes++;
            }
        }
    }

    qDebug()<<"iTimes = "<<iTimes;
    //    }
}

void DBMatchForm::GetFeatureAndImposterMatch(QString FilePath, int DistanceL, int DistanceH)
{
    int iTimes = 0;

    GetFeatureAndImposterMatchForSameUser();
    GetFeatureAndImposterMatchForDiffUser();
}

//ImposterMatch  用于相同UserID,但是手掌类型不同的比对函数
void DBMatchForm::GetFeatureAndImposterMatchForSameUser()
{
    QList<QString>                   UserIDListTmp;
    QMap<QString ,QList<MatchInfor>>::iterator itm;
    QElapsedTimer msTimerExe1;     // 定义对象

    UserIDListTmp    = UserIDList;

    for(itm = MatchInforMap.begin();itm != MatchInforMap.end();itm++)
    {
        MatchInfor        stMatchInfor,stMatchInforInitial;
        QList<MatchInfor> MatchInforList;
        QList<MatchInfor>::iterator it;

        MatchInforList = (*itm);

        //        qDebug()<<"init strUserID = "<<strUserID;
        for(int i = MatchInforList.size();i > 1;i--)
        {
            QString str;
            std::string   InitalFeature;
            PalmImageInfo InitalpalmImageInfo;
            AuxDev::StatusInfo InitStatusInfo;
            it = MatchInforList.begin();
            stMatchInforInitial = (*it);

            //提取比较基准图像并计算feature提取时间
            cv::Mat InitialPic = cv::imread(stMatchInforInitial.FilePath.toStdString());
            msTimerExe1.start();
            AlgorithmHandler::instance()->execute(InitialPic,InitalpalmImageInfo,InitalFeature,_StatusInfo);
            float fTimerExe1 = (double)msTimerExe1.nsecsElapsed()/(double)1000000;

            //提取比较基准图像，并读取该图像提取feature期间每个步骤的时间和图像
            InitStatusInfo = _StatusInfo;
            ReadAuxDevStatusInfo(_StatusInfo,str);
            WriteTxtFileIGData(FlifeUrl,"RunData",stMatchInforInitial.FileName,str,false);

            MatchInforList.erase(it);
            for(it = MatchInforList.begin();it != MatchInforList.end();it++)
            {
                std::string   MatchFeature;
                PalmImageInfo MatchpalmImageInfo;
                stMatchInfor = (*it);

                //                qDebug()<<"stMatchInforInitial.PalmType = "<<stMatchInforInitial.PalmType<<",stMatchInfor.PalmType stMatchInfor.PalmType = "<<stMatchInfor.PalmType;
                if(stMatchInforInitial.PalmType == stMatchInfor.PalmType)//不是同一类型手掌不比较
                    continue;

                //提取将要比对的图像并计算提取时间
                cv::Mat MatchPic = cv::imread(stMatchInfor.FilePath.toStdString());
                msTimerExe1.start();
                AlgorithmHandler::instance()->execute(MatchPic,MatchpalmImageInfo,MatchFeature,_StatusInfo);
                float fTimerExe2 = (double)msTimerExe1.nsecsElapsed()/(double)1000000;
                //                qDebug()<<"stMatchInforInitial.FileName = "<<stMatchInforInitial.FileName<<",stMatchInfor.FileName = "<<stMatchInfor.FileName;

                //提取将要比对的图像，并读取该图像提取feature期间每个步骤的时间和图像
                InitStatusInfo = _StatusInfo;
                ReadAuxDevStatusInfo(_StatusInfo,str);
                WriteTxtFileIGData(FlifeUrl,"RunData",stMatchInfor.FileName,str,false);

                //提取比较基准图像和将要比对的图像所需match的时间
                msTimerExe1.start();
                int ret = DBmatchVerify(InitalFeature,MatchFeature,stMatchInforInitial,stMatchInfor);
                float fTimerExe3 = (double)msTimerExe1.nsecsElapsed()/(double)1000000;

                //输出比较基准图像和将要对比图像的所耗费时间，记录至文档
                QString initStr  = stMatchInforInitial.FileName;
                QString matchStr = stMatchInfor.FileName;
                QString LogOutInit  = initStr + " and " + matchStr + "  " + "Init:" + QString::number(fTimerExe1,'g',6) + " | " +
                        "Match:" + QString::number(fTimerExe2,'g',6) + " | " +"Compare:" + QString::number(fTimerExe3,'g',6);
                WriteTxtFileTimeData(FlifeUrl,"Imposter",stMatchInforInitial.FileName,LogOutInit);
                //                qDebug()<<"fTimerExe1 = "<<fTimerExe1<<",fTimerExe2 = "<<fTimerExe2<<",fTimerExe3 = "<<fTimerExe3;

                //创建ImposterMatch 比对成功的文档，并把对比双方的图像输出至此文档
                QStringList FileNameList = stMatchInforInitial.FileName.split(".");
                QString str  = FileNameList.at(0);
                FileNameList = str.split("_");
                str          = FileNameList.at(1) + "_" + FileNameList.at(2);

                QString FilePathStr = FlifeUrl + "/Imposter/" + str;
                CreateTxtFile(FlifeUrl + "/Imposter",str);

                if(true == ret)
                    SaveImage(InitStatusInfo,_StatusInfo,FilePathStr,initStr,matchStr);
            }
        }
    }
}

//ImposterMatch  用于不同UserID的比对函数
void DBMatchForm::GetFeatureAndImposterMatchForDiffUser()
{
    QList<QString>                   UserIDListTmp;
    QMap<QString ,QList<MatchInfor>>::iterator itm;
    QMap<QString ,QList<MatchInfor>> MatchInforMapInit;
    QElapsedTimer msTimerExe1;     // 定义对象

    UserIDListTmp    = UserIDList;
    MatchInforMapInit = MatchInforMap;

    //取出MatchInforMapInit中某个UserID下属的所有QList<MatchInfor>
    for(itm = MatchInforMapInit.begin();itm != MatchInforMapInit.end();itm++)
    {
        MatchInfor        stMatchInfor,stMatchInforInitial;
        QList<MatchInfor> MatchInforListInit,MatchInforListMatch;
        QList<MatchInfor>::iterator it;
        QList<MatchInfor>::iterator it1;

        QString strUserID  = itm.key();
        MatchInforListInit = (*itm);
        MatchInforMapInit.erase(itm);

        //        qDebug()<<"init strUserID = "<<strUserID;
        //取出MatchInforMapInit中某个UserID下属的QList<MatchInfor>中的某个信息用于比对
        for(it = MatchInforListInit.begin();it != MatchInforListInit.end();it++)
        {
            QString str;
            std::string   InitalFeature;
            PalmImageInfo InitalpalmImageInfo;
            AuxDev::StatusInfo InitStatusInfo;

            //提取比较基准图像并计算feature提取时间
            stMatchInforInitial = (*it);
            cv::Mat InitialPic = cv::imread(stMatchInforInitial.FilePath.toStdString());
            msTimerExe1.start();
            AlgorithmHandler::instance()->execute(InitialPic,InitalpalmImageInfo,InitalFeature,_StatusInfo);
            float fTimerExe1 = (double)msTimerExe1.nsecsElapsed()/(double)1000000;

            //提取比较基准图像，并读取该图像提取feature期间每个步骤的时间和图像
            InitStatusInfo = _StatusInfo;
            ReadAuxDevStatusInfo(_StatusInfo,str);
            WriteTxtFileIGData(FlifeUrl,"RunData",stMatchInforInitial.FileName,str,false);

            //            qDebug()<<"stMatchInforInitial.FileName start = "<<stMatchInforInitial.FileName;
            //取出MatchInforMapInit中除某个UserID（指的是第一个for循环选中的UserID）下属的所有QList<MatchInfor>
            for(itm = MatchInforMapInit.begin();itm != MatchInforMapInit.end();itm++)
            {
                MatchInforListMatch = (*itm);
                //                qDebug()<<"match strUserID = "<<itm.key();

                //取出MatchInforMapInit中除某个UserID（指的是第一个for循环选中的UserID）下属
                //的QList<MatchInfor>中的某个信息用于比对
                for(it1 = MatchInforListMatch.begin();it1 != MatchInforListMatch.end();it1++)
                {
                    std::string   MatchFeature;
                    PalmImageInfo MatchpalmImageInfo;

                    stMatchInfor = (*it1);

                    //提取将要比对的图像并计算提取时间
                    cv::Mat InitialPic = cv::imread(stMatchInfor.FilePath.toStdString());
                    msTimerExe1.start();
                    AlgorithmHandler::instance()->execute(InitialPic,MatchpalmImageInfo,MatchFeature,_StatusInfo);
                    float fTimerExe2 = (double)msTimerExe1.nsecsElapsed()/(double)1000000;

                    //提取将要比对的图像，并读取该图像提取feature期间每个步骤的时间和图像
                    InitStatusInfo = _StatusInfo;
                    ReadAuxDevStatusInfo(_StatusInfo,str);
                    WriteTxtFileIGData(FlifeUrl,"RunData",stMatchInfor.FileName,str,false);

                    //提取比较基准图像和将要比对的图像所需match的时间
                    msTimerExe1.start();
                    int ret = DBmatchVerify(InitalFeature,MatchFeature,stMatchInforInitial,stMatchInfor);
                    float fTimerExe3 = (double)msTimerExe1.nsecsElapsed()/(double)1000000;

                    //输出比较基准图像和将要对比图像的所耗费时间，记录至文档
                    QString initStr  = stMatchInforInitial.FileName;
                    QString matchStr = stMatchInfor.FileName;
                    QString LogOutInit  = initStr + " and " + matchStr + "  " + "Init:" + QString::number(fTimerExe1,'g',6) + " | " +
                            "Match:" + QString::number(fTimerExe2,'g',6) + " | " +"Compare:" + QString::number(fTimerExe3,'g',6);
                    WriteTxtFileTimeData(FlifeUrl,"Imposter",stMatchInforInitial.FileName,LogOutInit);
                    //                qDebug()<<"fTimerExe1 = "<<fTimerExe1<<",fTimerExe2 = "<<fTimerExe2<<",fTimerExe3 = "<<fTimerExe3;

                    //创建ImposterMatch 比对成功的文档，并把对比双方的图像输出至此文档
                    QStringList FileNameList = stMatchInforInitial.FileName.split(".");
                    QString str  = FileNameList.at(0);
                    FileNameList = str.split("_");
                    str          = FileNameList.at(1) + "_" + FileNameList.at(2);

                    QString FilePathStr = FlifeUrl + "/Imposter/" + str;
                    CreateTxtFile(FlifeUrl + "/Imposter",str);

                    if(true == ret)
                        SaveImage(InitStatusInfo,_StatusInfo,FilePathStr,initStr,matchStr);
                }
            }
            //        qDebug()<<"stMatchInforInitial.FileName end = "<<stMatchInforInitial.FileName;
        }
        itm = MatchInforMapInit.begin();
    }
}

bool DBMatchForm::DBmatchVerify(const std::string &ovsTemplate, const std::string &feature,
                                MatchInfor InitialMatchInfor, MatchInfor MatchMatchInfor)
{
    bool bRe = false;
    const std::string &sTemplate = ovsTemplate;
    const std::string &sFeature = feature;

    Vtrf vtrTemplate;
    str2VtrTemplate(sTemplate,vtrTemplate);

    Vtrf vtrFeature;
    str2VtrTemplate(sFeature,vtrFeature);

    float hamming = 0.0f;
    bRe = AlgoMatch::instance()->computeFeature(vtrTemplate.data(),vtrFeature.data(),
                                                conCellFeatureBytes,hamming);

    qDebug()<<"DBmatchVerify bRe = "<<bRe<<",hamming = "<<hamming<<",InitialMatchInfor.FileName = "
           <<InitialMatchInfor.FileName<<",MatchMatchInfor.FileName = "<<MatchMatchInfor.FileName;

    QString str = "re:" + QString::number(bRe) + ",hamming:" + QString::number(hamming,'g', 6) +
            ",InitialFileName:" + InitialMatchInfor.FileName + "MatchFileName:" + MatchMatchInfor.FileName +
            ",FlifeUrl:" + FlifeUrl;

    if(true == bGOrIFlag)
    {
        if(true == bRe)
            WriteTxtFileFARFRRData(FlifeUrl + "/Genuine","Success",InitialMatchInfor.FileName,str,bRe);
        else
            WriteTxtFileFARFRRData(FlifeUrl + "/Genuine","Fail",InitialMatchInfor.FileName,str,bRe);

    }
    else
    {
        if(true == bRe)
            WriteTxtFileFARFRRData(FlifeUrl + "/Imposter","Success",InitialMatchInfor.FileName,str,bRe);
        else
            WriteTxtFileFARFRRData(FlifeUrl + "/Imposter","Fail",InitialMatchInfor.FileName,str,bRe);
    }


    return bRe;
}

void DBMatchForm::str2VtrTemplate(const string &str, Vtrf &vtr)
{
    if(str.empty()){
        return;
    }

    vtr.clear();
    vtr.resize(str.size());

    memcpy(vtr.data(),str.data(),str.size());
}

//参数初始化
void DBMatchForm::ResetParam()
{
    FlifeUrl.clear();
    UserIDList.clear();
    AllUserInfoList.clear();
}

void DBMatchForm::LogOut(QString str, int Level)
{
    switch (Level)
    {
    case LOG_DEBUG:
        Log::instance()->getLogPoint(LOG_MODULE_DBMATCH)->debug(str);
        break;
    case LOG_INFO:
        Log::instance()->getLogPoint(LOG_MODULE_DBMATCH)->info(str);
        break;
    case LOG_WARING:
        Log::instance()->getLogPoint(LOG_MODULE_DBMATCH)->warn(str);
        break;
    case LOG_ERROR:
        Log::instance()->getLogPoint(LOG_MODULE_DBMATCH)->error(str);
        break;
    default:
        break;
    }
}



