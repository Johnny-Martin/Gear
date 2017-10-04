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
	ADD_ATTR("color", "#FFFFFF")
	ADD_ATTR("face", "΢���ź�")
	ADD_ATTR("height", "12")
	ADD_ATTR("weight", "normal")//�Ӵֵ�������ʽ
	ADD_ATTR("underline", "0")//�»���
	ADD_ATTR("italic", "0")//��б
	ADD_ATTR("strickout", "0")//ɾ����
}
void ResFont::InitAttrValuePatternMap()
{
	//ADD_ATTR_PATTERN("color", R_CHECK_COLOR_VALUE);
	ADD_ATTR_PATTERN("height", R_CHECK_INT);
	ADD_ATTR_PATTERN("underline", R_CHECK_BOOL);
	ADD_ATTR_PATTERN("italic", R_CHECK_BOOL);
	ADD_ATTR_PATTERN("strickout", R_CHECK_BOOL);
}
void ResFont::InitAttrValueParserMap()
{

}