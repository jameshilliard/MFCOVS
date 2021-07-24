#ifndef __SONIXCAMERA_H__
#define __SONIXCAMERA_H__

//error number

typedef enum
{
	EC_UnKnow,
	EC_LibUnInit,
	EC_EnumDeviceFail,
	EC_DisableFlashWriteProtectFail,
	EC_EraseFlashFail,
	EC_EraseFlashSectorFail,
	EC_GetAsicIdFail,
	EC_GetAsicRomVersionFail,
	EC_GetAsicRomTypeFail,
	EC_ReadAsicRegisterFail,
	EC_WriteAsicRegisterFail,
	EC_UnKnowSerialFlashType,
	EC_BurnerCheckFail,
	EC_CoInitializeFail,
	EC_NoFindDevice,
	EC_MallocMemoryFail,
}ERROR_CODE;


typedef enum
{
	SFT_UNKNOW,
	SFT_MXIC,
	SFT_ST,
	SFT_SST,
	SFT_ATMEL_AT25F,
	SFT_ATMEL_AT25FS,
	SFT_ATMEL_AT45DB,
	SFT_WINBOND,
	SFT_PMC,
	SFT_MXIC_LIKE	,
	SFT_AMIC,
	SFT_EON,
	SFT_ESMT,
	SFT_GIGA
}SERIAL_FLASH_TYPE;

typedef enum
{
	ART_UNKNOW = -1,
	ART_32K,
	ART_64K,
	ART_128K
}ASIC_ROM_TYPE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���ʼ��
//�ڵ��ÿ�Ľӿں���֮ǰ����Ҫ���ȵ���SonixCam_Init������ֻ�иú������سɹ��󣬲ſ��Ե��������ӿں���
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_Init
*	Description:		��ĳ�ʼ��
*	Parameters:	vidpid�����ָ��vidpid�����Ѱ�Ҳ�ָ���豸�����û���ҵ�������S_FALSE��
*				��ָ��vidpid�����ҵ�Sonix�豸��ֱ�ӷ���S_OK����ָ���豸��û���ҵ�������S_FALSE
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_Init(char *vidpid = NULL);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_UnInit
*	Description:		
*	Parameters:	
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_UnInit();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�����豸
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_RestartDevice
*	Description:		
*	Parameters:
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_RestartDevice();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	��ô������
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetErrorCode
*	Description:
*	Parameters:
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetErrorCode(ERROR_CODE &ec);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	��õ�ǰ�豸��BaseFilterָ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetCurBaseFilter
*	Description:		
*	Parameters:		ppBaseFilter:�ɹ������豸��pBaseFilterָ��
*	Return :			�ɹ�����	S_OK
*/
extern "C" HRESULT WINAPI SonixCam_GetCurBaseFilter(void **ppBaseFilter);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Dsp�Ĵ�����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_AsicRegisterRead
*	Description:
*	Parameters:		addr:dsp�Ĵ�����ַ��pData��ȡ���ݻ��棬len��Ҫ��ȡ�������ֽ�����
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_AsicRegisterRead(unsigned short addr, unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Dsp�Ĵ���д
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_AsicRegisterWrite
*	Description:
*	Parameters:		addr:dsp�Ĵ�����ַ��pData��д���ݻ��棬len��Ҫд�������ֽ�����
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_AsicRegisterWrite(unsigned short addr, unsigned char pData[], long len);

//get nodeId
extern "C"  HRESULT WINAPI SonixCam_GetNodeId(LONG &nodeId, LONG id);
// XU read
extern "C"  HRESULT WINAPI SonixCam_XuRead(USHORT length, BYTE *pValue, BYTE cs, LONG nodeId);
// XU write
extern "C"  HRESULT WINAPI SonixCam_XuWrite(USHORT length, BYTE *pValue, BYTE cs, LONG nodeId);

extern "C"  HRESULT WINAPI SonixCam_SensorRegisterRead(unsigned char slaveId, unsigned char  addr, unsigned char pData[], long len);

extern "C"  HRESULT WINAPI SonixCam_SensorRegisterWrite(unsigned char slaveId, unsigned char  addr, unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	I2C��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_SensorRegisterCustomRead
*	Description:
*	Parameters:		slaveId:I2c���豸ID��addr��Ҫ���ĵ�ַ��pData�������棬 len�������ݳ��ȣ� pollSCL��SCL ����
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_SensorRegisterCustomRead(unsigned char slaveId, unsigned short  addr, unsigned char pData[], long len, bool pollSCL = false);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	I2Cд
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_SensorRegisterCustomWrite
*	Description:
*	Parameters:		slaveId:I2c���豸ID��addr��Ҫд�ĵ�ַ��pData��д���棬 len��д���ݳ��ȣ� pollSCL��SCL ����
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_SensorRegisterCustomWrite(unsigned char slaveId, unsigned short addr, unsigned char pData[], long len, bool pollSCL = false);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	FLASH��������ַ���Χ��0-256k�ռ�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_SerialFlashRead
*	Description:
*	Parameters:		addr��Ҫд�ĵ�ַ��pData��д���棬 len��д���ݳ���
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_SerialFlashRead(long addr, unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	FLASH�������ٶȱ�SonixCam_SerialFlashRead�������Ƕ���ַ�ռ����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_SerialFlashRead
*	Description:
*	Parameters:		addr��Ҫд�ĵ�ַ��pData��д���棬 len��д���ݳ���
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_SerialFlashCustomRead(long addr, unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	FLASHд��д��ַ���Χ��0-256k�ռ�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_SerialFlashSectorWrite
*	Description:		�Ȳ�������addr��ַ���ڵ�Flash����������ַ���ڵ���������ȫ������λ0xFF��Ȼ��д�����ݡ�
*	Parameters:		addr��д�ĵ�ַ��pData��д���棬 len��д���ݳ��ȣ�sft: FLASH������
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_SerialFlashSectorWrite(long addr, unsigned char pData[], long len, SERIAL_FLASH_TYPE sft);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	FLASHд��д�ٶȱ�SonixCam_SerialFlashSectorWrite��������д��ַ�ռ����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_SerialFlashSectorWrite
*	Description:
*	Parameters:		addr��д�ĵ�ַ��pData��д���棬 len��д���ݳ��ȣ�sft: FLASH������
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_SerialFlashSectorCustomWrite(long addr, unsigned char pData[], long len, SERIAL_FLASH_TYPE sft);

extern "C"  HRESULT WINAPI SonixCam_SerialFlashSectorWriteEx(long addr, unsigned char pData[], long len, SERIAL_FLASH_TYPE sft);

//get device information	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���FLASH������
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetSerialFlashType
*	Description:
*	Parameters:		sft���ɹ�����flash�����ͣ�check����Ϊtrue����flash��ȡflash����
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetSerialFlashType(SERIAL_FLASH_TYPE &sft, bool check = false);

extern "C"  HRESULT WINAPI SonixCam_GetParamTableAddr(long &paramTableAddr, long &paramTableLength, long &crcAddr, unsigned char *pFW = NULL);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���FW Code�汾��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetFWVersion
*	Description:
*	Parameters:		pData��FW Code�汾�Ż�������ָ�룬len��������������ֽ���
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetFWVersion(unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����豸Manufacturer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetManufacturer
*	Description:
*	Parameters:		pData��Manufacturer��������ָ�룬len��������������ֽ���
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetManufacturer(unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����豸Product
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetProduct
*	Description:
*	Parameters:		pData��Product��������ָ�룬len��������������ֽ���
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetProduct(unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����豸vidpid
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetVidPid
*	Description:
*	Parameters:		pData��vidpid��������ָ�룬len��������������ֽ���
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetVidPid(unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����豸String3
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetString3
*	Description:
*	Parameters:		pData��String3��������ָ�룬len��������������ֽ���
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetString3(unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����豸Interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetInterface
*	Description:
*	Parameters:		pData��Interface��������ָ�룬len��������������ֽ���
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetInterface(unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����豸���к�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:		SonixCam_GetSerialNumber
*	Description:
*	Parameters:		pData�����кŻ�������ָ�룬len��������������ֽ���
*	Return :			�ɹ�����	S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetSerialNumber(unsigned char pData[], long len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���Ȼص�����ָ��
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Description:	SonixCam_BurnerFW�� SonixCam_ExportFW��SonixCam_FullCheckFW�� SonixCam_CustomBurnerFW��
*				SonixCam_SetParamTableFormFWFile�п��Դ��ݺ���ָ����������¼����
*	Parameters:	ptrClass����ָ�룬fProcess����ǰ����ֵ
*	Return :		�ɹ�����S_OK
*/
typedef void(*SonixCam_SetProgress)(void *ptrClass, float fProcess);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	���rom����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_GetAsicRomType
*	Description:	
*	Parameters:	romType������rom���ͣ�chipID����ǰdsp��оƬidֵ
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_GetAsicRomType(ASIC_ROM_TYPE &romType, unsigned char &chipID);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	��¼�̼�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_BurnerFW
*	Description:
*	Parameters:	pFwBuffer��Ҫ��¼�Ĺ̼�����ָ�룬lFwLength���̼����泤�ȣ� setProgress����¼���Ȼص�ָ�룬 ptrClass����ǰ�࣬ sft��flash���ͣ� bFullCheckFW����¼��ɺ��Ƿ����ȫУ��
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_BurnerFW(unsigned char pFwBuffer[], LONG lFwLength, SonixCam_SetProgress setProgress, void *ptrClass, SERIAL_FLASH_TYPE sft, BOOL bFullCheckFW = FALSE);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�����̼�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_ExportFW
*	Description:
*	Parameters:	pFwBuffer��Ҫ����̼�����ָ�룬lFwLength���̼����泤�ȣ� setProgress���������Ȼص�ָ�룬 ptrClass����ǰ��
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_ExportFW(unsigned char  pFwBuffer[], LONG lFwLength, SonixCam_SetProgress setProgress, void *ptrClass);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�ر�FLASH��д����
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_DisableSerialFlashWriteProtect
*	Description:
*	Parameters:	sft��FLASH����
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI  SonixCam_DisableSerialFlashWriteProtect(SERIAL_FLASH_TYPE sft);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����FLASH
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_EraseSerialFlash
*	Description:
*	Parameters:	sft��FLASH����
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_EraseSerialFlash(SERIAL_FLASH_TYPE sft);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����FLASH��һ������
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_EraseSectorFlash
*	Description:
*	Parameters:	addr��Ҫ������������ַ��sft��FLASH����
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_EraseSectorFlash(long addr, SERIAL_FLASH_TYPE sft);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	����FLASH��һ������
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_EraseBlockFlash
*	Description:
*	Parameters:	addr��Ҫ�����Ŀ�����ַ��sft��FLASH����
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_EraseBlockFlash(long addr, SERIAL_FLASH_TYPE sft);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ȫУ��FW
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_FullCheckFW
*	Description:
*	Parameters:	pFwBuffer��Ҫ���бȽϵĹ̼�����ָ�룬fwLength�����泤�ȣ�setProgress���ȽϽ��Ȼص�����ָ�룬 ptrClass����ǰ�� 
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_FullCheckFW(unsigned char pFwBuffer[], LONG fwLength, SonixCam_SetProgress setProgress, void *ptrClass);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	��������Ϣ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	
*	����Ҫ�޸ĵĲ����ַ�����ֵ����Ӧ��ָ�룬����д��Ӧ�Ĳ����ַ����ַ�����
*	
*	
*/

typedef struct
{
	unsigned int SerialNumberLength;
	unsigned int ProductLength;
	unsigned int ManufactureLength;
	unsigned int FWVersionLength;
	unsigned int VidPidLength;
	unsigned int InterfaceLength;
	unsigned int String3Length;
	char* pSerialNumber;
	char* pProduct;
	char* pManufacture;
	char* pFWVersion;
	char* pVidPid;
	char* pString3;
	char* pInterface;
}ChangeParamInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�Զ�����¼�̼�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_CustomBurnerFW
*	Description:	�ú�����SonixCam_BurnerFW�������������Ը����豸��һЩ����
*	Parameters:	extendSet��Ҫ���ĵĲ�����Ϣ��pFwBuffer���̼�����ָ�룬lFwLength�����泤�ȣ�setProgress����¼���Ȼص�����ָ�룬 ptrClass����ǰ�࣬ sft:FLASH���ͣ�bFullCheckFW���Ƿ���¼��ɺ����ȫУ��
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_CustomBurnerFW(const ChangeParamInfo &extendSet, unsigned char pFwBuffer[], LONG lFwLength, SonixCam_SetProgress setProgress, void *ptrClass, SERIAL_FLASH_TYPE sft, BOOL bFullCheckFW = FALSE);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	�޸��豸��������Ϣ
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*	Function:	SonixCam_SetParamTableFormFWFile
*	Description:	�ú�����SonixCam_BurnerFW��������������Ҫ������FW��¼��Flash��ֻ��Ҫ��¼���������ڵ��������ݣ�pFW�̼��������ݱ�����豸FLASH�еĹ̼���ͬ
*	Parameters:	pFW���̼�����ָ�룬lFwLength�����泤�ȣ�setProgress����¼���Ȼص�����ָ�룬 ptrClass����ǰ�࣬ sft:FLASH���ͣ�pLogFilePath: Log�ļ�·���� bFullCheckFW���Ƿ���¼��ɺ����ȫУ��
*	Return :		�ɹ�����S_OK
*/
extern "C"  HRESULT WINAPI SonixCam_SetParamTableFormFWFile(unsigned char pFW[], long lFwLength, const ChangeParamInfo &paramInfo, SonixCam_SetProgress setProgress, void *ptrClass, SERIAL_FLASH_TYPE sft, char* pLogFilePath, BOOL bFullCheckFW = FALSE);

extern "C"  HRESULT WINAPI SonixCam_SetParamTableFormReadFlashSector(const ChangeParamInfo &paramInfo, SonixCam_SetProgress setProgress, void *ptrClass, SERIAL_FLASH_TYPE sft, char* pLogFilePath, BOOL bFullCheckFW = FALSE);

#endif __SONIXCAMERA_H__