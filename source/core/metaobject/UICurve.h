/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.26
Description:		界面中的线条
*****************************************************/
#pragma once
#include "../stdafx.h"
#include "../base/UIObject.h"

class UICurve :public UIObject
{
public:
									UICurve();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT					OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos);
protected:
	virtual HRESULT					CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget);
	virtual HRESULT					DiscardDeviceDependentResources();
private:
	ID2D1SolidColorBrush*			m_pColorBrush;
	ID2D1StrokeStyle*				m_pStrokeStyle;
	vector<D2D1_POINT_2F>			m_pointsVer;
#else
public:
	virtual HRESULT					OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos);
private:
	vector<Gdiplus::PointF>			m_pointsVer;
#endif
protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
protected:
	vector<FLOAT>					m_dashesVec;
};