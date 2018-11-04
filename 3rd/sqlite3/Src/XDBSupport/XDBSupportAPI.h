#pragma once

#include "XDBSupportDef.h"
#include "XDBOperInterface.h"

//--------------------------------------------
// CreateXDBInstance
//--------------------------------------------
HRESULT CreateXDBInstance(IXDBConnection** ppObj);

//--------------------------------------------
// DB oper help
//--------------------------------------------

//获取数据库当前时间字符
wstring GetDBDateString(IXDBConnection* pDB, bool bUTC);
//获取数据库的当前时间
HRESULT	GetDBDateTime(_variant_t& val, IXDBConnection* pDB, bool bUTC=true);

//--------------------------------------------
// Variant help
//--------------------------------------------

BOOL	BufToVariant(VARIANT& val, PVOID pBuf, int nSize);

int		BufFromVariantBegin(const VARIANT& val);//返回数据量
void	BufFromVariantEnd(PVOID pBuf, const VARIANT& val);