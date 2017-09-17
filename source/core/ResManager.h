/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.8.25
Description:		界面图片资源管理器
*****************************************************/
#pragma once
#include "stdafx.h"
//#include "pnginfo.h"
#include "png.h"
#include "Log.h"
#include "UIError.h"
#include "UIObject.h"
#include "UIRender.h"
using namespace std;

namespace Gear {
	namespace Res{

class ResPicture;
enum ResType
{
	RES_INVALIDE_TYPE = 0,
	RES_IMAGE = 1,
	RES_TEXTURE = 2,
	RES_IMAGELIST = 3,
	RES_TEXTURELIST = 4,
};

class ResManager
{
public:
									ResManager() {};
									ResManager(LPWSTR szResPath);
									~ResManager();
	RESERROR						AddResPath(const wstring& cwstrPath);
	static ResManager&				GetInstance();
	//主要接口
	ResPicture*						GetResObject(const string& strResID);
	bool							LoadResource(const string& strResID);
	bool							LoadResFromFile(const wstring& wstrFilePath, const string& strResID, ResType resType);
private:
	
	unsigned int					GetIndexFromPicListId(LPCSTR szPicListID);
	string							GetRealIdFromPicListId(LPCSTR szPicListID);
	wstring							GetResFilePath(LPCSTR szResID);
	wstring							m_wszResPath;
	//map<string, RPicture*>		m_resID2HandleMap;
	vector<wstring>					m_resPathVec;
	map<const string, ResPicture*>  m_resMap;
};

class ResPicture
{
public:
									ResPicture();
									~ResPicture();
									ResPicture(const wstring& wstrFilePath);
#ifdef USE_D2D_RENDER_MODE
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height) = 0;
#else
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height)  = 0;
#endif
protected:
	RESERROR						ReadPngFile(const string& strFilePath);
	bool							ReadPngFile(const wstring& wstrFilePath);
	bool							IsVerticalLine(unsigned int horizontalPos, COLORREF lineColor);
	bool							IsHorizontalLine(unsigned int verticalPos, COLORREF lineColor);
	RESERROR						DetectVerticalLine();
	RESERROR						DetectHorizontalLine();
protected:
	bool							m_bPngFileLoaded;
	wstring							m_wstrFilePath;
	png_uint_32						m_pngWidth;
	png_uint_32						m_pngHeight;
	png_byte						m_colorType;
	png_byte						m_bitDepth;
	png_byte						m_colorChannels;
	png_bytep*						m_rowPointers; //In fact, m_rowPointers is a two-dimensional array
	png_structp						m_pngStructPtr;
	png_infop						m_pngInfoPtr;

	HBITMAP							m_hResHandle;//??????
	RESERROR						m_resError;
	vector<unsigned int>			m_arrVerticalLinePos;
	vector<unsigned int>			m_arrHorizontalLinePos;
	const COLORREF					m_purpleLineColor;
};

class ResImage:public ResPicture
{
public:
									ResImage(const wstring& wstrFilePath);
#ifdef USE_D2D_RENDER_MODE
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height);
#else
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height);
#endif
};

class ResTexture :public ResPicture
{
public:
									ResTexture(const wstring& wstrFilePath);
#ifdef USE_D2D_RENDER_MODE
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height);
#else
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height);
#endif
};

class PicListDivider :public ResPicture
{
public:
									PicListDivider(const wstring& wstrFilePath);
#ifdef USE_D2D_RENDER_MODE
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height) { return nullptr; }
#else
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height)  { return nullptr; }
#endif
	unsigned int					GetPicCount();
	ResPicture*						GetPicByIndex();
};

//class UIImage
class UIBitmap :public UIObject, public ResPicture
{
public:
									UIBitmap();
#ifdef USE_D2D_RENDER_MODE
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height);
#else
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height);
#endif
	bool							Init(const XMLElement* pElement);
protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
private:
	ResPicture*						m_picObject;
};

	}//end of namespace Res
}//end of namespace Gear