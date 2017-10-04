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
	//以下属性应属于ResString
	ADD_ATTR("color", "#FFFFFF")
	ADD_ATTR("face", "微软雅黑")
	ADD_ATTR("height", "12")
	ADD_ATTR("weight", "normal")//加粗等字体样式
	ADD_ATTR("underline", "0")//下划线
	ADD_ATTR("italic", "0")//倾斜
	ADD_ATTR("strickout", "0")//删除线
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