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
				auto sHexR = regex_replace(valueExp, sPattern, string("$2"));
				auto sHexG = regex_replace(valueExp, sPattern, string("$3"));
				auto sHexB = regex_replace(valueExp, sPattern, string("$4"));
				UpdateColorValue(sHexR, sHexG, sHexB);
			}else if (valueExp.length() == 8){
				regex sPattern(R_CHECK_COLOR_HEX_8);
				auto sHexR = regex_replace(valueExp, sPattern, string("$2"));				
				auto sHexG = regex_replace(valueExp, sPattern, string("$3"));	
				auto sHexB = regex_replace(valueExp, sPattern, string("$4"));
				auto sHexA = regex_replace(valueExp, sPattern, string("$5"));
				UpdateColorValue(sHexR, sHexG, sHexB, sHexA);
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
void ResColor::UpdateColorValue(const string& sHexR, const string& sHexG, const string& sHexB)
{
	char* endstr;
	m_R = static_cast<unsigned char>(strtol(sHexR.c_str(), &endstr, 16));
	m_G = static_cast<unsigned char>(strtol(sHexG.c_str(), &endstr, 16));
	m_B = static_cast<unsigned char>(strtol(sHexB.c_str(), &endstr, 16));

#ifdef USE_D2D_RENDER_MODE
	UpdateD2D1ColorF();
#else
	UpdateGdiplusColor();
#endif
}
void ResColor::UpdateColorValue(const string& sHexR, const string& sHexG, const string& sHexB, const string& sHexA)
{
	UpdateColorValue(sHexR, sHexG, sHexB);
	char* endstr;
	m_A = static_cast<unsigned char>(strtol(sHexA.c_str(), &endstr, 16));

#ifdef USE_D2D_RENDER_MODE
	UpdateD2D1ColorF();
#else
	UpdateGdiplusColor();
#endif
}

#ifdef USE_D2D_RENDER_MODE
void ResColor::UpdateD2D1ColorF()
{
	m_d2d1ColorF.r = m_R / 255.f;
	m_d2d1ColorF.g = m_G / 255.f;
	m_d2d1ColorF.b = m_B / 255.f;
	m_d2d1ColorF.a = m_A / 255.f;
}
D2D1::ColorF ResColor::GetD2D1ColorF()
{
	return m_d2d1ColorF;
}
#else
void ResColor::UpdateGdiplusColor()
{
	m_gdiplusColor.SetValue(Color::MakeARGB(m_A, m_R, m_G, m_B));
}
Gdiplus::Color ResColor::GetGdiplusColor()
{
	return m_gdiplusColor;
}
#endif