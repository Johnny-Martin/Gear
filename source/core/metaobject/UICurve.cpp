#include "stdafx.h"
#include "UICurve.h"
#include "../resource/ResManager.h"

using namespace Gear::Res;

UICurve::UICurve()
#ifdef USE_D2D_RENDER_MODE
	:m_pColorBrush(nullptr)
	, m_pStrokeStyle(nullptr)
#else

#endif
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
void UICurve::InitAttrMap()
{
	ADD_ATTR("color",	"FFFFFF")
	ADD_ATTR("hline",	"0")
	ADD_ATTR("vline", "0")
	ADD_ATTR("linewidth", "1")
	ADD_ATTR("style", "0")
}
void UICurve::InitEventMap()
{
	
}
void UICurve::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("color",	R_CHECK_COLOR_VALUE)
	ADD_ATTR_PATTERN("hline",	R_CHECK_BOOL)
	ADD_ATTR_PATTERN("vline", R_CHECK_BOOL)
	ADD_ATTR_PATTERN("linewidth", R_CHECK_INT)
	ADD_ATTR_PATTERN("style", R_CHECK_INT)
}
void UICurve::InitAttrValueParserMap()
{

}

///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT	UICurve::OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos)
{
	HRESULT hr = S_OK;
	D2D1_POINT_2F pA, pB;
	int lineWidth = atoi(m_attrMap["linewidth"].c_str());
	
	if (m_attrMap["hline"] == "1") {
		pA.x = rcWndPos.left;
		pA.y = rcWndPos.top;
		pB.x = rcWndPos.right;
		pB.y = rcWndPos.top;
		pRenderTarget->DrawLine(pA, pB, m_pColorBrush, (FLOAT)lineWidth, m_pStrokeStyle);
	} else if (m_attrMap["vline"] == "1") {
		pA.x = rcWndPos.left;
		pA.y = rcWndPos.top;
		pB.x = rcWndPos.left;
		pB.y = rcWndPos.bottom;
		pRenderTarget->DrawLine(pA, pB, m_pColorBrush, (FLOAT)lineWidth, m_pStrokeStyle);
	} else {//if(m_gdiPointsVer.size() > 0)
		
	}
	return hr;
}
HRESULT	UICurve::CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget)
{
	if (m_pColorBrush) return S_OK;

	if (!pRenderTarget) {
		ERR("fatal error in CreateDeviceDependentResources, nullptr! OnDrawImpl will abort");
		return S_FALSE;
	}

	D2D1::ColorF curveColor = ResManager::GetInstance().GetColorObject(m_attrMap["color"])->GetD2D1ColorF();
	HRESULT hr = pRenderTarget->CreateSolidColorBrush(curveColor, &m_pColorBrush);
	if (FAILED(hr)) {
		ERR("fatal error in CreateSolidColorBrush, nullptr! hr:{}", hr);
		return hr;
	}

	D2D1_STROKE_STYLE_PROPERTIES prop = StrokeStyleProperties();
	//prop.dashStyle = D2D1_DASH_STYLE_DOT;
	hr = RenderManager::m_pD2DFactory->CreateStrokeStyle(prop,0,0,&m_pStrokeStyle);
	if (FAILED(hr)) {
		ERR("fatal error in m_pStrokeStyle, nullptr! hr:{}", hr);
		return hr;
	}
	return hr;
}
HRESULT	UICurve::DiscardDeviceDependentResources()
{
	HRESULT hr = S_OK;
	SafeRelease(&m_pColorBrush);
	SafeRelease(&m_pStrokeStyle);
	return hr;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
HRESULT	UICurve::OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos)
{
	ResColor* resColorObj = ResManager::GetInstance().GetColorObject(m_attrMap["color"]);
	Color color = resColorObj->GetGdiplusColor();
	int lineWidth = atoi(m_attrMap["linewidth"].c_str());
	Pen pen(color, (REAL)lineWidth);

	Status status = GenericError;
	if (m_attrMap["hline"] == "1"){
		Gdiplus::Point pA(rcWndPos.left, rcWndPos.top);
		Gdiplus::Point pB(rcWndPos.left + rcWndPos.width, rcWndPos.top);
		status = graphics.DrawLine(&pen, pA, pB);
	} else if(m_attrMap["vline"] == "1") {
		Gdiplus::Point pA(rcWndPos.left, rcWndPos.top);
		Gdiplus::Point pB(rcWndPos.left, rcWndPos.top + rcWndPos.height);
		status = graphics.DrawLine(&pen, pA, pB);
	} else  {//if(m_gdiPointsVer.size() > 0)
		m_gdiPointsVer.push_back(PointF(0, 0));
		m_gdiPointsVer.push_back(PointF(15, 5));
		m_gdiPointsVer.push_back(PointF(70, 9));
		m_gdiPointsVer.push_back(PointF(11, 50));
		std::size_t length = m_gdiPointsVer.size();
		Gdiplus::PointF* points = new Gdiplus::PointF[length];
		for (std::size_t i=0; i<length; ++i){
			points[i] = m_gdiPointsVer[i];
		}
		status = graphics.DrawLines(&pen, points, length);
	}
	return status == Ok ? S_OK : S_FALSE;
}
#endif