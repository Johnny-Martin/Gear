#pragma once
#include "../stdafx.h"
#include "../base/XmlUIElement.h"
class ResString :public XmlUIElement
{
public:
	ResString();
	wstring GetStringW();
protected:
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
	private:
};