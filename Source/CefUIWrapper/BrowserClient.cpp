#include "BrowserClient.h"

#include <string>
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/cef_browser.h"
#include "include/base/cef_lock.h"
#include "include/wrapper/cef_helpers.h"
#include <shlobj.h>


namespace ns_CefUI
{
	enum ContextMenuID {
		RIGHT_MENU_REFRESH = 1001,
		RIGHT_MENU_FORWARD,
		RIGHT_MENU_BACKOFF,
		RIGHT_MENU_SHOW_DEVTOOLS,
		RIGHT_MENU_HIDE_DEVTOOLS,
		RIGHT_MENU_SAVE_IMAGE,//右键菜单-图片另存为
	};

	CBrowserClient::CBrowserClient( std::unique_ptr<COtherHandleSlot> other_handle) :
	    is_closing_(false), 
		other_handle_(std::move(other_handle)),
		is_created_(false),
		browser_(NULL) {
		extension_ = new CV8ExtensionHandler();
		m_jscallfunc = nullptr;

		// 以下支持的协议内容，是从E:\Src\Chromium\src\src\net\url_request\url_request_job_manager.cc文件中抄取过来的
		m_ProtocalSet.insert(L"http");
		m_ProtocalSet.insert(L"https");
		m_ProtocalSet.insert(L"file");
		m_ProtocalSet.insert(L"ftp");
		m_ProtocalSet.insert(L"about");
		m_ProtocalSet.insert(L"data");
		m_ProtocalSet.insert(L"chrome");
		m_ProtocalSet.insert(L"chrome-devtools");
	}

	CBrowserClient::~CBrowserClient(void) {
		other_handle_->Release();
	}

	CefRefPtr<CefRequestHandler> CBrowserClient::GetRequestHandler() {
		return this;
	}

	CefRefPtr<CefContextMenuHandler> CBrowserClient::GetContextMenuHandler() {
		return this;
	}

	CefRefPtr<CefDisplayHandler> CBrowserClient::GetDisplayHandler() {
		return this;
	}

	CefRefPtr<CefLifeSpanHandler> CBrowserClient::GetLifeSpanHandler() {
		return this;
	}

	CefRefPtr<CefLoadHandler> CBrowserClient::GetLoadHandler() {
		return this;
	}
	CefRefPtr<CefDownloadHandler> CBrowserClient::GetDownloadHandler(){
		return this;
	}
	CefRefPtr<CefDragHandler> CBrowserClient::GetDragHandler(){
		return this;
	}

	cef_return_value_t CBrowserClient::OnBeforeResourceLoad(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefRequestCallback> callback) {
			CEF_REQUIRE_IO_THREAD();
			
			std::wstring strUrl = request->GetURL().ToWString(); 
			//size_t pos = strUrl.find(L"://");
			//if(pos == std::wstring::npos)
			//	return RV_CANCEL;

			//std::wstring strScheme = strUrl.substr(0, pos);
			//std::transform(strScheme.begin(), strScheme.end(), strScheme.begin(), towlower);
			//if(m_ProtocalSet.find(strScheme) == m_ProtocalSet.end())
			//	return RV_CANCEL;

			if (other_handle_) {
				return other_handle_->OnBeforeResourceLoad(browser, frame, request, callback);
			}
			return RV_CONTINUE;
	}

	bool CBrowserClient::OnBeforePopup(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		const CefString& target_frame_name,
		CefLifeSpanHandler::WindowOpenDisposition target_disposition,
		bool user_gesture,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings,
		bool* no_javascript_access) {
			wstring temp = target_url.ToWString();
			temp = L"OnBeforePopup" + temp;
			OutputDebugString(temp.c_str());

			if (other_handle_)
			{
				return	other_handle_->OnBeforePopup(browser, frame, target_url, target_frame_name,
						target_disposition,user_gesture,popupFeatures,	 windowInfo, client,
						settings,no_javascript_access);
			}

			ShellExecute(NULL, L"open", target_url.ToWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
			return true;
	}

	void CBrowserClient::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) {
		if ((params->GetTypeFlags() & CM_TYPEFLAG_MEDIA) != 0)//右键点击的是图片资源
		{
			model->Clear();
			model->AddItem(RIGHT_MENU_SAVE_IMAGE, L"图片另存为");
		}
		if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0 && GetKeyState(VK_MENU)< 0 && GetKeyState(VK_SHIFT)< 0) {
			// Add a separator if the menu already has items.
			if (model->GetCount() > 0) {
				model->AddSeparator();
				model->AddItem(RIGHT_MENU_SHOW_DEVTOOLS, L"打开调试");
				model->AddItem(RIGHT_MENU_HIDE_DEVTOOLS, L"关闭调试");
				model->AddItem(RIGHT_MENU_REFRESH, L"刷新");
			}
		}

	}

	bool CBrowserClient::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		EventFlags event_flags) {
			switch (command_id) {
			case RIGHT_MENU_REFRESH:
				browser_->Reload();
				return true;
			case RIGHT_MENU_FORWARD:
				if(browser_->CanGoForward()) {
					browser_->GoForward();
				}
				return true;
			case RIGHT_MENU_BACKOFF:
				if(browser_->CanGoBack()) {
					browser_->GoBack();
				}
				return true;
			case RIGHT_MENU_SHOW_DEVTOOLS:
				{
					//菜单ID匹配时，弹出DevTools
					CefWindowInfo   wi;
					CefBrowserSettings settings;
					CefPoint    cp;
					cp.x = 100;
					cp.y = 100;
					wi.SetAsPopup( browser->GetHost()->GetWindowHandle(), CefString("DevTools"));
					browser->GetHost()->ShowDevTools(wi,browser->GetHost()->GetClient(), settings, cp);
					return true;
				}
			case RIGHT_MENU_HIDE_DEVTOOLS:
				{
					browser->GetHost()->CloseDevTools();
					return true;
				}
			case RIGHT_MENU_SAVE_IMAGE:
				{
					CefString image_url = params->GetSourceUrl();
					if (!image_url.empty())
					{
						browser->GetHost()->StartDownload(image_url);
					}
					return true;
				}
			}
			return false;
	}

	void CBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
		CEF_REQUIRE_UI_THREAD();
		base::AutoLock lock_scope(lock_);

		if (!browser_) {
			browser_ = browser;
			m_BrowserId = browser->GetIdentifier();
			is_created_ = true;
		}

		if (other_handle_) {
			other_handle_->OnAfterCreated(browser);
		}
	}

	bool CBrowserClient::DoClose(CefRefPtr<CefBrowser> browser) {
		CEF_REQUIRE_UI_THREAD();

		base::AutoLock lock_scope(lock_);

	
		if (other_handle_) {
			other_handle_->DoClose(browser);
		}

		if (browser->GetIdentifier() == m_BrowserId)
		{
			is_closing_ = true;
		}
	
		return false;		
	}

	void CBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();

		base::AutoLock lock_scope(lock_);

		if (other_handle_)
		{
			other_handle_->OnBeforeClose(browser);
		}

		if (browser->GetIdentifier() == m_BrowserId)
			browser_ = NULL;
	}

	void CBrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) {
		if (other_handle_) {
			other_handle_->OnLoadError(browser, frame, errorCode, errorText, failedUrl);

		}
	}

	//加载状态改变
	void CBrowserClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward) {
		if (other_handle_) {
			other_handle_->OnLoadingStateChange(browser, isLoading, canGoBack, canGoForward);
		}

		//暂时去除，因为宽高较小时网页排版乱 delete by hewenkai 20180817
		//if (!isLoading)
		//{
		//	// Win7:
		//	// text size Larger 150% => ppix/ppiy 144
		//	// text size Medium 125% => ppix/ppiy 120
		//	// text size Smaller 100% => ppix/ppiy 96
		//	double oldZoomLevel = browser_->GetHost()->GetZoomLevel();
		//	HWND cefHandle = browser_->GetHost()->GetWindowHandle();
		//	HDC hdc = GetDC(cefHandle);
		//	int ppix = GetDeviceCaps(hdc, LOGPIXELSX);
		//	int ppiy = GetDeviceCaps(hdc, LOGPIXELSY);
		//	ReleaseDC(cefHandle, hdc);

		//	//解决系统125%缩放时网页仍然是100%显示的问题，150%时网页能跟随缩放
		//	if (ppix == 120)
		//	{
		//		browser_->GetHost()->SetZoomLevel(1.25);
		//		if (browser_->GetHost()->GetZoomLevel() != oldZoomLevel) {
		//			OutputDebugString(L"DPI 125% SetZoomLevel(1.25) success");
		//		}
		//		else
		//		{
		//			OutputDebugString(L"DPI 125% SetZoomLevel(1.25) failed");
		//		}
		//		return;
		//	}
		//}
	}

	//加载开始
	void CBrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		TransitionType transition_type) {
		if (other_handle_) {
			other_handle_->OnLoadStart(browser, frame, transition_type);

		}
	}

	//加载完成
	void CBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode) {
		if (other_handle_) {
			other_handle_->OnLoadEnd(browser, frame, httpStatusCode);

		}
	}

	bool CBrowserClient::IsClosing() const {
		return is_closing_;
	}

	bool CBrowserClient::IsCreated() const {
		return is_created_;

	}


	//地址改变
	void CBrowserClient::OnAddressChange(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& url) {
		if (other_handle_) {

			other_handle_->OnAddressChange(browser, frame, url);
		}	
	}

	//标题改变
	void CBrowserClient::OnTitleChange(CefRefPtr<CefBrowser> browser,
		const CefString& title) {
		CEF_REQUIRE_UI_THREAD();
		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
		SetWindowText(hwnd, title.ToWString().c_str());

		if (other_handle_) {
			other_handle_->OnTitleChange(browser, title);
		}
	}

	//页面图标改变
	void CBrowserClient::OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
		const std::vector<CefString>& icon_urls) {
		if (other_handle_) {
			other_handle_->OnFaviconURLChange(browser, icon_urls);

		}
	}

	//页面全屏时调用
	void CBrowserClient::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
		bool fullscreen) {
		if (other_handle_) {
			other_handle_->OnFullscreenModeChange(browser, fullscreen);
		}
	}

	//浏览器显示工具提示
	bool CBrowserClient::OnTooltip(CefRefPtr<CefBrowser> browser,
		CefString& text) {
		if (other_handle_) {
			return other_handle_->OnTooltip(browser, text);

		}
		return true;
	}

	//接收到状态信息时
	void CBrowserClient::OnStatusMessage(CefRefPtr<CefBrowser> browser,
		const CefString& value) {
		if (other_handle_) {
			other_handle_->OnStatusMessage(browser, value);

		}
	}

	//调用显示控制台信息
	bool CBrowserClient::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
		/*cef_log_severity_t level,*/
		const CefString& message,
		const CefString& source,
		int line) {
		cef_log_severity_t level = LOGSEVERITY_DEFAULT;

		if (other_handle_) {
			return other_handle_->OnConsoleMessage(browser, /*level,*/ message, source, line);
		}
		return true;
	}

	bool CBrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) {

		CefString messageName = message->GetName();
		CefRefPtr<CefListValue> args = message->GetArgumentList();
		if (m_jscallfunc != NULL)
		{
			m_jscallfunc(messageName, args);
			return true;
		}

		return false;
	}

	void CBrowserClient::OnBeforeDownload(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		const CefString& suggested_name,
		CefRefPtr<CefBeforeDownloadCallback> callback)
	{
		CEF_REQUIRE_UI_THREAD();
		// Continue the download and show the "Save As" dialog.
		TCHAR szFolderPath[MAX_PATH];
		wstring path = suggested_name.ToWString();

		// Save the file in the user's "My Documents" folder.
		if (SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL,
			0, szFolderPath) == S_OK) {
				path = szFolderPath;
				path += L"\\" + suggested_name.ToWString();
		}

		callback->Continue(path, true);
	}

	void CBrowserClient::OnDownloadUpdated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		CefRefPtr<CefDownloadItemCallback> callback)
	{
		CEF_REQUIRE_UI_THREAD();
		if (download_item->IsComplete())
		{
			wstring downloadlog = L"File \"" + download_item->GetFullPath().ToWString() + L"\" downloaded successfully.";
			OutputDebugString(downloadlog.c_str());
		}
	}

	bool CBrowserClient::OnDragEnter(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDragData> dragData,
		DragOperationsMask mask)
	{
		return true;//屏蔽文件拖动到网页
	}

	void CBrowserClient::OnDraggableRegionsChanged(
		CefRefPtr<CefBrowser> browser,
		const std::vector<CefDraggableRegion>& regions)
	{
	}

	void CBrowserClient::SetFunction(const CefString &name, CustomFunction function) {
		extension_->SetFunction(name, function);

	}

	bool CBrowserClient::SetCallBackJsCallFun(CallBackJsCall func)
	{
		m_jscallfunc = func;

		return true;
	}
}

