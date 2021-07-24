#ifndef TESTFORM_H
#define TESTFORM_H

#include "sqlhandler.h"
#include "algorithmhandler.h"

#include <QWidget>

namespace Ui {
class TestForm;
}

class TestForm : public QWidget
{
    Q_OBJECT

public:
    explicit TestForm(QWidget *parent = 0);
    ~TestForm();

private slots:
    void on_pushButtonBrowseTemp_clicked();
    void on_pushButtonBrowseMatch_clicked();
    void on_pushButtonStartMatch_clicked();
    void on_pushButtonBrowseEnroll_clicked();

private:
    std::string feature;
    Vtrf vtrsFeature;

    Ui::TestForm *ui;
    QString FlifeUrlTemp,FlifeUrlMatch;

    int load_feature(vector<float>& v, string file);
    void SaveTemplate(const std::string &str, string file);

    void ReadAllFileFromFolder(QString FilePath,QList<QString> &_FileNameList);
};

#endif // TESTFORM_H
