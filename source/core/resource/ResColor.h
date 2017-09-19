/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.18
Description:		界面颜色资源
*****************************************************/
#pragma once
#include "../stdafx.h"
#include "../base/XmlUIElement.h"
class ResColor:public XmlUIElement
{
public:
	ResColor();
	virtual bool								Init(const XMLElement* pElement);
protected:
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
};