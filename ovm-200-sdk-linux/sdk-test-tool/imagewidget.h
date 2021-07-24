#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <qlabel.h>

#include "ovm_200_feature.h"

typedef unsigned char BYTE;

/**
 * @brief The ImageWidget class
 */
class ImageWidget:public QWidget{
    Q_OBJECT

public:
    ImageWidget();
    ~ImageWidget();
public slots:
    void slotRevImage(const QImage&);
    void slotRevStage(const int);
protected:
    void paintEvent(QPaintEvent *event);
private:
    int _timerId;

    QLabel *_label;
    QImage _image;

    int
    _imageWidth,
    _imageHeight,
    _imageDataLength;

};

#endif // IMAGEWIDGET_H
