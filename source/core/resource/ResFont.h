#pragma once
#include "../stdafx.h"
#include "../base/XmlUIElement.h"

class ResFont:public XmlUIElement
{
public:
												ResFont();
#ifdef USE_D2D_RENDER_MODE
public:

#else
public:
private:
#endif
protected:
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
private:
};