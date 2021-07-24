# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
# 项目名称
SET(CMAKE_C_COMPILER  /usr/bin/gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_BUILD_TYPE "Release")

project(ovm_200_feature)
############################################################################
set(build_mode develop)
#
if(build_mode MATCHES develop)
    add_definitions(-DMODE_DEV)

    message(------------------------------------------------------------)
    message("build mode develop")
    message(------------------------------------------------------------)
else()
    message(+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++)
    message("build mode official")
    message(+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++)
endif()
############################################################################
#设置变量
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
	set(devicehandler_source_path ${PROJECT_SOURCE_DIR}/device-linux)
	set(opencv_path ${PROJECT_SOURCE_DIR}/../opencv-linux/lib/x64)
	MESSAGE(STATUS "Lib file path of Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
	set(devicehandler_source_path ${PROJECT_SOURCE_DIR}/device-windows)
	set(opencv_path ${PROJECT_SOURCE_DIR}/../opencv-windows/lib/x64)
	set(sonixcamera_path ${PROJECT_SOURCE_DIR}/../sonixcamera-windows/lib/x64)
	MESSAGE(STATUS "Lib file path of Windows ")
ENDIF ()
############################################################################
#添加头文件搜索路径
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
	include_directories(
	../algorithm/include
	../opencv-linux/include
	../opencv-linux/include/opencv
	../opencv-linux/include/opencv2
	../sonixcamera-linux/SonixCamera
	../logger
	${devicehandler_source_path}
	.)
	MESSAGE(STATUS "Header file path of Linux ")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
	include_directories(
	../algorithm/include
	../opencv-windows/opencv3410/include
	../opencv-windows/opencv3410/include/opencv
	../opencv-windows/opencv3410/include/opencv2
	../sonixcamera-windows/SonixCamera
	../logger
	${devicehandler_source_path}
	.)
	MESSAGE(STATUS "Header file path of Windows ")
ENDIF ()

#添加源码文件
AUX_SOURCE_DIRECTORY(.   src_list)
AUX_SOURCE_DIRECTORY(../logger   src_list)
AUX_SOURCE_DIRECTORY(../algorithm/src  src_list)
AUX_SOURCE_DIRECTORY(${devicehandler_source_path} src_list)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
AUX_SOURCE_DIRECTORY(../sonixcamera-linux/SonixCamera   src_list)
ENDIF()
############################################################################
#指定目标生成目录
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	if(build_mode MATCHES develop)
		set(target_path ${PROJECT_SOURCE_DIR}/../../lib-dev)
	else()
		set(target_path ${PROJECT_SOURCE_DIR}/../../lib)
	endif()
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	if(build_mode MATCHES develop)
		set(target_path ${PROJECT_SOURCE_DIR}/../../lib-dev-win)
	else()
		set(target_path ${PROJECT_SOURCE_DIR}/../../lib-win)
	endif()
ENDIF()

#设置生成的库文件输出目录
SET(LIBRARY_OUTPUT_PATH ${target_path})

#指定库名称
add_library(${PROJECT_NAME} SHARED ${src_list})
#版本
set(sdk_version 0.1.0.0)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION ${sdk_version})
############################################################################
#拷贝文件到输出目录

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(copy_list 
	ovm_200_feature.h
	../ovm_200_defines.h
	../run.sh
        ../algorithm/test/finger_hog_norm.yaml
        ../algorithm/test/palm_hog.yaml
	../logger/log.conf

#        ${opencv_path}/libopencv_calib3d.so.3.4.10
#        ${opencv_path}/libopencv_core.so.3.4.10
#        ${opencv_path}/libopencv_flann.so.3.4.10
#        ${opencv_path}/libopencv_highgui.so.3.4.10
#        ${opencv_path}/libopencv_imgcodecs.so.3.4.10
#        ${opencv_path}/libopencv_imgproc.so.3.4.10
#        ${opencv_path}/libopencv_ml.so.3.4.10
#        ${opencv_path}/libopencv_objdetect.so.3.4.10
#        ${opencv_path}/libopencv_video.so.3.4.10
#        ${opencv_path}/libopencv_videoio.so.3.4.10
#        ${opencv_path}/libopencv_dnn.so.3.4.10
#        ${opencv_path}/libopencv_features2d.so.3.4.10
#        ${opencv_path}/libopencv_photo.so.3.4.10
#        ${opencv_path}/libopencv_shape.so.3.4.10
#        ${opencv_path}/libopencv_stitching.so.3.4.10
#        ${opencv_path}/libopencv_superres.so.3.4.10
#        ${opencv_path}/libopencv_videostab.so.3.4.10

        ${opencv_path}/libopencv_calib3d.so.3.4
        ${opencv_path}/libopencv_core.so.3.4
        ${opencv_path}/libopencv_flann.so.3.4
        ${opencv_path}/libopencv_highgui.so.3.4
        ${opencv_path}/libopencv_imgcodecs.so.3.4
        ${opencv_path}/libopencv_imgproc.so.3.4
        ${opencv_path}/libopencv_ml.so.3.4
        ${opencv_path}/libopencv_objdetect.so.3.4
        ${opencv_path}/libopencv_video.so.3.4
        ${opencv_path}/libopencv_videoio.so.3.4
        ${opencv_path}/libopencv_dnn.so.3.4
        ${opencv_path}/libopencv_features2d.so.3.4
        ${opencv_path}/libopencv_photo.so.3.4
        ${opencv_path}/libopencv_shape.so.3.4
        ${opencv_path}/libopencv_stitching.so.3.4
        ${opencv_path}/libopencv_superres.so.3.4
        ${opencv_path}/libopencv_videostab.so.3.4

)

ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(copy_list 
	ovm_200_feature.h
	../ovm_200_defines.h
	../algorithm/test/finger_svm.yaml
	../algorithm/test/palm_det.yaml
	../logger/log.conf

	${opencv_path}/opencv_world3410.lib
	${opencv_path}/opencv_world3410.dll
	${opencv_path}/opencv_world3410d.lib
	${opencv_path}/opencv_world3410d.dll
	${opencv_path}/opencv_ffmpeg3410_64.dll
	${sonixcamera_path}/SonixCamera.dll
	${sonixcamera_path}/SonixCamera.lib)
ENDIF()

if(build_mode MATCHES develop)
    set(copy_list ovm_200_feature_dev.h  ${copy_list})
endif()

FOREACH(copy_file ${copy_list})
 file(COPY ${copy_file} DESTINATION ${target_path})
ENDFOREACH(copy_file)


#message(/////////////////////////////////////////////////////////////////)
#message(++ ${PROJECT_SOURCE_DIR})
#message(cmake-end)
#message(/////////////////////////////////////////////////////////////////)

