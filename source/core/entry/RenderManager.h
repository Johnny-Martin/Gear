/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.17
Description:		‰÷»æ≤ﬂ¬‘π‹¿Ì∆˜
*****************************************************/
#pragma once
#include "../stdafx.h"

class RenderManager
{
public:
	static HRESULT					Init();
	static HRESULT					UnInit();
#ifdef USE_D2D_RENDER_MODE
public:
	static ID2D1Factory*			m_pD2DFactory;
	static IDWriteFactory*			m_pD2DWriteFactory;
	static D2D1_PIXEL_FORMAT		m_gPixelFormat;
	static D2D1_BITMAP_PROPERTIES   m_gBitmapProperties;

#else
private:
	static ULONG_PTR				m_gdiplusToken;
	static GdiplusStartupInput		m_gdiStartupInput;
#endif
};

class UIObject;
class UIPos;
class RenderTarget
{
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT					Draw(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid, UIObject* pTargetObject = nullptr);
	virtual HRESULT					OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos) = 0;
	
protected:
	virtual HRESULT					CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget) = 0;
	virtual HRESULT					DiscardDeviceDependentResources() = 0;
#else
public:
	virtual HRESULT					Draw(Graphics& graphics, const RECT& rcInvalid, UIObject* pTargetObject = nullptr);
	virtual HRESULT					OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos) = 0;
#endif // USE_D2D_RENDER_MODE

};