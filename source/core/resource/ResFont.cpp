#include "stdafx.h"
#include "ResFont.h"

ResFont::ResFont()
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}

void ResFont::InitAttrMap()
{
	//��������Ӧ����ResString
	ADD_ATTR("color", "")
	ADD_ATTR("font", "΢���ź�")
	ADD_ATTR("weight", "12")
}
void ResFont::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("color", R_CHECK_COLOR_VALUE);
}
void ResFont::InitAttrValueParserMap()
{

}