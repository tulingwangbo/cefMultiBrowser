// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"

// Implement application-level callbacks for the browser process.
class SimpleApp : public CefApp,
				  public CefRenderProcessHandler,
                  public CefBrowserProcessHandler {
 public:
  SimpleApp();

  // CefApp methods:
  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
      OVERRIDE { return this; }

  // CefBrowserProcessHandler methods:
  virtual void OnContextInitialized() OVERRIDE;

  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
	  CefRefPtr<CefFrame> frame,
	  CefRefPtr<CefV8Context> context) 
  {
	  //CefRefPtr<CefV8Value> object = context->GetGlobal();
	  ////for (auto iter = function_name_.begin(); iter != function_name_.end(); ++iter)
	  ////{
	  ////	//Create the "NativeLogin" function.
	  ////	CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction((*iter), extension_handler_);
	  ////	//Add the "NativeLogin" function to the "window" object.
	  ////	object->SetValue((*iter), func, V8_PROPERTY_ATTRIBUTE_NONE);
	  ////}
	  //CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("GetResult", browser_client_);
	  ////	//Add the "NativeLogin" function to the "window" object.
	  //object->SetValue("GetResult", func, V8_PROPERTY_ATTRIBUTE_NONE);

	  //std::fstream f("d:\\log.txt");
	  //f.write("asdasdwewqexasdzxcewqesdasd", sizeof("asdasdwewqexasdzxcewqesdasd"));
	 
  }

 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleApp);
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
