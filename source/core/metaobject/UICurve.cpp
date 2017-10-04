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
	ADD_ATTR("color",	"#FFFFFF")
	ADD_ATTR("hline",	"0")
	ADD_ATTR("vline", "0")
	ADD_ATTR("linewidth", "1")
	ADD_ATTR("style", "solid")
	ADD_ATTR("cap", "flat")
	ADD_ATTR("dashes", "")//只有在style == custom的时候才会用到
}
void UICurve::InitEventMap()
{
	
}
void UICurve::InitAttrValuePatternMap()
{
	//ADD_ATTR_PATTERN("color",	R_CHECK_COLOR_VALUE)
	ADD_ATTR_PATTERN("hline",	R_CHECK_BOOL)
	ADD_ATTR_PATTERN("vline", R_CHECK_BOOL)
	ADD_ATTR_PATTERN("linewidth", R_CHECK_INT)
}
void UICurve::InitAttrValueParserMap()
{
	auto ParseDashes = [&](const string& sAttrName)->bool {
		string dashes = m_attrMap[sAttrName];
		if (dashes.empty()){
			return false;
		}
		EraseSpace(dashes);
		string item = "";
		for (std::size_t i=0; i<dashes.length(); ++i)
		{
			char num = dashes[i];
			if (num >= '0' && num <= '9')
			{
				item += num;
			} else if(num == ','){
				float dash = (float)atoi(item.c_str());
				item = "";
				m_dashesVec.push_back(dash);
				continue;
			} else {
				ERR("ParseDashes error: illegal character");
				return false;
			}
		}
		if (!item.empty())
		{
			float dash = (float)atoi(item.c_str());
			m_dashesVec.push_back(dash);
		}
		return true;
	};

	ADD_ATTR_PARSER("dashes", ParseDashes)
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
	} else if (m_pointsVer.size() > 0) {//
		std::size_t length = m_pointsVer.size();
		
		for (std::size_t i = 0; i < length - 1; ++i) {
			pA = m_pointsVer[i];
			pB = m_pointsVer[i + 1];
			pRenderTarget->DrawLine(pA, pB, m_pColorBrush, (FLOAT)lineWidth, m_pStrokeStyle);
		}
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

	//默认是solid(实线)风格
	D2D1_STROKE_STYLE_PROPERTIES prop = StrokeStyleProperties();
	auto style = m_attrMap["style"];
	auto cap = m_attrMap["cap"];
	int length = 0;
	FLOAT* floatVec = nullptr;
	if (style == "solid") {
		prop.dashStyle = D2D1_DASH_STYLE_SOLID;
	}else if (style == "dash") {
		prop.dashStyle = D2D1_DASH_STYLE_DASH;
	} else if (style == "dot") {
		prop.dashStyle = D2D1_DASH_STYLE_DOT;
	} else if (style == "dashdot") {
		prop.dashStyle = D2D1_DASH_STYLE_DASH_DOT;
	} else if (style == "dashdotdot") {
		prop.dashStyle = D2D1_DASH_STYLE_DASH_DOT_DOT;
	} else if (style == "custom" && m_dashesVec.size() > 0) {
		prop.dashStyle = D2D1_DASH_STYLE_CUSTOM;
		length = m_dashesVec.size();
		floatVec = new FLOAT[length];
		for (auto i=0; i<length; ++i)
		{
			floatVec[i] = m_dashesVec[i];
		}
	}

	if (cap == "flat") {
		prop.dashCap = D2D1_CAP_STYLE_FLAT;
	} else if (cap == "square") {
		prop.dashCap = D2D1_CAP_STYLE_SQUARE;
	} else if (cap == "round") {
		prop.dashCap = D2D1_CAP_STYLE_ROUND;
	} else if (cap == "triangle") {
		prop.dashCap = D2D1_CAP_STYLE_TRIANGLE;
	}

	hr = RenderManager::m_pD2DFactory->CreateStrokeStyle(prop, floatVec, length,&m_pStrokeStyle);
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
	auto SetStyle = [&](Pen& pen) {
		auto style = m_attrMap["style"];
		if (style == "solid") {
			pen.SetDashStyle(DashStyleSolid);
		} else if (style == "dash") {
			pen.SetDashStyle(DashStyleDash);
		} else if (style == "dot") {
			pen.SetDashStyle(DashStyleDot);
		} else if (style == "dashdot") {
			pen.SetDashStyle(DashStyleDashDot);
		} else if (style == "dashdotdot") {
			pen.SetDashStyle(DashStyleDashDotDot);
		} else if (style == "custom") {
			pen.SetDashStyle(DashStyleCustom);

			int length = (int)m_dashesVec.size();
			REAL* floatVec = new REAL[length];
			for (auto i = 0; i < length; ++i)
			{
				floatVec[i] = m_dashesVec[i];
			}
			pen.SetDashPattern(floatVec, length);
		}
	};
	
	auto SetCap = [&](Pen& pen) {
		auto cap = m_attrMap["cap"];
		if (cap == "flat") {
			pen.SetDashCap(Gdiplus::DashCapFlat);
		} else if (cap == "round") {
			pen.SetDashCap(Gdiplus::DashCapRound);
		} else if (cap == "triangle") {
			pen.SetDashCap(Gdiplus::DashCapTriangle);
		}
	};

	ResColor* resColorObj = ResManager::GetInstance().GetColorObject(m_attrMap["color"]);
	Color color = resColorObj->GetGdiplusColor();
	int lineWidth = atoi(m_attrMap["linewidth"].c_str());
	Gdiplus::Pen pen(color, (REAL)lineWidth);

	SetStyle(pen);
	SetCap(pen);

	Status status = GenericError;
	if (m_attrMap["hline"] == "1"){
		Gdiplus::Point pA(rcWndPos.left, rcWndPos.top);
		Gdiplus::Point pB(rcWndPos.left + rcWndPos.width, rcWndPos.top);
		status = graphics.DrawLine(&pen, pA, pB);
	} else if(m_attrMap["vline"] == "1") {
		Gdiplus::Point pA(rcWndPos.left, rcWndPos.top);
		Gdiplus::Point pB(rcWndPos.left, rcWndPos.top + rcWndPos.height);
		status = graphics.DrawLine(&pen, pA, pB);
	} else if (m_pointsVer.size() > 0) {//
		std::size_t length = m_pointsVer.size();
		Gdiplus::PointF* points = new Gdiplus::PointF[length];
		for (std::size_t i=0; i<length; ++i){
			points[i] = m_pointsVer[i];
		}
		status = graphics.DrawLines(&pen, points, length);
	}
	return status == Ok ? S_OK : S_FALSE;
}
#endif