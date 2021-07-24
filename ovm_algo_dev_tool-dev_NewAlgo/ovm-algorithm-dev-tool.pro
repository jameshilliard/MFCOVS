#-------------------------------------------------
#
# Project created by QtCreator 2021-01-08T08:59:49
#
#-------------------------------------------------

QT       += core gui sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ovm-algorithm-dev-tool
TEMPLATE = app

CONFIG   += c++11

lib_path_cv = $$PWD/opencv-linux/lib/x64

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
        $$PWD/logger \
        $$PWD/logger/log4qt \
        $$PWD/opencv-linux/include \
        $$PWD/opencv-linux/include/opencv \
        $$PWD/opencv-linux/include/opencv2 \
        $$PWD/sonixcamera-linux/SonixCamera

include($$PWD/logger/log4qt/log4qt.pri)

SOURCES += \
        main.cpp \
        Global.cpp \
        logger/Log.cpp \
        mainwindow.cpp \
        DevHandler.cpp \
        sqlhandler.cpp \
        algorithmhandler.cpp \
        algorithm/src/utils.cpp \
        algorithm/src/aux_utils.cpp \
        algorithm/src/palm_vein.cpp \
        algorithm/src/palmveinparser_impl.cpp \
        sonixcamera-linux/SonixCamera/SFData.c \
        sonixcamera-linux/SonixCamera/ROMData.c \
        sonixcamera-linux/SonixCamera/XUOptDev.c \
        sonixcamera-linux/SonixCamera/SonixCamera.c \
        device-linux/sonixhandler.cpp \
        MatchDB/DBMatchForm.cpp \
        MatchRealTime/ControlForm.cpp \
        MatchRealTime/imageWidget.cpp \
        MatchRealTime/ThreadHandler.cpp \
    algorithm/src/palmveintemplate_impl.cpp \
    TestForm.cpp

HEADERS += \
        Global.h \
        logger/Log.h \
        mainwindow.h \
        sqlhandler.h \
        DevHandler.h \
        algorithmhandler.h \
        algorithm/include/utils.hpp \
        algorithm/include/aux_utils.h \
        algorithm/include/palm_vein.hpp \
        algorithm/include/palmveinparser_impl.hpp \
        sonixcamera-linux/SonixCamera/util.h \
        sonixcamera-linux/SonixCamera/SFData.h \
        sonixcamera-linux/SonixCamera/ROMData.h \
        sonixcamera-linux/SonixCamera/XUOptDev.h \
        sonixcamera-linux/SonixCamera/SonixCamera.h \
        device-linux/sonixhandler.h \
        MatchDB/DBMatchForm.h \
        MatchRealTime/ControlForm.h \
        MatchRealTime/imageWidget.h \
        MatchRealTime/ThreadHandler.h \
    algorithm/include/palmveintemplate_impl.hpp \
    TestForm.h

FORMS += \
        mainwindow.ui \
        ControlForm.ui \
        DBMatchForm.ui \
        imageWidget.ui \
        DBMatchForm.ui \
        ControlForm.ui \
        imageWidget.ui \
    TestForm.ui

LIBS += \
        $${lib_path_cv}/libopencv_ml.so.4.5 \
        $${lib_path_cv}/libopencv_dnn.so.4.5 \
        $${lib_path_cv}/libopencv_core.so.4.5 \
        $${lib_path_cv}/libopencv_video.so.4.5 \
        $${lib_path_cv}/libopencv_flann.so.4.5 \
        $${lib_path_cv}/libopencv_gapi.so.4.5  \
        $${lib_path_cv}/libopencv_photo.so.4.5 \
        $${lib_path_cv}/libopencv_calib3d.so.4.5 \
        $${lib_path_cv}/libopencv_highgui.so.4.5 \
        $${lib_path_cv}/libopencv_videoio.so.4.5 \
        $${lib_path_cv}/libopencv_imgproc.so.4.5 \
        $${lib_path_cv}/libopencv_imgcodecs.so.4.5 \
        $${lib_path_cv}/libopencv_objdetect.so.4.5 \
        $${lib_path_cv}/libopencv_stitching.so.4.5 \
        $${lib_path_cv}/libopencv_features2d.so.4.5 \
        $$PWD/sonixcamera-linux/lib/x64/libSonixCamera.so
#LIBS += \
#        $${lib_path_cv}/libopencv_ml.so.3.4 \
#        $${lib_path_cv}/libopencv_dnn.so.3.4 \
#        $${lib_path_cv}/libopencv_core.so.3.4 \
#        $${lib_path_cv}/libopencv_video.so.3.4 \
#        $${lib_path_cv}/libopencv_shape.so.3.4 \
#        $${lib_path_cv}/libopencv_photo.so.3.4 \
#        $${lib_path_cv}/libopencv_flann.so.3.4 \
#        $${lib_path_cv}/libopencv_calib3d.so.3.4 \
#        $${lib_path_cv}/libopencv_highgui.so.3.4 \
#        $${lib_path_cv}/libopencv_videoio.so.3.4 \
#        $${lib_path_cv}/libopencv_imgproc.so.3.4 \
#        $${lib_path_cv}/libopencv_superres.so.3.4 \
#        $${lib_path_cv}/libopencv_imgcodecs.so.3.4 \
#        $${lib_path_cv}/libopencv_objdetect.so.3.4 \
#        $${lib_path_cv}/libopencv_videostab.so.3.4 \
#        $${lib_path_cv}/libopencv_stitching.so.3.4 \
#        $${lib_path_cv}/libopencv_features2d.so.3.4 \
#        $$PWD/sonixcamera-linux/lib/x64/libSonixCamera.so

cpcmd.files += $$LIBS
cpcmd.files += $${lib_path_cv}/log.conf
cpcmd.files += $${lib_path_cv}/palm_hog.yaml
cpcmd.files += $${lib_path_cv}/finger_hog_norm.yaml
cpcmd.path = $$OUT_PWD
COPIES += cpcmd

message(-------------------------------------)
message($$OUT_PWD)

DISTFILES += \
    opencv-linux/lib/x64/libopencv_features2d_copy.so.3.4


