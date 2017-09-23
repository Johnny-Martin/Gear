/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.17
Description:		界面中的矩形图元
*****************************************************/
#pragma once
#include "../stdafx.h"
#include "../base/UIObject.h"
#include "../entry/RenderManager.h"

class UIRectangle :public UIObject
{
public:
	UIRectangle();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT					OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid);
protected:
	virtual HRESULT					CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget);
	virtual HRESULT					DiscardDeviceDependentResources();
private:
	ID2D1SolidColorBrush*			m_pColorBrush;
	ID2D1SolidColorBrush*			m_pBorderColorBrush;
#else
public:
	virtual HRESULT					OnDrawImpl(Graphics& graphics, const RECT& rcInvalid);
#endif
protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
};
