#include "imagewidget.h"

#include <QHBoxLayout>
#include <qmessagebox.h>
#include <cstring>
#include <QHBoxLayout>
#include <qdebug.h>
#include <QPainter>
#include <qlabel.h>
#include <QtGlobal>
#include <unistd.h>

using namespace std;
//text position
const int textLeftX = 6;
const int textLeftY = 16;

const int textRightX = 460;
const int textRightY = 6;

const int textYInterval = 12;

ImageWidget::ImageWidget()
    :_imageDataLength(0)
{
    resize(777,777);

    _label = new QLabel(this);
    QHBoxLayout *layout =new QHBoxLayout;
    layout->addWidget(_label);
    setLayout(layout);

    _imageWidth = 0;
    _imageHeight = 0;
    _imageDataLength = 0;
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::slotRevImage(const QImage &image)
{
    _image = image;
    update();
}

void ImageWidget::slotRevStage(const int)
{

}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    QMatrix matrix;
    matrix.rotate(90);

    QPainter painter(this);

    if(_image.isNull()){
        painter.save();
        painter.setPen(Qt::black);
        painter.drawRect(0,0,539,539);
        painter.restore();
    }else{
        //        _image = _image.transformed(matrix,Qt::FastTransformation);
        //

        //        _image = _image.copy(0,420,1080,1080);
        //        _image = _image.scaled(540,540);

        //        _image = _image.mirrored(true,false);
        //
        QPixmap  pp = QPixmap::fromImage(_image);
        painter.drawPixmap(rect(),pp,QRect());
    }
}

