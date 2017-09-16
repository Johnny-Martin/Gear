// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <stack>
#include <map>
#include <regex>

#include <d2d1.h>
#include <dwrite.h>
#include "spdlog.h"
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include "Log.h"
#include "Util.h"

#define RGBA(r,g,b,a)  ((COLORREF) (((DWORD)(BYTE)(a))<<24 | (RGB(r,g,b))))

#define WARNING_HWND_MSG(hWnd, WarningStr) \
	::MessageBox(hWnd, WarningStr, _T("Error"), MB_OK | MB_ICONWARNING);


#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>

extern CAppModule _Module;
// TODO: reference additional headers your program requires here
