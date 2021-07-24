#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include "Global.h"
#include <QWidget>

namespace Ui {
class imageWidget;
}

class imageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit imageWidget(QWidget *parent = 0);
    ~imageWidget();

//    bool bUpdateImage;

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::imageWidget *ui;
    QImage _image;
    OVSInformation ImageInfo;

public slots:
    void slotRevImage(const QImage&,const OVSInformation&);

};

#endif // IMAGEWIDGET_H
