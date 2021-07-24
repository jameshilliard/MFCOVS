#include "TestForm.h"
#include "ui_TestForm.h"

#include <regex>
#include <iostream>
#include <fstream>
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>

TestForm::TestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestForm)
{
    ui->setupUi(this);
}

TestForm::~TestForm()
{
    delete ui;
}

void TestForm::on_pushButtonBrowseTemp_clicked()
{
    QString str = "选择模板路径";

//    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
////    FlifeUrlTemp = QFileDialog::getExistingDirectory(this, "选择文件",
////                                                 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)); //选择路径:默认桌面

//    FlifeUrlTemp = QFileDialog::getOpenFileName(
//            this,tr("open a file."),
//            ".", tr("All files(*.*)"));

//    qDebug()<<"on_pushButtonBrowse_clicked FlifeUrlTemp = "<<FlifeUrlTemp;

    QString FlifeUrlTemp =  "/home/lisa/code/build-ovm-algorithm-dev-tool-Desktop_Qt_5_9_0_GCC_64bit-Release/RunData/Enroll/Enroll-15066850733/L-Raw-.template";
    ui->lineEditBrowseTemp->setText(FlifeUrlTemp);

    load_feature(vtrsFeature,FlifeUrlTemp.toStdString());
//    QFile   _File(FlifeUrlTemp);
//    if (!_File.exists()) //文件不存在
//        return;
//    if (!_File.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;

//    QByteArray byteArray = _File.readAll();

//    Global::instance()->str2VtrTemplate(byteArray.toStdString(),vtrsFeature);

    qDebug()<<",vtrsFeature.size() = "<<vtrsFeature.size();


//    DataList     _DataList;
//    DataList::iterator it;
//    SqlHandler::instance()->loadData(_DataList);
//    for(it = _DataList.begin();it != _DataList.end();it++)
//    {

//    }
}

void TestForm::on_pushButtonBrowseMatch_clicked()
{
    AuxDev::StatusInfo _StatusInfo;
    QString str = "选择图片路径";

    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
//    FlifeUrlMatch = QFileDialog::getExistingDirectory(this, "选择文件",
//                                                 QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)); //选择路径:默认桌面

    FlifeUrlMatch = QFileDialog::getOpenFileName(
            this,tr("open a file."),
                ".", tr("All files(*.*)"));
//            ".", tr("images(*.png *jpeg *bmp);;video files(*.avi *.mp4 *.wmv);;All files(*.template)"));


    qDebug()<<"on_pushButtonBrowse_clicked FlifeUrlMatch = "<<FlifeUrlMatch;
    ui->lineEditBrowseMatch->setText(FlifeUrlMatch);

    QFile   _File(FlifeUrlMatch);
    if (!_File.exists()) //文件不存在
        return;

    cv::Mat imgbuf = cv::imread(FlifeUrlMatch.toStdString());

//    QString strPath = QDir::currentPath() + "/111.png";
//    cv::imwrite(strPath.toStdString(),imgbuf);

    int ret = AlgorithmHandler::instance()->
            execute(imgbuf,g_palmImageInfo,feature,_StatusInfo);

    qDebug()<<"ret = "<<ret;
}

void TestForm::on_pushButtonStartMatch_clicked()
{
    int succHanming,failHanming;
    AuxDev::StatusInfo _StatusInfo;
    Vtrf vtrsFeatureMatch;

    Global::instance()->str2VtrTemplate(feature,vtrsFeatureMatch);

    qDebug()<<"vtrsFeature.size() = "<<vtrsFeature.size()<<",vtrsFeatureMatch.size() = "<<vtrsFeatureMatch.size();
    int bRe = AlgoMatch::instance()->computeFeature(
                    vtrsFeature,
                    vtrsFeatureMatch,
                    succHanming,failHanming);

    qDebug()<<"succHanming = "<<succHanming<<",failHanming = "<<failHanming;

}

void TestForm::on_pushButtonBrowseEnroll_clicked()
{
//    QStandardPaths::writableLocation(".");
//    QString FlifeUrl = QFileDialog::getExistingDirectory(this, "选择文件",
//                                                 QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));//选择路径:默认桌面
    Vtrf vtr1,vtr2,vtr3,vtr4,vtr5;
    QList<QString>     FileNameList;
    AuxDev::StatusInfo _StatusInfo;

    QString FlifeUrl = QFileDialog::getExistingDirectory(this,"请选择模板保存路径...","./");

    qDebug()<<"on_pushButtonBrowseEnroll_clicked FlifeUrl = "<<FlifeUrl;
    ui->lineEditBrowseEnroll->setText(FlifeUrl);

    ReadAllFileFromFolder(FlifeUrl,FileNameList);

    qDebug()<<"FileNameList.size = "<<FileNameList.size();
    for (int var = 0; var < FileNameList.size(); var++)
    {
        QString FileName = FileNameList.at(var);
        QString FlifeUrlMatch = FlifeUrl + "/" + FileName;

        qDebug()<<"FlifeUrlMatch = "<<FlifeUrlMatch;
        QFile _File(FlifeUrlMatch);
        if (!_File.exists()) //文件不存在
            return;

        cv::Mat imgbuf = cv::imread(FlifeUrlMatch.toStdString());

    //    QString strPath = QDir::currentPath() + "/111.png";
    //    cv::imwrite(strPath.toStdString(),imgbuf);

        int ret = AlgorithmHandler::instance()->
                execute(imgbuf,g_palmImageInfo,feature,_StatusInfo);

        if(FileName.contains("58"))
            Global::instance()->str2VtrTemplate(feature,vtr1);
        else if(FileName.contains("64"))
            Global::instance()->str2VtrTemplate(feature,vtr2);
        else if(FileName.contains("75"))
            Global::instance()->str2VtrTemplate(feature,vtr3);
        else if(FileName.contains("83"))
            Global::instance()->str2VtrTemplate(feature,vtr4);
        else if(FileName.contains("89"))
            Global::instance()->str2VtrTemplate(feature,vtr5);

    }

    OVS::PalmVeinTemplate builder(0);
    bool ok = false;
    int iPtsNum = 0;

    while (!ok)
    {
        builder.feed(vtr1.data(), vtr1.size());
        builder.feed(vtr2.data(), vtr2.size());
        builder.feed(vtr3.data(), vtr3.size());
        builder.feed(vtr4.data(), vtr4.size());
        builder.feed(vtr5.data(), vtr5.size());

        iPtsNum = builder.check(ok);

        qDebug()<<"ok = "<<ok;
        qDebug()<<"iPtsNum = "<<iPtsNum;
    }
    vector<float> FinalTemp;
    builder.buildTemplate(FinalTemp);
    string feature1 = string((char*)FinalTemp.data(),(FinalTemp.size() * sizeof(float)));
    QString str1Raw = QDir::currentPath() + "/" + "RunData/Enroll/" + "test.template";
    SaveTemplate(feature1,str1Raw.toStdString());

}


int TestForm::load_feature(std::vector<float>& v, std::string file)
{
    std::ifstream ifs(file);
    vector<std::string> raw;
    while (!ifs.eof())
    {
        string str;
        getline(ifs, str);
        if (str.size() <= 2)
            break;
        raw.push_back(str);
    }

    //feature vec
    std::regex sep_regex("\\ ");
    int row = 0;
    for (auto line : raw)
    {
        std::vector<std::string> subs(
            std::sregex_token_iterator(line.begin(), line.end(), sep_regex, -1),
            std::sregex_token_iterator());
        for (auto item : subs)
            v.push_back(stof(item));
    }
}

void TestForm::SaveTemplate(const string &str, string file)
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

void TestForm::ReadAllFileFromFolder(QString FilePath, QList<QString> &_FileNameList)
{
    QDir dir(FilePath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList AllUserInfoList = dir.entryInfoList();
    qDebug()<<"ReadAllFileFromFolder AllUserInfoList.size() = "<<AllUserInfoList.size();
    for (int i = 0; i < AllUserInfoList.size(); ++i)
    {
        QFileInfo fileInfo  = AllUserInfoList.at(i);
        QString strUserInfo = fileInfo.fileName();

        qDebug()<<"ReadAllFileFromFolder trUserInfo = "<<strUserInfo;

        if(strUserInfo.contains("-Raw-") && !strUserInfo.contains("template"))
        {
            _FileNameList.append(strUserInfo);
        }
    }

}



