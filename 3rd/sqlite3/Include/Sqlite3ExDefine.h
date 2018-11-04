#if !defined(__H_SQLITE3EX_DEFINE_INCLUDE_H__)
#define __H_SQLITE3EX_DEFINE_INCLUDE_H__

//////////////////////////////////////////////////////////////////////////
//	Sqlite3ExDefine.h：封装的数据库连接类和数据表访问类，后者可以继承使用
//
//	CSqlite3Database：Sqlite3数据库连接类
//		Sqlite3数据库文件后缀一般为.db，在打开数据库文件时，如果文件不存在，
//		会自动创建。
//
//	CSqlite3DBAccessor：Sqlite3数据表访问基类，使用时，直接从该类继承即可
//		
//////////////////////////////////////////////////////////////////////////

#include "CppSQLite3.h"

// Sqlite3数据库连接类
class SQLITE3EX_CLASS CSqlite3Database
{
public:
	CSqlite3Database();
	virtual ~CSqlite3Database();

public:
	// SQLite3版本
	CString GetSQLiteVersion();

	// 打开数据库
	BOOL OpenDatabase(CString strDBFile)throw (CppSQLite3Exception&);

	// 获取数据库路径
	CString GetDatabasePath();

	// 关闭数据库
	void CloseDatabase();

	// 
	CppSQLite3DB& GetSqlite3DB();

	//
	BOOL IsDBOpen();

	//数据库事务相关操作
	bool BeginTrans();

	bool CommitTrans();

	bool Rollback();


private:
	CppSQLite3DB	m_sqliteDB;
	BOOL			m_bDBOpen;
	CString			m_strDBFilePath;
};

// Sqlite3数据库访问类
class SQLITE3EX_CLASS CSqlite3DBAccessor
{
public:
	CSqlite3DBAccessor();
	CSqlite3DBAccessor(const CppSQLite3DB& sqlite3DB);
	virtual ~CSqlite3DBAccessor();

	// 表存在性判断
	BOOL IsTableExist(CString strTableName);

	// 视图是否存在
	bool IsExistView(CString strViewName);

	// 设置Sqlite3DB
	void SetSqlite3DB(const CppSQLite3DB& sqlite3DB);

	// 创建数据表
	// 返回值：TRUE-成功，FALSE-失败
	BOOL CreateTable(CString strTableName, CString strSQL);

	// 删除表
	BOOL DropTable(CString strTableName);

	// 查询数据
	// 根据CppSQLite3Query对象可以得到查出的所有数据
	void QueryData(IN CString strSQL, OUT CppSQLite3Query& query);

	// 判断指定数据库表中是否存在指定字段
	BOOL IsExistField(CString strTableName, CString strFieldName);

	// 获取最近插入行号
	long GetLastInsertRowID();

	// 执行SQL
	// 返回值：影响行数
	int Execute(CString strSQL);

	CppSQLite3Statement CompileStatement(const char* pData);

private:
	CppSQLite3DB	m_sqliteDB;
};

#endif // __H_SQLITE3EX_DEFINE_INCLUDE_H__