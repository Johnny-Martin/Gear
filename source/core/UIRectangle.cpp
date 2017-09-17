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
	ADD_ATTR("corner", "1")
		ADD_ATTR("color", "000000")
		ADD_ATTR("border", "0")
		ADD_ATTR("bordercolor", "")
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
	HRESULT hr = S_OK;

	return hr;
}
HRESULT	UIRectangle::CreateDeviceDependentResources()
{
	HRESULT hr = S_OK;

	return hr;
}
HRESULT	UIRectangle::DiscardDeviceDependentResources()
{
	HRESULT hr = S_OK;

	return hr;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
HRESULT	UIRectangle::OnDrawImpl(HDC* pHdc, const RECT& rcInvalid)
{
	HRESULT hr = S_OK;

	return hr;
}

#endif