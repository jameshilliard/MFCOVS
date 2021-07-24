/**
*  Utility Functions Declaration
*
*  @author		JingHao Jin, XiaNi Jin
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			Internal Utility Declaration
*
**/

/**
*
*  Copyright (c) 2018 Qingdao NovelBeam Technology Co., Ltd. All Rights Reserved.
*
*  This software is the confidential and proprietary information of Qingdao NovelBeam Technology Co., Ltd. ("Confidential Information").
*  You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement
*  you entered into with Qingdao NovelBeam Technology Co., Ltd.
*
**/

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <string>
#include <stdlib.h>

#ifdef WIN32
#include <Windows.h>
#include <atltime.h>
#include <direct.h>
#include <DbgHelp.h>
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "DbgHelp.lib")
#define GetCurrentDir _getcwd
#endif //WIN32

#include "ovs_util.h"

char cCurrentPath[FILENAME_MAX];

#define _OVS_UTIL_MAX_PATH 2048
#define _OVS_UTIL_MAX_LOG 2048

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif //min
#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif //max
#ifndef WIN32
#define _mkdir(name) mkdir(name, 0777)
#endif //WIN32

#ifndef __min
#define __min(x,y) ((x)<(y)?(x):(y))
#endif //min
#ifndef __max
#define __max(x,y) ((x)>(y)?(x):(y))
#endif //max

#ifndef GetRValue
#define GetRValue( rgb )    ( ( BYTE )( rgb ) )
#define GetGValue( rgb )    ( ( BYTE )( ( ( WORD )( rgb ) ) >> 8 ) )
#define GetBValue( rgb )    ( ( BYTE )( ( rgb ) >> 16 ) )
#endif //GetRValue

namespace ovs_debug
{
	COvsTimer g_timer_stamp;
	COvsMutex g_mutex;
	std::string g_tmpdir = _OVS_DEBUG_DIR;
	unsigned int g_logLevel = 0;
	std::string g_outstr;

	const char *getDebugDir(const char *sub = NULL)
	{
		if (!ovs_util::isDir(g_tmpdir.c_str())) {
			_mkdir(g_tmpdir.c_str());
		}
		if (sub != NULL && ovs_util::isDir(g_tmpdir.c_str())) {
			g_outstr = g_tmpdir + "\\" + sub;
			if (!ovs_util::isDir(g_outstr.c_str())) {
				_mkdir(g_outstr.c_str());
			}
			return g_outstr.c_str();
		}
		return g_tmpdir.c_str();
	}
	bool setDebugDir(const char *dir)
	{
		if (dir == NULL) {
			g_tmpdir = _OVS_DEBUG_DIR;
		}
		else {
			g_tmpdir = dir;
		}
		if (!ovs_util::isDir(g_tmpdir.c_str())) {
			_mkdir(g_tmpdir.c_str());
		}

		if (!ovs_util::isDir(g_tmpdir.c_str())) {
			// directory not found - set as a default value
			g_tmpdir = _OVS_DEBUG_DIR;
			return false;
		}
		else if (g_tmpdir.rfind("\\") != g_tmpdir.length() - 1 && g_tmpdir.rfind("/") != g_tmpdir.length() - 1) {
			g_tmpdir += "/";
		}

		return true;
	}
	
	int saveBitmap(unsigned char* pixelData, int imageWidth, int imageHeight, const char* filename, unsigned int flag, int index)
	{
		if (imageWidth <= 0 || imageHeight <= 0) return -1;

		if (!pixelData) return -1;

		if ((flag & _OVS_DEBUG_SAVE_ENABLE) == 0 && flag != _OVS_DEBUG_OUT_ALWAYS) return 0;
		char _filename[_OVS_UTIL_MAX_PATH];
		if (index == -1) {
			sprintf_s(_filename, "%s%s", getDebugDir(), filename);
		}
		else {
			sprintf_s(_filename, "%s%s_%05d.bmp", getDebugDir(), filename, index);
		}
		_filename[_OVS_UTIL_MAX_PATH - 1] = 0;

		int targetWidth = (imageWidth % 4 == 0) ? imageWidth : imageWidth + 4 - imageWidth % 4;

		BITMAPFILEHEADER fileHeader;

		BITMAPINFO* bitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
		if (!bitmapInfo)
		{
			return -1;
		}

		bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo->bmiHeader.biWidth = imageWidth;
		bitmapInfo->bmiHeader.biHeight = imageHeight;
		bitmapInfo->bmiHeader.biPlanes = 1;
		bitmapInfo->bmiHeader.biBitCount = 8;
		bitmapInfo->bmiHeader.biCompression = BI_RGB;
		bitmapInfo->bmiHeader.biSizeImage = 0;
		bitmapInfo->bmiHeader.biXPelsPerMeter = 19686; // 500 * 39.37
		bitmapInfo->bmiHeader.biYPelsPerMeter = 19686;
		bitmapInfo->bmiHeader.biClrUsed = 0;
		bitmapInfo->bmiHeader.biClrImportant = 0;

		for (int i = 0; i < 256; i++)
		{
			bitmapInfo->bmiColors[i].rgbRed = i;
			bitmapInfo->bmiColors[i].rgbGreen = i;
			bitmapInfo->bmiColors[i].rgbBlue = i;
		}

		FILE* fp;
		fopen_s(&fp, _filename, "wb");

		if (!fp)
		{
			free(bitmapInfo);
			return -2;
		}

		// Fill in the fields of the file header 
		fileHeader.bfType = ((WORD)('M' << 8) | 'B');	// is always "BM"
		fileHeader.bfSize = sizeof(fileHeader) + sizeof(bitmapInfo) + targetWidth * imageHeight;
		fileHeader.bfReserved1 = 0;
		fileHeader.bfReserved2 = 0;
		fileHeader.bfOffBits = (DWORD)(sizeof(fileHeader) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

		int size = sizeof(fileHeader);

		if (fwrite(&fileHeader, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);

			return -3;
		}

		size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

		if (fwrite(bitmapInfo, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);

			return -4;
		}

		size = targetWidth * imageHeight;

		unsigned char* targetBuf = (unsigned char*)malloc(size);

		if (!targetBuf)
		{
			fclose(fp);
			free(bitmapInfo);

			return -5;
		}

		memset(targetBuf, 0, size);

		for (int i = 0; i < imageHeight; i++)
		{
			memcpy(targetBuf + (imageHeight - 1 - i) * targetWidth,
				pixelData + i * imageWidth, imageWidth);
		}

		if (fwrite(targetBuf, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);
			free(targetBuf);

			return -6;
		}

		fclose(fp);
		free(bitmapInfo);
		free(targetBuf);

		return 0;
	}
	int saveBitmap(unsigned char** pixelData, int imageWidth, int imageHeight, const char* filename, unsigned int flag, int index)
	{
		if ((flag & _OVS_DEBUG_SAVE_ENABLE) == 0 && flag != _OVS_DEBUG_OUT_ALWAYS) return 0;
		char _filename[_OVS_UTIL_MAX_PATH];
		if (index == -1) {
			sprintf_s(_filename, "%s%s", getDebugDir(), filename);
		}
		else {
			sprintf_s(_filename, "%s%s_%05d.bmp", getDebugDir(), filename, index);
		}
		_filename[_OVS_UTIL_MAX_PATH - 1] = 0;

		int targetWidth = (imageWidth % 4 == 0) ? imageWidth : imageWidth + 4 - imageWidth % 4;

		BITMAPFILEHEADER fileHeader;

		BITMAPINFO* bitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
		if (!bitmapInfo)
		{
			return -1;
		}

		bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo->bmiHeader.biWidth = imageWidth;
		bitmapInfo->bmiHeader.biHeight = imageHeight;
		bitmapInfo->bmiHeader.biPlanes = 1;
		bitmapInfo->bmiHeader.biBitCount = 8;
		bitmapInfo->bmiHeader.biCompression = BI_RGB;
		bitmapInfo->bmiHeader.biSizeImage = 0;
		bitmapInfo->bmiHeader.biXPelsPerMeter = 19686; // 500 * 39.37
		bitmapInfo->bmiHeader.biYPelsPerMeter = 19686;
		bitmapInfo->bmiHeader.biClrUsed = 0;
		bitmapInfo->bmiHeader.biClrImportant = 0;

		for (int i = 0; i < 256; i++)
		{
			bitmapInfo->bmiColors[i].rgbRed = i;
			bitmapInfo->bmiColors[i].rgbGreen = i;
			bitmapInfo->bmiColors[i].rgbBlue = i;
		}

		FILE* fp;
		fopen_s(&fp, _filename, "wb");

		if (!fp)
		{
			free(bitmapInfo);
			return -2;
		}

		// Fill in the fields of the file header 
		fileHeader.bfType = ((WORD)('M' << 8) | 'B');	// is always "BM"
		fileHeader.bfSize = sizeof(fileHeader) + sizeof(bitmapInfo) + targetWidth * imageHeight;
		fileHeader.bfReserved1 = 0;
		fileHeader.bfReserved2 = 0;
		fileHeader.bfOffBits = (DWORD)(sizeof(fileHeader) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

		int size = sizeof(fileHeader);

		if (fwrite(&fileHeader, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);

			return -3;
		}

		size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

		if (fwrite(bitmapInfo, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);

			return -4;
		}

		size = targetWidth * imageHeight;

		unsigned char* targetBuf = (unsigned char*)malloc(size);

		if (!targetBuf)
		{
			fclose(fp);
			free(bitmapInfo);

			return -5;
		}

		memset(targetBuf, 0, size);

		for (int i = 0; i < imageHeight; i++)
		{
			memcpy(targetBuf + (imageHeight - 1 - i) * targetWidth,
				pixelData[i], imageWidth);
		}

		if (fwrite(targetBuf, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);
			free(targetBuf);

			return -6;
		}

		fclose(fp);
		free(bitmapInfo);
		free(targetBuf);

		return 0;
	}
	int saveBitmap(float* pixelData, float pixelMax, int imageWidth, int imageHeight, const char* filename, unsigned int flag, int index)
	{
		if ((flag & _OVS_DEBUG_SAVE_ENABLE) == 0 && flag != _OVS_DEBUG_OUT_ALWAYS) return 0;
		char _filename[_OVS_UTIL_MAX_PATH];
		if (index == -1) {
			sprintf_s(_filename, "%s%s", getDebugDir(), filename);
		}
		else {
			sprintf_s(_filename, "%s%s_%05d.bmp", getDebugDir(), filename, index);
		}
		_filename[_OVS_UTIL_MAX_PATH - 1] = 0;

		int targetWidth = (imageWidth % 4 == 0) ? imageWidth : imageWidth + 4 - imageWidth % 4;

		BITMAPFILEHEADER fileHeader;

		BITMAPINFO* bitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
		if (!bitmapInfo)
		{
			return -1;
		}

		bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo->bmiHeader.biWidth = imageWidth;
		bitmapInfo->bmiHeader.biHeight = imageHeight;
		bitmapInfo->bmiHeader.biPlanes = 1;
		bitmapInfo->bmiHeader.biBitCount = 8;
		bitmapInfo->bmiHeader.biCompression = BI_RGB;
		bitmapInfo->bmiHeader.biSizeImage = 0;
		bitmapInfo->bmiHeader.biXPelsPerMeter = 19686; // 500 * 39.37
		bitmapInfo->bmiHeader.biYPelsPerMeter = 19686;
		bitmapInfo->bmiHeader.biClrUsed = 0;
		bitmapInfo->bmiHeader.biClrImportant = 0;

		for (int i = 0; i < 256; i++)
		{
			bitmapInfo->bmiColors[i].rgbRed = i;
			bitmapInfo->bmiColors[i].rgbGreen = i;
			bitmapInfo->bmiColors[i].rgbBlue = i;
		}

		FILE* fp;
		fopen_s(&fp, _filename, "wb");

		if (!fp)
		{
			free(bitmapInfo);
			return -2;
		}

		// Fill in the fields of the file header 
		fileHeader.bfType = ((WORD)('M' << 8) | 'B');	// is always "BM"
		fileHeader.bfSize = sizeof(fileHeader) + sizeof(bitmapInfo) + targetWidth * imageHeight;
		fileHeader.bfReserved1 = 0;
		fileHeader.bfReserved2 = 0;
		fileHeader.bfOffBits = (DWORD)(sizeof(fileHeader) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

		int size = sizeof(fileHeader);

		if (fwrite(&fileHeader, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);

			return -3;
		}

		size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

		if (fwrite(bitmapInfo, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);

			return -4;
		}

		size = targetWidth * imageHeight;

		unsigned char* targetBuf = (unsigned char*)malloc(size);

		if (!targetBuf)
		{
			fclose(fp);
			free(bitmapInfo);

			return -5;
		}

		memset(targetBuf, 0, size);

		if (pixelMax == 0.f) pixelMax = 1.f;
		for (int i = 0; i < imageHeight; i++)
		{
			unsigned char *pCurrentLine = targetBuf + (imageHeight - 1 - i) * targetWidth;
			for (int x = 0; x < imageWidth; x++) {
				float pv = pixelData[i * imageWidth + x] / pixelMax;
				*(pCurrentLine + x) = (unsigned char)max(0, min(255, pv * 255));
			}
		}

		if (fwrite(targetBuf, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);
			free(targetBuf);

			return -6;
		}

		fclose(fp);
		free(bitmapInfo);
		free(targetBuf);

		return 0;
	}
	int saveBitmap(int* pixelData, int pixelMax, int imageWidth, int imageHeight, const char* filename, unsigned int flag, int index)
	{
		if ((flag & _OVS_DEBUG_SAVE_ENABLE) == 0 && flag != _OVS_DEBUG_OUT_ALWAYS) return 0;
		char _filename[_OVS_UTIL_MAX_PATH];
		if (index == -1) {
			sprintf_s(_filename, "%s%s", getDebugDir(), filename);
		}
		else {
			sprintf_s(_filename, "%s%s_%05d.bmp", getDebugDir(), filename, index);
		}
		_filename[_OVS_UTIL_MAX_PATH - 1] = 0;

		int targetWidth = (imageWidth % 4 == 0) ? imageWidth : imageWidth + 4 - imageWidth % 4;

		BITMAPFILEHEADER fileHeader;

		BITMAPINFO* bitmapInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
		if (!bitmapInfo)
		{
			return -1;
		}

		bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo->bmiHeader.biWidth = imageWidth;
		bitmapInfo->bmiHeader.biHeight = imageHeight;
		bitmapInfo->bmiHeader.biPlanes = 1;
		bitmapInfo->bmiHeader.biBitCount = 8;
		bitmapInfo->bmiHeader.biCompression = BI_RGB;
		bitmapInfo->bmiHeader.biSizeImage = 0;
		bitmapInfo->bmiHeader.biXPelsPerMeter = 19686; // 500 * 39.37
		bitmapInfo->bmiHeader.biYPelsPerMeter = 19686;
		bitmapInfo->bmiHeader.biClrUsed = 0;
		bitmapInfo->bmiHeader.biClrImportant = 0;

		for (int i = 0; i < 256; i++)
		{
			bitmapInfo->bmiColors[i].rgbRed = i;
			bitmapInfo->bmiColors[i].rgbGreen = i;
			bitmapInfo->bmiColors[i].rgbBlue = i;
		}

		FILE* fp;
		fopen_s(&fp, _filename, "wb");

		if (!fp)
		{
			free(bitmapInfo);
			return -2;
		}

		// Fill in the fields of the file header 
		fileHeader.bfType = ((WORD)('M' << 8) | 'B');	// is always "BM"
		fileHeader.bfSize = sizeof(fileHeader) + sizeof(bitmapInfo) + targetWidth * imageHeight;
		fileHeader.bfReserved1 = 0;
		fileHeader.bfReserved2 = 0;
		fileHeader.bfOffBits = (DWORD)(sizeof(fileHeader) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

		int size = sizeof(fileHeader);

		if (fwrite(&fileHeader, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);

			return -3;
		}

		size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

		if (fwrite(bitmapInfo, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);

			return -4;
		}

		size = targetWidth * imageHeight;

		unsigned char* targetBuf = (unsigned char*)malloc(size);

		if (!targetBuf)
		{
			fclose(fp);
			free(bitmapInfo);

			return -5;
		}

		memset(targetBuf, 0, size);

		if (pixelMax == 0) pixelMax = 1;
		for (int i = 0; i < imageHeight; i++)
		{
			unsigned char *pCurrentLine = targetBuf + (imageHeight - 1 - i) * targetWidth;
			for (int x = 0; x < imageWidth; x++) {
				float pv = pixelData[i * imageWidth + x] * 255.f / pixelMax;
				*(pCurrentLine + x) = (unsigned char)max(0, min(255, pv));
			}
		}

		if (fwrite(targetBuf, 1, size, fp) != size)
		{
			fclose(fp);
			free(bitmapInfo);
			free(targetBuf);

			return -6;
		}

		fclose(fp);
		free(bitmapInfo);
		free(targetBuf);

		return 0;
	}
	int loadBitmap(unsigned char* pixelData, int *imageWidth, int *imageHeight, const char* filename, unsigned int flag, int index)
	{
		if (flag != _OVS_DEBUG_OUT_ALWAYS) return 0;

		char _filename[_OVS_UTIL_MAX_PATH];
		sprintf_s(_filename, "%s%s", getDebugDir(), filename);
		_filename[_OVS_UTIL_MAX_PATH - 1] = 0;

		FILE *pFile;
		fopen_s(&pFile, _filename, "rb");

		BITMAPFILEHEADER FileHeader;
		fread(&FileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);

		BITMAPINFOHEADER InfoHeader;
		fread(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

		if (InfoHeader.biBitCount < 8) return -7;

		int sourceWidth = (InfoHeader.biWidth % 4 == 0) ? InfoHeader.biWidth : InfoHeader.biWidth + 4 - InfoHeader.biWidth % 4;

		RGBQUAD	*pColor;
		if (InfoHeader.biBitCount == 8)
		{
			pColor = new RGBQUAD[256];
			fread(pColor, sizeof(RGBQUAD), 256, pFile);
		}
		unsigned char *pData = new unsigned char[sourceWidth*InfoHeader.biHeight];
		fread(pData, sizeof(unsigned char), sourceWidth*InfoHeader.biHeight, pFile);
		fclose(pFile);

		pixelData = NULL;
		pixelData = (unsigned char*)malloc(sourceWidth*InfoHeader.biHeight);
		memset(pixelData, 0, sourceWidth*InfoHeader.biHeight);

		for (int i = 0; i < InfoHeader.biHeight; i++)
		{
			memcpy(pixelData + i * InfoHeader.biWidth,
				pData + (InfoHeader.biHeight - 1 - i) * sourceWidth, InfoHeader.biWidth);
		}

		*imageWidth = InfoHeader.biWidth;
		*imageHeight = InfoHeader.biHeight;

		delete[] pData;

		return 0;
	}
	int loadBitmap(unsigned char** pixelData, int* imageWidth, int* imageHeight, const char* filename, unsigned int flag, int index)
	{
		if (flag != _OVS_DEBUG_OUT_ALWAYS) return 0;

		char _filename[_OVS_UTIL_MAX_PATH];
		sprintf_s(_filename, "%s%s", getDebugDir(), filename);
		_filename[_OVS_UTIL_MAX_PATH - 1] = 0;

		FILE *pFile;
		fopen_s(&pFile, _filename, "rb");

		BITMAPFILEHEADER FileHeader;
		fread(&FileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);

		BITMAPINFOHEADER InfoHeader;
		fread(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

		if (InfoHeader.biBitCount < 8) return -7;

		int sourceWidth = (InfoHeader.biWidth % 4 == 0) ? InfoHeader.biWidth : InfoHeader.biWidth + 4 - InfoHeader.biWidth % 4;

		RGBQUAD	*pColor;
		if (InfoHeader.biBitCount == 8)
		{
			pColor = new RGBQUAD[256];
			fread(pColor, sizeof(RGBQUAD), 256, pFile);
		}
		unsigned char *pData = new unsigned char[sourceWidth*InfoHeader.biHeight];
		fread(pData, sizeof(unsigned char), sourceWidth*InfoHeader.biHeight, pFile);
		fclose(pFile);

		*pixelData = NULL;
		*pixelData = (unsigned char*)malloc(sourceWidth*InfoHeader.biHeight);
		memset(*pixelData, 0, sourceWidth*InfoHeader.biHeight);

		for (int i = 0; i < InfoHeader.biHeight; i++)
		{
			memcpy(*pixelData + i * InfoHeader.biWidth,
				pData + (InfoHeader.biHeight - 1 - i) * sourceWidth, InfoHeader.biWidth);
		}

		*imageWidth = InfoHeader.biWidth;
		*imageHeight = InfoHeader.biHeight;

		delete[] pData;

		return 0;
	}

	int logLevel()
	{
		return g_logLevel;
	}
	void setLogLevel(int nLogLevel)
	{
		g_logLevel = nLogLevel;
	}

	void backupOversize(char *filename, size_t sizeLimit)
	{
		FILE *fp;
		fopen_s(&fp, filename, "rb");
		if (fp) {
			fseek(fp, 0, SEEK_END);
			size_t pos = ftell(fp);
			fclose(fp);
			if (pos > sizeLimit) {
				char new_filename_head[MAX_PATH];
				char new_filename[MAX_PATH];
				strcpy_s(new_filename_head, filename);
				char *extPos = strstr(new_filename_head, ".log");
				extPos[0] = 0;
				int i = 0;
				if (extPos) {
					sprintf_s(new_filename, "%s_%03d.log", new_filename_head, i);
					while ((fp = fopen(new_filename, "rb")) != NULL) {
						fclose(fp);
						i++;
						sprintf_s(new_filename, "%s_%03d.log", new_filename_head, i);
					}
					rename(filename, new_filename);
				}
			}
		}
	}
	void logDebug(unsigned int seclevel, unsigned int flag, const char* szFormat, ...)
	{
		unsigned int level = logLevel();
		if (level < seclevel || level == 4) {	// skip log at level 4
			if ((flag & _OVS_DEBUG_LOG_ENABLE) == 0 && flag != _OVS_DEBUG_OUT_ALWAYS) return;	// if flag == 0xffffffff, log always
		}

		char szTempBuf[_OVS_UTIL_MAX_LOG];
		char szTempBuf_fmt[_OVS_UTIL_MAX_LOG + 64];
		va_list vlMarker;

		va_start(vlMarker, szFormat);
		vsprintf(szTempBuf, szFormat, vlMarker);
		va_end(vlMarker);

		{
			char _szDatePath[_OVS_UTIL_MAX_LOG];
#ifdef WIN32
			CTime tToday = CTime::GetCurrentTime();
			sprintf_s(_szDatePath, " %04d-%02d-%02d  %02d:%02d:%02d ",
				tToday.GetYear(), tToday.GetMonth(), tToday.GetDay(), tToday.GetHour(), tToday.GetMinute(), tToday.GetSecond());
#endif //WIN32

			static bool bLineFed = true;
			szTempBuf[_OVS_UTIL_MAX_LOG - 1] = 0;
			char _filename[_OVS_UTIL_MAX_PATH];
			sprintf_s(_filename, "%sovs_sdk.log", getDebugDir());
			_filename[_OVS_UTIL_MAX_LOG - 1] = 0;
			backupOversize(_filename, 100000000);

			g_mutex.lock();
			FILE *fp = fopen(_filename, "a+");
			if (fp) {
				bool bLineFeed = strstr(szTempBuf, "\n") ? true : false;
				if (bLineFed) {
					sprintf_s(szTempBuf_fmt, "[LOG @ %s @ %8.3f] %s", _szDatePath, g_timer_stamp.getElapsedTime(), szTempBuf);
				}
				else {
					if (bLineFeed) {
						sprintf_s(szTempBuf_fmt, "\n[LOG @ %s @ %8.3f] %s", _szDatePath, g_timer_stamp.getElapsedTime(), szTempBuf);
					}
					else {
						sprintf_s(szTempBuf_fmt, "%s", szTempBuf);
					}
				}
				bLineFed = bLineFeed;
				if (level > 5) {
					fprintf(fp, szTempBuf_fmt);
				}
				else {
					size_t sz;
					char *base64_coded = ovs_util::encodeBase64((const unsigned char*)szTempBuf_fmt, strlen(szTempBuf_fmt), &sz);
					fwrite(base64_coded, 1, sz, fp);
					fwrite("\n", 1, 1, fp);
					delete[] base64_coded;
				}
				fflush(fp);
				fclose(fp);
			}
			g_mutex.unlock();
		}
	}
} //ovs_debug

#include "sha256.h"
namespace ovs_util
{
	mbstate_t in_cvt_state;
	mbstate_t out_cvt_state;
	std::string gRetString;
	std::string gOutString;

	const char* getCurrentDateTime(void)
	{
		struct tm now;
		time_t timer = time(0);
		localtime_s(&now, &timer);
		char date[64];

		sprintf_s(date, "%04d-%02d-%02d, %02d:%02d:%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
		gRetString = date;
		return gRetString.c_str();
	}

	const char* getCurrentDate(void)
	{
		struct tm now;
		time_t timer = time(0);
		localtime_s(&now, &timer);
		char date[20];

		sprintf_s(date, "%04d-%02d-%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);
		gRetString = date;
		return gRetString.c_str();
	}

	const char* getCurrentTime(void)
	{
		struct tm now;
		time_t timer = time(0);
		localtime_s(&now, &timer);
		char date[64];

		sprintf_s(date, "%02d:%02d:%02d", now.tm_hour, now.tm_min, now.tm_sec);
		gRetString = date;
		return gRetString.c_str();
	}

	const char* g_Months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	const char* g_Week[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

	int Month2Number(const char *sMonth)
	{
		for (int i = 0; i < 12; i++) {
			if (strcmp(sMonth, g_Months[i]) == 0)
				return i + 1;
		}
		return 0; // unknown
	}
	const char* Number2Month(unsigned int month)
	{
		return (g_Months[(month - 1) % 12]);
	}

	const char* Number2Week(unsigned int week)
	{
		return (g_Week[(week - 1) % 7]);
	}

	bool isValidDate(const char* validthru)
	{
		if (strcmp(validthru, "Eternal") == 0) {
			return true;
		}
		else {
#ifdef DATE_OVER_INTERNET
			WSADATA wsaData;
			DWORD ret = WSAStartup(MAKEWORD(2, 0), &wsaData);

			char *host = "pool.ntp.org"; /* Don't distribute stuff pointing here, it's not polite. */
			//char *host = "time.nist.gov"; /* This one's probably ok, but can get grumpy about request rates during debugging. */

			NTPMessage msg;
			/* Important, if you don't set the version/mode, the server will ignore you. */
			msg.clear();
			msg.version = 3;
			msg.mode = 3 /* client */;

			NTPMessage response;
			response.clear();

			int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
			sockaddr_in srv_addr;
			memset(&srv_addr, 0, sizeof(srv_addr));
			int rv = dns_lookup(host, &srv_addr); /* Helper function defined below. */

			if (rv >= 0) rv = msg.sendto(sock, &srv_addr);
			if (rv >= 0) rv = response.recv(sock);

			if (rv < 0){
				printf("  >>> Error: Check your internet connection!\n");
				printf("       the ovs sdk needs internet connection to be activated.\n");
				delete[] pBuf;
				return -5;	// environment fail
			}
			time_t t = response.tx.to_time_t();

			WSACleanup();

			char sTime[256];
			ctime_s(sTime, 256, &t);

			char sWeek[16] = { 0, }, sMonth[16] = { 0, }, sTimeSplit[32] = { 0, };
			int nDay = 1, nYear = 2000, nMonth = 1;

			char sWeekValid[16] = { 0, }, sMonthValid[16] = { 0, }, sTimeSplitValid[32] = { 0, };
			int nDayValid = 1, nYearValid = 2000, nMonthValid = 1;

			sscanf(sTime, "%s %s %d %s %d", sWeek, sMonth, &nDay, sTimeSplit, &nYear);
			sscanf(DATE_VALID_THRU, "%s %s %d %s %d", sWeekValid, sMonthValid, &nDayValid, sTimeSplitValid, &nYearValid);

			nMonth = Month2Number(sMonth);
			nMonthValid = Month2Number(sMonthValid);
#else //DATE_OVER_INTERNET
			int nDay = 1, nYear = 2000, nMonth = 1;
			int nDayValid = 1, nYearValid = 2000, nMonthValid = 1;
			char sWeekValid[16] = { 0, }, sMonthValid[16] = { 0, }, sTimeSplitValid[32] = { 0, };

			sscanf(validthru, "%s %s %d %s %d", sWeekValid, sMonthValid, &nDayValid, sTimeSplitValid, &nYearValid);

			time_t now = time(NULL);
			struct tm *lt = localtime(&now);

			nMonthValid = Month2Number(sMonthValid);
			nYear = lt->tm_year + 1900;
			nMonth = lt->tm_mon + 1;
			nDay = lt->tm_mday;
#endif //DATE_OVER_INTERNET

			if (nYear > nYearValid) {
				return false;	// expired
			}
			else if (nYear == nYearValid && nMonth > nMonthValid) {
				return false;	// expired
			}
			else if (nYear == nYearValid && nMonthValid == nMonth && nDay > nDayValid) {
				return false;	// expired
			}
			return true;
		}
	}

	const char* getDateString(unsigned int year, unsigned int month, unsigned int day, unsigned int week)
	{
		gOutString.resize(100);
#ifdef WIN32
		sprintf_s((char *)gOutString.data(), 100, "%s %s %d 23:59:59 %d", Number2Week(week), Number2Month(month), day, year);
#else
		sprintf((char *)gOutString.data(), "%s %s %d 23:59:59 %d", Number2Week(week), Number2Month(month), day, year);
#endif
		return gOutString.c_str();
	}

	const char* generateAuthenticationCode(const char* expiredate, const char* key)
	{
		std::string strSha = expiredate;
		strSha.append(";");
		strSha.append(key);

		gOutString = sha256(strSha);
		return gOutString.c_str();
	}

	bool isDir(const char *szPath){
#ifdef WIN32
		WIN32_FILE_ATTRIBUTE_DATA info;
		if (GetFileAttributesEx(szPath, GetFileExInfoStandard, &info) != 0) {
			return (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		}
		return false;
#endif //WIN32
	}

	bool isFile(const char *szPath){
		struct stat st;
		int result = stat(szPath, &st);
		return result == 0;
	}

	const char* getCurrentDir() {
		if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
			cCurrentPath[0] = 0;
		}
		return cCurrentPath;
	}

	static const char encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static const char decoding_table[] = {
		/* ASCII table */
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
		64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
		64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
	};
	static int mod_table[] = { 0, 2, 1 };

	char *encodeBase64(const unsigned char *data, size_t input_length, size_t *output_length)
	{
		*output_length = 4 * ((input_length + 2) / 3);

		char *encoded_data = new char[*output_length];
		if (encoded_data == NULL) return NULL;

		for (size_t i = 0, j = 0; i < input_length;) {

			UINT32 octet_a = i < input_length ? (unsigned char)data[i++] : 0;
			UINT32 octet_b = i < input_length ? (unsigned char)data[i++] : 0;
			UINT32 octet_c = i < input_length ? (unsigned char)data[i++] : 0;

			UINT32 triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

			encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
			encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
			encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
			encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
		}

		for (int i = 0; i < mod_table[input_length % 3]; i++)
			encoded_data[*output_length - 1 - i] = '=';

		return encoded_data;
	}
	unsigned char *decodeBase64(const char *data, size_t input_length, size_t *output_length)
	{
		if (input_length % 4 != 0) return NULL;

		*output_length = input_length / 4 * 3;
		if (data[input_length - 1] == '=') (*output_length)--;
		if (data[input_length - 2] == '=') (*output_length)--;

		unsigned char *decoded_data = new unsigned char[*output_length];
		if (decoded_data == NULL) return NULL;

		for (size_t i = 0, j = 0; i < input_length;) {

			UINT32 sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
			UINT32 sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
			UINT32 sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
			UINT32 sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

			UINT32 triple = (sextet_a << 3 * 6)
				+ (sextet_b << 2 * 6)
				+ (sextet_c << 1 * 6)
				+ (sextet_d << 0 * 6);

			if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
			if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
			if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
		}

		return decoded_data;
	}

	unsigned char *encodeAES(const unsigned char *pData, const unsigned int size, const char *pwd)
	{
		return NULL;
	}
	unsigned char *decodeAES(unsigned char *pData, const char *pwd, unsigned int &size)
	{
		return NULL;
	}

} //ovs_util

namespace ovs_image
{
	bool flipV(unsigned char *pData, int width, int height)
	{
		int y = 0;
		int height_1 = height - 1;
		int height_half = height >> 1;
		unsigned char *pSwapLine = (unsigned char*)malloc(width*sizeof(unsigned char));
		if (pSwapLine == NULL) return FALSE;

		for (y = 0; y < height_half; y++) {
			memcpy(pSwapLine, pData + y * width, width);
			memcpy(pData + y * width, pData + (height_1 - y) * width, width);
			memcpy(pData + (height_1 - y) * width, pSwapLine, width);
		}

		free(pSwapLine);
		return TRUE;
	}

	bool flipH(unsigned char *pData, int width, int height)
	{
		int x = 0, y = 0;
		int width_1 = width - 1;
		int width_half = width >> 1;
		int idx = 0;
		unsigned char bSwapPix = 0;

		for (y = 0; y < height; y++) {
			idx = y * width;
			for (x = 0; x < width_half; x++) {
				bSwapPix = pData[idx + x];
				pData[idx + x] = pData[idx + (width_1 - x)];
				pData[idx + (width_1 - x)] = bSwapPix;
			}
		}
		return TRUE;
	}
} // ovs_image

#include <map>
#include "ini.h"
namespace ovs_ini
{
	std::map<std::string, std::string> g_ini_map;
	char g_ini_path[MAX_PATH] = "./ovs_sdk.ini";
	char g_ini_subpath[MAX_PATH] = "C:\\ovs_sdk.ini";
	char g_databuf[1024] = "";
	OVS_CFG_t g_cfg;

	int pg_inikey_handler(void* user, const char* section, const char* name, const char* value)
	{
		std::map<std::string, std::string>* params = (std::map<std::string, std::string>*)user;
		if (params) {
			std::string name_form = section;
			name_form.append("::");
			name_form.append(name);
			if (value[0] == '\"' && value[strlen(value) - 1] == '\"') {
				std::string value_txt = value;
				g_ini_map[name_form] = value_txt.substr(1, value_txt.length() - 2);
			}
			else {
				g_ini_map[name_form] = value;
			}
		}
		return 1;
	}

	int getProfileInt(const char* szApp, const char* szKey, const char* szDefault)
	{
		std::string name_form = szApp;
		name_form.append("::");
		name_form.append(szKey);
		if (g_ini_map.find(name_form) != g_ini_map.end()) {
			strcpy(g_databuf, g_ini_map[name_form].c_str());
		}
		else {
			strcpy(g_databuf, szDefault);
		}

		return atoi(g_databuf);
	}
	const char* getProfileString(const char* szApp, const char* szKey, const char* szDefault)
	{
		std::string name_form = szApp;
		name_form.append("::");
		name_form.append(szKey);
		if (g_ini_map.find(name_form) != g_ini_map.end()) {
			strcpy(g_databuf, g_ini_map[name_form].c_str());
		}
		else {
			strcpy(g_databuf, szDefault);
		}
		return g_databuf;
	}

	bool loadIniFile()
	{
		g_ini_map.clear();

		GetModuleFileName(NULL, g_ini_path, MAX_PATH);
		PathRemoveFileSpec(g_ini_path);
		CString szIniPath = g_ini_path + CString("\\ovs_sdk.ini");
		if (ini_parse((CStringA)szIniPath, pg_inikey_handler, &g_ini_map) == -1) {
			if (ini_parse((CStringA)g_ini_subpath, pg_inikey_handler, &g_ini_map) == 0) {
				szIniPath = g_ini_subpath;
			}
		}

		bool res = false;
		int iniLevel = 0;
		if (ovs_util::isFile((CStringA)szIniPath))
		{
			g_cfg.matchingLevel = getProfileInt("Options", "MatchingLevel", "50");
			g_cfg.enrollType = getProfileInt("Options", "EnrollType", "0");
			iniLevel = getProfileInt("Advanced", "LogLevel", "0");
			res = true;
		}
		else
		{
			g_cfg.matchingLevel = 50;
			g_cfg.enrollType = 0;
			iniLevel = 0;
			res = false;
		}

		ovs_debug::setLogLevel(iniLevel);
		return res;
	}

	OVS_CFG_t getConfig()
	{
		return g_cfg;
	}
} //ovs_ini

namespace ovs_license
{
	std::map<std::string, std::string> g_ini_map;
	std::string g_license;
	std::string g_licensedate;
	char g_license_path[MAX_PATH] = "./ovs_license";
	char g_databuf[1024] = "";

	int pg_inikey_handler(void* user, const char* section, const char* name, const char* value)
	{
		std::map<std::string, std::string>* params = (std::map<std::string, std::string>*)user;
		if (params) {
			std::string name_form = section;
			name_form.append("::");
			name_form.append(name);
			if (value[0] == '\"' && value[strlen(value) - 1] == '\"') {
				std::string value_txt = value;
				g_ini_map[name_form] = value_txt.substr(1, value_txt.length() - 2);
			}
			else {
				g_ini_map[name_form] = value;
			}
		}
		return 1;
	}

	const char* getProfileString(const char* szApp, const char* szKey, const char* szDefault)
	{
		std::string name_form = szApp;
		name_form.append("::");
		name_form.append(szKey);
		if (g_ini_map.find(name_form) != g_ini_map.end()) {
			strcpy(g_databuf, g_ini_map[name_form].c_str());
		}
		else {
			strcpy(g_databuf, szDefault);
		}
		return g_databuf;
	}

	bool loadLicense()
	{
		g_ini_map.clear();

		GetModuleFileName(NULL, g_license_path, MAX_PATH);
		PathRemoveFileSpec(g_license_path);
		CString szIniPath = g_license_path + CString("\\ovs_license");
		if (ini_parse((CStringA)szIniPath, pg_inikey_handler, &g_ini_map) == -1) {
			return false;
		}

		if (ovs_util::isFile((CStringA)szIniPath)) {
			g_license = getProfileString("", "SDKLicence", "");
			g_licensedate = getProfileString("", "SDKExpireDate", "");
			return ovs_util::isValidDate(g_licensedate.c_str());
		}
		else {
			return false;
		}
	}

	bool isValid()
	{
		if (loadLicense()) {
			char szCode[1024];
			sprintf_s(szCode, "Purpose=%s;ExpireDate=%s", _T("ONLY_USE_MODULE"), g_licensedate.c_str());
			if (strcmp(g_license.c_str(), ovs_util::generateAuthenticationCode(szCode, "Q!ngDa0 N@ve1&e*m T2chN01O9y c0. !tD. (2018)"))) {
				return false;
			}
			else {
				return true;
			}
		}
		else {
			return false;
		}
	}

} //ovs_license

namespace ovs_extlib
{
	bool isExist()
	{
		char curr_path[MAX_PATH] = "./";
		GetModuleFileName(NULL, curr_path, MAX_PATH);
		PathRemoveFileSpec(curr_path);
		CString szFeaturePath = curr_path + CString("\\whois_feature.dll");
		CString szMatchPath = curr_path + CString("\\whois_match.dll");
		if (!ovs_util::isFile((CStringA)szFeaturePath) || !ovs_util::isFile((CStringA)szMatchPath)) {
			return false;
		}
		return true;
	}
} //ovs_extlib