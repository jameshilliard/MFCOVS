
#include "XUOptDev.h"
#include "util.h"
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include "SFData.h"
#include <string.h>

int fd = 0;
extern BOOL camera_init;
BYTE g_curExtendUnitID = 0x03;

BOOL XU_OpenCamera(char *devPath)
{
	struct v4l2_capability cap;
	if((fd = open(devPath,O_RDWR | O_NONBLOCK)) < 0)
	{
		return FALSE;
	} 

	memset(&cap, 0, sizeof(cap));
	if(ioctl(fd,VIDIOC_QUERYCAP,&cap)<0)
	{
		printf("Error opening device %s : unable to query device.\n", devPath);
		close(fd);
		return FALSE;
	}
    //printf("Device %s opened: %s.\n",devPath, cap.card);

	USHORT ExtendUnitID = 0;
	if(!(ExtendUnitID = XU_GetUVCExtendUnitID()))
		return FALSE;
	g_curExtendUnitID = ExtendUnitID;

	BYTE chipID;
	DSP_ARCH_TYPE dspArchType;
	DSP_ROM_TYPE romType = XU_GetChipRomType(&chipID, &dspArchType);
	if(DRT_Unknow == romType){
		return FALSE;
	}

	return TRUE;
}

BOOL XU_CloseCamera()
{
	if(!camera_init)
		return FALSE;
	
	if(-1 == close(fd))
	{
    //	perror("Fail to close fd");
		return FALSE;
	}
	return TRUE;
}

int XU_Get_Cur(__u8 xu_unit, __u8 xu_selector, __u16 xu_size, __u8 *xu_data)
{
	int err=0;
#if LINUX_VERSION_CODE > KERNEL_VERSION (3, 0, 36)
	struct uvc_xu_control_query xctrl;
	xctrl.unit = xu_unit;
	xctrl.selector = xu_selector;
	xctrl.query = UVC_GET_CUR;
	xctrl.size = xu_size;
	xctrl.data = xu_data;
	err=ioctl(fd, UVCIOC_CTRL_QUERY, &xctrl);
#else
	struct uvc_xu_control xctrl;	
	xctrl.unit = xu_unit;
	xctrl.selector = xu_selector;
	xctrl.size = xu_size;
	xctrl.data = xu_data;
	err=ioctl(fd, UVCIOC_CTRL_GET, &xctrl);
#endif	
	return err;
}

int XU_Set_Cur(__u8 xu_unit, __u8 xu_selector, __u16 xu_size, __u8 *xu_data)
{
	int err=0;
#if LINUX_VERSION_CODE > KERNEL_VERSION (3, 0, 36)
	struct uvc_xu_control_query xctrl;
	xctrl.unit = xu_unit;
	xctrl.selector = xu_selector;
	xctrl.query = UVC_SET_CUR;
	xctrl.size = xu_size;
	xctrl.data = xu_data;
	err=ioctl(fd, UVCIOC_CTRL_QUERY, &xctrl);
#else
	struct uvc_xu_control xctrl;	
	xctrl.unit = xu_unit;
	xctrl.selector = xu_selector;
	xctrl.size = xu_size;
	xctrl.data = xu_data;
	err=ioctl(fd, UVCIOC_CTRL_SET, &xctrl);
#endif		
	return err;
}

BOOL XU_RestartDevice()
{
	BYTE romVersion[10] = { 0 };
	if (TRUE != XU_GetAsicRomVersion(romVersion))
	{
		return FALSE;
	}
	if ((uiRomID == ROM220) || (uiRomID == ROM225)){
		return FALSE;
	}

	BYTE TempVar = 0;
	if(XU_ReadFromASIC(usbResetAddr, &TempVar)){
		TempVar &= 0xFE;
		XU_WriteToASIC(usbResetAddr, TempVar);		//Always Error because of HW reset, Ignore it.
	}

	return FALSE;
}

BOOL XU_ReadFromASIC(USHORT addr, BYTE *pValue)
{
	int ret = 0;
	__u8 ctrldata[4];

	//uvc_xu_control parmeters
	__u8 xu_unit= g_curExtendUnitID; 
	__u8 xu_selector= XU_SONIX_SYS_ASIC_RW;
	__u16 xu_size= 4;
	__u8 *xu_data= ctrldata;

	xu_data[0] = (addr & 0xFF);
	xu_data[1] = ((addr >> 8) & 0xFF);
	xu_data[2] = 0x0;
	xu_data[3] = 0xFF;		/* Dummy Write */
	
	/* Dummy Write */
	if ((ret=XU_Set_Cur(xu_unit, xu_selector, xu_size, xu_data)) < 0) 
	{
		printf("ioctl(UVCIOC_CTRL_SET) FAILED (%i) \n",ret);
		//if(ret==EINVAL)			printf("Invalid arguments\n");		
		return FALSE;
	}
	
	/* Asic Read */
	xu_data[3] = 0x00;
	if ((ret=XU_Get_Cur(xu_unit, xu_selector, xu_size, xu_data)) < 0) 
	{
		printf("ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n",ret);
		//if(ret==EINVAL)			printf("Invalid arguments\n");
		return FALSE;
	}
	*pValue = xu_data[2];
	if(ret < 0)
		return FALSE;
	return TRUE;
}

BOOL XU_WriteToASIC(USHORT addr, BYTE value)
{
	int ret = 0;
	__u8 ctrldata[4];

	//uvc_xu_control parmeters
	__u8 xu_unit= g_curExtendUnitID; 
	__u8 xu_selector= XU_SONIX_SYS_ASIC_RW;
	__u16 xu_size= 4;
	__u8 *xu_data= ctrldata;

	xu_data[0] = (addr & 0xFF);			/* Addr Low */
	xu_data[1] = ((addr >> 8) & 0xFF);	/* Addr High */
	xu_data[2] = value;
	xu_data[3] = 0x0;					/* Normal Write */
	
	/* Normal Write */
	if ((ret=XU_Set_Cur(xu_unit, xu_selector, xu_size, xu_data)) < 0) 
	{
		printf("ioctl(UVCIOC_CTRL_SET) FAILED (%i) \n",ret);
		//if(ret==EINVAL)			printf("Invalid arguments\n");	
		return FALSE;	
	}
	
	if(ret < 0)
		return FALSE;
	return TRUE;
}


BOOL XU_SetAsicArchInfo(DSP_ARCH_TYPE asicArchType)
{
	if (asicArchType <= DAT_UNKNOW || asicArchType >= DSP_ARCH_COUNT)
		return FALSE;

	dspIdAddr = g_AsicArchInfo[asicArchType].asicIdAddr;
	sfRdyAddr = g_AsicArchInfo[asicArchType].sfRdyAddr;
	usbResetAddr = g_AsicArchInfo[asicArchType].usbResetAddr;
	gpioInputAddr = g_AsicArchInfo[asicArchType].gpioInputAddr;
	gpioOutputAddr = g_AsicArchInfo[asicArchType].gpioOutputAddr;
	gpioOEAddr = g_AsicArchInfo[asicArchType].gpioOEAddr;
	sfModeAddr = g_AsicArchInfo[asicArchType].sfModeAddr;
	sfCSAddr = g_AsicArchInfo[asicArchType].sfCSAddr;
	sfWriteDataAddr = g_AsicArchInfo[asicArchType].sfWriteDataAddr;
	sfReadDataAddr = g_AsicArchInfo[asicArchType].sfReadDataAddr;
	sfReadWriteTriggerAddr = g_AsicArchInfo[asicArchType].sfReadWriteTriggerAddr;

	i2cDev = g_AsicArchInfo[asicArchType].i2cDev;
	i2cMode = g_AsicArchInfo[asicArchType].i2cMode;
	i2cTrg = g_AsicArchInfo[asicArchType].i2cTrg;
	i2cSclSelOD = g_AsicArchInfo[asicArchType].i2cSclSelOD;
	i2cSlaveID = g_AsicArchInfo[asicArchType].i2cSlaveID;
	i2cDataArrStartAddr = g_AsicArchInfo[asicArchType].i2cDataArrStartAddr;

	return TRUE;
}


BOOL XU_GetChipID(LONG idAddr, BYTE *pChipID)
{
	BYTE id = 0;
	BOOL hr = TRUE;
	int i;
	for (i = 0; i < 3; i++)
	{
		hr = XU_ReadFromASIC(idAddr, &id);
		if (TRUE == hr)
			break;
	}

	if (TRUE != hr)
		return FALSE;

	*pChipID = id;
	return TRUE;
}

DSP_ROM_TYPE XU_GetChipRomType(BYTE *pChipID, DSP_ARCH_TYPE *pAsicArchType)
{
	DSP_ROM_TYPE ret = DRT_Unknow;
	BYTE dspID = 0;
	DSP_ARCH_TYPE dspArchType = DAT_UNKNOW;
	BYTE idIndex;
	for (idIndex = 0; idIndex < DSP_ARCH_COUNT; idIndex++)
	{
		if (!XU_GetChipID(g_AsicArchInfo[idIndex].asicIdAddr, &dspID))
			return ret;

		switch (dspID)
		{
		case 0x15:
		case 0x16:
		case 0x22:
		case 0x23:
		case 0x25:
		case 0x32:
		case 0x33:
		case 0x56:
		case 0x70:
		case 0x71:
		case 0x75:
		case 0x87:
		case 0x88:
		case 0x90:
			dspArchType = DAT_FIRST;
			ret = DRT_64K; //64k rom;
			break;
		case 0x76:
		case 0x92:
		case 0x83:
			dspArchType = DAT_FIRST;
			ret = DRT_128K; //128k rom;
			break;
		case 0x85:
			dspArchType = DAT_SECOND;
			ret = DRT_128K; //128k rom;
			break;
		case 0x86:
			dspArchType = DAT_SECOND;
			ret = DRT_256K;
			break;
		case 0x67:
			dspArchType = DAT_FIRST;
			ret = DRT_32K; // 32k rom
			break;
		default:
			ret = DRT_Unknow; //unkonw asic
			break;
		}
		if (ret != DRT_Unknow)
		{
			*pChipID = dspID;
			*pAsicArchType = dspArchType;
			XU_SetAsicArchInfo(dspArchType);
			return ret;
		}
	}
	return ret;
}

BOOL XU_CustomReadFromSensor(BYTE slaveID, USHORT addr, BYTE addrByteNum, USHORT *pData, BYTE dataByteNum,  bool pollSCL)
{
	if (dataByteNum == 0)
		return FALSE;

	if (addrByteNum > 2) addrByteNum = 2;
	if (dataByteNum > 2) dataByteNum = 2;

	// IIC dummy write
	USHORT dataBuffer = 0;
	if (!XU_CustomWriteToSensor(slaveID, addr, addrByteNum, dataBuffer, 0, pollSCL))
		return FALSE;

	if (!XU_WriteToASIC(i2cMode, 0x01))
		return FALSE;
	BYTE status;
	if (pollSCL)
	{
		XU_ReadFromASIC(i2cMode, &status);
		XU_WriteToASIC(i2cMode, status | 0x03);
	}
	if (!XU_WriteToASIC(i2cSclSelOD, 0x01))	// SCL_SEL_OD=0
		return FALSE;
	BYTE I2C_Speed = 0;
	if (!XU_ReadFromASIC(i2cDev, &I2C_Speed))
		return FALSE;
	if (I2C_Speed & 0x01){
		if (!XU_WriteToASIC(i2cDev, 0x83 | (dataByteNum << 4)))	// I2C_DEV=1, I2C_SEL_RD=1(R)
			return FALSE;
	}
	else{
		if (!XU_WriteToASIC(i2cDev, 0x82 | (dataByteNum << 4)))	// I2C_DEV=1, I2C_SEL_RD=1(R)
			return FALSE;
	}

	if (!XU_WriteToASIC(i2cSlaveID, slaveID))
		return FALSE;
	if (!XU_WriteToASIC(i2cDataArrStartAddr, 0x00))
		return FALSE;
	if (!XU_WriteToASIC(i2cDataArrStartAddr + 1, 0x00))
		return FALSE;
	if (!XU_WriteToASIC(i2cDataArrStartAddr + 2, 0x00))
		return FALSE;
	if (!XU_WriteToASIC(i2cDataArrStartAddr + 3, 0x00))
		return FALSE;
	if (!XU_WriteToASIC(i2cDataArrStartAddr + 4, 0x00))
		return FALSE;
	if (!XU_WriteToASIC(i2cTrg, 0x10))	// I2C_RW_TRG=1
		return FALSE;
	// wait I2C ready (time-out 10ms)
	BYTE value = 0x00;
	int i;
	for (i = 0; (i < 10) && !(value & 0x04); ++i)
	{
		if (!XU_ReadFromASIC(i2cDev, &value))
			return FALSE;
		usleep(1000);
	}
	if ((value & 0x0C) != 0x04){
		return FALSE;
	}

	if (!XU_ReadFromASIC(i2cDataArrStartAddr, &value))
		return FALSE;
	if (!XU_ReadFromASIC(i2cDataArrStartAddr + 1, &value))
		return FALSE;
	if (!XU_ReadFromASIC(i2cDataArrStartAddr + 2, &value))
		return FALSE;
	if (!XU_ReadFromASIC(i2cDataArrStartAddr + 3, &value))
		return FALSE;
	if (dataByteNum == 2)
		*pData = (USHORT)value << 8;
	if (!XU_ReadFromASIC(i2cDataArrStartAddr + 4, &value))
		return FALSE;
	if (dataByteNum == 2)
		*pData |= value;
	else if (dataByteNum == 1)
		*pData = value;

    return TRUE;
}

BOOL XU_CustomWriteToSensor(BYTE slaveID, USHORT addr, BYTE addrByteNum, USHORT data, BYTE dataByteNum, bool pollSCL)
{
	USHORT i2c_data_addr;
	BYTE status;

	if (!XU_WriteToASIC(i2cMode, 0x01))
		return FALSE;
	if (pollSCL)
	{
		XU_ReadFromASIC(i2cMode, &status);
		XU_WriteToASIC(i2cMode, status | 0x03);
	}
	if (!XU_WriteToASIC(i2cSclSelOD, 0x01))
		return FALSE;
	BYTE I2C_Speed = 0;
	if (!XU_ReadFromASIC(i2cDev, &I2C_Speed))
		return FALSE;
	if (I2C_Speed & 0x01){
		if (!XU_WriteToASIC(i2cDev, 0x81 | ((addrByteNum + dataByteNum) << 4)))	// I2C_DEV=1, I2C_SEL_RD=0(W)
			return FALSE;
	}
	else{
		if (!XU_WriteToASIC(i2cDev, 0x80 | ((addrByteNum + dataByteNum) << 4)))	// I2C_DEV=1, I2C_SEL_RD=0(W)
			return FALSE;
	}
	if (!XU_WriteToASIC(i2cSlaveID, slaveID))
		return FALSE;

	//write addr
	i2c_data_addr = i2cDataArrStartAddr;
	if (addrByteNum > 1){
		if (!XU_WriteToASIC(i2c_data_addr++, (BYTE)(addr >> 8)))
			return FALSE;
		if (!XU_WriteToASIC(i2c_data_addr++, (BYTE)addr))
			return FALSE;
	}
	else{
		if (!XU_WriteToASIC(i2c_data_addr++, (BYTE)addr))
			return FALSE;
	}

	// write data
	if (dataByteNum > 1)
	{
		if (!XU_WriteToASIC(i2c_data_addr++, (BYTE)(data >> 8)))
			return FALSE;
		if (!XU_WriteToASIC(i2c_data_addr++, (BYTE)data))
			return FALSE;
	}
	else{
		if (!XU_WriteToASIC(i2c_data_addr++, (BYTE)data))
			return FALSE;
	}

	while (i2c_data_addr < (i2cDataArrStartAddr + 5)){
		if (!XU_WriteToASIC(i2c_data_addr++, 0x00))
			return FALSE;
	}


	//trigger to start i2c interface read/write
	if (!XU_WriteToASIC(i2cTrg, 0x10))
		return FALSE;

	// wait I2C ready (time-out 10ms)
	BYTE value = 0x00;
	int i;
	for (i = 0; (i < 10) && !(value & 0x04); ++i)
	{
		if (!XU_ReadFromASIC(i2cDev, &value))
			return FALSE;
		usleep(1000);
	}

	if ((value & 0x0C) == 0x04)
		return TRUE;
	else
		return FALSE;
}


BOOL XU_ReadDataFormFlash(LONG addr, BYTE pData[], BYTE dataLen)
{
	__u8 ctrldata[11]={0};
	__u8 xu_unit= g_curExtendUnitID; 
	__u8 xu_selector= 0x03;
	__u16 xu_size= 11;
	__u8 *xu_data= ctrldata;

	xu_data[0] = (BYTE)((addr << 8) >> 8);
	xu_data[1] = (BYTE)(addr >> 8);
	BYTE temp;
	if (addr < 0x10000) 
		temp = 0x88;
	else if (addr < 0x20000) 
		temp = 0x98;
	else if (addr < 0x30000) 
		temp = 0xA8;
	else 
		temp = 0xB8;
	xu_data[2]  = (temp & 0xF0) | dataLen;

	if (XU_Set_Cur(xu_unit, xu_selector, xu_size, xu_data) < 0) 
		return FALSE;

	//memset(xu_data, 0, xu_size);
    if (XU_Get_Cur(xu_unit, xu_selector, xu_size, xu_data) < 0) 
    	return FALSE;

	memcpy(pData, xu_data+3, dataLen);	
	return TRUE;
}

BOOL XU_WriteDataToFlash(LONG addr, BYTE pData[], BYTE dataLen)
{
	__u8 ctrldata[11]={0};
	__u8 xu_unit= g_curExtendUnitID; 
	__u8 xu_selector= 0x03;
	__u16 xu_size= 11;
	__u8 *xu_data= ctrldata;

	if(dataLen > 8)
		dataLen = 8;

	xu_data[0] = (BYTE)((addr << 8) >> 8);
	xu_data[1] = (BYTE)(addr >> 8);
	if (addr < 0x10000)
		xu_data[2] = 0x08;
	else if (addr < 0x20000)
		xu_data[2] = 0x18;
	else if (addr < 0x30000)
		xu_data[2] = 0x28;
	else
		xu_data[2]= 0x38;
	xu_data[2] &= 0xf0;
	xu_data[2] |= dataLen;

	memcpy(xu_data + 3, pData, dataLen);
	if (XU_Set_Cur(xu_unit, xu_selector, xu_size, xu_data) < 0) 
		return FALSE;

    return TRUE;
}



BOOL XU_ReadFormSF(LONG addr, BYTE pData[], LONG len)
{
	BYTE tempData[8];
	LONG startAddr = addr;
	LONG loop = len / 8;
	LONG ram = len % 8;
	BOOL hr = TRUE;
	LONG addrIndex = 0;
	LONG i;
	for (i = 0; i < loop; i++){
		memset(&tempData, 0xff, 8);
		hr = XU_ReadDataFormFlash(startAddr, tempData, 8);
		if (TRUE != hr){
			return hr;
		}
		memcpy(pData + addrIndex, tempData, 8);
		addrIndex += 8;
		startAddr += 8;
	}

	if (ram > 0){
		memset(&tempData, 0xff, 8);
		hr = XU_ReadDataFormFlash(startAddr, tempData, ram);
		if (TRUE != hr){
			return hr;
		}
		memcpy(pData + addrIndex, tempData, ram);
	}

	return TRUE;
}

BYTE XU_GetUVCExtendUnitID()
{
	BYTE chipID;
	DSP_ARCH_TYPE dspArchType;

	g_curExtendUnitID = 0x03;
	DSP_ROM_TYPE romType = XU_GetChipRomType(&chipID, &dspArchType);
	if(DRT_Unknow != romType){
		return 0x3;
	 }

	g_curExtendUnitID = 0x04;
	romType = XU_GetChipRomType(&chipID, &dspArchType);
	if(DRT_Unknow != romType)
		return 0x4;
	
	printf("Get uvc extend unit id fail\n");
	return 0;
}

BOOL XU_ReadFromROM(LONG addr, BYTE data[])
{
	unsigned int remain = 0, rd_size, offset;
	__u8 ctrldata[11]={0};
	__u8 xu_unit= g_curExtendUnitID; 
	__u8 xu_selector= 0x04;
	__u16 xu_size= 11;
	__u8 *xu_data= ctrldata;

	xu_data[0] = (BYTE)((addr << 8) >> 8);
	xu_data[1] = (BYTE)(addr >> 8);
	xu_data[2]  = 8;

	if (XU_Set_Cur(xu_unit, xu_selector, xu_size, xu_data) < 0) 
		return FALSE;

    if (XU_Get_Cur(xu_unit, xu_selector, xu_size, xu_data) < 0) 
    	return FALSE;

	memcpy(data, xu_data + 3, 8);	

	return TRUE;
}


BOOL XU_DefGetAsicRomVersion(BYTE data[])
{
	BYTE romString[8];
	int i = 0, j = 0;
	for (i = 0; i < ROMSTRADDRCNT; i++)
	{
		XU_ReadFromROM(RomStringAddr[i], romString);
		for (j = 0; j < ROMCOUNT; j++)
		{
			if (RomInfo[j].RomStringAddr == RomStringAddr[i])
			{
				if (RomInfo[j].IsNewestVer)
				{
					if (memcmp(romString, RomInfo[j].RomString, 4) == 0 &&
						romString[5] >= RomInfo[j].RomString[5])
					{
						memcpy(data, romString, 8);
						uiRomID = j;	// shawn 2010/05/14 add
						return TRUE;
					}
				}
				else
				{
					if (memcmp(romString, RomInfo[j].RomString, 4) == 0 &&	// shawn 2009/06/10 modify
						romString[5] == RomInfo[j].RomString[5])				// shawn 2010/04/12 only compare 6 bytes
					{
						memcpy(data, romString, 8);
						uiRomID = j;	// shawn 2010/05/14 add
						return TRUE;
					}
				}
			}

		}
	}
	data = 0;
	return FALSE;
}


BOOL XU_GetAsicRomVersion(BYTE data[])
{
	if (TRUE == XU_DefGetAsicRomVersion(data)){
		if (uiRomID == ROM276V1){
			BYTE byTmp1 = 0;
			BYTE byTmp2 = 0;
			XU_ReadFromASIC(0x1185, &byTmp1);
			byTmp1 |= 0x70;
			XU_WriteToASIC(0x1185, byTmp1);
			XU_ReadFromASIC(0x1185, &byTmp2);
			if ((byTmp2 & 0x70) == (byTmp1 & 0x70)){
				data[4] = 0x31;
			}
		}
		if (uiRomID == ROM288V1){
			BYTE byTmp1 = 0;
			BYTE byTmp2 = 0;
			XU_ReadFromASIC(0x101f, &byTmp1);
			if (byTmp1 == 0x89){
				data[2] = 0x39;
				uiRomID = ROM289V1;
			}
			else{
				byTmp1 = 0;
				XU_ReadFromASIC(0x1007, &byTmp1);
				byTmp1 &= 0xDF;
				XU_ReadFromASIC(0x1006, &byTmp2);
				byTmp2 |= 0x20;
				XU_WriteToASIC(0x1007, byTmp1);
				XU_WriteToASIC(0x1006, byTmp2);
				byTmp1 = 0;
				XU_ReadFromASIC(0x1005, &byTmp1);
				if ((byTmp1 & 0x20) == 0x20){
					data[4] = 0x31;
				}
				else{
					byTmp2 &= 0xDF;
					XU_WriteToASIC(0x1006, byTmp2);
				}
			}
		}
		if (uiRomID == ROM271V1){
			BYTE byTmp1 = 0;
			BYTE byTmp2 = 0;
			BYTE byTmp3 = 0;
			XU_ReadFromASIC(0x100A, &byTmp1);
			byTmp2 = (byTmp1 & 0x10);
			byTmp1 &= 0xEF;
			XU_WriteToASIC(0x100A, byTmp1);
			XU_ReadFromASIC(0x101F, &byTmp3);
			byTmp1 |= byTmp2;
			XU_WriteToASIC(0x100A, byTmp1);
			if (byTmp3 == 0x70){
				data[2] = 0x30;
				uiRomID = ROM270V1;
			}
			byTmp1 = 0;
			XU_ReadFromASIC(0x1007, &byTmp1);
			byTmp1 &= 0xF7;
			byTmp2 = 0;
			XU_ReadFromASIC(0x1006, &byTmp2);
			byTmp2 |= 0x08;
			XU_WriteToASIC(0x1007, byTmp1);
			XU_WriteToASIC(0x1006, byTmp2);
			byTmp1 = 0;
			XU_ReadFromASIC(0x1005, &byTmp1);
			if ((byTmp1 & 0x08) == 0x08){
				data[4] = 0x31;	// 270M, 271M
				byTmp2 &= 0xF7;
				XU_WriteToASIC(0x1006, byTmp2);
			}
			else{
				byTmp1 = 0;
				XU_ReadFromASIC(0x1007, &byTmp1);
				byTmp1 &= 0xEF;
				byTmp2 = 0;
				XU_ReadFromASIC(0x1006, &byTmp2);
				byTmp2 |= 0x10;
				XU_WriteToASIC(0x1007, byTmp1);
				XU_WriteToASIC(0x1006, byTmp2);
				byTmp1 = 0;
				XU_ReadFromASIC(0x1005, &byTmp1);

				if ((byTmp1 & 0x10) == 0x10){
					// 270A, 271A
					data[4] = 0x30;
				}
				else{
					if (uiRomID == ROM270V1){
						data[4] = 0x32;	// 270B
						byTmp2 &= 0xEF;
						XU_WriteToASIC(0x1006, byTmp2);
					}
				}
			}
		}
		if (uiRomID == ROM281V1){
			BYTE byTmp1 = 0;
			BYTE byTmp2 = 0;
			BYTE byTmp3 = 0;
			XU_ReadFromASIC(0x100A, &byTmp1);
			byTmp2 = (byTmp1 & 0x10);
			byTmp1 &= 0xEF;
			XU_WriteToASIC(0x100A, byTmp1);
			XU_ReadFromASIC(0x101F, &byTmp3);
			byTmp1 |= byTmp2;
			XU_WriteToASIC(0x100A, byTmp1);
			if (byTmp3 == 0x80){
				data[2] = 0x30;
				uiRomID = ROM280V1;
			}
			byTmp1 = 0;
			XU_ReadFromASIC(0x1007, &byTmp1);
			byTmp1 &= 0xF7;
			byTmp2 = 0;
			XU_ReadFromASIC(0x1006, &byTmp2);
			byTmp2 |= 0x08;
			XU_WriteToASIC(0x1007, byTmp1);
			XU_WriteToASIC(0x1006, byTmp2);
			byTmp1 = 0;
			XU_ReadFromASIC(0x1005, &byTmp1);
			if ((byTmp1 & 0x08) == 0x08){
				data[4] = 0x31;	// 280M, 281M
				byTmp2 &= 0xF7;
				XU_WriteToASIC(0x1006, byTmp2);
			}
			else{
				data[4] = 0x30;	// 280A, 281A
			}
		}
		return TRUE;
	}
	data = 0;
	return FALSE;
}

BOOL XU_EnableAsicRegisterBit(LONG addr, BYTE bit)
{
	BYTE bufs;
	BYTE bufd;
	if (!XU_ReadFromASIC(addr, &bufs))
		return FALSE;
	switch (bit)
	{
	case 0:
		bufd = bufs | 0x01;
		break;
	case 1:
		bufd = bufs | 0x02;
		break;
	case 2:
		bufd = bufs | 0x04;
		break;
	case 3:
		bufd = bufs | 0x08;
		break;
	case 4:
		bufd = bufs | 0x10;
		break;
	case 5:
		bufd = bufs | 0x20;
		break;
	case 6:
		bufd = bufs | 0x40;
		break;
	case 7:
		bufd = bufs | 0x80;
		break;
	default:
		break;
	}
	return XU_WriteToASIC(addr, bufd);
}

BOOL XU_DisableAsicRegisterBit(LONG addr, BYTE bit)
{
	BYTE bufs;
	BYTE bufd;

	if (!XU_ReadFromASIC(addr, &bufs))
		return FALSE;
	switch (bit)
	{
	case 0:
		bufd = bufs & 0xfe;
		break;
	case 1:
		bufd = bufs & 0xfd;
		break;
	case 2:
		bufd = bufs & 0xfb;
		break;
	case 3:
		bufd = bufs & 0xf7;
		break;
	case 4:
		bufd = bufs & 0xef;
		break;
	case 5:
		bufd = bufs & 0xdf;
		break;
	case 6:
		bufd = bufs & 0xbf;
		break;
	case 7:
		bufd = bufs & 0x7f;
		break;
	default:
		break;
	}
	return XU_WriteToASIC(addr, bufd);
}


BOOL XU_Read(unsigned char pData[], unsigned int length, BYTE unitID, BYTE cs)
{
	int ret = 0;
	__u8 xu_unit= unitID; 
	__u8 xu_selector= cs;
	__u16 xu_size= length;
	__u8 *xu_data= pData;

	if ((ret=XU_Get_Cur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
		return FALSE;
	}
	if(ret < 0)
		return FALSE;
	return TRUE;
}

BOOL XU_Write(unsigned char pData[], unsigned int length,  BYTE unitID, BYTE cs)
{
	int ret = 0;
	__u8 xu_unit= unitID; 
	__u8 xu_selector= cs;
	__u16 xu_size= length;
	__u8 *xu_data= pData;
	
	if ((ret=XU_Set_Cur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
		return FALSE;	
	}
	return TRUE;
}

void XU_ReadSFID(BYTE cmd, BYTE dummyNum, BYTE devIdNum)
{
	LONG data;
	data = 0x1;
	XU_WriteToASIC(0x1080, data);
	data = 0x0;
	XU_WriteToASIC(0x1091, data);
	data = cmd;
	XU_WriteToASIC(0x1082, data);
	data = 0x01;
	XU_WriteToASIC(0x1081, data);
	XU_SFWaitReady();

	while (dummyNum > 0)
	{
		data = 0x00;
		XU_WriteToASIC(0x1082, data);
		data = 0x01;
		XU_WriteToASIC(0x1081, data);
		XU_SFWaitReady();
		dummyNum--;
	}

	data = 0x0;
	XU_WriteToASIC(0x1083, data);
	data = 0x02;
	XU_WriteToASIC(0x1081, data);
	XU_SFWaitReady();
	XU_ReadFromASIC(0x1083, &sfManufactureID);
	if (sfManufactureID == SF_MFRID_CONT)
	{
		data = 0x0;
		XU_WriteToASIC(0x1083, data);
		data = 0x02;
		XU_WriteToASIC(0x1081, data);
		XU_SFWaitReady();
		XU_ReadFromASIC(0x1083, &sfManufactureID);
	}

	data = 0x0;
	XU_WriteToASIC(0x1083, data);
	data = 0x02;
	XU_WriteToASIC(0x1081, data);
	XU_SFWaitReady();
	XU_ReadFromASIC(0x1083, &sfDeviceID1);
	if (devIdNum == 2)
	{
		data = 0x0;
		XU_WriteToASIC(0x1083, data);
		data = 0x02;
		XU_WriteToASIC(0x1081, data);
		XU_SFWaitReady();
		XU_ReadFromASIC(0x1083, &sfDeviceID2);
	}
	else
		sfDeviceID2 = 0xFF;

	data = 0x00;
	XU_WriteToASIC(0x1080, data);
}

SERIAL_FLASH_TYPE XU_SerialFlashSearch()
{
	BYTE i, ubID_Num;
	BYTE ubSFType = SF_UNKNOW;
	ubID_Num = ubSFLib_GetIDSize();
	for (i = 1; i<ubID_Num; i++)
	{
		if (cbSFLib_ID[i][SFCMD_INFO_MFR] == sfManufactureID &&
			(cbSFLib_ID[i][SFCMD_INFO_DEVID1] == sfDeviceID1 || cbSFLib_ID[i][SFCMD_INFO_DEVID1] == 0xFF) &&
			(cbSFLib_ID[i][SFCMD_INFO_DEVID2] == sfDeviceID2 || cbSFLib_ID[i][SFCMD_INFO_DEVID2] == 0xFF))
		{
			break;
		}
	}
	if (i == ubID_Num)
		i = 0;
	ubSFType = cbSFLib_ID[i][SFCMD_INFO_TYPE];
	return (SERIAL_FLASH_TYPE)ubSFType;
}

SERIAL_FLASH_TYPE XU_SerialFlashIdentify()
{
	// MXIC-like series
	XU_ReadSFID(SFCMD_RDID_MXIC, 0, 2);
	SERIAL_FLASH_TYPE sfType = XU_SerialFlashSearch();
	if (sfType != SF_UNKNOW)
		goto sfIndetifyExit;

	// Atmel AT25F series
	XU_ReadSFID(SFCMD_RDID_AT25F, 0, 1);
	sfType = XU_SerialFlashSearch();
	if (sfType != SF_UNKNOW)
		goto sfIndetifyExit;

	// SST/Winbond/Other MXIC-like
	XU_ReadSFID(SFCMD_REMS_SST, 3, 1);
	sfType = XU_SerialFlashSearch();
	if (sfType != SF_UNKNOW)
		goto sfIndetifyExit;
	
	// ST/PMC
	XU_ReadSFID(SFCMD_RES_ST, 3, 1);
	sfType = XU_SerialFlashSearch();
	if (sfType != SF_UNKNOW)
		goto sfIndetifyExit;
	
sfIndetifyExit:
	return sfType;
}

BOOL XU_GetSerialFlashType(SERIAL_FLASH_TYPE *sft, bool check)
{
	*sft = XU_SerialFlashIdentify();
	BYTE sfType = 0;
	if(check){
		if(sft == SFT_UNKNOW){
			BYTE romVersion[8] = { 0 };
			if (!XU_GetAsicRomVersion(romVersion)){
				return FALSE;
			}
			BYTE chipID = 0;
			DSP_ARCH_TYPE archType = DAT_UNKNOW;
			if(DRT_Unknow == XU_GetChipRomType(&chipID, &archType)){
				return FALSE;
			}
			if (!XU_ReadFromASIC(RomInfo[uiRomID].SFTypeAddr, &sfType))
				return FALSE;
			if (sfType <= SFT_FENTECH)
				*sft = (SERIAL_FLASH_TYPE)sfType;
			else
				return FALSE;
		}
	}
	return TRUE;
}

BOOL XU_DisableSerialFlashWriteProtect(SERIAL_FLASH_TYPE sft)
{
	if (sft == SFT_UNKNOW)
	{
		return FALSE;
	}

	BYTE romVersion[8] = { 0 };
	if (TRUE != XU_GetAsicRomVersion(romVersion))
	{
		return FALSE;
	}
	
	BYTE chipID;
	DSP_ARCH_TYPE dspArchType;
	DSP_ROM_TYPE drt = XU_GetChipRomType(&chipID, &dspArchType);
	if (DRT_Unknow == drt)
	{
		return FALSE;
	}

	//setp 1 : disable hardware wirete protect
	BYTE data[2] = { 0 };
	if (drt == DRT_64K)
	{
		XU_ReadFromASIC(gpioOutputAddr, &data[0]);
		data[0] = data[0] | 0x1F;
		XU_WriteToASIC(gpioOutputAddr, data[0]);
		XU_ReadFromASIC(gpioOutputAddr, &data[0]);
		XU_ReadFromASIC(gpioOEAddr, &data[1]);
		data[1] = data[1] | 0x1F;
		XU_WriteToASIC(gpioOEAddr, data[1]);
	}
	else
	{
		XU_ReadFromASIC(gpioOutputAddr, &data[0]);
		data[0] = data[0] | 0xFF;
		XU_WriteToASIC(gpioOutputAddr, data[0]);
		XU_ReadFromASIC(gpioOutputAddr, &data[0]);
		XU_ReadFromASIC(gpioOEAddr, &data[1]);
		data[1] = data[1] | 0xFF;
		XU_WriteToASIC(gpioOEAddr, data[1]);
	}

	BYTE wpData = 0;
	switch (chipID)
	{
	case 0x33:
		XU_ReadFormSF(0x000f, &wpData, 1);
		break;
	case 0x32:
	case 0x76:
		XU_ReadFormSF(0xC034, &wpData, 1);
		break;
	case 0x16:
		XU_ReadFormSF(0x5834, &wpData, 1);
		break;
	case 0x71:
	case 0x85:
		wpData = 0xFF;
		break;
	case 0x75:
		XU_ReadFormSF(0xB034, &wpData, 1);
		break;
	default:
		XU_ReadFormSF(0x8034, &wpData, 1);
		break;
	}
	

	BYTE byTmpAddr = 0;
	BYTE byMemType = 0;
	BYTE wpGPIO = 0;
	BOOL isNewWPVer = FALSE;
	USHORT wpAddr = 0xFFFF;
	BYTE sfWriteEnable = 0;
	BYTE sfWriteCommand = 0;

	if (RomInfo[uiRomID].IsCompactMode)	// shawn 2010/05/14 modify
	{
		XU_GetMemType(&byMemType);
	}

	wpGPIO = (wpData >> 4) & 0x7;
	if ((uiRomID != ROM283) && (uiRomID != ROM292) && (uiRomID != ROM275V2) && (uiRomID != ROM287)){
		if ((wpData & 0x0C) == 0x08){
			isNewWPVer = TRUE;
		}
	}
	if (isNewWPVer){
		if ((wpData & 0x03) == 0x02){
			wpData = 1;
		}
		else if ((wpData & 0x03) == 0x03){
			wpData = 2;
		}
	}
	else{
		wpData = wpData & 0x03;
	}
	if (RomInfo[uiRomID].IsDisSFWriteCmd)
	{
		if (!XU_ReadFromASIC(0x05F3, &sfWriteEnable)){
			return FALSE;
		}
		if (isNewWPVer)
		{
			if (!XU_WriteToASIC(0x05F3, cbSFLib_Cmd[ubSFLib_CmdID][SFCMD_IDX_WREN]))	// SF write enable
				return FALSE;
		}
		else
		{
			if (!XU_WriteToASIC(0x05F3, 0x06))	// SF write enable(ubSFWREN)
				return FALSE;
		}
		//// SF write enable(ubSFWREN)
		//if (!XU_WriteToASIC(0x05F3, 0x06)){
		//	return FALSE;
		//}
		// Memkey1 low
		if (!XU_WriteToASIC(0x05F8, 0x12))	{
			return FALSE;
		}
		// MemKey1 high
		if (!XU_WriteToASIC(0x05F9, 0x12))	{
			return FALSE;
		}
		//memkey2 low
		if (!XU_WriteToASIC(0x05FA, 0xED))  {
			return FALSE;
		}
		//memkey2 high
		if (!XU_WriteToASIC(0x05FB, 0xED))  {
			return FALSE;
		}
		//ubsfwrite
		if (!XU_ReadFromASIC(0x05F5, &sfWriteCommand))  {
			return FALSE;
		}
		if (isNewWPVer){
			//SF write command
			if (!XU_WriteToASIC(0x05F5, cbSFLib_Cmd[ubSFLib_CmdID][SFCMD_IDX_PP])){
				return FALSE;
			}
		}
		else{
			if (sft == SFT_SST){
				//SF write command for SST
				if (!XU_WriteToASIC(0x05F5, 0xAF)){
					return FALSE;
				}
			}
			else{
				if (!XU_WriteToASIC(0x05F5, 0x02)){
					return FALSE;
				}
			}
		}
	}

	if (uiRomID == ROM292){
		if (wpData != 0){
			if (wpData == 1){
				if (wpGPIO < 8){
					wpData = 1 << wpGPIO;
					XU_WriteToASIC(0x1007, wpData);
					wpData = ~wpData;
					XU_WriteToASIC(0x1006, wpData);
					wpAddr = 0x1006;
				}
				else{ 
					wpData = 1 << (wpGPIO - 8);
					XU_WriteToASIC(0x100A, wpData);
					wpData = ~wpData;
					XU_WriteToASIC(0x1009, wpData);
					wpAddr = 0x1009;
				}
			}
			else{
				BYTE tmpVal = 0;
				if (wpGPIO < 8){
					wpData = 1 << wpGPIO;
					XU_WriteToASIC(0x1007, (wpData));
					XU_WriteToASIC(0x1006, (wpData));
					wpAddr = 0x1006;
				}
				else{
					wpData = 1 << (wpGPIO - 8);
					XU_WriteToASIC(0x100A, wpData);
					XU_WriteToASIC(0x1009, wpData);
					wpAddr = 0x1009;
				}
			}
		}
		BYTE TempVar = 0;
		XU_ReadFromASIC(0x1006, &TempVar);
		TempVar |= 0x04;
		XU_WriteToASIC(0x1006, TempVar);

		TempVar = 0;
		XU_ReadFromASIC(0x1007, &TempVar);
		TempVar |= 0x04;
		XU_WriteToASIC(0x1007, TempVar);

		TempVar = 0;
		XU_ReadFromASIC(0x17C1, &TempVar);
		TempVar |= 0x01;
		XU_WriteToASIC(0x17C1, TempVar);

		TempVar = 0;
		XU_ReadFromASIC(0x17C2, &TempVar);
		TempVar |= 0x01;
		XU_WriteToASIC(0x17C2, TempVar);
	}
	else if (uiRomID != ROM283){
		if (wpData != 0){
			// this firmware supports Write Protect
			USHORT wOutputEnableAddress = 0x1007;
			USHORT wOutputValueAddress = 0x1006;
			if (wpGPIO >= 8){
				wpGPIO -= 8;
				wOutputEnableAddress = 0x100A;
				wOutputValueAddress = 0x1009;
			}
			BYTE byOutputEnable = 0;
			BYTE byOutputValue = 0;
			if (!XU_ReadFromASIC(wOutputEnableAddress, &byOutputEnable)){
				return FALSE;
			}
			if (!XU_ReadFromASIC(wOutputValueAddress, &byOutputValue)){
				return FALSE;
			}
			BYTE byBitMask = 1 << wpGPIO;
			// Write Output Enable bit mask
			byOutputEnable |= byBitMask;
			if (!XU_WriteToASIC(wOutputEnableAddress, byOutputEnable)){
				return FALSE;
			}
			// Write Output Value bit mask
			// if (bWriteProtect == 1), High active protect, else, Low active protect
			if (wpData == 1){
				byOutputValue &= ~byBitMask;
			}
			else{
				byOutputValue |= byBitMask;
			}
			if (!XU_WriteToASIC(wOutputValueAddress, byOutputValue)){
				return FALSE;
			}
			wpAddr = wOutputValueAddress;
		}
	}
	
	if (RomInfo[uiRomID].IsCompactMode && byMemType == 2){
		return TRUE;
	}
	//disable write protect @ flash status register (bp0, bp1)
	if (!XU_WriteToASIC(sfModeAddr, 0x1)){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfCSAddr, 0x0)){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfWriteDataAddr, 0x06)){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfReadWriteTriggerAddr, 0x1)){
		return FALSE;
	}
	if (!XU_SFWaitReady()){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfCSAddr, 0x1)){
		return FALSE;
	}
	if (!XU_SFCMDreadStatus()){
		return FALSE;
	}
	if (sft == SFT_SST)
	{
		if (!XU_WriteToASIC(sfCSAddr, 0x0)){
			return FALSE;
		}
		// Enable-Write-Status-Register(EWSR)
		if (!XU_WriteToASIC(sfWriteDataAddr, 0x50)){
			return FALSE;
		}
		if (!XU_WriteToASIC(sfReadWriteTriggerAddr, 0x1)){
			return FALSE;
		}
		if (!XU_SFWaitReady()){
			return FALSE;
		}
		if (!XU_WriteToASIC(sfCSAddr, 0x1)){
			return FALSE;
		}
	}
	if (!XU_WriteToASIC(sfCSAddr, 0x0)){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfWriteDataAddr, 0x1)){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfReadWriteTriggerAddr, 0x1)){
		return FALSE;
	}
	if (!XU_SFWaitReady()){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfWriteDataAddr, 0x0)){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfReadWriteTriggerAddr, 0x1)){
		return FALSE;
	}
	if (!XU_SFWaitReady()){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfCSAddr, 0x1)){
		return FALSE;
	}
	if (!XU_SFCMDreadStatus()){
		return FALSE;
	}
	if (!XU_WriteToASIC(sfModeAddr, 0x0)){
		return FALSE;
	}
	if ((uiRomID == ROM283) || (uiRomID == ROM275V2) || (uiRomID == ROM287)){
		BYTE TempVar = 0;
		XU_ReadFromASIC(0x1006, &TempVar);
		TempVar |= 0x04;
		XU_WriteToASIC(0x1006, TempVar);
		TempVar = 0;
		XU_ReadFromASIC(0x1007, &TempVar);
		TempVar |= 0x04;
		XU_WriteToASIC(0x1007, TempVar);
	}
    return TRUE;
}

BOOL XU_EraseSectorForSerialFlash(LONG addr, SERIAL_FLASH_TYPE sft)
{
		// disable serial falsh write protect
	BYTE sectorEraseCode = 0x20;
	switch (sft)
	{
	case SFT_ST:
		sectorEraseCode = 0xd8;
		break;
	case SFT_SST:
	case SFT_MXIC:
	case SFT_GIGA:
	case SFT_WINBOND:
	case SFT_MXIC_LIKE:
	case SFT_ATMEL_AT25F:
	case SFT_ATMEL_AT25FS:
	case SFT_ATMEL_AT45DB:
	case SFT_AMIC:
	case SFT_EON:
	case SFT_FENTECH:
		sectorEraseCode = 0x20;
		break;
	case SFT_PMC:
		sectorEraseCode = 0xd7;
		break;
	case SFT_UNKNOW:
		return FALSE;	
		break;
	default:
		break;
	}

	BYTE data = 0x1;
	XU_WriteToASIC(sfModeAddr, data);//flash mode
	data = 0x0;
	XU_WriteToASIC(sfCSAddr, data);//CS=0
	data = 0x06;
	XU_WriteToASIC(sfWriteDataAddr, data);//WREN
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	data = 0x1;
	XU_WriteToASIC(sfCSAddr, data);//CS=1
	//sector erase
	data = 0x0;
	XU_WriteToASIC(sfCSAddr, data);//CS=0
	data = sectorEraseCode;
	XU_WriteToASIC(sfWriteDataAddr, data);	// for chip sector erase
	//SetRegData(0x1082,0x20);
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	//ldata = 0x00;
	data = addr >> 16;
	XU_WriteToASIC(sfWriteDataAddr, data);//addr
	//SetRegData(0x1082,0x00);
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	data = addr >> 8;
	XU_WriteToASIC(sfWriteDataAddr, data);
	//SetRegData(0x1082,SectorNum);
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	data = (BYTE)addr;//ldata = 0x00;
	XU_WriteToASIC(sfWriteDataAddr, data);
	//SetRegData(0x1082,0x00);
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	data = 0x1;
	XU_WriteToASIC(sfCSAddr, data);//CS=1
	//SF_CMDread_Status
	XU_SFWaitReady();//SF_CMDread_Status(iDevIndex);
	data = 0x0;
	XU_WriteToASIC(sfModeAddr, data);//flash mode disable
	return TRUE;
}

BOOL XU_EraseBlockForSerialFlash(LONG addr, SERIAL_FLASH_TYPE sft)
{
// disable serial falsh write protect
	BYTE sectorEraseCode = 0x52;
	switch (sft)
	{
	case SFT_ST:
	case SFT_GIGA:
	case SFT_FENTECH:
		sectorEraseCode = 0xd8;
		break;
	case SFT_SST:
	case SFT_MXIC:
	case SFT_WINBOND:
	case SFT_MXIC_LIKE:
	case SFT_ATMEL_AT25F:
	case SFT_ATMEL_AT25FS:
	case SFT_ATMEL_AT45DB:
	case SFT_AMIC:
	case SFT_EON:
		sectorEraseCode = 0x52;
		break;
	case SFT_PMC:
		sectorEraseCode = 0x52;
		break;
	case SFT_UNKNOW:
		return FALSE;
		break;
	default:
		break;
	}

	BYTE data = 0x1;
	XU_WriteToASIC(sfModeAddr, data);//flash mode
	data = 0x0;
	XU_WriteToASIC(sfCSAddr, data);//CS=0
	data = 0x06;
	XU_WriteToASIC(sfWriteDataAddr, data);//WREN
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	data = 0x1;
	XU_WriteToASIC(sfCSAddr, data);//CS=1
	//sector erase
	data = 0x0;
	XU_WriteToASIC(sfCSAddr, data);//CS=0
	data = sectorEraseCode;
	XU_WriteToASIC(sfWriteDataAddr, data);	// for chip sector erase
	//SetRegData(0x1082,0x20);
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	//ldata = 0x00;
	data = addr >> 16;
	XU_WriteToASIC(sfWriteDataAddr, data);//addr
	//SetRegData(0x1082,0x00);
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	data = addr >> 8;
	XU_WriteToASIC(sfWriteDataAddr, data);
	//SetRegData(0x1082,SectorNum);
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	data = (BYTE)addr;//ldata = 0x00;
	XU_WriteToASIC(sfWriteDataAddr, data);
	//SetRegData(0x1082,0x00);
	data = 0x01;
	XU_WriteToASIC(sfReadWriteTriggerAddr, data);
	XU_SFWaitReady();
	data = 0x1;
	XU_WriteToASIC(sfCSAddr, data);//CS=1
	//SF_CMDread_Status
	XU_SFWaitReady();//SF_CMDread_Status(iDevIndex);
	data = 0x0;
	XU_WriteToASIC(sfModeAddr, data);//flash mode disable
	return TRUE;
}

BOOL XU_SerialFlashErase(SERIAL_FLASH_TYPE sft)
{
	BOOL ret = TRUE;
	switch (sft){
	case SF_WINBOND:
	case SF_PMC:
	case SF_ST:
	case SF_AMIC:
		XU_WriteToASIC(sfModeAddr, 0x1);
		//SF_Set_WEL_Bit
		XU_WriteToASIC(sfCSAddr, 0x0);
		XU_WriteToASIC(sfWriteDataAddr, 0x06);
		XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01);
		XU_SFWaitReady();
		XU_WriteToASIC(sfCSAddr, 0x1);
		//chip erase
		XU_WriteToASIC(sfCSAddr, 0x0);
		XU_WriteToASIC(sfWriteDataAddr, 0xc7);	// for PMC chip erase
		XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01);
		XU_SFWaitReady();
		XU_WriteToASIC(sfCSAddr, 0x1);
		XU_SFCMDreadStatus();
		ret = XU_WriteToASIC(sfModeAddr, 0x0);
		break;
	case SF_SST:
		XU_WriteToASIC(sfModeAddr, 0x1);	// serial flash mode
		//SF_Set_EWSR_Bit
		XU_WriteToASIC(sfCSAddr, 0x0);	// chip select
		XU_WriteToASIC(sfWriteDataAddr, 0x50);	// write data
		XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01);	// trigger for write
		XU_SFWaitReady();
		XU_WriteToASIC(sfCSAddr, 0x1);
		//SF_Set_WRSR_Bit
		XU_WriteToASIC(sfCSAddr, 0x0);	// chip select
		XU_WriteToASIC(sfWriteDataAddr, 0x01);	// write data
		XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01);	// trigger for write
		XU_SFWaitReady();
		XU_WriteToASIC(sfWriteDataAddr, 0x00);
		XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01);	// trigger for write
		XU_SFWaitReady();
		XU_WriteToASIC(sfCSAddr, 0x1);
		//SF_Set_WEL_Bit
		XU_WriteToASIC(sfCSAddr, 0x0);	// chip select
		XU_WriteToASIC(sfWriteDataAddr, 0x06);	// write data
		XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01);	// trigger for write
		XU_SFWaitReady();
		XU_WriteToASIC(sfCSAddr, 0x1);
		//chip erase
		XU_WriteToASIC(sfCSAddr, 0x0);
		XU_WriteToASIC(sfWriteDataAddr, 0x60);
		XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01);
		XU_SFWaitReady();
		XU_WriteToASIC(sfCSAddr, 0x1);
		//SF_CMDread_Status
		XU_SFCMDreadStatus();
		ret = XU_WriteToASIC(sfModeAddr, 0x0);
		break;
	case SF_UNKNOW:
	case SF_MXIC:
	case SF_ATMEL_AT25FS:
	case SF_MXIC_LIKE:
	case SF_FENTECH:
	default:
		XU_WriteToASIC(sfModeAddr, 0x1);
		XU_WriteToASIC(sfCSAddr, 0x0);//CS#
		//SF_Set_WEL_Bit
		XU_WriteToASIC(sfWriteDataAddr, 0x06);//write enable cmd
		XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01);
		XU_SFWaitReady();
		//disable BP0 BP1
		XU_WriteToASIC(sfCSAddr, 0x1);
		//chip erase
		XU_WriteToASIC(sfCSAddr, 0x0);
		XU_WriteToASIC(sfWriteDataAddr, 0x60);//chip erase cmd
		//SetRegData(0x1082,0xc7);
		XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01);
		XU_SFWaitReady();
		XU_WriteToASIC(sfCSAddr, 0x1);
		//SF_CMDread_Status
		XU_SFCMDreadStatus();
		ret = XU_WriteToASIC(sfModeAddr, 0x0);
		break;
	}
    return TRUE;
}

BOOL XU_GetMemType(BYTE *pMemType)
{
	int ret = 0;
	__u8 ctrldata[11] = {0};

	//uvc_xu_control parmeters
	__u8 xu_unit= g_curExtendUnitID; 
	__u8 xu_selector= 5;
	__u16 xu_size= 11;
	__u8 *xu_data= ctrldata;

	if ((ret=XU_Get_Cur(xu_unit, xu_selector, xu_size, xu_data)) < 0) 
	{
		printf("ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n",ret);
		//if(ret==EINVAL)			printf("Invalid arguments\n");
		return FALSE;
	}

	*pMemType = xu_data[2];
    return TRUE;
}


BOOL XU_ReadBitFormAsic(LONG addr, BYTE bit)
{
	BYTE bufs;
	BYTE data;
	XU_ReadFromASIC(addr, &bufs);

	switch (bit)
	{
	case 0:
		data = bufs & 0x01;
		break;
	case 1:
		data = bufs & 0x02;
		break;
	case 2:
		data = bufs & 0x04;
		break;
	case 3:
		data = bufs & 0x08;
		break;
	case 4:
		data = bufs & 0x10;
		break;
	case 5:
		data = bufs & 0x20;
		break;
	case 6:
		data = bufs & 0x40;
		break;
	case 7:
		data = bufs & 0x80;
		break;
	default:
		break;
	}
	return data;
}

BOOL XU_SFWaitReady()
{
	BYTE i;
	LONG data = 0;
	for (i = 0; i < 50; i++)										//pooling ready; 500us timeout
	{
		if (XU_ReadBitFormAsic(sfRdyAddr, 0))                      //aISP_RDY
		{
			return TRUE;
		}
		usleep(1000);
	}
	return FALSE;
}

BOOL XU_SFCMDreadStatus()
{
	int i;
	unsigned char ucData;
	for (i = 0; i < 10000; ++i){
		// chip select to low
		if (TRUE != XU_WriteToASIC(sfCSAddr, 0x0)){
			return FALSE;
		}
		// Read status cmd
		if (TRUE != XU_WriteToASIC(sfWriteDataAddr, 0x05)){
			return FALSE;
		}
		// Write trig
		if (TRUE != XU_WriteToASIC(sfReadWriteTriggerAddr, 0x01)){
			return FALSE;
		}
		if (TRUE != XU_SFWaitReady()){
			return FALSE;
		}
		// Read reg cmd 
		if (TRUE != XU_WriteToASIC(sfReadDataAddr, 0x0)){
			return FALSE;;
		}
		// 2: Trigger for read data to Serial Flash in SF_MODE
		if (TRUE != XU_WriteToASIC(sfReadWriteTriggerAddr, 0x02)){
			return FALSE;
		}
		if (TRUE != XU_SFWaitReady()){
			return FALSE;
		}
		// Data read from Serial Flash
		if (TRUE != XU_ReadFromASIC(sfReadDataAddr, &ucData)){
			return FALSE;
		}
		// chip select to high
		if ((ucData & 0x01) != 0x01){
			if (TRUE != XU_WriteToASIC(sfCSAddr, 0x1)){
				return FALSE;
			}
			return TRUE;
		}
		usleep(1000);
	}
	return FALSE;
}

BOOL XU_GetParaTableAndCRCAddrFormFW(BYTE *pFW, ULONG* paraTableStartAddr, ULONG* paraTableEndAddr, ULONG* crcAddr)
{
	BYTE romVersion[8] = { 0 };
	if (!XU_GetAsicRomVersion(romVersion))
		return FALSE;

	if (memcmp(romVersion, "231R0", 4) == 0 && romVersion[5] >= 2) // 231R0_V2
	{
		return FALSE;
	}
	else if (((memcmp(romVersion, "232R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "275R0", 4) == 0 && romVersion[5] == 1 && romVersion[6] == 0x30)) ||
		((memcmp(romVersion, "276R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "290R0", 4) == 0 && romVersion[5] == 1)))
	{
		*paraTableStartAddr = 0xC000;
		*paraTableEndAddr = *paraTableStartAddr + 0xF00;
		*crcAddr = *paraTableStartAddr + 0xF00;
	}
	else if (((memcmp(romVersion, "232R0", 4) == 0 && romVersion[5] == 2)) ||
		((memcmp(romVersion, "290R0", 4) == 0 && romVersion[5] == 2)) ||
		((memcmp(romVersion, "288R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "289R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "270R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "271R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "280R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "281R0", 4) == 0 && romVersion[5] == 1)))
	{
		BYTE sectorTable[32];
		memcpy(sectorTable, pFW + 0x160, sizeof(sectorTable));

		// pbySectorTable[8] is parameter start address for deivce
		// pbySectorTable[9] is parameter size for deivce
		*paraTableStartAddr = (ULONG)sectorTable[0x08] << 8;
		*paraTableEndAddr = *paraTableStartAddr + ((ULONG)sectorTable[0x09] << 8);
		*crcAddr = (ULONG)sectorTable[0xE] << 8;
	}
	else if (((memcmp(romVersion, "272R0", 4) == 0) && (romVersion[5] == 1)) ||
		((memcmp(romVersion, "273R0", 4) == 0) && (romVersion[5] == 1)) ||
		((memcmp(romVersion, "275R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "283R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "285R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "286R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "287R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "267R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "292R0", 4) == 0) && (romVersion[5] == 1)))//wei add 292
	{
		// Get Parameter table start address
		// Parameter table start address is stored at 0x16F 
		BYTE sectorTable[0x2B];
		memcpy(sectorTable, pFW + 0x160, sizeof(sectorTable));

		*paraTableStartAddr = ((ULONG)sectorTable[0x0F] << 24) + ((ULONG)sectorTable[0x10] << 16) + ((ULONG)sectorTable[0x11] << 8) + sectorTable[0x12];
		ULONG dwParaTableSize = ((ULONG)sectorTable[0x13] << 24) + ((ULONG)sectorTable[0x14] << 16) + ((ULONG)sectorTable[0x15] << 8) + sectorTable[0x16];
		*paraTableEndAddr = *paraTableStartAddr + dwParaTableSize;
		*crcAddr = ((ULONG)sectorTable[0x27] << 24) + ((ULONG)sectorTable[0x28] << 16) + ((ULONG)sectorTable[0x29] << 8) + sectorTable[0x2a];
	}
	else if ((memcmp(romVersion, "216R0", 4) == 0))
	{
		*paraTableStartAddr = 0x5800;
		*paraTableEndAddr = *paraTableStartAddr + 0xF00;
		*crcAddr = *paraTableStartAddr + 0xF00;
	}
	else
	{
		*paraTableStartAddr = 0x8000;
		*paraTableEndAddr = *paraTableStartAddr + 0x800;
		*crcAddr = 0;
	}
	return TRUE;
}

BOOL XU_GetParaTableAndCRCAddrFormSF(ULONG *paraTableStartAddr, ULONG *paraTableEndAddr, ULONG *crcAddr)
{
	BYTE romVersion[8] = { 0 };
	if (TRUE != XU_GetAsicRomVersion(romVersion))
		return FALSE;

	if (memcmp(romVersion, "231R0", 4) == 0 && romVersion[5] >= 2) // 231R0_V2
	{
		return FALSE;
	}
	else if (((memcmp(romVersion, "232R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "275R0", 4) == 0 && romVersion[5] == 1 && romVersion[6] == 0x30)) ||
		((memcmp(romVersion, "276R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "290R0", 4) == 0 && romVersion[5] == 1)))
	{
		*paraTableStartAddr = 0xC000;
		*paraTableEndAddr = *paraTableStartAddr + 0xF00;
		*crcAddr = *paraTableStartAddr + 0xF00;
	}
	else if (((memcmp(romVersion, "232R0", 4) == 0 && romVersion[5] == 2)) ||
		((memcmp(romVersion, "290R0", 4) == 0 && romVersion[5] == 2)) ||
		((memcmp(romVersion, "288R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "289R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "270R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "271R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "280R0", 4) == 0 && romVersion[5] == 1)) ||
		((memcmp(romVersion, "281R0", 4) == 0 && romVersion[5] == 1)))
	{
		BYTE sectorTable[32];
		if (TRUE != XU_ReadFormSF(0x160, sectorTable, sizeof(sectorTable)))
			return FALSE;

		// pbySectorTable[8] is parameter start address for deivce
		// pbySectorTable[9] is parameter size for deivce
		*paraTableStartAddr = (ULONG)sectorTable[0x08] << 8;
		*paraTableEndAddr = *paraTableStartAddr + ((ULONG)sectorTable[0x09] << 8);
		*crcAddr = (ULONG)sectorTable[0xE] << 8;
	}
	else if (((memcmp(romVersion, "272R0", 4) == 0) && (romVersion[5] == 1)) ||
		((memcmp(romVersion, "273R0", 4) == 0) && (romVersion[5] == 1)) ||
		((memcmp(romVersion, "275R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "283R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "285R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "286R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "287R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "267R0", 4) == 0) && (romVersion[5] == 1) && (romVersion[6] == 0x46)) ||
		((memcmp(romVersion, "292R0", 4) == 0) && (romVersion[5] == 1)))//wei add 292
	{
		// Get Parameter table start address
		// Parameter table start address is stored at 0x16F
		BYTE sectorTable[0x2B];
		if (TRUE != XU_ReadFormSF(0x160, sectorTable, sizeof(sectorTable)))
			return FALSE;

		*paraTableStartAddr = ((ULONG)sectorTable[0x0F] << 24) + ((ULONG)sectorTable[0x10] << 16) + ((ULONG)sectorTable[0x11] << 8) + sectorTable[0x12];
		ULONG dwParaTableSize = ((ULONG)sectorTable[0x13] << 24) + ((ULONG)sectorTable[0x14] << 16) + ((ULONG)sectorTable[0x15] << 8) + sectorTable[0x16];
		*paraTableEndAddr = *paraTableStartAddr + dwParaTableSize;
		*crcAddr = ((ULONG)sectorTable[0x27] << 24) + ((ULONG)sectorTable[0x28] << 16) + ((ULONG)sectorTable[0x29] << 8) + sectorTable[0x2a];
	}
	else if ((memcmp(romVersion, "216R0", 4) == 0))
	{
		*paraTableStartAddr = 0x5800;
		*paraTableEndAddr = *paraTableStartAddr + 0xF00;
		*crcAddr = *paraTableStartAddr + 0xF00;
	}
	else
	{
		*paraTableStartAddr = 0x8000;
		*paraTableEndAddr = *paraTableStartAddr + 0x800;
		*crcAddr = 0;
	}
    return TRUE;
}

BOOL XU_GetStringSettingFormSF(BYTE* pbyString, DWORD stringSize, DWORD StringOffset, BOOL bIsCRCProtect)
{
	DWORD dwStringAddr = 0;
	ULONG dwParaTableStartAddr = 0;
	ULONG dwParaTableEndAddr = 0;
	ULONG dwCRCStartAddr = 0;

	if (TRUE != XU_GetParaTableAndCRCAddrFormSF(&dwParaTableStartAddr, &dwParaTableEndAddr, &dwCRCStartAddr))
		return FALSE;

	dwStringAddr = StringOffset;
	if (bIsCRCProtect)
		dwStringAddr += dwParaTableStartAddr;
	else
		dwStringAddr += dwCRCStartAddr;

	BYTE pbyStringBuf[0x40] = { 0 };
	if (TRUE != XU_ReadFormSF(dwStringAddr, pbyStringBuf, sizeof(pbyStringBuf)))
		return FALSE;

	// Calculate string length
	DWORD dwStringLength = 0;
	if (bIsCRCProtect)
		dwStringLength = (pbyStringBuf[0] - 2) / 2;
	else
	for (; (dwStringLength < 0x40 / 2) && (pbyStringBuf[dwStringLength] != 0xFF); ++dwStringLength);

	if (stringSize < dwStringLength)
		return FALSE;

	// Copy string to output buffer
	DWORD i;
	if (bIsCRCProtect)
	{
		for (i = 0; i<dwStringLength; ++i)
			pbyString[i] = pbyStringBuf[2 + i * 2];
	}
	else
	{
		memcpy(pbyString, pbyStringBuf, dwStringLength);
	}

    return TRUE;
}
