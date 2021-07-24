#ifndef __MULTICAMERA_H__
#define __MULTICAMERA_H__
#include "util.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ö���豸
//����SonixCam_EnumDevice���������Ҫ���������ӿں�������Ҫ�ȵ���SonixCam_SelectDeviceѡ��Ҫ�������豸��ӦΪ����
//SonixCam_EnumDevice��Ĭ��ָ�����һ��ö�ٵ����豸��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_EnumDevice
*	Description:		ö������Sonix�豸
*	Parameters:	pCameras��CameraInfo����ָ�룬deviceNum�����ػ�õ��豸���� maxDeviceNum��pCameras�������,���������С��
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_EnumDevice(CameraInfo pCameras[], LONG &deviceNum, LONG maxDeviceNum);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ѡ���豸
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_SelectDevice
*	Description:		ѡ��ָ�����豸
*	Parameters:	devIndex��Ҫѡ�е��豸�����ţ���һ���豸������0,�������ֵ��deviceNum - 1��
*	Return :			�ɹ�����	S_OK
*/

extern "C"  HRESULT WINAPI SonixCam_SelectDevice(LONG devIndex);

#endif
