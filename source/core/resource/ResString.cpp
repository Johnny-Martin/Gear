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
	//��������Ӧ����ResString
	ADD_ATTR("color", "")
	ADD_ATTR("font", "΢���ź�")
	ADD_ATTR("weight", "12")
}
void ResString::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("color", R_CHECK_COLOR_VALUE);
}
void ResString::InitAttrValueParserMap()
{
	
}