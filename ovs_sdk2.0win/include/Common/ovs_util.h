/**
*  Utility Functions Definition
*
*  @author		JingHao Jin, XiaNi Jin
*  @company		Qingdao NovelBeam Technology Co., Ltd.
*  @see			Internal Utility Definition
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

#ifndef __OVS_UTIL_H__
#define __OVS_UTIL_H__

#ifdef WIN32
#include <windows.h>
#endif //WIN32

#undef max
#include <vector>

#ifdef WIN32
struct OVSStopWatch {
	LARGE_INTEGER start;
	LARGE_INTEGER stop;
};
#endif //WIN32

#include <stdarg.h>

#ifdef WIN32
#include <xmmintrin.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

__forceinline __m128 operator+(__m128 l, __m128 r) { return _mm_add_ps(l, r); }
__forceinline __m128 operator-(__m128 l, __m128 r) { return _mm_sub_ps(l, r); }
__forceinline __m128 operator*(__m128 l, __m128 r) { return _mm_mul_ps(l, r); }
__forceinline __m128 operator/(__m128 l, __m128 r) { return _mm_div_ps(l, r); }

__forceinline __m128 operator+(__m128 l, float r) { return _mm_add_ps(l, _mm_set1_ps(r)); }
__forceinline __m128 operator-(__m128 l, float r) { return _mm_sub_ps(l, _mm_set1_ps(r)); }
__forceinline __m128 operator*(__m128 l, float r) { return _mm_mul_ps(l, _mm_set1_ps(r)); }
__forceinline __m128 operator/(__m128 l, float r) { return _mm_div_ps(l, _mm_set1_ps(r)); }

__forceinline __m128 operator&(__m128 l, __m128 r) { return _mm_and_ps(l, r); }
__forceinline __m128 operator|(__m128 l, __m128 r) { return _mm_or_ps(l, r); }

__forceinline __m128 operator<(__m128 l, __m128 r) { return _mm_cmplt_ps(l, r); }
__forceinline __m128 operator>(__m128 l, __m128 r) { return _mm_cmpgt_ps(l, r); }
__forceinline __m128 operator<=(__m128 l, __m128 r) { return _mm_cmple_ps(l, r); }
__forceinline __m128 operator>=(__m128 l, __m128 r) { return _mm_cmpge_ps(l, r); }
__forceinline __m128 operator!=(__m128 l, __m128 r) { return _mm_cmpneq_ps(l, r); }
__forceinline __m128 operator==(__m128 l, __m128 r) { return _mm_cmpeq_ps(l, r); }
#endif //WIN32


#ifdef WIN32
class COvsTimer
{
public:
	COvsTimer(bool bStart = true) { Init(); if (bStart) startTimer(); }
	~COvsTimer() {}

	void startTimer() {
		_cnt++;
		_not_in_cnt = false;
		QueryPerformanceCounter(&_start);
	}
	double getElapsedTime() {
		StopTimer();
		double rv = ConvertLI2msec(_stop.QuadPart - _start.QuadPart);
		if (_not_in_cnt) {
			_average = (_average * _cnt + rv) / (_cnt + 1);
			_not_in_cnt = false;
		}
		return rv;
	}
	double getAverage() {
		return _average;
	}
private:
	void Init() {
		_cnt = 0;
		_average = 0.;
		_not_in_cnt = false;
		QueryPerformanceFrequency(&_frequency);
	}
	void StopTimer() {
		QueryPerformanceCounter(&_stop);
		_cnt = 0;
		_average = 0;
	}
	double ConvertLI2msec(const LONGLONG L) {
		return ((double)(L * 1000) / (double)_frequency.QuadPart);
	}
	LARGE_INTEGER _start;
	LARGE_INTEGER _stop;
	LARGE_INTEGER _frequency;
	double _average;
	unsigned int _cnt;
	bool _not_in_cnt;
};
#endif //WIN32

class COvsMutex
{
#define OVSCreateMutex(handle) handle = CreateMutex( NULL, FALSE, NULL )
#define OVSLockMutex(handle) if (handle != INVALID_HANDLE_VALUE)	{	WaitForSingleObject( handle, INFINITE );	}
#define OVSUnlockMutex(handle) if (handle != INVALID_HANDLE_VALUE)	{	ReleaseMutex( handle );	}
#define OVSDestroyMutex(handle) if (handle != INVALID_HANDLE_VALUE) {	CloseHandle( handle );	handle = INVALID_HANDLE_VALUE;	}
	typedef HANDLE OVS_MUTEX;

public:
	COvsMutex() {
		OVSCreateMutex(m_Lock);
	};
	~COvsMutex() {
		OVSDestroyMutex(m_Lock);
	};
	void lock() {
		OVSLockMutex(m_Lock);
	};
	void unlock() {
		OVSUnlockMutex(m_Lock);
	};

protected:
	OVS_MUTEX m_Lock;
};

namespace ovs_debug
{
#define _OVS_DEBUG_NONE						0x00000000
#define _OVS_DEBUG_OUT_ALWAYS				0xffffffff

#define OVS_DEBUG_LOG_ALL					0xffff0000
#define OVS_DEBUG_LOG_PROCESS_API			0x00010000

#ifdef _DEBUG
#define _OVS_DEBUG_TRACE_ENABLE				(_OVS_DEBUG_NONE)
#define _OVS_DEBUG_TIMER_ENABLE				(_OVS_DEBUG_NONE)
#define _OVS_DEBUG_SAVE_ENABLE				(_OVS_DEBUG_NONE)
#define _OVS_DEBUG_LOG_ENABLE				(_OVS_DEBUG_NONE | OVS_DEBUG_LOG_PROCESS_API)
#else //_DEBUG
#define _OVS_DEBUG_TRACE_ENABLE				(_OVS_DEBUG_NONE)
#define _OVS_DEBUG_TIMER_ENABLE				(_OVS_DEBUG_NONE)
#define _OVS_DEBUG_SAVE_ENABLE				(_OVS_DEBUG_NONE)
#define _OVS_DEBUG_LOG_ENABLE				(_OVS_DEBUG_NONE)
#endif //_DEBUG

#ifdef WIN32
#define _OVS_DEBUG_DIR						"./tmp/"
#endif //WIN32

	int saveBitmap(unsigned char* pixelData, int imageWidth, int imageHeight, const char* filename, unsigned int flag = 0, int index = -1);
	int saveBitmap(unsigned char** pixelData, int imageWidth, int imageHeight, const char* filename, unsigned int flag = 0, int index = -1);
	int saveBitmap(float* pixelData, float pixelMax, int imageWidth, int imageHeight, const char* filename, unsigned int flag = 0, int index = -1);
	int saveBitmap(int* pixelData, int pixelMax, int imageWidth, int imageHeight, const char* filename, unsigned int flag = 0, int index = -1);
	int loadBitmap(unsigned char** pixelData, int* imageWidth, int* imageHeight, const char* filename, unsigned int flag = 0, int index = -1);
	int loadBitmap(unsigned char* pixelData, int imageWidth, int imageHeight, const char* filename, unsigned int flag = 0, int index = -1);

	void setLogLevel(int nLogLevel);
	int logLevel();
	void logDebug(unsigned int seclevel, unsigned int flag, const char* szFormat, ...);
}

namespace ovs_util
{
	const char* getCurrentDateTime(void);
	const char* getCurrentDate(void);
	const char* getCurrentTime(void);

	const char* generateAuthenticationCode(const char* expiredate, const char* key);
	bool isValidDate(const char* validthru);
	const char* getDateString(unsigned int year, unsigned int month, unsigned int day, unsigned int week);

	bool isFile(const char* filename);
	bool isDir(const char* dirname);
	const char* getCurrentDir();

	char *encodeBase64(const unsigned char *data, size_t input_length, size_t *output_length);
	unsigned char *decodeBase64(const char *data, size_t input_length, size_t *output_length);
}

namespace ovs_image
{
	bool flipV(unsigned char *pData, int width, int height);
	bool flipH(unsigned char *pData, int width, int height);
}

namespace ovs_ini
{
	typedef struct
	{
		int matchingLevel;
		int enrollType;
		
	} OVS_CFG_t;

	bool loadIniFile();

	OVS_CFG_t getConfig();
}

namespace ovs_license
{
	bool isValid();
}

namespace ovs_extlib
{
	bool isExist();
}

#endif //__OVS_UTIL_H__