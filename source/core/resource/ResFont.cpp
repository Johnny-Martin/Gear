#include "stdafx.h"
#include "ResFont.h"
#include "../entry/RenderManager.h"

ResFont::ResFont()
#ifdef USE_D2D_RENDER_MODE
	:m_d2d1TextFormatPtr(nullptr)
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
	:m_d2d1TextFormatPtr(nullptr)
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
	//��������Ӧ����ResString
	ADD_ATTR("color", "#FFFFFF")
	ADD_ATTR("face", "΢���ź�")
	ADD_ATTR("height", "12")
	ADD_ATTR("weight", "normal")//�Ӵֵ�������ʽ
	ADD_ATTR("bold", "0")//����
	ADD_ATTR("underline", "0")//�»���
	ADD_ATTR("italic", "0")//��б
	ADD_ATTR("strickout", "0")//ɾ����
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

///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
IDWriteTextFormat* ResFont::GetD2D1TextFormat()
{
	if (m_d2d1TextFormatPtr) {
		return m_d2d1TextFormatPtr;
	}
	wstring face = StringToWString(m_attrMap["face"]);
	FLOAT height = (FLOAT)atoi(m_attrMap["height"].c_str());
	DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL;
	if (m_attrMap["bold"] == "1") {
		weight = DWRITE_FONT_WEIGHT_BOLD;
	}
	DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL;
	if (m_attrMap["italic"] == "1") {
		style = DWRITE_FONT_STYLE_ITALIC;
	}
	DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL;
	HRESULT hr = RenderManager::m_pD2DWriteFactory->CreateTextFormat(
				face.c_str(),
		NULL,
		weight,
		style,
		stretch,
		height,
		L"en-us",
		&m_d2d1TextFormatPtr
	);
	if (SUCCEEDED(hr)){
		return m_d2d1TextFormatPtr;
	}
	return nullptr;
}
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
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
#endif