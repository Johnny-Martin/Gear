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
	ADD_ATTR("zh_cn", "")//��������
	ADD_ATTR("zh_hk", "")//��۷���
	ADD_ATTR("zh_tw", "")//̨�己��
	ADD_ATTR("en_us", "")//����Ӣ��
}
void ResString::InitAttrValuePatternMap()
{
	
}
void ResString::InitAttrValueParserMap()
{
	
}