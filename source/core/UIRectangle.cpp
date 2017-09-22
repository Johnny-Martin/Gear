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
	
	UIPos toWndPos = GetWndCoordinatePos();
	D2D1_RECT_F rectF;
	rectF.left	 = static_cast<FLOAT>(toWndPos.left);
	rectF.top	 = static_cast<FLOAT>(toWndPos.top);
	rectF.right  = static_cast<FLOAT>(toWndPos.width + toWndPos.left);
	rectF.bottom = static_cast<FLOAT>(toWndPos.height + toWndPos.top);

	int cornerRadius = static_cast<int>(atoi(m_attrMap["corner"].c_str()));
	if (cornerRadius == 0) {
		pRenderTarget->FillRectangle(rectF, m_pColorBrush);
	}else{
		D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rectF, (FLOAT)cornerRadius, (FLOAT)cornerRadius);
		pRenderTarget->FillRoundedRectangle(roundedRect, m_pColorBrush);
	}
	
	if (m_attrMap["border"] != "0") {
		FLOAT borderWidth = static_cast<FLOAT>(atoi(m_attrMap["border"].c_str()));
		FLOAT cornerRadius = static_cast<FLOAT>(atoi(m_attrMap["corner"].c_str()));
		if (cornerRadius == 0) {
			pRenderTarget->DrawRectangle(rectF, m_pBorderColorBrush);
		} else {
			D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rectF, (FLOAT)cornerRadius, (FLOAT)cornerRadius);
			pRenderTarget->DrawRoundedRectangle(roundedRect, m_pBorderColorBrush, borderWidth);
		}
		
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
HRESULT	UIRectangle::OnDrawImpl(Graphics& graphics, const RECT& rcInvalid)
{
	auto DrawRoundRectange = [](Gdiplus::Graphics &g, Gdiplus::Pen* pen, int x, int y, int width, int height, int corner){
		//矩形上边  
		g.DrawLine(pen, x + corner, y, x + width - corner, y);

		//矩形下边  
		g.DrawLine(pen, x + corner, y + height, x + width - corner, y + height);

		//矩形左边  
		g.DrawLine(pen, x, y + corner, x, y + height - corner);

		//矩形右边  
		g.DrawLine(pen, x + width, y + corner, x + width, y + height - corner);

		//左上角圆角  
		g.DrawArc(pen, x, y, corner * 2, corner * 2, 180, 90);

		//右下角圆角  
		g.DrawArc(pen, x + width - corner * 2, y + height - corner * 2, corner * 2, corner * 2, 360, 90);

		//右上角圆角  
		g.DrawArc(pen, x + width - corner * 2, y, corner * 2, corner * 2, 270, 90);

		//左下角圆角  
		g.DrawArc(pen, x, y + height - corner * 2, corner * 2, corner * 2, 90, 90);
	};
	auto FillRoundRectangle = [](Graphics &g, Gdiplus::Brush* brush, int x, int y, int width, int height, int corner){
		//矩形填充的步骤：  
		//1、把圆角矩形画分为四个圆角上分成四个同等的扇形外加三个直角矩形  
		//2、先填充三个直角矩形  
		//3、然后填充四个角上的扇形  

		//填充三个直角矩形  
		g.FillRectangle(brush, x, y + corner, width, height - corner * 2);
		g.FillRectangle(brush, x + corner, y, width - corner * 2, height);

		//填充四个角上的扇形区  
		//填充左上角扇形  
		const int PIE_OFFSET = 2;
		g.FillPie(brush, x, y, corner * 2 + PIE_OFFSET, corner * 2 + PIE_OFFSET, 180, 90);

		//填充右下角的扇形  
		g.FillPie(brush, x + width - (corner * 2 + PIE_OFFSET), y + height - (corner * 2 + PIE_OFFSET), corner * 2 + PIE_OFFSET, corner * 2 + PIE_OFFSET, 360, 90);

		//填充右上角的扇形  
		g.FillPie(brush, x + width - (corner * 2 + PIE_OFFSET), y, (corner * 2 + PIE_OFFSET), (corner * 2 + PIE_OFFSET), 270, 90);

		//填充左下角的扇形  
		g.FillPie(brush, x, y + height - (corner * 2 + PIE_OFFSET), (corner * 2 + PIE_OFFSET), (corner * 2 + PIE_OFFSET), 90, 90);
	};

	HRESULT hr = S_OK;
	//graphics.Clear(Color::White);

	Color rectColor = ResManager::GetInstance().GetColorObject(m_attrMap["color"])->GetGdiplusColor();
	SolidBrush linGrBrush(rectColor);
	//将m_pos（相对(父节点)坐标）转换成窗口坐标
	UIPos toWndPos = GetWndCoordinatePos();
	int cornerRadius = static_cast<int>(atoi(m_attrMap["corner"].c_str()));
	if (cornerRadius == 0) {
		graphics.FillRectangle(&linGrBrush, toWndPos.left, toWndPos.top, toWndPos.width, toWndPos.height);
	} else {
		FillRoundRectangle(graphics, &linGrBrush, toWndPos.left, toWndPos.top, toWndPos.width, toWndPos.height, cornerRadius);
	}
	
	if (m_attrMap["border"] != "0") {
		FLOAT borderWidth = static_cast<FLOAT>(atoi(m_attrMap["border"].c_str()));
		Color rectBorderColor = ResManager::GetInstance().GetColorObject(m_attrMap["bordercolor"])->GetGdiplusColor();
		Pen pen(rectBorderColor, borderWidth);

		if (cornerRadius == 0) {
			graphics.DrawRectangle(&pen, toWndPos.left, toWndPos.top, toWndPos.width, toWndPos.height);
		} else {
			DrawRoundRectange(graphics, &pen, toWndPos.left, toWndPos.top, toWndPos.width, toWndPos.height, cornerRadius);
		}
	}
	
	return hr;
}
#endif