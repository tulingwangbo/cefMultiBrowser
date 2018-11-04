// SQLite3DemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SQLite3Demo.h"
#include "SQLite3DemoDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSQLite3DemoDlg 对话框




CSQLite3DemoDlg::CSQLite3DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSQLite3DemoDlg::IDD, pParent)
	, m_strSqliteVersion(_T(""))
	, m_strDBFile(_T("DataLog.db"))
	, m_strFileName(_T(""))
	, m_strFileSize(_T("0"))
	, m_strFileExt(_T(""))
	, m_strKillStatus(_T("安全"))
	, m_strTask(_T("摆渡杀毒"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSQLite3DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SQLITE_VERSION, m_strSqliteVersion);
	DDX_Text(pDX, IDC_EDIT_DBFILE, m_strDBFile);
	DDX_Control(pDX, IDC_LIST_DBDATA, m_listDbData);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_EDIT_FILESIZE, m_strFileSize);
	DDX_Text(pDX, IDC_EDIT_FILEEXT, m_strFileExt);
	DDX_Text(pDX, IDC_EDIT_KILLSTATUS, m_strKillStatus);
	DDX_Text(pDX, IDC_EDIT_TASK, m_strTask);
}

BEGIN_MESSAGE_MAP(CSQLite3DemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_INSERTDATA, &CSQLite3DemoDlg::OnBtnInsertData)
	ON_BN_CLICKED(IDC_BTN_DELETEDATA, &CSQLite3DemoDlg::OnBtnDeleteData)
	ON_BN_CLICKED(IDC_BTN_BROWSEFILE, &CSQLite3DemoDlg::OnBtnBrowseFile)
END_MESSAGE_MAP()


// CSQLite3DemoDlg 消息处理程序

BOOL CSQLite3DemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	InitChildCtrls();

	// 打开数据库显示记录
	OpenDatabase();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSQLite3DemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSQLite3DemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CSQLite3DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CSQLite3DemoDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_sqlite3DB.CloseDatabase();

	return CDialog::DestroyWindow();
}

void CSQLite3DemoDlg::OnBtnInsertData()
{
	UpdateData(TRUE);

	if(m_strFileName.IsEmpty())
	{
		AfxMessageBox(_T("文件名称为空！"));
		return;
	}

	STableDataLog sTableDb;
	sTableDb.strID = TransUTF8ToUnicode(CreateGuidstring(TRUE).c_str());
	sTableDb.strFileName = m_strFileName;
	sTableDb.strFileSize = m_strFileSize;
	sTableDb.strFileExt = m_strFileExt;
	CTime time = CTime::GetCurrentTime();
	sTableDb.strStartTime = time.Format(_T("%Y-%m-%d %H:%M:%S"));
	sTableDb.strEndTime = time.Format(_T("%Y-%m-%d %H:%M:%S"));
	sTableDb.strKillStatus = m_strKillStatus;
	sTableDb.strTask = m_strTask;

	UpdateData(FALSE);

	m_dataLogAccessor.InsertData(sTableDb);

	InitDbListDatas();
}

void CSQLite3DemoDlg::OnBtnDeleteData()
{
	POSITION pos = m_listDbData.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		AfxMessageBox(_T("No items were selected!"));
		return;
	}

	while (pos)
	{
		int nItem = m_listDbData.GetNextSelectedItem(pos);
		CString strID = m_listDbData.GetItemText(nItem, 0);

		m_dataLogAccessor.DeleteData(strID);
		InitDbListDatas();
	}

	UpdateData(TRUE);
}

void CSQLite3DemoDlg::InitChildCtrls()
{
	UpdateData(TRUE);

	m_strSqliteVersion = m_sqlite3DB.GetSQLiteVersion();
	
	CString strCurPath = GetModuleDirectory(AfxGetInstanceHandle());
	strCurPath.TrimRight(_T("\\"));
	strCurPath += _T("\\");
	strCurPath += m_strDBFile;
	m_strDBFile = strCurPath;

	UpdateData(FALSE);


	SetListCtrlStyle(m_listDbData);
	// 设置表头
	int nIndex = 0;
	m_listDbData.InsertColumn( nIndex++, _T("ID"), LVCFMT_LEFT, 200 );
	m_listDbData.InsertColumn( nIndex++, _T("文件名称"), LVCFMT_LEFT, 300 );
	m_listDbData.InsertColumn( nIndex++, _T("文件大小"), LVCFMT_LEFT, 100 );
	m_listDbData.InsertColumn( nIndex++, _T("文件类型"), LVCFMT_LEFT, 150 );
	m_listDbData.InsertColumn( nIndex++, _T("任务开始时间"), LVCFMT_LEFT, 200 );
	m_listDbData.InsertColumn( nIndex++, _T("任务结束时间"), LVCFMT_LEFT, 200 );
	m_listDbData.InsertColumn( nIndex++, _T("杀毒状态"), LVCFMT_LEFT, 100 );
	m_listDbData.InsertColumn( nIndex++, _T("任务栏目"), LVCFMT_LEFT, 150 );
}

void CSQLite3DemoDlg::SetListCtrlStyle(CListCtrl& list)
{
	// 设置listctrl 风格及扩展风格
	LONG lStyle;
	lStyle = GetWindowLong(list.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(list.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	//dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	list.SetExtendedStyle(dwStyle); //设置扩展风格
}

void CSQLite3DemoDlg::OpenDatabase()
{
	UpdateData(TRUE);

	////--连接句柄最好不要多线程间共享，否则会锁定数据库--https://blog.csdn.net/wsmrcool/article/details/8287904///
	//测试一：
	//string ssTst = "update EventUpFailed set uid='dsqw'";
	//static sqlite3* m_db = nullptr; 
	//CW2M  sDB(m_strDBFile, CP_UTF8);
	//sqlite3_open(sDB, &m_db);

	//int ret = sqlite3_exec(m_db, ssTst.data(), NULL, NULL, NULL);
	//return ;

	//测试二：
// 	static CComPtr<IXDBConnection> dbCon, dbcon2;
// 	CreateXDBInstance(&dbCon);
// 	dbCon->Open(m_strDBFile);
// 	
// 	CreateXDBInstance(&dbcon2);
// 	dbcon2->Open(m_strDBFile);
// 	CString sss;
// 	sss.Format(_T("update db set TaskColumn='dsqw' where id='EE9FBFA7-7CC4-4866-8F54-8198692ED186'"));
// 	dbcon2->Excute(sss);
// 	return;

	if(FALSE == m_sqlite3DB.OpenDatabase(m_strDBFile))
	{
		AfxMessageBox(_T("OpenDatabase Failed!"));
		return;
	}

	// 创建表
	m_dataLogAccessor.SetSqlite3DB(m_sqlite3DB.GetSqlite3DB());
	m_dataLogAccessor.CreateTable();

	InitDbListDatas();
}


void CSQLite3DemoDlg::InitDbListData(STableDataLog& sDbData)
{
	int nIndex = 0;
	int nRow = m_listDbData.InsertItem(nIndex++, sDbData.strID);
	m_listDbData.SetItemText(nRow, nIndex++, sDbData.strFileName);
	m_listDbData.SetItemText(nRow, nIndex++, sDbData.strFileSize);
	m_listDbData.SetItemText(nRow, nIndex++, sDbData.strFileExt);
	m_listDbData.SetItemText(nRow, nIndex++, sDbData.strStartTime);
	m_listDbData.SetItemText(nRow, nIndex++, sDbData.strEndTime);
	m_listDbData.SetItemText(nRow, nIndex++, sDbData.strKillStatus);
	m_listDbData.SetItemText(nRow, nIndex++, sDbData.strTask);
}

void CSQLite3DemoDlg::InitDbListDatas()
{
	ASTableDataLogArray asTableDb;
	m_dataLogAccessor.GetAllDatas(asTableDb);

	m_listDbData.DeleteAllItems();
	for(int i=0; i<asTableDb.GetSize(); i++)
	{
		STableDataLog sDbData = asTableDb.GetAt(i);
		InitDbListData(sDbData);
	}
}


///////////////////////////////////////////////////////////////
//	函 数 名 : GetModuleDirectory
//	所属类名 : Global
//	函数功能 : 获取指定Module所在目录
//	处理过程 : 
//	备    注 : 返回结果以“\”结尾
//	作    者 : hesl
//	时    间 : 2008年7月22日
//	返 回 值 : CString——指定Module所在目录
//	参数说明 : HINSTANCE hInstCurModule——[in]指定模块句柄
///////////////////////////////////////////////////////////////
CString CSQLite3DemoDlg::GetModuleDirectory(HINSTANCE hInstCurModule)
{
	TCHAR chrModuleFileName[MAX_PATH];
	memset(chrModuleFileName, 0, sizeof(chrModuleFileName));
	GetModuleFileName(hInstCurModule, chrModuleFileName, MAX_PATH);

	CString strCurFullPath = chrModuleFileName;
	int nPos = strCurFullPath.ReverseFind('\\');
	if(nPos == -1)
	{
		return _T("C:\\");	
	}

	return strCurFullPath.Left(nPos + 1); 
}

///////////////////////////////////////////////////////////////
//	函 数 名 : GetFileSize
//	所属类名 : Global
//	函数功能 : 获取指定文件大小
//	处理过程 : 
//	备    注 : 
//	作    者 : hesl
//	时    间 : 2008年7月22日
//	返 回 值 : LONGLONG——指定文件大小
//	参数说明 : CString strFileFullPath——[in]指定文件全路径
///////////////////////////////////////////////////////////////
LONGLONG CSQLite3DemoDlg::GetFileSize(CString strFileFullPath)
{	
	LONGLONG llFileSize = 0;

	WIN32_FIND_DATA FileData;
	HANDLE hFind = FindFirstFile(strFileFullPath, &FileData);
	if(hFind != INVALID_HANDLE_VALUE)
	{
		llFileSize = ((LONGLONG)FileData.nFileSizeLow) | ((LONGLONG)FileData.nFileSizeHigh << 32);
		FindClose(hFind);
	}

	return llFileSize;
}

CString CSQLite3DemoDlg::GetFileSizeString(UINT64 ullFileSize)
{
	CString csFileSize;

	double lFileSize = (double)ullFileSize;
	if (ullFileSize < 1024)
	{
		csFileSize.Format(_T("%I64u B"), ullFileSize);
	}
	else if (ullFileSize < 1024 * 1024)
	{
		lFileSize /= 1024;
		csFileSize.Format(_T("%.0f KB"), lFileSize);
	}
	else if (ullFileSize < 1024 * 1024 * 1024)
	{
		lFileSize /= 1024 * 1024;
		csFileSize.Format(_T("%.2f MB"), lFileSize);
	}
	else
	{
		lFileSize /= 1024 * 1024 * 1024;
		csFileSize.Format(_T("%.2f GB"), lFileSize);
	}

	return csFileSize;
}

void CSQLite3DemoDlg::OnBtnBrowseFile()
{
	CFileDialog dlg(TRUE, _T("All Files (*.*)"), NULL, OFN_READONLY | OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"), NULL);
	if(IDOK == dlg.DoModal())
	{
		m_strFileName = dlg.GetPathName();
		m_strFileExt = dlg.GetFileExt();
		m_strFileSize = GetFileSizeString(GetFileSize(m_strFileName));
		UpdateData(FALSE);
	}
}
