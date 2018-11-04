// CefUIWrapper.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CefUIWrapper.h"

#include "CefHandlerBack.h"
#include "BrowserClient.h"
#include "UICefBrowser.h"


#ifdef _DEBUG
#     pragma comment(lib, __FILE__\\"..\\..\\lib\\CefUIWrapper_d.lib")
#else
#     pragma comment(lib, __FILE__\\"..\\..\\lib\\CefUIWrapper.lib")
#endif

