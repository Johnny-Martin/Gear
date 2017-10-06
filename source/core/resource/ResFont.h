#pragma once
#include "../stdafx.h"
#include "../base/XmlUIElement.h"

class ResFont:public XmlUIElement
{
public:
	ResFont();
	ResFont(const string& strFontDesc);
#ifdef USE_D2D_RENDER_MODE
public:

#else
public:
	Gdiplus::Font*								GetGdiplusFont();
private:
	Gdiplus::Font*								m_gdiplusFontPtr;
#endif
protected:
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
private:
};