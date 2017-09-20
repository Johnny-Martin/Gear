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
#ifdef USE_D2D_RENDER_MODE
public:

#else

#endif
protected:
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
private:
	unsigned char m_R;
	unsigned char m_G;
	unsigned char m_B;
	unsigned char m_A;
};