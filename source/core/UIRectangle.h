/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.17
Description:		界面中的矩形图元
*****************************************************/
#pragma once
#include "stdafx.h"
#include "UIObject.h"
#include "RenderManager.h"

class UIRectangle :public UIObject, public RenderTarget
{
public:
	UIRectangle();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT					OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid);
protected:
	virtual HRESULT					CreateDeviceDependentResources();
	virtual HRESULT					DiscardDeviceDependentResources();
#else
public:
	virtual HRESULT					OnDrawImpl(HDC* pHdc, const RECT& rcInvalid);
#endif
protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
};
