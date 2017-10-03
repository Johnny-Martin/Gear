#pragma once
#include "../stdafx.h"
#include "../base/XmlUIElement.h"
class ResString :public XmlUIElement
{
public:
	ResString();
#ifdef USE_D2D_RENDER_MODE
public:
	
#else
public:
#endif
protected:
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
	private:
};