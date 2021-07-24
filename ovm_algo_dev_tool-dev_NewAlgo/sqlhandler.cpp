#include "sqlhandler.h"

#include <stdio.h>
#include <iostream>
#include <codecvt>
#include <locale>
#include<regex>

#include <qdebug.h>
#include <QDataStream>
#include <qdatetime.h>
#include <QSqlQuery>
#include <qdebug.h>
#include <QSqlError>

const int conSingleFeatureBytesSize = 576;
const int conFeatureSize = 3;

const QString conDBName = "sdk_test_tool.db";
const QString conTbFeature = "tb_feature";

SqlHandler::SqlHandler(QObject *parent) : QObject(parent)
{
    //init db
    _dataBase = QSqlDatabase::addDatabase("QSQLITE");
    _dataBase.setDatabaseName(conDBName);
    if (!_dataBase.open()) {
        qDebug()<< "Error: Failed to connect database.";
        qDebug()<< _dataBase.lastError();
    }
    else {
        qDebug()<< "Succeed to connect database." ;
    }

    //
    QSqlQuery sqlQuery(_dataBase);
    QString str = QString("create table if not exists %1(\
                          id text,\
                          name text,\
                          featureL bolb,\
                          featureR bolb\
                          );")
    .arg(conTbFeature);

    if(sqlQuery.exec(str)) {
        qDebug()<< "user feature table created!";
    }
    else {
        qDebug()<< "Error: Fail to create feature table.";
        qDebug()<< sqlQuery.lastError();
    }

}

SqlHandler *SqlHandler::instance()
{
    static SqlHandler ins;
    return &ins;
}

void SqlHandler::loadData(DataList &mapData)
{
    string strTmp;
    QSqlQuery sqlQuery(_dataBase);
    QString str = QString("select * from %1")
            .arg(conTbFeature);

    if(!sqlQuery.exec(str)) {
        qDebug()<<"read Feature failed."
               <<sqlQuery.lastError();
    } else {
        while(sqlQuery.next()) {
            std::list<string> ListTmp;

            QString qsphone = sqlQuery.value("id").toString();
            QString qsName  = sqlQuery.value("name").toString();
            QByteArray qbaFeatureL = sqlQuery.value("featureL").toByteArray();
            QByteArray qbaFeatureR = sqlQuery.value("featureR").toByteArray();


            qDebug()<<"qbaFeatureL = "<<qbaFeatureL.size()<<",qsphone = "<<qsphone;
            string sTmpL(qbaFeatureL.data(),
                        qbaFeatureL.size());
            string sTmpR(qbaFeatureR.data(),
                        qbaFeatureR.size());
#if 0 //往数据库中添加测试数据
            strTmp = sTmpL;
#endif
            ListTmp.push_back(qsName.toStdString());
            ListTmp.push_back(sTmpL);
            ListTmp.push_back(sTmpR);

//            _PairData(qsphone,ListTmp);
            mapData.append(qMakePair(qsphone,ListTmp));
        }
    }


#if 0 //往数据库中添加测试数据
    for(int i = 0;i < 10000;i++)
    {
        QString strPhone = QString::number(i);
        QString strName  = QString::number(i);

        SqlHandler::instance()->addData(strPhone.toStdString(),strName.toStdString()
                                        ,strTmp,"");
    }
#endif
}

u32string SqlHandler::to_utf32(const string &str)
{
    qDebug()<<__func__;
    u32string u32sRe;
    for(int i = 0; i < conFeatureSize; i++){
        string sSub = str.substr(
                    i * conSingleFeatureBytesSize * 4,
                    conSingleFeatureBytesSize * 4);


        //        u32string u32sTmp = std::wstring_convert<
        //                std::codecvt_utf8<char32_t>,
        //                char32_t >{}.from_bytes(sSub);

        u32string u32sTmp = (char32_t*)sSub.data();

        qDebug()<<"u32sTmp:"<<QString::fromStdU32String(u32sTmp);

        u32sRe.push_back(*u32sTmp.data());
    }

    return u32sRe;
#if 0
    return std::wstring_convert<
            std::codecvt_utf8<char32_t>,
            char32_t >{}.from_bytes(str);
#endif
}


bool SqlHandler::addData(const string &phone, const string &name,
                         const string &featureL, const string &featureR)
{
    qDebug()<<"addData featureL size:"<<featureL.size()<<",name = "
            <<QString::fromStdString(name);

#if 0
    u32string u32sFeature = to_utf32(feature);
    QString qsFeature = QString::fromStdU32String(u32sFeature);
#endif

    //    QString qsFeature= QString::fromStdString(feature);
#if 0
    //    qDebug()<<"qsFeature size:"<<qsFeature.size();
    //    qDebug()<<qsFeature;
#endif

#if 0
    QString qsId  = QString::fromStdString(id);

    QString queryStr = QString("insert into %0(\
                               id, \
                               feature ) \
            ")
            ;
    QString selectStr = QString("select \'%1\',\
                                \'%2\' ");

                                queryStr = queryStr.append(selectStr);
            queryStr = queryStr
            .arg(conFeatureTb)
            .arg(qsId)
            .arg(qsFeature)
            ;
    queryStr = queryStr.append(";");
#endif


    QByteArray qbaFeatureL,qbaFeatureR;

    qbaFeatureL.resize(featureL.size());
    qbaFeatureR.resize(featureR.size());

    memcpy((void*)qbaFeatureL.data(),(void*)featureL.data(),featureL.size());
    memcpy((void*)qbaFeatureR.data(),(void*)featureR.data(),featureR.size());

//    qDebug()<<"qbaFeatureL:"<<qbaFeatureL.size();

    QString qsphone = QString::fromStdString(phone);
    QString qsName  = QString::fromStdString(name);

    QString queryStr = QString("insert into %0(id,name,featureL,featureR) values(:id,:name,:fl,:fr);" );
    queryStr = queryStr.arg(conTbFeature);

    qDebug()<<"queryStr:"
           <<queryStr;

#if 1
    QSqlQuery sqlQuery(_dataBase);
    sqlQuery.prepare(queryStr);
    sqlQuery.bindValue(":id",qsphone);
    sqlQuery.bindValue(":name",qsName);
    sqlQuery.bindValue(":fl",qbaFeatureL);
    sqlQuery.bindValue(":fr",qbaFeatureR);

    if(sqlQuery.exec()) {
        qDebug()<<"addFeature,succ.";
        return true;
    }
    else {
        qDebug()<<"addFeature,failed.";
        //        qDebug()<<"querStr:"<<queryStr;
        qDebug()<<sqlQuery.lastError();
        return false;
    }
#endif
    return false;
}

//更新用户特征信息
bool SqlHandler::UpdateData(const string &id, const string &name, bool isPamlL, const string &feature)
{
    qDebug()<<"UpdateData featureL size:"<<feature.size();
    QByteArray qbaFeature;

    qbaFeature.resize(feature.size());

    memcpy((void*)qbaFeature.data(),(void*)feature.data(),feature.size());

    qDebug()<<"qbaFeature:"<<qbaFeature.size()<<",isPamlL = "<<isPamlL;

    QSqlQuery query(_dataBase);
    //更新departID处对应的department内容
    if(isPamlL == true)
    {
        query.prepare("UPDATE  tb_feature  SET featureL =:featureL WHERE id =:id ");
        query.bindValue(":featureL",qbaFeature);
    }
    else if(isPamlL == false)
    {
        query.prepare("UPDATE  tb_feature  SET featureR =:featureR WHERE id =:id ");
        query.bindValue(":featureR",qbaFeature);
    }
    query.bindValue(":id",QString::fromStdString(id));

//    logde<<"query:"<<query;
    if(query.exec()) {
        qDebug()<<"updateFeature,succ.";

        return true;
    }
    else {
        qDebug()<<"updateFeature,failed.";
        //        logde<<"querStr:"<<queryStr;
        qDebug()<<query.lastError().text();
        return false;
    }
}

bool SqlHandler::delUserInfo(const QString &phone)
{
    QString queryStr = QString("delete from %0 where id = \"%1\";").
            arg(conTbFeature).
            arg(phone);

    qDebug()<<"del user info:"<<queryStr;

    QSqlQuery sqlQuery(_dataBase);

    qDebug()<<queryStr;

    if(sqlQuery.exec(queryStr)) {
        qDebug()<<__func__<<",succ.";
        return true;
    }
    else {
        qDebug()<<__func__<<",failed.error:"<<sqlQuery.lastError().text();
        return false;
    }
}

bool SqlHandler::delUserInfoAll()
{
    QString queryStr = QString("delete from %0 ;").
            arg(conTbFeature);

    QSqlQuery sqlQuery(_dataBase);

    qDebug()<<queryStr;

    if(sqlQuery.exec(queryStr)) {
        qDebug()<<__func__<<",succ.";
        return true;
    }
    else {
        qDebug()<<__func__<<",failed.error:"<<sqlQuery.lastError().text();
        return false;
    }
}
