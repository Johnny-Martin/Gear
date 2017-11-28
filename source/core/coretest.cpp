// Caller.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <windows.h>
#include <assert.h>

#include "resource/ResManager.h"
#include "entry/GearEnv.h"
#include "moduletest/ModuleTest.h"
//#include "..\luabridge\LuaBridge.h"
//#include "zlib.h"

//#define PNG_DEBUG 3
//#define PNG_VERSION_INFO_ONLY
#include "png.h"
#include "spdlog.h"
#include "tinyxml2.h"
#include "entry/XmlAgent.h"

using namespace std;
using namespace tinyxml2;
using namespace Gear::Res;

CAppModule _Module;

png_uint_32						m_pngWidth;
png_uint_32						m_pngHeight;
png_byte						m_colorType;
png_byte						m_bitDepth;
png_byte						m_colorChannels;
png_bytep*						m_rowPointers; //In fact, m_rowPointers is a two-dimensional array
png_structp						m_pngStructPtr;
png_infop						m_pngInfoPtr;
RESERROR ReadPngFile(const string& strFilePath)
{
	FILE *fp = fopen(strFilePath.c_str(), "rb");
	if (!fp)
		return RES_ERROR_FILE_NOT_FOUND;

#define CHECK_PNG_FILE_HEADER 4
	unsigned char header[CHECK_PNG_FILE_HEADER];    // 8 is the maximum size that can be checked
	fread(header, 1, CHECK_PNG_FILE_HEADER, fp);
	if (png_sig_cmp(header, 0, CHECK_PNG_FILE_HEADER))
		return RES_ERROR_ILLEGAL_FILE_TYPE;


	/* initialize stuff */
	m_pngStructPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!m_pngStructPtr)
		return RES_ERROR_PARSE_FILE_FALIED;

	m_pngInfoPtr = png_create_info_struct(m_pngStructPtr);
	if (!m_pngInfoPtr)
		return RES_ERROR_PARSE_FILE_FALIED;

	if (setjmp(png_jmpbuf(m_pngStructPtr)))
		return RES_ERROR_PARSE_FILE_FALIED;

	png_init_io(m_pngStructPtr, fp);
	png_set_sig_bytes(m_pngStructPtr, CHECK_PNG_FILE_HEADER);

	png_read_png(m_pngStructPtr, m_pngInfoPtr, PNG_TRANSFORM_SWAP_ALPHA, NULL);
	//png_read_info(m_pngStructPtr, m_pngInfoPtr);

	if (png_get_color_type(m_pngStructPtr, m_pngInfoPtr) != PNG_COLOR_TYPE_RGB
		&& png_get_color_type(m_pngStructPtr, m_pngInfoPtr) != PNG_COLOR_TYPE_RGBA)
		return RES_ERROR_ILLEGAL_PNG_FILE;

	m_pngWidth = png_get_image_width(m_pngStructPtr, m_pngInfoPtr);
	m_pngHeight = png_get_image_height(m_pngStructPtr, m_pngInfoPtr);
	m_colorType = png_get_color_type(m_pngStructPtr, m_pngInfoPtr);
	m_bitDepth = png_get_bit_depth(m_pngStructPtr, m_pngInfoPtr);
	m_colorChannels = png_get_channels(m_pngStructPtr, m_pngInfoPtr);


	m_rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * m_pngHeight);
	png_uint_32 rowSize = png_get_rowbytes(m_pngStructPtr, m_pngInfoPtr);
	png_byte* pngPixelData = (png_byte*)malloc(rowSize * m_pngHeight);
	for (unsigned int rowIndex = 0; rowIndex < m_pngHeight; ++rowIndex)
	{
		png_byte *rowHead = (png_byte*)((int)pngPixelData + rowIndex * rowSize);
		m_rowPointers[rowIndex] = (png_byte*)rowHead;
	}

	png_set_rows(m_pngStructPtr, m_pngInfoPtr, m_rowPointers);

	fclose(fp);
	png_destroy_read_struct(&m_pngStructPtr, &m_pngInfoPtr, nullptr);

	return RES_SUCCESS;
}

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	
	return 0;
}

void TestFunc()
{
	//int size =  sizeof(ID2D1Bitmap);
	//size += 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int ShowCmd)
//int _tmain(int argc, _TCHAR* argv[])
{
	//ReadPngFile("D:\\code\\temp\\Gear\\docs\\image.settingIcon.png");
	HRESULT hRes = ::CoInitialize(NULL);
	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls
	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
	GearEnv::Init();

	TestFunc();
	TestLuaCFunction_TestCode();
	TestLuaObj_TestCode();
	ResManager::GetInstance().AddResPath(L"..\\..\\..\\..\\docs\\");
	Gear::Xml::XmlAgent& xmlAgent = Gear::Xml::XmlAgent::GetInstance();
	xmlAgent.GetXmlRootElement("..\\..\\..\\docs\\SampleWnd.xml");

	int nRet = theLoop.Run();
	_Module.RemoveMessageLoop();
	_Module.Term();

	GearEnv::UnInit();
	::CoUninitialize();

	return nRet;
}



const wchar_t szAppName[] = L"异形窗口2 MoreWindows-(http://blog.csdn.net/MoreWindows)";  

/* 
* 函数名称: GetWindowSize 
* 函数功能: 得到窗口的宽高 
* hwnd      窗口句柄 
* pnWidth   窗口宽 
* pnHeight  窗口高 
*/  
void GetWindowSize(HWND hwnd, int *pnWidth, int *pnHeight);  


/* 
* 函数名称: InitBitmapWindow 
* 函数功能: 位图窗口初始化 
* hinstance 进程实例 
* nWidth    窗口宽 
* nHeight   窗口高 
* nCmdshow  显示方式-与ShowWindow函数的第二个参数相同 
*/  
BOOL InitBitmapWindow(HINSTANCE hinstance, int nWidth, int nHeight, int nCmdshow);  

// 位图窗口消息处理函数  
LRESULT CALLBACK BitmapWindowWndPrco(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParm);  


HBITMAP  g_hBitmap;  
/*int APIENTRY WinMain(HINSTANCE hInstance,  
					 HINSTANCE hPrevInstance,  
					 LPSTR     lpCmdLine,  
					 int       nCmdShow) 
{  
	//先创建一个无背影画刷窗口，  
	//然后在WM_CREATE中并指定透明颜色, 缩放位图后加载至s_hdcMem中.  
	//最后在WM_ERASEBKGND中用s_hdcMem贴图即可  
	g_hBitmap = (HBITMAP)LoadImage(NULL, L"Kitty.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);  
	if (g_hBitmap == NULL)  
	{  
		MessageBox(NULL, L"位图加载失败", L"Error", MB_ICONERROR);  
		return 0;  
	}  

	// 设置异形窗口大小  
	BITMAP bm;  
	GetObject(g_hBitmap, sizeof(bm), &bm);  
	int nWindowWidth = bm.bmWidth;  
	int nWindowHeight = bm.bmHeight + 100; //拉高100高度  

	if (!InitBitmapWindow(hInstance, nWindowWidth, nWindowHeight, nCmdShow))  
		return 0;  

	MSG msg;  
	while (GetMessage(&msg, NULL, 0, 0))  
	{  
		TranslateMessage(&msg);  
		DispatchMessage(&msg);  
	}  
	DeleteObject(g_hBitmap);  

	return msg.wParam;  
}  
*/ 

BOOL InitBitmapWindow(HINSTANCE hinstance, int nWidth, int nHeight, int nCmdshow)  
{  
	HWND hwnd;  
	WNDCLASS wndclass;  

	wndclass.style       = CS_VREDRAW | CS_HREDRAW;  
	wndclass.lpfnWndProc = BitmapWindowWndPrco;   
	wndclass.cbClsExtra  = 0;  
	wndclass.cbWndExtra  = 0;  
	wndclass.hInstance   = hinstance;     
	wndclass.hIcon       = LoadIcon(NULL, IDI_APPLICATION);  
	wndclass.hCursor     = LoadCursor(NULL, IDC_ARROW);  
	wndclass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);//窗口背影画刷为空  
	wndclass.lpszMenuName  = NULL;  
	wndclass.lpszClassName = szAppName;  

	if (!RegisterClass(&wndclass))  
	{  
		MessageBox(NULL, L"Program Need Windows NT!", L"Error", MB_ICONERROR);  
		return FALSE;  
	}  

	hwnd = CreateWindowEx(WS_EX_TOPMOST,  
		szAppName,  
		szAppName,   
		WS_POPUP,  
		CW_USEDEFAULT,   
		CW_USEDEFAULT,   
		nWidth,   
		nHeight,  
		NULL,  
		NULL,  
		hinstance,  
		NULL);  
	if (hwnd == NULL)  
		return FALSE;  

	ShowWindow(hwnd, nCmdshow);  
	UpdateWindow(hwnd);  

	return TRUE;  
}  

LRESULT CALLBACK BitmapWindowWndPrco(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParm)  
{  
	static HDC s_hdcMem; //放置缩放后的位图  

	switch (message)  
	{  
	case WM_CREATE:  
		{  
			// 设置分层属性  
			SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);  
			// 设置透明色   
			SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);   
		}  
		return 0;  
	case WM_KEYDOWN:   
		switch (wParam)  
		{  
		case VK_ESCAPE: //按下Esc键时退出  
			SendMessage(hwnd, WM_DESTROY, 0, 0);  
			return TRUE;  
		}  
		break;  
	case WM_LBUTTONDOWN: //当鼠标左键点击时可以拖曳窗口  
		PostMessage(hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);   
		return TRUE;  

	case WM_ERASEBKGND: //在窗口背景中直接贴图  
		{  
			HDC hdc = (HDC)wParam;  
			int nWidth, nHeight;  
			GetWindowSize(hwnd, &nWidth, &nHeight);  
			//BitBlt(hdc, 0, 0, nWidth, nHeight, s_hdcMem, 0, 0, SRCCOPY);  
			return TRUE;  
		}  
	case WM_DESTROY:  
		DeleteDC(s_hdcMem);  
		PostQuitMessage(0);  
		return 0;  
	}  
	return DefWindowProc(hwnd, message, wParam, lParm);  
}  


void GetWindowSize(HWND hwnd, int *pnWidth, int *pnHeight)  
{  
	RECT rc;  
	GetWindowRect(hwnd, &rc);  
	*pnWidth = rc.right - rc.left;  
	*pnHeight = rc.bottom - rc.top;  
}  
