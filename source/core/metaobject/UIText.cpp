#include "stdafx.h"
#include "UIText.h"



UIText::UIText()
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
void UIText::InitAttrMap()
{
	ADD_ATTR("str", "")
}
void UIText::InitEventMap()
{
	//ADD_EVENT("OnCreate", nullptr)
}
void UIText::InitAttrValuePatternMap()
{
}
void UIText::InitAttrValueParserMap()
{
}

///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT	UIText::OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos)
{
	return S_OK;
}
HRESULT	UIText::CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget)
{
	return S_OK;
}
HRESULT	UIText::DiscardDeviceDependentResources()
{
	return S_OK;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
HRESULT UIText::OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos)
{
	return S_OK;
}
#endif