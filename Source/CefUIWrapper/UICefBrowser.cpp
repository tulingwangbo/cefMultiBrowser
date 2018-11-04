

#include "BrowserClient.h"
#include "CefUIWrapper.h"
#include "..\DuiLib\UIlib.h"
#include "UICefBrowser.h"

using namespace DuiLib;
#ifdef _DEBUG
#     pragma comment(lib, "DuiLib_d.lib")
#else
#     pragma comment(lib, "DuiLib.lib")
#endif

namespace ns_CefUI {

	std::vector<wstring> CCefBrowserUI::m_vecV8Function;

 CCefBrowserUI::CCefBrowserUI(HWND parent,
 	CallBackJsCall func,
 	COtherHandleSlot* set_handle,
 	wstring _url)
 {
 	m_jscallfunc = func;
 	parent_hwnd_ = parent;
 	set_handle_ = set_handle;
 	is_created_ = false;
 	url_ = _url;
 }

CCefBrowserUI::~CCefBrowserUI(void) {
	CloseCefWindow();
}

LPCTSTR CCefBrowserUI::GetClass() const {
	return CCefBrowserUIClassName;

}

LPVOID CCefBrowserUI::GetInterface(LPCTSTR pstrName) {
	if( _tcscmp(pstrName, CCefBrowserUIItemInterface) == 0 ) 
		return static_cast<CCefBrowserUI*>(this);  
	return CControlUI::GetInterface(pstrName);  
}

void CCefBrowserUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	if ( _tcscmp(pstrName, _T("name")) == 0)
		browser_name_ = pstrValue;
	else if( _tcscmp(pstrName, _T("url")) == 0 && url_.empty()) 
		url_ = pstrValue;  
	CControlUI::SetAttribute(pstrName, pstrValue);  
}

void CCefBrowserUI::DoInit() {
	if (is_created_)
	{
		OutputDebugString(L"CCefBrowserUI::DoInit() is_created_ = true");
		return;
	}

	if (set_handle_) {
		auto other_handle = std::unique_ptr<COtherHandleSlot>(set_handle_);
		CreateCefWindow(parent_hwnd_, url_,  std::move(other_handle));
	} else {
		CreateCefWindow(parent_hwnd_, url_);
	}	

	is_created_ = true;
}


bool CCefBrowserUI::CreateCefWindow(HWND hWnd, CefString url,std::unique_ptr<COtherHandleSlot> other_handle) 
{
	
	//创建子窗口
	CefWindowInfo info;
	RECT rect = { 0, 0, 1, 1 };
	info.SetAsChild(hWnd, rect);
	
	m_handler = new CBrowserClient(std::move(other_handle));
	m_handler->SetCallBackJsCallFun(m_jscallfunc);

	CefBrowserSettings b_settings;
	CefBrowserHost::CreateBrowser(info, m_handler, url, b_settings, NULL);

	return true;
}

void CCefBrowserUI::CloseCefWindow() 
{
	if (!is_created_) {
		return;
	}
	is_created_ = false;

	if (GetBrowser()) {
		GetBrowser()->GetHost()->CloseBrowser(true);
	}
}

void CCefBrowserUI::SetInternVisible(bool bVisible) {
	CControlUI::SetInternVisible(bVisible);
	if (is_created_) {
		CefWindowHandle hwnd = GetBrowserHwnd();
		if( hwnd != NULL) 
			::ShowWindow(hwnd, IsVisible() ? SW_SHOW : SW_HIDE);

	}
}

void CCefBrowserUI::SetPos(RECT rc, bool bNeedInvalidate)
{
	CControlUI::SetPos(rc, bNeedInvalidate);
	if (!is_created_) {
		return;
	}
	CefWindowHandle hwnd = GetBrowserHwnd();
	if (hwnd) {
		::MoveWindow(hwnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);
	}
}

void CCefBrowserUI::SetVisible(bool bVisible) {
	CControlUI::SetVisible(bVisible);
	if (is_created_) {
		CefWindowHandle hwnd = GetBrowserHwnd();
		if( hwnd != NULL) 
			::ShowWindow(hwnd, IsVisible() ? SW_SHOW : SW_HIDE);
	}
}

bool CCefBrowserUI::InitCef(HINSTANCE hInstance, CefString cache_path, CefString log_file) {
	//高DPI支持
	CefEnableHighDPISupport();

	void* sandbox_info = NULL;
	CefMainArgs main_args(hInstance);

	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());

	if (!PathIsDirectory(cache_path.ToWString().c_str())) {
		SHCreateDirectory(NULL, cache_path.ToWString().c_str());

	}
	if (!PathIsDirectory(log_file.ToWString().c_str())) {
		SHCreateDirectory(NULL, log_file.ToWString().c_str());

	}

	//不同进程创建不同的app
	CefRefPtr<CefApp> app;
	ProcessType process_type = ClientApp::GetProcessType(command_line);
	if (process_type == BrowserProcess) {
		app = new CCefClientBrowserApp();
	}
	else if(process_type == RendererProcess){
		app = new CCefClientRendererApp(m_vecV8Function);
	}

	CefSettings settings;
	CefSettingsTraits::init(&settings);

	settings.no_sandbox = true;
	CefString(&settings.browser_subprocess_path).FromWString(L"Render.exe");
	//settings.single_process = true;
	settings.multi_threaded_message_loop = true;
	settings.log_severity = LOGSEVERITY_DISABLE;
	CefString(&settings.locale).FromWString(L"zh-CN");
	//缓存数据路径
	CefString(&settings.cache_path).FromWString(cache_path);
	//debug日志路径
	//CefString(&settings.log_file).FromWString(log_file);

	//初始化cef
	CefInitialize(main_args, settings, app, sandbox_info);
	CefSettingsTraits::clear(&settings);

	return true;

}

void CCefBrowserUI::Quit() {
	CefShutdown();
}

void CCefBrowserUI::SetFunction(const CefString &name, CustomFunction function) {
	if (!is_created_) {
		return;
	}
	m_handler->SetFunction(name, function);

}

void CCefBrowserUI::StartCefMessageLoop() {
	CefRunMessageLoop();
}


void CCefBrowserUI::LoadURL(std::wstring url) {
	if (GetBrowser())
	{
		GetBrowser()->GetMainFrame()->LoadURL(url);
	}
}

bool CCefBrowserUI::CanGoForward() {
	if (!is_created_) {
		return false;
	}
	return GetBrowser()->CanGoForward();
}

void CCefBrowserUI::GoForward() {
	if (!is_created_) {
		return;
	}
	
	GetBrowser()->CanGoForward();
}

void CCefBrowserUI::Reload() {
	if (!is_created_) {
		return;
	}
	GetBrowser()->Reload();
}

void CCefBrowserUI::ReloadIgnoreCache() {
	if (!is_created_) {
		return;
	}
	GetBrowser()->ReloadIgnoreCache();
}

void CCefBrowserUI::StopLoad() {
	if (!is_created_) {
		return;
	}
	GetBrowser()->StopLoad();
}

bool CCefBrowserUI::CanGoBack() {
	if (!is_created_) {
		return false;
	}
	return GetBrowser()->CanGoBack();

}

void CCefBrowserUI::GoBack() {
	if (!is_created_) {
		return;
	}
	GetBrowser()->GoBack();

}

void CCefBrowserUI::SetZoomLevel(double zoomLevel) {
	if (!is_created_) {
		return;
	}
	GetBrowser()->GetHost()->SetZoomLevel(zoomLevel);

}

CefString CCefBrowserUI::GetURL() {
	if (!is_created_) {
		return "";
	}
	return GetBrowser()->GetMainFrame()->GetURL();
}

void CCefBrowserUI::ExectueJS(const CefString &jscode) {
	if (!is_created_) {
		return;
	}
	
	GetBrowser()->GetMainFrame()->ExecuteJavaScript(jscode,
		GetBrowser()->GetMainFrame()->GetURL(), 0);

}

void CCefBrowserUI::ResizeBrowser(DuiLib::CDuiRect rect) {
	if (!is_created_) {
		return;
	}
	CefWindowHandle hwnd = GetBrowserHwnd();
	if (hwnd) {
		//::UpdateWindow(parent_hwnd_);
		::MoveWindow(hwnd, rect.left, rect.top, rect.GetWidth(), rect.GetHeight(), TRUE);
	}
}

bool CCefBrowserUI::SwitchUser()
{
	if (!is_created_) {
		return false;
	}

	CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(NULL);
	if (manager)
	{
		return manager->DeleteCookies("", "", NULL);
	}
	else
	{
		return false;
	}

	Sleep(100);

	LoadURL(url_);

	return true;
}

CefRefPtr<CefBrowser>  CCefBrowserUI::GetBrowser()
{
	if (m_handler)
	{
		return m_handler->browser_;
	}

	return nullptr;
}

CefWindowHandle CCefBrowserUI::GetBrowserHwnd()
{
	if (GetBrowser())
	{
		return GetBrowser()->GetHost()->GetWindowHandle();
	}

	return nullptr;
}

}