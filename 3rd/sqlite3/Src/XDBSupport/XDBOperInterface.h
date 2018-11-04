
#pragma once

#include "comutil.h"

//=======================================================================
// IXDBRecordset - 数据库记录集
//=======================================================================
MIDL_INTERFACE("97560C71-4CE9-4A7C-9A57-8E555F8E433A")
IXDBRecordset : public IUnknown
{
	virtual	long	GetFieldCount()const=0;
	virtual	HRESULT	GetFieldName(int nField, OUT _bstr_t& bstrField)const=0;

	//Start() -> GetFieldValue() -> Next()
	virtual HRESULT Start()=0;	//开始获取一条记录
	virtual HRESULT Next()=0;	//继续获取下一条记录
	virtual	HRESULT	GetFieldValue(LPCTSTR lpszField, OUT _variant_t& val)const=0;
	virtual	HRESULT	GetFieldValue(int nField, OUT _variant_t& val, OUT _bstr_t* pField=NULL)const=0;
};

//=======================================================================
// IXDBRecordsetAdd - 向数据库添加记录
//=======================================================================
MIDL_INTERFACE("F96CA966-28BC-479F-B6F1-38538613A4EF")
IXDBRecordsetAdd : public IUnknown
{
	//AddNew()->SetFieldValue()..SetFieldValue() -> Update()
	virtual HRESULT AddNew()=0;	//开始添加一条新记录
	virtual HRESULT Update()=0;	//更新记录到数据库张，可以多次AddNew()，最后一次性Update()
	virtual	HRESULT	SetFieldValue(LPCTSTR lpszField, const _variant_t& val)=0;
};

//=======================================================================
// IXDBConnection - 数据库连接对象
//=======================================================================
MIDL_INTERFACE("F5EE2307-6841-4973-869F-B29242F76825")
IXDBConnection : public IUnknown
{
	virtual	DWORD	GetDBType()const=0;	//获取数据库类型，创建数据库时指定

	virtual HRESULT	Open( LPCTSTR lpszDatabase)=0;
	virtual void	Close()=0;
	virtual	bool	IsOpen()const=0;
	virtual bool    IsExistTable(LPCTSTR lpszTableName)=0;

	virtual	HRESULT	Excute(LPCTSTR lpszCommand, OUT IXDBRecordset** ppRecdset = NULL)=0;
	virtual	HRESULT	AddRecord(LPCTSTR lpszTableName, IXDBRecordsetAdd** ppRecdset)=0;

	virtual	HRESULT	BeginTrans()=0;
	virtual	HRESULT CommitTrans()=0;
	virtual	HRESULT RollbackTrans()=0;
};
