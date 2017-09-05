#pragma once
/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.5
Description:		UI´°¿Ú£¬ÖØ¹¹UIFrameWnd
*****************************************************/

#include "stdafx.h"
#include "UIObject.h"
#include <list>
#include <xstring>


class UIWindow:public UIObject
{
public:
	UIWindow();
public:

protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
};