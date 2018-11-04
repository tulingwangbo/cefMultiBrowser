#pragma once
#include "ClientApp.h"
#include <string>
using namespace std;

namespace ns_CefUI{

// Implement application-level callbacks for the browser process.
class CCefClientBrowserApp : public ClientApp, public CefBrowserProcessHandler
{
public:
	CCefClientBrowserApp();
	~CCefClientBrowserApp();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()	OVERRIDE{ return this; }

	virtual void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line);

	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized
		() OVERRIDE;

private:

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCefClientBrowserApp);
	DISALLOW_COPY_AND_ASSIGN(CCefClientBrowserApp);

};



class CCefClientRendererApp : public ClientApp, public CefRenderProcessHandler
{
public:
	CCefClientRendererApp(std::vector<wstring> &vecV8Function);

	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() 	OVERRIDE{ return this; }


	//CefRenderProcessHandler methods
	virtual void OnWebKitInitialized() OVERRIDE;
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:
	std::vector<wstring> m_vecwstrFuncs;//º¯ÊýÁÐ±í

private:
	IMPLEMENT_REFCOUNTING(CCefClientRendererApp);
	DISALLOW_COPY_AND_ASSIGN(CCefClientRendererApp);
};

}
