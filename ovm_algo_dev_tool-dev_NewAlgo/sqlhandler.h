#ifndef SQLHANDLER_H
#define SQLHANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <string>

using namespace std;

//typedef std::map<string, std::list<string>> QmapList;

typedef QPair<QString, std::list<string>> PairData;
typedef QList<PairData> DataList;

class SqlHandler : public QObject
{
    Q_OBJECT
public:
    explicit SqlHandler(QObject *parent = nullptr);
    static SqlHandler *instance();

    bool addData(const string& phone,const string& name,const string& featureL,const string& featureR);
    bool UpdateData(const string& id,const string& name,bool isPamlL,const string& feature);
    bool delUserInfo(const QString& phone);
    bool delUserInfoAll();
    void loadData(DataList &mapData);
private:
    std::u32string to_utf32(const std::string &str );

private:
    QSqlDatabase _dataBase;
    PairData     _PairData;
    DataList     _DataList;
};

#endif // SQLHANDLER_H
