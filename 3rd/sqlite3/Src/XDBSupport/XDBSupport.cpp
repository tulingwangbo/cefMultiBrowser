#include "stdafx.h"
#include "XDBSupportAPI.h"
#include "SQLiteSupport.h"


HRESULT CreateXDBInstance( IXDBConnection** ppObj)
{
	*ppObj = new CXDBSQLiteConnection;
	if(*ppObj)
	{
		(*ppObj)->AddRef();
		return S_OK;
	}
	return E_FAIL;
}

BOOL BufToVariant(VARIANT& val, PVOID pBuf, int nSize)
{
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = nSize;
	val.parray = SafeArrayCreate(VT_UI1, 1, rgsabound);
	if(val.parray==NULL)return FALSE;
	val.vt = VT_ARRAY|VT_UI1;

	BYTE *buf = NULL;
	SafeArrayAccessData(val.parray, (void **)&buf); //安全指针
	memcpy(buf, pBuf, nSize);
	SafeArrayUnaccessData(val.parray);

	return TRUE;
}

int BufFromVariantBegin(const VARIANT& val)
{
	if(!((val.vt&VT_ARRAY) && val.parray))
		return 0;
	VARTYPE vt = val.vt&VT_TYPEMASK;
	if(vt!=VT_I1 && vt!=VT_UI1)//目前只处理这一种
		return 0;

	return val.parray->rgsabound[0].cElements;
}

void	BufFromVariantEnd(PVOID pBuf, const VARIANT& val)
{
	if(!((val.vt&VT_ARRAY) && val.parray))
		return;
	VARTYPE vt = val.vt&VT_TYPEMASK;
	if(vt!=VT_I1 && vt!=VT_UI1)//目前只处理这一种
		return;

	long len = val.parray->rgsabound[0].cElements;
	long nBytes = len;

	BYTE *buf = NULL;
	SafeArrayAccessData(val.parray, (void **)&buf); //安全指针
	memcpy(pBuf, buf, nBytes);
	SafeArrayUnaccessData(val.parray);
}

wstring GetDBDateString(IXDBConnection* pDB, bool bUTC)
{
	DWORD dwDBT = pDB->GetDBType();
	LPCTSTR lpszDateTime = NULL;
	//if(dwDBT==XDBTSQLite)
	{
		lpszDateTime = bUTC ? L"SELECT DATETIME('now')" : L"SELECT DATETIME('now','localtime')";
	}
	
	return lpszDateTime;
}

HRESULT	GetDBDateTime(_variant_t& val, IXDBConnection* pDB, bool bUTC)
{
	wstring strDBDateTime;
	strDBDateTime=GetDBDateString(pDB,bUTC);

	CASMPtr<IXDBRecordset> RecdsetPtr;
	HRESULT hr = pDB->Excute(strDBDateTime.c_str(), &RecdsetPtr);
	if(FAILED(hr))return hr;

	hr = RecdsetPtr->Start();
	if(FAILED(hr))return hr;

	return RecdsetPtr->GetFieldValue(0, val);
}
