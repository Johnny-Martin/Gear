/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.26
Description:		界面图片资源管理
*****************************************************/

#ifndef RESOURCRMANAGER_H
#define RESOURCRMANAGER_H

#include "stdafx.h"
#include "png.h"
#include "UIError.h"
#include "UIObject.h"
#include<gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "GdiPlus.lib")


#define USE_GDI_RENDERING
//#define USE_D2D_RENDERING
using namespace Gear::Res;

enum UIResType {
	RES_IMAGE			  = 0,
	RES_TEXTURE_THREE_H   = 1,
	RES_TEXTURE_THREE_V   = 2,
	RES_TEXTURE_NINE      = 3,
	RES_IMAGELIST_IMAGE	  = 4,
	RES_IMAGELIST_THREE_H = 5,
	RES_IMAGELIST_THREE_V = 6,
	RES_IMAGELIST_NINE    = 7,
};

class UIRes
{
public:
									UIRes(const string& strFilePath);
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height)	= 0;
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height)	= 0;
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

class UIImage :public UIBase
{
public:
	UIImage();
protected:
	bool										Load();
	void										InitAttrMap();
	void										InitEventMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
};

using namespace std;
namespace Gear {
	namespace Res {

enum PICLIST_TYPE{
	TEXTURELIST = 0,
	IMAGELIST,
};
enum TEXTURE_TYPE{
	THREE_V = 0,
	NINE,
	THREE_H,
};
//'R' is short for "Resource"

/*

*/
class RPicture
{
public:
									RPicture();
	virtual							~RPicture();
	void							SetResID(string resID);
	RESERROR						GetLastResError() const;
	RESERROR						WritePngFile(LPCWSTR wszFilePath);
	RESERROR						CreatePicByData(LPCSTR szResID, png_bytep* rowPointers, unsigned int width, unsigned int height, png_byte bitDepth, png_byte colorType);
	png_bytep*						GetRowPointers() const;
	png_infop						GetPngInfo() const;
	virtual RESERROR				Draw(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom) = 0;
protected:
	virtual RESERROR				LoadResource(LPCWSTR wszResPath) = 0;
	RESERROR						ReadPngFile(LPCWSTR wszFilePath);
	
	
	bool							IsVerticalLine(unsigned int horizontalPos, COLORREF lineColor);
	bool							IsHorizontalLine(unsigned int verticalPos, COLORREF lineColor);
	png_uint_32						m_pngWidth;
	png_uint_32						m_pngHeight;
	png_byte						m_colorType;
	png_byte						m_bitDepth;
	png_byte						m_pixelDepth;
	png_bytep*						m_rowPointers; //In fact, m_rowPointers is a two-dimensional array
	png_structp						m_pngStructPtr;
	png_infop						m_pngInfoPtr;

	HBITMAP							m_hResHandle;//??????
	string							m_szResID;
	string							m_szResTypeInfo;
	RESERROR						m_resError;

};

class RImage: public RPicture
{
public:
									RImage(){};
									RImage(LPCWSTR wszResPath, LPCSTR szResID);
									RImage(LPCSTR szResID, png_bytep* rowPointers, unsigned int width, unsigned int height,png_byte bitDepth, png_byte colorType);
	RESERROR						LoadResource(LPCWSTR wszResPath);
	RESERROR						Draw(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom);
private:
	RESERROR						CreateD2D1Bitmap(ID2D1RenderTarget* pRenderTarget);
	vector<ID2D1Bitmap*>			m_arrD2D1Bitmap;
};

/*
a texture object's resource ID *must* begin with "texture"
and next is texture's type, which include :
     vertical three in one、horizontal three in one、 nine in one **
    a texture should be used in xml like this:
 	 <texture id="bkg">
			<attr>
				<position>0,20,parent.width, parent.height-20</position>
				<resid>texture.ThreeInOne.customRectBkg</resid>
			</attr>
			......
		 </texture>
the texture's dividing line's color *must* be (255,0,255), and *one* pixel wide
*/
class RTexture: public RPicture
{
public:
									RTexture();
									RTexture(LPCWSTR wszResPath, LPCSTR szResID);
									//create a texture object with a two-dimensional array, and assign the png width and height
									RTexture(LPCSTR szResID, png_bytep* rowPointers, unsigned int width, unsigned int height, png_byte bitDepth, png_byte colorType);
	RESERROR						LoadResource(LPCWSTR wszResPath);
	RESERROR						Draw(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom);
protected:
	RESERROR						DetectVerticalLine();
	RESERROR						DetectHorizontalLine();
private:
	void							SetTextureType(LPCSTR resID);
	void							InitMemberVariable();
	RESERROR						ProcessTexture();
	RESERROR						CreateD2D1Bitmap(ID2D1RenderTarget* pRenderTarget);
	RESERROR						_CreateD2D1Bitmap_Nine(ID2D1RenderTarget* pRenderTarget);
	RESERROR						_CreateD2D1Bitmap_ThreeV(ID2D1RenderTarget* pRenderTarget);
	RESERROR						_CreateD2D1Bitmap_ThreeH(ID2D1RenderTarget* pRenderTarget);

	RESERROR						_Draw_Nine(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom);
	RESERROR						_Draw_ThreeV(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom);
	RESERROR						_Draw_ThreeH(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom);
									//vertical dividing line's position in horizontal direction
	vector<unsigned int>			m_arrVerticalLinePos;
									//horizontal dividing line's position in vertical direction
	vector<unsigned int>			m_arrHorizontalLinePos;
	vector<ID2D1Bitmap*>			m_arrD2D1Bitmap;
	const COLORREF					m_purpleLineColor;
	TEXTURE_TYPE					m_textureType;
};

/*a RPicList object's resource ID *must* begin with "imagelist" or "texturelist"
    a texture should be used in xml like this:
 	 <texture id="bkg">
			<attr>
				<position>0,20,parent.width, parent.height-20</position>
				<resid>texturelist.ThreeInOne.customRectBkg.3</resid>
			</attr>
			......
		 </texture>
		 <image id="bkg">
			<attr>
				<position>0,20,parent.width, parent.height-20</position>
				<resid>imagelist.customRectBkg.3</resid>
			</attr>
			......
		 </image>
the last part of resid must be a number, the index of pic in picture list
the texture's dividing line's color *must* be (127,0,127),different from 
texture's dividing line, and *one* pixel wide
*/
class RPicList: public RPicture
{
public:
									RPicList();
									RPicList(LPCWSTR wszResPath, LPCSTR resID);
									~RPicList();
	RPicture*						GetPicByIndex(unsigned int index);
	RESERROR						LoadResource(LPCWSTR wszResPath);

									//RPicList do not need a draw function
	RESERROR						Draw(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom);
protected:
	RESERROR						DetectVerticalLine();
	RESERROR						CreatePicFromMem();
private:
	void							SetPicListType(LPCSTR resID);
	
									//vertical dividing line's position in horizontal direction
	vector<unsigned int>			m_arrVerticalLinePos;
	const COLORREF					m_purpleLineColor;
	vector<RPicture*>				m_picListVector;
	PICLIST_TYPE					m_picListType;
	
};

class ResManager
{
public:
									ResManager(){};
									ResManager(LPWSTR szResPath);
									~ResManager();
	RESERROR						SetResPath(LPWSTR wszResPath);
	static bool						CheckPngFileHead(LPWSTR wszFilePath);
	RESERROR						GetResPicHandle(LPCSTR szResID, RPicture** hRes);
private:
	unsigned int					GetIndexFromPicListId(LPCSTR szPicListID);
	string							GetRealIdFromPicListId(LPCSTR szPicListID);
	wstring							GetPicPathByID(LPCSTR szResID);
	wstring							m_wszResPath;
	map<string, RPicture*>			m_resID2HandleMap;
};

	}// namespace Res
}// namespace Gear
#endif