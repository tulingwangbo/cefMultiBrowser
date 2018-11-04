#pragma once

#ifdef _DEBUG
	#pragma comment(lib, __FILE__"\\..\\..\\lib\\Win32\\XDBSupportD.lib")
	#pragma comment(lib, __FILE__"\\..\\..\\lib\\Win32\\sqlite3D.lib")
#else
	#pragma comment(lib, __FILE__"\\..\\..\\lib\\Win32\\XDBSupport.lib")
	#pragma comment(lib, __FILE__"\\..\\..\\lib\\Win32\\sqlite3.lib")
#endif


#include "..\src\XDBSupport\XDBSupportAPI.h"
#include "..\src\XDBSupport\CommonLibFunc.h"