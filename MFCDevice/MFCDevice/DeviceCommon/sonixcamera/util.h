#ifndef __UTIL_H__
#define __UTIL_H__

#include <windows.h>
#include <string>
using std::wstring;
using std::string;

typedef struct
{
	wstring deviceName;
	wstring devicePath;
	LONG dspRomSize;
	BYTE chipID;
}CameraInfo;

#endif