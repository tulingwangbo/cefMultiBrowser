#include "StdAfx.h"
#include "SQLiteSupport.h"
#include "atlcomtime.h"
#include "algorithm"
#include "string"
using namespace std;

#include "CommonLibFunc.h"

#pragma comment(lib, "comsupp.lib")


/************************************************************************/
// CXDBSQLiteRecordset
/************************************************************************/

CXDBSQLiteRecordset::CXDBSQLiteRecordset(sqlite3_stmt *stmt):
m_stmt(stmt)
{
	Start();//要先于下面的过程，否则sqlite3_column_type()始终为SQLITE_NULL

	if(m_stmt)
	{
		int nFieldCount = sqlite3_column_count(m_stmt);
		m_aFiledInfoOrg.SetSize(nFieldCount);
		m_aFiledInfoSort.SetSize(nFieldCount);
		for(int i=0; i<nFieldCount; i++)
		{
			LPCWSTR lpszFieldName = (const WCHAR *)sqlite3_column_name16(m_stmt, i);;

			m_aFiledInfoOrg[i].nField = i;
			m_aFiledInfoOrg[i].wcsName = lpszFieldName;
			m_aFiledInfoOrg[i].vt = XVTEmpty;

			//非标准表的查询，不一定有类型，例如：select cout(*) from xxx;
			const char *pType = sqlite3_column_decltype(m_stmt, i);
			int nType = sqlite3_column_type(m_stmt, i);
			switch(nType)
			{
			case SQLITE_INTEGER:
				m_aFiledInfoOrg[i].vt = XVTInt;
				break;
			case SQLITE_FLOAT:
				m_aFiledInfoOrg[i].vt = XVTDouble;
				break;
			case SQLITE_BLOB:
				{
					if(pType && _stricmp(pType, "GUID")==0)
						m_aFiledInfoOrg[i].vt = XVTGuid;
					else
						m_aFiledInfoOrg[i].vt = XVTBlob;
				}
				break;
			case SQLITE_NULL:
				break;
			case SQLITE_TEXT:
				{
					if(pType)
					{
						if(_stricmp(pType, "DATETIME")==0)
							m_aFiledInfoOrg[i].vt = XVTDate;
						if (strstr(pType, "CHAR") || strstr(pType, "TEXT"))
							m_aFiledInfoOrg[i].vt = XVTString;
					}
					else
					{
						if(_wcsnicmp(lpszFieldName, L"DATETIME(", 9)==0)
							m_aFiledInfoOrg[i].vt = XVTDate;
						else
							m_aFiledInfoOrg[i].vt = XVTString;
					}
				}
				break;
			default:
				break;
			}

			m_aFiledInfoSort[i] = m_aFiledInfoOrg[i];
		}

		if(nFieldCount>0)
		{
			std::sort(m_aFiledInfoSort.GetData(), m_aFiledInfoSort.GetData()+m_aFiledInfoSort.GetCount(), 
				[](SFieldInfo d1, SFieldInfo d2){return _wcsicmp(d1.wcsName, d2.wcsName)<0;});
		}
	}
}

CXDBSQLiteRecordset::~CXDBSQLiteRecordset()
{
	if(m_stmt)
	{
		sqlite3_finalize(m_stmt);
		m_stmt = NULL;
	}
}

HRESULT CXDBSQLiteRecordset::QueryInterface(REFIID riid, void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;
	else if (riid == IID_IUnknown)
	{
		*ppvObject = (IUnknown*)(IXDBRecordset*)this;
		((IUnknown*)*ppvObject)->AddRef();
	}
	else if (riid == __uuidof(IXDBRecordset))
	{
		*ppvObject = (IXDBRecordset *)this;
		((IXDBRecordset*)*ppvObject)->AddRef();
	}
	else
		*ppvObject = NULL;

	return *ppvObject ? S_OK : E_NOINTERFACE;
}

HRESULT CXDBSQLiteRecordset::Start()
{
	if(m_stmt==NULL)
		return E_FAIL;

	int ret = sqlite3_reset(m_stmt);
	ret = sqlite3_step(m_stmt);
	return ret==SQLITE_ROW ? S_OK : E_FAIL;
}

HRESULT CXDBSQLiteRecordset::Next()
{
	if(m_stmt==NULL)
		return E_FAIL;

	int ret = sqlite3_step(m_stmt);
	return ret==SQLITE_ROW ? S_OK : E_FAIL;
}

int CXDBSQLiteRecordset::_FIComp(const void *p1, const void *p2)
{
	LPCTSTR key = (LPCTSTR)p1;
	const CXDBSQLiteRecordset::SFieldInfo* s2 = (const CXDBSQLiteRecordset::SFieldInfo*)p2;
	return _tcsicmp(key, s2->wcsName);
}

HRESULT	CXDBSQLiteRecordset::GetFieldValue(LPCTSTR lpszField, _variant_t& val)const
{
	if(m_stmt==NULL || m_aFiledInfoSort.GetSize()==0 || lpszField==NULL)
		return E_FAIL;

	SFieldInfo* pInfo = (SFieldInfo*)bsearch(lpszField, m_aFiledInfoSort.GetData(), m_aFiledInfoSort.GetSize(), sizeof(SFieldInfo), _FIComp);
	if(pInfo==NULL)return E_FAIL;

	return GetFieldValue(pInfo->nField, val, NULL);
}

HRESULT	CXDBSQLiteRecordset::GetFieldValue(int nField, _variant_t& val, _bstr_t* pField)const
{
	if(m_stmt==NULL || nField<0 || nField>=m_aFiledInfoOrg.GetSize())
		return E_FAIL;
	
	const SFieldInfo& Info = m_aFiledInfoOrg[nField];
	if(pField)
		*pField = Info.wcsName;

	switch(Info.vt)
	{
	case XVTInt:
		val = (long)sqlite3_column_int(m_stmt, nField);
		break;
	case XVTDouble:
		val = sqlite3_column_double(m_stmt, nField);
		break;
	case XVTString:
		val = (LPCWSTR)sqlite3_column_text16(m_stmt, nField);
		break;
	case XVTDate:
		{
			LPCWSTR pV = (LPCWSTR)sqlite3_column_text16(m_stmt, nField);
			//不接受带毫秒的格式
			LPCWSTR pF = _tcschr(pV, '.');
			wstring wstr;
			if(pF)
			{
				wstr.assign(pV, pF-pV);
				pV = wstr.c_str();
			}

			COleDateTime odt;
			odt.ParseDateTime(pV);

			val.Clear();
			val.vt = VT_DATE;
			val.date = odt;
		}
		break;
	case XVTGuid:
		{
			ASSERT(sqlite3_column_bytes(m_stmt, nField)>=sizeof(GUID));

			const GUID* pGuid = (const GUID*)sqlite3_column_blob(m_stmt, nField);
			WCHAR strGUID[40];
			GuidToString(*pGuid, strGUID, true, true);
			val = strGUID;
		}
		break;
	case XVTBlob:
		{
			const void *pData = sqlite3_column_blob(m_stmt, nField);
			int nBytes = sqlite3_column_bytes(m_stmt, nField);

			SAFEARRAYBOUND rgsabound[1];
			rgsabound[0].lLbound = 0;
			rgsabound[0].cElements = nBytes;
			SAFEARRAY *psa = SafeArrayCreate(VT_UI1, 1, rgsabound);

			//PBYTE pD = (PBYTE)pData;
			//for(long i=0; i<nBytes; i++)
			//	SafeArrayPutElement(psa, &i, &pD[i]);

			BYTE *buf = NULL;
			SafeArrayAccessData(psa, (void **)&buf); //安全指针
			memcpy(buf, pData, nBytes);
			SafeArrayUnaccessData(psa);

			val.Clear();
			val.vt = VT_ARRAY|VT_UI1;
			val.parray = psa;
		}
		break;
	default:
		ASSERT(false);
		return E_FAIL;
	}
	return S_OK;
}

long CXDBSQLiteRecordset::GetFieldCount()const
{
	if(m_stmt==NULL)
		return 0;

	return m_aFiledInfoOrg.GetSize();
}

HRESULT	CXDBSQLiteRecordset::GetFieldName(int nField, _bstr_t& bstrField)const
{
	if(m_stmt==NULL || nField<0 || nField>=m_aFiledInfoOrg.GetSize())
		return E_FAIL;
	bstrField = m_aFiledInfoOrg[nField].wcsName;
	return S_OK;
}


/************************************************************************/
// CXDBSQLiteRecordsetAdd
// 第一轮，先收集字段名称，后面都按照此进行绑定
/************************************************************************/

CXDBSQLiteRecordsetAdd::CXDBSQLiteRecordsetAdd(sqlite3*	db, LPCTSTR lpszTableName):
m_stmt(NULL),m_db(db),m_strTableName(CW2M(lpszTableName, CP_UTF8))
{

}

CXDBSQLiteRecordsetAdd::~CXDBSQLiteRecordsetAdd()
{

}

HRESULT CXDBSQLiteRecordsetAdd::QueryInterface(REFIID riid, void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;
	else if (riid == IID_IUnknown)
	{
		*ppvObject = (IUnknown*)(IXDBRecordsetAdd*)this;
		((IUnknown*)*ppvObject)->AddRef();
	}
	else if (riid == __uuidof(IXDBRecordsetAdd))
	{
		*ppvObject = (IXDBRecordsetAdd *)this;
		((IXDBRecordsetAdd*)*ppvObject)->AddRef();
	}
	else
		*ppvObject = NULL;

	return *ppvObject ? S_OK : E_NOINTERFACE;
}

static bool _BindData(sqlite3* db, sqlite3_stmt* stmt, int nField, const _variant_t& val)
{
	int nRet = SQLITE_OK;
	switch(val.vt)
	{
	case VT_I2:
	case VT_I4:
	case VT_BOOL:
	case VT_I1:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
	case VT_INT:
	case VT_UINT:
		nRet = sqlite3_bind_int(stmt, nField, val.lVal);
		break;
	case VT_I8:
	case VT_UI8:
		nRet = sqlite3_bind_int64(stmt, nField, val.llVal);
		break;
	case VT_R4:
		nRet = sqlite3_bind_double(stmt, nField, val.fltVal);
		break;
	case VT_R8:
		nRet = sqlite3_bind_double(stmt, nField, val.dblVal);
		break;
	case VT_DATE:
		{
			COleDateTime dt(val.date);
			CString strDT = dt.Format(L"%y-%m-%d %H:%M:%S");
			nRet = sqlite3_bind_text16(stmt, nField, (LPCTSTR)strDT, -1, SQLITE_STATIC);
		}
		break;
	case VT_BSTR:
		nRet = sqlite3_bind_text16(stmt, nField, val.bstrVal, -1, SQLITE_STATIC);
		break;
	default:
		{
			if((val.vt&VT_ARRAY) && val.parray)
			{
				VARTYPE vt = val.vt&VT_TYPEMASK;

				long len = val.parray->rgsabound[0].cElements;
				long nBytes = len;
				ASSERT(vt==VT_I1 || vt==VT_UI1);

				BYTE *buf = NULL;
				SafeArrayAccessData(val.parray, (void **)&buf); //安全指针
				nRet = sqlite3_bind_blob(stmt, nField, buf, nBytes, NULL);
				SafeArrayUnaccessData(val.parray);
			}
			else
			{
				return false;
			}
		}
		break;
	}

	if( nRet != SQLITE_OK)
	{ 
		const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(db);//获得错误描述
		OutputDebugString(pErrMsg2);
		return false;
	}

	return true;
}

HRESULT	CXDBSQLiteRecordsetAdd::_UpdateRecord()
{
	//三种情况：
	//1、m_stmt==NULL 并且 m_aFiledInfo为空，首次插入数据，这时要插入的字段还不知道，只能等后续SetFieldValue时进行收集，这里什么都做不了
	//2、m_stmt==NULL 并且 m_aFiledInfo不为空，说明首次数据都准备好了，而且字段名称都统计出来了，这时开始进行首次的数据插入
	//3、m_stmt!=NULL 时 m_aFiledInfo必不为空，而且数据都已经绑定好了，只需要进行提交即可

	if(m_stmt==NULL && m_aFiledInfo.GetSize()>0)//进行首次数据的插入
	{
		ASSERT(m_aFiledInfo.GetSize()==m_lValStore.size());

		string strFieldName;
		string strFieldMark;
		for(int i=0; i<m_aFiledInfo.GetSize(); i++)
		{
			if(!strFieldName.empty())
				strFieldName += ",";
			strFieldName += CW2M(m_aFiledInfo[i].strName.c_str(), CP_UTF8);
			strFieldMark += (i==0) ? "?" : ",?";
		}
		//"INSERT INTO Table (name,num) VALUES(?,?);"
		string strSQL = string("INSERT INTO ") + m_strTableName + " ("+ strFieldName +") VALUES(" + strFieldMark +");";

		//开始插入 
		const char *zTail = NULL;
		int ret = sqlite3_prepare(m_db, strSQL.c_str(), -1, &m_stmt, &zTail); 
		if( ret != SQLITE_OK )
		{
			const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(m_db);//获得错误描述
			OutputDebugString(pErrMsg2);
			return E_FAIL;
		}

		//绑定数据
		int i=0;
		for(varlist::iterator it = m_lValStore.begin(); it!=m_lValStore.end(); it++,i++)
		{
			_BindData(m_db, m_stmt, m_aFiledInfo[i].nField, *it);
		}

		m_lValStore.clear();
		//对字段进行排序，为后续继续插入过程做优化准备
		std::sort(m_aFiledInfo.GetData(), m_aFiledInfo.GetData()+m_aFiledInfo.GetCount(), 
			[](SFieldInfo d1, SFieldInfo d2){return _tcsicmp(d1.strName.c_str(), d2.strName.c_str())<0;});
	}

	if(m_stmt)
	{
		ASSERT(m_aFiledInfo.GetSize()>0);
		ASSERT(m_lValStore.size()==0);//m_stmt有效时，必定不会使用m_lValStore

		if( sqlite3_step(m_stmt) != SQLITE_DONE)
		{ 
			const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(m_db);//获得错误描述
			OutputDebugString(pErrMsg2);
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CXDBSQLiteRecordsetAdd::AddNew()
{
	if(m_db==NULL)return E_FAIL;

	HRESULT hr = _UpdateRecord();
	if(FAILED(hr))return hr;

	if(m_stmt)
		sqlite3_reset(m_stmt);

	return S_OK;
}

HRESULT CXDBSQLiteRecordsetAdd::Update()
{
	if(m_db==NULL)return E_FAIL;

	HRESULT hr = _UpdateRecord();
	if(FAILED(hr))return hr;

	int ret = sqlite3_finalize(m_stmt);
	m_stmt = NULL;
	m_aFiledInfo.RemoveAll();
	if( ret != SQLITE_OK )
	{
		const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(m_db);//获得错误描述
		OutputDebugString(pErrMsg2);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	CXDBSQLiteRecordsetAdd::SetFieldValue(LPCTSTR lpszField, const _variant_t& val)
{
	if(m_db==NULL)return E_FAIL;

	if(lpszField==NULL || lpszField[0]==0)
		return E_FAIL;

	if(m_stmt==NULL)//首轮先收集字段名称信息
	{
		bool bExist = false;
		for(int i=0; i<m_aFiledInfo.GetSize(); i++)
		{
			SFieldInfo& info = m_aFiledInfo[i];
			if(_tcsicmp(info.strName.c_str(), lpszField)==0)
			{
				ASSERT(FALSE);
				varlist::iterator it = m_lValStore.begin();
				for(int j=0; j<i; j++)
					it++;
				*it = val;
				bExist = true;
				break;
			}
		}

		if(!bExist)
		{
			SFieldInfo FI = {lpszField, m_aFiledInfo.GetSize()+1};//nField从1开始
			m_aFiledInfo.Add(FI);
			m_lValStore.push_back(val);
		}
	}
	else//直接绑定数据
	{
		SFieldInfo* pInfo = (SFieldInfo*)bsearch(lpszField, m_aFiledInfo.GetData(), m_aFiledInfo.GetSize(), sizeof(SFieldInfo), _FIComp);
		if(pInfo==NULL)return E_FAIL;

		_BindData(m_db, m_stmt, pInfo->nField, val);
	}

	return S_OK;
}

int CXDBSQLiteRecordsetAdd::_FIComp(const void *p1, const void *p2)
{
	LPCTSTR key = (LPCTSTR)p1;
	const CXDBSQLiteRecordsetAdd::SFieldInfo* s2 = (const CXDBSQLiteRecordsetAdd::SFieldInfo*)p2;
	return _tcsicmp(key, s2->strName.c_str());
}

/************************************************************************/
// CXDBSQLiteConnection
/************************************************************************/

CXDBSQLiteConnection::CXDBSQLiteConnection():
m_db(NULL)
{

}

CXDBSQLiteConnection::~CXDBSQLiteConnection()
{
	Close();
}

HRESULT CXDBSQLiteConnection::QueryInterface(REFIID riid, void** ppvObject)
{
	if (ppvObject == NULL)
		return E_POINTER;
	else if (riid == IID_IUnknown)
	{
		*ppvObject = (IUnknown*)(IXDBConnection*)this;
		((IUnknown*)*ppvObject)->AddRef();
	}
	else if (riid == __uuidof(IXDBConnection))
	{
		*ppvObject = (IXDBConnection *)this;
		((IXDBConnection*)*ppvObject)->AddRef();
	}
	else
		*ppvObject = NULL;

	return *ppvObject ? S_OK : E_NOINTERFACE;
}

HRESULT CXDBSQLiteConnection::Open( LPCTSTR lpszDatabase)
{
	if(m_db!=NULL)
		return E_FAIL;

	LPCTSTR lpszDB = lpszDatabase;
	if(lpszDB==NULL || lpszDB[0]==0)return E_FAIL;

	string strDB = CW2M(lpszDB, CP_UTF8);

	//连接数据库
	int ret = sqlite3_open(strDB.c_str(), &m_db);

	if(ret != SQLITE_OK)
	{
		const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(m_db);//获得错误描述
		sqlite3_close(m_db);
		OutputDebugString(pErrMsg2);
		return E_FAIL;
	}

	sqlite3_busy_timeout(m_db, 5000);

	return S_OK;
}

void CXDBSQLiteConnection::Close()
{
// 	if(m_db)
// 	{
// 		sqlite3_close(m_db);
// 		m_db = NULL;
// 	}

	//If sqlite3_close() is called on a database connection 
	//that still has outstanding prepared statements or BLOB handles, then it returns SQLITE_BUSY
	if(m_db)
	{
		sqlite3 *db = m_db;
		m_db = NULL;
		int rc = sqlite3_close(db);

		while( rc == SQLITE_BUSY)
		{
			// set rc to something that will exit the while loop
			rc = SQLITE_OK;
			sqlite3_stmt * stmt = sqlite3_next_stmt(db, NULL);

			if(stmt != NULL)
			{
				rc = sqlite3_finalize(stmt);
				if(rc == SQLITE_OK)
				{
					rc = sqlite3_close(db);
				}
			}
		}
	}
}

bool CXDBSQLiteConnection::IsOpen()const
{
	return m_db!=NULL;
}

bool  CXDBSQLiteConnection::IsExistTable(LPCTSTR lpszTableName)
{
	char szSQL[128];
	string sTb = CW2M(lpszTableName, CP_UTF8);
	sprintf(szSQL,
		"select count(*) from sqlite_master where type='table' and name='%s'",
		sTb.data());

	char* szError=0;
	sqlite3_stmt* pVM;

	int nRet = sqlite3_prepare_v2(m_db, szSQL, -1, &pVM,/* &szTail*/NULL);
	if (nRet != SQLITE_OK)
	{
		szError = (char*)sqlite3_errmsg(m_db);
		return false;
	}

	nRet = sqlite3_step(pVM);
	if (nRet == SQLITE_ROW)
	{
		const unsigned char *pVal = sqlite3_column_text(pVM, 0);
		int iNum = atoi((const char*)pVal);
		sqlite3_finalize(pVM);
		return iNum > 0;
	}
	
	sqlite3_finalize(pVM);
	return false;
}

HRESULT	CXDBSQLiteConnection::Excute(LPCTSTR lpszCommand, IXDBRecordset** ppRecdset)
{
	if(ppRecdset)
		*ppRecdset = NULL;

	if(m_db==NULL)return E_FAIL;

	if(ppRecdset)
	{
		sqlite3_stmt *stmt = NULL;
		const char *zTail = NULL;
		int ret = sqlite3_prepare(m_db, CW2M(lpszCommand, CP_UTF8), -1, &stmt, &zTail); 
		if( ret != SQLITE_OK )
		{
			const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(m_db);//获得错误描述
			OutputDebugString(pErrMsg2);
			return E_FAIL;
		}

		*ppRecdset = new CXDBSQLiteRecordset(stmt);
		(*ppRecdset)->AddRef();
	}
	else
	{
		int ret = sqlite3_exec(m_db, CW2M(lpszCommand, CP_UTF8), NULL, NULL, NULL);
		if( ret != SQLITE_OK )
		{
			const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(m_db);//获得错误描述
			OutputDebugString(pErrMsg2);
			return E_FAIL;
		}else{
			int iRet = sqlite3_changes(m_db);
		}
	}

	return S_OK;
}

HRESULT	CXDBSQLiteConnection::AddRecord(LPCTSTR lpszTableName, IXDBRecordsetAdd** ppRecdset)
{
	if(m_db==NULL)return E_FAIL;

	ASSERT(ppRecdset);
	*ppRecdset = new CXDBSQLiteRecordsetAdd(m_db, lpszTableName);
	(*ppRecdset)->AddRef();

	return S_OK;
}

HRESULT CXDBSQLiteConnection::BeginTrans()
{
	if(m_db==NULL)return -1;

	int ret = sqlite3_exec(m_db, "begin;", 0, 0, 0);  // 开启事务

	if(ret!=SQLITE_OK)
	{
		const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(m_db);//获得错误描述
		OutputDebugString(pErrMsg2);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CXDBSQLiteConnection::CommitTrans()
{
	if(m_db==NULL)return E_FAIL;

	int ret = sqlite3_exec(m_db, "commit;", 0, 0, 0);  // 提交事务

	if(ret!=SQLITE_OK)
	{
		const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(m_db);//获得错误描述
		OutputDebugString(pErrMsg2);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CXDBSQLiteConnection::RollbackTrans()
{
	if(m_db==NULL)return E_FAIL;

	int ret = sqlite3_exec(m_db, "rollback;", 0, 0, 0);  // 回滚事务

	if(ret!=SQLITE_OK)
	{
		const WCHAR* pErrMsg2 = (const WCHAR*)sqlite3_errmsg16(m_db);//获得错误描述
		OutputDebugString(pErrMsg2);

		return E_FAIL;
	}

	return S_OK;
}