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
	//ADD_ATTR("color", ""))
}
void ResString::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("color", R_CHECK_COLOR_VALUE);
}
void ResString::InitAttrValueParserMap()
{
	
}