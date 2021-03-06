/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.26
Description:		界面图片资源管理
*****************************************************/
#pragma once
#include "../stdafx.h"
#include "../base/UIObject.h"


class UIText :public UIObject
{
public:
	UIText();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT					OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos);
protected:
	virtual HRESULT					CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget);
	virtual HRESULT					DiscardDeviceDependentResources();
private:
	ID2D1SolidColorBrush*			m_pColorBrush;
#else
	virtual HRESULT					OnDrawImpl(Gdiplus::Graphics& graphics, const UIPos& rcWndPos);
#endif
protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
};