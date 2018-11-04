// ÒÀÀµlibCefºÍduiLib·â×°µÄCefUI
// wb 2018-11-03

#ifdef CEFUIWRAPPER_EXPORTS
#define CEFUIWRAPPER_API __declspec(dllexport)
#else
#define CEFUIWRAPPER_API __declspec(dllimport)
#endif
