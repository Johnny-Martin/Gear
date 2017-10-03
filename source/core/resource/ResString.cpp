#include "stdafx.h"
#include "ResString.h"

ResString::ResString()
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}

void ResString::InitAttrMap()
{
	//以下属性应属于ResString
	ADD_ATTR("color", "")
	ADD_ATTR("font", "微软雅黑")
	ADD_ATTR("weight", "12")
}
void ResString::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("color", R_CHECK_COLOR_VALUE);
}
void ResString::InitAttrValueParserMap()
{
	
}