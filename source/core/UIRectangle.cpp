#include "stdafx.h"
#include "UIRectangle.h"
#include "resource/ResManager.h"

using namespace Gear::Res;
#ifdef USE_D2D_RENDER_MODE
UIRectangle::UIRectangle():m_pColorBrush(nullptr)
						  ,m_pBorderColorBrush(nullptr)
{
#else
UIRectangle::UIRectangle(){
#endif
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}

void UIRectangle::InitAttrMap()
{
	ADD_ATTR("corner",		"1")
	ADD_ATTR("color",		"FFFFFF")
	ADD_ATTR("border",		"0")
	ADD_ATTR("bordercolor", "FFFFFF")
}
void UIRectangle::InitEventMap()
{

}
void UIRectangle::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("corner", R_CHECK_INT);
	ADD_ATTR_PATTERN("border", R_CHECK_INT);
	ADD_ATTR_PATTERN("color", R_CHECK_COLOR_VALUE);
	ADD_ATTR_PATTERN("bordercolor", R_CHECK_COLOR_VALUE);
}
void UIRectangle::InitAttrValueParserMap()
{
	//ADD_ATTR_PARSER("value", ParseHexValue)
}

///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT	UIRectangle::OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid)
{
	if (m_attrMap["visible"] == "0") return S_OK;
	//窗口在重绘时，应该只通知处于rcInvalid区域的UI对象
	
	D2D1_RECT_F rectF;
	rectF.left	 = static_cast<FLOAT>(m_pos.left);
	rectF.top	 = static_cast<FLOAT>(m_pos.top);
	rectF.right  = static_cast<FLOAT>(m_pos.width + m_pos.left);
	rectF.bottom = static_cast<FLOAT>(m_pos.height + m_pos.top);

	if (m_attrMap["corner"] == "0") {
		pRenderTarget->FillRectangle(rectF, m_pColorBrush);
	}else{
		FLOAT cornerRadius = static_cast<FLOAT>(atoi(m_attrMap["corner"].c_str()));
		D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rectF, cornerRadius, cornerRadius);
		pRenderTarget->FillRoundedRectangle(roundedRect, m_pColorBrush);
	}
	
	if (m_attrMap["border"] != "0") {
		FLOAT cornerRadius = static_cast<FLOAT>(atoi(m_attrMap["corner"].c_str()));
		FLOAT borderWidth  = static_cast<FLOAT>(atoi(m_attrMap["border"].c_str()));
		D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rectF, cornerRadius, cornerRadius);
		pRenderTarget->DrawRoundedRectangle(roundedRect, m_pBorderColorBrush, borderWidth);
	}

	return S_OK;
}
HRESULT	UIRectangle::CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget)
{
	if (m_pColorBrush) return S_OK;
	
	if (!pRenderTarget) {
		ERR("fatal error in CreateDeviceDependentResources, nullptr! OnDrawImpl will abort");
		return S_FALSE;
	}

	D2D1::ColorF rectColor = ResManager::GetInstance().GetColorObject(m_attrMap["color"])->GetD2D1ColorF();
	HRESULT hr = pRenderTarget->CreateSolidColorBrush(rectColor, &m_pColorBrush);
	if (FAILED(hr)) {
		ERR("fatal error in CreateSolidColorBrush, nullptr! hr:{}", hr);
		return hr;
	}
	if (m_attrMap["border"] != "0") {
		D2D1::ColorF rectBorderColor = ResManager::GetInstance().GetColorObject(m_attrMap["bordercolor"])->GetD2D1ColorF();
		hr = pRenderTarget->CreateSolidColorBrush(rectBorderColor, &m_pBorderColorBrush);
		if (FAILED(hr)) {
			ERR("fatal error in CreateSolidColorBrush, nullptr! hr:{}", hr);
			return hr;
		}
	}
	return hr;
}
HRESULT	UIRectangle::DiscardDeviceDependentResources()
{
	SafeRelease(&m_pColorBrush);
	SafeRelease(&m_pBorderColorBrush);
	return S_OK;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
HRESULT	UIRectangle::OnDrawImpl(HDC* pHdc, const RECT& rcInvalid)
{
	HRESULT hr = S_OK;
	Graphics graphics(*pHdc);
	graphics.Clear(Color::White);

	Gdiplus::Color rectColor = ResManager::GetInstance().GetColorObject(m_attrMap["color"])->GetGdiplusColor();
	Gdiplus::SolidBrush linGrBrush(rectColor);
	graphics.FillRectangle(&linGrBrush, 10, 10, 100, 100);

	if (m_attrMap["border"] != "0") {
		Gdiplus::Color rectBorderColor = ResManager::GetInstance().GetColorObject(m_attrMap["bordercolor"])->GetGdiplusColor();
		FLOAT borderWidth = static_cast<FLOAT>(atoi(m_attrMap["border"].c_str()));
		Pen pen(rectBorderColor, borderWidth);
		graphics.DrawRectangle(&pen, 10, 10, 100, 100);
	}
	return hr;
}

#endif