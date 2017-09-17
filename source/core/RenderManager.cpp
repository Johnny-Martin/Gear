#include "stdafx.h"
#include "UIRender.h"

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

#ifdef USE_D2D_RENDER_MODE
HRESULT RenderTarget::Draw(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid)
{
	HRESULT hr = S_OK;
	hr = CreateDeviceDependentResources();
	if (FAILED(hr)){
		ERR("Fatal error in RenderTarget::Draw， CreateDeviceDependentResources failed! hr: {}", hr);
		return hr;
	}
	hr = OnDrawImpl(pRenderTarget, rcInvalid);
	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceDependentResources();
	}
	return hr;
}
#endif

#ifndef USE_D2D_RENDER_MODE
HRESULT	RenderTarget::Draw(HDC* pHdc, const RECT& rcInvalid)
{
	return OnDrawImpl(pHdc, rcInvalid);
}
#endif