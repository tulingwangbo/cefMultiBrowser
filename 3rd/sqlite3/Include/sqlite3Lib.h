#if !defined(_INCLUDE_SQLITE3LIB_H__)
#define _INCLUDE_SQLITE3LIB_H__

#define SQLITE_API	AFX_API_IMPORT

#include ".\sqlite3.h"

#ifdef WIN64
	#if (defined _UNICODE) || (defined UNICODE)
		#ifdef _DEBUG
			#pragma message("******** sqlite3：当前编译环境 = Win64_UnicodeDebug ********")
			#pragma comment(lib, __FILE__"\\..\\..\\Lib\\Win64\\sqlite3d.lib")
		#else
			#pragma message("******** sqlite3：当前编译环境 = Win64_UnicodeRelease ********")
			#pragma comment(lib, __FILE__"\\..\\..\\Lib\\Win64\\sqlite3.lib")
		#endif	//_DEBUG
	#endif //_UNICODE
#elif defined(WIN32)
	#if (defined _UNICODE) || (defined UNICODE)
		#ifdef _DEBUG
			#pragma message("******** sqlite3：当前编译环境 = Win32_UnicodeDebug ********")
			#pragma comment(lib, __FILE__"\\..\\..\\Lib\\Win32\\sqlite3d.lib")
		#else
			#pragma message("******** sqlite3：当前编译环境 = Win32_UnicodeRelease ********")
			#pragma comment(lib, __FILE__"\\..\\..\\Lib\\Win32\\sqlite3.lib")
		#endif	//_DEBUG
	#endif //_UNICODE
#endif //WIN64

#endif // _INCLUDE_SQLITE3_LIB_H__