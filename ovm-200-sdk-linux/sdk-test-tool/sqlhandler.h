#ifndef SQLHANDLER_H
#define SQLHANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <string>

using namespace std;

class SqlHandler : public QObject
{
    Q_OBJECT
public:
    explicit SqlHandler(QObject *parent = nullptr);
    static SqlHandler *instance();

    bool addData(const string& id,const string& feature);
    void loadData(map<string,string>& mapData);
private:
    std::u32string to_utf32(const std::string &str );

private:
    QSqlDatabase _dataBase;
};

#endif // SQLHANDLER_H
