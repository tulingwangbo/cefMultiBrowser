#if !defined(_INCLUDE_SQLITE3EXLIB_H__)
#define _INCLUDE_SQLITE3EXLIB_H__

//////////////////////////////////////////////////////////////////////////
//	外部模块，使用Sqlite3Ex库时，仅需要包含该头文件即可
//	
//	CppSQLite3.h：网上别人封装的使用sqlite3的CPP类，导出为外部类，也可以直
//		接使用
//
//	Sqlite3ExDefine.h：封装的数据库连接类和数据表访问类，后者可以继承使用
//////////////////////////////////////////////////////////////////////////

#define SQLITE3EX_API		AFX_API_IMPORT
#define SQLITE3EX_CLASS		AFX_CLASS_IMPORT

#include "CppSQLite3.h"
#include "Sqlite3ExDefine.h"


#ifdef WIN64
	#if (defined _UNICODE) || (defined UNICODE)
		#ifdef _DEBUG
			#pragma message("******** Sqlite3Ex：当前编译环境 = Win64_UnicodeDebug ********")
			#pragma comment(lib, __FILE__"\\..\\..\\Lib\\Win64\\Sqlite3ExUD.lib")
		#else
			#pragma message("******** Sqlite3Ex：当前编译环境 = Win64_UnicodeRelease ********")
			#pragma comment(lib, __FILE__"\\..\\..\\Lib\\Win64\\Sqlite3ExU.lib")
		#endif	//_DEBUG
	#endif //_UNICODE
#elif defined(WIN32)
	#if (defined _UNICODE) || (defined UNICODE)
		#ifdef _DEBUG
			#pragma message("******** Sqlite3Ex：当前编译环境 = Win32_UnicodeDebug ********")
			#pragma comment(lib, __FILE__"\\..\\..\\Lib\\Win32\\Sqlite3ExUD.lib")
		#else
			#pragma message("******** Sqlite3Ex：当前编译环境 = Win32_UnicodeRelease ********")
			#pragma comment(lib, __FILE__"\\..\\..\\Lib\\Win32\\Sqlite3ExU.lib")
		#endif	//_DEBUG
	#endif //_UNICODE
#endif //WIN64

#endif // _INCLUDE_SQLITE3EXLIB_H__