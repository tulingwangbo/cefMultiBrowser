
//*********************************************************
// 文件名：DYEStringUtils.h
// 作  者：lvkun
// 日  期：
// 目  的：字符串实用类——头文件
// 备  注：hesl整理于2008-07-23！ 
//*********************************************************

#if !defined DYESTRINGUTILS_H
#define DYESTRINGUTILS_H

#if _MSC_VER > 1000 
#pragma once
#endif

#include <string>
#include <sstream>
using namespace std;

#pragma comment(lib, "Rpcrt4.lib") 

/// 为兼容独立文件发布与Dll发布两种方式而设计的宏：
/// 独立发布时为空，Dll发布时为DY_EXT_****
#if !defined DYECOMMON_EXT_CLASS
	#define DYECOMMON_EXT_CLASS
#endif

#if !defined DYECOMMON_EXT_API
	#define DYECOMMON_EXT_API
#endif

#if !defined DYECOMMON_EXT_DATA
	#define DYECOMMON_EXT_DATA
#endif


/// 字符串实用类
namespace ns_DYEStringUtils {

	/**
	*	\brief	将指定的UTF8字符串转换为Unicode后输出
	*	\param[in]	pszSource	源字符串(UTF8)
	*/
	DYECOMMON_EXT_API CString TransUTF8ToUnicode(const char* pszSource);

	/**
	*	\brief	将指定的Unicode字符串转换为UTF8 string后输出
	*	\param[in]	strSource	源字符串(Unicode)
	*	\return		UTF8 string
	*/
	DYECOMMON_EXT_API string  TransUnicodeToUTF8(CString strSource);

	///	将CString字符串转换为std::string字符串
	DYECOMMON_EXT_API string TranslateCStringTostring(CString &strSource);

#if (defined _UNICODE) || (defined UNICODE)
	///	将CString字符串转换为std::wstring字符串
	DYECOMMON_EXT_API wstring TranslateCStringTowstring(CString const &strSource);
#endif

	///	将std::string字符串转换为CString字符串
	DYECOMMON_EXT_API CString TranslatestringToCString(string const &strSource);

	///	将std::wstring字符串转换为CString字符串
	DYECOMMON_EXT_API CString TranslatewstringToCString(wstring const &strSource);
	
	///	将wstring字符串转换为string字符串
	DYECOMMON_EXT_API string TranslatewstringTostring(wstring const &strSource);

#if (defined _UNICODE) || (defined UNICODE)
	///	将string字符串转换为wstring字符串
	DYECOMMON_EXT_API wstring TranslatestringTowstring(string const &strSource);
#endif

	/// 将CString字符串输出至stringstream
	DYECOMMON_EXT_API void TranslateCStringTostringstream(CString &strSource, stringstream &ss);

#if (defined _UNICODE) || (defined UNICODE)
	/// 将wstring字符串输出至stringstream
	DYECOMMON_EXT_API void TranslatewstringTostringstream(wstring const &strSource, stringstream &ss);
#endif

	/// 将string字符串输出至stringstream
	DYECOMMON_EXT_API void TranslatestringTostringstream(string const &strSource, stringstream &ss);

	///	将std::stringstream字符串输出为CString字符串
	DYECOMMON_EXT_API CString TranslatestringstreamToCString(stringstream const &ss);

#if (defined _UNICODE) || (defined UNICODE)
	///	将std::stringstream字符串输出为wstring字符串
	DYECOMMON_EXT_API wstring TranslatestringstreamTowstring(stringstream const &ss);
#endif

	///	将std::stringstream字符串输出为string字符串
	DYECOMMON_EXT_API string TranslatestringstreamTostring(stringstream const &ss);

#if (defined _UNICODE) || (defined UNICODE)
	/// 返回wstring形式的GUID生成器
	DYECOMMON_EXT_API wstring CreateGuidwstring(BOOL bIncludeJointSign = TRUE);
#endif

	/// 返回string形式的GUID生成器
	DYECOMMON_EXT_API string CreateGuidstring(BOOL bIncludeJointSign = TRUE);


	/// 转换二进制数据为Base64字符串
	DYECOMMON_EXT_API CString TranBytesToBase64Str(IN  DWORD nSizeInByte, IN  PBYTE pData);

};// namespace ns_DYEStringUtils

#endif