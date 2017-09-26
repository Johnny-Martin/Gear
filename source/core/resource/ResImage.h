#pragma once
#include "../stdafx.h"
#include "../entry/RenderManager.h"
#include "../base/XmlUIElement.h"
#include "ResPicture.h"
#include "png.h"

namespace Gear {
	namespace Res {

class ResImage :public XmlUIElement, public ResPicture
{
public:
												ResImage();
												ResImage(const string& strImageDesc);
												ResImage(const wstring& strImageDesc);
	virtual										~ResImage();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual ID2D1Bitmap*						GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight);
#else
public:
	virtual Gdiplus::Bitmap*					GetGDIBitmap(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight);
#endif
protected:
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
};

	}//end of namespace Res
}//end of namespace Gear