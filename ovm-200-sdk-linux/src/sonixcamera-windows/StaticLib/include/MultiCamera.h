#ifndef __MULTICAMERA_H__
#define __MULTICAMERA_H__
#include "util.h"
#include "SonixCamera_win.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ö���豸
//����SonixCam_EnumDevice���������Ҫ���������ӿں�������Ҫ�ȵ���SonixCam_SelectDeviceѡ��Ҫ�������豸��Ϊ����
//SonixCam_EnumDevice��Ĭ��ָ�����һ��ö�ٵ����豸��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_EnumDevice
*	Description:		ö������Sonix�豸
*	Parameters:	pCameras��CameraInfo����ָ�룬deviceNum�����ػ�õ��豸���� maxDeviceNum��sizeof(pCameras) / sizeof(CameraInfo)��
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���нӿں���֧�ֶ��߳�ͬʱ������ͬ���豸����������ڸ����̺߳����е�����¼������ͬʱ��¼�̼�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"  HRESULT WINAPI SonixCam_MulGetCurBaseFilter(unsigned long devIndex, void **ppBaseFilter);

extern "C"  HRESULT WINAPI SonixCam_MulAsicRegisterWrite(unsigned long devIndex, unsigned short addr, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulAsicRegisterRead(unsigned long devIndex, unsigned short addr, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulGetNodeId(unsigned long devIndex, LONG &nodeId, LONG id);

extern "C"  HRESULT WINAPI SonixCam_MulXuRead(unsigned long devIndex, unsigned char pData[], unsigned int length, BYTE cs, LONG nodeId);

extern "C"  HRESULT WINAPI SonixCam_MulXuWrite(unsigned long devIndex, unsigned char pData[], unsigned int length, BYTE cs, LONG nodeId);

extern "C"  HRESULT WINAPI SonixCam_MulSensorRegisterCustomRead(unsigned long devIndex, unsigned char slaveId, unsigned short  addr, unsigned char pData[], long len, bool pollSCL = false);

extern "C"  HRESULT WINAPI SonixCam_MulSensorRegisterCustomWrite(unsigned long devIndex, unsigned char slaveId, unsigned short addr, unsigned char pData[], long len, bool pollSCL = false);

extern "C"  HRESULT WINAPI SonixCam_MulSerialFlashRead(unsigned long devIndex, long addr, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulSerialFlashCustomRead(unsigned long devIndex, long addr, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulSerialFlashSectorWrite(unsigned long devIndex, long addr, unsigned char pData[], long len, SERIAL_FLASH_TYPE sft);

extern "C"  HRESULT WINAPI SonixCam_MulSerialFlashWrite(unsigned long devIndex, long addr, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulSerialFlashSectorCustomWrite(unsigned long devIndex, long addr, unsigned char pData[], long len, SERIAL_FLASH_TYPE sft);

extern "C"  HRESULT WINAPI SonixCam_MulGetSerialFlashType(unsigned long devIndex, SERIAL_FLASH_TYPE &sft, bool check = false);

extern "C"  HRESULT WINAPI SonixCam_MulGetParamTableAddr(unsigned long devIndex, long &paramTableAddr, long &paramTableLength, long &crcAddr, unsigned char *pFW = NULL);

extern "C"  HRESULT WINAPI SonixCam_MulGetFWVersion(unsigned long devIndex, unsigned char pData[], long len, BOOL bNormalExport = TRUE);

extern "C"  HRESULT WINAPI SonixCam_MulGetFWVersionFromFile(unsigned long devIndex, unsigned char pFwFile[], unsigned char pData[], long len, BOOL bNormalExport = TRUE);

extern "C"  HRESULT WINAPI SonixCam_MulGetManufacturer(unsigned long devIndex, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulGetProduct(unsigned long devIndex, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulGetVidPid(unsigned long devIndex, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulGetString3(unsigned long devIndex, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulGetInterface(unsigned long devIndex, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulGetSerialNumber(unsigned long devIndex, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_MulGetAsicRomType(unsigned long devIndex, ASIC_ROM_TYPE &romType, unsigned char &chipID);

extern "C"  HRESULT WINAPI SonixCam_MulBurnerFW(unsigned long devIndex, unsigned char pFwBuffer[], LONG lFwLength, SonixCam_SetProgress setProgress, void *ptrClass, SERIAL_FLASH_TYPE sft, BOOL bFullCheckFW = FALSE);

extern "C"  HRESULT WINAPI SonixCam_MulWriteFwToFlash(unsigned long devIndex, unsigned char pFwBuffer[], LONG lFwLength, SonixCam_SetProgress setProgress, void *ptrClass, BOOL bFullCheckFW = FALSE);

extern "C"  HRESULT WINAPI SonixCam_MulExportFW(unsigned long devIndex, unsigned char  pFwBuffer[], LONG lFwLength, SonixCam_SetProgress setProgress, void *ptrClass);

extern "C"  HRESULT WINAPI SonixCam_MulDisableSerialFlashWriteProtect(unsigned long devIndex, SERIAL_FLASH_TYPE sft);

extern "C"  HRESULT WINAPI SonixCam_MulEraseSerialFlash(unsigned long devIndex, SERIAL_FLASH_TYPE sft);

extern "C"  HRESULT WINAPI SonixCam_MulEraseSectorFlash(unsigned long devIndex, long addr, SERIAL_FLASH_TYPE sft);

extern "C"  HRESULT WINAPI SonixCam_MulEraseBlockFlash(unsigned long devIndex, long addr, SERIAL_FLASH_TYPE sft);

extern "C"  HRESULT WINAPI SonixCam_MulFullCheckFW(unsigned long devIndex, unsigned char pFwBuffer[], LONG fwLength, SonixCam_SetProgress setProgress, void *ptrClass);

extern "C"  HRESULT WINAPI SonixCam_MulCustomBurnerFW(unsigned long devIndex, const ChangeParamInfo &paramInfo, unsigned char pFwBuffer[], LONG lFwLength, SonixCam_SetProgress setProgress, void *ptrClass, SERIAL_FLASH_TYPE sft, BOOL bFullCheckFW = FALSE);

extern "C"  HRESULT WINAPI SonixCam_MulSetParamTableFromFWFile(unsigned long devIndex, unsigned char pFW[], long lFwLength, const ChangeParamInfo &paramInfo, SonixCam_SetProgress setProgress, void *ptrClass, SERIAL_FLASH_TYPE sft, char* pLogFilePath, BOOL bFullCheckFW = FALSE);


#endif
