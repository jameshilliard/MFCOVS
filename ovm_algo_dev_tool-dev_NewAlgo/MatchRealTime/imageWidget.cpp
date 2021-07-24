#include "MatchRealTime/imageWidget.h"
#include "ui_imageWidget.h"

#include <QPainter>

imageWidget::imageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::imageWidget)
{
    ui->setupUi(this);

    g_bUpdateImage = true;
}

imageWidget::~imageWidget()
{
    delete ui;
}

void imageWidget::paintEvent(QPaintEvent *event)
{
    QMatrix matrix;
    matrix.rotate(90);

    QPainter painter(this);

    //绘手掌图像
    qDebug()<<"paintEvent 00";
    QPixmap  pp = QPixmap::fromImage(_image);
    painter.drawPixmap(rect(),pp,QRect());
    qDebug()<<"paintEvent 11  _image.isNull() = "<<_image.isNull()
           <<",g_bUpdateImage = "<<g_bUpdateImage<<",palm_cx = "<<ImageInfo.palm_cx
           <<",palm_cy = "<<ImageInfo.palm_cy;

    if(!_image.isNull())
    {
        QPen pen(Qt::yellow);
        pen.setWidth(3);
        painter.setPen(pen);
        painter.setBrush(QBrush(Qt::transparent));

        painter.drawEllipse(QPointF(50 * 5.4,60 * 5.4),20 * 5.4,20 * 5.4);
    }

    if(ImageInfo.palm_cx > 0 && ImageInfo.palm_cy > 0)
    {
        if(g_bUpdateImage)
        {
            QPen pen(Qt::green);
            pen.setWidth(3);
            painter.setPen(pen);
            painter.setBrush(QBrush(Qt::transparent));

            painter.drawEllipse(QPointF(ImageInfo.palm_cx * 5.4,ImageInfo.palm_cy * 5.4)
                                ,10 * 5.4,10 * 5.4);
        }
    }

    if(!g_bUpdateImage)
    {
        painter.setPen(QPen(Qt::green,4));
        painter.drawRect(2,2,536,536);//画矩形
    }
    qDebug()<<"paintEvent 22";
}

void imageWidget::slotRevImage(const QImage &image, const OVSInformation &ovsImageInfo)
{
    _image    = image;
    ImageInfo = ovsImageInfo;

//    qDebug()<<"palm_cx = "<<ImageInfo.palm_cx<<",palm_cy = "<<ImageInfo.palm_cy;

    if(g_bUpdateImage)
        update();
}

