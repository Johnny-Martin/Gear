#include "stdafx.h"
#include "UIRectangle.h"
#include "../resource/ResManager.h"

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

///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT	UIRectangle::OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos, const RECT* rcInvalidPtr/* = nullptr*/)
{
	int cornerRadius = static_cast<int>(atoi(m_attrMap["corner"].c_str()));
	if (cornerRadius == 0) {
		pRenderTarget->FillRectangle(rcWndPos, m_pColorBrush);
	}else{
		D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rcWndPos, (FLOAT)cornerRadius, (FLOAT)cornerRadius);
		pRenderTarget->FillRoundedRectangle(roundedRect, m_pColorBrush);
	}
	
	if (m_attrMap["border"] != "0") {
		FLOAT borderWidth = static_cast<FLOAT>(atoi(m_attrMap["border"].c_str()));
		FLOAT cornerRadius = static_cast<FLOAT>(atoi(m_attrMap["corner"].c_str()));
		if (cornerRadius == 0) {
			pRenderTarget->DrawRectangle(rcWndPos, m_pBorderColorBrush);
		} else {
			D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(rcWndPos, (FLOAT)cornerRadius, (FLOAT)cornerRadius);
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
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
HRESULT	UIRectangle::OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos)
{
	auto DrawRoundRectange = [](Gdiplus::Graphics &g, Gdiplus::Pen* pen, int x, int y, int width, int height, int corner){
		//�����ϱ�  
		g.DrawLine(pen, x + corner, y, x + width - corner, y);

		//�����±�  
		g.DrawLine(pen, x + corner, y + height, x + width - corner, y + height);

		//�������  
		g.DrawLine(pen, x, y + corner, x, y + height - corner);

		//�����ұ�  
		g.DrawLine(pen, x + width, y + corner, x + width, y + height - corner);

		//���Ͻ�Բ��  
		g.DrawArc(pen, x, y, corner * 2, corner * 2, 180, 90);

		//���½�Բ��  
		g.DrawArc(pen, x + width - corner * 2, y + height - corner * 2, corner * 2, corner * 2, 360, 90);

		//���Ͻ�Բ��  
		g.DrawArc(pen, x + width - corner * 2, y, corner * 2, corner * 2, 270, 90);

		//���½�Բ��  
		g.DrawArc(pen, x, y + height - corner * 2, corner * 2, corner * 2, 90, 90);
	};
	auto FillRoundRectangle = [](Graphics &g, Gdiplus::Brush* brush, int x, int y, int width, int height, int corner){
		//�������Ĳ��裺  
		//1����Բ�Ǿ��λ���Ϊ�ĸ�Բ���Ϸֳ��ĸ�ͬ�ȵ������������ֱ�Ǿ���  
		//2�����������ֱ�Ǿ���  
		//3��Ȼ������ĸ����ϵ�����  

		//�������ֱ�Ǿ���  
		g.FillRectangle(brush, x, y + corner, width, height - corner * 2);
		g.FillRectangle(brush, x + corner, y, width - corner * 2, height);

		//����ĸ����ϵ�������  
		//������Ͻ�����  
		const int PIE_OFFSET = 2;
		g.FillPie(brush, x, y, corner * 2 + PIE_OFFSET, corner * 2 + PIE_OFFSET, 180, 90);

		//������½ǵ�����  
		g.FillPie(brush, x + width - (corner * 2 + PIE_OFFSET), y + height - (corner * 2 + PIE_OFFSET), corner * 2 + PIE_OFFSET, corner * 2 + PIE_OFFSET, 360, 90);

		//������Ͻǵ�����  
		g.FillPie(brush, x + width - (corner * 2 + PIE_OFFSET), y, (corner * 2 + PIE_OFFSET), (corner * 2 + PIE_OFFSET), 270, 90);

		//������½ǵ�����  
		g.FillPie(brush, x, y + height - (corner * 2 + PIE_OFFSET), (corner * 2 + PIE_OFFSET), (corner * 2 + PIE_OFFSET), 90, 90);
	};

	HRESULT hr = S_OK;
	//graphics.Clear(Color::White);

	Color rectColor = ResManager::GetInstance().GetColorObject(m_attrMap["color"])->GetGdiplusColor();
	SolidBrush linGrBrush(rectColor);
	
	int cornerRadius = static_cast<int>(atoi(m_attrMap["corner"].c_str()));
	if (cornerRadius == 0) {
		graphics.FillRectangle(&linGrBrush, rcWndPos.left, rcWndPos.top, rcWndPos.width, rcWndPos.height);
	} else {
		FillRoundRectangle(graphics, &linGrBrush, rcWndPos.left, rcWndPos.top, rcWndPos.width, rcWndPos.height, cornerRadius);
	}
	
	if (m_attrMap["border"] != "0") {
		FLOAT borderWidth = static_cast<FLOAT>(atoi(m_attrMap["border"].c_str()));
		Color rectBorderColor = ResManager::GetInstance().GetColorObject(m_attrMap["bordercolor"])->GetGdiplusColor();
		Pen pen(rectBorderColor, borderWidth);

		if (cornerRadius == 0) {
			graphics.DrawRectangle(&pen, rcWndPos.left, rcWndPos.top, rcWndPos.width, rcWndPos.height);
		} else {
			DrawRoundRectange(graphics, &pen, rcWndPos.left, rcWndPos.top, rcWndPos.width, rcWndPos.height, cornerRadius);
		}
	}
	
	return hr;
}
#endif