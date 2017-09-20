#include "stdafx.h"
#include "../Micros.h"
#include "ResColor.h"

#ifdef USE_D2D_RENDER_MODE
ResColor::ResColor() :m_R(0), m_G(0), m_B(0), m_A(255), m_d2d1ColorF(0,0,0)
#else
ResColor::ResColor() : m_R(0), m_G(0), m_B(0), m_A(255)
#endif
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
#ifdef USE_D2D_RENDER_MODE
ResColor::ResColor(const string& sColorHexValue) :m_R(0), m_G(0), m_B(0), m_A(255), m_d2d1ColorF(0, 0, 0)
#else
ResColor::ResColor(const string& sColorHexValue) : m_R(0), m_G(0), m_B(0), m_A(255)
#endif
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();

	SetAttrValue("id", CreateGUIDAsResID());
	SetAttrValue("value", sColorHexValue);
}
bool ResColor::Init(const XMLElement* pElement)
{
	//保存element的属性
	auto pAttr = pElement->FirstAttribute();
	while (pAttr) {
		auto attrName = pAttr->Name();
		SetAttrValue(attrName, pAttr->Value());
		pAttr = pAttr->Next();
	}

	return true;
}
#ifdef USE_D2D_RENDER_MODE
D2D1::ColorF ResColor::GetD2D1ColorF()
{
	return m_d2d1ColorF;
}
#endif
void ResColor::InitAttrMap()
{
	ADD_ATTR("value",	"")//十六进制RGB(A)值，alpha通道可选
	ADD_ATTR("rgb",		"")//十进制RGB值，逗号分割
	ADD_ATTR("rgba",	"")
	ADD_ATTR("argb",	"")
}
void ResColor::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("value",	R_CHECK_COLOR_VALUE);
	ADD_ATTR_PATTERN("rgb",		R_CHECK_COLOR_RGB);
	ADD_ATTR_PATTERN("rgba",	R_CHECK_COLOR_RGBA);
	ADD_ATTR_PATTERN("argb",	R_CHECK_COLOR_RGBA);
}
void ResColor::InitAttrValueParserMap()
{
	auto ParseHexValue = [&](const string& sAttrName)->bool {
		try {
			auto valueExp = m_attrMap[sAttrName];
			if (valueExp.length() == 6){
				regex sPattern(R_CHECK_COLOR_HEX_6);
				char* endstr;
				string channelHex = regex_replace(valueExp, sPattern, string("$2"));
				m_R = static_cast<unsigned char>(strtol(channelHex.c_str(), &endstr, 16));

				channelHex = regex_replace(valueExp, sPattern, string("$3"));
				m_G = static_cast<unsigned char>(strtol(channelHex.c_str(), &endstr, 16));

				channelHex = regex_replace(valueExp, sPattern, string("$4"));
				m_B = static_cast<unsigned char>(strtol(channelHex.c_str(), &endstr, 16));
			}else if (valueExp.length() == 8){
				regex sPattern(R_CHECK_COLOR_HEX_8);
				char* endstr;
				string channelHex = regex_replace(valueExp, sPattern, string("$2"));
				m_R = static_cast<unsigned char>(strtol(channelHex.c_str(), &endstr, 16));
				
				channelHex = regex_replace(valueExp, sPattern, string("$3"));
				m_G = static_cast<unsigned char>(strtol(channelHex.c_str(), &endstr, 16));

				channelHex = regex_replace(valueExp, sPattern, string("$4"));
				m_B = static_cast<unsigned char>(strtol(channelHex.c_str(), &endstr, 16));

				channelHex = regex_replace(valueExp, sPattern, string("$5"));
				m_A = static_cast<unsigned char>(strtol(channelHex.c_str(), &endstr, 16));
			} else {
				ERR("ParseHexValue error: illegal value length, color valueExp: {}", valueExp);
				return false;
			}
			return true;
		}
		catch (...) {
			ERR("ParseValue error: catch exception");
			return false;
		}
	};

	ADD_ATTR_PARSER("value", ParseHexValue)
}