#pragma once

//////////////////////////////////////////////////////////////////////////
//	数据表、字段名称定义
//////////////////////////////////////////////////////////////////////////

// 数据表
#define TableDataLog_Name			_T("Db")
// 字段名称
#define TableDataLog_ID				_T("ID")
#define TableDataLog_FileName		_T("FileName")
#define TableDataLog_FileSize		_T("FileSize")
#define TableDataLog_FileExt		_T("FileExt")
#define TableDataLog_StartTime		_T("StartTime")
#define TableDataLog_EndTime		_T("EndTime")
#define TableDataLog_KillStatus		_T("KillStatus")
#define TableDataLog_TaskColumn		_T("TaskColumn")

//////////////////////////////////////////////////////////////////////////
// 数据表结构定义
//////////////////////////////////////////////////////////////////////////

struct STableDataLog
{
	CString strID;
	CString strFileName;		// 文件名
	CString strFileSize;		// 文件大小
	CString strFileExt;			// 文件类型
	CString strStartTime;		// 任务开始时间
	CString strEndTime;			// 任务结束时间
	CString strKillStatus;		// 杀毒状态
	CString strTask;			// 任务栏目

	STableDataLog()
	{
		strID = _T("00000000-0000-0000-0000-000000000000");
		strFileName = _T("");
		strFileExt = _T("*.*");
		strStartTime = _T("");
		strEndTime = _T("");
		strKillStatus = _T("");
		strTask = _T("");
	}

	STableDataLog& operator = (const STableDataLog& other)
	{
		if(this != &other)
		{
			strID = other.strID;
			strFileName = other.strFileName;
			strFileSize = other.strFileSize;
			strFileExt = other.strFileExt;
			strStartTime = other.strStartTime;
			strEndTime = other.strEndTime;
			strKillStatus = other.strKillStatus;
			strTask = other.strTask;
		}

		return *this;
	}
};

typedef CArray<STableDataLog, STableDataLog&>	ASTableDataLogArray;


//////////////////////////////////////////////////////////////////////////
// 数据表访问类
//////////////////////////////////////////////////////////////////////////


class CTableDataLogAccessor : public CSqlite3DBAccessor
{
public:
	CTableDataLogAccessor(void);
	CTableDataLogAccessor(const CppSQLite3DB& sqlite3DB);
	virtual ~CTableDataLogAccessor(void);

public:
	// 创建表
	BOOL CreateTable();

	// 删除表
	BOOL DropTable();

	// 插入数据
	int InsertData(STableDataLog& sDataLog);

	// 删除数据
	int DeleteData(CString strID);

	// 获取数据
	int GetAllDatas(OUT ASTableDataLogArray& asTableDB, IN CString strCondition = _T(""));

private:
	CppSQLite3DB	m_sqliteDB;
	CString			m_strTableName;
};
