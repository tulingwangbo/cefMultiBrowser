#include "RenderAndBrowserApp.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "V8ExtensionHandler.h"

static wstring s_Funcs[] = {L"WindowsConfig", L"HandleMessage", L"MessageRead", L"UnReadMessage", 
	L"SendState", L"OtherPlaceLogin", L"ReLoginState", L"ChangeUser", L"QuitState", 
	L"WindowsInterface"};//2.0新加通用接口，第一项参数传函数名称，后续参数数量和含义根据函数来定

namespace ns_CefUI {

//浏览器进程
CCefClientBrowserApp::CCefClientBrowserApp() 
{
		
}

CCefClientBrowserApp::~CCefClientBrowserApp(void) 
{
		
}

void CCefClientBrowserApp::OnContextInitialized()
{
	CEF_REQUIRE_UI_THREAD();
}

void CCefClientBrowserApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line)
{
	//主进程
	if (process_type.empty()) {
		command_line->AppendSwitch("disable-gpu");
		//command_line->AppendSwitchWithValue("--force-device-scale-factor", "1");

		command_line->AppendSwitchWithValue("--renderer-process-limit", "8");
	}
}


//渲染进程
CCefClientRendererApp::CCefClientRendererApp(std::vector<wstring> &vecV8Function)
{
	m_vecwstrFuncs.swap(vecV8Function);
}

void CCefClientRendererApp::OnWebKitInitialized()
{

}

void CCefClientRendererApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	CefRefPtr<CefV8Value> object = context->GetGlobal();

	CefRefPtr<CV8ExtensionHandler> m_myV8Handler = new CV8ExtensionHandler();
	for (int i = 0; i < m_vecwstrFuncs.size(); i++)
	{
		CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(m_vecwstrFuncs[i], m_myV8Handler);
		object->SetValue(m_vecwstrFuncs[i], func, V8_PROPERTY_ATTRIBUTE_NONE);
	}
}

void CCefClientRendererApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{

}

bool CCefClientRendererApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	return true;
}

}

