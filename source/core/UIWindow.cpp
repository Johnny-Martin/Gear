#include "stdafx.h"
#include "UIWindow.h"

UIWindow::UIWindow()
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}

void UIWindow::InitAttrMap()
{
	ADD_ATTR("title",	"")
	ADD_ATTR("layered", "0")	
	ADD_ATTR("topmost", "0")	
	ADD_ATTR("blur",	"0")	
}

void UIWindow::InitEventMap()
{
	ADD_EVENT("OnShowWindow",  nullptr)
	ADD_EVENT("OnNcActivate",  nullptr)
	ADD_EVENT("OnFocusChange", nullptr)
}

void UIWindow::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("layered", R_CHECK_BOOL);
	ADD_ATTR_PATTERN("topmost", R_CHECK_BOOL);
	ADD_ATTR_PATTERN("blur",	R_CHECK_BOOL);
}

void UIWindow::InitAttrValueParserMap()
{

}
