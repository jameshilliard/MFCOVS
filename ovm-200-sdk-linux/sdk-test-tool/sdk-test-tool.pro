#-------------------------------------------------
#
# Project created by QtCreator 2020-11-10T10:57:36
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sdk_test_tool
TEMPLATE = app

CONFIG   += c++11

lib_path = ../lib-dev

SOURCES += main.cpp\
        mainwindow.cpp \
    imagewidget.cpp \
    ../src/logger/logger.cpp \
    sqlhandler.cpp

HEADERS  += mainwindow.h \
        imagewidget.h \
        $${lib_path}/ovm_200_feature_dev.h \
        $${lib_path}/ovm_200_feature.h \
        $${lib_path}/ovm_200_match.h \
        $${lib_path}/ovm_200_defines.h \
        ../src/logger/logger.h \
        sqlhandler.h

FORMS    += mainwindow.ui

INCLUDEPATH += \
    $${lib_path} \
    ../src/logger

#LIBS += \
#$${lib_path}/libovm_200_feature.so.0.1.0.0\
#$${lib_path}/libovm_200_match.so.0.1.0.0 \
#$${lib_path}/libopencv_core.so.3.4.10 \
#$${lib_path}/libopencv_highgui.so.3.4.10 \
#$${lib_path}/libopencv_imgcodecs.so.3.4.10 \
#$${lib_path}/libopencv_imgproc.so.3.4.10 \
#$${lib_path}/libopencv_objdetect.so.3.4.10 \
#$${lib_path}/libopencv_ml.so.3.4.10 \
#$${lib_path}/libopencv_videoio.so.3.4.10 \
#$${lib_path}/libopencv_flann.so.3.4.10 \
#$${lib_path}/libopencv_calib3d.so.3.4.10

LIBS += \
$${lib_path}/libovm_200_feature.so.0.1.0.0\
$${lib_path}/libovm_200_match.so.0.1.0.0 \
$${lib_path}/libopencv_core.so.3.4 \
$${lib_path}/libopencv_highgui.so.3.4 \
$${lib_path}/libopencv_imgcodecs.so.3.4 \
$${lib_path}/libopencv_imgproc.so.3.4 \
$${lib_path}/libopencv_objdetect.so.3.4 \
$${lib_path}/libopencv_ml.so.3.4 \
$${lib_path}/libopencv_videoio.so.3.4 \
$${lib_path}/libopencv_flann.so.3.4 \
$${lib_path}/libopencv_calib3d.so.3.4

cpcmd.files += $$LIBS
cpcmd.files += $${lib_path}/log.conf
cpcmd.files += $${lib_path}/palm_hog.yaml
cpcmd.files += $${lib_path}/finger_hog_norm.yaml
cpcmd.path = $$OUT_PWD
COPIES += cpcmd

message(-------------------------------------)
message($$OUT_PWD)


