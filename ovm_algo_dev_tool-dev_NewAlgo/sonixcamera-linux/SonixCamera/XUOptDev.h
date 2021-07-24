#ifndef __LUOPTDEV_H__
#define __LUOPTDEV_H__

#include "util.h"
#include "ROMData.h"

#include <linux/videodev2.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION (3, 0, 36)
#include <linux/uvcvideo.h>
#endif

#define XU_SONIX_SYS_ASIC_RW	      			0x01

#if LINUX_VERSION_CODE > KERNEL_VERSION (3, 0, 36)
#define UVC_SET_CUR					0x01
#define UVC_GET_CUR					0x81
#define UVCIOC_CTRL_MAP		_IOWR('u', 0x20, struct uvc_xu_control_mapping)
#define UVCIOC_CTRL_QUERY	_IOWR('u', 0x21, struct uvc_xu_control_query)
#else
#define UVCIOC_CTRL_ADD		_IOW('U', 1, struct uvc_xu_control_info)
#define UVCIOC_CTRL_MAP		_IOWR('U', 2, struct uvc_xu_control_mapping)
#define UVCIOC_CTRL_GET		_IOWR('U', 3, struct uvc_xu_control)
#define UVCIOC_CTRL_SET		_IOW('U', 4, struct uvc_xu_control)
#endif

BOOL XU_OpenCamera(char *devPath);
BOOL XU_CloseCamera();

BOOL XU_RestartDevice();

BOOL XU_ReadFromASIC(USHORT addr, BYTE *pValue);
BOOL XU_WriteToASIC(USHORT addr, BYTE value);

BYTE XU_GetUVCExtendUnitID();
BOOL XU_GetChipID(LONG idAddr, BYTE *pChipID);
DSP_ROM_TYPE XU_GetChipRomType(BYTE *pChipID, DSP_ARCH_TYPE *pAsicArchType);

BOOL XU_CustomReadFromSensor(BYTE slaveID, USHORT addr, BYTE addrByteNum, USHORT *pData, BYTE dataByteNum, bool pollSCL);
BOOL XU_CustomWriteToSensor(BYTE slaveID, USHORT addr, BYTE addrByteNum, USHORT data, BYTE dataByteNum, bool pollSCL);

BOOL XU_ReadDataFormFlash(LONG addr, BYTE pData[], BYTE dataLen);
BOOL XU_ReadFormSF(LONG addr, BYTE pData[], LONG len);

BOOL XU_WriteDataToFlash(LONG addr, BYTE pData[], BYTE dataLen);

BOOL XU_GetAsicRomVersion(BYTE data[]);
BOOL XU_DefGetAsicRomVersion(BYTE data[]);
BOOL XU_ReadFromROM(LONG addr, BYTE data[]);

BOOL XU_EnableAsicRegisterBit(LONG addr, BYTE bit);
BOOL XU_DisableAsicRegisterBit(LONG addr, BYTE bit);

BOOL XU_Read(unsigned char pData[], unsigned int length, BYTE unitID, BYTE cs);
BOOL XU_Write(unsigned char pData[], unsigned int length, BYTE unitID, BYTE cs);

BOOL XU_GetSerialFlashType(SERIAL_FLASH_TYPE *sft, bool check);

BOOL XU_DisableSerialFlashWriteProtect(SERIAL_FLASH_TYPE sft);

BOOL XU_EraseSectorForSerialFlash(LONG addr, SERIAL_FLASH_TYPE sft);
BOOL XU_EraseBlockForSerialFlash(LONG addr, SERIAL_FLASH_TYPE sft);
BOOL XU_SerialFlashErase(SERIAL_FLASH_TYPE sft);

BOOL XU_GetMemType(BYTE *pMemType);
BOOL XU_SFWaitReady();
BOOL XU_SFCMDreadStatus();

BOOL XU_GetParaTableAndCRCAddrFormFW(BYTE *pFW, ULONG* paraTableStartAddr, ULONG* paraTableEndAddr, ULONG* crcAddr);
BOOL XU_GetParaTableAndCRCAddrFormSF(ULONG *paraTableStartAddr, ULONG *paraTableEndAddr, ULONG *crcAddr);
BOOL XU_GetStringSettingFormSF(BYTE* pbyString, DWORD stringSize, DWORD StringOffset, BOOL bIsCRCProtect);

#endif





