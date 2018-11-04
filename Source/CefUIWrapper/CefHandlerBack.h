#pragma once

#include "include/cef_client.h"

#include "include/cef_app.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_request_handler.h"
#include "include/cef_context_menu_handler.h"
#include "include/cef_parser.h"
#include <vector>
#include <map>
#include <functional>


namespace ns_CefUI{

typedef std::function<CefRefPtr<CefV8Value>(CefV8ValueList)> CustomFunction;

typedef std::function<bool(CefString, CefRefPtr<CefListValue>)> CallBackJsCall;


class COtherHandleSlot :
	public CefContextMenuHandler,
	public CefRequestHandler,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefDownloadHandler,
	public CefDragHandler
{
public:
	COtherHandleSlot() {};
	~COtherHandleSlot() {};

};

}