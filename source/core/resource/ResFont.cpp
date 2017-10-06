#include "stdafx.h"
#include "ResFont.h"

ResFont::ResFont()
#ifdef USE_D2D_RENDER_MODE

#else
	: m_gdiplusFontPtr(nullptr)
#endif
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
ResFont::ResFont(const string& strFontDesc)
#ifdef USE_D2D_RENDER_MODE

#else
	: m_gdiplusFontPtr(nullptr)
#endif
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();

	vector<string> attrNames{"face", "height", "bold", "italic", "underline", "strickout"};
	auto vecDesc = Split(strFontDesc, ".");
	for (size_t i=0; i<attrNames.size() && i<vecDesc.size(); ++i){
		SetAttrValue(attrNames[i], vecDesc[i]);
	}
}
void ResFont::InitAttrMap()
{
	//以下属性应属于ResString
	ADD_ATTR("color", "#FFFFFF")
	ADD_ATTR("face", "微软雅黑")
	ADD_ATTR("height", "12")
	ADD_ATTR("weight", "normal")//加粗等字体样式
	ADD_ATTR("bold", "0")//粗体
	ADD_ATTR("underline", "0")//下划线
	ADD_ATTR("italic", "0")//倾斜
	ADD_ATTR("strickout", "0")//删除线
}
void ResFont::InitAttrValuePatternMap()
{
	//ADD_ATTR_PATTERN("color", R_CHECK_COLOR_VALUE);
	ADD_ATTR_PATTERN("height", R_CHECK_INT);
	ADD_ATTR_PATTERN("bold", R_CHECK_BOOL);
	ADD_ATTR_PATTERN("underline", R_CHECK_BOOL);
	ADD_ATTR_PATTERN("italic", R_CHECK_BOOL);
	ADD_ATTR_PATTERN("strickout", R_CHECK_BOOL);
}
void ResFont::InitAttrValueParserMap()
{

}
Gdiplus::Font* ResFont::GetGdiplusFont()
{
	if (m_gdiplusFontPtr){
		return m_gdiplusFontPtr;
	}
	wstring faceName = StringToWString(m_attrMap["face"]);
	REAL height = (REAL)atoi(m_attrMap["height"].c_str());
	Gdiplus::FontStyle style;
	if (m_attrMap["bold"] == "1"){
		style = Gdiplus::FontStyle::FontStyleBold;
	} else if (m_attrMap["underline"] == "1") {
		style = Gdiplus::FontStyle::FontStyleUnderline;
	} else if (m_attrMap["italic"] == "1") {
		style = Gdiplus::FontStyle::FontStyleItalic;
	}else if (m_attrMap["strickout"] == "1"){
		style = Gdiplus::FontStyle::FontStyleStrikeout;
	}

	m_gdiplusFontPtr = new Gdiplus::Font(faceName.c_str(), height, style);
	return m_gdiplusFontPtr;
}