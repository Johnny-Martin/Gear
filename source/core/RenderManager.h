/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.17
Description:		‰÷»æ≤ﬂ¬‘π‹¿Ì∆˜
*****************************************************/
#pragma once
#include "stdafx.h"

class RenderManager
{
public:
	static HRESULT					Init();
	static HRESULT					UnInit();
#ifdef USE_D2D_RENDER_MODE
public:
	static ID2D1Factory*			m_pD2DFactory;
#else
private:
	static ULONG_PTR				m_gdiplusToken;
	static GdiplusStartupInput		m_gdiStartupInput;
#endif
};

class UIObject;
class RenderTarget
{
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT					Draw(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid, UIObject* pTargetObject = nullptr);
	virtual HRESULT					OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid) = 0;
	
protected:
	virtual HRESULT					CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget) = 0;
	virtual HRESULT					DiscardDeviceDependentResources() = 0;
#else
public:
	virtual HRESULT					Draw(Graphics& graphics, const RECT& rcInvalid, UIObject* pTargetObject = nullptr);
	virtual HRESULT					OnDrawImpl(Graphics& graphics, const RECT& rcInvalid) = 0;
#endif // USE_D2D_RENDER_MODE

};