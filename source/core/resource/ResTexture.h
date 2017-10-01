#pragma once
#include "../stdafx.h"
#include "../entry/RenderManager.h"
#include "../base/XmlUIElement.h"
#include "ResPicture.h"
#include "png.h"

namespace Gear {
	namespace Res {

class ResTexture :public XmlUIElement, public ResPicture
{
public:
	ResTexture();
	ResTexture(const wstring& wstrFilePath);
	ResTexture(const string& strImageDesc);
	ResTexture::ResTexture(png_bytep* rowPointers, png_uint_32 width, png_uint_32 height, png_byte colorType, png_byte colorChannels, png_byte bitDepth);
#ifdef USE_D2D_RENDER_MODE
	virtual ID2D1Bitmap*						GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight);
	virtual HRESULT								OnDrawImplEx(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos, UIObject* obj /*= nullptr*/);
private:
	vector<ID2D1Bitmap*>						m_arrD2D1Bitmap;
#else
	virtual Gdiplus::Bitmap*					GetGDIBitmap(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight);
	virtual HRESULT								OnDrawImplEx(Graphics& graphics, const UIPos& rcWndPos, UIObject* obj = nullptr);
	vector<Gdiplus::Bitmap*>					m_arrGdiplusBitmap;
#endif
protected:
	png_bytep*									CreateAndCopySubBlock(unsigned int subWidth, unsigned int subHeight, unsigned int offsetXInPix, unsigned int offsetYInPix);
	size_t										__CreateMultiSubBitmapBlock_AllStyle();
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
private:
	png_bytep*									__CreateSingleBitmapBlock_NineInOn(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight);
	png_bytep*									__CreateSingleBitmapBlock_ThreeH(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight);
	png_bytep*									__CreateSingleBitmapBlock_ThreeV(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight);
	const COLORREF								m_purpleLineColor;
	vector<unsigned int>						m_arrVerticalLinePos;
	vector<unsigned int>						m_arrHorizontalLinePos;
	bool										IsVerticalLine(unsigned int horizontalPos, COLORREF lineColor);
	bool										IsHorizontalLine(unsigned int verticalPos, COLORREF lineColor);
	RESERROR									DetectVerticalLine();
	RESERROR									DetectHorizontalLine();
	png_uint_32									m_lastQueryWidth;
	png_uint_32									m_lastQueryHeight;
	vector<png_bytep*>							m_arrSubBitmapBlock;
	vector<pair<unsigned int, unsigned int>>	m_arrSubBitmapBlockSize;
};

	}//end of namespace Res
}//end of namespace Gear