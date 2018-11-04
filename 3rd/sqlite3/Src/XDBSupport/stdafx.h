// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

#include "tchar.h"
#include "assert.h"
#include "objidl.h"
#include "string"
using namespace std;

#if defined(_DEBUG)
	#define ASSERT(x) assert(x)
#else
	#define ASSERT(x)
#endif

#ifdef _UNICODE
typedef std::wstring	stdstring;
#else
typedef std::string		stdstring;
#endif 

#include <list>

#include "..\..\Include\TArray.h"
#include "..\..\Include\ASComImplHelp.h"
#include "..\..\Include\sqlite3.h"
