#include "stdafx.h"
#include "UIText.h"
#include "../resource/ResColor.h"
#include "../resource/ResManager.h"



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
	ADD_ATTR("color", "#FFFFFF")
	ADD_ATTR("font", "")
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
	DECLEAR_LAMBDA_HANDLE_CHINESE(HandleChinese)
	
	ADD_ATTR_PARSER("str", HandleChinese)
	ADD_ATTR_PARSER("font", HandleChinese)
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
	string str = m_attrMap["str"];
	wstring wstr = StringToWString(str);
	ResColor* pColor = Gear::Res::ResManager::GetInstance().GetColorObject(m_attrMap["color"]);
	Color color = pColor->GetGdiplusColor();
	Gdiplus::SolidBrush brush(color);
	
	ResFont* pResFont = Gear::Res::ResManager::GetInstance().GetFontObject(m_attrMap["font"]);
	Gdiplus::Font* pFont = pResFont->GetGdiplusFont();
	Gdiplus::PointF point((REAL)rcWndPos.left, (REAL)rcWndPos.top);
	
	graphics.DrawString(wstr.c_str(), wstr.length(), pFont, point, &brush);
	return S_OK;
}
#endif