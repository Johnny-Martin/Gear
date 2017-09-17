#include "stdafx.h"
#include "UIRectangle.h"

UIRectangle::UIRectangle() {
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}

void UIRectangle::InitAttrMap()
{
	ADD_ATTR("corner",		"1")
	ADD_ATTR("color",		"000000")
	ADD_ATTR("border",		"0")
	ADD_ATTR("bordercolor", "000000")
}
void UIRectangle::InitEventMap()
{

}
void UIRectangle::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("corner", R_CHECK_INT);
	ADD_ATTR_PATTERN("border", R_CHECK_INT);
}
void UIRectangle::InitAttrValueParserMap()
{

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

	pRenderTarget->FillRectangle(rectF, m_pColorBrush);

	return S_OK;
}
HRESULT	UIRectangle::CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget)
{
	if (m_pColorBrush) return S_OK;
	
	if (!pRenderTarget) {
		ERR("fatal error in CreateDeviceDependentResources, nullptr! OnDrawImpl will abort");
		return S_FALSE;
	}

	HRESULT hr = pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Gold),
		&m_pColorBrush
	);
	if (FAILED(hr)) {
		ERR("fatal error in CreateSolidColorBrush, nullptr! hr:{}", hr);
		return hr;
	}
	if (m_attrMap["border"] != "0") {
		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::LightGreen),
			&m_pColorBrush
		);
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

	return hr;
}

#endif