#ifndef __MULTICAMERA_H__
#define __MULTICAMERA_H__
#include "util.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	枚举设备
//调用SonixCam_EnumDevice函数后如果要调用其它接口函数，需要先调用SonixCam_SelectDevice选中要操作的设备，应为调用
//SonixCam_EnumDevice后默认指定最后一个枚举到的设备。
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_EnumDevice
*	Description:		枚举所有Sonix设备
*	Parameters:	pCameras：CameraInfo数组指针，deviceNum：返回获得的设备数， maxDeviceNum：pCameras数组个数,不是数组大小。
*	Return :			成功返回	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_EnumDevice(CameraInfo pCameras[], LONG &deviceNum, LONG maxDeviceNum);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	选中设备
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_SelectDevice
*	Description:		选中指定的设备
*	Parameters:	devIndex：要选中的设备索引号，第一个设备索引是0,最大索引值是deviceNum - 1。
*	Return :			成功返回	S_OK
*/

extern "C"  HRESULT WINAPI SonixCam_SelectDevice(LONG devIndex);

#endif
