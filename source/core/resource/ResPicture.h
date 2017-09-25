#pragma once
#include "../stdafx.h"
#include "png.h"

namespace Gear {
	namespace Res {

class ResPicture
{
public:
	ResPicture();
	~ResPicture();
	ResPicture(const wstring& wstrFilePath);
#ifdef USE_D2D_RENDER_MODE
public:
	virtual ID2D1Bitmap*			GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight) = 0;
	virtual HRESULT					DiscardD2D1Bitmap();
protected:
	ID2D1Bitmap*					m_d2d1BitmapPtr;
#else
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight) = 0;
protected:
	Gdiplus::Bitmap*				m_gdiplusBitmapPtr;
#endif
protected:
	RESERROR						ReadPngFile(const string& strFilePath);
	bool							ReadPngFile(const wstring& wstrFilePath);
	bool							IsVerticalLine(unsigned int horizontalPos, COLORREF lineColor);
	bool							IsHorizontalLine(unsigned int verticalPos, COLORREF lineColor);
	RESERROR						DetectVerticalLine();
	RESERROR						DetectHorizontalLine();
protected:
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

	}//end of namespace Res
}//end of namespace Gear