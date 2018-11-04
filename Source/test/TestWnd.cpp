#include "TestWnd.h"

#include "..\CefUIWrapper\CefUIWrapperHeads.h"
using namespace ns_CefUI;
//
//CTestWnd::CTestWnd(void) : browser_(NULL), browser_pan_(NULL){
//
//}
//
//
//CTestWnd::~CTestWnd(void) {
//
//}
//
//LPCTSTR CTestWnd::GetWindowClassName() const {
//	return L"TestWndClass";
//}
//
//CDuiString CTestWnd::GetSkinFolder() {
//	return L"skin";
//}
//
//CDuiString CTestWnd::GetSkinFile() {
//	return L"TestWnd.xml";
//}
//
//UILIB_RESTYPE CTestWnd::GetResourceType() const {
//	return UILIB_FILE;
//}
//
//void CTestWnd::InitWindow() {
//	browser_ = static_cast<CCefBrowserUI*>(m_pm.FindControl(_T("TestBrowser")));
//	browser_pan_ = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("BrowserPan")));
//}
//
//CCefBrowserUI *g_pWeb = NULL;
//void CTestWnd::Notify(TNotifyUI& msg) {
//	CDuiString sName = msg.pSender->GetName();
//
//	if (msg.sType == L"click")
//	{
//		if (sName == L"settting_btn")
//		{
//			if (g_pWeb == nullptr)
//			{
//				g_pWeb = new CCefBrowserUI(m_hWnd, nullptr, this);
//				g_pWeb->SetAttribute(L"name", L"TestBrowser");
//				browser_pan_->Add(g_pWeb);
//				g_pWeb->LoadURL(L"http://www.qq.com");
//			}
//			else
//			{
//				browser_pan_->SetDelayedDestroy(false);
//				g_pWeb->CloseCefWindow();
//				browser_pan_->Remove(g_pWeb);
//				g_pWeb = nullptr;
//			}
//
//		}
//	}
//	
//	__super::Notify(msg);
//}
//
//LRESULT CTestWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) {
//	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
//}
//
//void CTestWnd::Show() {
//	Create(GetHWND(), _T("TestWnd"), UI_WNDSTYLE_FRAME&~WS_THICKFRAME&~WS_MAXIMIZEBOX, NULL);
//	CenterWindow();
//	ShowModal();
//	SetFocus(m_hWnd);
//}
//
//CControlUI* CTestWnd::CreateControl(LPCTSTR pstrClass) {
//	//创建Cef控件类******************************************************************************
//	if (_tcsicmp(pstrClass, CCefBrowserUIItemInterface) == 0) {
//		return  new CCefBrowserUI(m_hWnd, this);
//	}
//	return NULL;  
//}
//
//
//void CTestWnd::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
//	//可以在这里动态加载URL
//	CDuiRect rect = browser_pan_->GetPos();
//	if (g_pWeb)
//	{
//		browser_->SetVisible(false);
//		g_pWeb->SetPos(rect);
//		g_pWeb->LoadURL(L"http://www.qq.com");
//	}
//	else
//		browser_->SetPos(rect);
//}
//
//void CTestWnd::DoClose(CefRefPtr<CefBrowser> browser) {
//	
//}
//
////加载出错
//void CTestWnd::OnLoadError(CefRefPtr<CefBrowser> browser,
//	CefRefPtr<CefFrame> frame,
//	CefLoadHandler::ErrorCode errorCode,
//	const CefString& errorText,
//	const CefString& failedUrl) {
//
//}
//
////加载状态改变
//void CTestWnd::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
//	bool isLoading,
//	bool canGoBack,
//	bool canGoForward) {
//
//}
//
////加载开始
//void CTestWnd::OnLoadStart(CefRefPtr<CefBrowser> browser,
//	CefRefPtr<CefFrame> frame) {
//
//}
//
////加载完成
//void CTestWnd::OnLoadEnd(CefRefPtr<CefBrowser> browser,
//	CefRefPtr<CefFrame> frame,
//	int httpStatusCode) {
//		CDuiRect rect = browser_pan_->GetPos();
//		browser_->SetPos(rect);
//}
//
//int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR command_line, int) {
//	CCefBrowserUI::InitCef(instance);
//	CTestWnd wnd;
//	wnd.Show();
//	CCefBrowserUI::Quit();
//}
