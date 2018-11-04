
//*********************************************************
// 文件名：DYEStringUtils.cpp
// 作  者：lvkun
// 日  期：
// 目  的：字符串实用类——实现文件
// 备  注：hesl整理于2008-07-23！ 
//*********************************************************

#include "stdafx.h"
#include "DYEStringUtils.h"
#include "CharSetTransferMacro.h"
#include <atlenc.h>



namespace ns_DYEStringUtils {

CString TransUTF8ToUnicode(const char* pszSource)
{
#if (defined _UNICODE) || (defined UNICODE)
	USES_MYCP_CONVERSION;
	USES_CP_UTF8;
	return CP2W(pszSource, CP_UTF8);
#else
	// 多字节模式
	return (CString)pszSource;
#endif
}


string TransUnicodeToUTF8(CString strSource)
{
#if (defined _UNICODE) || (defined UNICODE)
	USES_MYCP_CONVERSION;
	USES_CP_UTF8;
	return W2CP(strSource.GetBuffer(0), CP_UTF8);
#else
	// 多字节模式
	return strSource.GetBuffer(0);
#endif
}


string TranslateCStringTostring( CString &strSource )
{
	return TransUnicodeToUTF8( strSource );
}

#if (defined _UNICODE) || (defined UNICODE)
wstring TranslateCStringTowstring( CString const &strSource )
{
	return (LPCWSTR)strSource;
}
#endif


CString TranslatestringToCString(string const &strSource)
{
	return TransUTF8ToUnicode( strSource.c_str() );
}


CString TranslatewstringToCString(wstring const &strSource)
{
	return (CString)strSource.c_str();
}


CString TranslatestringstreamToCString(stringstream const &ss)
{
	string strTemp = ss.str();
	return TranslatestringToCString(strTemp);
}


#if (defined _UNICODE) || (defined UNICODE)
wstring TranslatestringstreamTowstring(stringstream const &ss)
{
	return TranslateCStringTowstring(TranslatestringstreamToCString(ss));
}
#endif

string TranslatewstringTostring(wstring const &strSource)
{
	return TranslateCStringTostring(TranslatewstringToCString(strSource));
}


#if (defined _UNICODE) || (defined UNICODE)
wstring TranslatestringTowstring(string const &strSource)
{
	return TranslateCStringTowstring(TranslatestringToCString(strSource));
}
#endif


void TranslateCStringTostringstream(CString &strSource, stringstream &ss)
{
	ss.str( TranslateCStringTostring(strSource) );
}


#if (defined _UNICODE) || (defined UNICODE)
void TranslatewstringTostringstream(wstring const &strSource, stringstream &ss)
{
	ss.str(TranslatewstringTostring(strSource));
}
#endif


void TranslatestringTostringstream(string const &strSource, stringstream &ss)
{
	ss.str(strSource);
}


string TranslatestringstreamTostring(stringstream const &ss)
{
	return (string)ss.str();
}


// 生成GUID字符串。参数的含义：
// BOOL bIncludeJointSign /*= TRUE*/——[in]指示产生的GUID字符串是否包含连接符“-”。默认为包含！
// 备注：为避免DYEStringUtils依赖于DYECommonUtils，将DYECommonUtils.cpp中的这个函数直接拷贝到这里来！
CString DYECoCreateGuidString(BOOL bIncludeJointSign /*= TRUE*/)
{
	GUID guid = GUID_NULL;
	CoCreateGuid(&guid);

#ifdef UNICODE
	WCHAR* pUuidString;
#else
	BYTE* pUuidString;
#endif

#if (defined _UNICODE) || (defined UNICODE)
	UuidToString(&guid, (RPC_WSTR*)&pUuidString);
	CString str = pUuidString;
	RpcStringFree((RPC_WSTR*)&pUuidString);
#else
	UuidToString(&guid, (RPC_CSTR*)&pUuidString);
	CString str = pUuidString;
	RpcStringFree((RPC_CSTR*)&pUuidString);
#endif

	if( !bIncludeJointSign )
	{
		str.Replace(_T("-"), _T(""));
	}

	str.MakeUpper();
	return str;	
}


// 返回string形式的GUID生成器。参数的含义：
// BOOL bIncludeJointSign /*= TRUE*/——[in]指示产生的GUID字符串是否包含连接符“-”。默认为包含！
string CreateGuidstring(BOOL bIncludeJointSign /*= TRUE*/)
{
	return TranslateCStringTostring( DYECoCreateGuidString(bIncludeJointSign) );
}


#if (defined _UNICODE) || (defined UNICODE)
// 返回string形式的GUID生成器。参数的含义：
// BOOL bIncludeJointSign /*= TRUE*/——[in]指示产生的GUID字符串是否包含连接符“-”。默认为包含！
wstring CreateGuidwstring(BOOL bIncludeJointSign /*= TRUE*/)
{
	return TranslateCStringTowstring( DYECoCreateGuidString(bIncludeJointSign) );
}
#endif


// 转换二进制数据为Base64字符串
CString TranBytesToBase64Str(IN  DWORD nSizeInByte, IN  PBYTE pData)
{
	if ( 0 == nSizeInByte || NULL == pData )
	{
		return _T("");
	}

	CHAR * pCharTemp = new CHAR[nSizeInByte*2];
	memset(pCharTemp, 0, nSizeInByte*2); 

	int nDesLen = nSizeInByte*2;

	if ( !::Base64Encode( (BYTE*)pData, nSizeInByte, (LPSTR)pCharTemp, &nDesLen ) )
	{
		OutputDebugString(_T("CDYEXMLHandler::CreateNode---Base64编码失败！"));

		delete [] pCharTemp;
		pCharTemp = NULL;

		return _T("");
	}

	CString strBase64( pCharTemp );

	delete [] pCharTemp;
	pCharTemp = NULL;

	return strBase64;
}

};// namespace ns_DYEStringUtils










