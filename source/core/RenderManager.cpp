#include "stdafx.h"
#include "RenderManager.h"
#include "UIObject.h"

#ifdef USE_D2D_RENDER_MODE
ID2D1Factory*		RenderManager::m_pD2DFactory{ nullptr };
#else
ULONG_PTR			RenderManager::m_gdiplusToken{NULL};
GdiplusStartupInput RenderManager::m_gdiStartupInput{};
#endif

HRESULT RenderManager::Init()
{
	HRESULT hr = S_OK;
#ifdef USE_D2D_RENDER_MODE
	//初始化Direct2D环境
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pD2DFactory);
#else
	//初始化GDI+环境
	Gdiplus::Status status = GdiplusStartup(&m_gdiplusToken, &m_gdiStartupInput, NULL);
	if (status != Gdiplus::Status::Ok) {
		ERR("Initialize GDI+ envirenment failed!");
		hr = S_FALSE;
	}
#endif // USE_D2D_RENDER_MODE
	return hr;
}
HRESULT RenderManager::UnInit()
{
	HRESULT hr = S_OK;
#ifdef USE_D2D_RENDER_MODE
	m_pD2DFactory->Release();
	m_pD2DFactory = nullptr;
#else
	GdiplusShutdown(m_gdiplusToken);
#endif
	return hr;
}
bool CalcIntersection(const RECT& rcInvalid, const UIPos& curObjWndPos)
{
	//IntersectRect();
	return true;
}

#ifdef USE_D2D_RENDER_MODE
HRESULT RenderTarget::Draw(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid, UIObject* pTargetObject/*= nullptr*/)
{
	HRESULT hr = S_OK;
	hr = CreateDeviceDependentResources(pRenderTarget);
	if (FAILED(hr)){
		ERR("Fatal error in RenderTarget::Draw， CreateDeviceDependentResources failed! hr: {}", hr);
		return hr;
	}
	shared_ptr<const string> sAntialias = pTargetObject->GetAttrValue("antialias");
	if (*sAntialias == "0") {
		pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	} else {
		pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}

	UIPos curObjWndPos = pTargetObject->GetWndCoordinatePos();
	auto bIntersection = CalcIntersection(rcInvalid, curObjWndPos);
	auto spStrLimit = pTargetObject->GetAttrValue("limit");
	auto ret = S_OK;

	if (bIntersection) {
		ret = OnDrawImpl(pRenderTarget, rcInvalid);
	}

	if (bIntersection || *spStrLimit == "0") {
		for (auto it = pTargetObject->m_pVecChildrenPair->begin(); it != pTargetObject->m_pVecChildrenPair->end(); ++it) {
			it->second->Draw(pRenderTarget, rcInvalid, it->second);
		}
	}
	return ret;

	//	窗口内在OnPaint中hr = m_pRenderTarget->EndDraw();后，需要判断是否丢弃设备相关资源
	//	DiscardDeviceDependentResources();
}
#endif

#ifndef USE_D2D_RENDER_MODE
HRESULT	RenderTarget::Draw(Graphics& graphics, const RECT& rcInvalid, UIObject* pTargetObject/*= nullptr*/)
{
	auto spStrAntialias = pTargetObject->GetAttrValue("antialias");
	if (*spStrAntialias == "0") {
		graphics.SetSmoothingMode(SmoothingModeNone);
	} else {
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	}
	//--//计算是否相交
	UIPos curObjWndPos = pTargetObject->GetWndCoordinatePos();
	auto bIntersection = CalcIntersection(rcInvalid, curObjWndPos);
	auto spStrLimit	   = pTargetObject->GetAttrValue("limit");
	auto ret = S_OK;
	
	if (bIntersection) {
		ret = OnDrawImpl(graphics, rcInvalid);
	}
	if (bIntersection || *spStrLimit == "0") {
		for (auto it = pTargetObject->m_pVecChildrenPair->begin(); it != pTargetObject->m_pVecChildrenPair->end(); ++it) {
			it->second->Draw(graphics, rcInvalid, it->second);
		}
	}
	return ret;
}
#endif