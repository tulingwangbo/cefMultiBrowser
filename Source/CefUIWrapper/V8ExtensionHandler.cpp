#include "V8ExtensionHandler.h"

namespace ns_CefUI
{

	CV8ExtensionHandler::CV8ExtensionHandler() {

	}

	CV8ExtensionHandler::~CV8ExtensionHandler() {
		function_map_.clear();

	}

	bool CV8ExtensionHandler::Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception) {

		// 创建消息对象
		auto msg = CefProcessMessage::Create(name);

		// 构造参数
		auto args = msg->GetArgumentList();
		args->SetSize(arguments.size());
		for (int i=0; i<arguments.size(); i++)
		{
			args->SetString(i, arguments[i]->GetStringValue());
		}

		// 发送给浏览器进程
		auto browser = CefV8Context::GetCurrentContext()->GetBrowser();
		browser->SendProcessMessage(PID_BROWSER, msg);
		retval = CefV8Value::CreateString("TRUE");

		return true;
	}

	void CV8ExtensionHandler::SetFunction(const CefString &name, CustomFunction function) {
		function_map_[name] = function;

	}

}
