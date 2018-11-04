#include "StdAfx.h"
#include "DataLog.h"

CTableDataLogAccessor::CTableDataLogAccessor(void)
: m_strTableName(TableDataLog_Name)
{
	
}
CTableDataLogAccessor::CTableDataLogAccessor(const CppSQLite3DB& sqlite3DB)
{
	m_sqliteDB = sqlite3DB;
}


CTableDataLogAccessor::~CTableDataLogAccessor(void)
{
}

// 创建表
BOOL CTableDataLogAccessor::CreateTable()
{
	CString strSQL;
	strSQL.Format(_T("create table %s(%s %s, %s %s, %s %s, %s %s, %s %s, %s %s, %s %s, %s %s);"),
		m_strTableName, 
		TableDataLog_ID, _T("char(36)"),
		TableDataLog_FileName, _T("char(255)"),
		TableDataLog_FileSize, _T("char(255)"),
		TableDataLog_FileExt, _T("char(255)"),
		TableDataLog_StartTime, _T("char(64)"),
		TableDataLog_EndTime, _T("char(64)"),
		TableDataLog_KillStatus, _T("char(64)"),
		TableDataLog_TaskColumn, _T("char(255)"));
	

	return __super::CreateTable(m_strTableName, strSQL);
}

// 删除表
BOOL CTableDataLogAccessor::DropTable()
{
	return __super::DropTable(m_strTableName);
}

// 插入数据
int CTableDataLogAccessor::InsertData(STableDataLog& sDataLog)
{
	CString strSQL;
	strSQL.Format(_T("insert into %s values (\'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\');"),
		TableDataLog_Name,
		sDataLog.strID,
		sDataLog.strFileName,
		sDataLog.strFileSize,
		sDataLog.strFileExt,
		sDataLog.strStartTime,
		sDataLog.strEndTime,
		sDataLog.strKillStatus,
		sDataLog.strTask);

	return __super::Execute(strSQL);
}

// 删除数据
int CTableDataLogAccessor::DeleteData(CString strID)
{
	CString strSQL;
	strSQL.Format(_T("delete from %s where %s=\'%s\'"), m_strTableName, TableDataLog_ID, strID);

	return __super::Execute(strSQL);
}

// 获取数据
int CTableDataLogAccessor::GetAllDatas(OUT ASTableDataLogArray& asTableDB, IN CString strCondition/* = _T("")*/)
{
	asTableDB.RemoveAll();

	CString strSQL;
	strSQL.Format(_T("select * from %s where %s"), m_strTableName, strCondition.IsEmpty() ? _T("1=1") : strCondition);

	CppSQLite3Query query;
	__super::QueryData(strSQL, query);

	while(!query.eof())
	{
		STableDataLog sTableDb;
		sTableDb.strID = TransUTF8ToUnicode(query.fieldValue(TransUnicodeToUTF8(TableDataLog_ID).c_str()));
		sTableDb.strFileName = TransUTF8ToUnicode(query.fieldValue(TransUnicodeToUTF8(TableDataLog_FileName).c_str()));
		sTableDb.strFileSize = TransUTF8ToUnicode(query.fieldValue(TransUnicodeToUTF8(TableDataLog_FileSize).c_str()));
		sTableDb.strFileExt = TransUTF8ToUnicode(query.fieldValue(TransUnicodeToUTF8(TableDataLog_FileExt).c_str()));
		sTableDb.strStartTime = TransUTF8ToUnicode(query.fieldValue(TransUnicodeToUTF8(TableDataLog_StartTime).c_str()));
		sTableDb.strEndTime = TransUTF8ToUnicode(query.fieldValue(TransUnicodeToUTF8(TableDataLog_EndTime).c_str()));
		sTableDb.strKillStatus = TransUTF8ToUnicode(query.fieldValue(TransUnicodeToUTF8(TableDataLog_KillStatus).c_str()));
		sTableDb.strTask = TransUTF8ToUnicode(query.fieldValue(TransUnicodeToUTF8(TableDataLog_TaskColumn).c_str()));

		asTableDB.Add(sTableDb);
		
		query.nextRow();
	}

	return asTableDB.GetSize();
}
