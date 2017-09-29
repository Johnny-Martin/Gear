#pragma once
#include "../stdafx.h"
#include "../entry/RenderManager.h"
#include "../base/XmlUIElement.h"
#include "ResPicture.h"
#include "ResManager.h"
#include "png.h"

namespace Gear {
	namespace Res {

enum ResType;
class ResList :public XmlUIElement, public ResPicture
{
public:
	ResList();
	ResList(const string& strImageDesc, const wstring& wstrPath);
	ResList(const wstring& strImageDesc);
	virtual										~ResList();
	png_uint_32									LoadAllSubPictures();
	ResPicture*									GetSubPicObjByIndex(unsigned int posIndex);
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
private:
	unsigned char								m_hCount;
	unsigned char								m_vCount;
	png_uint_32									m_subPicWidth;
	png_uint_32									m_subPicHeight;
	vector<ResPicture*>							m_subPicVec;
	ResType										m_subPicType;
};

	}//end of namespace Res
}//end of namespace Gear