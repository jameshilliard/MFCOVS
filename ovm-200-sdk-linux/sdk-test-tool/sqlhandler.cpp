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
                          feature bolb\
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

void SqlHandler::loadData(map<string, string> &mapData)
{
    QSqlQuery sqlQuery(_dataBase);
    QString str = QString("select id,feature from %1")
            .arg(conTbFeature);

    if(!sqlQuery.exec(str)) {
        qDebug()<<"read Feature failed."
               <<sqlQuery.lastError();
    } else {
        while(sqlQuery.next()) {
            QString qsId = sqlQuery.value("id").toString();
            QByteArray qbaFeature =
                    sqlQuery.value("feature").toByteArray();

//            mapData.insert(make_pair(qsId.toStdString(),
//                                     qsFeature.toStdString()));

            string sTmp(qbaFeature.data(),
                        qbaFeature.size());
//            sTmp.resize(qbaFeature.size());

//            memcpy(sTmp.data(),qbaFeature.data(),qbaFeature.size());

            mapData.insert(make_pair(qsId.toStdString(),
                                     sTmp));
        }
    }
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


bool SqlHandler::addData(const string &id, const string &feature)
{
    qDebug()<<"addData feature size:"<<feature.size();

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


    QByteArray qbaFeature;
//    QDataStream qds(&qbaFeature,QIODevice::WriteOnly);
//    qds.setVersion(QDataStream::Qt_5_9);
//    qds<<QString::fromStdString(feature);

    qbaFeature.resize(feature.size());

    memcpy((void*)qbaFeature.data(),
           (void*)feature.data(),
           feature.size());

    qDebug()<<"qbaFeature:"<<qbaFeature.size();

    QString qsId  = QString::fromStdString(id);

    QString queryStr = QString("insert into %0(id,feature) values(:id,:f);" );
    queryStr = queryStr.arg(conTbFeature);

    qDebug()<<"queryStr:"
           <<queryStr;

#if 1
    QSqlQuery sqlQuery(_dataBase);
    sqlQuery.prepare(queryStr);
    sqlQuery.bindValue(":id",qsId);
    sqlQuery.bindValue(":f",qbaFeature);

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
