/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.8.25
Description:		界面图片资源管理器
*****************************************************/
#pragma once
#include "stdafx.h"
#include "png.h"
#include "Log.h"
#include "UIError.h"
#include "UIObject.h"
#include<gdiplus.h>
#pragma comment(lib, "GdiPlus.lib")

using namespace std;
using namespace Gdiplus;

namespace Gear {
	namespace Res{

class UIRes;
class ResManager
{
public:
	ResManager() {};
	ResManager(LPWSTR szResPath);
	~ResManager();
	RESERROR						SetResPath(LPWSTR wszResPath);
	ResManager*						GetInstance();
	UIRes*							GetResource(const string& strResID);
private:
	unsigned int					GetIndexFromPicListId(LPCSTR szPicListID);
	string							GetRealIdFromPicListId(LPCSTR szPicListID);
	wstring							GetPicPathByID(LPCSTR szResID);
	wstring							m_wszResPath;
	//map<string, RPicture*>			m_resID2HandleMap;

};

class UIPicture
{
public:
	UIPicture();
	UIPicture(const string& strFilePath);
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height) = 0;
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height) = 0;
protected:
	RESERROR						ReadPngFile(const string& strFilePath);
	bool							IsVerticalLine(unsigned int horizontalPos, COLORREF lineColor);
	bool							IsHorizontalLine(unsigned int verticalPos, COLORREF lineColor);
	RESERROR						DetectVerticalLine();
	RESERROR						DetectHorizontalLine();
protected:
	string							m_strFilePath;
	png_uint_32						m_pngWidth;
	png_uint_32						m_pngHeight;
	png_byte						m_colorType;
	png_byte						m_bitDepth;
	png_byte						m_pixelDepth;
	png_bytep*						m_rowPointers; //In fact, m_rowPointers is a two-dimensional array
	png_structp						m_pngStructPtr;
	png_infop						m_pngInfoPtr;

	HBITMAP							m_hResHandle;//??????
	RESERROR						m_resError;
	vector<unsigned int>			m_arrVerticalLinePos;
	vector<unsigned int>			m_arrHorizontalLinePos;
	const COLORREF					m_purpleLineColor;
};

//class UIImage
class UIBitmap :public UIObject, public UIPicture
{
public:
	UIBitmap();
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height);
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height);
	bool							Init(const XMLElement* pElement);
protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
};

	}//end of namespace Res
}//end of namespace Gear