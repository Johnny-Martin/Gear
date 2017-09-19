// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

///////标准库////////
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <stack>
#include <map>
#include <regex>

///////XML解析////////
#include "tinyxml2.h"

///////日志库////////
#include "spdlog.h"
#include "Log.h"

///////常用函数////////
#include "Util.h"


#define RGBA(r,g,b,a)  ((COLORREF) (((DWORD)(BYTE)(a))<<24 | (RGB(r,g,b))))

#define WARNING_HWND_MSG(hWnd, WarningStr) \
	::MessageBox(hWnd, WarningStr, _T("Error"), MB_OK | MB_ICONWARNING);

///////WTL///////
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlstr.h>
extern CAppModule _Module;


///////GDI+以及Direct2D////////
#define USE_D2D_RENDER_MODE
#ifdef USE_D2D_RENDER_MODE
	#include <d2d1.h>
	#include <d2d1helper.h>
	#include <dwrite.h>
	#include <wincodec.h>

	#pragma comment(lib, "d2d1.lib")
	#pragma comment(lib, "dwrite.lib")
	using namespace D2D1;
#else
	#include<gdiplus.h>
	using namespace Gdiplus;
	#pragma comment(lib, "GdiPlus.lib")
#endif // USE_D2D_RENDER_MODE
