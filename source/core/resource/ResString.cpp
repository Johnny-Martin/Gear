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
	ADD_ATTR("zh_cn", "")//简体中文
	ADD_ATTR("zh_hk", "")//香港繁体
	ADD_ATTR("zh_tw", "")//台湾繁体
	ADD_ATTR("en_us", "")//美国英文
}
void ResString::InitAttrValuePatternMap()
{
	
}
void ResString::InitAttrValueParserMap()
{
	
}