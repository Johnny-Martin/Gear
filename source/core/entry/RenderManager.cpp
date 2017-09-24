#include "stdafx.h"
#include "RenderManager.h"
#include "../base/UIObject.h"

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
	//��ʼ��Direct2D����
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pD2DFactory);
#else
	//��ʼ��GDI+����
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
	RECT curObjWndRECT = curObjWndPos.GetWndRECT();
	RECT retRc{0};
	IntersectRect(&retRc, &rcInvalid, &curObjWndRECT);

	return (retRc.right > retRc.left) && (retRc.bottom > retRc.top);
}

///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT RenderTarget::Draw(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid, UIObject* pTargetObject/*= nullptr*/)
{
	HRESULT hr = S_OK;
	auto visible = pTargetObject->GetAttrValue("visible");
	if (*visible == "0") { 
		return S_OK; 
	}

	UIPos curObjWndPos = pTargetObject->GetWndCoordinatePos();
	auto bIntersection = CalcIntersection(rcInvalid, curObjWndPos);
	auto spStrLimit = pTargetObject->GetAttrValue("limit");
	//���ڵ㲻�ڻ����������ӽڵ㱻�޶��ڸ��׷�Χ�ڣ��򲻽����κλ���
	if (!bIntersection && *spStrLimit == "1") {
		return S_OK;
	}

	hr = CreateDeviceDependentResources(pRenderTarget);
	if (FAILED(hr)){
		ERR("Fatal error in RenderTarget::Draw�� CreateDeviceDependentResources failed! hr: {}", hr);
		return hr;
	}
	auto sAntialias = pTargetObject->GetAttrValue("antialias");
	if (*sAntialias == "0") {
		pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	} else {
		pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}

	if (bIntersection) {
		UIPos toWndPos = pTargetObject->GetWndCoordinatePos();
		D2D1_RECT_F rcWndPos;
		rcWndPos.left = static_cast<FLOAT>(toWndPos.left);
		rcWndPos.top = static_cast<FLOAT>(toWndPos.top);
		rcWndPos.right = static_cast<FLOAT>(toWndPos.width + toWndPos.left);
		rcWndPos.bottom = static_cast<FLOAT>(toWndPos.height + toWndPos.top);
		hr = OnDrawImpl(pRenderTarget, rcWndPos);
	}
	
	ATLASSERT(pTargetObject->m_pVecChildrenPair != nullptr);
	for (auto it = pTargetObject->m_pVecChildrenPair->begin(); it != pTargetObject->m_pVecChildrenPair->end(); ++it) {
		it->second->Draw(pRenderTarget, rcInvalid, it->second);
	}
	
	return hr;

	//	��������OnPaint��hr = m_pRenderTarget->EndDraw();����Ҫ�ж��Ƿ����豸�����Դ
	//	DiscardDeviceDependentResources();
}
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
HRESULT	RenderTarget::Draw(Graphics& graphics, const RECT& rcInvalid, UIObject* pTargetObject/*= nullptr*/)
{
	auto visible = pTargetObject->GetAttrValue("visible");
	if (*visible == "0") {
		return S_OK;
	}

	UIPos curObjWndPos = pTargetObject->GetWndCoordinatePos();
	auto bIntersection = CalcIntersection(rcInvalid, curObjWndPos);
	auto spStrLimit = pTargetObject->GetAttrValue("limit");
	//���ڵ㲻�ڻ����������ӽڵ㱻�޶��ڸ��׷�Χ�ڣ��򲻽����κλ���
	if (!bIntersection && *spStrLimit == "1") {
		return S_OK;
	}

	auto spStrAntialias = pTargetObject->GetAttrValue("antialias");
	if (*spStrAntialias == "0") {
		graphics.SetSmoothingMode(SmoothingModeNone);
	} else {
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	}
	
	HRESULT hr = S_OK;
	if (bIntersection) {
		UIPos rcWndPos = pTargetObject->GetWndCoordinatePos();
		hr = OnDrawImpl(graphics, rcWndPos);
	}

	for (auto it = pTargetObject->m_pVecChildrenPair->begin(); it != pTargetObject->m_pVecChildrenPair->end(); ++it) {
		it->second->Draw(graphics, rcInvalid, it->second);
	}
	return hr;
}
#endif